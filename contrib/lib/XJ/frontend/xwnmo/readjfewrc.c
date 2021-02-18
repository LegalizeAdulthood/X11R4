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

/*	Version 4.0
 */

#include <stdio.h>
#include "xwnmo.h"
#include "commonhd.h"
#ifdef SYSVR2
#   include <string.h>
#endif
#ifdef BSD42
#   include <strings.h>
#endif
#include "sdefine.h"
#include "sheader.h"

char	*read_jfew_rc_prgnm = "jfewrc";
char	*read_jfew_rc_msg = "Warning! ";
int	read_jfew_rc_err = 1;

/** jfew_rc_file を見てパラメータの設定 */
read_jfew_rcfile(filenm, expand_filenm)
char	*filenm;
char	*(*expand_filenm)();
{
    char buf[BUFSIZ];
    char entrynm[64];
    char s[7][MAXPATHLEN + 1];
    int	n, ret, d1;
    int	ln, warn_occur, illegal, no_entry;
    FILE *fp;
    int t = 1;
    extern struct User_uumrc user_uumrc[];

    ln = warn_occur = illegal = no_entry = 0;

    if ((fp = fopen(filenm, "r")) == NULL) {
	return(-1);
    }

    while(fgets(buf, BUFSIZ, fp)) {
	ln++;
	if (comment_char(*buf) ||
	    (n = sscanf(buf, "%s %s %s %s %s %s %s %s", entrynm,
			s[0],s[1],s[2],s[3],s[4],s[5],s[6])) < 1) {
		continue;
	}
	switch(*(entrynm + 3)) {
	case '_':
	    if (!strcmp(entrynm, "set_xwnmo_title")) {
		if (n > 1) {
#ifndef	SYSVR2
		    xscanf(buf, "%s %s", entrynm, s[0]);
#endif  /* !SYSVR2 */
		    strncpy(xwnmo_title, s[0], 80);
		} else {
		    illegal++;
		}
	    } else
		no_entry++;
	    break;
	case 'f':
	    if (!strcmp(entrynm, "setfontascii")) {
		if (n > 1) {
		  if (!(defined_by_option & OPT_ASCFONT)) {
		    strncpy(ascii_font, s[0], 62);
		    if (n > 2)
			strncpy(ascii_escape, s[1], 4);
		    }
		} else
		    illegal++;
	    } else if (!strcmp(entrynm, "setfontkanji")) {
		if (n > 1) {
		  if(!(defined_by_option & OPT_KANJIFONT)) {
		    strncpy(kanji_font, s[0], 62);
		    if (n > 2)
			strncpy(kanji_escape, s[1], 4);
		  }
		} else
		    illegal++;
	    } else if (!strcmp(entrynm, "setfontkana")) {
		if (n > 1) {
		  if(!(defined_by_option & OPT_KANAFONT)) {
		    strncpy(kana_font, s[0], 62);
		    if (n > 2)
			strncpy(kana_escape, s[1], 4);
		  }
		} else
		    illegal++;
	    } else if (!strcmp(entrynm, "setfontgaiji")) {
		if (n > 1) {
		  if(!(defined_by_option & OPT_GAIJIFONT)) {
		    strncpy(gaiji_font, s[0], 62);
		    if (n > 2)
			strncpy(gaiji_escape, s[1], 4);
		  }
		} else
		    illegal++;
	    } else if (!strcmp(entrynm, "setfontutilascii")) {
		if (n > 1) {
		  if(!(defined_by_option & OPT_UTILASCFONT)) {
		    strncpy(util_ascii_font, s[0], 62);
		    if (n > 2)
			strncpy(ascii_escape, s[1], 4);
		  }
		} else
		    illegal++;
	    } else if (!strcmp(entrynm, "setfontutilkanji")) {
		if (n > 1) {
		  if(!(defined_by_option & OPT_UTILKANJIFONT)) {
		    strncpy(util_kanji_font, s[0], 62);
		    if (n > 2)
			strncpy(kanji_escape, s[1], 4);
		  }
		} else
		    illegal++;
	    } else if (!strcmp(entrynm, "setfontutilkana")) {
		if (n > 1) {
		  if(!(defined_by_option & OPT_UTILKANAFONT)) {
		    strncpy(util_kana_font, s[0], 62);
		    if (n > 2)
			strncpy(kana_escape, s[1], 4);
		  }
		} else
		    illegal++;
	    } else if (!strcmp(entrynm, "setfontutilgaiji")) {
		if (n > 1) {
		  if(!(defined_by_option & OPT_UTILGAIJIFONT)) {
		    strncpy(util_gaiji_font, s[0], 62);
		    if (n > 2)
			strncpy(gaiji_escape, s[1], 4);
		  }
		} else
		    illegal++;
	    } else
		no_entry++;
	    break;
	case 'd':
	    if (!strcmp(entrynm, "setdefwidth")) {
		if (n > 1 && (change_ascii_to_int(s[0],&d1) != -1)) {
		    def_width = d1;
		} else
		    illegal++;
	    } else if (!strcmp(entrynm, "setdefbwidth")) {
		if (n > 1 && (change_ascii_to_int(s[0],&d1) != -1)) {
		    def_bwidth = d1;
		} else
		    illegal++;
	    } else  if (!strcmp(entrynm, "setdisplayname")) {
		if (n > 1) {
		  if(!(defined_by_option & OPT_DISPLAYNAME)) {
		    strncpy(display_name, s[0], MAXNAMLEN);
		  }
		} else
		    illegal++;
	    } else
		no_entry++;
	    break;
	case 'c':
	    if (!strcmp(entrynm, "setcolorfore")) {
		if (n > 1) {
		  if(!(defined_by_option & OPT_FOREGROUND)) {
		    strncpy(fore_color, s[0], MAXNAMLEN);
		  }
		} else
		    illegal++;
	    } else if (!strcmp(entrynm, "setcolorback")) {
		if (n > 1) {
		  if(!(defined_by_option & OPT_BACKGROUND)) {
		    strncpy(back_color, s[0], MAXNAMLEN);
		  }
		} else
		    illegal++;
	    } else if (!strcmp(entrynm, "setcolorborder")) {
		if (n > 1) {
		  if(!(defined_by_option & OPT_BORDERGROUND)) {
		    strncpy(boder_color, s[0], MAXNAMLEN);
		  }
		} else
		    illegal++;
	    } else if (!strcmp(entrynm, "setcvtfunkey")) {
		if (n > 1) {
		  if(!(defined_by_option & OPT_FUNFILE)) {
		    if ((*expand_filenm)(s[0]) != 0) {
			fprintf(stderr , "Can't expand %s.\r\n", s[0]);
		    }
		    strncpy(cvt_fun_file, s[0], MAXPATHLEN);
		  }
		} else
		    illegal++;
	    } else if (!strcmp(entrynm, "setcvtmetakey")) {
		if (n > 1) {
		  if(!(defined_by_option & OPT_MTFILE)) {
		    if ((*expand_filenm)(s[0]) != 0) {
			fprintf(stderr , "Can't expand %s.\r\n", s[0]);
		    }
		    strncpy(cvt_meta_file, s[0], MAXPATHLEN);
		  }
		} else
		illegal++;
	    } else
		no_entry++;
	    break;
	case 'j':
	    if (!strcmp(entrynm, "setjumpkey"))
		jumpkey_mode = 1;
	    else
		no_entry++;
	    break;
	case 'u':
	    if (!strcmp(entrynm, "setuumrc")) {
		if (n > 2) {
		    strncpy(user_uumrc[t].user_name, s[0], MAXPATHLEN);
		    if ((*expand_filenm)(s[1]) != 0) {
			fprintf(stderr , "Can't expand %s.\r\n", s[1]);
		    }
		    strncpy(user_uumrc[t].uumrc_name, s[1], MAXPATHLEN);
		    t++;
		} else if (n > 1)  {
		    if ((*expand_filenm)(s[0]) != 0) {
			fprintf(stderr , "Can't expand %s.\r\n", s[0]);
		    }
		    strncpy(user_uumrc[0].uumrc_name, s[0], MAXPATHLEN);
		} else
		    illegal++;
	    } else
		no_entry++;
	    break;
	default:
	    no_entry++;
	    break;
	}
	if (no_entry) {
	    warn_occur++;
	    warning(filenm);
	    fprintf(stderr, "\tunknown entry name \"%s\".", entrynm);
	    no_entry = 0;
	}
	if (illegal) {
	    warn_occur++;
	    warning(filenm);
	    fprintf(stderr, "\tIllegal at line %d.", ln);
	    illegal = 0;
	}
    }
    fclose(fp);
    return(warn_occur ? 1 : 0);
}



