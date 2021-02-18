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
 *	Do Dic operation by DicNo
 */

#include <stdio.h>

#include "demcom.h"
#include "commonhd.h"
#include "de_header.h"
#include "kaiseki.h"
#include "jdata.h"
#include "hinsi_file.h"


extern	struct	cnv_env	*env[];
extern	struct	wnn_file	files[];
struct	wnn_dic	dic_table[MAX_DIC];

dic_init()
{int i;
	for(i=0;i<MAX_DIC;i++){
		dic_table[i].body = -1;
		dic_table[i].hindo = -1;
	}
}

/*
	Dic. OPE by DicNo.
 */

/*	js_dic_info	*/
js_dic_info()
{int dic_no,envi;
	envi = get4_cur();	/* env_id */
	dic_no=get4_cur();
        if(!dic_in_envp(dic_no,envi)){
	    wnn_errorno = WNN_DICT_NOT_IN_ENV;
	    error_ret(); putc_purge();
	    return;
        }
	put4_cur(0); 
 	put_dic_info(dic_no);
	putc_purge();
}

put_dic_info(dic_no)
int dic_no;
{
    int fid, hfid; 
    struct JT *jtl;

    put4_cur(dic_no);
    put4_cur(fid = dic_table[dic_no].body);
    put4_cur(hfid = dic_table[dic_no].hindo);
    put4_cur(dic_table[dic_no].rw);
    put4_cur(dic_table[dic_no].hindo_rw);
    put4_cur(dic_table[dic_no].enablef);
    put4_cur(dic_table[dic_no].nice);
    put4_cur(dic_table[dic_no].rev);
/* added H.T */
    jtl = (struct JT *)files[fid].area;
    putnws_cur(jtl->comment, jtl->maxcomment);
    puts_cur(files[fid].name);
    if(hfid >= 0) puts_cur(files[hfid].name);
    else puts_cur("");
    puts_cur(files[fid].passwd);
    if(hfid >= 0) puts_cur(files[hfid].passwd);
    else puts_cur(jtl->hpasswd);
    put4_cur(jtl->syurui);
    put4_cur(jtl->gosuu);
    put4_cur(files[fid].localf);
    if(hfid >= 0) put4_cur(files[hfid].localf);
    else put4_cur(-1); 
}




/*	js_word_add	*/
js_word_add()
{int dic_no,hinsi,envi,hindo,ret;
 w_char	yomi[LENGTHYOMI], kanji[LENGTHKANJI], comment[LENGTHKANJI];
        envi = get4_cur();	/* env_id */
	dic_no=get4_cur();
	getws_cur(yomi);
	getws_cur(kanji);
	getws_cur(comment);
	hinsi=get4_cur();
	hindo=get4_cur();
 	ret = word_add(envi, dic_no, yomi, kanji, hinsi, hindo, comment);
	if(ret == -1){
	    error_ret();
	}else{
	    put4_cur(ret);
	}
	putc_purge();
}

/*	js_word_delete	*/
js_word_delete()
{int dic_no,dic_entry,envi, ret;
	envi = get4_cur();	/* env_id */
	dic_no=get4_cur();
	dic_entry=get4_cur();
	ret =  word_delete(envi,dic_no,dic_entry);
	if(ret == -1){
	    error_ret();
	}else{
	    put4_cur(ret);
	}
	putc_purge();
}

js_word_search_by_env()
{
    w_char yomi[LENGTHYOMI];
    w_char ryomi[LENGTHYOMI];

    c_env=env[get4_cur()];
    getws_cur(yomi);
    Sreverse(ryomi, yomi);
    init_jmt(0);
    jishobiki(ryomi, jmt_ptr);
    return_jbiki(jmt_ptr[Strlen(yomi) - 1]);
    putc_purge();
}


js_word_search()
{
    w_char yomi[LENGTHYOMI];
    w_char ryomi[LENGTHYOMI];
    int dic_no;

    get4_cur();	/* env_id */
    dic_no = get4_cur();
    getws_cur(yomi);
    Sreverse(ryomi, yomi);
    init_jmt(0);
    word_search(dic_no, ryomi, jmt_ptr);
    return_jbiki(jmt_ptr[Strlen(yomi) - 1]);
    putc_purge();
}


