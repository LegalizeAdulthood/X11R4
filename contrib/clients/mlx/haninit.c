

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

/* Filename: haninit.c  15 Nov. 1989,	By Hu Xian Xiang */

#include	<stdio.h>
#include 	<X11/X.h>
#include 	<X11/Xos.h>
#include 	<X11/Xlib.h>
#include	"ptyx.h"
#include	"conf.h"
#include	"cursor.h"

#ifdef	HANZI
extern		XtermWidget	term;
ML_FONT		L_FONT[32];
ML_CODE		GC_CODE[32];
int		ML_MAX;

/****************************************
* ASSUME only less than 32 languages 	*
* exist at the same time.		*
* But they can be any languages. 	*
* Inside the XTERM, each language has 	*
* a language number from 0x80 to 0xa0   *
*****************************************/


/*
 * Set and initilize all the languages font, style, etc.
*/
FontsInit(screen)
TScreen	*screen;
{
	int	i,j,main_f;
	Config	b;

	ML_MAX=0;
	main_f = open("mlconfig",2);	/* Read & Write */
	if (main_f < 0) {
		if(chdir(getenv("MLDIR"))) {
	    		fprintf(stderr,
				"Cannot access $MLDIR environment variable\n");
			return;
		}
		main_f = open("mlconfig",2);	/* Read & Write */
		if (main_f < 0) {
			fprintf(stderr,
		    	"Cannot open main configuration file.\n");
			chdir(getenv("PWD")) ;
			return;
		}
	}

/*	printf("\nThe MLTERM can process these languages: \n");	*/
	for (i=0; read(main_f,&b,REC_LEN)==REC_LEN;){
		if (!(b.code&0x80)) continue;
		if (b.class == 'L') {
			L_FONT[i].l_no   = ON;
			L_FONT[i].l_dir  = 'L'; /* L, R, U, D */
			L_FONT[i].l_fontid = screen->fnt_norm->fid;
			L_FONT[i].l_bold_fontid = screen->fnt_bold->fid;
			L_FONT[i].width =  screen->fullVwin.f_width * 2;
			L_FONT[i].height=  screen->fullVwin.f_height* 2;
		}else {
			L_FONT[i].l_no   = OFF;
			L_FONT[i].l_dir  = b.direction[0]; /* L, R, U, D */

			for (j=0; L_FONT[i].l_boldname[j]= b.nor_font[j];j++);
			/* nor_font is used by input window */

			for (j=0; L_FONT[i].l_fontname[j]= b.bld_font[j];j++);
			/* bld_font is used by xterm window */

			GCset(screen,i);
		}
		GC_CODE[i].language=b.code;
		GC_CODE[i].class   =b.class;
		ML_MAX++;i++;
/*		printf("%s.\n",b.name);	*/
	}
	close(main_f);
	chdir(getenv("PWD")) ;
}

GCset(screen,langno)
int	langno;
TScreen *screen;
{
	XFontStruct  *tmpfont_struct;
	char	*fontname;

	if (L_FONT[langno].l_no == ON)  return;
	L_FONT[langno].l_no = ON;
	fontname = L_FONT[langno].l_fontname;
	if (! strlen(fontname) ) {
		L_FONT[langno].l_fontid = screen->fnt_norm->fid;
		L_FONT[langno].l_bold_fontid = L_FONT[langno].l_fontid ; 
		L_FONT[langno].width =  screen->fullVwin.f_width * 2;
		L_FONT[langno].height=  screen->fullVwin.f_height* 2;
		return;
	}

	tmpfont_struct = XLoadQueryFont(screen->display,fontname);
	if ( tmpfont_struct == NULL ){
		printf("Could not open font: %s.\n",fontname);
		L_FONT[langno].l_fontid = screen->fnt_norm->fid;
		L_FONT[langno].width =  screen->fullVwin.f_width * 2;
		L_FONT[langno].height=  screen->fullVwin.f_height* 2;
	}else {
		L_FONT[langno].l_fontid = tmpfont_struct->fid;
		L_FONT[langno].width =  tmpfont_struct->max_bounds.rbearing-
					tmpfont_struct->min_bounds.lbearing;
		L_FONT[langno].height=  tmpfont_struct->max_bounds.ascent +
					tmpfont_struct->max_bounds.descent;
	}

	fontname = L_FONT[langno].l_boldname;	/* Bold font */
	tmpfont_struct = XLoadQueryFont(screen->display,fontname);
	if ( tmpfont_struct == NULL ){
		printf("Could not open FONT: %s.\n",fontname);
		L_FONT[langno].l_bold_fontid = L_FONT[langno].l_fontid ; 
	}else {
		L_FONT[langno].l_bold_fontid = tmpfont_struct->fid;
	}
}

/********Converts Internal Code to GC number ********
* INPUT:					    *
*	Standard ML Internla Code: 3-byte string    *
* 	MLcode --- 3-byte: 0x81  0x80/1  xx	    *
*			   0x82  Lcode   xxx	    *
*			   Lcode 0xa0~   xx	    *
*	Lcode ---language code in the configuration *
*		 file, and it begin with 0x83.	    *
* OUTPUT:					    *
*	GCcode --- 3-byte: [0x80~0x9f],  xxx , xxx  *
*			   [0x80~0x9f],  0x80, xxx  *
*			   [0x80~0x9f],  0x82, xxx  *
*****************************************************/

