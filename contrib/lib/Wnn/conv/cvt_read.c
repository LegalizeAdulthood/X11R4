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
			convert_read.c			 ������
						88/06/19�������
							 ������
	�ե��󥯥���󥭡�����С��Ƚ����Ρ��Ѵ�ɽ�꡼����
	convert_key.c ����ʬ�ʡ����ꤷ���Ѵ�ɽ�ڤ�termcap��
	terminfo������ɤߤ��ߤ�Ԥ��ؿ���������Ƥ��롣
***********************************************************************/

/*	Version 4.0
 */

#include <stdio.h>
#include <ctype.h>
#include "cvt_head.h"

#ifdef BSD42
#include <strings.h>
#endif

#ifdef SYSVR2
#include <string.h>
#include <fcntl.h>
#include <curses.h>
#include <term.h>
#endif

#define ENTRY_LEN 10 /* ����С���ɽ�Υ���ȥ�̾�κ���Ĺ */

/*
#define	NOFILE_NOT_ABORT
*/
 /* �����define���Ƥ����ȡ��Ѵ�ɽ��open�Ǥ��ʤ����˥��顼��λ������
    ñ���Ѵ��ؿ��������ؿ��ˤʤ롣*/

#define is_digit(x) (isascii(x) && isdigit(x))
#define is_xdigit(x) (isascii(x) && isxdigit(x))
#define is_upper(x) (isascii(x) && isupper(x))
#define is_lower(x) (isascii(x) && islower(x))
#define is_space(x) (isascii(x) && isspace(x))

#define is_eofnl(x) ((x) == '\n'|| (x) == EOF)
#define is_eofsp(x) (is_space(x) || (x) == EOF)
#define is_octal(x) (is_digit(x) && (x) < '8')

#define ESCCHR	'\033'

 /*	convert_key ���Ѵ�ɽ�Υե�����̾�Υǥե���Ȥ򥻥åȡ�
	�Ѵ�ɽ��̾������Ū�˻��ꤵ�줿�顢���줬�����ѿ����ͤˤʤ롣*/
static	char	*convert_filename = CONVERT_FILENAME;

static	FILE	*convert_hyo;


 /** ���顼��å�������ɽ�� */
#define errdsp1(mes, conv_fnm)						\
	fprintf(stderr, "%s: %s.\r\n%s = %s\r\n\n",			\
		"convert_key", mes, "convert-table filename", conv_fnm)

#define errdsp2(mes, termnm)						\
	fprintf(stderr, "%s: %s.\r\n%s = %s\r\n\n",			\
		"convert_key", mes, "termname", termnm)

 /* Warning��å�������ɽ�� */
#define warn1(conv_fnm)							\
	fprintf(stderr, "%s: %s.\r\n%s.\r\n%s = %s\r\n\n",		\
		"convert_key",						\
		"Warning! Convert-table file is not found",		\
		"Key conversion will not be performed",			\
		"convert-table filename", conv_fnm)

#define warn2(name, conv_fnm)						\
	fprintf(stderr, "%s: %s \"%s\" %s.\r\n%s.\r\n%s = %s\r\n\n",	\
		"convert_key",						\
		"Warning! Entry name", name, "is duplicated",		\
		"The previous one was ignored",				\
		"convert-table filename", conv_fnm)

#define warn3(name, conv_fnm)						\
	fprintf(stderr, "%s: %s \"%s\" %s.\r\n%s = %s\r\n\n",		\
		"convert_key",						\
		"Warning! Illegal entry name", name, "was ignored",	\
		"convert-table filename", conv_fnm)

#define warn4(entry, conv_fnm, code) {					\
	fprintf(stderr, "%s: %s \"%s\" %s,\r\n%s ",			\
		"convert_key", "Warning! Entry name", entry,		\
		"isn't described in TERMCAP/INFO",			\
		"so the corresponding key can't generate");		\
	printcode(stderr, code);					\
	fprintf(stderr, ".\r\n%s = %s\r\n\n",				\
		"convert-table filename", conv_fnm);			\
}						

 /** ������c���ܤ˸�������ǰ��� */
