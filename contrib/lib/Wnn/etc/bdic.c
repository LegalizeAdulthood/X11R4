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
  Binary (Updatable, Stable) dictionary common routine.
*/


#ifdef UX386
#include <X11/Xos.h>
#else
#include <sys/types.h>
#endif
#ifndef JS
#include <sys/stat.h>
#include <stdio.h>
#include "commonhd.h"
#include "jslib.h"
#include "jh.h"
#endif
#include "jdata.h"
#ifdef SYSVR2
#include "string.h"
#endif
#ifdef BSD42
#include "strings.h"
#endif
#ifdef JSERVER
#define vputc(X, pt) ((pt)? putc((X), (pt)):xputc_cur(X))
#define vgetc(pt) ((pt)? getc((pt)):xgetc_cur())
#else
#define vputc(X, pt) putc((X), (pt))
#define vgetc(pt) getc(pt)
#endif

#ifndef min
#define min(a, b) ((a > b)? b:a)
#define max(a, b) ((a < b)? b:a)
#endif


#ifdef JS
static
#endif
putnull(ofpter, n)
register FILE *ofpter;
register int n;
{
    for(;n;n--){
	vputc('\0', ofpter);
    }
}

#ifdef JS
static
#endif
put_n_str(ofpter, c, n)
register FILE *ofpter;
register int n;
register char *c;
{
    for(;n;n--){
	vputc(*c++, ofpter);
    }
}  

#ifdef JS
static
#endif
put_n_EU_str(ofpter, c, n)
register FILE *ofpter;
register w_char *c;
register int n;
{
    for(;n;n--){
	put_short(ofpter, (int)(*c++));
    }
}

#ifdef JS
static
#endif
put_short(ofpter, i)
register FILE *ofpter;
register int i;
{
    vputc(i >> 8, ofpter);
    vputc(i     , ofpter);
}

#ifdef JS
static
#endif
putint(ofpter, i)
register FILE *ofpter;
register int i;
{
    vputc(i >> 24, ofpter);
    vputc(i >> 16, ofpter);
    vputc(i >> 8 , ofpter);
    vputc(i      , ofpter);
}

#ifdef JS
static
#endif
getnull(ifpter, n)
register FILE *ifpter;
register int n;
{
    register int k;
    
    for(;n;n--){
	k = vgetc(ifpter);
	if(k == -1)return(-1);
    }
    return(0);
}

#ifdef JS
static
#endif
getnstr(ifpter, n, st)
register FILE *ifpter;
register int n;
register char *st;
{
    int k;
    
    for(;n;n--){
	k = vgetc(ifpter);
	*st++ = k;
	if(k == -1)return(-1);
    }
    return(0);
}

#ifdef JS
static
#endif
get_n_EU_str(ifpter, n, st)
register FILE *ifpter;
register int n;
register w_char *st;
{
    register    int k;
    
    for(;n;n--){
	k = get_short((short *)st++, ifpter);
	if(k == -1)return(-1);
    }
    return(0);
}

#ifdef JS
static
#endif
get_short(sp,ifpter)
register short *sp;
FILE *ifpter;
{
    register int i = 0;
    register int k;
    
    i |= (k = vgetc(ifpter)) << 8;
    if(k == -1)return(-1);
    i |= (k = vgetc(ifpter));
    if(k == -1)return(-1);
    *sp = i;
    return(0);
}

#ifdef JS
static
#endif
getint(ip,ifpter)
register FILE *ifpter;
int *ip;
{
    register int i = 0;
    register int k;
    
    i |= (k = vgetc(ifpter)) << 24;
    if(k == -1)return(-1);
    i |= (k = vgetc(ifpter)) << 16;
    if(k == -1)return(-1);
    i |= (k = vgetc(ifpter)) << 8;
    if(k == -1)return(-1);
    i |= (k = vgetc(ifpter));
    if(k == -1)return(-1);
    *ip = i;
    return(0);
}

