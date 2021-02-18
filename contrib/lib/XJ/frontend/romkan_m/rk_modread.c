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
 *	Original Source: ./lib/Wnn/romkan/rk_modread.c
 *
 *	Modified by OMRON TATEISI ELECTRONICS CO., to handle XJp-library.
 *
 *	Author:	Seiji Kuwari	OMRON TATEISI ELECTRONICS CO.
 *				kuwa@frf.omron.junet
 *				uunet!nff.ncl.omron.co.jp!kuwa
 */				

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
/***********************************************************************
			rk_modread.c
						88. 6.16  �� ��

	�⡼�����ɽ���ɤ߹��ߤ�ô������ץ���ࡣ
***********************************************************************/

/*	Version 3.0
 */

#include "rk_header.h"
#include "rk_extvars.h"
#include "rk_mult.h"
#ifdef WNNDEFAULT
#  include "config.h"
 /* �ޥ���LIBDIR������ʤΤ�������ˡ�����ѥ�����ϡ��إå��ե������
    �������ѥ��ˡ�Wnn�Υ��󥯥롼�ɥե�����Τ��꤫�����ꤷ�Ƥ������ȡ�*/
#endif
#include <pwd.h>

#define Terminator 0 /* int�����naibu[]�ˤν��ߥ����� */

char	codeeval(), *getenv();
extern	struct	passwd	*getpwnam(), *getpwuid();
extern	char	*chrcat(), *strend(), *ename();
extern	void	romkan_clear();
char	*modhyopath;

struct	kwdpair {
	 /* ������ɤȤ�������ɽ�����б���Ϳ���빽¤�Ρ�����ɽ����
	    �����ʤ���Τ��Ф��Ƥ�0��Ϳ�����Ƥ��롣*/
	char	*name;
	int	code;
} modfn[] = {
	"defmode",	0,
	"if",		XY2INT(2, 0),
	"when",		XY2INT(2, 1),
	"path",		0,
	"search",	0,
	"on_dispmode",	XY2INT(5, 0),
	"off_dispmode", XY2INT(5, 1),
	"on_unchg",	XY2INT(6, 0),
	"off_unchg",	XY2INT(6, 1),
	NULL
}; /* ���򸫤� ����������ɽ�Ϥޤ�������Ĥ���Τ� */

struct	kwdpair modcond[] = {
	"not",	XY2INT(3, 0),
	"and",	XY2INT(3, 1),
	"or",	XY2INT(3, 2),
	"true", XY2INT(3, 3),
	"false",XY2INT(3, 4),
	"=",	XY2INT(3, 5),
	"!=",	XY2INT(3, 6),
	"<",	XY2INT(3, 7),
	">",	XY2INT(3, 8),
	NULL
};
int	condarg[] = {1, 2, 2, 0, 0, 2, 2, 2, 2}; /* ���Ƚ�Ǵؿ��ΰ����Ŀ� */

struct	kwdpair swstat[] = {
	"on",	0,
	"off",	0,
	NULL
};
 /* 1��2^24��ϥ⡼��̾��4��2^24��ϥ⡼��ɽ��ʸ����7��2^24��ϥ⡼�ɾ�����
    ��������ɽ���˻ȤäƤ��� */

 /** ������ɡ�if, and�ʤɡˤ������ʤ�Τ������å����������ֹ���֤� */
kwdsrc(hyou, wd)
struct	kwdpair *hyou;	/* �ɤΥ������ɽ��Ȥ��� */
char	*wd;		/* �����å�����륭����� */
{
	int	i;

	for(i = 0; hyou[i] . name != NULL; i++)
		if(!mystrcmp(hyou[i] . name, wd)) return(i);
	ERRMOD(9);
	 /*NOTREACHED*/
}

 /** �⡼��ɽ���ɤ߹��� */