printcode(f, c)
FILE	*f;
int	c;
{
	fprintf(f, (' ' <= c && c < '\177') ? "'%c'" :
		   ((c & ~0xff) ? "0x%X" : "'\\%03o'"), c);
}

/*
getterm_fkeydata(pterp)
char	**pterp;
{�ѻ�}
*/

 /**	8��10��16�ʥ������ѤΥ���饯����ºݤΥ����ɤ�ľ�������ϤΥ����å���
	���ʤ������Ϥϱѿ����Ȳ��ꤷ�Ƥ���ˡ�*/
charvalue(c)
char	c;
{
	if(isupper(c)) return(c - 'A' + 10);
	if(islower(c)) return(c - 'a' + 10);
	return(c - '0');
}


#ifdef BSD42
char	*cvttbl_entry[] = {
	"k0", "k1", "k2", "k3", "k4", "k5", "k6", "k7", "k8", "k9",
# ifdef	SX9100BSD
	"k;",
	"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA",
	"FB", "FC", "FD", "FE", "FF", "FG", "FH", "FI", "FJ", "FK",
	"FL",
# endif	SX9100BSD
	"kb", "kh", "ku", "kr", "kl", "kd",
	"!del", /* ���� */
};
#endif
#ifdef SYSVR2
char	*cvttbl_entry[] = {
	"kf0", "kf1", "kf2", "kf3", "kf4", "kf5", "kf6", "kf7", 
	"kf8", "kf9", "kf10",
#  ifdef SX8870
	"kf11", "kf12", "kf13", "kf14", "kf15", "kf16", "kf17", 
	"kf18", "kf19", "kf20", "kf21", "kf22", "kf23", "kf24",
	"kf25", "kf26", "kf27", "kf28", "kf29", "kf30", "kf31",
#  endif
	"kbs", "ktbc", "kclr", "kctab", "kdch1", "kdl1", "kcud1",
	"krmir", "kel", "ked", "khome", "kich1", "kil1", "kcub1",
	"kll", "knp", "kpp", "kcuf1", "kind", "kri", "khts", "kcuu1",
};
#endif

#define ENTRY_CNT (sizeof(cvttbl_entry) / sizeof(*cvttbl_entry))
#define CVTTBLSIZ ENTRY_CNT

char	*keydef_key[ENTRY_CNT]; /* terminfo/termcap�γƥǡ����ؤΥݥ��� */
char	tdataarea[AREASIZE];	/* �ǡ�����Ǽ�Τ�������ѥ��ꥢ */

struct	CONVCODE tbl[CVTTBLSIZ];
int	cnv_tbl_cnt;	/* convert table count */
 /* �����ϸ��� convert_key.c���static�ѿ����ä� */

#ifdef BSD42
 /**	tgetstr��Ʊ���δؿ���â��������ȥ�̾��!del�ס�DEL�������Ѵ��򤹤뤿��
	����Ū���ɲä��Ƥ��륨��ȥ�ˤ��Ф��Ƥϡ���ʸ����"\177"���֤���*/
char	*my_tgetstr(name, ptr)
char	*name, **ptr;
{
	extern	char	*tgetstr();	
	static	char	*del = "\177";

	if(strcmp(name, "!del") == 0) return(del);
	return(tgetstr(name, ptr));
}
#endif

#ifdef SYSVR2
 /**	pp1�λؤ��Ƥ��륨�ꥢ��p2�����Ƥ����줿�塢pp1�Υݥ��󥿤�ʤ�Ƥ���*/
char	*stradd(pp1, p2)
char	**pp1, *p2;
{
	if(p2 != NULL){
		strcpy(*pp1, p2);
		(*pp1) += strlen(p2 = *pp1) + 1;
	}
	return(p2);
}
#endif

 /** termcap/info�����ɤ�������Υǡ��������ѥ��ꥢ�˥��ԡ���*/