#ifndef JS
udytoS(yomi,tnum,hostart,tary)
register w_char *yomi;
register int tnum;
char *hostart;
register struct uind1 *tary;
{
    struct uind2 *hop;
    int len;
    
    hop = (struct uind2 *)((char *)hostart + tary[tnum].pter);
    yomi[0] = tary[tnum].yomi1 >> 16;
    yomi[1] = tary[tnum].yomi1 & 0xffff;
    if(yomi[1]){
	yomi[2] = tary[tnum].yomi2 >> 16;
	if(yomi[2]){
	    yomi[3] = tary[tnum].yomi2 & 0xffff;
	}
    }
    len = hop->yomi[0];
    /*Strncpy(yomi + 4, (hop->yomi) + 1, len - 4);
      In order not to use ../etc/strings.c*/
    bcopy((char *)((hop->yomi) + 1), (char *)(yomi + 4),  max(0, ((len - 4)) * 2));
    yomi[len] = 0;
}
#endif



#ifdef JS
static
#endif
create_file_header(ofpter, file_type, file_passwd)
FILE *ofpter;
int file_type;
char *file_passwd;
{
    struct stat buf;
    char hostname[WNN_HOSTLEN];
    struct wnn_file_head fh;

    if(fstat(fileno(ofpter), &buf) == -1){
	return(-1);
    }
    gethostname(hostname, WNN_HOSTLEN);

    fh.file_uniq.time = (int)buf.st_ctime;
    fh.file_uniq.dev = (int)buf.st_dev;
    fh.file_uniq.inode = (int)buf.st_ino;
    bzero(fh.file_uniq.createhost, WNN_HOSTLEN);
    strcpy(fh.file_uniq.createhost, hostname);
    
    /* file_uniq_org */
    fh.file_uniq_org.time = (int)buf.st_ctime;
    fh.file_uniq_org.dev = (int)buf.st_dev;
    fh.file_uniq_org.inode = (int)buf.st_ino;
    bzero(fh.file_uniq_org.createhost, WNN_HOSTLEN);
    strcpy(fh.file_uniq_org.createhost, hostname);

    fh.file_type = file_type;
    if(file_passwd){
	strncpy(fh.file_passwd, file_passwd, WNN_PASSWD_LEN);
    }else{
	bzero(fh.file_passwd, WNN_PASSWD_LEN);
    }
    if(output_file_header(ofpter, &fh) == -1) return(-1);
    return(0);
}

/* 128 Bytes File Header */

#ifdef JS
static
#endif
output_file_header(ofpter, hp)
FILE *ofpter;
struct wnn_file_head *hp;
{
    put_n_str(ofpter, WNN_FILE_STRING, WNN_FILE_STRING_LEN);
    putint(ofpter, hp->file_type);
    output_file_uniq(&hp->file_uniq, ofpter);
    output_file_uniq(&hp->file_uniq_org, ofpter);
    put_n_str(ofpter, hp->file_passwd, WNN_PASSWD_LEN);
    putnull(ofpter, 36);		/* Future Use */
    return(0);
}

#ifdef JS
static
#endif
input_file_header(ifpter, hp)
FILE *ifpter;
struct wnn_file_head *hp;
{
    
    char wnn_file_string[WNN_FILE_STRING_LEN + 1];
    int err = 0;
    
    getnstr(ifpter, WNN_FILE_STRING_LEN, wnn_file_string);
    if(strncmp(wnn_file_string, WNN_FILE_STRING, WNN_FILE_STRING_LEN))
	err = -1;
    if(getint((&hp->file_type), ifpter) == -1) err = -1;
    if(input_file_uniq(&(hp->file_uniq), ifpter) == -1) err = -1;
    if(input_file_uniq(&(hp->file_uniq_org), ifpter) == -1) err = -1;
    getnstr(ifpter, WNN_PASSWD_LEN, hp->file_passwd);
    getnull(ifpter, 36);
    return(err);
}

