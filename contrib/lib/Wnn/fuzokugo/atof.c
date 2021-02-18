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
  Fuzokugo file convert command.
  Standard input UJIS format.
  Standard output jserver format.
  */
/*
  UJIS FORMAT

\attribute
{属性名}
{属性名}
..........
..........
\jiritugo-attr
{自立語品詞の属性定義}
{自立語品詞の属性定義}
..........
..........
\fuzokugo-id
{品詞の定義}
{品詞の定義}
..........
..........
\jiritugo
{自立語の品詞名}/{接続ベクトル}
{自立語の品詞名}/{接続ベクトル}
..........
..........
\fuzokugo
{同じ品詞の付属語の定義}
{同じ品詞の付属語の定義}
..........
..........
\syuutenv
\svkanren/{接続ベクトル}
\svkantan/{接続ベクトル}
\svbunsetsu/{接続ベクトル}
\bye


品詞の定義 :==
{品詞名} | 
{品詞名}@{属性名},{属性名},.....

自立語品詞の属性定義 :==
{自立語品詞名} | 
{自立語品詞名}@{属性名},{属性名},.....

注意）「自立語品詞の名前」は、品詞ファイルの中で定義されたものでないと
いけません。それに対し、「属性名」「品詞の名前」は、自分で勝手に名前を
定義してかまいません。


同じ品詞の付属語の定義 :==
\{付属語の品詞名}
{付属語の読み}/{コメント}/{接続ベクトル}
{付属語の読み}/{コメント}/{接続ベクトル}
..........
..........


接続ベクトル :==
{品詞名}:{品詞名}:{品詞名}:......

品詞名 :==
{品詞ファイルで定義された幹語品詞} |
{品詞の定義で定義された品詞名} |
@{品詞名}.{その品詞に属する付属語の読み} |
@{属性の定義で定義された属性} | 
@{属性の定義で定義された属性}&{属性の定義で定義された属性}&... |
@svkanren. | @svkantan. | @svbunsetsu.

*/



#include <stdio.h>
#include "commonhd.h"
#include "hinsi.h"
#include "jslib.h"

#define FUZOKUGO_LEN 8
#define OUT_INT_KOSUU 20
#define FUZOKUGO_KOSUU 600	/* fuzokugo kosuu */
#define ATTR_KOSUU 20
#define ID_HEIKIN_LEN 15
#define ID_MAX_LEN 128
#define SYUUTANV_KOSUU 3
#define F_COMMENT_LEN 80
#define ID_KOSUU (OUT_INT_KOSUU * sizeof(int) * 8)
/* fuzokugo hinsi kosuu */
#define HEAP_LEN (ID_KOSUU * ID_HEIKIN_LEN)

#define ATTR "\\attribute"
#define FUZOKUGO_ID "\\fuzokugo-id"
#define JIRITUGO_ATTR "\\jiritugo-attr"
#define JIRITUGO "\\jiritugo"
#define FUZOKUGO  "\\fuzokugo"
#define END_ID "\\bye"
#define SYUUTANV "\\syuutanv"
#define SV0 "\\svkanren"
#define SV1 "\\svkantan"
#define SV2 "\\svbunsetsu"

char *SV[] = {"svkanren","svkantan","svbunsetsu"};

int wnnerror;

#define TCSEP '.'
#define ATTR_CHAR '@'
#define ATTR_START '@'
#define ATTR_NEXT ','
#define ATTR_AND '&'

#define ESCAPE_CHAR '\\'
#define COMMENT_CHAR ';'
#define DEVIDE_CHAR '/'
#define IGNORE_CHAR1 ' '
#define IGNORE_CHAR2 '\t'

#define UJIS_TEN 0xa1a2  
#define UJIS_MARU 0xa1a3  

#define LINE_SIZE 1024

extern int mhinsi;

int line_no = 0;

char outfile[LINE_SIZE];
char *com_name;

char *hinsi_file_name = NULL;

struct attr_struct
{
  int id_set[OUT_INT_KOSUU];
  char *name;
} attr[ATTR_KOSUU];
int attr_num = 0;


