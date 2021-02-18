/*

    Copyright (c) 1989 University of Southern California

	Paul Raveling
	USC Information Sciences Institute
	4676 Admiralty Way
	Marina del Rey, California 90292


	The authors grant without fee permission to use, copy, modify,
	distribute, and sell this software for any purpose, provided:

	--  The above copyright notice appears in all copies.

	--  This disclaimer appears in all source code copies.

	--  The names of ISI and USC are not used in advertising or
	    publicity  pertaining  to distribution  of the software
	    without prior specific written permission from ISI.

	ISI provides this software freely as a public service.  It is
	NOT a commercial product,  and therefore is not subject to an
	an implied  warranty  of  merchantability  or  fitness  for a
	particular purpose.  ISI provides it as is, without warranty.

	This software  is furnished  only on the basis that any party
	who receives it indemnifies  and holds harmless  the original
	authors and their sponsors  against any claims,  demands,  or
	liabilities  connected  with  using  it,   furnishing  it  to
	others, or providing it to a third party.


	The author  of this software is  Paul Raveling.   Development
	used the author's after-work hours and ISI's facilities.

	Thanks are due to ISI for allowing  such extracurricular work
	and  to Hewlett Packard,   whose  University  Grants  program
	provided  the 9000/370  workstation  and  supporting software
	which was the platform for this work.
*/




/*0
======================================================================
======								======
======		Program excolors:	Examine Colors		======
======			      File dispimg.c			======
======								======
======================================================================

	See manual entry for info.


  Module history:

	11-28-89	Paul Raveling
	Updated copyright/legal disclaimer notice to suit
	X Consortium requirements

	11-27-89	Paul Raveling
	Finalized version 1.1 for contribution to X11R4.

	Early 1989	Paul Raveling
	Created original file without module header
*/



#include <stdio.h>

#include <imglib/imglib.h>	/*  Include sys-independent defs	*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>		/*  Include X11 definitions		*/
#include <X11/Xos.h>
#include <X11/keysym.h>

#include <imglib/imgXlib.h>	/*  Include sys-dependent defs for X11	*/


#define	PATCHWIDTH	150
#define	PATCHHEIGHT	25
#define	TEXTWIDTH	300
#define	RGBOFF		150
#define	NUMPATCHES	14
#define	IMWIDTH		(PATCHWIDTH+TEXTWIDTH)
#define	IMHEIGHT	(PATCHHEIGHT*NUMPATCHES)
#define	IMSIZE		(IMWIDTH*IMHEIGHT)

static	int	 status;
static	int	 ic;
static	char	 c;


Byte	 cvu(f)

	FILE	*f;

{
	Byte	 uval;

uval	= 0;

while ((ic != EOF) && ((c == ' ')||(c == '\t')||(c == '\n')||(c == '\r')))
    {
    ic = getc(f);
    c = ic;
    }

if ((c < '0') || (c > '9'))
   return 0;

while ((ic != EOF) && (c >= '0') && (c <= '9'))
    {
    uval *= 10;
    uval += c - '0';
    ic = getc(f);
    c = ic;
    }

++ status;
return uval;
}


Byte	 cvs(f,s)

	FILE	*f;
	char	*s;

{

while ( ic != EOF )
    {
    if ((c != ' ') && (c != '\t'))
	break;
    ic = getc(f);
    c = ic;
    }

if ((c == '\n') || (c == '\r'))
   {
   *s = 0;
   return;
   }

while ((ic != EOF) && (c != '\n') && (c != '\r'))
    {
    *s++ = c;
    ic = getc(f);
    c = ic;
    }

*s = 0;
++ status;
return;
}



main(argc,argv)

	int	 argc;
	char	*argv[];

