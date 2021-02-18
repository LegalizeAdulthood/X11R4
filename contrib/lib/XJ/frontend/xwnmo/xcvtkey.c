/*
 *	Copyright 1989 OMRON TATEISI ELECTRONICS CO.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of OMRON not be used in 
 * advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission.  OMRON makes no representations 
 * about the suitability of this software for any purpose.  It is provided 
 * "as is" without express or implied warranty.
 *
 * OMRON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * OMRON BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE. 
 *
 *	Author:	Seiji Kuwari	OMRON TATEISI ELECTRONICS CO.
 *				kuwa@frf.omron.junet
 *				uunet!nff.ncl.omron.co.jp!kuwa
 */				

/*************************************************************************
		xcvtkey.c
*/

#include <stdio.h>
#include <ctype.h>
#include "xcvtkey.h"
#ifdef BSD42
#include <strings.h>
#endif
#ifdef SYSVR2
#include <string.h>
#include <fcntl.h>
#endif

#define	ENTRY_CNT	256
struct CONVCODE cvt_fun_tbl[ENTRY_CNT];
struct CONVCODE cvt_meta_tbl[ENTRY_CNT];

int cvt_fun_tbl_cnt = 0; /* convert table count */
int cvt_meta_tbl_cnt = 0; /* convert table count */

cvt_fun_setup(filename)
char *filename;
{
    int compar();
    register int c, i, k, cnt = 0;
    int warn_occur = 0, ln = 0;
    char ksname[32], code[32];
    char buf[BUFSIZ];
    FILE *fp;

    cvt_fun_tbl_cnt = 0;
    if ((fp = fopen(filename, "r")) == NULL) {
	return(-1);
    }

    while(fgets(buf, BUFSIZ, fp)) {
	ln++;
	if (comment_char(*buf) || (k = sscanf(buf, "%s %s", ksname, code)) <= 0)
	    continue;
	if (k != 2) {
	} else if ((c = XStringToKeysym(ksname)) == NULL) {
	    /*msg(0, "\t\"%s\" is not define Keysym.", ksname); */
	} else {
	    cvt_fun_tbl[cnt].fromkey = c;
	    cvt_fun_tbl[cnt].tokey = strtol(code, (char **)NULL, 0);
	    cnt++;
	}
    }
    fclose(fp);
    cvt_fun_tbl_cnt = cnt;
}

cvt_meta_setup(filename)
char *filename;
{
    register int c, i, k, cnt = 0;
    int warn_occur = 0, ln = 0;
    char ksname[32], code[32];
    char buf[BUFSIZ];
    FILE *fp;

    cvt_meta_tbl_cnt = 0;
    if ((fp = fopen(filename, "r")) == NULL) {
		return(-1);
    }

    while(fgets(buf, BUFSIZ, fp)) {
	ln++;
	if (comment_char(*buf) || (k = sscanf(buf, "%s %s", ksname, code)) <= 0)
	    continue;
	if (k != 2) {
	} else if ((c = XStringToKeysym(ksname)) == NULL) {
	    /*msg(0, "\t\"%s\" is not define Keysym.", ksname); */
	} else {
	    cvt_meta_tbl[cnt].fromkey = c;
	    cvt_meta_tbl[cnt].tokey = strtol(code, (char **)NULL, 0);
	    cnt++;
	}
    }
    fclose(fp);
    cvt_meta_tbl_cnt = cnt;
}


/* Function of convert */

cvtfun(keycode)
unsigned int	keycode;
{
    register int cnt;

    for(cnt = 0; cnt < cvt_fun_tbl_cnt; cnt++) {
	if (cvt_fun_tbl[cnt].fromkey == keycode)
	    return(cvt_fun_tbl[cnt].tokey);
    }
    return(-1);
}


cvtmeta(keycode)
unsigned int	keycode;
{
    register int cnt;

    for(cnt = 0; cnt < cvt_meta_tbl_cnt; cnt++) {
	if (cvt_meta_tbl[cnt].fromkey == keycode)
	    return(cvt_meta_tbl[cnt].tokey);
    }
    return(-1);
}

char *comment_char_str = "#;";

comment_char(c)
register char	c;
{
	return((strchr(comment_char_str, c) != NULL) ? 1 : 0);
}
