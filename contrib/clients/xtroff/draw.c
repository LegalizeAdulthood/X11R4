/*
 *	SunTroff -  A program to display the output of Device Independent
 *		Troff as a window on a Sun Workstation.
 *
 *	Troff_draw.c - Code to do the draw the simple stuff in the page
 *		bit map.
 *
 *	Authors - The original version of this program was written by
 *			Richard L. Hyde (Purdue)
 *			David Slattengren (Berkeley)
 *		It was nearly all rewritten, cleaned up and a more elegant
 *		user interface installed by
 *			Malcolm Slaney (Schlumberger Palo Alto Research)
 *		Combine characters into words
 *			David Cahlander (Cray Research, Inc.)
 *
 *	Legalese -  This command was developed as an independent project
 *		to satisfy a need of the author.  This program may contain
 *		bugs and the user is cautioned to independently verify that
 *		the program is suitable for the user's intended purpose.
 *		The program is made available on an ``as is'' basis with
 *		all faults and without any implied or expressed warranties
 *		or support from either the author, Malcolm Slaney, or
 *		Schlumberger Palo Alto Research Laboratory.
 *
 *		I am putting this program in the public domain.  You are
 *		free to use it as you wish.  In return I ask two things.
 *		First, that you do not remove the names of the authors
 *		from this work.  Secondly, if you make changes or
 *		improvements to this program that you pass these back to
 *		the author so that everybody can benefit from the
 *		improvements.
 *
 *					Malcolm Slaney  (December 1986)
 *					Schlumberger Palo Alto Research
 *					3340 Hillview Avenue
 *					Palo Alto, CA 94304
 *					(415) 496-4669
 *					spar!malcolm@decwrl.dec.com
 *					malcolm@ecn.purdue.edu
 *					malcolm@spar.slb.com (Someday)
 */

#include	"suntroff.h"
#ifdef SUNTOOLS
#include	<suntool/sunview.h>
#include	<suntool/panel.h>
#include	<suntool/canvas.h>
#include	<suntool/scrollbar.h>

extern	Frame	BaseFrame;		/* Top Level Window Frame */
extern	Canvas	DrawingCanvas;		/* Main Drawing Window for Text */
extern	Menu	MainMenu;		/* Top Level User Menu */
#else SUNTOOLS
#include <X11/Intrinsic.h>
#include <math.h>
extern Window DrawingCanvas;
extern Widget CanvasWidget;
extern Display *dpy;
extern GC gc;
extern GC fillgc;
extern GC cleargc;
extern Pixmap PagePixRect;
extern int ViewTop, ViewLeft, ViewHeight, ViewWidth;
#endif SUNTOOLS
extern int SUNRES;
extern	int	DisplayOutput;		/* Display the output for user? */

#define	FastScale(x)	(((x) * SUNRES) / UserTypesetter->Device.res)

Scale(x)
int	x;
{
	return(((x) * SUNRES) / UserTypesetter->Device.res);
}

ClearPage(){
#ifdef DEBUG
	printf("clearing page\n");
#endif /* DEBUG */
#ifdef SUNTOOLS
	extern struct pixrect *PagePixRect;

	pr_rop(PagePixRect,0,0,1000000,1000000,PIX_CLR,NULL,0,0);
#else /* !SUNTOOLS */
	XFillRectangle(dpy, PagePixRect, cleargc, 0, 0, 
	 (unsigned int) PAGE_PIXEL_WIDTH, (unsigned int) PAGE_PIXEL_HEIGHT);
#endif /* !SUNTOOLS */
}

RefreshPage(){
#ifdef DEBUG
	printf("refreshing page\n");
#endif
#ifdef SUNTOOLS
	RepaintCanvas(DrawingCanvas,canvas_pixwin(DrawingCanvas),0);	
#else
	RepaintCanvas(CanvasWidget, (caddr_t) NULL, (XEvent *) NULL);
#endif 
}

#ifndef SUNTOOLS
DrawString(x,y,s)
int	x, y;
char	*s;
{
	if (!CurrentFont)
		fatal("Don't have a Current Font.\n");

	if (!CurrentFont->Bits)
		fatal("Don't have the bits for the Current Font.\n");

	XDrawString(dpy, PagePixRect, gc, FastScale(x), FastScale(y), s, strlen(s));
}
#endif /* SUNTOOLS */

/*ARGSUSED*/
DrawCharacter(x,y,c,cwidth)
int	x, y, c, cwidth;
{
#ifdef SUNTOOLS
	struct pr_prpos where;
	char	Text[2];
	extern struct pixrect *PagePixRect;

	if (!CurrentFont){
		fatal("Don't have a Current Font.\n");
	}

	if (!CurrentFont->Bits){
		fatal("Don't have the bits for the Current Font.\n");
	}

	where.pos.x = FastScale(x);
	where.pos.y = FastScale(y);
	where.pr = PagePixRect;

	Text[0] = c;
	Text[1] = 0;

	if (DisplayOutput)
		pf_text(where,PIX_SRC|PIX_DST,CurrentFont->Bits,Text);
#else
	static char s[2] = " ";
	int dx = 0;
	
	if (!CurrentFont){
		fatal("Don't have a Current Font.\n");
	}

	if (!CurrentFont->Bits){
		fatal("Don't have the bits for the Current Font.\n");
	}

	if (DisplayOutput) {
		s[0] = c;
#ifndef NOADJUST
		/* 
		 *  This kludge tries to centre the X char within the
		 *  troff char space. Looks a bit better. Thanks to Dave
		 *  Blythe, U of Toronto.
		 */
		dx = (CurrentFont->Bits->per_char)? 
		 CurrentFont->Bits->per_char[c-1].width : 
		 CurrentFont->Bits->min_bounds.width;
		dx = (FastScale(cwidth) - dx)/2;
#ifdef FONTDEBUG
		printf("char \"%c\"(0x%x), dx = %d, xwid = %d (lb,rb = %d,%d), cwid = %d (%d scaled)\n",
		 c, c, dx, CurrentFont->Bits->per_char[c-1].width,
		 CurrentFont->Bits->per_char[c-1].lbearing,
		 CurrentFont->Bits->per_char[c-1].rbearing,
		 cwidth, FastScale(cwidth));
#endif
#endif NOADJUST
		XDrawString(dpy, PagePixRect, gc, 
		 FastScale(x) + dx, 
		 FastScale(y), s, 1);
	}
#endif
}