struct id_struct
{int bit;
 char *str;
}id[ID_KOSUU];
int id_num = 0;
int	idn = 0;	/* 品詞の数  ADD KURI */

char heap[HEAP_LEN];
char *hp = heap;

struct vector
{int sv[OUT_INT_KOSUU];
 struct vector* tc; /* for including other fuzokugo entries */
 char *tc_name;
};

struct fuzokugo
{w_char y[FUZOKUGO_LEN + 1];
 char cy[FUZOKUGO_LEN * 2 + 1];
 int hinsi;
 char comment[F_COMMENT_LEN];
 struct vector vect;
}fz[FUZOKUGO_KOSUU];
int fz_num;

struct jiritugov
{
  struct vector v;
  int vnum;
}jiritugov[MAXHINSI];

int	actjv[MAXHINSI];	/* ADD KURI */

/* KURI
struct vector actjv[MAXHINSI];
*/
int mactjv;

int out_int_kosuu = 0;

struct vector syuutanv[SYUUTANV_KOSUU];

#define SIZE 1024
char buf[SIZE];

error_exit_long()
{
  error_format("Too Long Line.\n");
  exit(1);
}

error_format(s,d1,d2,d3,d4,d5)
char *s;
{
  fprintf(stderr, "Bad format near line %d \"%s\".\n", line_no,buf);
  fprintf(stderr, s, d1, d2, d3, d4, d5);
  exit(1);
}

error_eof()
{
  fprintf(stderr, "Error. File must end with %s\n",END_ID);
  exit(1);
}

error_no_heap()
{
  fprintf(stderr, "Error. Heap area is exhausted\n");
  fprintf(stderr, "Please compile it again.");
  exit(1);
}
  
error_tc(c)
char *c;
{
  fprintf(stderr, "No fuzokugo entry %s.\n",c);
  exit(1);
}

main(argc, argv)
int argc;
char **argv;
{
  int c;
  com_name = argv[0];
  init(argc, argv);
  read_attr();
  read_id();
  sort_id();
  out_int_kosuu = (idn - 1) / 32 + 1;
  read_jiritugov();
#ifdef nodef
  outputall();
#endif
  make_fuzokugo_table();
  read_syuutanv();
  set_tc_from_name();
  manipulate_tc();
  sort();
  check_fuzokugo();
  print_out();
  exit(0);
} 

/* KURI 
outputall()
{
  int d;
  for(;;){
    d = wnn_get_phrase(buf,SIZE);
    printf("%c\t%s\n",d,buf);
  }
}
*/


get_char0()
{
  int c,d;

  for(;(c = getchar()) == COMMENT_CHAR ||
      c == IGNORE_CHAR1 || c == IGNORE_CHAR2;){
      if(c == COMMENT_CHAR){
	  for(;;){
	      if((c = getchar())== EOF){
		  return(EOF);
	      }
	      if(c == '\n'){
	      ungetc(c,stdin);
	      break;
	    }
	  }
	}
      }
  if(c == '\n')line_no += 1;
  return(c);
}

get_char1() /* remove null lines "\\n"*/
{
  static int c = -1;
  int d;
  static int fufufu = 0;


  if(c != -1){
    d = c;
    c = -1;
    return(d);
  }else{
    if(fufufu == 0){  /* remove all new lines in the head of the file */
      for(;(d = get_char0()) == '\n';);
      fufufu = 1;
    }else{
      d = get_char0();
    }
    if(d == '\n'){
      while((c = get_char0()) == '\n');
    }
    return(d);
  }
}

get_char()
{

    static int hold = -1;    
    int c,d;

    if(hold != -1){
	c = hold;
	hold = -1;
	return(c);
    }else{
      start:
	c = get_char1();
	if(c == '\\'){
	    if((d = get_char1())== EOF){
		return(EOF);
	    }
	    if(d == '\n'){
		goto start;
	    }else{
		hold = d;
		return(c);
	    }
	}
	return(c);
    }
}


