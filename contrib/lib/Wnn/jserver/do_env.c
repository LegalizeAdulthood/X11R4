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

#include <stdio.h>

#include "demcom.h"
#include "commonhd.h"
#include "de_header.h"
#include "ddefine.h"	/* Add KURI */
#include "wnn_malloc.h"


struct	cnv_env	*env[MAX_ENV];

/*
	client routines
 */

js_open()
{register int version;
 register int i;
 	version = get4_cur();
	gets_cur( c_c->host_name);
	gets_cur( c_c->user_name);
	error1("Inet user=%s@%s\n",c_c -> user_name,c_c->host_name);
	for(i=0;i<WNN_MAX_ENV_OF_A_CLIENT;i++){
		(c_c->env)[i]= -1;
	}
 	if(version != JLIB_VERSION){ 
	    wnn_errorno = WNN_BAD_VERSION;
	    error_ret(); return;
	}
	put4_cur(0);
	putc_purge();
}

void
js_close()
{
	put4_cur(0);
	putc_purge();
	del_client();
}

/*
	env. control routines
 */

env_init()
{register int i;
 for(i=0;i<MAX_ENV;i++){
	env[i]=NULL;
 }
}

/*	connect	*/
js_connect()
{char n[256];
 register int x;
 gets_cur(n);
 x=conn1(n);
 if(x==-1){error_ret(); return;}
 put4_cur(x);
 putc_purge();
}

js_env_sticky()
{
 register int eid;
 eid=get4_cur();
 env[eid]->sticky = 1;
 put4_cur(0);
 putc_purge();
}

js_env_un_sticky()
{
 register int eid;
 eid=get4_cur();
 env[eid]->sticky = 0;
 put4_cur(0);
 putc_purge();
}

conn1(n)
register char *n;
{register int eid;
 register struct cnv_env *ne;
 if((eid=find_env_by_name(n)) != -1){ /* exist */
	if(find_env_in_client(eid) != -1) return eid;/* exist in client */
	if(add_env_to_client(eid) == -1) return -1;
	env[eid]->ref_count ++;
	return eid;
 }
 /* new */
 for(eid=0;eid<MAX_ENV ; eid++){
	if(env[eid]==NULL) goto new;
 }
 if((eid = disconnect_last_sticky()) < 0)
     return -1;	/* no more env */
new:
 if((ne= (struct cnv_env *)malloc(sizeof(struct cnv_env)))==0) return -1;
 env[eid]=ne; new_env(eid,n);
 if(add_env_to_client(eid) == -1){free(ne); env[eid]=NULL; return -1;}
 error1("new_env: Created , %s env_id=%d\n",n,eid);
 return eid;
}

js_env_exist()
{
    char n[256];

    gets_cur(n);
    if(find_env_by_name(n) != -1){ /* exist */
	put4_cur(1);
    }else{
	put4_cur(0);
    }
    putc_purge();
}

add_env_to_client(eid)
{register int j;
 for(j=0;j<WNN_MAX_ENV_OF_A_CLIENT;j++){
	if((c_c->env)[j] == -1){ (c_c->env)[j] = eid; return j;}
 }
 return -1;	/* no more env of Client */
}

find_env_by_name(n)
register char *n;
{
 register int i;

 if(n[0] == 0) return -1;	/* H.T. Null-string means new_env! */
 for(i=0;i<MAX_ENV ; i++){
	if(env[i]==NULL) continue;
	if(strcmp(env[i]->env_name,n)==0){ return i; }
 }
 return -1;
}

extern struct wnn_param default_para;

