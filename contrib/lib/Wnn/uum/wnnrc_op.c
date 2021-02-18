/*
	Copyright
		Kyoto University Research Institute for Mathematical Sciences
		1987, 1988, 1989
	Copyright OMRON TATEISI ELECTRONICS CO. 1987, 1988, 1989
	Copyright ASTEC, Inc. 1987, 1988, 1989


    Permission to use, copy, modify, and distribute this software
    and its documentation for any purpose and without any fee is
    hereby granted, subject to the following restrictions:

    The above copyright notice and this permission notice must appear
    in all versions of this software;

    The name of "Wnn" may not be changed; 

    All documentations of software based on "Wnn" must contain the wording
    "This software is based on the original version of Wnn developed by
    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
    OMRON TATEISI ELECTRONICS CO. and
    ASTEC, Inc.", followed by the above copyright notice;

    The name of KURIMS, OMRON and ASTEC may not be used
    for any purpose related to the marketing or advertising
    of any product based on this software.

    KURIMS, OMRON and ASTEC make no representations about
    the suitability of this software for any purpose.
    It is provided "as is" without express or implied warranty.

    KURIMS, OMRON and ASTEC DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
    SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
    IN NO EVENT SHALL KURIMS, OMRON AND ASTEC BE LIABLE FOR ANY SPECIAL,
    INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
    LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
    OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.

    Author: Hideki Tsuiki	Kyoto University
				tsuiki%kaba.or.jp@uunet.uu.net

	    Hiroshi Kuribayashi	OMRON TATEISI ELECTRONICS CO.
				kuri@frf.omron.co.jp
				uunet!nff.ncl.omron.co.jp!kuri

	    Naouki Nide		Kyoto University
				nide%kaba.or.jp@uunet.uu.net

	    Shozo Takeoka	ASTEC, Inc.
				take%astec.co.jp@uunet.uu.net

	    Takashi Suzuki	Advanced Software Technology & Mechatronics
				Research Institute of KYOTO
				suzuki%astem.or.jp@uunet.uu.net

*/
/*	Version 4.0
 */

/* uumrc operations */


#include "stdio.h"
#include <ctype.h>
#include <sys/errno.h>
#include <pwd.h>
#include "jllib.h"
#include "commonhd.h"
#include "config.h"
#include "buffer.h"
#include "rk_spclval.h"
#include "sdefine.h"
#include "sheader.h"

#ifdef BSD42
#include <strings.h>
#else
#include <string.h>
#endif

extern char     *getenv();
extern FILE	*fopen();
extern int conv_keyin();
extern int keyin0();


getfname(file,path)
char *path , *file;
{
    char *c;

    c = path + strlen(path);
    for(;*c != '/' && c >= path ; c--);
    strcpy(file , c + 1);
}



 /**	s1とs2がマッチしたら非0を返す。s1にはワイルドカード '*' を
	含んでよい（s2にはだめ）。*/
strmatch(s1, s2)
char	*s1, *s2;
{
	while(1){
		if(*s1 == '\0') return(*s2 == '\0');
		if(*s1 != '*'){
			if(*s1 != *s2) return(0);
			s1++; s2++;
			continue;
		}

		s1++;
		do{
			if(strmatch(s1, s2)) return(1);
		} while(*s2++ != '\0');
		return(0);
	}
}

 /**	KEYBOARDという環境変数が設定されていればそれが、又、いなければTERMが
	返る */
char	*get_kbd_env()
{
	char	*term;

	return(NULL != (term = getenv(WNN_KEYBOARD_ENV)) ? term :
							   getenv("TERM"));
}	

 /**	KEYBOARDという環境変数が設定されていればそれが、又、いなければTERMが、
	引き数とマッチするかどうかの検査（マッチしたら非0が返る）。convert_key
	の初期設定に使う。*/
lookWnnterm(s)
char	*s;
{
	char	*term;

	return(NULL == (term = get_kbd_env()) ? 0 : strmatch(s, term));
}

expand_expr(s)
 /**	~user、@HOME、@LIBDIRの展開(但し、文字列の先頭のみ)。できない時は-1が
	返り、その場合sの中身は着々とそのまんま。sの長さ＜256と仮定してる。*/
 /**	@USR (env名、logname)の展開(但し、先頭の一つだけ) */