/* get one phrase and return the separater */
get_phrase(s0,size)
     char *s0;
     int size;
{
  char *s = s0;
  int c;
  static int eof = 0;

  if(eof)error_eof();	      

  while((c = get_char()) != '\n' && c != '/' && c != ':' && c != EOF){
    if(s - s0 >= size){
      error_exit_long();
    }
    *s++ = c;
  }
  if(c == EOF) eof = 1;
  if(s - s0 >= size - 1){
    error_exit_long();
  }
  *s++ = '\0';
  return(c);
}

read_attr()
{
  int k,j;

  if(get_phrase(buf,SIZE) != '\n' ||    
     strcmp (ATTR, buf) != 0){
    error_format("File must start with %s\n",ATTR);
  }

  for(attr_num = 0;;){
    if(get_phrase(buf,SIZE) != '\n'){
      error_format("Newline is expected\n");
    }
    if(strcmp(JIRITUGO_ATTR, buf) == 0) break;
    set_heap(&attr[attr_num].name, buf);
    if((++attr_num) >= ATTR_KOSUU){
      error_format("Too many ATTR's\n");
    }
  }

  for(k = 0 ; k < attr_num ; k++){
    for(j = 0 ; j < OUT_INT_KOSUU ; j++){
      attr[k].id_set[j] = 0;
    }
  }
} 
  
extern char *wnn_get_hinsi_name();

read_id()
{
  char *d;
  char buf1[SIZE];
/* KURI
  int idn = 0;
*/
  int n;

/*
 *  jiritugo
 */
  if (wnn_loadhinsi(hinsi_file_name) < 0)
     error_format("Cannot open hinsi.data\n");
  for(idn = 0 ; idn < mhinsi;idn++){
    if(d = wnn_get_hinsi_name(idn)){
      set_heap(&id[id_num].str, d);
      id[id_num++].bit = idn;
    }
    if(id_num >= ID_KOSUU){
      error_format("Too many ID's\n");
    }
  }
  idn = (idn + 31) & ~31;
/*
 *  set attribute to jiritugo
 */

  for(;;){
    get_phrase(buf,SIZE);
    if(strcmp(FUZOKUGO_ID, buf) == 0) break;
    get_id_part(buf1,buf);
    n = wnn_find_hinsi_by_name(buf1);
    if(n == -1)
      error_format("Hinsi which is not defined in Hinsifile.\n");
    strcpy(buf1,buf); /* in order to use it in check_attrs */
    check_attrs(n,buf1);
  }

/*
 *  fuzokugo
 */
  for(;;idn++){
    get_phrase(buf,SIZE);
    if(strcmp(JIRITUGO, buf) == 0) break;
    get_id_part(buf1,buf);
    set_heap(&(id[id_num].str), buf1);
    strcpy(buf1,buf); /* in order to use it in check_attrs */
    id[id_num].bit = idn;
    check_attrs(idn,buf1);
    if((++id_num) >= ID_KOSUU){
      error_format("Too many ID's\n");
    }
  }
}


sort_func_id(a,b)
char *a, *b;
{
  return(strcmp(((struct id_struct *)a)->str,((struct id_struct *)b)->str));
}

sort_id()
{
  qsort((char *)&id[0],id_num, sizeof(struct id_struct),sort_func_id);
}

/* s is subseq of t */
subseq(s,t)
     char *s,*t;
{
  while(*s){
    if(*s++ != *t++){
      return(-1); /* false */
    }
  }
  return(0); /* true */
}

