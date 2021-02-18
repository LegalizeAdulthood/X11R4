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
/* $Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/common/RCS/ibmCmdArgs.c,v 30.2 89/06/14 18:33:00 jeff Exp $ */
/* $Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/common/RCS/ibmCmdArgs.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/common/RCS/ibmCmdArgs.c,v 30.2 89/06/14 18:33:00 jeff Exp $" ;
#endif

#include "X.h"
#include "cursorstr.h"
#include "miscstruct.h"
#include "scrnintstr.h"

#include "ibmCmdArgs.h"
#include "ibmScreen.h"

#include "OSio.h"

#include "ibmTrace.h"

/* Command-Line Globals */
int ibmDontZap = FALSE ;

typedef struct argumentDescriptor {
	char *argString ;
	char *usageString ;
	int (*processFunc)() ;
} ArgumentStr ;

#ifndef DISABLE_ADDITIONAL_SCREENS
ibmPerScreenInfo *(*ibmAdditionalScreenArg)();
#endif

/***====================================================================***/

#if !defined(IBM_MUST_USE_HDWR) && defined(ibm032)
static int
doUseHardwareArg( myArg )
char **myArg ;
{
ibmUseHardware = FALSE ;
return 1 ;
}
#endif

#ifdef IBM_SPECIAL_MALLOC
static int
doMallocArg( myArg )
char **myArg ;
{
register int lvl = atoi( myArg[1] ) ;

SetMallocCheckLevel( lvl ) ;
ibmInfoMsg( "allocator check level set to %d...\n", lvl ) ;
return 2 ; /* Swallow An Extra Argument */
}

static int
doPlumberArg( myArg )
char **myArg ;
{
ibmSetupPlumber( myArg[1] ) ;
return 2 ; /* Swallow An Extra Argument */
}
#endif

static int
doOpenAllScreens( myArg )
char **myArg ;
{
ibmOpenAllScreens = TRUE ;
return 1 ;
}

#if defined( IBM_BS_FLAGS )
static int
processDashBS( myArg )
char **myArg ;
{
ibmAllowBackingStore = 1 ;
return 1 ;
}

static int
processDashNoBS( myArg )
char **myArg ;
{
ibmAllowBackingStore = 0 ;
return 1 ;
}
#endif /* IBM_BS_FLAGS */

#if defined(AIXrt) || defined(AIX386)

static int
dontZap( myArg )
char **myArg ;
{
ibmDontZap = TRUE ;
return 1 ;
}

static int
processKeyClick( myArg )
char **myArg ;
{
if ( ( myArg[ 1 ][ 0 ] >= '0') && ( myArg[ 1 ][ 0 ] <= '9' ) )
	*myArg = "c" ;
return 0 ;
}
#endif

#if defined(AIXrt) || defined(AIX386)
static int
processRGBpath( myArg )
char **myArg ;
{
extern char *rgbPath ;
return ( rgbPath = myArg[1] ) ? 2 : 0 ;
}
#endif

#if defined(IBM_GSL)
static int
processGSLscreen( myArg )
char **myArg ;
{
extern char *gslCommandLineScreenOption ;
return ( gslCommandLineScreenOption = myArg[1] ) ? 2 : 0 ;
}
#endif

#if defined IBM_OS_HAS_HFT
/* HFT GLOBALS */
extern int AIXMouseChordDelay ;
extern int ibmRefreshOnActivate ;
extern char *display ;

static int
processHFTmouseDelay( myArg )
char **myArg ;
{
if ( myArg[1] && myArg[1][0] >= '0' && myArg[1][0] <= '9' ) {
	AIXMouseChordDelay = atoi( myArg[1] ) ;
	return 2 ; /* Swallow An Extra Argument */
}
else
	return 0 ;
}

