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
/*
	Nihongo Henkan Library
*/
/*
	entry functions

	jd_open		jd_open_in	jd_close
	jd_begin	jd_end
	jd_next		jd_reconv	jd_tanconv
	jd_udp		jd_udchg	jd_freqsv
	jd_dicadd	jd_dicdel	jd_dicinfo
	jd_wdel		jd_wsch		jd_wreg
	jd_setevf	jd_getevf
	jd_dicstatus    jd_whdel
	jd_mkdir	jd_access
	jd_who		jd_version

extern	Variables
	int	wnn_errorno;
*/


#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <pwd.h>
#ifdef	SYSVR2
#include <malloc.h>
#else
extern char *malloc();
#endif

#include "jllib.h"
#include "jlib.h"
#include "wnnerror.h"

#ifdef SYSVR2
#define	bzero(adr,n)	memset((adr),0,(n))
#endif

static struct wnn_buf *buf;

jmp_buf	jd_server_dead_env;
int	jd_server_dead_env_flg = 0;

/*	j Lib.	*/
static	char	*jlib_work_area = NULL;

/*	bun joho	*/
static	struct	bunjoho	bun;		/** ʸ����	**/

static int current_bun_no = -1;	/* ��������äƤ���ʸ�� */
static int current_ud = -1;	/* �����ȡ��桼������ */

static struct wnn_ret_buf ret_buf = {0, NULL};

/*	get login name form /etc/passwd file	*/
static	char *
getlogname()
{
    extern struct passwd *getpwuid();
    struct passwd *p;

    if ((p = getpwuid(getuid())) == NULL)
	return(NULL);
    return(p->pw_name);
}

/*
 *		Lib. Functions
 */

/**	jd_close	**/
jd_close()
{
    if (jlib_work_area) {free(jlib_work_area);}
    jl_dic_save_all(buf);
    jl_close(buf);
    return (0);
}

/**	jd_reconv	**/
jd_reconv(bunsetsu_no,kbuf,kb_siz) wchar *kbuf;
{
    if(bunsetsu_no==0)
	return jd_begin(kbuf,kb_siz);
    jl_ren_conv(buf, &(bun.kana_buf)[((bun.klist)[bunsetsu_no]).s_ichi],
		bunsetsu_no, -1, WNN_USE_MAE);
    return henkan_rcv(bunsetsu_no,kbuf,kb_siz);
}

/**	jd_begin	**/
jd_begin(kbuf,kb_siz)wchar *kbuf;
{
    current_bun_no = -1;
    jl_kill(buf, 0, -1);
    jl_ren_conv(buf, bun.kana_buf, 0, -1, WNN_NO_USE);
    return (henkan_rcv(0,kbuf,kb_siz));
}

/**	jd_tanconv	**/
jd_tanconv(bunsetsu_no,moji_no,kbuf,kb_siz) wchar *kbuf;
{
    if(moji_no<0)
	return (-1);
    jl_nobi_conv(buf, bunsetsu_no, moji_no, -1, WNN_USE_MAE, WNN_SHO);
    return henkan_rcv(bunsetsu_no,kbuf,kb_siz);
}

