#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <Xcu/Wlm.h>

static void exit_routine () ;

main(argc, argv)
    int argc ;
    char **argv ;
{
Arg wlm_arg ;
Widget top, wlm_id ;

top = XtInitialize ( NULL, "top", NULL, NULL, &argc, argv ) ;
XtSetArg (wlm_arg, XtNfile, argv[1]) ;
wlm_id = XtCreateManagedWidget ( "wlm", xcuWlmWidgetClass, top, &wlm_arg, 1 ) ;

XcuWlmAddCallback ( wlm_id, NULL,
		   "XcuCommand", "exit", "callback",
		   exit_routine, NULL ) ;
XtRealizeWidget (top) ;

for (;;) {
	XcuWlmRequest ( wlm_id ) ;
	}
}

static void
exit_routine ()
{
exit (0) ;
}

#include <Xcu/WlmP.h>
#include <Xcu/Wlm.h>
#include <Xcu/Command.h>

void
make_tag_class_list (ww)
    XcuWlmWidget ww ;
{
TAG_CLASS_ENTRY(ww, "XcuWlm", xcuWlmWidgetClass) ;
TAG_CLASS_ENTRY(ww, "XcuCommand", xcuCommandWidgetClass) ;
return ;
}

/*
 * Copyright 1989 Cornell University
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Cornell U. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Cornell U. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * CORNELL UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORNELL UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Gene W. Dykes, Program of Computer Graphics
 *          120 Rand Hall, Cornell University, Ithaca, NY 14853
 *          (607) 255-6713   gwd@freedom.graphics.cornell.edu
 */

