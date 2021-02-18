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
  辞書を ASCII 形式の辞書に変換するプログラム。
  */

#include <stdio.h>
#include "commonhd.h"
#include "jslib.h"
#include "jh.h"
#include "jdata.h"

int which_dict;

char *com_name;
char infile[LINE_SIZE];
int esc_expand = 0;

int serial_out = 0;
int need_sort = 1;
struct JT jt;
struct wnn_file_head file_head;
struct uind1 *tary;		/* index 1 */
char *hostart;

struct rind1 *yri1, *kri1;
struct rind2 *ri2;

char *hinsi_file_name = NULL;

main(argc, argv)
int argc;
char **argv;
{
    FILE *ifpter;

    com_name = argv[0];

    parse_options(&argc, &argv);

    if((ifpter = fopen(infile , "r")) == NULL){
	fprintf(stderr , "Can't open the input file %s.\n" , infile);
	perror("");
	exit(1);
    }
    if(input_header(ifpter, &jt, &file_head) == -1)exit(1);
    which_dict = jt.syurui;
    input_comment(ifpter);
    input_hinsi_list(ifpter);
    input_dic(ifpter);
/*
    if(which_dict == WNN_STATIC_DICT){
	input_sd(ifpter);
    }else{
	input_ud(ifpter);
    }
*/
    if(little_endian()){
	revdic(&jt, 0);
/*	revdic_jt(&jt, 0, hostart, tary); */
    }
    ghindo(argc - 2 , argv + 2);	/* Gather Hindo */
    if(which_dict == WNN_STATIC_DICT){
	kset();
	sdtoujis(hostart, 0);
/*	reverse_yomi(); */
    }else if(which_dict == WNN_REV_DICT){
	rdtoujis();
    }else{
	udtoujis();
/*	reverse_yomi(); */
    }
    if(need_sort){
	sort();
    }
    output_ujis(stdout, serial_out, esc_expand);
    exit (0);
}


parse_options(argc, argv)
int *argc;
char ***argv;
{
    int c;
    extern int optind;
    extern char *optarg;

    while ((c = getopt(*argc,*argv,"nseh:")) != EOF) {
	switch(c){
	case 'n': 
	    need_sort = 0;
	    break;
	case 's':
	    serial_out = 1;
	    break;
	case 'e':
	    esc_expand = 1;
	    break;
	case 'h':
	    hinsi_file_name = optarg;
	    break;
	default:
	    usage();
	}
    }
    if (optind) {
	optind--;
	*argc -= optind;
	*argv += optind;
    }
    if(*argc != 2){
	usage();
    }
    strcpy(infile, (*argv)[1]);
    wnn_loadhinsi(hinsi_file_name);
}


usage()
{
  fprintf(stderr , "Usage: %s [-nse -h hinsi_file_name] <dictionary file> <hindo file>*\n",com_name);
  fprintf(stderr , "Output the ascii dictionary to stdout\n" );
  exit1();
}


/* UD Routine */
UCHAR *kanji;

input_dic(ifpter)
FILE *ifpter;
{

    init_heap(0,			/* jt.maxkanji,*/
	    jt.maxserial * YOMI_PER_LINE * 5, /* may be enough */
	    jt.maxserial, jt.maxserial, stdin);
    tary = (struct uind1 *)malloc(jt.maxtable * sizeof(struct uind1));
    hostart = (char *)malloc(jt.maxhontai);
    yri1 = (struct rind1 *)malloc(jt.maxri1[D_YOMI] * sizeof(struct rind1));
    kri1 = (struct rind1 *)malloc(jt.maxri1[D_KANJI] * sizeof(struct rind1));
    kanji = (UCHAR *)malloc(jt.maxkanji + 1);
    ri2 = (struct rind2 *)malloc(jt.maxserial * sizeof(struct rind2));
    if(!tary || !hostart || !yri1 || !kri1 || !kanji || !ri2){
	fprintf(stderr, "Malloc Failed.\n");
	exit(1);
    }
    init_jeary();

    input_hindo(ifpter);
    input_hinsi(ifpter);
    fread(kanji, 1, jt.maxkanji, ifpter);
/*  input_kanji(ifpter); */
    fread(tary , sizeof(struct uind1), jt.maxtable , ifpter);
    fread(yri1, sizeof(struct rind1), jt.maxri1[D_YOMI], ifpter);
    fread(kri1, sizeof(struct rind1), jt.maxri1[D_KANJI], ifpter);
    fread(hostart , 1 , jt.maxhontai , ifpter);
    fread(ri2, sizeof(struct rind2), jt.maxserial, ifpter);
}