GCcode(str)
unsigned char	*str;
{
	register int	i,j;
	unsigned char	*mlcode,lcode, lingual, class;

	mlcode = str;
	for (j=strlen(mlcode); j>2 ; j--, mlcode++){
		lcode = *mlcode;
		if (!(lcode & 0x80)) continue;	/* ASCII */
		lingual  = lcode &0x7f;
		if (lingual == 2 ) {
				lcode= *(mlcode+1);
				*(mlcode+1) = 0x82;	/* S-class */
		}
		for (i=0; i<ML_MAX; i++)
		if (GC_CODE[i].language==lcode) {
			*mlcode = i|0x80;	/* New GC-CODE */
			break;
		}
		j -= 2; mlcode += 2;	/* totally  -3 & +3  */
	}
}
		
/********Converts GC Code into MLingual Code*********
* INPUT:					    *
*	GCcode --- 3-byte: [0x80~0x9f], xxx , xxx   *
* OUTPUT:					    *
* 	MLcode --- 3-byte: 0x81  0x80/1  xx	    *
*			   0x82  0x81~   xxx	    *
*			   0x83~ 0xa0~   xx	    *
*****************************************************/

MLcode(GCcode)
char	*GCcode;
{
	register int	i,j,len;
	register char	class;
	char	*gccode;

	gccode = GCcode;
	for (j=strlen(gccode); j>2 ; j--, gccode++){
		if (!(*gccode & 0x80)) continue;	/* ASCII */
		i = (*gccode) & 0x7f;
		class = GC_CODE[i].class;
		if (class == 'B' )
			*gccode = GC_CODE[i].language;
		else 
			if (class == 'L')
				*gccode = 0x81;
			else {
				*gccode = 0x82;
				*(gccode+1) = GC_CODE[i].language;	
			}
		j -= 2; gccode += 2;	/* totally  -3 & +3  */
	}
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

DrawMLString(display,drawable,gc,x,y,string,length,true)
Display	*display;
Drawable drawable;
GC	gc;
int	x,y;
char	*string;	/* 2-byte string */
int	length;		/* XChar2b	 */
int	true;		/* 1 -- no Image */
{
	char	lcode,*str_buf,*str_p;
	int	i;

	str_buf = (char *) malloc(length*2);
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
		for (i=length;i>0;i--,string +=2)
			*str_p++ = (*string) & 0x7f;
		if (true)
			XDrawString(display,drawable,gc,x,y,str_buf,length);
		else
		    XDrawImageString(display,drawable,gc,x,y,str_buf,length);
		return;
	}
	/*	Big Class */
	if (true)
		XDrawString16(display,drawable,gc,x,y,
			(XChar2b *)string,length);
	else
		XDrawImageString16(display,drawable,gc,x,y+2,
			(XChar2b *)string,length);
	return;
}

extern	char _xiang;	/* Defined in ML Xlib */

MLCodeInput(event)
XEvent	event;
{
	register TScreen *screen = &term->screen;
	int	pty = term->screen.respond;
	char	*mlbuf;
	int  	j;

	mlbuf = (char *)XReadMLCode(screen->display,VWindow(screen),event);
	if (!_xiang){		/* Clear 'ML' flag in the top-left corner */
		GC      currentGC;
		int	flags;
	    	unsigned long fg, bg;

	    	bg = term->core.background_pixel;
	    	if (screen->mousecolor == term->core.background_pixel) {
			fg = screen->foreground;
	    	} else {
			fg = screen->mousecolor;
	    	}
		ScrnRefresh(screen,0,0,2,2); /* recover normal chars */
		XDefineCursor(screen->display,TextWindow(screen),
			make_xterm(fg,bg));
		return;
	}
	if (j = strlen(mlbuf))
		if (write(pty, mlbuf, j) != j)
			Panic("Can't write to pty.\n", 0);
}


Cursor make_hanzi_cursor(fg,bg)
unsigned long fg,bg;
{
	register TScreen *screen = &term->screen;
	register Display *CurDpy = screen->display;
	Cursor c;
	XColor bg1,fg1,real_color;
	Pixmap source,mask;
	Colormap   def_cm; 
	int	iscolor,scr;

	source = XCreateBitmapFromData(CurDpy,VWindow(screen),
			tmp1_bits,tmp1_width,tmp1_height);
	mask = NULL;

	scr = DefaultScreen(CurDpy);
	def_cm = DefaultColormap(CurDpy, scr);
	if (DisplayCells(CurDpy, scr) > 2)
		iscolor = 1;
	else
		iscolor = 0;

	if (iscolor) {
		XAllocNamedColor (CurDpy, def_cm, "blue", &bg1, &real_color);
		XAllocNamedColor (CurDpy, def_cm, "white",&fg1, &real_color);
	} else {
		XAllocNamedColor (CurDpy, def_cm, "balck",&bg1, &real_color);
		XAllocNamedColor (CurDpy, def_cm, "white",&fg1, &real_color);
	}
	c = XCreatePixmapCursor(CurDpy,source,mask,
		&bg1,&fg1,tmp1_width/2,tmp1_height/2);
	return(c);
}
	
SetHanziRec()
{
	register TScreen *screen = &term->screen;
    	unsigned long fg, bg;

	XSetMLInput(screen->display, VWindow(screen));
	bg = term->core.background_pixel;
	if (screen->mousecolor == term->core.background_pixel) {
		fg = screen->foreground;
	} else {
		fg = screen->mousecolor;
	}
	XDefineCursor(screen->display,TextWindow(screen),
		make_xterm(fg,bg));
}

#endif	/* HANZI */
