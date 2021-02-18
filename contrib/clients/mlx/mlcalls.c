

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

/* Filename: mlcalls.c  15 Nov. 1989,	By Hu Xian Xiang */

/****************************************
 * This is the source code file of ML	*
 * library which is similar as X11.a.	*
 * ML functions are:			*
 *					*
 *	XSetMLInput()			*
 *	XReadMLCode()			*
 *	XLookforGC()			*
 *	XDrawMLString()			*
 *	XDrawImageMLString()		*
 ****************************************/

#include <stdio.h>
#include <X11/X.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h> 
#include <X11/Xatom.h> 
#include "conf.h" 
#include "atoms.h" 

extern  char *malloc(),*getenv();
Atom	mlcode,mltype;
char	_xiang;

XSetMLInput(dpy,winID)		/* Set ML Input Flag and Clear other*/
Display	*dpy;
Window	winID;
{
	ML_code	data;
	static	char	firstime;
	int	mode;

	data.flag = winID;		/* Send to other clients */
	mlcode= XInternAtom(dpy,hancode1, True);
	mltype= XInternAtom(dpy,hancode2, True);
	XChangeProperty(dpy,DefaultRootWindow(dpy), mlcode, mltype,
	    8, PropModeReplace, &data, sizeof(ML_code));
	_xiang = 1;
	if (!firstime) {
		firstime++;
		XSelectInput(dpy,DefaultRootWindow(dpy),PropertyChangeMask);
	}
}

char *	XReadMLCode(dpy,winID,event)/* Return a string pointer 	*/
Display	*dpy;
Window	winID;
XEvent	event;
{
	int	format,i,j;
	Atom	type;
	ML_code	*retdata;
	Window	root;
	static	char	code[4];

	code[0] = 0;	/* Null string */
	if (! _xiang) return(code);
	root = DefaultRootWindow(dpy);
	switch (event.type) {
	case	PropertyNotify:
		if (event.xproperty.window == root &&
		    event.xproperty.atom   == mlcode)
		{
			if (XGetWindowProperty(dpy, root,
			    mlcode,0L,2L, 0,mltype,
			    &type,&format,&i,&j,&retdata)== Success
			    && type == mltype) {
				if (! retdata->flag){ /* From INPUT WINDOW */
					code[0] = retdata->code[0];
					code[1] = retdata->code[1];
					code[2] = retdata->code[2];
					XFree(retdata);
					return(code);
				}  else	
				if (retdata->flag == winID) /* From Itself */
					return(code);
				else {	/* From Other X Window */
					_xiang = 0;
					return(code);
				}
			}
		}
		else	
		default:
			return(code);
	}
}

/* 	Set new font  for a language */
GC	_OpenGC(fontname,CurDpy,win)
char	*fontname;
Window	win;
Display	*CurDpy;
{
	XFontStruct  *tmpfont_struct;
	XGCValues	gcv;
	unsigned long	gcm,bg,fg;

	if (!strlen(fontname)) return(NULL);
	tmpfont_struct = XLoadQueryFont(CurDpy,fontname);
	if ( tmpfont_struct == NULL ){
		fprintf(stderr,"Can not open font: %s.\n",fontname);
		return(NULL);
	}
	fg = BlackPixel(CurDpy,DefaultScreen(CurDpy));
	bg = WhitePixel(CurDpy,DefaultScreen(CurDpy));
	gcm = 0;
	gcm |= GCFont;
	gcv.font = tmpfont_struct->fid;
	gcm |= GCForeground;
	gcv.foreground = fg;
	gcm |= GCBackground;
	gcv.background = bg;
	/*	fprintf(stderr,"Set up.. %s GC \n",fontname); */
	return(XCreateGC(CurDpy, win, gcm, &gcv));

}

/****************************************
*	Open mlconfig file which is a   *
*	configuration of multilingual   *
*	environment built up by tool	*
*	builder.			*
*	direction returns writing dir.  *
*****************************************/

GC	XLookforGC(display,win,string,direction)
char	*string;
Window	win;
Display	*display;
char	*direction;
{
	char	S_class,lcode;
	Config	buff;
	int	file, i,ling;
	static	GC	mlGC[31];
	char	*p,mlconf[50];
	static	char name[]="/mlconfig";

	lcode = *string;
	i = lcode & 0x3f;
	if (mlGC[i]) return (mlGC[i]);
	ling = i;

        p = getenv("MLDIR");
	for (i=0; mlconf[i]= *p++;i++);
	for (p = name; mlconf[i]= *p++;i++);

	file = open(mlconf,0);
	if (file<0) {
		fprintf(stderr,"Cannot Open ML configuration file. \n");
		return(NULL);
	}
	for(;1;) {
		if (read(file,&buff,REC_LEN) <1 )break;
		if (lcode==(char)0x82) {
			S_class = *(string+1);
			if (buff.class =='S' && buff.code==S_class)
				break;
		} else {
			if (buff.class =='B' && buff.code==lcode)
				break;
		}
	}
	close(file);
	*direction = buff.direction[0];
	mlGC[ling] = _OpenGC(buff.bld_font,display,win);
	return(mlGC[ling]);
}