udtoujis()
{
    
    w_char  yomi[LENGTHYOMI]; 	/* 2 byte yomi */
    w_char  kan[LENGTHYOMI]; 	/* 2 byte kanji */
    w_char  comm[LENGTHYOMI];
    w_char ryomi[LENGTHYOMI];
    register struct uind2 *hop;
    register int i, k;
    register int serial;
    int len;
    UCHAR *c;
    
    for(i = 0 ; i < jt.maxtable; i++){
	for(hop = (struct uind2 *)(tary[i].pter + (char *)hostart);
	    hop != (struct uind2 *)hostart;
	    hop = (struct uind2 *)(hop->next  + (char *)hostart)){
	    /* Though each hop has the same yomi, but calculate yomi each time. */
	    yomi[0] = tary[i].yomi1 >> 16;
	    yomi[1] = tary[i].yomi1 & 0xffff;
	    if(yomi[1]){
		yomi[2] = tary[i].yomi2 >> 16;
		if(yomi[2]){
		    yomi[3] = tary[i].yomi2 & 0xffff;
		}
	    }
	    len = hop->yomi[0];
	    Strncpy(yomi + 4, (hop->yomi) + 1, len - 4);
	    yomi[len] = 0;
	    serial = hop->serial;
	    for(k = hop->kosuu, 
		c = hop->kanjipter + kanji
		;k > 0;
		k--, serial++){
		get_kanji_str(c, kan, NULL, comm);
		jeary[serial]->kanji = c;
		c += *c;
		jeary[serial]->serial = serial;
		Sreverse(ryomi, yomi);
		w_stradd(ryomi, &jeary[serial]->yomi);
		w_stradd(kan, &jeary[serial]->kan);
		w_stradd(comm, &jeary[serial]->comm);
	    }
	}
    }
}
/* SD routine */

w_char  yomi[LENGTHYOMI];
int je = 0;

kset()
{
    int i;
    UCHAR *c;
    w_char  kan[LENGTHYOMI]; 	/* 2 byte kanji */
    w_char  comm[LENGTHYOMI]; 	/* 2 byte kanji */
    

    c = kanji;
    for(i = 0 ; i < jt.maxserial; i++){
	get_kanji_str(c, kan, NULL, comm);
	w_stradd(kan, &jeary[i]->kan);
	w_stradd(comm, &jeary[i]->comm);
	jeary[i]->kanji = c;
	c += *c;
    }
}