make_fuzokugo_table()
{
  int id_num;
  char d;
  int i;
  w_char tmp[FUZOKUGO_LEN + 1];


  fz_num = 0;
  d = get_phrase(buf, SIZE); 
  if(strcmp (SYUUTANV, buf) == 0) return;
  if(*buf != '\\'){
    error_format("Fuzokugo_id is expected\n");
  }  
  if((id_num = find_id(buf + 1))< 0){
    error_format("Unknown identifier %s\n",buf + 1);
  }
  for(;fz_num < FUZOKUGO_KOSUU;){
    d = get_phrase(buf,SIZE); 
    if(strcmp (SYUUTANV, buf) == 0) return;
    if(*buf == '\\'){
      if((id_num = find_id(buf + 1))< 0){
	error_format("Unknown identifier %s\n",buf + 1);
      }
      continue;
    }
    if(d != '/'){
      error_format("'/' and definition of fuzokugo is expected.\n");
    }

    if(strlen(buf) >= FUZOKUGO_LEN * 2){
      error_format("Too long fuzokugo \"%s\"\n", buf);
    }
    
    strcpy(fz[fz_num].cy,buf);
    Sstrcpy(tmp, buf);
    Sreverse(fz[fz_num].y,tmp);
    fz[fz_num].hinsi = id_num;

    d = get_phrase(buf, SIZE); 
    if(strlen(buf) >= F_COMMENT_LEN){
      error_format("Too long comment\n");
    }
    strcpy(fz[fz_num].comment,buf);

    if(d != '/'){
      error_format("'/' is expected.\n");
    }
      
    for(i = 0 ; i < OUT_INT_KOSUU ; i++){
      fz[fz_num].vect.sv[i] = 0;
    }
    fz[fz_num].vect.tc = NULL;
    fz[fz_num].vect.tc_name = NULL;
    read_a_vector(&fz[fz_num].vect);
    fz_num += 1;
  }
}

find_id(c)
     char *c;
{
  int k;
  if((k = bsch(c,0,id_num - 1)) < 0){
    return(-1);
  }
  return(id[k].bit);
}

bsch(c,st,end)
     char *c;
     int st;
     int end;
{
  int tmp;
  int m = (st + end )/2;
  if(st >= end){
    if(strcmp(c,id[m].str) == 0){
      return(st);
    }else{
      return(-1);
    }
  }

  tmp = strcmp(c,id[m].str);
  if(tmp == 0)
    return(m);
  if(tmp < 0)
    return(bsch(c,st,m - 1));
  return(bsch(c,m + 1,end));
}

sort_func_fz(a,b)
char *a, *b;
{
  int c;

  if((c = Strcmp(((struct fuzokugo *)a)->y,((struct fuzokugo *)b)->y)) == 0){
    if(((struct fuzokugo*)a)->hinsi > ((struct fuzokugo *)b)->hinsi){
      return(1);
    }else if(((struct fuzokugo*)a)->hinsi < ((struct fuzokugo *)b)->hinsi){
      return(-1);
    }
    return(0);
  }
  return(c);
}

sort()
{
  qsort((char *)&fz[0],fz_num, sizeof(struct fuzokugo),sort_func_fz);
}

read_a_vector(vect)
struct vector *vect;
{
  int *v = vect->sv;
  int d;
  int id_num;
  int finish = 0;

  for(;finish == 0 && ((d = get_phrase(buf, SIZE)) == ':' || d == '\n');){
    if(d == '\n'){
      finish = 1;
    }
    if(*buf == '\0')continue;
    if(*buf == ATTR_CHAR){
      if(tcp(buf + 1)){
	set_heap(&vect->tc_name, buf + 1);
      }else{
	set_attribute(buf + 1, vect);
      }
    }else {
      if((id_num = find_id(buf))< 0){
	error_format("Unknown identifier in vector %s\n",buf);
      }
      v[id_num / (sizeof(int) * 8)] |= 
	1 << id_num % (sizeof(int) * 8); 
/*      1 << (((sizeof(int) * 8 - 1) - id_num % (sizeof(int) * 8))); */
    }
  }
  if(finish == 0){
    error_format("%c is not permitted here.\n", d);
  }
}

read_syuutanv()
{
  int d,k,j;
/*
  for(k = 0 ; k < SYUUTANV_KOSUU;k++){
    for(j = 0 ; j < OUT_INT_KOSUU ;j++){
      syuutanv[k].sv[j] = 0;
    }
    syuutanv[k].tc = NULL;
  }
*/
  for(;;){
    d = get_phrase(buf,SIZE);
    if(strcmp(buf, END_ID) == 0){
      break;
    }
    if(d != '/'){
      error_format("");
    }
    if(strcmp(buf, SV0) == 0){
      read_a_vector(&syuutanv[0]);
    }
    if(strcmp(buf, SV1) == 0){
      read_a_vector(&syuutanv[1]);
    }
    if(strcmp(buf, SV2) == 0){
      read_a_vector(&syuutanv[2]);
    }
  }
}