/**	�Ѵ���̤��������	**/
static
int
henkan_rcv(bunsetsu_no,kbuf,kb_siz)
int	bunsetsu_no;	/** ʸ���ֹ� **/
wchar	*kbuf;		/** �����Хåե��� **/
int	kb_siz;		/** �����Хåե��������� **/
{

    register wchar *b, *p;
    register int i,bun_no;
    register KOUHO_ENT *klp;
    int kanji_len = 0;
    int tmp;

    bun_no = jl_bun_suu(buf);
    if (jl_kanji_len(buf, bunsetsu_no, - 1) >= kb_siz)
	return -1;

    if(bun_no >= bun.klist_size) {
	wnn_errorno= WNN_LONG_MOJIRETSU;
	return -1;
    }
    for(i = bunsetsu_no, klp= &((bun.klist)[bunsetsu_no]), b=kbuf;
	    i < bun_no; klp++,i++) {
	klp->s_ichi = jl_yomi_len(buf, 0, i);
	klp->jl = jl_jiri_len(buf, i);
	klp->fl = jl_fuzoku_len(buf, i);
	klp->jishono= buf->bun[i]->dic_no;
	klp->serial = buf->bun[i]->entry;
	if(jl_kanji_len(buf, i, i + 1) + kanji_len >= kb_siz) {
	    wnn_errorno= WNN_LONG_MOJIRETSU;
	    return (-1);
	}
	klp->k_data = b;
	jl_get_kanji(buf, i, i + 1, b);
	tmp = jl_jiri_kanji_len(buf, i);
	b += tmp;
	kanji_len += tmp + 1;
	*b =0;
	b++;
	klp->pl = 0;
	for (p = klp->k_data; *p; p++) {
	    klp->pl += (*p & 0x8000) ? 2 : 1;
	}
	klp->pl += (klp->fl * 2);
    }
    klp->s_ichi= -1;
    return (bun_no - bunsetsu_no);
}

/**	jd_end		**/

jd_end()
{
    int ret;

    up_date_jikouho();
    current_bun_no = -1;
    ret = jl_update_hindo(buf, 0, -1);
    jl_kill(buf, 0, -1);
    return (ret);
}


/**	jd_next		**/
jd_next(bunsetsu_no,to_go)JIKOUHOJOHO *to_go;
{
    register int no_of_kouho,i;
    register wchar *b;
    register JIKOUHO_ENT *jlp;

    if (current_bun_no != -1 && current_bun_no != bunsetsu_no)
	up_date_jikouho();
    current_bun_no = bunsetsu_no;
    jl_zenkouho(buf, bunsetsu_no, WNN_USE_MAE, WNN_UNIQ);
    no_of_kouho = jl_zenkouho_suu(buf);

    if(no_of_kouho >= to_go->jlist_size-1) {
	wnn_errorno=WNN_JKTAREA_FULL;
	return (-1);
    }
    for(i = 0, jlp = to_go->jlist, b = to_go->kanji_buf;
	    i < no_of_kouho; jlp++, i++){

	jlp->jl = jl_jiri_len(buf, bunsetsu_no);
	jlp->jishono = buf->bun[bunsetsu_no]->dic_no;
	jlp->serial = buf->bun[bunsetsu_no]->entry;
	jlp->k_data = b;
	if(jl_kanji_len(buf, bunsetsu_no, bunsetsu_no + 1) >= to_go->kanji_buf_size) {
	    wnn_errorno=WNN_JKTAREA_FULL;
	    return (-1);
	}
	jl_get_kanji(buf, bunsetsu_no, bunsetsu_no + 1, b);
	b += jl_jiri_kanji_len(buf, bunsetsu_no);
	*b =0;
	b++;
	jl_next(buf);
    }
    jlp->jl = -1;
    return no_of_kouho;
}

static
up_date_jikouho()
{
    register int no_of_kouho,i;
    int jishono, serial;

    jishono = (bun.klist)[current_bun_no].jishono;
    serial = (bun.klist)[current_bun_no].serial;

    no_of_kouho = jl_zenkouho_suu(buf);

    for(i=0; i < no_of_kouho; i++){
	if (jishono == buf->bun[current_bun_no]->dic_no &&
	    serial == buf->bun[current_bun_no]->entry)
		return (0);
	jl_next(buf);
    }
    wnn_errorno=WNN_WORD_NO_EXIST;
    return (-1);
}

/*	JD dictionary	*/