js_word_comment_set()
{
    int dic_no, envi, entry;
    int ret;
    wchar comment[LENGTHKANJI];

    envi = get4_cur();	/* env_id */
    dic_no = get4_cur();
    entry = get4_cur();
    getws_cur(comment);

    ret = word_comment_set(envi, dic_no, entry, comment);
    if(ret == -1){
	error_ret();
    }else{
	put4_cur(0);
    }
    putc_purge();
}

js_word_info()
{
    int dic_no, entry;
    struct jdata jdata;
    w_char yomi[LENGTHYOMI+1];
    w_char ryomi[LENGTHYOMI+1];

    get4_cur();	/* env_id */
    dic_no = get4_cur();
    entry = get4_cur();
    if(inspect(dic_no, entry, yomi, &jdata) == -1){
	error_ret();
	putc_purge();
    }else{
	put4_cur(0);
	Sreverse(ryomi, yomi);
	putws_cur(ryomi);
	return_jbiki(&jdata);
	putc_purge();
    }
}


return_jbiki(jd)
struct jdata *jd;
{
	put4_cur(count_jdata(jd));
	put4_cur(count_jdata_kanji(jd));
	return_jbiki1(jd);
}

return_jbiki1(jd)
struct jdata *jd;
{
    int t;
    w_char kouho[LENGTHKANJI];
    w_char comment[LENGTHKANJI];
    struct jdata *p;

    for(p=jd;p;p = p->jptr){
	for(t = 0 ; t < p->kosuu; t++){
	    put4_cur(p->jishono);
	    put4_cur(p->serial + t);

	    put4_cur(*(p->hinsi + t));

	    put4_cur(motoni2(*(p->hindo + t) & 0x7f));
	    put4_cur(*(p->hindo + t) >> 7);

/* internal hindo */
	    if(p->hindo_in){
		put4_cur(motoni2(*(p->hindo_in + t) & 0x7f));
		put4_cur(*(p->hindo_in + t)  >> 7);
	    }else{
		put4_cur(-1);
		put4_cur(-1);
	    }
	}
    }
    put4_cur(-1);
    for(p=jd;p;p = p->jptr){
	for(t = 0 ; t < p->kosuu; t++){
	    get_knj_com(p,t,kouho, comment);
/*	    get_knj(p, t, kouho);  H.T. Return Also Comment */
	    putws_cur(kouho); /* kanji */
	    putws_cur(comment); /* comment */
	}
    }
}

count_jdata(p)
struct jdata *p;
{ int sum;

    for(sum=0;p;p = p->jptr){
	sum += p->kosuu; 
    }
    return sum;
}

count_jdata_kanji(p)
struct jdata *p;
{
 int sum,t;
 w_char kouho[LENGTHKANJI];
 w_char comment[LENGTHKANJI];

 for(sum=0;p;p = p->jptr){
	for(t = 0 ; t < p->kosuu; t++){
	    get_knj_com(p, t, kouho, comment);
	    sum += Strlen(kouho); /* kanji */
	    sum += Strlen(comment); /* comment */
	}
 }
 return sum;
}

extern wchar *wnn_hinsi_name();

js_hinsi_name()
{
    wchar *c;
    int no;

    no = get4_cur();
    if((c = wnn_hinsi_name(no)) == NULL){
	error_ret(); putc_purge(); return;
    }
    put4_cur(Strlen(c));
    putws_cur(c);
    putc_purge();
}

js_hinsi_number()
{
    wchar name[WNN_HINSI_NAME_LEN];
    int no;

    getws_cur(name);

    if((no = wnn_hinsi_number(name)) == -1){
	error_ret(); putc_purge(); return;
    }
    put4_cur(no);
    putc_purge();
}


