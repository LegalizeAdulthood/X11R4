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
UJIS 形式を、辞書登録可能形式, 及び固定形式辞書に変換するプログラム。
*/

#include <stdio.h>
#include <ctype.h>
#include "commonhd.h"
#include "jslib.h"
#include "jh.h"
#include "jdata.h"

/* Switcher variable between UD and SD */

int which_dict = WNN_REV_DICT;

/* Variables both for UD and SD */
reverse_dict = NORMAL;

char *com_name;
int maxserial = MAX_ENTRIES;
int kanjicount = 0 ;
char outfile[LINE_SIZE];
struct wnn_file_head file_head;
struct JT jt;
char *hostart,*hoend;	/* index 2 */
int to_esc = 0;


/* For SD */
#define HONTAI_PER_ENTRY 20
int node_count = 0;
char *hopter;


/* For UD */
struct uind1 *tary;		/* index 1 */
int tnum = 0;
struct uind2 *uhopter;

char *malloc();
char *hinsi_file_name = NULL;


init(argc,argv)
     int argc;
     char **argv;
{
  extern int optind;
  extern char *optarg;
  int c;
  
  maxserial = MAX_ENTRIES;
  while ((c = getopt(argc,argv,"SURrs:P:p:Nneh:")) != EOF) {
    switch(c){
    case 'S': 
	which_dict = WNN_STATIC_DICT;
	break;
    case 'U': 
	which_dict = WNN_UD_DICT;
	break;
    case 'R':
	which_dict = WNN_REV_DICT;
	break;
    case 'r':
	reverse_dict = REVERSE;
	break;
    case 'P':
	get_pwd(optarg, file_head.file_passwd);
	break;
    case 'p':
	get_pwd(optarg, jt.hpasswd);
	break;
    case 'N':
	strcpy(file_head.file_passwd, "*");
	break;
    case 'n':
	strcpy(jt.hpasswd, "*");
	break;
    case 'e':
	to_esc = 1;
	break;
    case 'h':
	hinsi_file_name = optarg;
	break;
    case 's':
      if(sscanf(optarg , "%d" , &maxserial) == 0){
	usage();
	exit(1);
      }
      break;
    }
  }
  if(to_esc && which_dict == WNN_REV_DICT){
      fprintf(stderr, "You can't make the kanji component of reverse dictionary compact.\n");
      exit(1);
  }
  if (optind) {
    optind--;
    argc -= optind;
    argv += optind;
  }
  if(argc != 2){
    usage();
    exit(1);
  }
  strcpy(outfile , argv[1]);
  wnn_loadhinsi(hinsi_file_name);

  if(init_heap(DEF_ENTRIES * HEAP_PER_LINE,
	       DEF_ENTRIES * YOMI_PER_LINE,
	       maxserial, DEF_ENTRIES, stdin) == -1)
      exit(1);
}

alloc_area()
{
  if(which_dict == WNN_STATIC_DICT){
      if((hostart = (char *)malloc(maxserial * HONTAI_PER_ENTRY)) == NULL){
	  fprintf(stderr, "Malloc Failed\n");
	  exit(1);
      }
      hopter = hostart;
      hoend = (char *)hostart + maxserial * HONTAI_PER_ENTRY;
  }else {
      if((tary = (struct uind1 *)malloc(jt.maxserial * sizeof(struct uind1))) == NULL ||
                                /* Too large? */
	 (hostart =(char *)malloc(jt.maxserial *
		      (sizeof(struct uind2) + sizeof(w_char) * 2))) == NULL){
                                /* 2 must be enough? */
	  fprintf(stderr, "Malloc Failed\n");
	  exit(1);
      }
      hoend = (char *)((char *)hostart +
	       jt.maxserial * (sizeof(struct uind2) + sizeof(w_char) * 2));
  }
}

extern int sort_func_sdic();
extern int sort_func_je();
FILE *ofpter , *fopen();

