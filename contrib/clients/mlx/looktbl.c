

#include <X11/copyright.h>

/*
 * Copyright 1989    Institute of Systems Science (ISS),
 *		     National University of Singapore , Singapore
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of ISS not be used 
 * in advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission. ISS makes no representations
 * about the suitability of this software for any purpose. It is provided
 * "as is" without express or implied warranty.
 *
 *
 * ISS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * ISS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 * CONTACT:      HU XIAN XIANG,
 * 		 INSTIUTE OF SYSTEMS SCIENCE, 
 *               NATIONAL UNIVERSITY OF SINGAPORE, 	
 *               REPUBLIC OF SINGAPORE  0511
 * FAX:          (65)7782571
 * TELEX:        ISSNUS RS 39988
 * BITNET:       ISSMLX@NUSVM 
*/

/* Filename: looktbl.c  15 Nov. 1989,	By Hu Xian Xiang */

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include "looktbl.h"
#include "extern.h"

retrieve(c)
char c;
{
	int	k;
	char	*p,*p1,*pk;

	if (ischoicekey(c)) {
		k = m + (c&0x1f)*2;
		if (k>=n){
			beep();return(0);
		}
		wordflag = 0;
		pk = hzbuf+k;
		hzcode[0]= *pk | 0x80;hzcode[1]= *(pk+1) | 0x80;
		return(2);
	}
	if (c=='>') {	/*Select next page */
		if(m < (n-fkeyno*2)) {
			m += (fkeyno*2);
			drawbuf();
		}
		else	beep ();
		return(0);
	}
	if(c=='<') { 	/*Select piror page */
		if(m>=fkeyno*2) {
			m -= (fkeyno*2);
			drawbuf();
		}
		else	beep ();
		return(0);
	}
/*
	if(c==DELETE || Hifreq_flg) {
		drawcode(' ',hcc=0);
		return(0);
	}
*/
	if(modeflag==':' && !isalnum(c) || modeflag==';' && !isdigit(c)
	    || modeflag=='.' && !isletter(c)|| modeflag==',' && !isxdigit(c)
	    || modeflag=='*' && !isallkey(c) || modeflag=='+' &&!issmallkbd(c)){
		if(hcc == 0 ) {
			hzcode[0] = c;
			return(0);
		}
		else if(c==bspace || c==delete){
			c = bspace;
			if (38 == TblAddr[6]) 
				drawmlcode(c,hcc);
			else	drawcode(c,hcc);
			hcc--;
		}
		else	beep ();
	}
	else 
		if(hcc >= 8) {
			drawcode(' ',hcc=0);
			beep ();
			return(0);
		}else	ext_string[hcc++]=c;
	if (38 != TblAddr[6]) 
		drawcode(ext_string[hcc-1],hcc);
	n= QueryDict(ext_string,hcc,0,&next);
	m = 0;
	drawbuf();
	if (hcc>1) return(n);
	else	return(0);
}