print_out()
{
  int k, j,i;
  int count = 0;
  char *c;
  FILE *ofpter;
  
  if((ofpter = fopen(outfile , "w")) == NULL){
      fprintf(stderr , "Can't open the output file %s.\n" , outfile);
      perror("");
      exit(1);
  }

  create_file_header(ofpter, WNN_FT_FUZOKUGO_FILE, NULL);

/* KURI
  for(k = 0; k < id_num; k++){
*/
  for(k = 0; k < idn; k++){
      if((k % 32) == 0){
	  fprintf(ofpter,"\n");
      }
      for(j = 0 ; j < id_num;j++){
	  if(id[j].bit == k){
	      fprintf (ofpter, ";%d\t%s\n",id[j].bit,id[j].str);
	      break;
	  }
      }
  }

/* KURI
   fprintf(ofpter,"\n%d %d %d\n",count_yomi(), out_int_kosuu, count_fz_num());
*/
  fprintf(ofpter,"\n%d ;付属語の個数\n", count_yomi());
  fprintf(ofpter,"%d ;接続ベクタの長さ\n", out_int_kosuu);
  fprintf(ofpter,"%d ;付属語ベクタの個数\n", count_fz_num());
  fprintf(ofpter,"%d ;幹語品詞のワード数\n",((mhinsi + 31 ) >> 5));
  fprintf(ofpter,"%d ;幹語ベクトルの数\n",mactjv);
  fprintf(ofpter,"%d ;幹語ベクトルの数\n",mhinsi);
  for(k = 0;(count = count_same_yomi(k)) > 0;){
      char tmp[FUZOKUGO_LEN * 2 + 1];
    if(fz[k].y[0] == 0){
	fprintf(ofpter,";%s\n", fz[k].comment);
      k++;
      continue;
    }
    sStrcpy(tmp, fz[k].y);
    fprintf(ofpter,"%s %d\n",tmp, count);
    for(; count > 0;k++){
      if(fz[k].y[0]){
	  fprintf(ofpter,"%4d ",fz[k].hinsi);
	  for(i = 0 ; i < out_int_kosuu; i++){
	      fprintf(ofpter,"%8x ", fz[k].vect.sv[i]);
	  }
	  fprintf(ofpter,";%s\n", fz[k].comment);
	count--;
      }else{
	  fprintf(ofpter,";%s\n", fz[k].comment);
      }
    }
  }
  fprintf(ofpter,"\n\n");
  fprintf(ofpter,";\n");
  fprintf(ofpter,";syuutan vectors\n");
  fprintf(ofpter,";\n");
  for(k = 0 ; k < SYUUTANV_KOSUU;k++){
    for(i = 0 ; i < out_int_kosuu; i++){
	fprintf(ofpter,"%8x ", syuutanv[k].sv[i]);
    }
    fprintf(ofpter,"\n");
  }
  fprintf(ofpter,"; 幹語接続ベクタNo.	幹語接続ベクタ\n");
  for(k = 0 ; k < mactjv; k++){
/*fprintf(ofpter,"%4d ",jiritugov[k].vnum); */
      fprintf(ofpter,"\n%4d ", k);	/* Change KURI */
    for(i = 0 ; i < out_int_kosuu; i++){
	fprintf(ofpter,"%8x ", jiritugov[k].v.sv[i]);
    }
  }
  fprintf(ofpter,"\n\n; 幹語接続ベクタ\n");	/* KURI */
  for(k = 0 ; k < mhinsi; k++){
    c = wnn_get_hinsi_name(k);
/* DEL KURI
    for(i = 0 ; i < mhinsi; i++){
      if(id[i].bit == k)break;
    }
*/
    if(c){
	fprintf(ofpter,"\t%d\t%d\t;%s\n",k, actjv[k], c); /* KURI */
    } else { /* KURI */
	fprintf(ofpter,"\t%d\t-1\t;未定義\n",k); /* KURI */
    }
  }
}