/**	jd_dicadd	**/
jd_dicadd(fn,fn1,prio,hrdonly)
char *fn,*fn1;
int prio,hrdonly;
{
    int rdonly;
    WNN_DIC_INFO ret;
    int dic_no;
    if (fn1 == 0 || *fn1 == 0) { /* UD */
	rdonly = hrdonly;
    } else if ((hrdonly == WNN_DIC_RDONLY) &&
    /* READ ONLY SD && hind file is none */
	       (js_access(jl_env(buf), fn, 4) != -1) &&
	       (js_access(jl_env(buf), fn1, 4) == -1)) {
	if ((dic_no = jl_dic_add(buf, fn, fn1, WNN_DIC_ADD_NOR, prio,
			WNN_DIC_RDONLY,
			WNN_DIC_RW, NULL, NULL, WNN_CREATE, NULL)) < 0)
	    return (-1);
	if(jl_dic_delete(buf, dic_no) < 0)
	    return (-1);
	rdonly = WNN_DIC_RDONLY;
    } else {
	rdonly = WNN_DIC_RDONLY;
    }
    if ((dic_no = jl_dic_add(buf, fn, fn1, WNN_DIC_ADD_NOR, prio,
			     rdonly, hrdonly,
			     NULL, NULL, WNN_CREATE, NULL)) < 0)
			/*   pw_d  pw_h  err   mes */
	return (-1);
    if (js_dic_info(jl_env(buf), dic_no, &ret) < 0)
	return (-1);
    if ((ret.type == WNN_UD_DICT || ret.type == WNN_REV_DICT) && ret.rw == WNN_DIC_RW)
	current_ud = dic_no;
    return (dic_no);
}

/**	jd_dicdel	**/
jd_dicdel(dict_no)
{
    return(jl_dic_delete(buf, dict_no));
}


/**	jd_udchg	**/
jd_udchg(dic_no)
{
    WNN_DIC_INFO ret;

    if (js_dic_info(jl_env(buf), dic_no, &ret) < 0)
	return (-1);
    if (ret.type != WNN_UD_DICT && ret.type != WNN_REV_DICT) {
	wnn_errorno = WNN_NOT_A_UD;
	return (-1);
    } else if (ret.rw != WNN_DIC_RW || ret.enablef != 1) {
	wnn_errorno = WNN_RDONLY;
	return (-1);
    }
    current_ud = dic_no;
    return (0);
}

/**	jd_udp	**/
jd_udp(dic_no)
int dic_no;
{
    WNN_DIC_INFO ret;
    int rdonly = 0;

    if (js_dic_info(jl_env(buf), dic_no, &ret) < 0)
	return (-1);
    if (ret.rw == WNN_DIC_RDONLY || ret.enablef == 0)
	rdonly |= 0x4;
    if (ret.type == WNN_UD_DICT || (ret.type == WNN_REV_DICT && ret.rw == WNN_DIC_RW)) {
	rdonly = 1;
	if (current_ud == dic_no)
	    rdonly |= 0x2;
    }

    return(rdonly);
}

/**	jd_freqsv	**/
jd_freqsv()
{
    return(jl_dic_save_all(buf));
}

/*	JD word		*/

/**	jd_whdel		**/
jd_whdel(ser_no,yomi,bunpou)
wchar *yomi;
{
    return (jl_word_delete(buf, current_ud, ser_no));
}


/**	jd_wdel		**/
jd_wdel(ser_no,yomi)
wchar *yomi;
{
    return (jl_word_delete(buf, current_ud, ser_no));
}

/**	jd_wreg		**/
jd_wreg(kanji,yomi,bunpo)
wchar *kanji,*yomi;
{
#ifdef	NEW_HINSI
    return(jl_word_add(buf, current_ud, yomi, kanji, NULL, bunpo, 0));
#else
    int	i;
    int ret;
    int hinsi_suu;
    unsigned short *hinsi;
    /* hinsi No conversion */
    hinsi_suu = oldh_to_newh(bunpo, &hinsi);
    for (i = 0; i < hinsi_suu; i++, hinsi++) {
	ret = jl_word_add(buf, current_ud, yomi, kanji, NULL, *hinsi, 0);
	if (ret < 0)
		return (ret);
    }
#endif
}

