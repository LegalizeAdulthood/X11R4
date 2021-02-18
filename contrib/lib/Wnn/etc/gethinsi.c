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
/*      品詞ファイルの構造に関する定義
*/

#include <stdio.h>
#include "commonhd.h"
#include "config.h"
#include "wnnerror.h"
#include "jslib.h"
#include "hinsi_file.h"
    
    /*
      wnn_loadhinsi(NULL)			 品詞の情報を読み込む 
      
      wnn_find_hinsi_by_name(c)		 名前を与えて、品詞番号を取る 
      char *c;
      
      char *wnn_get_hinsi_name(k)	 品詞番号から名前を取る 
      int k;
      
      int
      wnn_get_fukugou_component(k,str, )  複合品詞に対して 構成要素を求める 
      int k;			      構成要素の個数が返値として返され、
      unsigned short **str;	      構成要素は str 以降に返される。
      
#ifdef nodef
      int wnn_get_hinsi_list(area)  品詞のリストを得る。
      品詞は、品詞名の配列として管理されている。
      配列の先頭番地を area に入れ、配列の大きさを返す。
      wchar ***area;

      int wnn_get_fukugou_list(area, start) 複合品詞のリストを得る
      複合品詞は、複合品詞構造体の配列として管理されている。
      配列の先頭番地を area に入れ、配列の大きさを返す。
      n 番目の複合品詞の品詞番号は、FUKUGOU_START - n である。
      FUKUGOU_START の値を start に入れて返す。
      struct wnn_fukugou **area;
      int *start;
      int wnn_hinsi_node_component(name, area)
      wchar **area;
      wchar *name;
      品詞ノード名から、その子どものノードの名前の列を得る。
      個数が返る。
      もし、ノードではなく本当の品詞名(リーフ)なら、0 が返る。
      ノードでも品詞名でもないとき、-1 が返る。
#endif
      */
    
    extern int wnn_errorno;

/*
 *here start the real program 
 */


int wnnerror_hinsi;

static int hinsi_loaded = 0;

static int line_no = 0;

static wchar heap[HEAP_LEN];
static wchar *hp = heap;

static unsigned short wheap[WHEAP_LEN];
static unsigned short *whp = wheap;

#define SIZE 1024

static wchar *hinsi[MAXHINSI];
static struct wnn_fukugou fukugou[MAXFUKUGOU];
static struct wnn_hinsi_node node[MAXNODE];

int mhinsi = 0;
int mfukugou = 0;
int mnode = 0;

static
    error_long()
{
    wnnerror_hinsi = WNN_TOO_LONG_HINSI_FILE_LINE;
}

static
    error_no_heap()
{
    wnnerror_hinsi = WNN_TOO_BIG_HINSI_FILE;
}

static
    get_char0(fp)
FILE *fp;
{
    int c,d;
    
    for(;(c = getc(fp)) == COMMENT_CHAR || c == CONTINUE_CHAR ||
	c == IGNORE_CHAR1 || c == IGNORE_CHAR2;){
	if(c == CONTINUE_CHAR){
	    if((d = getc(fp))== EOF){
		break;
	    }
	    if(d == '\n'){
		line_no += 1;
		continue;
	    }else{
		ungetc(d,fp);
		break;
	    }
	}else if(c == COMMENT_CHAR){
	    for(;;){
		if((c = getc(fp))== EOF){
		    return(EOF);
		}
		if(c == '\n'){
		    ungetc(c,fp);
		    line_no += 1;
		    break;
		}
	    }
	}
    }
    if(c == '\n')line_no += 1;
    return(c);
}

static
    get_char(fp) /* remove null lines */
FILE *fp;
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
	    for(;(d = get_char0(fp)) == '\n';);
	    fufufu = 1;
	}else{
	    d = get_char0(fp);
	}
	if(d == '\n'){
	    while((c = get_char0(fp)) == '\n');
	}
	return(d);
    }
}