readmode(modfname)
char	*modfname; /* �⡼��ɽ��̾ */
{
	char	buf[MDHMAX], *bufp;
#ifdef RKMODPATH
	char	*genv, *pathenv, *pathp;
#endif

	cur_rk->mcurread = buf; /* ���顼������ */

#ifdef RKMODPATH
	if(!fixednamep(modfname) &&
	   NULL != (pathenv = genv = getenv(RKMODPATH)) && *genv != '\0'){
	 /* PATH�˾��ʤ��Ȥ��ĤΥ������ѥ��������� */
		for(;;){
			 /* �������ѥ��γơ���������ΰ�ˤϡ�pathmeimem�����
			    ���Ƥ��롣��ǡ��������ѥ�����Ƭ�ˡ��⡼��ɽ�Τ���
			    �ǥ��쥯�ȥ�����ꤹ��Τǡ������ص��Τ���⤢��*/
			for(pathp = cur_rk->pathmeimem; *genv != ':' && *genv; genv++)
				*pathp++ = *genv;
			*pathp = '\0';

			if(*(strend(cur_rk->pathmeimem)) != KUGIRI) *pathp++ = KUGIRI;
			 /* path�ζ��ڤ��DG��MV�ˤǤ��äƤ�'/' */

			strcpy(pathp, modfname);
			if(NULL != (cur_rk->modefile = fopen(cur_rk->pathmeimem, "r"))){
				 /* Now Mode-hyo found */
				if(cur_rk->flags & RK_VERBOS)
					fprintf(stderr,
					"romkan: using Mode-hyo %s ...\r\n",
						pathmeimem);
				cur_rk->curdir = cur_rk->pathmeimem; /* ���λ����Ǥϥե�����̾
					���ߤ��������Ȥǥѥ�̾�����ˤʤ� */
				cur_rk->curfnm = ename(modfname);
				break;
			}

			if(*genv != ':'){ /* Mode-hyo not found */
				if(cur_rk->flags & RK_VERBOS){
					fprintf(stderr, "no %s in ",modfname);
					for(genv = pathenv; *genv; genv++){
						fputc((*genv == ':' ?
						      ' ' : *genv), stderr);
					}
					fprintf(stderr, ".\n");
				}
				ERMOPN(0);
			} else genv++; /* coutinues searching Mode-hyo */
		}
	} else
#endif
	{
		if(NULL == (cur_rk->modefile = fopen(modfname, "r"))) ERMOPN(0);
		if(cur_rk->flags & RK_VERBOS)
			fprintf(stderr, "romkan: using Mode-hyo %s ...\r\n",
				modfname);
		strcpy(cur_rk->pathmeimem, modfname);
	}

	 /* �������ѥ�����Ƭ�ˡ��⡼��ɽ�Τ���ǥ��쥯�ȥ�����ꤷ�Ƥ��롣*/
	*(ename(cur_rk->pathmeimem)) = '\0';
	modhyopath = *(cur_rk->pathmeiptr)++ = cur_rk->pathmeimem;
	*(cur_rk->pathmeiptr) = NULL;
	strtail(cur_rk->pathmeimem);
	*(cur_rk->pathareaorg = ++cur_rk->pathmeimem) = '\0';
	 /* pathareaorg�ϡ�pathmeimem_[]�Τ����⡼��ɽ�Τ���ǥ��쥯�ȥ�̾��
	    ��Ǽ�����Ĥ����ʬ����Ƭ��ؤ���*/

	while(bufp = buf, read1tm(&bufp, 0)) mod_evl(buf);
	fclose(cur_rk->modefile);
}

 /**	���ꤵ�줿�ʥ������ѥ��򸫤�ɬ�פΤʤ��˥ե�����̾���Ф��Ƥ���0��
	�֤������ߤΤȤ��� / ./ ../ �Τɤ줫�ǻϤޤ��ΤȤ��Ƥ��뤬�ʺ�Ԥ�
	���ǡˡ�Ŭ�����Ѥ��Ƥ褤��strchr(s,'/')!=NULL �Ȥ��������ɤ����� */
fixednamep(s)
char	*s;
{
	return(!strncmp("/",s,1)|| !strncmp("./",s,2)|| !strncmp("../",s,3));
}

 /**	�⡼��ɽ�ΰ줫���ޤ�ʥꥹ�ȡ��ե�����̾���⡼��ɽ��ʸ����ˤ�
	��᤹�롣�֤��ͤϡ�defmode,search�ڤ�path�λ�0������ʳ��ʤ�1��*/
mod_evl(s)
char	*s; /* �⡼��ɽ������ɽ������ؤΥݥ��� */
{
	char	md1[MDT1LN], *bgn, *end;
	int	num, retval = 1;

	if(*s != '('){
		if(*s != '"'){
			num = fnmsrc_tourk(s);
			*(cur_rk->naibu++) = XY2INT(4, num);
		} else {
			s++;
			if(*(end = strend(s)) != '"') ERRMOD(10);
			*end = '\0';
			num = dspnamsrc_tourk(s);
			*(cur_rk->naibu)++ = XY2INT(5, 0);
			*(cur_rk->naibu)++ = num;
		}
	} else {
		s++;
		scan1tm(&s, md1, 1);
		switch(num = kwdsrc(modfn, md1)){
			case 0: /* defmode */
				retval = 0;
				scan1tm(&s, md1, 1); /* modename */
				num = modsrc_tourk(md1, 0);
				if(scan1tm(&s, md1, 0) == 0){
				 /* ���on-off�ˤĤ��Ʋ���񤤤Ƥʤ�����
				    default��off */
					cur_rk->modesw[num] . moderng = 2;
					cur_rk->modesw[num] . curmode = 0;
					break;
				}

				if(*md1 == '('){
					char	tmp[MDT1LN], *s;
					unsigned int	i, j;

					s = md1 + 1;

					scan1tm(&s, tmp, 1);
					if(chk_get_int(tmp, &i, 0) != 0)
						ERRMOD(8);
					cur_rk->modesw[num] . moderng = i;
					scan1tm(&s, tmp, 1);
					if(chk_get_int(tmp, &j,
						  cur_rk->modesw[num] . moderng) != 0)
						ERRMOD(8);
					cur_rk->modesw[num] . curmode = j;
					if(
#ifdef ModeNotInt
					   cur_rk->modesw[num] . moderng != i ||
					   cur_rk->modesw[num] . curmode != j ||
#endif
					   i == 1 || (i != 0 && j >= i)){
					   	ERRMOD(8);
					}
					scan1tm(&s, tmp, 2);
				} else {
					switch(kwdsrc(swstat, md1)){
						case 0: cur_rk->modesw[num] . curmode
								= 1; break;
						case 1: cur_rk->modesw[num] . curmode
								= 0; break;
					}
					cur_rk->modesw[num] . moderng = 2;
				}
				scan1tm(&s, md1, 2); /* �����err */
				break;
			case 1: /* if */
			case 2: /* when */
				*(cur_rk->naibu)++ = modfn[num] . code;
				scan1tm(&s, md1, 1); /* condition */
				cond_evl(md1);
				while(scan1tm(&s, md1, 0)){
					if(mod_evl(md1) == 0) ERRMOD(17);
				}
				*(cur_rk->naibu)++ = Terminator;
				break;
			case 3: /* path */
				*(cur_rk->pathmeimem = cur_rk->pathareaorg) = '\0';
				*(cur_rk->pathmeiptr = cur_rk->pathmeiorg) = NULL;
			case 4: /* search */
				retval = 0;
				if(cur_rk->hyomeiptr != cur_rk->hyomeiorg) ERRMOD(11);
				 /* �������ѥ��λ���ϥե�����̾�νи����
				    ��Ԥ��ʤ���Фʤ�ʤ��Ȥ��Ƥ�����*/

				while(scan1tm(&s, md1, 0)){/* find pathname */
					pathsrc_tourk(md1);
				}
				break;
			case 5: /* on_dispmode */
			case 6: /* off_dispmode */
				*(cur_rk->naibu)++ = modfn[num] . code;
				scan1tm(&s, md1, 1); /* dispmode string */

				if(*(bgn = md1) != '"') ERRMOD(12);
				bgn++;
				if(*(end = strend(bgn)) != '"') ERRMOD(10);
				*end = '\0';
				*(cur_rk->naibu)++ = dspnamsrc_tourk(bgn);
				scan1tm(&s, md1, 2); /* �����err */
				break;
			case 7: /* on_unchg */
			case 8: /* off_unchg */
				*(cur_rk->naibu)++ = modfn[num] . code;
				scan1tm(&s, md1, 2); /* �����err */
				break;
		}

	}
	*(cur_rk->naibu) = Terminator;
	return(retval);
}

 /** ��Ｐ�ʥ⡼��̾ ����not,and�ʤɤμ��˰�Ĥ��� */