/**	jd_wsch		**/
jd_wsch(yomi,jbuf,jbuf_siz,kbuf,kbuf_siz)
wchar *yomi,*kbuf;
JISHOJOHO *jbuf;
{
    register int i,klen;
    register wchar *b;

    struct wnn_jdata *jdp;
    int len;
    int cnt;

    if((cnt = jl_word_search_by_env(buf, yomi, &jdp)) < 0)
	return (-1);

    if(cnt >= jbuf_siz - 1) {
	wnn_errorno=WNN_JISHOTABLE_FULL;
	return (-1);
    }
    for(klen = 0, i = cnt, b = kbuf; i > 0; jbuf++, jdp++, i--) {
	jbuf->hindo= jdp->hindo;
#ifdef	NEW_HINSI
	jbuf->bumpo= jdp->hinshi;
#else	/* hinsi No conversion */
	jbuf->bumpo= newh_to_oldh(jdp->hinshi);
#endif
	jbuf->jisho= jdp->dic_no;
	jbuf->serial= jdp->serial;
	jbuf->k_data= b;
	len = Strlen(jdp->kanji) + 1;
	klen += len;
	if(klen >= kbuf_siz) {
	    wnn_errorno=WNN_JISHOTABLE_FULL;
	    return (-1);
	}
	Strcpy(b, jdp->kanji);
	b += len;
    }
    jbuf->hindo= -1;
    return cnt;
}


/*	Parameter set/get	*/
/**	jd_setenvf		**/
jd_setevf(bunsetsu_su,par1,par2,par3,par4,par5)
{
    struct wnn_param para;
    int ret;
    ret = jl_param_get(buf, &para);
    if (ret < 0)
	return (ret);
    para.n = bunsetsu_su;
    para.p1 = par1;
    para.p2 = par2;
    para.p3 = par3;
    para.p4 = par4;
    para.p5 = par5;
    return(jl_param_set(buf, &para));
}


/**	jd_getenvf		**/
jd_getevf(bunsetsu_su,par1,par2,par3,par4,par5)
int *bunsetsu_su,*par1,*par2,*par3,*par4,*par5;
{
    struct wnn_param para;
    int ret;

    ret = jl_param_get(buf, &para);
    *bunsetsu_su=para.n;
    *par1=para.p1;	/* ��������� */
    *par2=para.p2;	/* ��ʸ��Ĺ */
    *par3=para.p3;	/* ��Ω��Ĺ */
    *par4=para.p4;	/* ���Ȥä���ӥå� */
    *par5=para.p5;	/* ���� */
    if (ret < 0);
	return (ret);
}


/**	jd_dicinfo		**/
jd_dicinfo(jbuf,jbuf_siz,fbuf,fbuf_siz)
DICINFO *jbuf;int jbuf_siz;char *fbuf;int fbuf_siz;
{
    register int i,klen,x;
    register char *b;

    WNN_DIC_INFO *dic;
    int cnt, len;

    if ((cnt = js_dic_list(jl_env(buf), &ret_buf)) < 0)
	return (-1);

    if(cnt >= jbuf_siz - 1) {
	wnn_errorno=WNN_JISHOTABLE_FULL;
	return (-1);
    }
    dic = (WNN_DIC_INFO *)ret_buf.buf;
    for(klen = i = 0, b = fbuf; i < cnt; jbuf++, dic++, i++){
	jbuf->dic_no = dic->dic_no;
	jbuf->ttl_hindo = 0;	/* ??? */
	/*
	jbuf->dic_type = dic->type;
	*/
	if (dic->type == WNN_STATIC_DICT || (dic->type == WNN_REV_DICT && dic->rw == WNN_DIC_RDONLY))
	    jbuf->dic_type = SYSTEM_DIC;
	else if (dic->type == WNN_UD_DICT || (dic->type == WNN_REV_DICT && dic->rw == WNN_DIC_RW))
	    jbuf->dic_type = USER_DIC;
	jbuf->udp = jbuf->dic_no == current_ud ? 1 : 0;
	jbuf->dic_size = dic->gosuu;
	jbuf->prio = dic->nice;
	jbuf->rdonly = dic->rw | !dic->enablef;
	jbuf->file_name= b;
	len = strlen(dic->fname) + 1;
	klen += len;
	if(klen >= fbuf_siz) {
	    wnn_errorno=WNN_JISHOTABLE_FULL;
	    return (-1);
	}
	strcpy(b, dic->fname);
	b + = len;
	jbuf->h_file_name= b;
	len = strlen(dic->hfname) + 1;
	klen += len;
	if(klen >= fbuf_siz) {
	    wnn_errorno=WNN_JISHOTABLE_FULL;
	    return (-1);
	}
	strcpy(b, dic->hfname);
	b + = len;
    }
    jbuf->dic_no= -1;
    return i;
}


