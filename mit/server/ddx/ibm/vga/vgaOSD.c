/***********************************************************
		Copyright IBM Corporation 1987,1988

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/vga/RCS/vgaOSD.c,v 6.7 89/08/13 22:25:39 jeff Exp $ */
/* $Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/vga/RCS/vgaOSD.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/vga/RCS/vgaOSD.c,v 6.7 89/08/13 22:25:39 jeff Exp $";
#endif

#ifdef BSDrt
#include <sys/file.h>
#include <machinecons/buf_emul.h>
#else
#if defined(AIX386)
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#else
	******** ERROR ********
#endif
#endif

#include "X.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursorstr.h"

#include "OScompiler.h"

#include "ibmScreen.h"

#include "vgaVideo.h"
#include "vgaSave.h"

#if defined(BSDrt)
#if !defined(IBM_OS_HAS_X_QUEUE)
#define IBM_OS_HAS_X_QUEUE 1
#endif
#else
#if defined(AIX386)
#include "ibmXhftMap.h"
#endif
#endif

#if defined(IBM_OS_HAS_X_QUEUE)
#include <sys/ioctl.h>
#endif

#if defined(IBM_OS_HAS_X_QUEUE) || defined(BSDrt)
#define USE_SLASH_DEV_FILES 1
#else
#define USE_SLASH_DEV_FILES 0
#endif

/* Global Variables */
int vgaDisplayTubeType = 0 ;

int vgaCheckDisplay( fd )
register int fd ; /* Device File Decriptor */
{
#if defined(BSDrt)
static unsigned long int infoword ;

return ( ioctl( fd, BUFDISPINFO, &infoword ) < 0 ) ? -1 : ( infoword & 3 ) ;
#else

return COLOR_TUBE ; /* THIS IS CERTAINLY WRONG !! */

#endif
}

static int vga_file_desc = -1 ; /* Set IFF open on VGA_DEV_FILE succeedes */

#if defined(IBM_OS_HAS_HFT)
extern int ibm8514FD;
#endif

int
vgaProbe()
{
register int fd ;

#if USE_SLASH_DEV_FILES
    if ( ( fd = open( VGA_DEV_FILE, O_RDWR | O_NDELAY ) ) >= 0 ) {
#if defined(i386) && defined(IBM_OS_HAS_X_QUEUE)
	/* Set the display to BIOS mode 0x12 -- AIX386 mode 0x1C */
	if ( ioctl( fd, EGAMODE, 0x1C ) < 0 ) {
	    ErrorF( "vgaProbe: vgaioctl, set ega mode\n" ) ;
	    (void) close( fd ) ;
	    return -1 ;
	}
	/* Map the display buffer into user space */
	if ( SCREEN_ADDR != ioctl( fd, MAPCONS, 0 ) ) {
	    ErrorF(
		"vgaProbe: vgaioctl, vga map video memory address wrong\n" ) ;
	    (void) close( fd ) ;
	    return -1 ;
	}
#endif /* defined(i386) && defined(IBM_OS_HAS_X_QUEUE) */
	vga_file_desc = fd ;
    }
    else /* Open of device file failed */
#endif /* USE_SLASH_DEV_FILES */
#if defined(IBM_OS_HAS_HFT)
    {
	if ( ibm8514FD != -1 )
	    fd = ibm8514FD;
	else if ( ( fd = hftProbe( VGAxHFTid ) ) < 0 )
	    return fd ;
    }
#else /* No hft and initial devFile open failed */
    {
	ErrorF( "vgaProbe: Failed to open %s\n", VGA_DEV_FILE ) ;
	return -1 ;
    }
#endif /* defined(IBM_OS_HAS_HFT) */

    if ( ( vgaDisplayTubeType = vgaCheckDisplay( fd ) ) <= 0 ) {
	(void) close( fd ) ;
	return -1 ;
    }
    ibmInfoMsg( ( vgaDisplayTubeType == COLOR_TUBE )
      ? "Found a vga with color display.\n" : 
	"Found a vga with gray scale display.\n" ) ;

    return fd ;
}

#if !defined(DOS) && !defined(AIX)
static void
unix_vga_init()
{
register int fd ;
#if defined(BSDrt) && defined(ATRIO)
static const int mode = (int) MODE_124 ;
static unsigned page = SCREEN_ADDR ;

extern void exit() ;
#endif

extern int ioctl() ;

/* Open Device File */
if ( ( fd = vga_file_desc ) < 0 ) {
	ErrorF( "unix_vga_init: vga file descriptor invalid, exiting...\n" ) ;
	exit( 1 ) ;
}

#if defined(BSDrt) && defined(ATRIO)

/* Set 128k window to point to vga display buffer */
if ( ioctl( fd, BUFSETWIND, &page ) < 0 )
	ErrorF( "unix_vga_init: vgaioctl, set window\n" ) ;
/* Set The Display Mode To Avoid Confusing The BIOS Later */
if ( ioctl( fd, BUFINITVGA, &mode ) < 0 )
	ErrorF( "unix_vga_init: vgaioctl, vga init\n" ) ;
#endif

return ;
}

#endif

extern void save_vga_state() ;
extern void restore_vga_state() ;
extern void save_dac() ;
extern void restore_dac() ;
extern void set_graphics_mode() ;

/* Video State On Program Entry */
static struct vga_video_hardware_state VS_Start ;
static DAC_TABLE init_dac_table ;

int
vgaScreenInitHW( index )
register int index ;
{
static char been_here = 0 ;

if ( !been_here ) {
#if !defined(DOS) && !defined(AIX)
	unix_vga_init() ;
#endif
	/* Save Extant Video State & Setup For Graphics */
	save_vga_state( &VS_Start ) ;
	save_dac( init_dac_table ) ;
	been_here = 1 ;
}
/* If The Display Is Turned Off Or Changed It Should Take Effect Now */
if ( ( vgaDisplayTubeType = vgaCheckDisplay( vga_file_desc ) ) > 0 ) {
	if ( !( vgaDisabled & PhysicalVgaAccessDisabled ) )
		set_graphics_mode( &VS_Start ) ;
	else
		vgaDisabled = ( vgaDisabled & ~ VirtualVgaHardwareClosePending )
			    | VirtualVgaHardwareOpenPending ;
	return 1 ; /* SUCCESS !! */
}
else {
	ErrorF( "Headless vga!\n Check monitor cables!\n" ) ;
	return 0 ; /* Error Condition -- FAILURE !! */
}
/*NOTREACHED*/
}

/*ARGSUSED*/
void
vgaCloseHW( index )
register int index ;
{
if ( !( vgaDisabled & PhysicalVgaAccessDisabled ) ) {
#if defined(i386) && !defined(DOS)
	/* Set the display to BIOS mode 0x7 -- AIX386 mode 0x3 */
	if ( ( vga_file_desc >= 0 )
	  && ( ioctl( vga_file_desc, EGAMODE, 0x3 ) < 0 ) )
		ErrorF( "vgaCloseHW: vgaioctl, set ega mode\n" ) ;
#endif
	restore_dac( init_dac_table ) ;
	restore_vga_state( &VS_Start ) ;
}
else
	vgaDisabled = ( vgaDisabled & ~ VirtualVgaHardwareOpenPending )
		    | VirtualVgaHardwareClosePending ;

return ;
}