js_hinsi_list()
{
    wchar name[WNN_HINSI_NAME_LEN];
    wchar *c, *c1;
    int envi, dic_no, no;
    int l, l1, k;
    int fid = 0, mmynode;
    struct wnn_hinsi_node *mynode;
    struct JT *jtl;

    envi = get4_cur();
    dic_no = get4_cur();
    getws_cur(name);

    if(dic_no == -1){
/*	fid = env[envi]->fzk_fid; */
	fid = 0;
	mynode = NULL;
	mmynode = 0;
    }else{
#ifdef nodef   /* fid ha toumen tukawanai */
	if(!dic_in_envp(dic_no,envi)){
	    wnn_errorno = WNN_DICT_NOT_IN_ENV;
	    error_ret(); putc_purge();
	    return;
	}
#endif
	if(dic_no >= MAX_DIC || dic_no < 0){
	    wnn_errorno = WNN_DICT_NOT_IN_ENV;
	    error_ret(); putc_purge();
	    return;
	}

	fid = dic_table[dic_no].body;
	if(fid == -1){
	    wnn_errorno = WNN_DICT_NOT_IN_ENV;
	    error_ret(); putc_purge();
	    return;
	}
	jtl = (struct JT *)(files[fid].area);
	mynode = jtl->node;
	mmynode = jtl->maxnode;
	if(mmynode == 0) mynode = NULL;
    }

    if((no = wnn_hinsi_list( name, &c, mynode, mmynode)) == -1){
	error_ret(); putc_purge(); return;
    }
    put4_cur(no);
    for(l = 0, c1 = c, k = 0;k < no; k++){
/*	l += (l1 =  Strlen(c1));
	c1 += l1 + 1; H.T.*/
	l += (l1 =  Strlen(c1) + 1);
	c1 += l1;
    }
    put4_cur(l);
    for(k = 0, c1 = c; k < no; k++){
	putws_cur(c1);
	c1 += Strlen(c1) + 1;
    }
    putc_purge();
}

js_hinsi_dicts()
{
    int envi;
    w_char *name;
    register int k;
    int cnt;
    struct JT *jtl;
    int ret[MAX_DIC];
    int no;
    int mmynode;
    struct wnn_hinsi_node *mynode;
    int max;
    register int dno;

    envi = get4_cur();
    no = get4_cur();
    if(no != -1){		/* H.T. n == -1 to return all dicts */
	if((name = wnn_hinsi_name(no)) == NULL){
	    error_ret(); putc_purge(); return;
	}
    }
    cnt = 0;
    max = env[envi]->jishomax;
    for(k=0;k<max;k++){
	if((dno = (env[envi]->jisho)[k]) == -1) continue;
	if(dic_table[dno].body==-1) continue;
	if(dic_table[dno].rw == WNN_DIC_RDONLY) continue; /* H.T. */
	if(no == -1){
	    ret[cnt++] = dno;
	}else{
	    jtl = (struct JT *)(files[dic_table[dno].body].area);
	    mynode = jtl->node;
	    mmynode = jtl->maxnode;
	    if(mmynode == 0) mynode = NULL;
	    if(wnn_has_hinsi(mynode, mmynode, name)) ret[cnt++] = dno;
	}
    }
    put4_cur(cnt);
    for(k=0;k<cnt;k++){
	put4_cur(ret[k]);
    }
    putc_purge();
}    

js_hinsi_table_set()
{
    wchar table[HEAP_LEN];
    int envi, dic_no, fid;
    struct wnn_hinsi_node *mynode;
    struct JT *jtl;

    envi = get4_cur();
    dic_no = get4_cur();
    getws_cur(table);


    if(!dic_in_envp(dic_no,envi)){
	wnn_errorno = WNN_DICT_NOT_IN_ENV;
	error_ret(); putc_purge();
	return;
    }

    if(dic_no >= MAX_DIC || dic_no < 0){
	wnn_errorno = WNN_DICT_NOT_IN_ENV;
	error_ret(); putc_purge();
	return;
    }

    fid = dic_table[dic_no].body;
    if(fid == -1){
	wnn_errorno = WNN_DICT_NOT_IN_ENV;
	error_ret(); putc_purge();
	return;
    }
    jtl = (struct JT *)(files[fid].area);
    if(hinsi_table_set(jtl, table) == -1){
	error_ret(); putc_purge(); return;
    }
     put4_cur(0);
    putc_purge();
}