static int
processAIXdisplaySpec( myArg )
char **myArg ;
{
char *dpy= myArg[1] ;

if ( !dpy ) {
	ErrorF( "must specify display number after -n\n" ) ;
	UseMsg() ;
	exit( 1 ) ;
}
if ( dpy[0] == ':' )
	display = &dpy[1] ;
else {
	ErrorF( "display specification must begin with ':'\n" ) ;
	UseMsg() ;
	exit( 1 ) ;
}
return 2 ; /* Swallow An Extra Argument */
}

static int
processRefreshArg( myArg )
char **myArg ;
{
ibmRefreshOnActivate = 1 ;
return 1 ;
}

static int
processRepaintArg( myArg )
char **myArg ;
{
ibmRefreshOnActivate = 0 ;
return 1 ;
}

#endif

static int
dolockKey( myArg )
char **myArg ;
{
ibmLockEnabled = 1 ;
return 1 ;
}

static int
doNolockKey( myArg )
char **myArg ;
{
ibmLockEnabled = 0 ;
return 1 ;
}

static int
doPCkeys( myArg )
char **myArg ;
{
ibmUsePCKeys = 1 ;
return 1 ;
}

static int
doRTkeys( myArg )
char **myArg ;
{
ibmUsePCKeys = 0 ;
return 1 ;
}

#ifdef TRACE_X
static int
doTrace( myArg )
char **myArg ;
{
ibmTrace = TRUE ;
return 1 ;
}
#endif

static int
setQuiet( myArg )
char **myArg ;
{
ibmQuietFlag = 1 ;
return 1 ;
}

static int
setVerbose( myArg )
char **myArg ;
{
ibmQuietFlag = 0 ;
return 1 ;
}

static int
setWrapX( myArg )
char **myArg ;
{
ibmXWrapScreen = TRUE ;
return 1 ;
}

static int
setWrapY( myArg )
char **myArg ;
{
ibmYWrapScreen = TRUE ;
return 1 ;
}

static int
setWrapXY( myArg )
char **myArg ;
{
ibmXWrapScreen = ibmYWrapScreen = TRUE ;
return 1 ;
}

static int
getWhitePixelText( myArg )
char **myArg ;
{
return ( ibmWhitePixelText = myArg[1] ) ? 2 : 0 ;
}

static int
getBlackPixelText( myArg )
char **myArg ;
{
return ( ibmBlackPixelText = myArg[1] ) ? 2 : 0 ;
}

#if defined(AIXrt) || defined(AIX386)
#define REALLY_DIX_ARG( string ) string
#else /* BSD */
#define REALLY_DIX_ARG( string ) ( (char *) 0 )
#endif