convert_getstrs(flg)
int	flg; /* verbose�⡼�ɤǵ��������Υե饰�ʺ��νꤽ���������������
		��ĥ�����뤫�⤷��ʤ��Τǡ�1��0��Ϳ���Ʋ����������Υե�����
		��Ρ�̾����convert_�ǻϤޤ�¾�δؿ��Ǥ�Ʊ���Ǥ���*/
{
	char	*tdataptr;
	int	i;

	tdataptr = tdataarea;

#ifdef BSD42
	for(i = 0; i < ENTRY_CNT; i++)
		keydef_key[i] = my_tgetstr(cvttbl_entry[i], &tdataptr);
	/* keydef_key[i] ���ͤϡ�NULL����tdataarea���ϰ���Ȥϸ¤�ʤ���
	   "\177"�ʤɤ��ü�ǡ�����ؤ����Ȥ⤢�롣*/

	if(flg) fprintf(stderr, "convert_key: finished getting TERMCAP.\r\n");
#endif

#ifdef SYSVR2
	i = 0;
	keydef_key[i++] = stradd(&tdataptr, key_f0);
	keydef_key[i++] = stradd(&tdataptr, key_f1);
	keydef_key[i++] = stradd(&tdataptr, key_f2);
	keydef_key[i++] = stradd(&tdataptr, key_f3);
	keydef_key[i++] = stradd(&tdataptr, key_f4);
	keydef_key[i++] = stradd(&tdataptr, key_f5);
	keydef_key[i++] = stradd(&tdataptr, key_f6);
	keydef_key[i++] = stradd(&tdataptr, key_f7);
	keydef_key[i++] = stradd(&tdataptr, key_f8);
	keydef_key[i++] = stradd(&tdataptr, key_f9);
	keydef_key[i++] = stradd(&tdataptr, key_f10);
#ifdef SX8870
	keydef_key[i++] = stradd(&tdataptr, key_f11);
	keydef_key[i++] = stradd(&tdataptr, key_f12);
	keydef_key[i++] = stradd(&tdataptr, key_f13);
	keydef_key[i++] = stradd(&tdataptr, key_f14);
	keydef_key[i++] = stradd(&tdataptr, key_f15);
	keydef_key[i++] = stradd(&tdataptr, key_f16);
	keydef_key[i++] = stradd(&tdataptr, key_f17);
	keydef_key[i++] = stradd(&tdataptr, key_f18);
	keydef_key[i++] = stradd(&tdataptr, key_f19);
	keydef_key[i++] = stradd(&tdataptr, key_f20);
	keydef_key[i++] = stradd(&tdataptr, key_f21);
	keydef_key[i++] = stradd(&tdataptr, key_f22);
	keydef_key[i++] = stradd(&tdataptr, key_f23);
	keydef_key[i++] = stradd(&tdataptr, key_f24);
	keydef_key[i++] = stradd(&tdataptr, key_f25);
	keydef_key[i++] = stradd(&tdataptr, key_f26);
	keydef_key[i++] = stradd(&tdataptr, key_f27);
	keydef_key[i++] = stradd(&tdataptr, key_f28);
	keydef_key[i++] = stradd(&tdataptr, key_f29);
	keydef_key[i++] = stradd(&tdataptr, key_f30);
	keydef_key[i++] = stradd(&tdataptr, key_f31);
#endif /* SX8870 */
/* �ʲ����
	"kbs", "ktbc", "kclr", "kctab", "kdch1", "kdl1", "kcud1",
	"krmir", "kel", "ked", "khome", "kich1", "kil1", "kcub1",
	"kll", "knp", "kpp", "kcuf1", "kind", "kri", "khts", "kcuu1"
*/
	keydef_key[i++] = stradd(&tdataptr, key_backspace);
	keydef_key[i++] = stradd(&tdataptr, key_catab);
	keydef_key[i++] = stradd(&tdataptr, key_clear);
	keydef_key[i++] = stradd(&tdataptr, key_ctab);
	keydef_key[i++] = stradd(&tdataptr, key_dc);
	keydef_key[i++] = stradd(&tdataptr, key_dl);
	keydef_key[i++] = stradd(&tdataptr, key_down);
	keydef_key[i++] = stradd(&tdataptr, key_eic);
	keydef_key[i++] = stradd(&tdataptr, key_eol);
	keydef_key[i++] = stradd(&tdataptr, key_eos);
	keydef_key[i++] = stradd(&tdataptr, key_home);
	keydef_key[i++] = stradd(&tdataptr, key_ic);
	keydef_key[i++] = stradd(&tdataptr, key_il);
	keydef_key[i++] = stradd(&tdataptr, key_left);
	keydef_key[i++] = stradd(&tdataptr, key_ll);
	keydef_key[i++] = stradd(&tdataptr, key_npage);
	keydef_key[i++] = stradd(&tdataptr, key_ppage);
	keydef_key[i++] = stradd(&tdataptr, key_right);
	keydef_key[i++] = stradd(&tdataptr, key_sf);
	keydef_key[i++] = stradd(&tdataptr, key_sr);
	keydef_key[i++] = stradd(&tdataptr, key_stab);
	keydef_key[i++] = stradd(&tdataptr, key_up);

	if(flg) fprintf(stderr,"convert_key: finished getting TERMINFO.\r\n");
#endif
}

 /**	SYSVR2 �ξ��� setupterm()��BSD42�ξ��� tgetent()�ν�����Ԥä�
	�塢������ȯ�����륷�����󥹤Υǡ��������ѤΥ��ꥢ�˥��ԡ����롣
	termcap/info�Υǡ����١����������ץ�Ǥ��ʤ��ä�����0���֤���*/