#ifdef JS
static
#endif
output_file_uniq(funiq, ofpter)
struct wnn_file_uniq *funiq;
FILE *ofpter;
{
    putint(ofpter, funiq->time);
    putint(ofpter, funiq->dev);
    putint(ofpter, funiq->inode);
    put_n_str(ofpter, funiq->createhost, strlen(funiq->createhost));
    putnull(ofpter, WNN_HOSTLEN - strlen(funiq->createhost));
}

#ifdef JS
static
#endif
input_file_uniq(funiq, ifpter)
struct wnn_file_uniq *funiq;
FILE *ifpter;
{
    if(
       getint(&(funiq->time), ifpter) == -1 ||
       getint(&(funiq->dev), ifpter) == -1 ||
       getint(&(funiq->inode), ifpter) == -1 ||
       getnstr(ifpter, WNN_HOSTLEN, funiq->createhost) == -1)
	return(-1);
    return(0);
}

#ifdef JS
static
#endif
check_inode(f, fh)
FILE *f;
struct wnn_file_head *fh;
{
    struct stat buf;
    if(fstat(fileno(f), &buf) == -1){
	return(-1);
    }
    if((int)buf.st_ino != fh->file_uniq.inode){
	return(-1);
    }
    return(0);
}

#ifdef JS
static
#endif
change_file_uniq(fh, n)
struct wnn_file_head *fh;
char *n;
{
    int x;
    FILE *fp;
    if((fp = fopen(n,"r+")) == NULL){
	return(-1);
    }
    create_file_header(fp, fh->file_type, fh->file_passwd);
    fseek(fp, 0, 0);
    x = input_file_header(fp, fh);/* It must not occur. */
    fclose(fp);
    if(x == -1) return(-1); 
    return(0);
}    


#ifdef JS
static
#endif
f_uniq_cmp(a,b)
char a[],b[];
{
    return  bcmp(a,b,sizeof(struct wnn_file_uniq));
}

#ifdef JS
static
#endif
vputs(c, fp)
register char *c;
register FILE *fp;
{
    while(*c){
	vputc(*c++, fp);
    }
}

#ifdef JS
static
#endif
vputws(w, fp)
register w_char *w;
register FILE *fp;
{
    register int n;
    
    for (n = 0;*w;w++){
	if(*w & 0xff00){
	    vputc(*w >> 8, fp);
	    vputc(*w, fp);
	    n++; n++;
	}else{
	    vputc(*w, fp);
	    n++;
	}
    }
    return(n);
}


extern char *wnn_get_hinsi_name();
#ifndef JS

put_yomi_str(yomi, ofpter)
w_char *yomi;
FILE *ofpter;
{
    register int c;
    register int i = 0;
    for(;*yomi;yomi++){
	c = *yomi;
	if(c & 0xff00){
	    put_short(ofpter, c);
	    i += 2;
	}else{   /* one byte char */
	    if(c == '\\'){
		vputc('\\', ofpter);
		vputc('\\', ofpter);
		i += 2;
	    }else if(c > 0x20){
		vputc(c, ofpter);
		i += 1;
	    }else{
		char tmp[8];
		sprintf(tmp , "\\0%o\\" , c);
		vputs(tmp, ofpter);
		i += strlen(tmp);
	    }
	}
    }
    return(i);
}

Get_knj2(kptr,kanji2, kouho, yomi, comment)
UCHAR *kptr;
int kanji2;
w_char *kouho, *comment, *yomi;
{
	int tcnt;
	for (tcnt = 0; tcnt < kanji2; tcnt++){
		kptr += *kptr;
	}
	get_kanji_str(kptr, kouho, yomi, comment);
	return;
}

get_kanji_str(kptr, kanji, yomi, comment)
UCHAR *kptr;
w_char *kanji, *comment, *yomi;
{
    w_char *tmpy;
    w_char *tmpk;
    w_char *tmpc;

    Get_kanji_str_r(kptr, &tmpk, &tmpy, &tmpc);
    if(kanji && tmpk)Sreverse(kanji, tmpk);
    if(yomi && tmpy)Sreverse(yomi, tmpy);
    if(comment && tmpc)Strcpy(comment, tmpc);
}
    