main(argc,argv)
     int argc ;
     char **argv;
{
  com_name = argv[0];
  init(argc , argv);

  ujis_header();
  read_ujis(reverse_dict, to_esc, (which_dict == WNN_REV_DICT)? 1:0);
  reverse_yomi();

  if((ofpter = fopen(outfile , "w")) == NULL){
      fprintf(stderr , "Can't open the output file %s.\n" , outfile);
      perror("");
      exit(1);
  }

  if(which_dict == WNN_REV_DICT){
      create_rev_dict();
  }else{
      alloc_area();
      if(which_dict == WNN_STATIC_DICT){
	  sdic_sort();
	  uniq_je(sort_func_sdic);
	  output_dic_data();
	  ujistosd(0,0);
      }else{
	  sort_if_not_sorted();
	  uniq_je(sort_func_je);
	  output_dic_data();
	  ujistoud();
	  set_pter1();
      }
  }
  output_dic_index();
  rewind(ofpter);
  output_header(ofpter, &jt, &file_head); /* dummy */
  exit (0);
}


w_char *
addyomient(tn , yomi)
register int tn;
register w_char *yomi;
{

  int len = Strlen(yomi);
  tary[tn].yomi2 = 0;
  tary[tn].yomi1 = yomi[0] << 16;

  uhopter->yomi[0] = len;
  if(yomi[1]){
    tary[tn].yomi1 |= yomi[1];
    if(yomi[2]){
      tary[tn].yomi2 = yomi[2] << 16;
      if(yomi[3]){
	tary[tn].yomi2 |= yomi[3];
      }
      if(len > 4){
	Strncpy(uhopter->yomi + 1, yomi + 4,len - 4);
	return(uhopter->yomi + 1 + len - 4);
      }
    }
  }
  return(uhopter->yomi + 1);
}

ujistoud()
{
  register w_char  *yomi;
  w_char dummy = 0; 	/* 2 byte yomi */
  w_char *pyomi;		/* maeno tangono yomi */
  w_char *wcp;
  register int serial_count;
  w_char *kosuup;

  *(int *)hostart = 0;
  uhopter = (struct uind2 *)((int *)hostart + 1);

  yomi = &dummy;

  for(serial_count = 0 ;serial_count < jt.maxserial; serial_count++){
    pyomi = yomi;
    yomi = jeary[serial_count]->yomi;

    if(Strcmp(yomi, pyomi)){
      tary[tnum++].pter = (char *)uhopter - (char *)hostart;
      uhopter->next = 0;
      uhopter->serial = serial_count;
      uhopter->kanjipter = kanjicount;
      uhopter->kosuu = 1;
      kosuup = &uhopter->kosuu;
      wcp = addyomient(tnum - 1 , yomi);
      uhopter = (struct uind2 *)(AL_INT(wcp));
    }else{
      *kosuup += 1;
    }
    upd_kanjicount(serial_count);
  }
}

upd_kanjicount(k)
int k;
{
    kanjicount += *jeary[k]->kanji;
}

set_pter1()
{
  register int k;
  register int len;
  w_char oyomi[LENGTHYOMI],nyomi[LENGTHYOMI];
		/* May be a little slow, but simple! */
  int lasts[LENGTHYOMI];			/* pter_to */

  for(k = 0 ; k < LENGTHYOMI; k++){
    lasts[k] = -1;
  }

  for(k = 0 ; k < tnum ; k++){
    len = ((struct uind2 *)((char *)hostart + tary[k].pter))->yomi[0];
    lasts[len] = k;
    for(len--;len ; len--){
      if(lasts[len] >= 0){
	udytoS(oyomi,lasts[len],hostart,tary);
	udytoS(nyomi,k,hostart,tary);
	if(Substr(oyomi,nyomi)){
	  tary[k].pter1 = lasts[len];
	  break;
	}
      }
    }
    if(len == 0) tary[k].pter1 = -1;
  }

}


