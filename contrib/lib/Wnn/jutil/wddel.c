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
  UJIS �ե�������ɤߤȤäơ�ñ����Ͽ��Ԥʤ���
*/

#include <stdio.h>
#include "commonhd.h"
#include "config.h"
#include "jslib.h"

#define WORD_DELETE 1
#define COMMENT_SET 2
#define HINDO_SET 3

char *def_server;

int dic_no = -1;
char *server_n;
char *env_n = "wddel";
char *fname = NULL;
int fid;
int client = 0;			/* server site dict */
int what = WORD_DELETE;

WNN_JSERVER_ID	*js;
struct wnn_env *env, *rev_env;
static struct wnn_ret_buf rb = {0, NULL};

main(argc, argv)
int argc;
char **argv;
{
    extern char *getenv();
    extern int optind;
    extern char *optarg;
    struct wnn_dic_info *info;
    int c;
    int k;
    int num;
    int sno;
    char s[256];

    if(getenv(WNN_JSERVER_ENV)){
	def_server = getenv(WNN_JSERVER_ENV);
    }else {
	def_server = "";
    }
    server_n = def_server;

    while ((c = getopt(argc,argv,"D:n:d:CHEL")) != EOF) {
	switch(c){
	case 'D':
	    server_n = optarg;
	    break;
	case 'n':
	    env_n = optarg;
	    break;
	case 'd':
	    dic_no = atoi(optarg);
	    break;
	case 'C':
	    what = COMMENT_SET;
	    break;
	case 'H':
	    what = HINDO_SET;
	    break;
	case 'E':
	    what = WORD_DELETE;
	    break;
	case 'L':
	    client = 1;
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
    if(argc > 1){
	fname = argv[1];
    }
    if(fname && dic_no != -1)usage();
    if(!fname && dic_no == -1)usage();


    rb.buf = (char *)malloc((unsigned)(rb.size = 0));

    if((js = js_open(server_n)) == NULL) err();
    if((env=js_connect(js, env_n)) == NULL) err(); 
    if(fname){
	if(client){
	    if((fid = js_file_send(env, fname)) == -1) err();
	}else{
	    if((fid = js_file_read(env, fname)) == -1) err();
	}
	if((dic_no = js_dic_add(env, fid, -1, 0, 0, WNN_DIC_RW, WNN_DIC_RW, "", "")) == -1)
	    err();
    }

    if((num = js_dic_list(env, &rb)) == -1) err();
    
    info = (struct wnn_dic_info *)(rb.buf);
    for(k = 0 ;k < num; k++){
	if(info[k].dic_no == dic_no) break;
    }
    if(k == num){
	fprintf(stderr, "���ꤵ�줿�ֹ�μ���ϡ��Ķ���¸�ߤ��ޤ���\n");
	exit(1);
    }
    if(info[k].type != WNN_UD_DICT && info[k].type != WNN_REV_DICT){
	fprintf(stderr, "���ꤵ�줿�ֹ�μ���ϡ���Ͽ��ǽ�ǤϤ���ޤ���\n");
	exit(1);
    }
    while(gets(s)){
	char com[LENGTHYOMI];
	char Com[LENGTHYOMI];
	int ima, hindo;
	if(s[0] == '\\') continue;
	switch(what){
	case WORD_DELETE:
	    if(sscanf(s, "%d", &sno) <= 0){
		fprintf(stderr, "Bad line \"%s\"", s);
		continue;
	    }
	    if(js_word_delete(env, dic_no, sno) == -1){
		fprintf(stderr, "serial-no = %d\n", sno);
		err();
	    }
	    break;
	case COMMENT_SET:
	    if(sscanf(s, "%d %s",&sno, com) <= 1){
		fprintf(stderr, "Bad line \"%s\"", s);
		continue;
	    }
	    Sstrcpy(Com, com);
	    if(js_word_comment_set(env, dic_no, sno, Com) == -1){
		fprintf(stderr, "serial-no = %d\n", sno);
		err();
	    }
	    break;
	case HINDO_SET:
	    if(sscanf(s, "%d %n %n",&sno, &ima, &hindo) <= 2){
		fprintf(stderr, "Bad line \"%s\"", s);
		continue;
	    }
	    if(js_hindo_set(env, dic_no, sno, ima, hindo) == -1){
		fprintf(stderr, "serial-no = %d\n", sno);
		err();
	    }
	    break;
	}
    }
    if(fname){
	if(client){
	    if(js_file_receive(env, fid, "") == -1) err();
	}else{
	    if(js_file_write(env, fid, "") == -1) err();
	}
	fprintf(stderr, "Wrote the file back.\n");
    }
    exit(0);
}

usage()
{
    fprintf(stderr, "wdreg [-D server_name] [-n env_name][-d dic_no][-L][-E][-H][-C] [file_name]  < text dict\n");
    fprintf(stderr, "file_name or -d dic_no must be specified\n");
    fprintf(stderr, "default env_name = wdreg\n");
    fprintf(stderr, "default server_name = %s\n", def_server);
    fprintf(stderr, "L is to specify that the file at the client site.\n");
    fprintf(stderr, "-E is word_delete, -C is word_comment_set, -C is hindo_set.\n");
    exit(1);
}

err()
{
    printf(wnn_perror());
    printf("\n bye.\n");
    exit(1);
}