/**	jd_open_in			**/
struct bunjoho *
jd_open_in(kana_len,klist_len,kanji_len,server,user,host)
int kana_len,klist_len,kanji_len;
char *server,*user,*host;
{
    register int kasz,klsz,kjsz;
    char b[256];
    int cnt, i;
    WNN_FILE_INFO_STRUCT   *files;

    if (jlib_work_area != NULL)
	free(jlib_work_area);

    if (user == NULL || *user == '\0') {	/* ENV name */
	user= getlogname();
    }

    if((buf = jl_open(user, server, NULL, WNN_NO_CREATE, NULL, 0)) == NULL)
	return (NULL);
    else if (buf->env == NULL)
	return (NULL);
    kasz = sizeof(wchar)*(kana_len + 1);
    klsz = sizeof(KOUHO_ENT)*(klist_len + 1);
    kjsz = sizeof(wchar)*kanji_len;
    if(!(jlib_work_area = malloc(kasz+klsz+kjsz))){
	wnn_errorno = WNN_MALLOC_INITIALIZE;return 0;
    }
    bun.klist = (KOUHO_ENT *)jlib_work_area;
    bun.klist_size = klist_len;
    bun.kana_buf = (wchar *)(jlib_work_area + klsz) ;
    bun.kana_size = kana_len;
    bun.kanji_buf = (wchar *)(jlib_work_area + klsz + kasz);
    bun.kanji_buf_size = kanji_len;

    if((cnt = js_file_list_all(buf->env->js_id, &ret_buf)) <= 0)
	return(NULL);
    for (i = 0, files = (WNN_FILE_INFO_STRUCT *) ret_buf.buf; i < cnt; i++) {
	if(files->type == WNN_FT_FUZOKUGO_FILE)
		break;
	files++;
    }
    if (i == cnt)
	return(NULL);
    if(jl_fuzokugo_set(buf,files->name) == -1){
	return(NULL);
    }
    return &bun;
}

/**	jd_open		**/
struct bunjoho *
jd_open(kana_len,klist_len,kanji_len)
int kana_len,klist_len,kanji_len;
{
 return  (struct bunjoho *)
   jd_open_in(kana_len,klist_len,kanji_len,NULL,getlogname(),NULL);
}


/**	jd_dicstatus	**/
jd_dicstatus(fn)
char *fn;
{
    WNN_FILE_STAT s;
    if(js_file_stat(jl_env(buf), fn, &s) < 0)
	return (-1);
    if (s.type == WNN_STATIC_DICT)
	return(SYSTEM_DIC);
    else if (s.type == WNN_UD_DICT || s.type == WNN_REV_DICT)
	return(USER_DIC);
    else if (s.type == WNN_FILE_NOT_EXIST)
	return(NO_EXIST);
    else
	return(NOT_A_JISHO);
}

/**	jd_mkdir	**/
jd_mkdir(dn)
char *dn;
{
    return(js_mkdir(jl_env(buf), dn));
}