ArgumentStr osCmdArgTable[] = {
#if defined(AIXrt)
	{ "-m",
	  "   -m              use monochrome display characteristics\n",
	 (int (*)()) 0 },
#endif
	{ "-bp",
	  "   -bp <color>     specify a BlackPixel color\n",
	  getBlackPixelText },
	{ "-wp",
	  "   -wp <color>     specify a WhitePixel color\n",
	  getWhitePixelText },
#if defined(AIXrt) || defined(AIX386)
	{ "-a",
	  "   -a  <accel>     specify the mouse acceleration\n",
	  (int (*)()) 0 }, /* DIX Argument */
	{ "-c",
	  "   -c  <volume>    specify the key click volume\n",
	  processKeyClick },
	{ "-D",
	  "   -D  <file>      specify the color definition data base file\n",
	  processRGBpath },
	{ "-f",
	  "   -f  <volume>    specify the beep volume\n",
	  (int (*)()) 0 }, /* DIX Argument */
	{ "-fc",
	  "   -fc <font>      specify the font for cursor glyphs and masks\n",
	  (int (*)()) 0 }, /* DIX Argument */
	{ "-fn",
	  "   -fn <font>      specify the default text font\n",
	  (int (*)()) 0 }, /* DIX Argument */
	{ "-fp",
	  "   -fp <path>      specify the search path for fonts\n",
	  (int (*)()) 0 }, /* DIX Argument */
	{ "-n",
	  "   -n :<num>       specify the connection number\n",
	  processAIXdisplaySpec },
	{ "-p",
	  "   -p  <interval>  specify the screen saver interval\n",
	  (int (*)()) 0 }, /* DIX Argument */
	{ "-s",
	  "   -s <delay>      specify the screen saver delay\n",
	  (int (*)()) 0 }, /* DIX Argument */
	{ "-t",
	  "   -t <threshold>  specify the mouse threshold\n",
	  (int (*)()) 0 }, /* DIX Argument */
	{ "-to",
	  "   -to <time>      specify the time between connection checks\n",
	  (int (*)()) 0 }, /* DIX Argument */
	{ "-r",
	  "   -r              disables auto repeat\n",
	  (int (*)()) 0 }, /* DIX Argument */
	{ "-T",
	  "   -T              disable the Ctrl-Alt-Backspace key sequence\n",
	  dontZap },
	{ "-v",
	  "   -v              activate screen saver with background color\n",
	  (int (*)()) 0 }, /* DIX Argument */
#endif
	{ "-lock",
	  "   -lock           enable caps lock key (default)\n",
	  dolockKey },
	{ "-nolock",
	  "   -nolock         disable caps lock key\n",
	  doNolockKey },
#if !defined(NO_SECRETS)
#if defined(IBM_OS_HAS_HFT)
#if defined(IBM_GSL)
	{ "-dd",
	  "   -dd <ScreenID>  use screen of type <ScreenID> for gsl device\n",
	  processGSLscreen },
#endif
	{ "-mdelay",
	  "   -mdelay <count> set delay for middle button chord (default 10)\n",
	  processHFTmouseDelay },
#endif
	{ "-quiet",
	  "   -quiet          do not print information messages (default)\n",
	  setQuiet },
	{ "-verbose",
	  "   -verbose        print information messages\n",
	  setVerbose },
	{ "-rtkeys",
	  "   -rtkeys         use CAPS LOCK and CTRL as labelled (default)\n",
	  doRTkeys },
	{ "-pckeys",
	  "   -pckeys         swap CAPS LOCK and CTRL (for touch typists)\n",
	  doPCkeys },
#if defined( IBM_BS_FLAGS )
	{ "-bs",
	  "   -bs             enable backing store (default)\n",
	  processDashBS },
	{ "-nobs",
	  "   -nobs           disable backing store\n",
	  processDashNoBS },
#endif /* IBM_BS_FLAGS */
#if defined(IBM_OS_HAS_HFT)
	{ "-refresh",
	  "   -refresh        refresh clients to restore HFT\n",
	  processRefreshArg },
	{ "-repaint",
	  "   -repaint        repaint to restore HFT (default)\n",
	  processRepaintArg },
#endif
#if !defined(IBM_MUST_USE_HDWR) && defined(ibm032)
	{ "-nohdwr",
	  "   -nohdwr         use generic functions where applicable\n",
	  doUseHardwareArg },
#endif
#ifdef IBM_SPECIAL_MALLOC
	{ "-malloc",
	  "   -malloc [0-5]   set malloc check level\n",
	  doMallocArg },
	{ "-plumber",
	  "   -plumber <file> dump malloc arena to named file\n",
	  doPlumberArg },
#endif
#ifdef TRACE_X
	{ "-trace",
	  "   -trace          trace execution of IBM specific functions\n",
	  doTrace },
#endif /* TRACE_X */
	{ "-wrap",
	  "   -wrap           wrap mouse in both dimensions\n",
	  setWrapXY },
	{ "-wrapx",
	  "   -wrapx          wrap mouse in X only\n",
	  setWrapX },
	{ "-wrapy",
	  "   -wrapy          wrap mouse in Y only\n",
	  setWrapY },
/* This MUST be last, i.e. just before screen processing */
	{ "-all",
	  (char *) 0, /* The use string is hard-wired below !! */
	  doOpenAllScreens },
#endif
	{ 0, 0, 0 }
} ;

int osCmdArgTableSize = ( sizeof osCmdArgTable / sizeof (ArgumentStr) ) - 1 ;

/***==================================================================***/