cond_evl(cod)
char	*cod; /* ��Ｐ������ɽ������ؤΥݥ��� */
{
	char	md1[MDT1LN];
	unsigned int	num;
	int	i; 

	if(is_digit(*cod) || *cod == '-'){
		*(cur_rk->naibu)++ = XY2INT(7, 0);
		if(0 != chk_get_int(cod, &num, 0)) ERRMOD(4);
		*(cur_rk->naibu)++ = num;
	} else if(*cod != '('){
		num = modsrc_tourk(cod, 1);
		*(cur_rk->naibu)++ = XY2INT(1, num);
	} else {
		cod++;
		scan1tm(&cod, md1, 1); /* not;and;or */
		num = kwdsrc(modcond, md1);
		*(cur_rk->naibu)++ = XY2INT(3, num);
		for(i = condarg[num]; i; i--){
			scan1tm(&cod, md1, 0);
			cond_evl(md1);
		}
		scan1tm(&cod, md1, 2);
	}
	*(cur_rk->naibu) = Terminator;
}

 /**	s�ǻ��ꤵ�줿�ե�����̾������Ͽ��õ�����ʤ������Ͽ��â��������Ͽ��
	�ɤ����Υ����å��ϸ�̩�ǤϤʤ������㤨�С�Ʊ���ե�����Ǥ⡢
	�ѥ�̾�դ���̵���ȤǤϡ�Ʊ���ȸ��ʤ��ˡ��ե�����̾������Ͽ���ɤ���
	�����å�����Τϡ���������Τ����Ʊ��ɽ���ɤ߹���Τ��ɤ�����
	�����ʤΤǡ�����ʳ��ˤ��̤˺������Ϥʤ���*/
fnmsrc_tourk(s)
char	*s;
{
	int	n;

	for(n = 0; cur_rk->hyomeiorg[n] != NULL; n++)
		if(!mystrcmp(cur_rk->hyomeiorg[n], s)) return(n);

	if(cur_rk->hyomeiorg + n != cur_rk->hyomeiptr) BUGreport(101);

	*(cur_rk->hyomeiptr)++ = cur_rk->hyomeimem;
	*(cur_rk->hyomeiptr) = NULL;
	mystrcpy(cur_rk->hyomeimem, s);
	if(!(cur_rk->hyoshu[n] = filnamchk(cur_rk->hyomeimem))) ERRMOD(3);
	strtail(cur_rk->hyomeimem);
	*++(cur_rk->hyomeimem) = '\0';
	return(n);
}

 /**	s�ǻ��ꤵ�줿�������ѥ�̾������Ͽ��õ�����ʤ������Ͽ��â����fnmsrc_
	tourk()Ʊ�͡�����Ͽ���ɤ����Υ����å��ϸ�̩�ǤϤʤ�������ʤ���*/
pathsrc_tourk(s)
char	*s;
{
	int	n;
	char	fnm_addsla[MDT1LN];

	mystrcpy(fnm_addsla, s);
	if( !(*fnm_addsla == '\0' || *(strend(fnm_addsla)) == KUGIRI))
		chrcat(fnm_addsla, KUGIRI);
	 /* �ѥ�̾��'/'�ǽ���äƤʤ���С�������ղä��롣*/

	for(n = 0; cur_rk->pathmeiorg[n] != NULL; n++)
		if(!strcmp(cur_rk->pathmeiorg[n], fnm_addsla)) return(n);

	if(cur_rk->pathmeiorg + n != cur_rk->pathmeiptr) BUGreport(104);

	*(cur_rk->pathmeiptr)++ = cur_rk->pathmeimem;
	*(cur_rk->pathmeiptr) = NULL;
	strcpy(cur_rk->pathmeimem, fnm_addsla);

	strtail(cur_rk->pathmeimem);

	*++(cur_rk->pathmeimem) = '\0';
	return(n);
}

 /** s�ǻ��ꤵ�줿�⡼��ɽ��ʸ���󤬴���Ͽ��õ�����ʤ������Ͽ */