QueryDict(code,i,rw,next)
char	*code;
int	i,rw;
unsigned	int	*next;
{
	char	*p,*q,ch;
	unsigned  int	nextitem,dec,dec0,h,mid,t,TabStart;
	int		j,lng,c_num;
	unsigned char	cnum[12],inp[12];

	*next=0;
	if(!i) return(0);
	dec = TblAddr[6];
/*
	if (dec == 18 ) 
		return(querywbzx(code,i));	/* WBZX Input Method */

	if (dec == 38 ) 
		return(cacis(code[i-1])) ;		/* CACIS */
	for (c_num=1;dec != modu[c_num-1];c_num++);
	if(!ExtToKeyNo(code,c_num,i,cnum)) return(0);
	lng = rpress(cnum,c_num,i,inp);/* lng=2, 4, 6 */
	if (lng == 6) inp[1] = inp[1] | 0x40;
	TabStart= 0x20;			/* table stated at 0x20 */
	for (j=0; j<lng; j+=2) {
		short	tmp;
		h=TabStart;
		tmp = TblAddr[17+j+j];
		tmp = tmp << 8;
		tmp |= TblAddr[16+j+j];
		t =h + tmp & 0xffff; /* *(unsigned short *)(TblAddr+16+j+j);*/
		TabStart=t;
	}			/* get head and tail of the table */
	dec=reset(t-2,lng);		/* adjust location to item head */
	t=dec;
	if (t<h)  return(0);
	for(j=1,dec0=0;;) {
		mid= (t>>1)+(h>>1)& 0xfffe;
		dec =  reset(mid,lng);
		nextitem=0;
		*next=dec;
		if (dec==dec0) {
			if(j>0) {
				dec += lng;
				do	{
					ch = *(TblAddr+dec);
					if(ch & 0x80) dec += 2;
				}while(ch & 0x80);/* move to next item */
				*next=dec;
				nextitem=1;
			}
			else	return(0);
		}
		dec0=dec;	/* store previous index */
		mid=dec;
		if(t<h) break;
		p = TblAddr;
		p += dec;             /* read an item */
		j=strncmp(p,inp,lng);
		if(j > 0) {
			h = mid;
			continue;
		}
		if(j < 0) {
			if(h >= t || nextitem==1) break;
			t = mid;
			continue;
		}
		break;
	}
	if(!j) {
		p = TblAddr+dec+lng; /* read an item as long as possible */
		q = hzbuf;
		for (lng=0; *p & 0x80;lng +=2) {
			if(rw==0) {
				*q++ = *p++, *q++ = *p++;
			}
			else	{
				*p++ = *q++, *p++ = *q++;
			}
		}
		*q = '\0';
		return(lng);
	}
	return(0);
}
/* adjust the location to the begin of each item, lng is the length of item,
the allc is the initial location, return the displacement of location */
reset(allc,lng)
unsigned int allc;
int	 lng;
{
	char	*p;
	p = TblAddr;
	p += allc;
	for(; *p & 0x80;) p -= 2;
	switch (lng) {
	case 4:
		if(!(*(p-2) & 0x80)) p -= 2;
	case 2:
		break;
	case 6:
		if(*p & 0x40)
			p -= 4;
		else
			p -= 2;
		if(*p & 0x80) p += 2;
		break;
	}
	return(p-TblAddr);
}
SelectTab(File)
char	*File;
{
	int	tblfile;
	char	tmp[5];

	tblfile=open(File,O_RDONLY);
	if (tblfile<0){
		printf("Error: No Input Table.\n");
		return;
	}
	read(tblfile,TblAddr,64000);
	close(tblfile);
	modeflag = *(TblAddr+4);
	Hifreq_flg =0;
/*	SetChoiceKey();*/
	tmp[0]= *(TblAddr+0);
	tmp[1]= *(TblAddr+1);
	tmp[2]= *(TblAddr+2);
	tmp[3]= *(TblAddr+3);
	tmp[4]= 0;
	Drawstr(tmp,0);
}

/*  Display Hanzi buf at prompt window */
drawbuf()
{
	char	*p, *p2, *q, selbuf[81];
	short	int i;

	i=n-m, i >>=1;
	i -= fkeyno;
	setlabel(0,intTOstr(0,i));
	setlabel(1,intTOstr(1,m/2));
	p  = hzbuf + m;
	p2 = hzbuf + n;
	q=selbuf;
	for(i=0;i<fkeyno ;i++) 
		if (p < p2) {
			*q++ = *p++ | 0x80; *q++ = *p++ | 0x80;
			*q++ =0xa9 ; *q++ =0xa1 ;
		}else {
			*q++ =0xa9 ; *q++ =0xa1 ;
			*q++ =0xa9 ; *q++ =0xa1 ;
		}
	*q = 0;
	hzcode[0]=selbuf[0];
	hzcode[1]=selbuf[1];
	hzcode[2]=0;		/* Use SPACE BAR select first Hanzi */
	Drawstr(selbuf,7);
	return;
}

char	*intTOstr(flag,i)
int	flag;
int	i;
{
	char	tmp[8];
	int	no;

	if (i<1) i =0;
	no = 0;
	tmp[no++]=' ';
	if (!flag) {
		tmp[no++]='=';
		tmp[no++]='>';
	}

	if(i<100) tmp[no]=' ';
	else tmp[no]=i/100+'0', i %=100;

	if(i<10 && tmp[no]==' ' ) tmp[++no]=' ';
	else	tmp[++no]=i/10 +'0';

	tmp[++no]=i%10 +'0';

	if (flag) {
		tmp[++no]='<';
		tmp[++no]='=';
	}
	tmp[++no]=' ';
	tmp[++no]=0;
	return(tmp);
}

