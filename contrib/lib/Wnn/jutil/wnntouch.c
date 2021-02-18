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
  辞書のヘッダを書き換えて、辞書の inode と合わせるプログラム。
  */

#include <stdio.h>
#include "jslib.h"
#include "commonhd.h"

char *com_name;
struct wnn_file_head fh;

main(argc, argv)
int argc;
char **argv;
{
    FILE *ifpter;
    int k;

    com_name = argv[0];
    parse_options(argc, argv);

    for(k = 1 ; k < argc ; k++){
	if((ifpter = fopen(argv[k] , "r")) == NULL){
	    fprintf(stderr , "Can't open the input file %s.\n" , argv[k]);
	    perror("");
	    exit(1);
	}
	if(input_file_header(ifpter, &fh) == -1){
	    fprintf(stderr, "%s %s: I's not a Wnn File.\n", com_name, argv[k]);
	    exit(1);
	}
	if(check_inode(ifpter, &fh) == -1){
	    if(change_file_uniq(&fh, argv[k]) == -1){
		fprintf(stderr, "%s %s: Can't change file_uniq.\n", com_name, argv[k]);
		perror("");
		exit(1);
	    }
	}
	fclose(ifpter);
    }
    exit(0);
}

parse_options(argc, argv)
int argc;
char **argv;
{
    int c;
    extern int optind;
    extern char *optarg;

    while ((c = getopt(argc,argv,"")) != EOF) {
    }
    if (optind) {
	optind--;
	argc -= optind;
	argv += optind;
    }
    if(argc < 2){
	usage();
    }
}


usage()
{
  fprintf(stderr , "Usage: %s Wnn_file_name* \n",com_name);
  exit(1);
}