sdtoujis(hopter,level)
char *hopter;
register int level;
{
  register int k, j;
  register int tsize;
  register w_char *charst;
  register w_char *sumst;
  int *ptrst;
  static w_char ry[LENGTHYOMI];

  yomi[level + 1] = 0;
  switch(*(w_char *)hopter){
  case ST_NORMAL:
    tsize = *(w_char *)(hopter + 2);
    charst = (w_char *)(hopter + 12);
    sumst = ((w_char *)charst + tsize + 2); /* + 2 keeps two zero words */
    ptrst = (int *)((w_char *)sumst + tsize);
    for(k = 0 ; k < tsize ; k++){
      j = sumst[k] - sumst[k - 1];
      if(j){
	yomi[level] = charst[k];
	for(;j > 0;j--){
	    jeary[je]->serial = je;
	    Sreverse(ry, yomi);
	    w_stradd(ry, &jeary[je++]->yomi);
	}
      }
    }
    for(k = 0 ; k < tsize ; k++){
      if(ptrst[k] != ENDPTR){
	yomi[level] = charst[k];
	sdtoujis((char *)hostart + ptrst[k],level + 1);
      }
    }
    break;
  case ST_NOENT:
    tsize = *(w_char *)(hopter + 2);
    charst = (w_char *)(hopter + 4);
    ptrst = (int *)AL_INT((w_char *)charst + tsize);
    for(k = 0 ; k < tsize ; k++){
      if(ptrst[k] != ENDPTR){
	yomi[level] = charst[k];
	sdtoujis((char *)hostart + ptrst[k],level + 1);
      }
    }
    break;
  case ST_NOPTER:
    tsize = *(w_char *)(hopter + 2);
    charst = (w_char *)(hopter + 12);
    sumst = ((w_char *)charst + tsize + 2); /* + 2 keeps two zero words */
    for(k = 0 ; k < tsize ; k++){
      j = sumst[k] - sumst[k - 1];
      if(j){
	yomi[level] = charst[k];
	for(;j > 0;j--){
	    jeary[je]->serial = je;
	    Sreverse(ry, yomi);
	    w_stradd(ry, &jeary[je++]->yomi);
	}
      }
    }
    break;
  case ST_SMALL:
    yomi[level] = *(w_char *)(hopter + 2);
    sdtoujis(hopter + 4,level + 1);
    break;
  }
}


rdtoujis()
{
    
    w_char  yomi[LENGTHYOMI]; 	/* 2 byte yomi */
    w_char  kan[LENGTHYOMI]; 	/* 2 byte yomi */
    w_char  comm[LENGTHYOMI]; 	/* 2 byte yomi */
    register struct rind2 *hop;
    register int hopi;
    register int i;
    register int serial;
    
    for(i = 0 ; i < jt.maxri1[D_YOMI]; i++){
	for(hopi = yri1[i].pter; hopi != RD_ENDPTR; hopi = hop->next[D_YOMI]){
	    hop = hopi + ri2;
	/* Though each hop has the same yomi, but calculate yomi each time. */
	    get_kanji_str(hop->kanjipter + kanji, kan, yomi, comm);
	    serial = hopi;
	    jeary[serial]->kanji = hop->kanjipter + kanji;
	    jeary[serial]->serial = serial;
	    w_stradd(yomi, &jeary[serial]->yomi);
	    w_stradd(kan, &jeary[serial]->kan);
	    w_stradd(comm, &jeary[serial++]->comm);
	}
    }
}


ghindo(argc , argv)
int argc;
char **argv;
{
    FILE *fp;
    struct wnn_file_uniq uniq;
    struct wnn_file_head file_head;
    struct HJT hjt;


    while(argc-- > 0){
	if((fp = fopen(*argv++ , "r"))== NULL){
	    fprintf(stderr , "can't open %s\n" , *(argv - 1));
	    perror("error in ghindo");
	    exit(1);
	}
	if(input_hindo_header(fp, &hjt, &file_head) == -1){
	    perror("error in ghindo 1");
	    exit(1);
	}
	if(bcmp(&uniq,&file_head.file_uniq,sizeof(struct wnn_file_uniq))){
	    fprintf(stderr, "%s(%s)Dic File and Hindo File mismatch.\n",
		    infile, *(argv - 1));
	    exit(1);
	}
	add_hindo(fp, hjt.maxserial);
    }
}

add_hindo(ifpter, num)
register FILE *ifpter;
int num;
{
    register int i;
    register int k;
    for(i = 0 ; i < num ; i++){
	jeary[i]->hindo += motoni2((k = getc(ifpter)) & 0x7f);
	if(k == -1)return(-1);
    }
  return(0);
}