void
ddxUseMsg()
{
register ArgumentStr *argDescPtr ;
register int i = osCmdArgTableSize ;

#if defined(AIXrt) || defined(AIX386)
ErrorF( "The \"X\" command accepts the following flags:\n" ) ;
#endif

ErrorF( "Recognized screens are:\n" ) ;
ErrorF( "    -all           opens all attached, supported screens\n" ) ;
for ( i = 0 ; ibmPossibleScreens[i] ; i++ ) {
	ErrorF( "    %s\n", ibmPossibleScreens[i]->ibm_ScreenFlag ) ;
}
ErrorF( "Other device dependent options are:\n" ) ;
for ( argDescPtr = &osCmdArgTable[0], i = osCmdArgTableSize ;
      i-- ;
      argDescPtr++ )
	if ( argDescPtr->usageString )
		ErrorF( argDescPtr->usageString ) ;
ErrorF(
   "See the X User's Guide or X server manual page for more information\n" ) ;
return ;
}

int
ddxProcessArgument( argc, argv, i )
int argc ;
char **argv ;
int i ;
{
register ArgumentStr *argDescPtr ;
register int swallowed ;
register int j ;
register int skip ;

for ( argv += i, argc -= i, skip = 0, swallowed = 1 ; swallowed && argc ; ) {
	for ( swallowed = 0, argDescPtr = &osCmdArgTable[0],
	      j = osCmdArgTableSize ;
	      !swallowed && j-- ;
	      argDescPtr++ )
		if ( !strcmp( *argv, argDescPtr->argString ) )
			if ( !argDescPtr->processFunc || !( swallowed =
				  (* argDescPtr->processFunc)( argv ) ) )
				return skip ;
			else { /* Done with this arg */
				skip += swallowed ;
				argc -= swallowed ;
				argv += swallowed ;
			}
	/* Only remaining possibility is the name of a display */
	/* Now Check If it's a Screen Name */
	if ( !swallowed ) {
		register ibmPerScreenInfo **ppScr = ibmPossibleScreens ;
		register ibmPerScreenInfo  *pScr ;

		while ( ( pScr = *ppScr++ ) && pScr->ibm_ScreenFlag ) {
		    if ( !( swallowed = strcmp( *argv, pScr->ibm_ScreenFlag ) ) ) {
			ibmScreensWanted++ ;
			if ( ( pScr->ibm_ScreenFD = (*(pScr->ibm_ProbeFunc))() ) >= 0 )
			    ibmScreens[ibmNumScreens++] = pScr ;
			else
			    ErrorF( "%s not available\n", *argv + 1 ) ;
			break ; /* We Found The Display Name */
		    }
		}
#ifndef DISABLE_ADDITIONAL_SCREENS
		if (swallowed && (ibmAdditionalScreenArg)) {
		    ibmInfoMsg("Looking for vendor displays...\n");
		    pScr = (*ibmAdditionalScreenArg)(*argv);
		    if (pScr) {
			swallowed = 0; /* flag this guy finished */
			ibmScreensWanted++ ;
			if ((pScr->ibm_ScreenFD = (*(pScr->ibm_ProbeFunc))()) >= 0)
			    ibmScreens[ibmNumScreens++] = pScr ;
			else
			    ErrorF( "%s not available\n", *argv + 1 ) ;
		    }
		}
#endif /* DISABLE_ADDITIONAL_SCREENS */
		if ( swallowed ) {
#if defined(DYNAMIC_LINK_SCREENS)
		    /* No More pre-linked screens! Try dynamic linking. */
		    if ( pScr = ibmDynamicScreenAttach( *argv ) ) {
			ibmScreens[ ibmNumScreens++ ] = pScr ;
		    }
		    else
#endif
			/* We Have Used Up ALL Of The Possible Argument Strings
			 * Then This Doesn't Belong To Us.
			 * So It's Time To Return.
			 */
			return skip ;
		}
		else
			skip++ ;
	}
    /* Process Next Argument */
}

return skip ;
}