/**	jd_access	**/
jd_access(dn,amode) char *dn;
{
    return(js_access(jl_env(buf), dn, amode));
}

/**	jd_who		**/
jd_who(u,str)
JWHO u[];
char *str;
{
    register int i;
    register int cnt;
    WNN_JWHO *jwho;

    if ((cnt = js_who(jl_env(buf)->js_id, &ret_buf)) < 0)
	return (-1);
    jwho = (WNN_JWHO *)ret_buf.buf;
    for (i = 0; i < cnt; i++, jwho++) {
	u[i].sd = jwho[0].sd;
	u[i].user_name = str;
	strcpy(str, jwho->user_name);
	str += strlen(u[i].user_name) + 1;
	u[i].host_name = str;
	strcpy(str, jwho->host_name);
	str += strlen(u[i].host_name) + 1;
    }
    u[i].sd = -1;
    return (0);
}

/**	jd_version		**/
jd_version(serv,libv)
int *serv,*libv;
{
    return(js_version(jl_env(buf)->js_id, serv, libv));
}

#ifndef	NEW_HINSI
#define	KOMEI	0x20000000
#define	MEISI	0x00040000
#define	JOSUU	0x04000000
#define	SETTO	0x01000000
#define	SETUBI	0x02000000
#define	KEIYOU	0x00010000
#define	KEIDOU	0x00020000
#define	OLD_HINSI_SZ	32

static char *new_name[] = {
    "���Ը���",    "����(�Ԥ�)",  "���Ը���",   "���Ը���",
    "���Ը���",    "�ʹԸ���",    "�йԸ���",   "�޹Ը���",
    "��Ը���",    "��Ը���",    "����&̾��",  "����",
    "����(����)&̾��",  "����(����)",    "����(����)",    "��(��)",
    "���ƻ�",    "����ư��",      "̾��",      "Ϣ�λ�",
    "����",     "��³��,��ư��",    "��(��)",  "��(��)",
    "��Ƭ��",    "������",    "������",    "����",
    "���(������)",    "��ͭ̾��",    "����ư��(����)","ñ����"
};
/*
:��(��):��(��):��(��)
*/


#define	KOMEI_SZ	3	/* "��ͭ̾��" */
static char *komei_name[] = {"��̾", "��̾", "��̾&��̾"};

#define	MEISI_SZ	1	/* "̾��" */
static char *meisi_name[] = {"����ư��&̾��"};

#define	JOSUU_SZ	2	/* "������" */
static char *josuu_name[] = {"��Ƭ������", "����������"};

#define	SETTO_SZ	7	/* "��Ƭ��" */
static char *setto_name[] = {"��Ƭ�줪", "��Ƭ����", "��Ƭ��̾",
	"���ƻ첽����ư��", "����ư��", "����(����)&̾�첽������",
	"����ư�첽������"};

#define	SETUBI_SZ	4	/* "������" */
static char *setubi_name[] = {
	"������̾", "������̾", "������̾", "��������(����)����̾̾��"
};

#define	DOUSI_SZ	15
static char *dousi_name[] = {
    "��ư��",	"",         "��ư��",   "��ư��",
    "��ư��",	"��ư��",   "��ư��",   "��ư��",
    "��ư��",	"��ư��",   "",         "��ư��",
    "",		"����ư��", "����ư��"
};

static char *dou_renyou_name[] = {
    "��ưϢ�Ѵ�",   "",             "��ưϢ�Ѵ�",   "��ưϢ�Ѵ�",
    "��ưϢ�Ѵ�",   "��ưϢ�Ѵ�",   "��ưϢ�Ѵ�",   "��ưϢ�Ѵ�",
    "��ưϢ�Ѵ�",   "��ưϢ�Ѵ�",   "",             "��ưϢ�Ѵ�",
    "",             "����ưϢ�Ѵ�", "����ưϢ�Ѵ�"
};

