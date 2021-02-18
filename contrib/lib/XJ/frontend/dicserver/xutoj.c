/*
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

#define	NEED_CR

#include "commonhd.h"

#define	HANKAKU_JIS_IN	'\016'
#define	HANKAKU_JIS_OUT	'\017'

#define JIS_IN "\033$B"
#define JIS_IN_1 "\033$@"
#define JIS_OUT "\033(J"
#define JIS_OUT_1 "\033(B"
#define JIS_OUT_2 "\033(H"
#define	HANKAKU_JIS	2
#define	ZENKAKU_JIS	1
#define	ASCII		0


static	unsigned char	*j;
static	w_char		*iu;
static	unsigned char	*eu;
static	unsigned char	*sj;

static
putj(x)
{
	*j++=x;
}

static
puteu(x)
{
	*eu++=x;
}

static
putsj(x)
{
	*sj++ = x;
}

static
putsjw(x)
{
	*sj++ = x>>8;
	*sj++ = x;
}

static int oj_mode = ASCII;	/* 出力時のｊｉｓコードのモード */

#ifdef	JIS7
/*	内部 U-jis を 7bit jis コードに変換します
	文字列の長さを返します			*/
extern
iujis_to_jis(jis,iujis,iusiz)
unsigned char	*jis;	/*	jisコードになったものをおくbuf	*/
w_char	*iujis;	/*	iujisコードのものをおいてくるbuf*/
int	iusiz;	/*	iujis の大きさ			*/
{
 /*
 int oj_mode;
 */
 int x;
 j=jis;
 iu=iujis;
 /*
 for(oj_mode=ASCII;iusiz>0;iusiz-=sizeof(w_char)){
 */
 for(;iusiz>0;iusiz-=sizeof(w_char)){
	x = *iu++;
	if(((x & 0xFF00)==0x8E00) || ((x & 0xFF80)==0x8080){
		change_mode(&oj_mode,HANKAKU_JIS);
		putj(x & 0x7f);
	}else if(x& 0x8000){
		change_mode(&oj_mode,ZENKAKU_JIS);
		putj((x >>8) & 0x7f);putj(x & 0x7f);
	}else{
		change_mode(&oj_mode,ASCII);
		putj(x);
	}
 }
 change_mode(&oj_mode,ASCII);
 return (j - jis);
}
#endif	JIS7

/*	内部 U-jis を 8bit jis コードに変換します
	文字列の長さを返します			*/
extern
iujis_to_jis8(jis,iujis,iusiz)
unsigned char	*jis;	/*	jisコードになったものをおくbuf	*/
w_char	*iujis;	/*	iujisコードのものをおいてくるbuf*/
int	iusiz;	/*	iujis の大きさ			*/
{
 /*
 int oj_mode;
 */
 int x;
 j=jis;iu=iujis;
 /*
 for(oj_mode=ASCII;iusiz>0;iusiz-=sizeof(w_char)){
 */
 for( ;iusiz>0;iusiz-=sizeof(w_char)){
	x = *iu++;
	if(((x & 0xFF00)==0x8E00) || ((x & 0xFF80) == 0x80)){
		change_mode(&oj_mode,ASCII);
		putj(x & 0xff);
	}else if(x & 0x8000){
		change_mode(&oj_mode,ZENKAKU_JIS);
		putj((x >> 8) & 0x7f);putj(x & 0x7f);
	}else{
		change_mode(&oj_mode,ASCII);
		putj(x);
	}
 }
 change_mode(&oj_mode,ASCII);
 return (j - jis);
}

static
change_mode(mode,new_mode)
int *mode;
int new_mode;
{
 if(*mode==new_mode)return;
 switch(*mode){
	case ZENKAKU_JIS :
		putj('\033'); putj('('); putj('J');break;
#ifdef	JIS7
	case HANKAKU_JIS :
		putj(HANKAKU_JIS_OUT);break;
#endif	JIS7
	default: ;
 }
 *mode=new_mode;
 switch(new_mode){
	case ZENKAKU_JIS :
		putj('\033'); putj('$'); putj('B');break;
#ifdef	JIS7
	case HANKAKU_JIS :
		putj(HANKAKU_JIS_IN);break;
#endif	JIS7
	default: ;
 }
}


#ifdef	JIS7
/*	外部 U-jis を 7bit jis コードに変換します	*/
extern
eujis_to_jis(jis,eujis,eusiz)
unsigned char *jis,*eujis;
int eusiz;
{
    static int kanji1 = 0;
    static char kanji1_code = 0;
    /* 0: normal
       1: get SS2
       2: get kanji 1 byte */
    /*
    int oj_mode;
    */
    int x;
    j=jis;
    eu=eujis;
    /* ADD KURI */
    if (kanji1 != 0) {
	if (kanji1 == 2) {
	    putj(kanji1_code & 0x7f);
	}
	putj(*eu & 0x7f);
	eusiz -= sizeof(char);
	kanji1 = 0;
    }
    /* ADD KURI end */
    /*
    for(oj_mode=ASCII;eusiz>0;eusiz-=sizeof(char)){
    */
    for( ;eusiz>0;eusiz-=sizeof(char)){
	x = *eu++;
	if((x & 0xFF)==0x8E){
	    change_mode(&oj_mode,HANKAKU_JIS);
	    if (eusiz > 1) {
		putj(*eu++ & 0x7f);
		eusiz -= sizeof(char);
	    } else {
		kanji1 = 1;
	    }
	}else if(x & 0x80){
	    change_mode(&oj_mode,ZENKAKU_JIS);
	    if (eusiz > 1) {
		putj(x & 0x7f);putj(*eu++ & 0x7f);
		eusiz -= sizeof(char);
	    } else {
		kanji1 = 2;
		kanji1_code = x & 0x7f;
	    }
	}else{
	    change_mode(&oj_mode,ASCII);
	    putj(x);
	}
    }
    if (kanji1 == 0)
	change_mode(&oj_mode,ASCII);
    return(j - jis);
}
#endif	JIS7

/*	外部 U-jis を 8bit jis コードに変換します	*/
extern
eujis_to_jis8(jis,eujis,eusiz)
unsigned char *jis,*eujis;
int eusiz;
{
    static int kanji1 = 0;
    static unsigned char kanji1_code = 0;
    /* 0: normal
       1: get SS2
       2: get kanji 1 byte */
    /*
    int oj_mode;
    */
    int x;
    j=jis;
    eu=eujis;
    /* ADD KURI */
    if (kanji1 != 0) {
	if (kanji1 == 2) {
	    putj(kanji1_code & 0x7f);
	    putj(*eu & 0x7f);
	} else {
	    putj(*eu);
	}
	eusiz -= sizeof(char);
	kanji1 = 0;
	eu++;
    }
    /* ADD KURI end */
    /*
    for(oj_mode=ASCII;eusiz>0;eusiz-=sizeof(char)){
    */
    for( ;eusiz>0;eusiz-=sizeof(char)){
	x = *eu++;
	if((x & 0xFF)==0x8E){
	    change_mode(&oj_mode,ASCII);
	    if (eusiz > 1) {
		putj(*eu++);
		eusiz -= sizeof(char);
	    } else {
		kanji1 = 1;
	    }
	}else if(x & 0x80){
	    change_mode(&oj_mode,ZENKAKU_JIS);
	    if (eusiz > 1) {
		putj(x & 0x7f);putj(*eu++ & 0x7f);
		eusiz -= sizeof(char);
	    } else {
		kanji1 = 2;
		kanji1_code = x;
	    }
	}else{
	    change_mode(&oj_mode,ASCII);
	    putj(x);
	}
    }
    if (kanji1 == 0)
	change_mode(&oj_mode,ASCII);
    return(j - jis);
}

/*	内部 U-jis を 外部 U-jis コードに変換します	*/
extern
iujis_to_eujis(eujis,iujis,iusiz)
unsigned char *eujis;
w_char *iujis;
int iusiz;
{
 int x;
 iu=iujis;eu=eujis;
 for(;iusiz>0;iusiz-=sizeof(w_char)){
	x = *iu++;
	if(x & 0xFF80 == 0x80) puteu(0x8e);
	else if(x & 0xFF00) puteu(x>>8);
	puteu(x);
 }
 return(eu - eujis);
}

static int ij_mode = 1;
  /* 1:ascii
     2:ascii + ESC
     3:ascii + ESC + $
     4:JIS
     5:JIS + ESC
     6:JIS + ESC + (
   */

jis_to_eujis(eujis,jis,jsiz)
unsigned char *jis ,*eujis;
int jsiz;
{

  unsigned char *jend = jis + jsiz;
  eu = eujis;

  for(;jis<jend;jis++){
    switch(ij_mode){
    case 1:	/* ASCII */
      if(*jis == ESC){
	ij_mode = 2;
      } else {
	if (*jis > 0xa0 && *jis < 0xe0) {	/* 8bit JIS 半角かな */
	    *eu++ = 0x8e;
	}
	*eu++ = *jis;
      }
      break;
    case 2:
      if(*jis == '$'){	/* AFTER ESC */
	ij_mode = 3;
      }else{
	*eu++ = ESC;
	if (*jis > 0xa0 && *jis < 0xe0) {	/* 8bit JIS 半角かな */
	    *eu++ = 0x8e;
	}
	*eu++ = *jis;
	ij_mode = 1;
      }
      break;
    case 3:
      if(*jis == 'B' || *jis == '@'){	/* AFTER ESC $ */
	ij_mode = 4;
      }else{
	*eu++ = ESC;
	*eu++ = '$';
	if (*jis > 0xa0 && *jis < 0xe0) {	/* 8bit JIS 半角かな */
	    *eu++ = 0x8e;
	}
	*eu++ = *jis;
	ij_mode = 1;
      }
      break;
    case 4:
      if(*jis == ESC){	/* KANJI */
	ij_mode = 5;
      }else{
	*eu++ = *jis | 0x80;
      }
      break;
    case 5:
      if(*jis == '('){	/* AFTER ESC */
	ij_mode = 6;
      }else{
	*eu++ = ESC;
	*eu++ = *jis | 0x80;
	ij_mode = 4;
      }
      break;
    case 6:
      if(*jis == 'J' || *jis == 'B' || *jis == 'H'){	/* AFTER ESC ( */
	ij_mode = 1;
      }else{
	*eu++ = ESC;
	*eu++ = '(';
	*eu++ = *jis | 0x80;
	ij_mode = 5;
      }
      break;
    }
  }
  return(eu - eujis);
}

/* 溜まっているコードを吐き出す */
jis_flush(eujis)
unsigned char *eujis;
{
  /* 1:ascii
     2:ascii + ESC
     3:ascii + ESC + $
     4:JIS
     5:JIS + ESC
     6:JIS + ESC + (
     */

    switch(ij_mode){
    case 1:	/* ASCII */
    case 4:	/* JIS */
	return (0);
    case 2:
	*eujis++ = ESC;
	ij_mode = 1;
	return (1);
    case 3:
	*eujis++ = ESC;
	*eujis++ = '$';
	return (2);
    case 5:
	*eujis++ = ESC;
	ij_mode = 4;
	return (1);
    case 6:
	*eujis++ = ESC;
	*eujis++ = '(';
	ij_mode = 5;
	return (2);
    }
}

/*
 *	Shifted JIS
 */

/*	外部 U-jis を S-jis コードに変換します
	文字列の長さを返します			*/
extern
eujis_to_sjis(sjis,eujis,eusiz)
unsigned char	*sjis;	/*	sjisコードになったものをおくbuf	*/
unsigned char	*eujis;	/*	eujisコードのものをおいてくるbuf*/
int	eusiz;	/*	eujis の大きさ			*/
{register int x;
 sj=sjis;eu=eujis;
 for(;eusiz>0;){
	if((x = *eu++) & 0x80){
		if(x == 0x8e){
			putsj(*eu++ | 0x80);
		}else{
			putsjw(jtosj(x & 0x7F, *eu++ & 0x7F)) ;
		}
		eusiz-=2;
	}else{
		putsj(x);
		eusiz--;
	}
 }
 return(sj - sjis);
}

/*	内部 U-jis を S-jis コードに変換します
	文字列の長さを返します			*/
extern
iujis_to_sjis(sjis,iujis,iusiz)
unsigned char	*sjis;	/*	sjisコードになったものをおくbuf	*/
w_char	*iujis;	/*	iujisコードのものをおいてくるbuf*/
int	iusiz;	/*	iujis の大きさ			*/
{
    register int x;
    sj=sjis;
    iu=iujis;
    for(;iusiz>0;iusiz-=sizeof(w_char)){
	if((x = *iu++) & 0xff00){
		if ((x & 0xff00) == 0x8e00){
			putsj((x & 0xff) | 0x80);
		} else {
			putsjw(jtosj((x>>8) & 0x7f, x & 0x7f)) ;
		}
	}else{
		putsj(x);
	}
    }
    return(sj - sjis);
}

/* convert JIS code to shift-JIS code */
static
jtosj(high,low)
unsigned high, low;
{
	if(high & 1)
		low += 0x1f;
	else
		low += 0x7d;
	if(low >= 0x7f) low++;
	high = (high - 0x21 >> 1) + 0x81;
	if(high > 0x9f)
		high += 0x40;
	return((high << 8) | low);
}

/* convert shift-JIS to JIS code */ 
static
sjtoj(high, low)
register unsigned high, low;
{
	high -= (high <= 0x9f) ? 0x71 : 0xb1;
	high = high * 2;
	if (low > 0x7f)
		low--;
	if (low >=0x9e) {
		high += 2;
		low -= 0x7d;
	}else{
		high++;
		low -= 0x1f;
	}
	return((high << 8) | low);
}

sjis_to_iujis(iujis,sjis,ssiz)
w_char	*iujis;		/* iujisコードになったものをおくbuf */
unsigned char	*sjis;	/* sjisコードのものをおいてくるbuf */
int	ssiz;		/* sjis の大き */
{register int x;
 sj=sjis;iu=iujis;
 for(;ssiz>0;){
	if((x = *sj++) & 0x80){
		*iu++ = ((sjtoj(x,*sj++)) | 0x8080);
		ssiz-=2;
	}else{
		*iu++ = (x);
		ssiz--;
	}
 }
 return((char *)iu - (char *)iujis);
}

sjis_to_eujis(eujis,sjis,ssiz)
unsigned char	*eujis;	/*	eujisコードになったものをおくbuf	*/
unsigned char	*sjis;	/*	sjisコードのものをおいてくるbuf*/
int	ssiz;	/*	sjis の大きさ			*/
{register int x;unsigned char *sj;
 sj=sjis;eu=eujis;
 for(;ssiz>0;){
	if((x = *sj++) & 0x80){
		/* x = (sjtoj(x,sj++) | 0x8080); */	/* バグ発見 !! */
		x = (sjtoj(x,*sj++) | 0x8080);		/* 変えました KUWA */
		x = (sjtoj(x,sj++) | 0x8080);
		puteu(x>>8);
		puteu(x);
		ssiz-=2;
	}else{
		puteu(x);
		ssiz--;
	}
 }
 return(eu - eujis);
}

#ifdef	JIS7
sjis_to_jis(jis,sjis,siz)
unsigned char *jis,*sjis;
int siz;
{char buf[4000];
 int len;
 len= sjis_to_eujis(buf,sjis,siz);
 return(eujis_to_jis(jis,buf,len));
}
#endif	JIS7

sjis_to_jis8(jis,sjis,siz)
unsigned char *jis,*sjis;
int siz;
{char buf[4000];
 int len;
 len= sjis_to_eujis(buf,sjis,siz);
 return(eujis_to_jis8(jis,buf,len));
}

jis_to_sjis(sjis,jis,siz)
unsigned char *sjis,*jis;
int siz;
{char buf[4000];
 int len;
 len= jis_to_eujis(buf,jis,siz);
 return(eujis_to_sjis(sjis,buf,len));
}


jis_to_iujis(iujis, jis, jsiz)
w_char *iujis;
unsigned char *jis;
int jsiz;
{
	char buf[4000];
	int len;

	len = jis_to_eujis(buf, jis, jsiz);
	return(eujis_to_iujis(iujis, buf, len));
}

eujis_to_iujis(iujis, eujis, eusiz)
w_char *iujis;
unsigned char *eujis;
int eusiz;
{
	register int x;

	iu = iujis;
	eu = eujis;

	for(;eusiz > 0;){
		x = *eu++;
		if(x & 0x80){
		    if (x == 0x8e) {
			*iu++ = *eu++;
		    } else {
			*iu++ = ((x << 8) & 0xff00) | *eu++;
		    }
		    eusiz -= 2;
		}else{
			*iu++ = x;
			eusiz--;
		}
	}
	return((char *)iu - (char *)iujis);
}