static warning(filenm)
char	*filenm;
{
	if (read_jfew_rc_err) {
		fprintf(stderr, "%s%s: File \"%s\".", read_jfew_rc_msg, read_jfew_rc_prgnm, filenm);
	}
}


char	*getenv();

read_jfewrc()
{
	register char	*n;
	char	jfew_rc_file[MAXPATHLEN + 1];
	char	*expand_expr(), *check_usr_dic_dir_var();
	extern char	*read_jfew_rc_prgnm, *read_jfew_rc_msg;

	if ((n = getenv(XWNMO_RCENV)) && *n) {
		if (read_jfew_rcfile(n, expand_expr) >= 0) {
			/*
			using_file(read_jfew_rc_prgnm, n);
			*/
			return(0);
		}
		fprintf(stderr, "%s%s : File \"%s\" can't open.", read_jfew_rc_msg, read_jfew_rc_prgnm, n);
	}
	if ((n = getenv("HOME")) && *n) {
		strcat(strcpy(jfew_rc_file, n), USR_XWNMORC);
		if (read_jfew_rcfile(jfew_rc_file, expand_expr) >= 0) {
			/*
			using_file(read_jfew_rc_prgnm, jfew_rc_file);
			*/
			return(0);
		}
	}
	if (read_jfew_rcfile(XWNMORCFILE, expand_expr) >= 0) {
		/*
		using_file(read_jfew_rc_prgnm, XWNMORCFILE);
		*/
	}
	return(0);
}