output_dic_data()
{

    fprintf(stderr , "%d words are in this dictionary\n" , jt.maxserial);

    jt.syurui = which_dict;
    jt.gosuu = jt.maxserial;
    output_header(ofpter, &jt, &file_head); /* dummy */
    output_comment(ofpter);
    output_hinsi_list(ofpter);
    output_hindo(ofpter);
    output_hinsi(ofpter);
    output_kanji(ofpter);
}

output_dic_index()
{
    if(which_dict == WNN_UD_DICT){
	fprintf(stderr, " tnum = %d\n ind2= %d\n kanji = %d\n",tnum,(char *)uhopter - (char *)hostart,kanjicount);
	jt.maxtable = tnum;
	jt.maxhontai = (char *)uhopter - (char *)hostart;
	jt.maxri2 = jt.maxri1[D_YOMI] = jt.maxri1[D_KANJI] = 0;
    }else if(which_dict == WNN_REV_DICT){
	fprintf(stderr, "kanji = %d\n",kanjicount);
	jt.maxtable = 0;
	jt.maxhontai = 0;
	jt.maxri2 = jt.maxserial;
    }else{
	fprintf(stderr, "node_count = %d  ind= %d\n kanji = %d\n",node_count,(char *)hopter - (char *)hostart,kanjicount);
	jt.maxtable = 0;
	jt.maxhontai = (char *)hopter - (char *)hostart;
	jt.maxri2 = jt.maxri1[D_YOMI] = jt.maxri1[D_KANJI] = 0;
    }
    jt.maxkanji = kanjicount;

    if(little_endian()){
	revdic(&jt, 1);
/*	revdic_jt(&jt, 1, hostart, tary); */
    }
    if(which_dict == WNN_UD_DICT){
	fwrite((char *)tary , sizeof(struct uind1), tnum , ofpter);
	fwrite(hostart , 1 , (char *)uhopter - (char *)hostart , ofpter);
    }else if(which_dict == WNN_REV_DICT){
	output_ri(ofpter);
    }else{
	fwrite(hostart , 1 , (char *)hopter - (char *)hostart , ofpter);
    }
}

output_kanji(ofpter)
register FILE *ofpter;
{
    register int len;
    register int i;

    for(i = 0 ; i < jt.maxserial ; i++){
	fwrite(jeary[i]->kanji, 1, *(jeary[i]->kanji) , ofpter);
    }
    return(0);
}


usage()
{
    fprintf(stderr , "Usage : %s [-r -R -S -e -s maximum word count(default %d) -P passwd (or -N) -p hindo_passwd (or -n) -h hinsi_file_name] <dictonary filename>\n",com_name ,MAX_ENTRIES );
  fprintf(stderr , "Input the ascii dictionary from stdin\n" );
  fprintf(stderr , "-r is for creating dictionary with normal and reverse index\n");
  fprintf(stderr , "-r is for creating reverse dictionary\n");
  fprintf(stderr , "-S is for creating static  dictionary.\n");
  fprintf(stderr , "-e is for compacting kanji string.\n");
  exit1();
}

/* SD commands */

yStrcmp(a,b)
register w_char *a,*b;
{
  register int c,d;
  for(;*a && *a == *b; a++, b++);
  if(*a == 0) return(- (int) *b);
  if(*b == 0) return((int) *a);
  c = *(a + 1);
  d = *(b + 1);
  if(c == 0 && d == 0) return((int)*a - (int)*b);
  if(c == 0) return(-1);
  if(d == 0) return(1);
  return((int)*a - (int)*b);
}

sort_func_sdic(a,b)
char *a, *b;
{
    register int tmp;
    register struct je *pa, *pb;
    pa = *((struct je **)a);
    pb = *((struct je **)b);
    tmp = yStrcmp(pa->yomi,pb->yomi);
    if(tmp)return(tmp);
    /* Changed the order to sort, from yomi->kanji->hinsi 
     * to yomi->hinsi->kanji, in order to separate (struct jdata) by
     * hinsi'es.
     */
    if(pa->hinsi != pb->hinsi) return ((int)(pa->hinsi) - (int)(pb->hinsi));
    return(Strcmp(pa->kan,pb->kan));
}


