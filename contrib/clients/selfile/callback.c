#ifndef lint
static char rcsid[] = "$Header: callback.c,v 1.5 89/08/30 16:02:06 erik Exp $";
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
#include <X11/Xos.h>
#include <sys/stat.h>

extern char *re_comp();

static int
match(exp, str)
	char	*exp, *str;
{
	char	*err;

	if (err = re_comp(exp)) {
		(void) fprintf(stderr, "re_comp(\"%s\"): %s\n", exp, err);
		exit(1);
	}

	return re_exec(str);
}

int
fileCheck(realFileName, shownFileName, fileStat)
	char		*realFileName;
	char		**shownFileName;
	struct stat	*fileStat;
{
	static char	shown[256];

	switch (match("^..*\\.plt$", realFileName)) {
	case 0:
	case -1:
	default:
		break;
	case 1:
		if ((fileStat->st_mode & S_IFMT) == S_IFREG) {
			(void) strcpy(shown, realFileName);
			*(rindex(shown, '.')) = 0;
			*shownFileName = shown;
		}
		return 1;
	}

	switch (match("^..*\\.xy$", realFileName)) {
	case 0:
	case -1:
	default:
		break;
	case 1:
		return 0;
	}

	switch (match("^..*\\.z$", realFileName)) {
	case 0:
	case -1:
	default:
		break;
	case 1:
		return 0;
	}

	return 1;
}