#ifdef BSD42
convert_getterm(termname, flg)
char	*termname;
int	flg;
{
	extern	int	tgetent();
	char	tcaparea[AREASIZE];

	if(flg) fprintf(stderr, "convert_key: using TERMCAP entry %s...\r\n",
			termname);
	if(tgetent(tcaparea, termname) <= 0){
		errdsp2("Can't get termcap entry", termname);
		return(1);
	}
	convert_getstrs(flg);
	return(0);
}
#endif
#ifdef SYSVR2
convert_getterm(termname, flg)
char	*termname;
int	flg;
{
	int	fd, rsl;

	if(flg) fprintf(stderr, "convert_key: using TERMINFO entry %s...\r\n",
			termname);
	fd = open("/dev/null", O_WRONLY, 0);
	setupterm(termname, fd, &rsl);
	close(fd);
	if(rsl != 1){
		errdsp2("Can't get terminfo entry", termname);
		 /* ���ΤȤ�resetterm()��ɬ�פ��� */
		return(1);
	}
	convert_getstrs(flg);
	resetterm();
	return(0);
}
#endif

 /**	termcap/info�����ä������Υǡ������������ѥ��ꥢ�˼��ޤäƤ����
	���ơ������Ȥä�convert_key�����ν�����򤹤롣���ͤ����ｪλ��0��
	ɽ�ɤ߹��߻���Warning��ȯ��������1��Error�򸡽Ф����Ȥ�-1��*/
