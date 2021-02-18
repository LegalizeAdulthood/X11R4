#ifndef lint
static char rcsid[] = "$Header: xdir.c,v 1.4 89/08/30 16:02:39 erik Exp $";
#endif

/*
 * Copyright 1989 Software Research Associates, Inc., Tokyo, Japan
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Software Research Associates
 * makes no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * SOFTWARE RESEARCH ASSOCIATES DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL SOFTWARE RESEARCH ASSOCIATES BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Erik M. van der Poel
 *         Software Research Associates, Inc., Tokyo, Japan
 *         erik@sra.co.jp
 */

#include <stdio.h>
#include <X11/Intrinsic.h>

main(argc, argv)
	int	argc;
	char	*argv[];
{
	Widget	XDir;
	char	*path;

	XDir = XtInitialize(argv[0], "XDir", (XrmOptionDescRec *) NULL,
		(Cardinal) 0, (Cardinal *) &argc, argv);

	while (1) {
		if (XsraSelFile(
			XtDisplay(XDir),
			(unsigned char *) "Pathname to store in cut buffer:",
			(unsigned char *) "Cut",
			(unsigned char *) "Exit",
			(char *) NULL,
			(int (*)()) NULL,
			&path
		)) {
			break;
		}
		XStoreBytes(XtDisplay(XDir), path, strlen(path));
		XtFree(path);
	}

	return 0;
}