new_env(env_id,n)
register int env_id;
register char *n;
{register int i;
 struct cnv_env *e = env[env_id];

 strcpy(e->env_name,n);
 e->ref_count = 1;

 for(i=0;i<WNN_MAX_JISHO_OF_AN_ENV;i++){
	(e->jisho)[i]= -1;
 }
 for(i=0;i<WNN_MAX_FILE_OF_AN_ENV;i++){
	(e->file)[i]= -1;
 }

 e->fzk_fid = -1;  /* Not Loaded */
 e->nbun = default_para.n;
 e->nshobun = default_para.nsho;

 e->hindoval = default_para.p1;
 e->lenval = default_para.p2;
 e->jirival = default_para.p3;
 e->flagval = default_para.p4;
 e->jishoval = default_para.p5;
 e->sbn_val = default_para.p6;
 e->dbn_len_val = default_para.p7;
 e->sbn_cnt_val = default_para.p8;

 /* 疑似品詞の頻度のセット */
 e->suuji_val = default_para.p9;
 e->kana_val = default_para.p10;
 e->eisuu_val = default_para.p11;
 e->kigou_val = default_para.p12;
 e->toji_kakko_val = default_para.p13;
 e->fuzokogo_val = default_para.p14;
 e->kaikakko_val = default_para.p15;

 /* 疑似文節のためのデフォルト候補のセット */
 e->giji.number = WNN_NUM_HAN;
 e->giji.eisuu = WNN_ALP_HAN;
 e->giji.kigou = WNN_KIG_HAN;
}

/*	disconnect all env of current client (socket)	*/
disconnect_all_env_of_client()
{register int i,eid;
	for(i=0;i<WNN_MAX_ENV_OF_A_CLIENT;i++){
		eid=(c_c->env)[i];
		if(eid== -1) continue;
		disconnect(eid);
	}
}

/*	disconnect	*/
js_disconnect()
{register int eid,j;
 eid=get4_cur();

 if((j=find_env_in_client(eid)) == -1){
	 put4_cur(-1);
	 putc_purge();
	 return;
 }
 del_env_from_client(j);
 disconnect(eid);
 put4_cur(0);
 putc_purge();
}

static int sticky_cnt = 0;
static int sticky_time = 0;

disconnect(eid)
{
    if(env[eid]->ref_count && (--(env[eid]->ref_count))!=0) return;
    if(env[eid]->sticky) {		/* ADD KURI */
	env[eid]->sticky = ++sticky_time;
	if (++sticky_cnt > max_sticky_env)
	   disconnect_last_sticky();
	return;
    }
    del_all_dic_in_env(eid);
    del_all_file_in_env(eid);
    free(env[eid]); env[eid]=NULL;
    error1("disconnect: Free eid=%d\n",eid);
}

disconnect_last_sticky()
{
    register int i;

    register int min = 0x7fffffff;
    register int eid = -1;
    for(i=0;i<MAX_ENV ; i++){
	if(env[i]==NULL) continue;
	if(env[i]->ref_count==0) {
	    if (min >= env[i]->sticky){
		eid = i;
		min = env[i]->sticky;
	    }
	}
    }
    if (eid >= 0) {
	sticky_cnt--;
	del_all_dic_in_env(eid);
	del_all_file_in_env(eid);
	free(env[eid]); env[eid]=NULL;
	error1("disconnect: Free eid=%d\n",eid);
    }
    return (eid);
}

/* */
del_env_from_client(e)
{
 (c_c->env)[e] = -1;
}

find_env_in_client(eid)
{register int j;
 for(j=0;j<WNN_MAX_ENV_OF_A_CLIENT;j++){
	if((c_c->env)[j] == eid) return j;
 }
 return -1;
}

/*	env_list	*/
js_env_list()
{register int i,c,j;
 for(c=i=0;i<MAX_ENV ; i++){
	if(env[i]==NULL) continue;
	c++;
 }
 put4_cur(c);
 for(c=i=0;i<MAX_ENV ; i++){
	if(env[i]==NULL) continue;
	put4_cur(i);
	puts_cur(env[i]->env_name);
	put4_cur(env[i]->ref_count);
	put4_cur(env[i]->fzk_fid);
	put4_cur(env[i]->jishomax);
	for(j=0;j<WNN_MAX_JISHO_OF_AN_ENV;j++){
		put4_cur((env[i]->jisho)[j]);
	    }
	for(j=0;j<WNN_MAX_FILE_OF_AN_ENV;j++){
	    put4_cur((env[i]->file)[j]);
	}
 }
 putc_purge();
}

/*
	conv. param routines
 */