sdic_sort()
{
    if(!Sorted((char *)jeary,(int)jt.maxserial, sizeof(struct je*),sort_func_sdic)){
   qsort((char *)jeary,(int)jt.maxserial, sizeof(struct je*),sort_func_sdic);
    }
}


static w_char chartable[YOMI_KINDS];
static w_char numtable1[YOMI_KINDS] ;
static w_char *numtable = numtable1 + 1;
static int endt = 0;

analize_size(start_je,level,statep,end_jep,mid_jep)
int start_je,level;
int *statep,*end_jep,*mid_jep;
{
  w_char *c = jeary[start_je]->yomi;
  register int je;
  int level1 = level + 1;
  int end_je,mid_je;
  register w_char y1,y2;
  register int je1,je2;

  for(;endt-- > 0 ;){
    chartable[endt] = numtable[endt] = 0;
  }
  endt = 0;		/* This line is needed when endt is originally 0*/
  for(je = start_je;je < jt.maxserial && Strncmp(c,jeary[je]->yomi,level) == 0 &&
                    jeary[je]->yomi[level1] == 0;je++);
  *mid_jep = mid_je = je;
  for(je = start_je;je < jt.maxserial && Strncmp(c,jeary[je]->yomi,level) == 0;je++);
  *end_jep = end_je = je;

  je1 = start_je;
  je2 = mid_je;
  y1 = (je1 >= mid_je)? 0xffff: jeary[je1]->yomi[level];
  y2 = (je2 >= end_je)? 0xffff: jeary[je2]->yomi[level];
  for(;;){
    if(y1 > y2){
      chartable[endt] = y2;
      for(numtable[endt] = numtable[endt - 1];
	  je2 < end_je && jeary[je2]->yomi[level] == y2;je2++);
      y2 = (je2 >= end_je)? 0xffff: jeary[je2]->yomi[level];
      endt++;
    }else if(y1 < y2){
      chartable[endt] = y1;
      for(numtable[endt] = numtable[endt - 1];
	  je1 < end_je && jeary[je1]->yomi[level] == y1;je1++,numtable[endt]++);
      y1 = (je1 >= mid_je)? 0xffff: jeary[je1]->yomi[level];
      endt++;
    }else{
      if(y1 >= 0xffff && y2 >= 0xffff)break;
      chartable[endt] = y1;
      for(;je2 < end_je &&jeary[je2]->yomi[level] == y2;je2++);
      for(numtable[endt] = numtable[endt - 1];
	  je1 < mid_je && jeary[je1]->yomi[level] == y1;je1++,numtable[endt]++);
      y2 = (je2 >= end_je)? 0xffff: jeary[je2]->yomi[level];
      y1 = (je1 >= mid_je)? 0xffff: jeary[je1]->yomi[level];
      endt++;
    }      
  }
  if(mid_je == start_je){
    if(endt == 1) *statep = ST_SMALL;
    else *statep = ST_NOENT;
  }else if(mid_je == end_je) *statep = ST_NOPTER;
  else *statep = ST_NORMAL;		

  return(endt);
}  