dspnamsrc_tourk(s)
char	*s;
{
	int	n;

	for(n = 0; cur_rk->dspnambgn[n] != NULL; n++)
		if(!mystrcmp(cur_rk->dspnambgn[n], s)) return(n);

	if(cur_rk->dspnambgn + n != cur_rk->dspnamptr) BUGreport(103);

	*(cur_rk->dspnamptr)++ = cur_rk->dspcod;
	*(cur_rk->dspnamptr) = NULL;
	mystrcpy(cur_rk->dspcod, s);
	strtail(cur_rk->dspcod);
	*++(cur_rk->dspcod) = '\0';
	return(n);
}

 /**	��Ͽ����Ƥ���⡼��̾���椫�顢s�ǻ��ꤵ�줿�⡼��̾��õ����*np ��
	�⡼���ֹ椬���롣���Ĥ���ʤ��ȸ��⡼��̾����������롣���ξ��
	���ͤ�0��*/
modnam_src(s, np)
char	*s;
int	*np;
{
	for(*np = 0; cur_rk->modmeibgn[*np] != NULL; (*np)++ )
		if(!mystrcmp(cur_rk->modmeibgn[*np], s)) return(1);
	return(0);
}

 /**	s�ǻ��ꤵ�줿�⡼��̾��õ�����ʤ������Ͽ��â����flg����0�ʤ顢
	���Ĥ���ʤ���Х��顼 */
modsrc_tourk(s, flg)
char	*s;
int	flg;
{
	int	n;

	if(modnam_src(s, &n)) return(n);

	if(flg) ERRMOD(5);

	if(cur_rk->modmeibgn + n != cur_rk->modmeiptr) BUGreport(102);

	*(cur_rk->modmeiptr)++ = cur_rk->modmeimem;
	*(cur_rk->modmeiptr) = NULL;
	mystrcpy(cur_rk->modmeimem, s);
	if(!modnamchk(cur_rk->modmeimem)) ERRMOD(4);
	strtail(cur_rk->modmeimem);
	*++(cur_rk->modmeimem) = '\0';
	return(n);
}

 /** �ե����뤫���ʸ���ɤ�ʶ���ʸ�������Ф��ˡ��ɤ��ʸ����EOF�ʤ�0���֤� */
char	fspcpass()
{
	register int	c;

	while(EOF != (c = chkchar_getc(cur_rk->modefile)) && is_nulsp(c));
	return(c == EOF ? '\0' : c);
}

 /**	�⡼��ɽ�ˤ϶���ʸ���ʳ��Υ���ȥ���ʸ�������ǤϺ�����ʤ���Τ�
	���롣�����äƤ������ϥ����å����Ĥġ�getc��Ԥ���*/
chkchar_getc(f)
FILE	*f;
{
	register int	c;

	c = getc(f);
	if(is_cntrl(c) && !isspace(c)){
		sprintf(cur_rk->mcurread, "\\%03o", c);
		ERRMOD(16);
	}
	return(c);
}	

modehyo_getc()
{
	return(chkchar_getc(cur_rk->modefile));
}

modehyo_ungetc(c)
register int	c;
{
	return(ungetc(c, cur_rk->modefile));
}

 /**	soc��̾�Υ桼���Υ����󡦥ǥ��쥯�ȥ�̾��dest�����졢*dest�ˤ���
	������ؤ����롣â��soc������ʤ鼫ʬ�Υ����󡦥ǥ��쥯�ȥ�̾��
	NULL�ʤ鼫ʬ�Υۡ��ࡦ�ǥ��쥯�ȥ�̾��������ξ��⡢����������
	���⤷�ʤ������ͤϡ���������-1��getenv("HOME")���Ի�������-2�ˡ�*/
get_hmdir(dest, soc)
char	**dest, *soc;
{
	struct	passwd	*usr;
	char	*p;

	if(soc == NULL){
		if(NULL == (p = getenv("HOME"))) return(-2);
	} else {
		if(NULL == (usr = (*soc? getpwnam(soc) : getpwuid(getuid()))))
			return(-1);
		p = usr -> pw_dir;
	}
	strcpy(*dest, p);
	strtail(*dest);
	return(0);
}

 /**	�⡼��ɽ���б�ɽ��Ρ��ե�����̾����ʬ���ɤ߹��ߡ���Ƭ�� @ ���� ~ ��
	���ϡ��ü������Ԥ��������ϡ�����ɤ߹��ߡ�����ᤷ��ʸ������Ф���
	�ؿ��ȡ���̤�����륨�ꥢ�����ϤؤΥݥ��󥿡������ɤޤ��ʸ���������
	�ݥ��󥿡����ͤϡ����ｪλ��0��@HOME�ǥۡ��ࡦ�ǥ��쥯�ȥ꤬���ʤ���
	1��@�Τ��Ȥ��Ѥʤ�Τ��褿��2��~�Ǽ�ʬ�Υۡ��ࡦ�ǥ��쥯�ȥ꤬���ʤ�
	��3��~�Τ��Ȥ�¸�ߤ��ʤ��桼��̾���褿��4��*/