static int
oldh_to_newh(hinsi, new)
int	hinsi;
unsigned short **new;
{
    register int i, j;
    unsigned short tmp[OLD_HINSI_SZ + 1];
    register int tmptmp;	/* H.T. to compile PCC */

    
    for (i = j = 0; hinsi != 0 && i < OLD_HINSI_SZ; i++) {
	if (hinsi & 0x01 != 0) {
	    if((tmptmp = wnn_find_hinsi_by_name(new_name[i])) != -1){
		tmp[j] = tmptmp;
		j++;
	    }else{
		tmp[j] = tmptmp;
	    }

/*	H.T. unsigned short comparison with -1
	    if ((tmp[j] = wnn_find_hinsi_by_name(new_name[i])) != -1)
	    	j++;
*/
	    }
	hinsi >>= 1;
    }

    tmp[j] = -1;
    *new = tmp;
    return (j);
}

static int
newh_to_oldh(hinsi)
unsigned short hinsi;
{
    register char *new_n;
    register int i;
    extern char *wnn_get_hinsi_name();
    
    if ((new_n = wnn_get_hinsi_name(hinsi)) == 0)
	return (-1);

    for (i = 0; i < OLD_HINSI_SZ; i++) {
	if (strcmp(new_n, new_name[i]) == 0)
	    return( 0x1 << i);
    }

    for (i = 0; i < DOUSI_SZ; i++) {
	if (strcmp(new_n, dousi_name[i]) == 0)
	    return( 0x1 << i);
    }
    for (i = 0; i < DOUSI_SZ; i++) {
	if (strcmp(new_n, dou_renyou_name[i]) == 0)
	    return(wnn_find_hinsi_by_name(new_name[i]));
    }

    for (i = 0; i < KOMEI_SZ; i++) {
	if (strcmp(new_n, komei_name[i]) == 0)
	    return (KOMEI);
    }
    for (i = 0; i < MEISI_SZ; i++) {
	if (strcmp(new_n, meisi_name[i]) == 0)
	    return (MEISI);
    }
    for (i = 0; i < JOSUU_SZ; i++) {
	if (strcmp(new_n, josuu_name[i]) == 0)
	    return (JOSUU);
    }
    for (i = 0; i < SETTO_SZ; i++) {
	if (strcmp(new_n, setto_name[i]) == 0)
	    return (SETTO);
    }
    for (i = 0; i < SETUBI_SZ; i++) {
	if (strcmp(new_n, setubi_name[i]) == 0)
	    return (SETUBI);
    }
    return (0);
}
#endif	/* ! NEWHINSI */

/*******************************/
/* From ../etc/strings.c */
typedef unsigned short w_char;

Strcpy(s1,s2)
register w_char *s1;
register w_char *s2;
{
	register w_char *d;

	for (d = s1;(*d++ = *s2++) != 0;);
}

int Strlen(s)
register w_char *s;
{
	register int n;

	for (n = 0;*s++ != 0;n++);
	return n;
}

int Strcmp(s1,s2)
register w_char *s1;
register w_char *s2;
{
  for (;*s1 != 0 && *s1 == *s2;s1++, s2++);
  return (int)(*s1 - *s2);
}

/************************************/
/* From ../etc/sstrings.c */
int 
sStrcpy(c , w)
register char *c;
register w_char *w;
{
    for(;*w != 0 ; w++){
	if(*w & 0xff00)
	  *c++ = (*w & 0xff00) >> 8;
	else if (*w & 0x80)
	  *c++ = 0x8e;
	*c++ = *w & 0x00ff;
    }
    *c++ = 0;
}


Sstrcpy(w , c )
w_char *w;
unsigned char *c;
{
    int a = 0;

    for(;*c;a++){
	if(*c & 0x80){
	    if (*c == 0x8e)
		*c = 0;
	    *w++ = (int)*c << 8 | *(c + 1);
	    c += 2;
	}else{
	    *w++ = *c;
	    c += 1;
	}
    }
    *w = 0;
    return(a);
}