DrawLine(x,y)
{
	if (DisplayOutput)
#ifdef SUNTOOLS
		pr_vector(PagePixRect,
			  FastScale(HorizontalPosition),
			  FastScale(VerticalPosition),
			  FastScale(HorizontalPosition+x),
			  FastScale(VerticalPosition+y),
			  PIX_SET,1);
#else /* !SUNTOOLS */
#ifdef DEBUG
		printf("Drawline to %d, %d\n", FastScale(HorizontalPosition + x), FastScale(VerticalPosition + y));
#endif /* DEBUG */
		XDrawLine(dpy, PagePixRect, gc, 
			  FastScale(HorizontalPosition),
			  FastScale(VerticalPosition),
			  FastScale(HorizontalPosition+x),
			  FastScale(VerticalPosition+y));
#endif /* !SUNTOOLS */
	HorizontalPosition += x;
	VerticalPosition += y;
}

#ifndef SUNTOOLS
box(x1, y1, x2, y2)
float	x1, y1, x2, y2;
{
	int i1, j1, i2, j2;

	i1 = round(ditsiz * x1);
	j1 = round(ditsiz * y1);
	i2 = round(ditsiz * x2);
	j2 = round(ditsiz * y2);
	XFillRectangle(dpy, PagePixRect, gc, 
	    FastScale(HorizontalPosition) + i1,
	    FastScale(VerticalPosition) + j1,
	    (unsigned int) (i2 - i1), (unsigned int) (j2 - j1));
}

line(x1, y1, x2, y2)
float	x1, y1, x2, y2;
{
	int i1, j1, i2, j2;

	i1 = round(ditsiz * x1);
	j1 = round(ditsiz * y1);
	i2 = round(ditsiz * x2);
	j2 = round(ditsiz * y2);
	XDrawLine(dpy, PagePixRect, gc, 
	    FastScale(HorizontalPosition) + i1,
	    FastScale(VerticalPosition) + j1,
	    FastScale(HorizontalPosition) + i2,
	    FastScale(VerticalPosition) + j2);
}

round(x)
float	x;
{
	if (x < 0)
		return(x - .5);
	else
		return(x + .5);
}

draw_fraction(num, den, size)
char num, den;
int size;
{
	char s[2];

	SetFontSize(round(0.6 * size));
	LoadFontBits();
	s[0] = num;
	XDrawString(dpy, PagePixRect, gc,
	 FastScale(HorizontalPosition), 
	 FastScale(VerticalPosition) - round(0.3 * size), s, 1);
	s[0] = den;
	XDrawString(dpy, PagePixRect, gc,
	 FastScale(HorizontalPosition) + round(0.5 * size),
	 FastScale(VerticalPosition), s, 1);
	SetFontSize(size);
	LoadFontBits();
	s[0] = '\244';
	XDrawString(dpy, PagePixRect, gc,
	 FastScale(HorizontalPosition) + round(.16 * ditsiz), 
	 FastScale(VerticalPosition), s, 1);
}


#endif /*SUNTOOLS*/

point(x,y)
int	x, y;
{
	if (DisplayOutput)
#ifdef SUNTOOLS
		pr_put(PagePixRect,x,y,1);
#else
		XDrawPoint(dpy, PagePixRect, gc, x, y);
#endif
}

#ifndef SUNTOOLS
/*
 *  draw a circle
 *	x1 - x position in character box (0. - 1.)
 *	y1 - y position in character box (0. - 1.)
 *	r  - radius of arc (0. - 1.)
 *	fill - TRUE if filled circle
 */

circle(x1, y1, r, fill)
float	x1, y1, r;
int	fill;
{
	arc(x1, y1, r, 0, 360, fill);
}

/*
 *  draw an arc
 *	x1 - x position in character box (0. - 1.)
 *	y1 - y position in character box (0. - 1.)
 *	r  - radius of arc (0. - 1.)
 *	a1 - start angle of arc (degrees)
 *	a2 - length of arc (degrees)
 *	fill - TRUE if filled arc
 */

arc(x1, y1, r, a1, a2, fill)
float	x1, y1, r;
int	a1, a2;
int	fill;
{
	int i1, j1, r1;

	i1 = round(ditsiz * x1);
	j1 = round(ditsiz * y1);
	r1 = round(ditsiz * r);
	if (fill)
		XFillArc(dpy, PagePixRect, fillgc,
		    FastScale(HorizontalPosition) + i1 - r1,
		    FastScale(VerticalPosition) + j1 - r1,
		    (unsigned int) (2 * r1), (unsigned int) (2 * r1),
		    a1 * 64, a2 * 64);
	else
		XDrawArc(dpy, PagePixRect, gc,
		    FastScale(HorizontalPosition) + i1 - r1,
		    FastScale(VerticalPosition) + j1 - r1,
		    (unsigned int) (2 * r1), (unsigned int) (2 * r1),
		    a1 * 64, a2 * 64);
}
#endif /*SUNTOOLS*/