ujistosd(start_je,level)
int start_je;
int level;
{
  int state;
  int tsize;
  w_char *charst;
  w_char *sumst;
  int *ptrst;
  int mid_je, end_je;
  register int je;
  register int k;

  node_count++;

  tsize = analize_size(start_je,level,&state,&end_je,&mid_je);

  *(w_char *)hopter = state;
  hopter += 2;
  switch(state){
  case ST_NORMAL:
    if(hopter + tsize * 8 + 12 + 16 >= hoend) not_enough_area();
    *(w_char *)hopter = tsize;
    hopter += 2;
    *(int *)hopter = start_je;
    hopter += 4;
    *(int *)hopter = kanjicount;
    hopter += 4;
    charst = (w_char *)hopter;
    sumst = ((w_char *)hopter + tsize + 2); /* + 2 keeps two zero words */
    charst[tsize] = charst[tsize + 1] = 0;
    ptrst = (int *)((w_char *)sumst + tsize);
    hopter = (char *)(ptrst + tsize);
    for(k = 0;k < tsize;k++){
      charst[k] = chartable[k];
      sumst[k] = numtable[k];
    }      
    for(k = start_je;k < mid_je; k++){
	upd_kanjicount(k);
    }
    for(je = mid_je,k = 0; je < end_je;){
      for(;k < tsize && charst[k] < jeary[je]->yomi[level];k++)ptrst[k] = ENDPTR;
      if(k == tsize)fprintf(stderr, "Error\n");
      ptrst[k] = (char *)hopter - (char *)hostart;
      ujistosd(je,level + 1);
      for(;je < end_je && jeary[je]->yomi[level] == charst[k];je++);
      k++;
    }
    break;
  case ST_NOENT:
    if(hopter + tsize * 8 + 4 + 16 >= hoend) not_enough_area();
    *(w_char *)hopter = tsize;
    hopter += 2;
    charst = (w_char *)hopter;
    ptrst = (int *)AL_INT((w_char *)charst + tsize);
    hopter = (char *)(ptrst + tsize);
    for(k = 0;k < tsize;k++){
      charst[k] = chartable[k];
    }      
    for(je = mid_je,k = 0; je < end_je;){
      for(;k < tsize && charst[k] < (jeary[je]->yomi)[level];k++)ptrst[k] = ENDPTR;
      if(k == tsize)fprintf(stderr, "Error\n");
      ptrst[k] = (char *)hopter - (char *)hostart;
      ujistosd(je,level + 1);
      for(;je < end_je && (jeary[je]->yomi)[level] == charst[k];je++);
      k++;
    }
    break;
  case ST_NOPTER:
    if(hopter + tsize * 4 + 12 + 16 >= hoend) not_enough_area();
    *(w_char *)hopter = tsize;
    hopter += 2;
    *(int *)hopter = start_je;
    hopter += 4;
    *(int *)hopter = kanjicount;
    hopter += 4;
    charst = (w_char *)hopter;
    sumst = ((w_char *)hopter + tsize + 2); /* + 2 keeps two zero words */
    charst[tsize] = charst[tsize + 1] = 0;
    hopter = (char *)((w_char *)sumst + tsize);
    for(k = 0;k < tsize;k++){
      charst[k] = chartable[k];
      sumst[k] = numtable[k];
    }      
    for(k = start_je;k < mid_je; k++){
	upd_kanjicount(k);
    }
    break;
  case ST_SMALL:
    if(hopter + 4 + 16 >= hoend) not_enough_area();
    *(w_char *)hopter = chartable[0];
    hopter += 2;
    ujistosd(start_je,level + 1);
    break;
  }
}

#define INCR_HO 150

not_enough_area()
{
    char *oldstart = hostart;
    int size = hoend - hostart + INCR_HO * HONTAI_PER_ENTRY;
  fprintf(stderr, "Sorry allocated area is exhausted.node_count = %d\n",node_count);
  fprintf(stderr, "Retry with option -S <dict_size>.\n");
  exit(1);


/*
 * Only for Sdic 
 * Can't Realloc!! many pointeres exist on the stack!!
 */

/*
    fprintf(stderr, "Realloc.\n");
    if((hostart = (char *)realloc(hostart, size)) == NULL){
	fprintf(stderr, "Ralloc Failed\n");
	exit(1);
    }
    hopter = hostart + (hopter - oldstart);
    hoend = (char *)((char *)hostart + size);
*/    
}


get_pwd(fname, cripted)
char *fname, *cripted;
{
    char pwd[WNN_PASSWD_LEN];
    FILE *fp;

    if((fp = fopen(fname, "r")) == NULL){
	fprintf(stderr, "No password_file %s.\n", fname);
	exit(1);
    }
    fgets(pwd, WNN_PASSWD_LEN, fp);
    fclose(fp);
    new_pwd(pwd, cripted);
}
    