Get_kanji_str_r(kptr, tmpk, tmpy, tmpc)
UCHAR *kptr;
w_char **tmpk, **tmpy, **tmpc;
{
    int state = *(kptr + 1);
    static w_char dmy = 0; /* Must not be allocated on Stack!! */
    int adres;

    if(state & FORWARDED){
	/* Forward occures when comment-set is used */
	int adred;		/* Because of int boundary, read it as
				   two w_chars. which makes bite order change
				   easier.
				 */
	adres = (*(w_char *)(kptr + 2)) << 16 | (*(w_char *)(kptr + 4));
	Get_kanji_str_r(kptr + adres, tmpk, tmpy, tmpc);
	return;
    }
/*    get_kanji_str_r(kptr, tmpk, tmpy, comment); */
    if(state & HAS_YOMI){
	*tmpy = ((w_char *)(kptr + 2));
	*tmpk = *tmpy + Strlen(*tmpy) + 1;
    }else{
	*tmpy = &dmy;
	*tmpk = ((w_char *)(kptr + 2));
    }
    if(state & HAS_COMMENT){
	*tmpc = *tmpk + Strlen(*tmpk) + 1;
    }else{
	*tmpc = &dmy;
    }
}

#ifdef nodef
Get_kanji_len(kptr, which)
UCHAR *kptr;
int which;
{
    w_char tmp[LENGTHYOMI];

    if(which == D_KANJI){
	get_kanji_str_r(kptr, tmp, NULL, NULL);
    }else{
	get_kanji_str_r(kptr, NULL, tmp, NULL);
    }	
    return(Strlen(tmp));
}

get_kanji_str_r(kptr, kanji, yomi, comment)
UCHAR *kptr;
w_char *kanji, *comment, *yomi;
{
    w_char *c;
    int n , k;
    
    n = *kptr++ - 1;
    c = kanji;
    if(comment)*comment = 0;
    if(yomi)*yomi = 0;
    for(k = 0 ; k < n ;) {
	if(*kptr == DIC_COMMENT_CHAR){
	    if(c) *c = 0;
	    c = comment;
	    kptr++;
	    k++;
	}else if(*kptr == DIC_YOMI_CHAR){
	    if(c) *c = 0;
	    c = yomi;
	    kptr++;
	    k++;
	}else if(*kptr & 0x80){  /* kanji char */
	    if(c) *c++ = ((int) *kptr << 8 | *(kptr + 1));
	    kptr += 2;
	    k+= 2;
	}else{
	    if(c) *c++ = *kptr;
	    kptr++;
	    k++;
	}
    }
    if(c)*c = 0;
}
#endif

kanjiaddr(d0, kanji, yomi, comment)
UCHAR *d0;
w_char *kanji, *yomi, *comment;
{
    int len = 0;
    w_char *dest = (w_char *)(d0 + 2);
    int state = 0;
    w_char *pt;
    w_char tmp[LENGTHKANJI];

    if(yomi && *yomi){
	pt = yomi + Strlen(yomi) - 1;
	for(;pt >= yomi;){
	    *dest++ = *pt--;
	}
	state |= HAS_YOMI;
	*dest++ = 0;
    }

    pt = kanji + Strlen(kanji) - 1;
    for(;pt >= kanji;){
	*dest++ = *pt--;
    }
    *dest++ = 0;

    if(comment && *comment){
	pt = comment;
	for(;*pt;){
	    *dest++ = *pt++;
	}
	state |= HAS_COMMENT;
	*dest++ = 0;
    }
    *d0 = (UCHAR *)dest - d0;
    *(d0 + 1) = state;
    return(*d0);
}	

#endif