/* get one phrase and return the separater */
static
get_phrase(s0,size,fp)
UCHAR *s0;
int size;
FILE *fp;
{
    UCHAR *s = s0;
    int c;
    static int eof = 0;
    
    if(eof){
	*s0 = 0;
	return(EOF);
    }
    while((c = get_char(fp)) != '\n' && c != DEVIDE_CHAR && c != NODE_CHAR &&
	  c != HINSI_SEPARATE_CHAR && c != EOF){
	if(s - s0 >= size){
	    error_long();
	    return(ERR);
	}
	*s++ = c;
    }
    if(c == EOF) eof = 1;
    if(s - s0 >= size - 1){
	error_long();
	return(ERR);
    }
    *s++ = '\0';
    return(c);
}

static
    stradd(cp,str)
wchar **cp;
char *str;
{
    int len = strlen(str);
    
    if(hp + len + 1 >= heap + HEAP_LEN){
	error_no_heap();
	return(-1);
    }
    *cp = hp;
    Sstrcpy(hp,str);
    hp += Strlen(hp) + 1;
    return(0);
}

static
    w_stradd(cp,str)
unsigned short **cp;
unsigned short *str;
{
    
    *cp = whp;
    for(;*str != TERMINATE; str++,whp++){
	if(whp >= wheap + WHEAP_LEN){
	    error_no_heap();
	    return(-1);
	}
	*whp = *str;
    }
    *whp++ = TERMINATE;
    return(0);
}

wnn_loadhinsi(fname)
unsigned char *fname;
{
    FILE *fp;
    UCHAR buf[SIZE];
    unsigned short fukugou_str[MAXHINSI];
    int sep;
    int h;
    unsigned short *c;
    
    if(fname == NULL){
#ifdef	JSERVER
	if(hinsi_loaded) return(0);
#endif	JSERVER
	fname = (unsigned char *)HINSIDATA_FILE;
    }
    hinsi_loaded = 1;
    if((fp = fopen(fname, "r")) == NULL){
	wnnerror_hinsi = WNN_NO_HINSI_DATA_FILE;
	return(ERR);
    }
    while((sep = get_phrase(buf,SIZE,fp)) != EOF){
	if(sep == ERR){
	    return(ERR);		/* wnnerror_hinsi set in get_phrase */
	}
	if(buf[0] == YOYAKU_CHAR){	/* yoyaku */
	    if(sep != '\n'){
		wnnerror_hinsi = WNN_BAD_HINSI_FILE;
		return(ERR);
	    }
	    hinsi[mhinsi++] = NULL;
	}else if(sep == '\n'){	/* hinsi */
	    if(stradd(&hinsi[mhinsi++],buf))return(ERR);
	}else if(sep == DEVIDE_CHAR){	/* fukugou */
	    if(stradd(&fukugou[mfukugou].name,buf))return(ERR);
	    c = fukugou_str;
	    while((sep = get_phrase(buf,SIZE,fp)) != EOF){
		if(sep == -1){
		    return(ERR);		/* wnnerror_hinsi set in get_phrase */
		}
		if(sep != EOF && sep != HINSI_SEPARATE_CHAR && sep != '\n'){
		    wnnerror_hinsi = WNN_BAD_HINSI_FILE;
		    return(ERR);
		}
		if((h = wnn_find_hinsi_by_name(buf)) == -1 ||
		   h >= mhinsi){
		    wnnerror_hinsi = WNN_BAD_HINSI_FILE;
		    return(ERR);
		}
		*c++ = h;
		if(sep == '\n' || sep == EOF)
		    break;
	    }
	    *c = TERMINATE;
	    if(w_stradd(&fukugou[mfukugou++].component,fukugou_str))return(ERR);
	}else if(sep == NODE_CHAR){
	    int first = 1;
	    wchar *dummy;

	    node[mnode].kosuu = 0;
	    if(stradd(&node[mnode].name,buf))return(ERR);
	    while((sep = get_phrase(buf,SIZE,fp)) != EOF){
		if(sep == -1){
		    return(ERR);		/* wnnerror_hinsi set in get_phrase */
		}
		if(sep != EOF && sep != HINSI_SEPARATE_CHAR && sep != '\n'){
		    wnnerror_hinsi = WNN_BAD_HINSI_FILE;
		    return(ERR);
		}
		node[mnode].kosuu++;
		if(first){
		    if(stradd(&node[mnode].son,buf))return(ERR);
		    first = 0;
		}else{
		    if(stradd(&dummy,buf))return(ERR);
		}
		if(sep == '\n' || sep == EOF)
		    break;
	    }
	    mnode++;
	}
    }
    return(0);
}