readfnm(readchar_func, unreadc_func, readstr_func, areap, lastcptr)
register int	(*readchar_func)(), (*unreadc_func)(), (*readstr_func)();
char	**areap;
int	*lastcptr;
{
	char	*head;
	register int	c;

	c = (*readchar_func)();
	if(c == '@'){ /* @HOME, @MODEDIR, @LIBDIR */
		*(*areap)++ = c;
		head = *areap;
		(*readstr_func)(areap, 1);

		if(mystrcmp("HOME", head) == 0){
			*areap = --head;
			if(get_hmdir(areap, (char *)NULL) != 0){
				*areap = head;
				return(1);
			}
		} else
		if(mystrcmp("MODEDIR", head) == 0){
			strcpy(*areap = --head, modhyopath);
			if(KUGIRI== *(*areap= strend(*areap))) **areap = '\0';
		} else
#ifdef WNNDEFAULT
		if(mystrcmp("LIBDIR", head) == 0){
			strcpy(*areap = --head, LIBDIR);
			strtail(*areap);
		} else
#endif
		{
			*areap = --head;
			return(2);
		}

	} else
	if(c == '~'){ /* ~user */
		int	err;
			
		*(*areap)++ = c;
		head = *areap;
		(*readstr_func)(areap, 1);

		mystrcpy(head, head);
		*areap = head - 1;
		if((err = get_hmdir(areap, (*head ? head : NULL)))!= 0){
			*areap = --head;
			return(err == -2 ? 3 : 4);
		}

	} else {
		(*unreadc_func)(c);
	}

	*lastcptr = (*readstr_func)(areap, 0);
	return(0);
}

 /**	�⡼��ɽ�����ʸ��ʬ���Ф���Ȥ򡢶��򡦳�̤Τɤ줫
	����EOF�����ޤ�³���롣flg & 01����0�ʤ顢'/'����Ƥ�
	����롣���ͤϡ������ɤޤ��ʸ����*/
rd_string(readfile, sptr, flg)
register FILE	*readfile;
char	**sptr;
int	flg;
{
	int	c;

	while(EOF != (c = chkchar_getc(readfile)) &&
	      !(is_nulsp(c) || c == '(' || c == ')') &&
	      !(flg & 01 && c == KUGIRI)){
		switch(c){
			case '\\': rd_bcksla(readfile, sptr); break;
			case '^': rd_ctrl(readfile, sptr); break;
			default: *(*sptr)++ = c;
		}
	}
	**sptr = '\0';
	return(ungetc(c, readfile));
}

rd_str_from_modefile(sptr, flg)
char	**sptr;
int	flg;
{
	return(rd_string(cur_rk->modefile, sptr, flg));
}


 /**	�⡼��ɽ����Хå�����å�������ΰ�ʸ��ʬ����Ф���'\��8�ʡ�;'
	�η���ľ����â������Ƭ��'\\'�ϴ����ɤޤ줿���ȡ�*/
rd_bcksla(readfile, sptr)
register FILE	*readfile;
char	**sptr;
{
	int	c, code = 0, digflg = 0;

	switch(c = chkchar_getc(readfile)){
		case 'n':
			code = '\n'; digflg = 1; break;
		case 't':
			code = '\t'; digflg = 1; break;
		case 'b':
			code = '\b'; digflg = 1; break;
		case 'r':
			code = '\r'; digflg = 1; break;
		case 'f':
			code = '\f'; digflg = 1; break;
		case 'e':
		case 'E':
			code = ESCCHR; digflg = 1; break;
		case 'o':
			while(c = chkchar_getc(readfile), is_octal(c)){
				code <<= 3;
				code += ctov(c);
				digflg = 1;
			}
			if(c != ';') ungetc(c, readfile);
			break;
		case 'd':
			while(c = chkchar_getc(readfile), is_digit(c)){
				code *= 10;
				code += ctov(c);
				digflg = 1;
			}
			if(c != ';') ungetc(c, readfile);
			break;
		case 'x':
			while(c = chkchar_getc(readfile), is_xdigit(c)){
				code <<= 4;
				code += ctov(c);
				digflg = 1;
			}
			if(c != ';') ungetc(c, readfile);
			break;
		default:
			if(is_octal(c)){
				digflg = 1;
				code = ctov(c);
				while(c= chkchar_getc(readfile), is_octal(c)){
					code <<= 3;
					code += ctov(c);
				}
				if(c != ';') ungetc(c, readfile);
			} else {
				code = c;
				digflg = 1;
			}
	}

	if(digflg == 0) ERRMOD(7);
	sprintf(*sptr, "\\%o;", code);
	strtail(*sptr);
}

 /**	�⡼��ɽ���饳��ȥ��륳���ɷ����ΰ�ʸ��ʬ����Ф���
	'\��8�ʡ�;' �η���ľ����â������Ƭ��'^'�ϴ����ɤޤ줿���ȡ�*/
rd_ctrl(readfile, sptr)
register FILE	*readfile;
char	**sptr;
{
	int	c;

	if(!(' ' <= (c = chkchar_getc(readfile)) && c < '\177')) ERRMOD(7);
	if(c == '?') c = '\177'; else c &= 0x1f;

	sprintf(*sptr, "\\%o;", c);
	strtail(*sptr);
}

 /**	�⡼��ɽ�ΰ줫���ޤ�ʥꥹ�ȡ��ե�����̾���⡼��ɽ��ʸ����ˤ�
	�ڤ�Ф������κݡ��ü��ɽ����'^','\'�ˤ��ˤϡ�'\��8�ʡ�;' ��
	����ľ����flg����0�ʤ顢EOF�ǥ��顼�򵯤�����')'��0���֤���*/
