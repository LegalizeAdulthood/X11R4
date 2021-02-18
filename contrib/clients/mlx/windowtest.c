

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

/* Filename: windowtest.c  15 Nov. 1989,	By Hu Xian Xiang */

#include <X11/X.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <X11/StringDefs.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <Xw/Form.h>
#include <Xw/WorkSpace.h>
#include "cursor.h"
#include "example.h"

main(argc, argv)
int argc;
char **argv;
{
	XEvent	event;
	int	i,j;
	
	static Arg setargs[]={
		{XtNborder, 0 },
		{XtNwidth, 300},
		{XtNheight,300}
	};	

	text_buf = text_p =malloc(5120);
	i = open(".TEXT", O_RDONLY | O_CREAT , 0666);
	j = read(i,text_buf,5120);
	close(i);
	text_p += j;
	toplevel = XtInitialize( "sample", "Sample", NULL,
		 0,&argc, argv);
	XtSetValues(toplevel, setargs, XtNumber(setargs));
	CurDpy = XtDisplay(toplevel);
	screen = DefaultScreen(CurDpy);

	frame = XtCreateManagedWidget( "frame", XwformWidgetClass, toplevel,
	    setargs,XtNumber(setargs));
	tmp = XtCreateManagedWidget("textarea", XwworkSpaceWidgetClass, 
				frame, setargs,XtNumber(setargs));

	XtAddEventHandler(frame,KeyReleaseMask,0,HandleKeyPress,NULL);
	XtAddEventHandler(frame,ExposureMask,       0,Refresh,  NULL);
	XtAddEventHandler(tmp,  ButtonPressMask,    0,MLinput,  NULL);
	XtAddEventHandler(frame,StructureNotifyMask,0,Setsize,  NULL);
	XtAddEventHandler(toplevel,StructureNotifyMask,0,Refresh,NULL);

	XtRealizeWidget(toplevel);
	TextWindow= XtWindow(tmp);
	XDefineCursor(XtDisplay(toplevel),XtWindow(toplevel),
	    XCreateMLC_Cursor());
/*	XtMainLoop();	*/
	while(1) {
		XtNextEvent(&event);
		if (event.type == PropertyNotify)
			if (event.xproperty.window==DefaultRootWindow(CurDpy)){
				HandleMLCode(event);
				continue;
			}
		XtDispatchEvent(&event);
	}
}

/************************************************
 * Creat input window special mouse cursor 	*
 * Cursor shape is defined in cursor.h		*
 * which is genrated by bitmap command. 	*
 * ML_XTERM also uses this cursor.		*
 * Please see function "make_hanzi_cursor"  in	*
 * misc.c of ML_XTERM source code.		*
 ************************************************/

XCreateMLC_Cursor( )
{
	Cursor c;
	XColor bg,fg,real_color;
	Pixmap source,mask;
	Colormap   def_cm; 
	int	iscolor;


	source = XCreateBitmapFromData(CurDpy,XtWindow(toplevel),
			tmp1_bits,tmp1_width,tmp1_height);
	mask = NULL;
	if (source==NULL ) {
		printf("Make hanzi mouse cursor unsuccessful.\n");
		return(XCreateFontCursor( XtDisplay(toplevel), XC_heart)); 
	}

	def_cm = DefaultColormap(CurDpy, screen);
	if (DisplayCells(CurDpy, screen) > 2)
		iscolor = 1;
	else
		iscolor = 0;

	if (iscolor) {
		XAllocNamedColor (CurDpy, def_cm, "blue", &bg, &real_color);
		XAllocNamedColor (CurDpy, def_cm, "white",&fg, &real_color);
	} else {
		XAllocNamedColor (CurDpy, def_cm, "black",&bg, &real_color);
		XAllocNamedColor (CurDpy, def_cm, "white",&fg, &real_color);
	}
	c = XCreatePixmapCursor(CurDpy,source,mask,
		&bg,&fg,tmp1_width/2,tmp1_height/2);
	return(c);
}

HandleMLCode(event)
XEvent	event;
{
	char	*mlcode,cursor_c;

	mlcode = (char *)XReadMLCode(CurDpy,TextWindow,event);
	if (*mlcode) {
		*text_p++ = *mlcode;
		*text_p++ = *(mlcode+1);
		*text_p++ = *(mlcode+2);
		*text_p = 0;
		XDrawImageMLString(CurDpy,TextWindow,x,y+1,mlcode,3);
		x += 24;
		if (x > w90) {
			y += 24;
			x =0;
		}
	}
	cursor_c = 0x7f;
	XDrawImageString(CurDpy,TextWindow,
		DefaultGC(CurDpy,screen),x,y,&cursor_c,1);
}

MLinput(widg,closure,event)
Widget	widg;
caddr_t closure;
XButtonEvent	*event;
{
	int	i,j;

	if (event->button == Button3) { /* Save text */
		i = open(".TEXT",O_WRONLY | O_TRUNC);
		j = write(i, text_buf,text_p-text_buf);
		close(i);
		return;
	} 
	if (event->button == Button2) {	/* Clear text */
		text_p = text_buf;
		*text_buf = 0;
		XClearWindow(CurDpy,TextWindow);
		Refresh();
		return;
	}
	if (event->button == Button1)	/* Input ML text */
		XSetMLInput(CurDpy,TextWindow);
	return;
}

HandleKeyPress(widg,closure,event)
Widget	widg;
caddr_t closure;
XKeyPressedEvent  *event;
{
	char	strbuf[2];
	char	cursor_c;
	int	i,nbyte;
	KeySym	keycode;
	static  XComposeStatus	compose_status = {
		NULL,0		};

	nbyte = XLookupString(event,strbuf,2,&keycode,&compose_status);

	XDrawImageString(CurDpy,TextWindow,
		DefaultGC(CurDpy,screen),x,y,strbuf,nbyte);
	x += 10*nbyte;
	for (i=0;nbyte--;)
		*text_p++ = strbuf[i++];
	*text_p = 0;
	if (x > w90) {
		y += 24;
		x =0;
	}
	cursor_c = 0x7f;
	XDrawImageString(CurDpy,TextWindow,
		DefaultGC(CurDpy,screen),x,y,&cursor_c,1);
	return;
}

Setsize()
{
	Dimension	x1;
	Arg	arg[2];

	XtSetArg(arg[0],XtNwidth ,&w90);
	XtSetArg(arg[1],XtNheight,&x1);
	XtGetValues(frame,arg,2);
	XtSetArg(arg[0],XtNwidth ,w90);
	XtSetArg(arg[1],XtNheight,x1);
	XtSetValues(tmp,arg,2);
	Refresh();
}

Refresh(wid,closure,event)
Widget	wid;
caddr_t closure;
XEvent	*event;
{
	char	*p,cursor_c;
	int	x1,y1;

	y1 =20; x1=0;
	for(p=text_buf;*p;) 
		if( *p & 0x80) {	 /* ML Code */
		    XDrawImageMLString(CurDpy,TextWindow,x1,y1+1,p,3);
			p +=3;
			x1 +=24;
			if (x1 >w90) {
				y1+=24;
				x1 = 0;
			}
		}else {		/* ASCII */
			XDrawImageString(CurDpy,TextWindow,
			   DefaultGC(CurDpy,screen),x1,y1,p,1);
			p++;
			x1 +=10;
			if (x1 >w90){
				y1+=24;
				x1 = 0;
			}
		}
	x = x1; y = y1;
	cursor_c = 0x7f;
	XDrawImageString(CurDpy,TextWindow,
		DefaultGC(CurDpy,screen),x,y,&cursor_c,1);
}
/* end of file */