char	*s;
{
	char	tmp[EXPAND_PATH_LENGTH];
	register char	*p, *s1;
	int	noerr, expandsuc;
	struct	passwd	*u;
	extern struct passwd *getpwnam();

	if(*s != '~' && *s != '@') return(0);
	if(strlen(s) >= EXPAND_PATH_LENGTH) return(-1);

	s1 = s;
#ifdef BSD42	
	if(NULL != (p = index(++s1, '/'))){
#else
	if(NULL != (p = strchr(++s1, '/'))){
#endif
		strcpy(tmp, p);
		*p = '\0';
	} else *tmp = '\0';
 /* ここまでは準備。s…先頭、s1…２文字目、p…最初の'/'のあったところ
    （ここで一旦切る）、tmp…それ以後のコピー。*/

	if(*s == '~'){
		if(*s1){
			noerr = expandsuc = 
			(NULL != (u = getpwnam(s1)) &&
			 strlen(p = u -> pw_dir) + strlen(tmp) <
			 EXPAND_PATH_LENGTH );

		} else {
			noerr = expandsuc =
			(NULL != (p = getenv("HOME")) &&
			 strlen(p) + strlen(tmp) < EXPAND_PATH_LENGTH);
		}

	} else { /* then, *s must be '@' */
		if(!strcmp(s1, "HOME")){
			noerr = expandsuc = 
			(NULL != (p = getenv("HOME")) &&
			 strlen(p) + strlen(tmp) < EXPAND_PATH_LENGTH);
		} else if(!strcmp(s1, "LIBDIR")){
			noerr = expandsuc = 
			(strlen(p= LIBDIR)+ strlen(tmp) < EXPAND_PATH_LENGTH);
		} else { /* @HOME, @LIBDIR igai ha kaenai */
			noerr = 1; expandsuc = 0;
		}
	}

	if(expandsuc) strcpy(s, p);

    if (noerr) {
	int len = strlen("@USR");
	p = tmp;
	for(; *p; p++){
	    if (!strncmp(p, "@USR", len)) {
		if (strlen(username) + strlen(p) + strlen(s)
			    - len < EXPAND_PATH_LENGTH) {
		    strncat(s, tmp, p - tmp);
		    strcat(s, username);
		    strcat(s, p + len);
		    return(0);
		} else {
		    return (-1);
		}
	    }
	}
    }
	strcat(s, tmp);
	return(noerr ? 0 : -1);
}



/** uumrc を見てのパラメータの設定 */
uumrc_set_entry(data,dn )
char *data;
int *dn;
{
    int num,d1;
    char code[256];
    char s[7][EXPAND_PATH_LENGTH];
    char tmp[1024];
    FILE *fp;
    char  *s0or1;

    num = sscanf(data,"%s %s %s %s %s %s %s %s",code,
    		 s[0],s[1],s[2],s[3],s[4],s[5],s[6]) ;
    if (num <= 0) { return(-1); }

    if(code[0] == ';'){return(0);
    }else
    if (strcmp(code, "include") == 0){
	if(expand_expr(s[0]) != 0){
	    fprintf(stderr , "Can't expand %s.\r\n", s[0]);
	}
	if((fp = fopen(s[0], "r")) != NULL){
	    while(fgets(tmp,1024,fp  ) != NULL){
		uumrc_set_entry(tmp,dn);
	    }   
	    fclose(fp);
	}else{
	    fprintf(stderr , "Can't open %s.\r\n", s[0]);
	}
    }else if(strcmp(code , "setuumkey") == 0){
	/* setuumkey --- this must be a file.*/
	if(!uumkey_defined_by_option && (num > 1)) {
	    if(expand_expr(s[0]) != 0){
		fprintf(stderr , "Can't expand %s.\r\n", s[0]);
	    }
	    strncpy(uumkey_name_in_uumrc, s[0], PATHNAMELEN);
	}
    }else if(strcmp(code , "setconvenv") == 0){
	/* setconvenv*/
	if(num > 2) {
	    if(!(strcmp(s[num - 2], "sticky"))) {
		kanakan_sticky = 1;
		num--;
	    }
	}
	if(num > 1) {
	    if (num == 2) s0or1 = s[0];
	    else {
		s0or1 = s[1];
		if (*jserver_name_in_uumrc == '\0')
		    strncpy(jserver_name_in_uumrc, s[0], PATHNAMELEN);
	    }
	    if(expand_expr(s0or1) != 0){
		fprintf(stderr , "Can't expand %s.\r\n", s0or1);
	    }
	    strncpy(envrc_name_in_uumrc, s0or1, PATHNAMELEN);
	}
    } else if(strcmp(code , "setkankanaenv") == 0){
	/* setkankanaenv *//* ADD KURI */
	if(num > 2) {
	    if(!(strcmp(s[num - 2], "sticky"))){
		kankana_sticky = 1;
		num--;
	    }
	}
	if(num > 1) {
	    if (num == 2) s0or1 = s[0];
	    else {
		s0or1 = s[1];
		if (kankana_jserver_name_in_uumrc[0] == '\0')
		    strncpy(kankana_jserver_name_in_uumrc, s[0], PATHNAMELEN);
	    }
	    if(expand_expr(s0or1) != 0){
		fprintf(stderr , "Can't expand %s.\r\n", s0or1);
	    }
	    strncpy(kankana_env_name_in_uumrc, s0or1, PATHNAMELEN);
	}
    } else if(strcmp(code , "setkanaromenv") == 0){
	/* setkankanaenv *//* ADD KURI */
	if(num > 2) {
	    if(!(strcmp(s[num - 2], "sticky"))){
		kanarom_sticky = 1;
		num--;
	    }
	}
	if(num > 1) {
	    if (num == 2) s0or1 = s[0];
	    else {
		s0or1 = s[1];
		if (kanarom_jserver_name_in_uumrc[0] == '\0')
		    strncpy(kanarom_jserver_name_in_uumrc, s[0], PATHNAMELEN);
	    }
	    if(expand_expr(s0or1) != 0){
		fprintf(stderr , "Can't expand %s.\r\n", s0or1);
	    }
	    strncpy(kanarom_env_name_in_uumrc, s0or1, PATHNAMELEN);
	}
    } else if(strcmp(code , "setrkfile") == 0){
	/* setrkfile */
	if(!rkfile_defined_by_option && (num > 1)){
	    if(expand_expr(s[0]) != 0){
		fprintf(stderr , "Can't expand %s.\r\n", s[0]);
	    }
	    strncpy(rkfile_name_in_uumrc, s[0], PATHNAMELEN);
	}
    } else if(strcmp(code , "setconvkey") == 0) {
	/* setconvkey --- pair of TERM name and used table name */
	if(!convkey_defined_by_option){
	    if (num == 2) s0or1 = s[0];
	     else if ((num > 2) && lookWnnterm(s[0])) s0or1 = s[1];
	     else goto NOMATCH;

	    if(expand_expr(s0or1) != 0){
		fprintf(stderr , "Can't expand %s.\r\n", s0or1);
	    }
	    strncpy(convkey_name_in_uumrc, s0or1, PATHNAMELEN);
NOMATCH:    ;
	}
    }else if(strcmp(code , "flow_control_on") == 0){
	/* setflowctrl */
	if(!(defined_by_option & OPT_FLOW_CTRL)) {
	    flow_control = 1;
	}
    }else if(strcmp(code , "flow_control_off") == 0){
	/* setflowctrl */
	if(!(defined_by_option & OPT_FLOW_CTRL)) {
	    flow_control = 0;
	}
    }else if(strcmp(code , "waking_up_in_henkan_mode") == 0){
	/* */
	if(!(defined_by_option & OPT_WAKING_UP_MODE)) {
	    henkan_off_flag = 0;
	}
    }else if(strcmp(code , "waking_up_no_henkan_mode") == 0){
	/* */
	if(!(defined_by_option & OPT_WAKING_UP_MODE)) {
	    henkan_off_flag = 1;
	}
	/* 暫定? */
    }else if(strcmp(code , "convkey_always_on") == 0){
	keyin_when_henkan_off = conv_keyin;
    }else if(strcmp(code , "convkey_not_always_on") == 0){
	keyin_when_henkan_off = keyin0;
	/* end of "暫定?" */
    }else if(strcmp(code,"simple_delete")==0){
    	excellent_delete = 0;
    }else if(strcmp(code,"excellent_delete")==0){
    	excellent_delete = 1;
    } else if(strcmp(code , "send_ascii_char") == 0){
	/* send ascii char or not */
	send_ascii_char = 1;
    }else if(strcmp(code , "not_send_ascii_char") == 0){
	send_ascii_char = 0;
    }else if (strcmp(code, "setmaxchg") == 0){
	/* setmaxchg ----- henkan kanou saidai mojisuu */
	if (change_ascii_to_int(s[0],&d1) != -1){
	    maxchg = (d1 <= 0) ? maxchg:d1;
	}
    } else if (strcmp(code, "setmaxbunsetsu") == 0){
      /* setmaxbunsetsu ---- kaiseki kanou saidai bunsetsu kosuu */
	if(num >= 2){
	    if (change_ascii_to_int(s[0],&d1) != -1){
		maxbunsetsu = (d1 <= 0) ? maxbunsetsu:d1;
	    }
	}
    } else if (strcmp(code, "setmaxichirankosu") == 0){
      /* setmaxichirankosuu --- jikouho ichiran hyouji kosuu */
	if(num >= 2){
	    if (change_ascii_to_int(s[0],&d1) != -1){
		max_ichiran_kosu = (d1 <= 0) ? max_ichiran_kosu:d1;
	    }
	}
    } else if (strcmp(code, "setmaxhistory") == 0){
      /* setmaxhistory --- set depth of history */
	if(num >= 2){
	    if (change_ascii_to_int(s[0],&d1) != -1){
		max_history = (d1 <= 0) ? max_history:d1+1;	
		/* 62/06/25 kuri ヒストリの数が一つ少ないよ */
	    }
	}
    }else if(strcmp(code, "remove_cs") == 0){
      remove_cs_from_termcap = 1;
    }else if(strcmp(code, "not_remove_cs") == 0){
      remove_cs_from_termcap = 0;
    } else if (strcmp(code, "setjishopath") == 0){
      /* setjishopath --- 辞書のあるディレクトリ */
	if(num >= 2){
	    if(expand_expr(s[0]) != 0){
		fprintf(stderr , "Can't expand %s.\r\n", s[0]);
	    }
	    chartowchar(s[0], jishopath);
	}
    } else if (strcmp(code, "sethindopath") == 0){
      /* sethindopath --- 頻度のあるディレクトリ */
	if(num >= 2){
	    if(expand_expr(s[0]) != 0){
		fprintf(stderr , "Can't expand %s.\r\n", s[0]);
	    }
	    chartowchar(s[0], hindopath);
	}
    } else if (strcmp(code, "setfuzokugopath") == 0){
	if(num >= 2){
	    if(expand_expr(s[0]) != 0){
		fprintf(stderr , "Can't expand %s.\r\n", s[0]);
	    }
	    chartowchar(s[0], fuzokugopath);
	}
/*
    } else if (strcmp(code, "dai_kugiri_str") == 0){
	if(num >= 2){
	    char tmp[MAXKUGIRI * 2];
	    read_kanji_str(tmp, s[0]);
	    chartowchar(tmp, kugiri_str);
	}else{
	    kugiri_str[0] = 0;
	}
*/
    }else if (strcmp(code, "touroku_comment") == 0){
	touroku_comment = 1;
    }else if (strcmp(code, "touroku_no_comment") == 0){
	touroku_comment = 0;
    }else{
/*      printf("Undefined entry \"%s\" in uumrc.\r\n",code); */
    }
    return(0);
}

change_ascii_to_int(st,dp)
char *st;
int *dp;
{
    register int total,flag;

    total = 0;
    flag = 0;
    while(*st != NULL){
	if (isdigit(*st)){
	    total = total * 10 + (*st - '0');
	} else if (*st == '+') {
	    if (flag != 0) { return(-1); }
	    flag = 1;
	} else if (*st == '-') {
	    if (flag != 0) { return(-1); }
	    flag = -1;
	} else { return(-1); }
	st++;
    }
    if (flag == 0){
	flag = 1;
    }
    *dp = total * flag;
    return(1);
}

always_onebyte(s)
unsigned char	*s;
/* ARGSUSED */
{
	return(1);
}


/* uumrc ファイルからデータを読みその設定を行う。*/
uumrc_get_entries()
{
 extern FILE *open_uumrc();
 FILE *fp;
 int dn; char data[1024];

 /* default setting */

 maxchg = MAXCHG;
 maxbunsetsu = MAXBUNSETSU;
 max_ichiran_kosu = MAX_ICHIRAN_KOSU;

 max_history = MAX_HISTORY + 1;

 keyin_when_henkan_off = keyin0;

 if((fp= open_uumrc())==NULL){	return -1; }
 dn = 0;
 while(fgets(data,1024,fp  ) != NULL){
   uumrc_set_entry(data,&dn);
 }
 fclose(fp);
 if (*kankana_jserver_name_in_uumrc == 0)
    strcpy(kankana_jserver_name_in_uumrc, jserver_name_in_uumrc);
 if (*kanarom_jserver_name_in_uumrc == 0)
    strcpy(kanarom_jserver_name_in_uumrc, jserver_name_in_uumrc);
 return(0);
}


/** uumrc ファイルのオープン */
FILE *
open_uumrc()
{
 FILE *fp;
 char *n, buf[1024];

 if(n = getenv(WNN_UUM_ENV)){
    if(fp = fopen(n, "r")){
	return fp;
    }else{
	goto error;
    }
 }
 if(n = getenv("HOME")){
	strcat(strcpy(buf,n),USR_UUMRC);
	if(fp = fopen(buf, "r")){
		return fp;
	}
 }
 if((fp = fopen(RCFILE, "r")) != NULL){
		return fp;
 }
 error:
 fprintf(stderr , "uumrc ファイルがありません。");
 fflush(stderr);
 return(0);
}


read_kanji_str(c , o)
register char *c , *o;
{
    for(;*o;c++){
	if(*o == '\\'){
	    if(*++o == '0'){
		o += 1;
		if(*o >= '0' && *o <= '7'){
		    *c = (*o++ - '0');
		}else continue;
		if(*o >= '0' && *o <= '7'){
		    *c *= 8;
		    *c |= (*o++ - '0');
		}else continue;
	    }else{
		*c = *o++;
	    }
	}else{
	    *c = *o++;
	}
    }
    *c = 0;
}