read1tm(sptr, flg)
char	**sptr; /* �⡼��ɽ������ɽ������ؤΥݥ��󥿤ؤΥݥ��󥿡�
		   rd_bcksla()��rd_ctrl()��codeeval()�Ǥ�Ʊ�� */
int	flg;
{
	int	c, err, retval = 1;
	char	*s;

	s = *sptr;

	while((c = fspcpass()) == ';'){
	  /* ���ʸ�򸡽Ф����顢�����ޤǤȤФ��ƺƻ�ԡ�*/
		while((c = chkchar_getc(cur_rk->modefile)) != '\n' && c != EOF);
	}

	switch(c){
		case '\0': /* EOF��ɽ�� */
			if(flg) ERRMOD(0);
			 else retval = 0;
			break;
		case ')':
			if(flg) retval = 0;
			 else ERRMOD(1);
			break;
		case '(':
			*s++ = c;
			*s++ = ' ';
			while(read1tm(&s, 1)) *s++ = ' ';
			*s++ = ')';
			break;
		case '"':
			*s++ = c;
			while((c = chkchar_getc(cur_rk->modefile)) != '"'){
				switch(c){
					case EOF : ERRMOD(0);
					case '\\': rd_bcksla(cur_rk->modefile, &s);
						   break;
					case '^' : rd_ctrl(cur_rk->modefile, &s);
						   break;
					default	 : *s++ = c;
				}
			}
			*s++ = '"';
			break;
		default:
			ungetc(c, cur_rk->modefile);
		 /* ��Ƭ�� @ ���� ~ �λ��ϡ��ü������*/
			err = readfnm(modehyo_getc, modehyo_ungetc,
				      rd_str_from_modefile, &s, &c);
			if(err){
				cur_rk->mcurread = s;
				switch(err){
					case 1:
					case 3: ERRMOD(13);
					case 2: ERRMOD(14);
					case 4: ERRMOD(15);
				}
			}

			if(c == EOF && flg) ERRMOD(0);
			if(c == ')' && !flg) ERRMOD(1);
	}

	*s = '\0';
	*sptr = s;
	return(retval);
}

 /**	8��10��16�ʥ������ѤΥ���饯����ºݤΥ����ɤ�ľ�������ϤΥ����å���
	���ʤ���*/
ctov(c)
char	c;
{
	if(is_upper(c)) return(c - 'A' + 10);
	if(is_lower(c)) return(c - 'a' + 10);
	return(c - '0');
}

 /**	�ꥹ�Ȥ���Ȥ�scan�����ѡ�')'��0���֤���EOL����ʤ��Ϥ���
	flg == 1 �ΤȤ������Ф��˼��Ԥ����饨�顼��
	flg == 2 �ΤȤ������Ф������������饨�顼��
	�ü�ʥ�����ɽ���ϴ������� '\��8�ʡ�;' �η���ľ�äƤ���Ȧ��*/
scan1tm(socp, dest, flg)
char	**socp, *dest;
	 /* socp�λؤ��Ƥ���ݥ��󥿤��ؤ��Ƥ���꤫����Ф���dest������롣
	    ���θ塢socp���ؤ��Ƥ���ݥ��󥿤�ʤ�롣*/
int	flg;
{
	char	c;
	int	retval = 1;

	while(c = *(*socp)++, is_nulsp(c)) if(c == '\0') ERRMOD(6);
	switch(c){
		case ')':
			retval = 0;
			break;
		case '(':
			*dest++ = c;
			*dest++ = ' ';
			while(scan1tm(socp, dest, 0)){
				strtail(dest);
				*dest++ = ' ';
			}
			*dest++ = ')';
			break;
		case '"':
			*dest++ = c;
			while((c = *dest++ = *(*socp)++) != '"'){
				if(c == '\\'){ /* '\��8�ʡ�;'�β�� */
					while(c = *dest++ = *(*socp)++,
					      is_octal(c));
				}
			}
			break;
		default:
			*dest++ = c;
			while(!is_nulsp(**socp)) *dest++ = *(*socp)++;
	}

	*dest = '\0';
	if(flg == 1 && retval == 0 || flg == 2 && retval == 1) ERRMOD(6);
	return(retval);
}

 /** �⡼��̾�Ȥ��������������å����ѿ�������ʤäƤ���Ф��� */
modnamchk(s)
char	*s;
{
	if(is_digit(*s)) return(0);
	for(; *s; s++) if(!is_alnum(*s) && *s != '_') return(0);
	return(1);
}

#define modu1(a, b) ((b) ? ((a) % (b)) : (a))
#define curmod(num) (cur_rk->modesw[num] . curmode)
#define modrng(num) (cur_rk->modesw[num] . moderng)

 /**	num���ܤΥ⡼�ɤ�����󥸤����Ѵ�ɽ������ľ�������� mode ���ͤ�0�ʤ�
	�⡼�ɤ�off��1�ʤ�on���뤳�Ȥˤʤ롣�ʤ�����mode���ͤ��֤���*/
