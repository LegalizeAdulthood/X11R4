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
#include "commonhd.h"
#include "config.h"

WNN_JSERVER_ID	*js;
struct wnn_ret_buf rb = {0, NULL};
#define WNNSTAT_ENV "This_is_wnnstat"

#define W_WHO 0
#define W_ENV 1
#define W_ALL 2
#define W_FILE 3
#define W_DIC 4
#define W_DIC_ALL 5
#define W_VERSION 6

int ocode = TTY_KCODE;

int com = W_WHO;

main(argc,argv)
int argc;
char **argv;
{
    char *tmp;
    int c, i,j;
    WNN_ENV_INFO *w;
    extern int optind;
    extern char *optarg;
    extern char *getenv();

    while ((c = getopt(argc,argv,"EewdDfFUSJV")) != EOF) {
	switch(c){
	case 'w':
	    com = W_WHO;
	    break;
	case 'E': 
	    com = W_ALL;
	    break;
	case 'e': 
	    com = W_ENV;
	    break;
	case 'd':
	    com = W_DIC;
	    break;
	case 'D':
	    com = W_DIC_ALL;
	    break;
	case 'f':
	case 'F':
	    com = W_FILE;
	    break;
	case 'V':
	    com = W_VERSION;
	    break;
	case 'U':
	    ocode = J_EUJIS;
	    break;
	case 'J':
	    ocode = J_JIS;
	    break;
	case 'S':
	    ocode = J_SJIS;
	    break;
	default:
	    usage();
	    break;
	}
    }
    if (optind) {
	optind--;
	argc -= optind;
	argv += optind;
    }
    rb.buf = (char *)malloc((unsigned)(rb.size = 0));
    if(argc > 1){
	tmp = argv[1];
    }else if(getenv(WNN_JSERVER_ENV)){
	tmp = getenv(WNN_JSERVER_ENV);
    }else {
	tmp = "";
    }

    if((js=js_open(tmp, WNN_TIMEOUT)) == NULL){
	out("wnnstat:");
	out(tmp);
	out(wnn_perror());
	putchar('\n');
/*	out(" jserver と接続出来ません。\n"); */
	exit(1);
    }
/*
    if((env=js_connect(js, "It's me")) == NULL) err();
*/
    switch (com) {
    case W_WHO:
	who();
	break;
    case W_ALL:
	if((c = js_env_list(js, &rb)) == -1) err();
	w=(WNN_ENV_INFO *)rb.buf;
	printall(w, c);
	break;
    case W_ENV:
	if((c = js_env_list(js, &rb)) == -1) err();
	w=(WNN_ENV_INFO *)rb.buf;
	printpat(w, c);
	break;
    case W_DIC:
	dic();
	break;
    case W_DIC_ALL:
	dic_all();
	break;
    case W_FILE:
	file();
	break;
    case W_VERSION:
	js_version(js, &i, &j);
	printf("Jserver Version %x, Jlib Version %x\n", i, j);
    }
    exit (0);
}

printall(w, c)
WNN_ENV_INFO *w;
int c;
{
    int i, j;
    out("Env No.\t環境名\t参照数\t付属語\t辞書数(辞書No.)\t\tファイル\n");
    for(i = 0 ; i < c ; i++, w++){
	out("%d\t%s\t%d\t%d\t%d(", w->env_id, w->env_name, w->ref_count, w->fzk_fid,
	       w->jishomax);
	for(j = 0 ; j < w->jishomax; j++){
	    if(w->jisho[j] != -1)
		out("%d ", w->jisho[j]);
	}
	out(")\t");
	for(j = 0 ; j < WNN_MAX_FILE_OF_AN_ENV; j++){
	    if(w->file[j] != -1)
		out("%d ", w->file[j]);
	}
	putchar ('\n');
    }
}

printpat(w, c)
WNN_ENV_INFO *w;
int c;
{
    int i;
    out("Env No.\t環境名\t参照数\n");
    for(i = 0 ; i < c ; i++, w++){
	out("%d\t%s\t%d\n",w->env_id, w->env_name, w->ref_count);
    }
}

err()
{
    out(wnn_perror());
    putchar('\n');
    exit(255);
}

usage()
{
    fprintf(stderr, "wnnstat [-weEdDfFSUJ] [server_name]\n");
    exit(1);
}