count_same_yomi(start)
     int start;
{
  int count = 1;
  w_char yomi[FUZOKUGO_LEN + 1];
  int i;

  if(start >= fz_num)return(0);
  Strcpy(yomi,fz[start].y);
  for(i = start + 1; i < fz_num;i++){
    if(fz[i].y[0]){
      if(Strcmp(yomi,fz[i].y) == 0){
	count += 1;
      }else{
	return(count);
      }
    }
  }
  return(count);
}

count_yomi()
{
  int k;
  w_char yomi[FUZOKUGO_LEN + 1];
  int count = 0;
  
  yomi[0] = 0;
  for(k = 0 ; k < fz_num;k++){
    if(fz[k].y[0]){
      if(Strcmp(yomi,fz[k].y)!=0){
	count += 1;
	Strcpy(yomi,fz[k].y);
      }
    }
  }
  return(count);
}

/* reverse for char as w_char */
#ifdef nodef
reverse(d,s)
char *d,*s;
{
  int k;
  int len = strlen(s);
  if(len % 2){
    error_format("length of yomi string is odd\n");
  }
  len /= 2;
  for(k = 0 ; k < len ; k++){
    d[k * 2] = s[(len - 1 - k) * 2];
    d[k * 2 + 1] = s[(len - 1 - k) * 2 + 1];
  }
  d[len * 2] = 0;
  d[len * 2 + 1] = 0;
}


w_str_cmp(a,b)
w_char *a, *b;
{
    w_char wa,wb;

  for(;*a != 0 && *b != 0;){
    wa = *a;
    wb = *b;
    if(wa != wb){
      return(w_char_cmp(wa,wb));
    }
  }
  if(*a == 0 && *b == 0)return(0);
  if(*a == 0)return(-1);
  return(1);

}
    
w_char_cmp(wa,wb)
     unsigned short wa,wb;
{
  if(wa == wb)return(0);
  if(wa == UJIS_TEN){
    if(wb == UJIS_MARU)return(-1);
    return(1);
  }
  if(wa == UJIS_MARU)return(1);
  if(wb == UJIS_MARU || wb == UJIS_TEN)return(-1);

  if(wb > wa)return(-1);
  return(1);
}
#endif

check_fuzokugo()
{
  int k;
  int point = 0;
  
  for(k = 1 ; k < fz_num;k++){
    if(Strcmp(fz[point].y,fz[k].y) == 0 &&
       fz[point].hinsi == fz[k].hinsi){
	fprintf(stderr, "Entries with same yomi \"%s\"and same hinsi \"%s\" are merged.\n",
		fz[k].cy, id[find_id_name(fz[k].hinsi)].str);
      bit_or(fz[point].vect.sv, fz[k].vect.sv);
      fz[k].y[0] = 0;
    }else{
      point = k;
    }
  }
}

bit_or(bit1,bit2)
int bit1[];
int bit2[];
{
  int i;
  for(i = 0 ; i < out_int_kosuu; i++){
    bit1[i] |= bit2[i];
  }
}

find_id_name(i)
int i;
{
  int k;
  for(k = 0 ; k < id_num; k++){
    if(id[k].bit == i){
      return(k);
    }
  }
  return -1;
}

count_fz_num()
{
  int k;
  int count = 0;
  for(k = 0 ; k < fz_num; k++){
    if(fz[k].y[0]){
      count +=1 ;
    }
  }
  return(count);
}

get_id_part(buf1,buf)
char *buf1;
char *buf;
{
  char *c = buf1;

  strcpy(buf1,buf);
  for(;*c;c++){
    if(*c == ATTR_START){
      *c = '\0';
      return;
    }
  }
  return;
}  

char *
attr_start(c)
char *c;
{
  for(;*c;c++){
    if(*c == ATTR_START){
      return(c + 1);
    }
  }
  return(NULL);
}

char *
get_attr_part(c,intp)
char *c;
int *intp;
{
  char tmp[ID_MAX_LEN];
  char *c1 = tmp;

  strcpy(tmp,c);
  for(;*c1;c1++){
    if(*c1 == ATTR_NEXT){
      *c1 = '\0';
      *intp = search_attr(tmp);
      return(c + 1 + (c1 - tmp));
    }
  }
  *intp = search_attr(tmp);
  return(NULL);
}