modetyp	chgmod(num, mode)
int	num;
modetyp mode;
{
	modetyp oldmod;

	oldmod = curmod(num);
	curmod(num) = modu1(mode, modrng(num));
	choosehyo();
	return(oldmod);
}

 /** ���⡼�ɤ�ޤȤ���ڤ��ؤ��� */
allchgmod(mode)
modetyp	mode;
{
	int	i;

	for(i = 0; cur_rk->modmeibgn[i] != NULL; i++){
		curmod(i) = modu1(mode, modrng(i));
	}
	choosehyo();
}

 /**	num���ܤΥ⡼�ɤ���ꤷ�����������󥯥���Ȥ�����mode���ͤ��֤���*/
modetyp	incmod(num, dmode)
int	num;
modetyp	dmode;
{
	modetyp	oldmod, newmod;

	newmod = oldmod = curmod(num);
	newmod += dmode;
	if(oldmod > newmod) newmod -= modrng(num);
	return(chgmod(num, newmod));
}

 /**	num���ܤΥ⡼�ɤ���ꤷ���������ǥ�����Ȥ�����mode���ͤ��֤����Թ�
	�ˤ�ꡢincmod�Ȥ��̤��Ѱդ��ʤ��ƤϤʤ�ʤ���*/
modetyp	decmod(num, dmode)
int	num;
modetyp	dmode;
{
	modetyp	oldmod, newmod;

	newmod = oldmod = curmod(num);
	newmod -= dmode;
	if(oldmod < newmod) newmod += modrng(num);
	return(chgmod(num, newmod));
}

 /**	name��̾�Υ⡼�ɤ��ʤ������0���֤�������Ф��Υ⡼���ֹ桦�ڤӤ���
	���֤κ����͡ܣ��ȸ��ߤξ��֤��äƤ��� */
romkan_getmode(name, nump, modep, moderngp)
char	*name;
int	*nump;
modetyp	*modep, *moderngp;
{
	if(!modnam_src(name, nump)) return(-1);
	*modep = curmod(*nump);
	*moderngp = modrng(*nump);
	return(0);
}

 /**	name��̾�Υ⡼�ɤ��ʤ������0���֤�������Ф��ξ��֤򥻥åȤ���
	�Ѵ�ɽ�������θ塢����֤�������0���֤���*/
romkan_setmode(name, modep)
char	*name;
modetyp	*modep;
{
	modetyp	oldmode, moderng;
	int	modenum;

	if(romkan_getmode(name, &modenum, &oldmode, &moderng)!= 0) return(-1);
	chgmod(modenum, *modep);
	*modep = oldmode;
	return(0);
}

 /** �Ѵ�ɽ�Υ��ꥢ */
romkan_reset()
{
	cur_rk->naibu_[0] = Terminator;
	choosehyo();
	romkan_clear();
}

 /** �Ѵ��б�ɽ�������Ԥ� */
choosehyo()
{
	int	*naibup, i;

	naibup = cur_rk->naibu_;
	cur_rk->usemaehyo[0] = cur_rk->usehyo[0] = cur_rk->useatohyo[0] = -1;
	for(i = 0; i < 2; i++){
		cur_rk->dspmod[1][i] = cur_rk->dspmod[0][i];
		cur_rk->dspmod[0][i] = NULL;
	}

	look_choose(&naibup, 1);
}

 /**	�⡼��ɽ������������缡���Ƥ���������ɽ������ڤӥ⡼��ɽ��ʸ�����
	�����ԤäƤ�����â��flg��0�ʤ饹���åפ������ */
look_choose(naibupp, flg)
int	**naibupp;   /* �⡼��ɽ������ɽ������ؤΥݥ��󥿤ؤΥݥ��󥿡�
			look_cond()��evlcond()�Ǥ�Ʊ�� */
int	flg;
{
	int	*naibup, naibu1, naibu2, branch, lcrsl;

	naibup = *naibupp;

	while((naibu1 = *naibup++) != Terminator){
		switch(SHUBET(naibu1)){
			case 4: /* ɽ̾ */
				if(flg) hyouse(LWRMSK(naibu1));
				break;
			case 2: /* ��Ｐ */
				branch = LWRMSK(naibu1); /* if;when */
				lcrsl = look_cond(&naibup, flg);
				if(branch == 0 && lcrsl) flg = 0;
				break;
			case 5: /* romkan��on��off�����줾���
				   �⡼��ɽ��ʸ���� */
				naibu2 = *naibup++;
				if(flg) cur_rk->dspmod[0][LWRMSK(naibu1)] =
							    cur_rk->dspnambgn[naibu2];
				break;
			case 6: /* romkan�����줾��on��off���Υ⡼��ɽ��
				   ʸ��������Τޤޤ� */
				if(flg) cur_rk->dspmod[0][LWRMSK(naibu1)] = 
						    cur_rk->dspmod[1][LWRMSK(naibu1)];
				break;
			default:
				BUGreport(6);
		}
	}

	*naibupp = naibup;
}

 /**	*naibupp ��������ɽ������Ǿ�Ｐ��ɽ���Ȥ����ؤ��Ƥ���Ȧ�ʤΤǡ�
	�����ɾ���������ʤ餽��³�����ᤷ�ˤ��������ʤ��ɤ����Ф���
	���ͤϡ��ǽ��ɾ��������Ｐ�ο����͡�*/
