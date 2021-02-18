/*
 * Copyright IBM Corporation 1987,1988,1989
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
/***********************************************************
		Copyright IBM Corporation 1988

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
#ifndef __HAVE_IO_ARCHITECTURE__
#define __HAVE_IO_ARCHITECTURE__
/* $Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/common/RCS/ibmIOArch.h,v 9.1 88/10/31 23:09:54 paul Exp $ */
/* $Source: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/common/RCS/ibmIOArch.h,v $ */

/*
 * --- FOR 6152
 * References to all pc ( i.e. '286 ) memory in the
 * regions used by the X server ( the 128K windows )
 * MUST be long-word ( i.e. 32-bit ) reads or writes.
 * --- FOR RTPC
 * Natural Alignment For Memory-Mapped I/O
 * --- FOR PS2
 * Natural Alignment For Memory-Mapped I/O
 *
 * --- NOTE:
 * These definitions will change for other memory
 * architectures or operating systems ( e.g. AIX-Rt )
 */

#if defined(lint) || defined(PCIO) || defined(i386) || defined(iAPX286)

extern unsigned char inb( ) ;
extern unsigned char outb( ) ;
extern unsigned short int inw( ) ;
extern unsigned short outw( ) ;
extern void INTS_ON() ;
extern void INTS_OFF() ;

#else

#define INTS_ON() /**/
#define INTS_OFF() /**/

#ifdef ATRIO

#define inb( pc_io_address ) ( ( *( (volatile unsigned long int *) \
	( 0xD00F0000 | ( ( pc_io_address ) ) ) ) ) )
#define inw( pc_io_address ) ( ( *( (volatile unsigned long int *) \
	( 0xD00E0000 | ( ( pc_io_address ) ) ) ) ) )
#define outb( pc_io_address, value ) ( ( *( (volatile unsigned long int *) \
	( 0xD00F0000 | ( pc_io_address ) ) ) = ( value ) ) )
#define outw( pc_io_address, value ) ( ( *( (volatile unsigned long int *) \
	( 0xD00E0000 | ( ( pc_io_address ) ) ) ) = ( value ) ) )

#else
#ifdef RTIO
/* For Normal RT-PC */

#define inb( pc_io_address ) ( ( *( (volatile unsigned char *) \
	( 0xF0000000 | ( ( pc_io_address ) ) ) ) ) )
#define inw( pc_io_address ) ( ( *( (volatile unsigned short *) \
	( 0xF0000000 | ( ( pc_io_address ) ) ) ) ) )
#define outb( pc_io_address, value ) ( ( *( (volatile unsigned char *) \
	( 0xF0000000 | ( pc_io_address ) ) ) = ( value ) ) )
#define outw( pc_io_address, value ) (( *( (volatile unsigned short *) \
 	( 0xF0000000 | ( ( pc_io_address ) ) ) ) = ( value ) ) )
#endif
#endif
#endif

/* So that lint doesn't complain about constructs it doesn't understand */
#ifdef lint
#ifdef volatile
#undef volatile
#endif
#define volatile /**/
#ifdef const
#undef const
#endif
#define const    /**/
#ifdef signed
#undef signed
#endif
#define signed   /**/
#endif

#endif /* __HAVE_IO_ARCHITECTURE__ */