#ifdef JS
static
#endif
create_null_dic(fn,comm, passwd, hpasswd, which)
char  *fn;
w_char *comm;
char *passwd, *hpasswd;  /* not encoded passwd */
int which;
{
    FILE *fp;
    struct JT jt;
    char epasswd[WNN_PASSWD_LEN];
    
    jt.total = 0;
    jt.gosuu = 0;
    if(hpasswd){
	new_pwd(hpasswd, jt.hpasswd);
    }else{
	bzero(jt.hpasswd, WNN_PASSWD_LEN);
    }
    jt.syurui = which;
    jt.maxserial = 0;
    jt.maxtable = 0;
    jt.maxhontai = (which == WNN_UD_DICT)? 4: 0;
    jt.maxkanji = 0;
    if(comm){
	jt.maxcomment = Strlen(comm);
    }else{
	jt.maxcomment = 0;
    }
    jt.maxhinsi_list = 0;
    jt.maxri1[D_YOMI] = 0;
    jt.maxri1[D_KANJI] = 0;
    jt.maxri2 = 0;

    if((fp = fopen(fn, "w")) == NULL){
	/*	error1( "Jserver:create_null_ud:No file %s.\n", fn); */
	return(-1);
    }
    if(passwd){
	new_pwd(passwd, epasswd);
    }else{
	bzero(epasswd, WNN_PASSWD_LEN);
    }
    if(create_file_header(fp, WNN_FT_DICT_FILE,epasswd) == -1 ||
       output_header_jt(fp, &jt) == -1){
	fclose(fp);return(-1);
    }
    put_n_EU_str(fp, comm, jt.maxcomment);
    if(which == WNN_UD_DICT){
	putint(fp, 0);			/* hontai[0] */
    }

#ifdef	BSD42
    fchmod(fileno(fp), 0664);
    fclose(fp);
#else	/* SYSV */
    fclose(fp);
    chmod(fn, 0664);
#endif	/* BSD42 */
    return(0);
}


#ifdef JS
static
#endif
create_hindo_file(funiq, fn, comm, passwd, serial)
struct wnn_file_uniq *funiq;
char *fn;
w_char *comm;
char *passwd;			/* Not encoded */
int serial;
{
    FILE *fp;
    struct HJT hjt;
    char epasswd[WNN_PASSWD_LEN];
    w_char tmp[1];

    tmp[0] = 0;
    if(comm == NULL) comm = tmp;
    bcopy(funiq, &(hjt.dic_file_uniq), WNN_F_UNIQ_LEN);
    hjt.maxcomment = Strlen(comm);
    
    hjt.maxserial = serial;

    if((fp = fopen(fn, "w")) == NULL){
	return(-1);
    }
    if(passwd){
	new_pwd(passwd, epasswd);
    }else{
	bzero(epasswd, WNN_PASSWD_LEN);
    }
    if(create_file_header(fp, WNN_FT_HINDO_FILE, epasswd) == -1){fclose(fp);return(-1);}
    output_header_hjt(fp, &hjt);
    put_n_EU_str(fp, comm, hjt.maxcomment);
    putnull(fp, serial);

#ifdef	BSD42
    fchmod(fileno(fp), 0664);
    fclose(fp);
#else	/* SYSV */
    fclose(fp);
    chmod(fn, 0664);
#endif	/* BSD42 */
    return(0);
}

/* Header Total 128 Bytes */

#ifdef JS
static
#endif
input_header_jt(ifpter, jt1)
FILE *ifpter;
struct JT *jt1;
{
    if(
       getint(&jt1->syurui , ifpter) == -1 ||
       getint(&jt1->maxcomment, ifpter) == -1 ||
       getint(&jt1->maxhinsi_list, ifpter) == -1 ||
       getint(&jt1->maxserial , ifpter) == -1 ||
       getint(&jt1->maxkanji , ifpter) == -1 ||
       getint(&jt1->maxtable , ifpter) == -1 ||
       getint(&jt1->maxhontai , ifpter) == -1 ||
       getint(&jt1->gosuu , ifpter) == -1 ||
       getnstr(ifpter, WNN_PASSWD_LEN, jt1->hpasswd) == -1 ||
       getint(&jt1->total , ifpter) == -1 ||
       getint(&jt1->maxri1[D_YOMI] , ifpter) == -1 ||
       getint(&jt1->maxri1[D_KANJI] , ifpter) == -1 ||
       getint(&jt1->maxri2, ifpter) == -1 ||
       getnull(ifpter, 56 ) == -1)
	return(-1);
    return(0);
}

