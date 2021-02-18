#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: exp_path.c,v 5.7 89/11/22 15:15:03 glennw Exp $";
#endif /* RCS_ID */
#endif /* LINT */

/*
 * Copyright 1987, 1988 by Digital Equipment Corporation, Maynard,
 * Massachusetts, and the Massachusetts Institute of Technology, Cambridge,
 * Massachusetts.  Copyright 1987 by Jordan Hubbard.  Copyright 1987, 1988 by
 * Ardent Computer Corporation, Sunnyvale, Ca.  Copyright 1989 by Tektronix,
 * Inc., Beaverton, Oregon.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Ardent Computer Corporation, Jordan
 * Hubbard, Digital Equipment, Tektronix or M.I.T.  not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Ardent Computer Corporation,
 * Jordan Hubbard, Digital Equipment, Tektronix and M.I.T.  make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL EQUIPMENT, TEKTRONIX,
 * AND M.I.T.  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL
 * EQUIPMENT, TEKTRONIX, OR M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 */

#ifdef PCS
#include <unistd.h>
#endif
#include <pwd.h>
#include <stdio.h>
#if defined(vax)
#include <sys/file.h>
#else
/* <fcntl.h> included in Xos.h */
#endif /* vax */
#include "awm.h"

/*
 * WARNING: This code is obscure.
 * Modify at your Own Risk.
 */
char *expand_file(s)
register char *s;
{
     static char tmp[256], *cp, err[80];

     Entry("expand_file")
     
     /* zero tmp */
     tmp[0] = 0;
     if (*s == '/')
	  Leave(s)
     if (*s == '~') {
	  if (s[1] == '/') { /* It's $HOME */
	       if (!(cp = (char *)getenv("HOME"))) {
		    sprintf(err, "expand: Can't find $HOME!\n");
		    yywarn(err);
		    Leave(0)
	       }
	       strcpy(tmp, cp);
	       strcat(tmp, s + 1);
	       Leave(tmp)
	  }
	  else { /* it's another user */
	       struct passwd *pwd;
	       char uname[32];
	       int i;

	       for (i = 1; s[i] != '/'; i++)
		    uname[i - 1] = s[i];
	       uname[i] = '\0';
	       pwd = getpwnam(uname);
	       if (!pwd) {
		    sprintf(err, "expand: user '%s' not found.\n", uname);
		    yywarn(err);
		    Leave(0)
	       }
	       strcpy(tmp, pwd->pw_dir);
	       strcat(tmp, s + i);
	       Leave(tmp)
	  }
     }
     else
	  Leave(s)
}

char *deblank(s)
register char *s;
{
     Entry("deblank")

     if (!s)
	  Leave(s)
     while (*s && (*s == ' ' || *s == '\t'))
	  s++;
     Leave(s)
}

char *expand_from_path(s)
register char *s;
{
     char tmp[256], *tm;
     int i, plen;

     Entry("expand_from_path")

     tmp[0] = '\0';
     s = deblank(s);
     if (!s || !*s)
	  Leave(0)
     s = expand_file(s);
     if (!s)
	  Leave(0)
     if (!access(s, R_OK))
	  Leave(s)
     /*
      * If it starts with a slash, we know it either expanded and couldn't
      * be found, or that it started with a slash in the first place and
      * just plain couldn't be found.
      */
     if (*s == '/')
	  Leave(0)
     /*
      * At this stage we haven't found the file by name, so it's time to
      * search the path.
      */
     if (!awmPath || !*awmPath)
	  Leave(0)
     plen = strlen(awmPath);
     i = 0;
     while (1) {
	  int p, l;
	  
	  tmp[0] = '\0';
	  while (i < plen && awmPath[i] == ' ' || awmPath[i] == '\t')
	       i++;
	  for (p = i; p < plen && awmPath[p] != ' ' && awmPath[p] != '\t'; p++)
	       tmp[p - i] = awmPath[p];
	  if (!*tmp)
	       Leave(0)
	  tmp[p - i] = '\0';
	  i = p;
	  tm = expand_file(tmp);
	  if (!tm || !*tm)
	       continue;
	  l = strlen(tm);
	  if (l < 1)
	       continue;
	  if (tm[l - 1] != '/') { /* append / if necessary */
	       tm[l] = '/';
	       tm[++l] = '\0';
	  }
	  strcat(tm, s);
	  if (!access(tm, R_OK))
	       Leave(tm)
     }
}