look_cond(naibupp, flg)
int	**naibupp, flg;
{
	int	*naibup, condrsl;

	naibup = *naibupp;

	condrsl = evlcond(&naibup); /* ɬ��ɾ�����ʤ��Ȥ����ʤ����� */
	flg = flg && condrsl;
	look_choose(&naibup, flg);

	*naibupp = naibup;
	return(flg);
}

 /** ��Ｐ�ο����ͤ�ɾ��  ���ͤ�0��1�Ȥϸ¤�� */
evlcond(naibupp)
int	**naibupp;
{
	int	*naibup, naibu1, retval, tmpval[ARGMAX], i, imax;

	naibup = *naibupp;

	naibu1 = *naibup++;
	switch(SHUBET(naibu1)){
		case 7: /* ���� */
			retval = *naibup++; break;
		case 1: /* �⡼��̾ */
			retval = cur_rk->modesw[LWRMSK(naibu1)] . curmode; break;
		case 3: /* and�ʤ� */
			imax =	condarg[LWRMSK(naibu1)];
			for(i = 0; i < imax; i++)
				tmpval[i] = evlcond(&naibup);
			switch(LWRMSK(naibu1)){
			 /* �夫����true,false,not,and,or */
				case 0: retval = !tmpval[0]; break;
				case 1: retval = tmpval[0]&& tmpval[1]; break;
				case 2: retval = tmpval[0]|| tmpval[1]; break;
				case 3: retval = 1; break;
				case 4: retval = 0; break;
				case 5: retval = (tmpval[0] == tmpval[1]);
					break;
				case 6: retval = (tmpval[0] != tmpval[1]);
					break;
				case 7: retval = ((unsigned int)tmpval[0] <
						  (unsigned int)tmpval[1]);
					break;
				case 8: retval = ((unsigned int)tmpval[0] >
						  (unsigned int)tmpval[1]);
					break;
			}
			break;
	}

	*naibupp = naibup;
	return(retval);
}

 /** num���ܤ�ɽ�򡢻��Ѥ����ΤȤ�����Ͽ���롣�����ܡ�������ζ��̤⤹�� */
hyouse(num)
int	num;
{
	int	*ptr;

	switch(cur_rk->hyoshu[num]){
		case 1: ptr = cur_rk->usemaehyo; break;
		case 2: ptr = cur_rk->usehyo; break;
		case 3: ptr = cur_rk->useatohyo; break;
		default: BUGreport(11);
	}
	for(; *ptr != -1; ptr++) if(*ptr == num) return;
	*ptr = num;
	*++ptr = -1;
}

 /** strcmp��Ʊ��  â����'\��8�ʡ�;'���᤹�롣*/
mystrcmp(s1, s2)
char	*s1, *s2;
{
	char	c1, c2;

	while((c1 = codeeval(&s1)) == (c2 = codeeval(&s2)))
		if(c1 == '\0') return(0);
	return(c1 > c2 ? 1 : -1);
}

 /** strcpy��Ʊ�� â��'\��8�ʡ�;'���᤹�롣s1 <= s2�ʤ�����ư���Ϥ� */
mystrcpy(s1, s2)
char	*s1, *s2;
{
	while(*s1++ = codeeval(&s2));
}

 /**	��ʸ���β���Ԥ������̤�ʸ���Ϥ��Τޤޡ�'\��8�ʡ�;'�ϼºݤΥ����ɤ�
	ľ�������θ塢ʸ����ؤΥݥ��󥿤��ʸ��ʬ�ʤ�Ƥ����ʾ��ʤ��Ȥ�
	���Х���ʬ�ʤळ�Ȥ��ݾڤ����Ϥ��ˡ�*/
char	codeeval(sptr)
register char	**sptr;
{
	register char	c;
	char	code = 0;

	if((c = *(*sptr)++) != '\\') return(c);
	while(c = *(*sptr)++, is_octal(c)){
		code <<= 3;
		code += ctov(c);
	}
	if(c != ';') BUGreport(12);
	return(code);
}

 /** romkan��on���Υ⡼��ɽ��ʸ������֤��ؿ���̵����Ǥ��ä�RK_DSPNIL�ե饰��
     Ω�äƤ�����϶�ʸ������֤���*/
char	*romkan_dispmode()
{
	return(cur_rk->dspmod[0][0] == NULL && (cur_rk->flags & RK_DSPNIL) ?
	       cur_rk->nulstr : cur_rk->dspmod[0][0]);
}

 /** romkan��off���Υ⡼��ɽ��ʸ������֤��ؿ���̵����Ǥ��ä�RK_DSPNIL�ե饰
     ��Ω�äƤ�����϶�ʸ������֤���*/
char	*romkan_offmode()
{
	return(cur_rk->dspmod[0][1] == NULL && (cur_rk->flags & RK_DSPNIL) ?
	       cur_rk->nulstr : cur_rk->dspmod[0][1]);
}

 /** ʸ����10�������ʤ餽�β��򤷡������Ǥʤ������0���֤� */
chk_get_int(p, ip, range)
char	*p;
unsigned int	*ip;
modetyp range;
{
	int	sgn = 1;
	modetyp	out;

	if(*p == '-'){
		p++;
		sgn = -1;
	}
	for(out = 0; *p; p++){
		if(!is_digit(*p)) return(-1);
		out *= 10;
		out += ctov(*p);
	}
	if(range != 0) out %= range;
	if(sgn == -1 && out != 0) out = range - out;
	*ip = out;
	return(0);
}
