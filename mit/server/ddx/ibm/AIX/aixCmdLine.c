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
#include <sys/select.h>
#include <ctype.h>

#include "hftUtils.h"

#include "ibmTrace.h"

static char sccsid[] = "@(#)AIXCmdLine.c	3.3 88/09/26 03:52:28";

extern	int	 AIXMouseChordDelay;

#ifdef IBM_GSL
extern	char	*gslCommandLineScreenOption;
#endif

/***====================================================================***/

#ifndef NO_SECRETS
static	int	aixShowSecretArgs= 0;
#endif

void
AIXUseMsg()
{
    ErrorF("The \"X\" command accepts the following flags:\n");
    ErrorF("   -a  <accel>     specify the mouse acceleration\n");
    ErrorF("   -bp <color>     specify a BlackPixel color\n");
    ErrorF("   -c  <volume>    specify the key click volume\n");
    ErrorF("   -D  <file>      specify the color definition data base file\n");
    ErrorF("   -f  <volume>    specify the beep volume\n");
    ErrorF("   -fc <font>      specify the font for cursor glyphs and masks\n");
    ErrorF("   -fn <font>      specify the default text font\n");
    ErrorF("   -fp <path>      specify the search path for fonts\n");
    ErrorF("   -m              use monochrome display characteristics\n");
    ErrorF("   -n :<num>       specify the connection number\n");
    ErrorF("   -p  <interval>  specify the screen saver interval\n");
    ErrorF("   -r              disables auto repeat\n");
    ErrorF("   -s <delay>      specify the screen saver delay\n");
    ErrorF("   -t <threshold>  specify the mouse threshold\n");
    ErrorF("   -to <time>      specify the time between connection checks\n");
    ErrorF("   -T              disable the Ctrl-Alt-Backspace key sequence\n");
    ErrorF("   -v              activate screen saver with background color\n");
    ErrorF("   -wp <color>     specify a WhitePixel color\n");
#ifndef NO_SECRETS
    if (aixShowSecretArgs) {
    ErrorF("For the moment, X also accepts some undocumented, unsupported\n");
    ErrorF("flags.  These flags are *not* guaranteed to exist in future\n");
    ErrorF("releases or to work all that well in this release.\n");
    ErrorF("These unsupported flags are:\n");
    ErrorF("   -bs             enable backing store (default)\n");
#ifdef IBM_SPECIAL_MALLOC
    ErrorF("   -malloc [0-5]   set malloc check level\n");
#endif
    ErrorF("   -mdelay <count> set delay for middle button chord (default 10)\n");
    ErrorF("   -nobs           disable backing store\n");
    ErrorF("   -nohdwr         use generic functions where applicable\n");
    ErrorF("   -pckeys         swap CAPS LOCK and CTRL (for touch typists)\n");
#ifdef IBM_SPECIAL_MALLOC
    ErrorF("   -plumber <file> dump malloc arena to named file\n");
#endif
    ErrorF("   -quiet          do not print information messages (default)\n");
    ErrorF("   -refresh        refresh clients to restore HFT\n");
    ErrorF("   -repaint        repaint to restore HFT (default)\n");
    ErrorF("   -rtkeys         use CAPS LOCK and CTRL as labelled (default)\n");
#ifdef TRACE_X
    ErrorF("   -trace          trace execution of IBM specific functions\n");
#endif /* TRACE_X */
    ErrorF("   -verbose        print information messages\n");
    ErrorF("   -wrap           wrap mouse in both dimensions\n");
    ErrorF("   -wrapx          wrap mouse in X only\n");
    ErrorF("   -wrapy          wrap mouse in Y only\n");
    }
#endif
    ErrorF("See the X User's Guide or X server manual page for more information\n");
}

/***====================================================================***/

extern	int	ibmRefreshOnActivate;

int
AIXProcessArgument(argc,argv,i)
int	 argc;
char	*argv[];
int	i;
{
extern	char	*rgbPath;
extern	char	*display;

    TRACE(("AIXProcessArgument(%d,0x%x,%d)\n",argc,argv,i));
    if	    (( strcmp( argv[i], "-c" ) == 0) && ((i+1) < argc))
    {
	   if ((argv[i+1][0] >= '0') && (argv[i+1][0] <= '9'))
		argv[i] = "c";
	   return(0);
    }
    else if ( strcmp( argv[i], "-D" ) == 0)
    {
	    if(++i < argc)
		rgbPath = argv[i];
	    else 
		return(0);
	    return(2);
    }
#ifdef IBM_GSL
    else if (strcmp(argv[i], "-dd") == 0) {
	if (++i<argc) {
	    gslCommandLineScreenOption= argv[i];
	    return(2);
	}
    }
#endif
    else if (strcmp(argv[i], "-mdelay") == 0 ) { 
	if ((++i<argc)&&(isdigit(argv[i][0]))) {
	    AIXMouseChordDelay= atoi(argv[i]);
	    return(2);
	}
    }
    else if ( strcmp( argv[i], "-n") == 0) {
	char *dpy= argv[++i];

	if (i<argc) {
	    if (dpy[0]==':') {
		display= &dpy[1];
	    }
	    else {
		ErrorF("display specification must begin with ':'\n");
		UseMsg();
		exit(1);
	    }
	}
	else {
	    ErrorF("must specify display number after -n\n");
	    UseMsg();
	    exit(1);
	}
	return(2);
    }
    else if ( strcmp( argv[i], "-refresh" ) == 0 ) {
	ibmRefreshOnActivate= 1;
	return(1);
    }
    else if ( strcmp( argv[i], "-repaint" ) == 0 ) {
	ibmRefreshOnActivate= 0;
	return(1);
    }
#ifndef NO_SECRETS
    else if (strcmp(argv[i],"-secrethelp")==0) {
	aixShowSecretArgs= 1;
	AIXUseMsg();
	exit(1);
    }
    else if (strcmp(argv[i],"-erik")==0) {
	extern int ibmUsePCKeys,ibmXWrapScreen,ibmRefreshOnActivate;
	ibmUsePCKeys= 1;
	ibmXWrapScreen= 1;
	ibmRefreshOnActivate= 1;
	return(1);
    }
#endif
    return(0);
}