XDrawImageMLString(display,drawable,x,y,string,length)
Display	*display;
Drawable drawable;
int	x,y;
char	*string;	/* 3-byte string */
int	length;
{
	_DrawMLString(display,drawable,x,y,string,length,0);
}

XDrawMLString(display,drawable,x,y,string,length)
Display	*display;
Drawable drawable;
int	x,y;
char	*string;	/* 3-byte string */
int	length;
{
	_DrawMLString(display,drawable,x,y,string,length,1);
}

/************************************************
 * MLDrawTextFunction 3-byte Internal Coderr	*
 * string is required. GCs and Length are not	*
 * required. OVERWRITE means text will clear	*
 * the existing image.				*
 ************************************************/

_DrawMLString(display,drawable,x,y,string,length,overwrite)
Display	*display;
Drawable drawable;
int	x,y;
char	*string;	/* 3-byte string */
int	length;
int	overwrite;	/* 1 -- no Image */
{
	char	dir,lcode,*str_buf,*str_p;
	int	len;
	GC	tmpgc;

	str_buf= malloc(length);
	len   =length;
	str_p = str_buf;
	lcode = *string;
	for (;len>0;len -= 3) {
		if (lcode == *string) {
			*str_buf++ = *++string;
			*str_buf++ = *++string;
			*str_buf = 0;
			string++;
			continue;
		} else {
			*str_buf = 0;
	    		tmpgc = XLookforGC(display,drawable,string-3,&dir);
			_drawMLstring(display,drawable,tmpgc,dir,
			    x,y,str_p,strlen(str_p)*2,overwrite);
			str_buf = str_p;
		}
	}
	if(str_buf != str_p) {
	    	tmpgc = XLookforGC(display,drawable,string-3,&dir);
		_drawMLstring(display,drawable,tmpgc,dir,
	    		x,y,str_p,strlen(str_p)*2,overwrite);
	}
	free(str_p);
}


/****************************************
*	FUNCTION:			*
*	Draw non-ASCII string in screen *
*	The string includes:		*
*	--- Latin class			*
*	One font for whole L-class	*
*	--- Small class			*
*	One font per language in S-class*
*	--- Big   class			*
*	One font per language in B-class*
*****************************************/

_drawMLstring(display,drawable,gc,direction,x,y,string,length,true)
Display	*display;
Drawable drawable;
GC	gc;
int	x,y;
char	*string;	/* 2-byte string */
int	length;		/* Char2b string */
int	true;		/* 1 -- no Image */
char	direction;	/* direction: L,R,U */
{
	char	lcode,*str_buf,*str_p;
	int	i,height;

	if (gc==NULL) return;
	str_buf = malloc(length*2);
	str_p = str_buf;
	lcode = *string;
	if ((lcode==(char)0x80)|| (lcode==(char)0x81)) {   /* L-Class    */
		for (i=length;i>0;i--)
			if ((*string++) == (char) 0x81)
				*str_p++ = *string++;
			else	*str_p++ = (*string++) & 0x7f;
		if (true)
			XDrawString(display,drawable,gc,x,y,str_buf,length);
		else
			XDrawImageString(display,drawable,gc,x,y,str_buf,length);
		return;
	}
	if (lcode == (char)0x82){	/* S-Class  */
		switch(direction){
		case	'R':
			for (i=length,string += length*2;i>0;i--,string--)
				*str_p++ = *(--string) & 0x7f;
			break;
		case	'U':
			for (i=length,height=y;i>0;i--){
				*str_p = (*string) & 0x7f;
				if (true)
					XDrawString(display,drawable,gc,
					    x,height,str_buf,1);
				else
					XDrawImageString(display,drawable,gc,
					    x,height,str_buf,1);
				string += 2;
				height += 16;	/* Default */
			}
			return;
		case	'L':
		default:
			for (i=length;i>0;i--,string +=2)
				*str_p++ = (*string) & 0x7f;
			break;
		}
		if (true)
			XDrawString(display,drawable,gc,x,y,str_buf,length);
		else
			XDrawImageString(display,drawable,gc,x,y,str_buf,length);
		return;
	}
	/*	Big Class */
	switch(direction){
	case	'R':
		for (i=length,string += length*2;i>0;i--){
			*str_p++ = *(string-2);
			*str_p++ = *(string-1);
			string -= 2;
		}
		break;
	case	'U':
		for (i=length,height=y;i>0;i--){
			*str_p = *string;
			if (true)
				XDrawString16(display,drawable,gc,
				    x,height,(XChar2b *)str_buf,1);
			else
				XDrawImageString16(display,drawable,gc,
				    x,height,(XChar2b *)str_buf,1);
			string += 2;
			height += 16;	/* Default */
		}
		return;
	case	'L':
	default:
		break;
	}
	if (true)
		XDrawString16(display,drawable,gc,x,y,
		    (XChar2b *)string,length);
	else
		XDrawImageString16(display,drawable,gc,x,y,
		    (XChar2b *)string,length);
	return;
}