convert_key_setup(filename, flg)
char	*filename;
int	flg;
{
	register int	i;
	int	c, d, cnt = 0, warn_occur = 0, entry_found;
	char	name[ENTRY_LEN + 1]; 

	int	keydef_code[ENTRY_CNT]; /* convert code */

	if(CHANGE_MAX < div_up(ENTRY_CNT, BITSIZ)){
		fprintf(stderr, "%s%s%d%s",
			"Sorry, please set CHANGE_MAX(in file ",
			"conv/cvt_head.h) larger than ",
			div_up(ENTRY_CNT, BITSIZ) - 1,
			",\r\nand recompile.\r\n");
		return(-1);
	}

	for(i = 0; i < ENTRY_CNT; i++) keydef_code[i] = -1;
	 /* code�� -1 �Τޤޤʤ顢convert_keyɽ�˥���ȥ꤬�ʤ����Ȥ򼨤���*/

	if(NULL != filename) convert_filename = filename;
	if(flg) fprintf(stderr,
			"convert_key: using convert_key table %s...\r\n",
			convert_filename);
	if(NULL == (convert_hyo = fopen(convert_filename, "r"))){
#ifdef NOFILE_NOT_ABORT
		warn1(convert_filename);
		cnv_tbl_cnt = 0;
		return(1);
#else
		errdsp1("Convert-table file is not found", convert_filename);
		return(-1);
#endif
	}

 /* ɽ������ɤߤ��� */
	while(EOF != (d = getc(convert_hyo))){
		if(d == '#' || d == ';'){
		 /* '#'����';'�ǻϤޤ�Ԥ����Ȥ����ɤ����Ф���*/
			linepass(convert_hyo);
			continue;
		}
		ungetc(d, convert_hyo);

		d = blank_ign_getc(convert_hyo);
		if(is_eofnl(d)) continue; /* ���� */
		 else ungetc(d, convert_hyo);
		if(!get_entrynm(name)){
			errdsp1("Entry name too long", convert_filename);
			fclose(convert_hyo);
			return(-1);
		} /* ����ȥ�̾���ɤ߹������*/

		d = blank_ign_getc(convert_hyo);
		if(is_eofnl(d)){
			errdsp1("Convert code missing", convert_filename);
			fclose(convert_hyo);
			return(-1);
		} else ungetc(d, convert_hyo);
		if(get_code(&c) != 0 || linepass(convert_hyo) != 0){
			errdsp1("Convert code illegal", convert_filename);
			fclose(convert_hyo);
			return(-1);
		} /* �Ѵ������ɤ��ɤ߹������*/
#ifdef DEBUG
		printf("%s %d\n", name, c);
#endif
		for(entry_found = i = 0; i < ENTRY_CNT; i++){
			if(!strcmp(cvttbl_entry[i] , name)){
				if(keydef_code[i] != -1){
					warn2(name, convert_filename);
					warn_occur = 1;
				}
				keydef_code[i] = c;
				entry_found = 1;
				break;
			}
		}
		if(!entry_found){
			warn3(name, convert_filename);
			warn_occur = 1;
		}
	}

 /* �ɤߤ�����ǡ�����ơ��֥�˥��å� */
	for(i = 0; i < ENTRY_CNT; i++){
		if(keydef_key[i] != NULL){
			if(keydef_code[i] != -1 &&
			   (strlen(keydef_key[i]) > 1 ||
			    *keydef_key[i] != keydef_code[i])){
				tbl[cnt] . fromkey = keydef_key[i];
				tbl[cnt++] . tokey = keydef_code[i];
			}
		} else if(keydef_code[i] != -1){
			warn4(cvttbl_entry[i], convert_filename,
			      keydef_code[i]);
			warn_occur = 1;
		}
	}

	cnv_tbl_cnt = cnt;
	fclose(convert_hyo);
	if(flg) fprintf(stderr, "convert_key: finished setting up.\r\n");
	return(warn_occur);
}

 /**	convert_key����������롼����SYSVR2 �ξ��� setupterm()��BSD42��
	���� tgetent()�򤽤줾�쥳���뤷����˸Ƥ֡����ͤ�convert_key_setup
	���ȡ�*/
convert_key_init(filename, flg)
char	*filename;
int	flg;
{
	convert_getstrs(flg);
	return(convert_key_setup(filename, flg));
}

 /**	SYSVR2 �ξ��� setupterm()��BSD42�ξ��� tgetent()�ޤǴޤ�Ƽ¹�
	convert_key����������롼�����֤��ͤ�convert_key_setup���ȡ�â����
	termcap/info���ɤ߹��ߤ˼��Ԥ������ϡ�-2���֤롣*/