who()
{
    WNN_JWHO *w;
    int c, k, j;
    struct wnn_env *env;

    if((env = js_connect(js, WNNSTAT_ENV)) == NULL) err();

    if((c = js_who(js, &rb)) == -1) err();
    w=(WNN_JWHO *)rb.buf;
    out("ユーザ名:ホスト名\t(ソケットNo.)\t環境番号\n");
    for(k = 0 ; k < c ; k++, w++){
	if(w->env[0] == env->env_id) continue;
	out("%s:%s\t\t(%d)\t\t", w->user_name, w->host_name, w->sd);
	for(j = 0 ; j < WNN_MAX_ENV_OF_A_CLIENT; j++){
	    if(w->env[j] != -1){
		out("%d ", w->env[j]);
	    }
	}
	out("\n");
    }
    exit(c - 1);
}

dic()
{
    char ch[256];
    WNN_DIC_INFO *w;
    int c, k, j;
    if((c = js_dic_list_all(js, &rb)) == -1) err();
    w=(WNN_DIC_INFO *)rb.buf;
    out("No.\t種類\tニックネーム\tファイル\t\n");
    for(k = 0 ; k < c ; k++, w++){
	wsttost(ch, w->comment);
	out("%d:%s\t%s\t%d:%s", w->dic_no, 
	    wnn_dic_types[w->type],
	       ch, w->body, w->fname);
	if(w->hindo != -1){
	    out("(%d)\n", w->hindo);
	}else{
	    out("\n");
	}
    }
}

dic_all()
{
    char ch[256];
    WNN_DIC_INFO *w;
    int c, k, j;
    if((c = js_dic_list_all(js, &rb)) == -1) err();
    w=(WNN_DIC_INFO *)rb.buf;
    out("No.種類\t正/逆\t語数\t更新(頻)使用 プライオリティ\t[ニックネーム]\tファイル[(頻度)]\t[パスワード(頻度)]\n");
    for(k = 0 ; k < c ; k++, w++){
	wsttost(ch, w->comment);
	out("%d:%s\t%s\t%d\t%s(%s)\t%s   %d\t",
	       w->dic_no, 
	    wnn_dic_types[w->type],
/*	       (w->type == WNN_UD_DICT)? "登録":"固定", */
	    w->rev ? "逆変換":"正変換",
	       w->gosuu, 
	       (w->rw)? "不":"可",
	       (w->hindo_rw)? "不":"可",
	       (w->enablef)? "中":"停止",
	       w->nice);
	if(*ch) out("%s\t", ch);
	out("%d:%s\t", w->body, w->fname);
	if(w->hindo != -1){
	    out("(%d:%s)\t",w->hindo, w->hfname);
	}
	if(w->passwd[0])out("%s", w->passwd);
	if(w->hpasswd[0])out("(%s)", w->hpasswd);
	out("\n");
    }
}


file()
{
    char *d;
    WNN_FILE_INFO_STRUCT *w;
    int c, k, j;
    if((c = js_file_list_all(js, &rb)) == -1) err();
    w=(WNN_FILE_INFO_STRUCT *)rb.buf;
    out("Fid\t種類\t場所\t参照数\t\tファイル名\n");
    for(k = 0 ; k < c ; k++, w++){
	switch (w->type){
	case WNN_FT_DICT_FILE:
	    d = "辞書";
	    break;
	case WNN_FT_HINDO_FILE:
	    d = "頻度";
	    break;
	case WNN_FT_FUZOKUGO_FILE:
	    d = "付属語";
	    break;
	}
	out("%d\t%s\t%s\t%d\t%s\n", w->fid, d, 
	       w->localf ? "LOCAL": "REMOTE",
	       w->ref_count, w->name);
    }
}




wsttost(c, w)
char *c;
w_char *w;
{
    while(*w){
	if(*w & 0xff00)
	    *c++ = (*w & 0xff00) >> 8;
	*c++ = *w++ & 0x00ff;
    }
    *c = 0;
}

through(x, y, z)
char *x, *y;
{
    bcopy(y, x, z);
    return z;
}

extern	int eujis_to_jis8(), eujis_to_sjis();

out(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)
char *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10, *a11, *a12, *a13;
{
    int len;
    char buf[1024];
    char jbuf[1024];
    sprintf(buf, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);

    len = strlen(buf);
    switch(ocode){
    case J_EUJIS:
	strncpy(jbuf, buf, len + 1);
	break;
    case J_JIS:
	eujis_to_jis8(jbuf, buf, len + 1);
	break;
    case J_SJIS:
	eujis_to_sjis(jbuf, buf, len + 1);
	break;
    }
    printf("%s", jbuf);
}