/* Tranlate a extern-code string into a key number for retrieving; */
ExtToKeyNo(code,c_num,i,cnum)
char code[],cnum[];
int  c_num,i;
{
	static char cha_tab[11]={
		'-','=','[',']',';','\'','`','\\',',','.','/'		};
	char  *p,*q;
	int   j,k;
	char  c;
	p = code;
	for(q = cnum, j = 0; j<i; j++, p++) { /*one word*/
		c= *p;
		switch(c_num) {
		case 1:
			if(c >= '0' && c <= '9') {
				*q++ = c - '0'+1;
				break;
			}
			else return(0);
		case 2:
			if(c >= 'a' && c <= 'z') {
				*q++ = c - 'a' + 1;
				break;
			}
			else return(0);
		case 3:
			if(c >= '0' && c <= '9') {
				*q++ = c - '0'+1;
				break;
			}
			if(c >= 'a' && c <= 'z') {
				*q++ = c - 'a' + 11;
				break;
			}
			else return(0);
		case 4:
			c &= 0x7f;
			if(c>=0x21 && c<=0x7e) {
				*q++ =(c-0x20);
				break;
			}
			else return(0);
		case 5:
			if(c >='0' && c<='9') {
				*q++ = c - '0' + 1;
				break;
			}
			if(c == '-') {
				*q++ = 11;
				break;
			}
			if(c == '+') {
				*q++ = 12;
				break;
			}
			if(c == '.') {
				*q++ = 13;
				break;
			}
			return(0);
		case 6:
			if(c >= '0' && c <= '9') {
				*q++ = c - '0' + 1;
				break;
			}
			if(c >= 'a' && c <= 'z') {
				*q++ = c - 'a' + 11;
				break;
			}
			for(k=0;k<11;k++) if(c==cha_tab[k]) break;
			if(k<11) {
				*q++ = 37 + k;
				break;
			}
			return(0);
		case 7:
			if(c >= 'a' && c <= 'z') {
				*q++ = c - 'a' + 1;
				break;
			}
			if(c >= 'A' && c <= 'Z') {
				*q++ = c - 'Z' + 1;
				break;
			}
			if (c == '-') {
				*q++ = 27;
				break;
			}
			return(0);
		}
	}
	return(1);
}
rpress(fp,c_num,numb,inp)
unsigned char	*fp,*inp;
int  c_num, numb;
{
	unsigned char *q,*p;
	unsigned short	int	md, i,j,l,n;
	unsigned int	k;	/* long i.e. 4bytes*/
	j=c_num-1;
	k=0;
	l=0;
	q=fp;
	p=inp;
	n=numb;
	md=modu[j];
	if (n<=lmax[j][0]) {
		for (i=0; i<n; i++) {
			l *= md;
			l += *q++;
		}
		q= (unsigned char *)&l;
		*p++= *q++;
		*p++= *q;
		return(2);
	} else if (n<=lmax[j][1]) {
		for (i=0; i<n; i++) {
			k *= md;
			k += *q++;
		}
		q = (unsigned char *)&k;
		*p++= *q++, *p++= *q++, *p++= *q++, *p++= *q;
		return(4);
	} else {
		for (i=lmax[j][1]; i>0; i--) {
			k *= md, k += *q++;
		}
		if (n>lmax[j][2]) n=lmax[j][2];
		for (i=n-lmax[j][1]; i>0; i--) {
			l *= md, l += *q++;
		}
		q= (unsigned char *)&l;
		*p++= *q++, *p++= *q;
		q= (unsigned char *)&k;
		*p++= *q++, *p++= *q++, *p++= *q++, *p++= *q;
		return(6);
	}
}

isallkey(c)
char c;
{
	if(isalnum(c)) return(1);
	if(c=='-'||c=='='||c=='['||c==']'||c==';'||c=='\'') return(1);
	if(c=='`'||c=='\\'||c==','||c=='.'||c=='/') return(1);
	return(0);
}

strncmp(p,q,len)
unsigned char	*q,*p;
int	len;
{
	int j;
	for(j=0;j<len;j++,p++,q++) {
		if(*q < *p) return(-1);
		if(*q > *p) return( 1);
	}
	return(0);
}

qwcode(x)
int    x;
{
	int	x1;
	x1=x & 0xf0;
	x1 = (x1>>3) & 0x1e;
	return(x1+(x1<<2)+(x & 0xf) +0xa0);
}

HexcodeToNo(c)
char	c;
{
	return((c<='9')?c-'0':c-'a'+10);
}


/***********************************************
 *  Convert 4-digit to 2-byte internal-code    *
 ***********************************************/

standard(c)
char c;
{
	static char code[4];
	register int	i,k;

	if (!isdigit(c) && c!=8) {
		beep();
		return(0);
	}
	if ((c == 8) && (hcc>0)){
		drawchar(' ',hcc);
		hcc--;
		drawchar('<',hcc);
	}else{
		code[hcc++] = c;
		drawcode(c,hcc);
	}
	if (hcc<4 ) {
		return(0);
	}else{
		for(k=0,i=0; i<4; i++) 
			k = (k<<4) + HexcodeToNo(code[i]);
		hzcode[0]=(qwcode((k >> 8) & 0xff));
		hzcode[1]=qwcode(k & 0xff);
		hcc=0;
		drawcode(c,hcc);
		return(2);
	}
}

