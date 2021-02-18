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
#include "jslib.h"
/*#include "commonhd.h" */

#define FZK_FILE "/usr/local/lib/wnn/dic/pubdic/full.fzk"
#define KIHON_DIC "/usr/local/lib/wnn/dic/pubdic/pd-kihon.sys"
#define SETTO_DIC "/usr/local/lib/wnn/dic/pubdic/pd-setsuji.sys"
#define REV_DIC "/usr/local/lib/wnn/dic/pubdic/pd-kihon.sysR"
#define REV_DIC_SETTO "/usr/local/lib/wnn/dic/pubdic/pd-setsuji.sysR"

WNN_JSERVER_ID	*js;
struct wnn_env *env, *rev_env;
static struct wnn_ret_buf rb = {0, NULL};

int	dcnt,scnt;

char yomi[1024];
char kanji[5000];

int kihon_file, setto_file, rev_file, fzk_file, rev_setto_file;
int kihon_dic_no, setto_dic_no, rev_dic_no, rev_setto_dic_no;

main(argc, argv)
int argc;
char **argv;
{
    char *mname = "";
    rb.buf = (char *)malloc((unsigned)(rb.size = 0));
    if(argc > 1) mname = argv[1];

    if((js=js_open(mname)) == NULL) err();

    if((env=js_connect(js, "kana")) == NULL) err(); 
    if((fzk_file = js_file_read(env,FZK_FILE)) == -1) err();
    if((kihon_file = js_file_read(env,KIHON_DIC)) == -1) err();
    if((setto_file = js_file_read(env,SETTO_DIC)) == -1) err();

    if(js_fuzokugo_set(env,fzk_file) == -1) err();
    if((kihon_dic_no = js_dic_add(env,kihon_file,-1,1,WNN_DIC_RDONLY, WNN_DIC_RDONLY, NULL, NULL)) == -1) err();
    if((setto_dic_no=js_dic_add(env,setto_file,-1,1,WNN_DIC_RDONLY, WNN_DIC_RDONLY, NULL, NULL)) == -1) err();
    p_set(env);

    if((rev_env=js_connect(js, "kanji")) == NULL) err();

    if((fzk_file = js_file_read(rev_env,FZK_FILE)) == -1) err();
    if((rev_file = js_file_read(rev_env,REV_DIC)) == -1) err();
    if((rev_setto_file = js_file_read(rev_env,REV_DIC_SETTO)) == -1) err();

    if(js_fuzokugo_set(rev_env,fzk_file) == -1) err();
    if((rev_dic_no = js_dic_add(rev_env,rev_file,-1,1,WNN_DIC_RDONLY, WNN_DIC_RDONLY, NULL, NULL)) == -1) err();
    if((rev_setto_dic_no = js_dic_add(rev_env,rev_setto_file,-1,1,WNN_DIC_RDONLY, WNN_DIC_RDONLY, NULL, NULL)) == -1) err();
    p_set(rev_env);

#ifdef DEBUG
printf("Now discard file push any key\n");
getchar();
    js_file_discard(rev_env, rev_file);
printf("Now discard file\n");
#endif

    henkan();
    js_close(js);	
}

henkan()
{
    wchar u[1024];
    struct wnn_env *c_env = env;
    struct wnn_dai_bunsetsu *dp;

    for(;;){
	if(c_env == env){printf("yomi> ");fflush(stdout);}
	else {printf("kanji> ");fflush(stdout);}
	if(gets(yomi) == NULL)return;
	if(yomi[0] == '!')return;
	if(yomi[0] == '@'){
	    c_env = (c_env == env)? rev_env:env;
	    continue;
	}
	strtows(u,yomi);
	dcnt =js_kanren(c_env,u,WNN_BUN_SENTOU, NULL, WNN_VECT_KANREN, WNN_VECT_NO,&rb);
	dp = (struct wnn_dai_bunsetsu *)rb.buf;
	print_kanji(dp,dcnt );
    }
}

print_kanji(dlist, cnt)
struct wnn_dai_bunsetsu	*dlist;
int	cnt;
{
    int i;
    struct wnn_sho_bunsetsu  *sbn;

    if (dlist == 0) return;
    putchar('\n');
    for ( ; cnt > 0; dlist++, cnt --) {
	sbn = dlist->sbn;
	for (i = dlist->sbncnt; i > 0; i--) {
	    putws(sbn->kanji); printf("-"); 
	    putws(sbn->fuzoku); printf(" ");
	    sbn++;
	}
	printf("|"); 
    }
    putchar('\n');
    fflush(stdout);
}

p_set(env1)
struct wnn_env *env1;
{
struct wnn_param pa;
	pa.n= 2;		/* n_bun */
	pa.nsho = 10;	/* nshobun */
	pa.p1 = 2;	/* hindoval */
	pa.p2 = 40;	/* lenval */
	pa.p3 = 0;	/* jirival */
	pa.p4 = 100;	/* flagval */
	pa.p5 = 5;	/* jishoval */
	pa.p6 = 1;	/* sbn_val */
	pa.p7 = 15;	/* dbn_len_val */
	pa.p8 = -20;	/* sbn_cnt_val */
	pa.p9 = 0;	/* kan_len_val */

 js_param_set(env1,&pa);
}


putwchar(x)
unsigned short x;
{
    putchar( x >> 8);
    putchar( x );
}

putws(s)
unsigned short *s;
{
    while(*s) putwchar(*s++);
}

strtows(u,e)
wchar *u;
unsigned char *e;
{int x;
 for(;*e;){
	x= *e++;
	if(x & 0x80) x = (x << 8)  | *e++;
	*u++= x;
 }
 *u=0;
}

err()
{
    printf(wnn_perror());
    printf("\n bye.\n");
    exit(1);
}