{

	Img	*im;
	X11env	*X11_env;
	XEvent	*event;
   XExposeEvent *expose;	/*  Pointer for expose events	*/
   Sir	*sir;			/*  Pointer to sys resources	*/
	Col	 colormap[NUMPATCHES+2];
	Byte	*pixdata;
	Byte	*pp;

register int	 x, y;

	FILE	*rgbdb;
	struct	 dbent
		{
		struct dbent	*next;
		struct dbent	*prev;
		char		*name;
		Col		 color;
		};
  struct dbent	*first_col;
  struct dbent	*last_col;
  struct dbent	*top_col;
register  struct dbent	*cur_col;
	Word	 r, g, b;
	Byte	 curname[100];

	int	 i;
	Display	*dsp;
	Pixmap	 pix;
	Window	 win;
	GC	 gc;

static	KeySym	noshifts[2]	= {0, 0};



/*:::::::::::::::::::::::::::::::::::::::::*/
/*	Macro to abort with a message	   */
/*:::::::::::::::::::::::::::::::::::::::::*/

#define	abort(msg) {fprintf(stderr,"excolors:  %s\n",msg); exit(2);}



/*------------------------------*/
/*	Create basic image	*/
/*------------------------------*/

pixdata = (Byte *) malloc ( IMWIDTH * IMHEIGHT );
if ( pixdata == NULL )
   abort("Insufficient memory")

colormap[0].red   = 0;
colormap[0].green = 0;
colormap[0].blue  = 0;
colormap[1].red   = 255;
colormap[1].green = 255;
colormap[1].blue  = 255;



im = create_img ( mapped_image, IMWIDTH, IMHEIGHT, 0, NULL,
		      NUMPATCHES+2, colormap, pixdata );
if ( im == NULL )
   abort("Couldn't create image")

pp = pixdata;
for ( y=0; y < IMHEIGHT; ++y )
    {
    int	 patchcolor;

    patchcolor = y/PATCHHEIGHT + 2;
    for ( x=0; x < PATCHWIDTH; ++x )
	*pp++ = patchcolor;
    for ( x=PATCHWIDTH; x < IMWIDTH; ++x )
	*pp++ = 1;
    }



/*--------------------------------*/
/*	Read the RGB database	  */
/*--------------------------------*/

if ( argc < 2 )
   rgbdb = fopen ( "/usr/lib/X11/rgb.txt", "r" );
else
   rgbdb = fopen ( argv[1], "r" );
if ( rgbdb == NULL )
   abort("Can't open color database file")

ic = getc(rgbdb);
c  = ic;

/*  status = fscanf (rgbdb, "%u %u %u %s", &r, &g, &b, curname);	*/
status = 0;
r = cvu (rgbdb);
g = cvu (rgbdb);
b = cvu (rgbdb);
cvs (rgbdb, curname);
if ( status != 4 )
   abort ("Color database file is empty")

cur_col = first_col = last_col =
	  (struct dbent *) malloc (sizeof(struct dbent));
if ( cur_col == NULL )
   abort("Insufficient memory")
cur_col -> next = NULL;
cur_col -> prev = NULL;
cur_col -> name = (char *) malloc (strlen(curname));
if ( cur_col -> name == NULL )
   abort("Insufficient memory")
strcpy ( cur_col -> name, curname );
cur_col -> color.red   = r;
cur_col -> color.green = g;
cur_col -> color.blue  = b;

/*  status = fscanf (rgbdb, "%u %u %u %s", &r, &g, &b, curname);	*/
status = 0;
r = cvu (rgbdb);
g = cvu (rgbdb);
b = cvu (rgbdb);
cvs (rgbdb, curname);
while ( status == 4 )
    {
    cur_col = (struct dbent *) malloc (sizeof(struct dbent));
    if ( cur_col == NULL )
       abort("Insufficient memory")

    last_col -> next = cur_col;
    cur_col -> next = NULL;
    cur_col -> prev = last_col;
    last_col = cur_col;

    cur_col -> name = (char *) malloc (strlen(curname));
    if ( cur_col -> name == NULL )
       abort("Insufficient memory")
    strcpy ( cur_col -> name, curname );
    cur_col -> color.red   = r;
    cur_col -> color.green = g;
    cur_col -> color.blue  = b;

    /*  status = fscanf (rgbdb, "%u %u %u %s", &r, &g, &b, curname);	*/
    status = 0;
    r = cvu (rgbdb);
    g = cvu (rgbdb);
    b = cvu (rgbdb);
    cvs (rgbdb, curname);
    }

fclose ( rgbdb );





/*------------------------------------------------------*/
/*	Start X interface, Display image in window	*/
/*------------------------------------------------------*/

#define	set_colors \
 { \
 XSetForeground (dsp, gc, ((Sir *) (im -> sysres)) -> pixels[1]); \
 XFillRectangle (dsp, win, gc, PATCHWIDTH, 0, TEXTWIDTH, IMHEIGHT); \
 XSetForeground (dsp, gc, ((Sir *) (im -> sysres)) -> pixels[0]); \
 cur_col = top_col; \
 for ( i=2; i < NUMPATCHES+2; ++i ) \
     { \
     if ( cur_col != NULL ) \
	{ \
	im -> colormap[i].red   = cur_col -> color.red; \
	im -> colormap[i].green = cur_col -> color.green; \
	im -> colormap[i].blue  = cur_col -> color.blue; \
	XDrawImageString (dsp, win, gc, PATCHWIDTH+10, (i-1)*PATCHHEIGHT-10, \
			cur_col -> name, strlen(cur_col -> name)); \
	sprintf ( curname, "#%02X%02X%02X  (%3u %3u %3u)", \
			cur_col -> color.red, \
			cur_col -> color.green, \
			cur_col -> color.blue, \
			cur_col -> color.red, \
			cur_col -> color.green, \
			cur_col -> color.blue); \
	XDrawImageString (dsp, win, gc, PATCHWIDTH+10+RGBOFF, \
			(i-1)*PATCHHEIGHT-10, curname, strlen(curname)); \
	cur_col = cur_col -> next; \
	} \
     else \
	{ \
	im -> colormap[i].red   = 0; \
	im -> colormap[i].green = 0; \
	im -> colormap[i].blue  = 0; \
	} \
     } \
 XFlush (dsp); \
 }


X11_env = start_X11 ( argc, argv );	/*  Start X11 interface		*/
if ( X11_env == NULL )
   abort("Couldn't start X interface")

display_img ( im, X11_env, ButtonPressMask | KeyPressMask );


sir = (Sir *) im -> sysres;			/*  Point to X resources  */
expose = (XExposeEvent *) &sir -> event;	/*  Point to event data	  */
event  = &sir->event;

dsp = X11_env -> display;
gc  = X11_env -> gc;
pix = ((Sir *) (im -> sysres)) -> pix;
win = ((Sir *) (im -> sysres)) -> win;
XSetBackground (dsp, gc, ((Sir *) (im -> sysres)) -> pixels[1]);
XSetForeground (dsp, gc, ((Sir *) (im -> sysres)) -> pixels[0]);
/* XSetFont       (dsp, gc, %%%% );	*/

XRebindKeysym ( dsp, XK_Up, noshifts, 0, "<", 1 );
XRebindKeysym ( dsp, XK_Down, noshifts, 0, ">", 1 );
XRebindKeysym ( dsp, XK_Prior, noshifts, 0, "{", 1 );
XRebindKeysym ( dsp, XK_Next, noshifts, 0, "}", 1 );

top_col = first_col;



XNextEvent ( X11_env->display, event );	/*  Wait for 1st exposure event	*/
while (event->type != Expose)
   XNextEvent ( X11_env->display, event );

set_colors
update_colors(im);
XCopyArea ( X11_env->display, sir->pix, sir->win, X11_env->gc,
			expose->x, expose->y, expose->width, expose->height,
			expose->x, expose->y );
set_colors
XFlush(dsp);


while (1)
   {
   char	 	 keybuf[20];	/* &*@$#*&@$ keyboard paraphernalia	*/
   int		 n;
   KeySym	 keysym;
   XComposeStatus compstat;




   XNextEvent ( X11_env->display, event );

   switch ( event -> type )
	{
	case NoExpose:		/*  A couple noise events...		*/
	case GraphicsExpose:
		break;

	case Expose:

		XCopyArea ( X11_env->display, sir->pix, sir->win, X11_env->gc,
			expose->x, expose->y, expose->width, expose->height,
			expose->x, expose->y );
		update_colors(im);
		set_colors
		break;

	case ButtonPress:

		update_colors(im);
		set_colors
		break;

	case KeyPress:

	        n = XLookupString ( event, keybuf, 20, &keysym, &compstat );
		if ( n <= 0 )
		   break;

		if ( *keybuf == 0x03 )
		   exit (0);
		else if ( *keybuf == '>' )
		   {
		   if ( top_col -> next != NULL )
		      top_col = top_col -> next;
		   set_colors
		   update_colors(im);
		   }
		else if ( *keybuf == '<' )
		   {
		   if ( top_col -> prev != NULL )
		      top_col = top_col -> prev;
		   set_colors
		   update_colors(im);
		   }
		else if ( *keybuf == '}' )
		   {
		   for ( n=0; n < 8; ++n )
		       if ( top_col -> next != NULL )
		          top_col = top_col -> next;
		   set_colors
		   update_colors(im);
		   }

		else if ( *keybuf == '{' )
		   {
		   for ( n=0; n < 8; ++n )
		       if ( top_col -> prev != NULL )
		          top_col = top_col -> prev;
		   set_colors
		   update_colors(im);
		   }
		break;

	default:
		fprintf ( stderr, "excolors:  Unexpected event type, %d\n",
				event->type );
		break;
	}
   }

}