js_param_set()
{
    register int eid;
    register int ret = 0;
    if((eid= envhandle()) == -1){
	ret = -1;
    }

    env[eid]->nbun = get4_cur();	/* Ｎ(大)文節解析のＮ */
    env[eid]->nshobun = get4_cur();	/* 大文節中の小文節の最大数 */

    env[eid]->hindoval = get4_cur();	/* 幹語の頻度のパラメータ */
    env[eid]->lenval = get4_cur();	/* 小文節長のパラメータ */
    env[eid]->jirival = get4_cur();	/* 幹語長のパラメータ */
    env[eid]->flagval = get4_cur();	/* 今使ったよbitのパラメータ */
    env[eid]->jishoval = get4_cur();	/* 辞書のパラメータ */

    env[eid]->sbn_val = get4_cur();	/* 小文節の評価値のパラメータ */
    env[eid]->dbn_len_val = get4_cur();	/* 大文節長のパラメータ */
    env[eid]->sbn_cnt_val = get4_cur();	/* 小文節数のパラメータ */

    env[eid]->suuji_val = get4_cur();	/* 疑似品詞 数字の頻度 */
    env[eid]->kana_val = get4_cur();	/* 疑似品詞 カナの頻度 */
    env[eid]->eisuu_val = get4_cur();	/* 疑似品詞 英数の頻度 */
    env[eid]->kigou_val = get4_cur();	/* 疑似品詞 記号の頻度 */
    env[eid]->toji_kakko_val = get4_cur(); /* 疑似品詞 閉括弧の頻度 */
    env[eid]->fuzokogo_val = get4_cur(); /* 疑似品詞 付属語の頻度 */
    env[eid]->kaikakko_val = get4_cur(); /* 疑似品詞 開括弧の頻度 */

    if(ret == -1){error_ret(); return;}
    put4_cur(0);
    putc_purge();
#ifdef DEBUG
 error1("param_set:eid=%d\n",eid);
#endif
}


js_param_get()
{
    register int eid;
    if((eid= envhandle()) == -1){
	error_ret(); return;
    }
    put4_cur(0);
    put4_cur(env[eid]->nbun);		/* Ｎ(大)文節解析のＮ */
    put4_cur(env[eid]->nshobun);	/* 大文節中の小文節の最大数 */
    put4_cur(env[eid]->hindoval);	/* 幹語の頻度のパラメータ */
    put4_cur(env[eid]->lenval);		/* 小文節長のパラメータ */
    put4_cur(env[eid]->jirival);	/* 幹語長のパラメータ */
    put4_cur(env[eid]->flagval);	/* 今使ったよビットのパラメータ */
    put4_cur(env[eid]->jishoval);	/* 辞書のパラメータ */

    put4_cur(env[eid]->sbn_val);	/* 小文節の評価値のパラメータ */
    put4_cur(env[eid]->dbn_len_val);	/* 大文節長のパラメータ */
    put4_cur(env[eid]->sbn_cnt_val);	/* 小文節数のパラメータ */

    put4_cur(env[eid]->suuji_val);	/* 疑似品詞 数字の頻度 */
    put4_cur(env[eid]->kana_val);	/* 疑似品詞 カナの頻度 */
    put4_cur(env[eid]->eisuu_val);	/* 疑似品詞 英数の頻度 */
    put4_cur(env[eid]->kigou_val);	/* 疑似品詞 記号の頻度 */
    put4_cur(env[eid]->toji_kakko_val);	/* 疑似品詞 閉括弧の頻度 */
    put4_cur(env[eid]->fuzokogo_val);	/* 疑似品詞 付属語の頻度 */
    put4_cur(env[eid]->kaikakko_val);	/* 疑似品詞 開括弧の頻度 */

    putc_purge();
#ifdef DEBUG
 error1("param_get:eid=%d\n",eid);
#endif
}

envhandle()
{
#ifdef nodef
    register int eid;
    eid = get4_cur();
    if(find_env_in_client(eid) == -1){
	wnn_errorno = WNN_BAD_ENV;
	return(-1);
    }
    c_env = env[eid];
    return(eid);
#endif
    return(get4_cur());
}	
