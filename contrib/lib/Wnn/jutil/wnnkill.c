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
char *serv;
int ocode = TTY_KCODE;

main(argc, argv)
int argc;
char **argv;
{
    extern char *getenv();
    int c;
    char *tmp;
    int x;
    extern int optind;
    extern char *optarg;

    while ((c = getopt(argc,argv,"USJ")) != EOF) {
	switch(c){
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
	    break;
	}
    }
    if (optind) {
	optind--;
	argc -= optind;
	argv += optind;
    }

    if(argc > 1){
	serv = argv[1];
    }else if(tmp = getenv(WNN_JSERVER_ENV)){
	serv = tmp;
    }else{
	serv = "";
    }
    if((js=js_open(serv, WNN_TIMEOUT)) == NULL){
	out("%s:", serv);
	out("%s\n",wnn_perror()); 
/*	fprintf(stderr, "Can't connect to jserver.\n"); */
	exit(255);
    }
    if((x = js_kill(js)) > 0){
	out("%d Users Exists.\n", x);
	out("Jserver Not Killed.\n");
	exit(1);
    }else if(x == 0){
	out("Jserver Terminated\n");
	exit(0);
    }else{
	out("Jserver Terminated\n");
	exit(2);
    }
     exit (0);
}
	
extern	int eujis_to_jis8(), eujis_to_sjis();

out(a1, a2, a3, a4, a5, a6, a7, a8)
char *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8;
{
    int len;
    char buf[1024];
    char jbuf[1024];
    sprintf(buf, a1, a2, a3, a4, a5, a6, a7, a8);

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
    fprintf(stderr, "%s", jbuf);
}