search_attr(c)
char *c;
{
  int k;

  for(k = 0 ; k < attr_num ; k++){
    if(strcmp(attr[k].name, c) == 0){
      return(k);
    }
  }
  error_format("Not defined attribute %s.\n",c);
  return -1;
}

check_attrs(id_n,c)
int id_n;
char *c;
{
  int attr_n;

  for(c = attr_start(c);c;){
    c = get_attr_part(c,&attr_n);
    set_id(attr_n,id_n);
  }
}

set_id(attr_n,id_n)
int attr_n,id_n;
{
  attr[attr_n].id_set[id_n / (sizeof(int) * 8)] |= (1 << id_n % (sizeof(int) * 8));
}

vector_or(sv,attrv)
int sv[];
int attrv[];
{
  int k;

  for(k = 0 ; k < OUT_INT_KOSUU ; k++){
    sv[k] |= attrv[k];
  }
}

vector_and(sv,attrv)
int sv[];
int attrv[];
{
  int k;

  for(k = 0 ; k < OUT_INT_KOSUU ; k++){
    sv[k] &= attrv[k];
  }
}

manipulate_tc_vector(vectp)
struct vector *vectp;
{
  if(vectp->tc){
    manipulate_tc_vector(vectp->tc);
    vector_or(vectp->sv, (vectp->tc)->sv);
    vectp->tc = NULL;
  }
}


manipulate_tc()
{
  int k;

  for(k = 0 ; k < fz_num ; k++){
    manipulate_tc_vector(&fz[k].vect);
  }
  for(k = 0 ; k < SYUUTANV_KOSUU; k++){
    manipulate_tc_vector(&syuutanv[k]);
  }
/* KURI
  for(k = 0 ; k < mhinsi; k++){
    manipulate_tc_vector(&jiritugov[k].v);
  }
*/
}

set_heap(cp,str)
char **cp;
char *str;
{
  int len = strlen(str);

  if(hp + len + 1 >= heap + HEAP_LEN){
    error_no_heap();
  }
  *cp = hp;
  strcpy(hp,str);
  hp += len + 1;
}
  
char svstr[] = "sv*";

struct vector *
find_tc_vect(c)
char *c;
{
  int k;
  char tmp[ID_MAX_LEN + FUZOKUGO_LEN + 1 ];
  w_char tmp1[FUZOKUGO_LEN + 1];
  w_char tmp2[FUZOKUGO_LEN + 1];
  int id_num;  
  char *cc;

  strcpy(tmp, c);
  for(cc = tmp; *cc; cc++){
    if(*cc == TCSEP){
      *cc = 0;
      cc += 1;
      break;
    }
  }


  if(strlen(tmp) == strlen(c)){
    error_tc(c);
  }

  for(k = 0 ; k < SYUUTANV_KOSUU; k++){
    if(strcmp(tmp,SV[k]) == 0){
      return(&syuutanv[k]);
    }
  }

  if((id_num = find_id(tmp)) < 0){
    error_tc(c);
  }    

  Sstrcpy(tmp1, cc);
  Sreverse(tmp2,tmp1);
  for(k = 0 ; k < fz_num; k++){
    if((Strcmp(fz[k].y,tmp2) == 0) &&
       (fz[k].hinsi == id_num)){
      return(&fz[k].vect);
    }
  }
  error_tc(c);
  return(NULL);
}

tcp(c)
char *c;
{
  char *c0 = c;
  int k;

  for(;*c;c++){
    if(*c == TCSEP)
      return(1);
  }
  return(0);
}