#ifdef JS
static
#endif
output_header_jt(ofpter, jt1)
FILE *ofpter;
struct JT *jt1;
{
    putint(ofpter, jt1->syurui);
    putint(ofpter, jt1->maxcomment);
    putint(ofpter,  jt1->maxhinsi_list);
    putint(ofpter, jt1->maxserial);
    putint(ofpter, jt1->maxkanji);
    putint(ofpter, jt1->maxtable);
    putint(ofpter, jt1->maxhontai);
    putint(ofpter, jt1->gosuu);
    put_n_str(ofpter, jt1->hpasswd, WNN_PASSWD_LEN);
    putint(ofpter, jt1->total);
    putint(ofpter, jt1->maxri1[D_YOMI]);
    putint(ofpter, jt1->maxri1[D_KANJI]);
    putint(ofpter, jt1->maxri2);
    putnull(ofpter, 56);
    return(0);
}

/* Header 64 Byte */
#ifdef JS
static
#endif
input_header_hjt(ifpter, hjt1)
FILE *ifpter;
struct HJT *hjt1;
{
    if(
       input_file_uniq(&hjt1->dic_file_uniq, ifpter) == -1 ||  /* 7 * 4 */
       getint(&hjt1->maxcomment, ifpter) == -1 ||
       getint(&hjt1->maxserial, ifpter) == -1 ||
       getnull(ifpter, 28) == -1)  {
	return(-1);
    }
    return(0);
}

#ifdef JS
static
#endif
output_header_hjt(ofpter, hjt1)
FILE *ofpter;
struct HJT *hjt1;
{
    output_file_uniq(&hjt1->dic_file_uniq, ofpter);
    putint(ofpter, hjt1->maxcomment);
    putint(ofpter, hjt1->maxserial);
    putnull(ofpter, 28);
    return(0);
}


#ifndef JS
Print_entry(yomi, kstr, cstr, hindo, ima, hinsi, serial, ofpter, esc_exp)
w_char *yomi, *kstr,*cstr;
int serial, hindo, ima, hinsi;
FILE *ofpter;
int esc_exp;
{
    register int len;
    char *k;
    char buf[32];
    register int klen;
    static w_char revy[LENGTHKANJI];
    
    if(serial != -1){
	sprintf(buf, "%d\t", serial);
	vputs(buf, ofpter);
    }

    len = put_yomi_str(yomi, ofpter);
    if(len < 8)vputc('\t' , ofpter);
    if(len < 16)vputc('\t' , ofpter);
    vputc('\t' , ofpter);
    
    if(esc_exp){
	Sreverse(revy, yomi);
	kanji_esc_str(kstr, revy, Strlen(yomi));
	len = put_yomi_str(kstr, ofpter);
    }else{
	len = vputws(kstr, ofpter); 
    }
    if(len < 8)vputc('\t' , ofpter);
    if(len < 16)vputc('\t' , ofpter);
    vputc('\t' , ofpter);
    
    k = wnn_get_hinsi_name(hinsi);
    if(k){
	vputs(k, ofpter);
	if(strlen(k) < 8)vputc('\t' , ofpter);
	vputc('\t' , ofpter);
    }else{
	sprintf(buf, "%d\t\t", hinsi);
	vputs(buf, ofpter);
    }
    
    if(ima) vputc('*', ofpter);
    if(hindo == -1){  	/*  Real hindo == -1 means Not to use it */
	vputs("-", ofpter);
    }else{
	sprintf(buf, "%d", hindo);
	vputs(buf, ofpter);
    }	
    if(cstr && cstr[0]){
	vputc('\t', ofpter);
	len = vputws(cstr, ofpter); 
    }
    vputc('\n', ofpter);
}
#endif