convert_getterm_init(termname, filename, flg)
char	*termname, *filename;
int	flg;
{
	if(convert_getterm(termname, flg) != 0) return(-2);
	return(convert_key_setup(filename, flg));
}

 /** ����С���ɽ�Υ���ȥ���ʬ��Ȥ롣����������Ĺ��Ķ������0���֤� */
get_entrynm(buf)
char	*buf;
{
	int	i, c;

	for(i = 0; i <= ENTRY_LEN; i++){
		c = getc(convert_hyo);
		if(is_eofsp(c)){
			ungetc(c, convert_hyo);
			*buf = '\0';
			return(1);
		} else *buf++ = c;
	}
	return(0);
}

 /** ����ʲ���ʸ��������ˤ�ȤФ��Ƽ���ʸ�����֤� */
blank_ign_getc(f)
FILE	*f;
{
	int	c;

	while(c = getc(f), is_space(c) && c != '\n');
	return(c);
}

 /** ����������EOF�ˤޤ����Ф������δ֤������ʸ�������ä�����0���֤���*/
linepass(f)
FILE	*f;
{
	int	c, flg = 0;

	while(c = getc(f), !is_eofnl(c)) flg = (flg || !is_space(c));
	return(flg);
}

 /**	������ɽ����Ȥäơ�cptr�λؤ���ˤ��Υ����ɤ�����롣
	ʸˡ���顼ȯ��������0���֤���*/
get_code(cptr)
int	*cptr;
{
	int	c;

	switch(c = getc(convert_hyo)){
		case '\\': return(get_bcksla(cptr));
		case '^': return(get_ctrl(cptr));
		default: *cptr = c; return(0);
	}
}

 /** �Хå�����å�������Υ�����ɽ����ʸ��ʬ��Ȥ롣���顼���������0���֤�*/
get_bcksla(cptr)
int	*cptr;
{
	int	c, code = 0, digflg = 0;

	switch(c = getc(convert_hyo)){
		case 'n':
			*cptr = '\n'; return(0);
		case 't':
			*cptr = '\t'; return(0);
		case 'b':
			*cptr = '\b'; return(0);
		case 'r':
			*cptr = '\r'; return(0);
		case 'f':
			*cptr = '\f'; return(0);
		case 'e':
		case 'E':
			*cptr = ESCCHR; return(0);
		case 'o':
			while(c = getc(convert_hyo), is_octal(c)){
				code <<= 3;
				code += charvalue(c);
				digflg = 1;
			}
			ungetc(c, convert_hyo);
			*cptr = code;
			return(digflg == 0);
		case 'd':
			while(c = getc(convert_hyo), is_digit(c)){
				code *= 10;
				code += charvalue(c);
				digflg = 1;
			}
			ungetc(c, convert_hyo);
			*cptr = code;
			return(digflg == 0);
		case 'x':
			while(c = getc(convert_hyo), is_xdigit(c)){
				code <<= 4;
				code += charvalue(c);
				digflg = 1;
			}
			ungetc(c, convert_hyo);
			*cptr = code;
			return(digflg == 0);
		default:
			if(is_octal(c)){
				code = charvalue(c);
				while(c = getc(convert_hyo), is_octal(c)){
					code <<= 3;
					code += charvalue(c);
				}
				ungetc(c, convert_hyo);
				*cptr = code;
				return(0);
			} else {
				*cptr = c;
				return(0);
			}
	}
}

 /** ����ȥ��륳���ɷ����Υ�����ɽ����ʸ��ʬ��Ȥ롣���顼����������͡�0*/
get_ctrl(cptr)
int	*cptr;
{
	int	c;

	if(!(' ' <= (c = getc(convert_hyo)) && c < '\177')) return(1);
	*cptr = (c == '?' ? '\177' : c & 0x1f);
	return(0);
}