set_tc_from_name()
{
  int k;
  for(k = 0 ; k < fz_num;k++){
    if(fz[k].vect.tc_name){
      fz[k].vect.tc = find_tc_vect(fz[k].vect.tc_name);
    }
  }
  for(k = 0 ; k < SYUUTANV_KOSUU; k++){
    if(syuutanv[k].tc_name){
      syuutanv[k].tc = find_tc_vect(syuutanv[k].tc_name);
    }
  }
/* KURI
  for(k = 0 ; k < mhinsi; k++){
    if(jiritugov[k].v.tc_name){
      jiritugov[k].v.tc = find_tc_vect(jiritugov[k].v.tc_name);
    }
  }
*/
}

 
set_attribute(c, vect)
char *c;
struct vector *vect;
{

  int tmp_v[OUT_INT_KOSUU];
  char tmp[ID_MAX_LEN];
  char *d = tmp;
  int k;
  
  for(k = 0 ; k < OUT_INT_KOSUU; k++){
    tmp_v[k] = 0xffffffff;
  }

  for(;*c;c++){
    if(*c == ATTR_AND){
      *d = 0;
      vector_and(tmp_v, attr[search_attr(tmp)].id_set);
      d = tmp;
    }else{
      *d++ = *c;
    }
  }
  *d++ = 0;
  vector_and(tmp_v, attr[search_attr(tmp)].id_set);

  vector_or(vect->sv,tmp_v);
}

/* 自立語ベクタの読み込み */
read_jiritugov()
{
	char	d;
	register int	i,n;
	int	vno;	/* ADD KURI */

	for(mactjv = 0, i = 0; i < MAXHINSI; i++){	/* ADD KURI */
		d = get_phrase(buf, SIZE); 
		if(strcmp (FUZOKUGO, buf) == 0)	return;
		n = wnn_find_hinsi_by_name(buf);
		if(n == -1)
			error_format(
				"Hinsi which is not defined in Hinsifile.\n");
		if(d != '/'){
			error_format(
				"'/' and setuzoku vector is expected.\n");
		}
		pre_clear_jiritugo_v(mactjv);	/* ADD KURI */
		read_a_vector(&jiritugov[mactjv].v);
		if ((vno = is_same_jiritugov(mactjv)) == -1) {
			vno = mactjv;
			mactjv++;
		}
		actjv[n] = vno;
	}	/* ADD KURI */
}

/* ADD KURI */
/* 自立語ベクタ領域をクリアーする */
pre_clear_jiritugo_v(n)
int	n;
{
	int	i;

	for(i = 0; i < OUT_INT_KOSUU; i++)
		jiritugov[n].v.sv[i] = 0;
}

/* ADD KURI */
/* 同じ自立語ベクタあったか */
is_same_jiritugov(n)
int	n;
{
	register int	i, j;

	for (i = 0; i < n; i++) {
		for (j = 0; j < OUT_INT_KOSUU; j++) {
			if (jiritugov[i].v.sv[j] != jiritugov[n].v.sv[j])
				break;
		}
		if (j == OUT_INT_KOSUU)
			return (i);	/* 同じベクタ */
	}
	return (-1);	/* 同じものがなかった */
}

/* KURI
classify_jiritugov()
{
  int k,j;
  int mac;
  for(k = 0 ; k < mhinsi; k++){
    for(j = 0 ; j < mactjv; j++){
      if(eqv(jiritugov[k].v.sv,actjv[j].sv)){
	jiritugov[k].vnum = j;
	break;
      }
    }
    if(j == mactjv){
      vcp(actjv[mactjv++].sv, jiritugov[k].v.sv);
    }
  }
}

eqv(a,b)
int a[],b[];
{
  int k;
  for(k = 0 ; k < OUT_INT_KOSUU; k++){
    if(!(a[k] == b[k]))return(0);
  }
  return(1);
}

vcp(a,b)
     int a[],b[];
{
  int k;
  for(k = 0 ; k < OUT_INT_KOSUU; k++){
    a[k] = b[k];
  }
}
*/

init(argc, argv)
int argc;
char **argv;
{

  int c;
  extern int optind;
  extern char *optarg;

  while ((c = getopt(argc,argv,"h:")) != EOF) {
    switch(c){
    case 'h':
	hinsi_file_name = optarg;
	break;
    }
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

}
    
usage()
{
  fprintf(stderr , "Usage : %s [-h <hinsi filename>] <fzk.data filename>\n",com_name);
}