find_hinsi_by_name(c)
register wchar *c;
{
    register int k;
    if(!hinsi_loaded)wnn_loadhinsi(NULL);
    
    for(k = 0 ; k < mhinsi; k++){
	if(hinsi[k] && Strcmp(hinsi[k],c) == 0){
	    return(k);
	}
    }
    for(k = 0 ; k < mfukugou; k++){
	if(fukugou[k].name && Strcmp(fukugou[k].name,c) == 0){
	    return(FUKUGOU_START - k);
	}
    }
    return(-1);
}


wnn_find_hinsi_by_name(c)
register char *c;
{
    wchar hin[WNN_HINSI_NAME_LEN];

    Sstrcpy(hin, c);
    return(find_hinsi_by_name(hin));
}


wchar *
get_hinsi_name(k)
int k;
{
    if(!hinsi_loaded)wnn_loadhinsi(NULL);
    if(k < mhinsi && k >= 0){
	return(hinsi[k]);
    }else if(k > FUKUGOU_START - mfukugou){
	return(fukugou[FUKUGOU_START - k].name);
    }
    return(NULL);
}

char *
wnn_get_hinsi_name(k)
int k;
{
    wchar *s;
    static char hin[WNN_HINSI_NAME_LEN * 2];

    if((s = get_hinsi_name(k)) == NULL)return(NULL);
    sStrcpy(hin, s);
    return(hin);
}

#ifndef	JSERVER
static
#endif	JSERVER
wnn_get_fukugou_component_body(k,shp)
register int k;
register unsigned short **shp;
{
    static unsigned short tmp;    
    register unsigned short *s;
    if(k < mhinsi && k >= 0){
	tmp = k;
	*shp = &tmp;
	return(1);
    }
    if(k > FUKUGOU_START - mfukugou && k <= FUKUGOU_START){
	for(*shp = s = fukugou[FUKUGOU_START - k].component;*s != TERMINATE;s++);
	return(s - *shp);
    }
    return(-1);
}

wnn_get_fukugou_component(k,shp)
register int k;
register unsigned short **shp;
{
    if(!hinsi_loaded)wnn_loadhinsi(NULL);
    wnn_get_fukugou_component_body(k, shp);
}


#ifdef JSERVER

wchar *
wnn_hinsi_name(no)
int no;
{
    wchar *c;
    if((c = get_hinsi_name(no)) == NULL){
	wnn_errorno = WNN_BAD_HINSI_NO;
    }
    return(c);
}

wnn_hinsi_number(name)
wchar *name;
{
    int n;
    if((n = find_hinsi_by_name(name)) == -1){
	wnn_errorno = WNN_BAD_HINSI_NAME;
    }
    return(n);
}

wnn_hinsi_list(name, c, mynode, mmynode)
wchar *name;
wchar **c;
struct wnn_hinsi_node *mynode;
int mmynode;
{
    int k;

    if(mynode == NULL){
	mynode = node;
	mmynode = mnode;
    }
    if(!hinsi_loaded)wnn_loadhinsi(NULL);
    for(k = 0 ; k < mmynode; k++){
	if(Strcmp(name, mynode[k].name) == 0){
	    *c = mynode[k].son;
	    return(mynode[k].kosuu);
	}
    }
    if(find_hinsi_by_name(name) == -1){
	wnn_errorno = WNN_BAD_HINSI_NAME;
	return(-1);
    }
    return(0);
}

wnn_has_hinsi(mynode, mmynode, name)
struct wnn_hinsi_node *mynode;
int mmynode;
w_char *name;
{
    w_char *c;
    int k, j;
    if(mynode == NULL){
	mynode = node;
	mmynode = mnode;
    }
    for(k = 0 ; k < mmynode; k++){
	if(Strcmp(name, mynode[k].name) == 0){
	    return(1);
	}else{
	    c =  mynode[k].son;
	    for(j = 0 ; j < mynode[k].kosuu; j++){
		if(Strcmp(name, c) == 0){
		    return(1);
		}else{
		    c += Strlen(c) + 1;
		}
	    }
	}
    }
    return(0);
}

#endif
