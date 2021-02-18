/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	January 1985.
 *	1st revision : Aug 1985.
 *
 *	%W%	%G%
*/
#ifndef	X11

#include <sys/types.h>
#include <sys/file.h>
#include <sys/time.h>

#else

#include <X11/Xos.h>

#endif

#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#ifndef M_PI
#define M_PI    3.14159265358979323846
#define M_PI_2  1.57079632679489661923
#endif

#ifndef	X11

#include <suntool/tool_hs.h>
#include <suntool/wmgr.h>
#include <suntool/menu.h>

#else

#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Command.h>
#include <X11/Label.h>
#include <X11/Dialog.h>
#include <X11/Box.h>
#include <X11/Form.h>

#endif	X11
