/*
 * Copyright 1989 Waseda University.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Waseda University not be used
 * in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.
 * Waseda University makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * WASEDA DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL WASEDA
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:	Nobuhiko Nakajima
 *		Waseda University
 *
 *	61-505, Sci&Eng department, Waseda University
 *	3-1-9 shinjyuku-ku ookubo, Tokyo, Japan	169
 *	(03) 232-0551
 *
 *	Junet	hiko@ohara.waseda.ac.jp
 */

#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <math.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xlib.h>

#include "Image.h"
#include "xmoon.h"
#include "moon.bm"

unsigned char	MoonLast[MoonHeight*MoonWidth/8];
unsigned char	MoonShape[MoonHeight*MoonWidth/8];
unsigned char	MoonPhase[MoonHeight*MoonWidth/8];
unsigned char	MoonShade[MoonHeight*MoonWidth/8];
int        	redraw_moon;
Widget		toplevel, text;
Display		*dpy;
int		scrNum;
int		kickme;
Pixmap		MoonPixmap;
unsigned int	update;
long		times;

static Boolean	test_flag = FALSE;    /* Rapid warp through time */
static Boolean	small_flag = FALSE;   /* Rapid warp through time */

/*  Forward functions  */
extern double	jtime(), phase();
extern void	phasehunt(), ringgg(), RemakeMoon(), jyear(), jhms();

static XrmOptionDescRec options[] = {
    {"-test",	"*test",	XrmoptionNoArg,		"True" },
    {"-small",	"*small",	XrmoptionNoArg,		"True" },
    {"-update",	"*update",	XrmoptionSepArg,	(caddr_t) NULL },
};

static XtResource resources[] = {
  { "test",	"test",		XtRBoolean,	sizeof(Boolean),
			(Cardinal)&test_flag,	XtRString,	"False" },
  { "small",	"small",	XtRBoolean,	sizeof(Boolean),
			(Cardinal)&small_flag,	XtRString,	"False" },
  { "update",	"update",	XtRInt,		sizeof(int),
			(Cardinal)&update,	XtRString,	"5" },
};

extern ImageString small;
extern ImageStr smallstr[];
extern ImageString boad;
extern ImageStr boadstr[];
ImageString small = { 3, smallstr, NULL };
ImageStr smallstr[] = {
    { 4,  5,  "                              " },
    { 2,  6,  "                              " },
    { 1,  7,  "                              " }
};

ImageString boad = { 15, boadstr, NULL };
ImageStr boadstr[] = {
    { 15,  0,  "***** MoonTool *****" },
    { 0, 1,
    "                                                                      " },
    { 0,  2,
    "                                                                      " },
    { 0,  3,  
    "                                                                      " },
    { 0,  5,  
    "                                                                      " },
    { 0,  6,
    "                                                                      " },
    { 0,  7,
    "                                                                      " },
    { 0,  8,
    "                                                                      " },
    { 0,  10,
    "                                                                      " },
    { 0,  11,
    "                                                                      " },
    { 0,  13,
    "                                                                      " },
    { 0,  14,
    "                                                                      " },
    { 0,  15,
    "                                                                      " },
    { 0,  16,
    "                                                                      " },
    { 0,  17,
    "                                                                      " }
};

main( argc, argv )
int	argc;
char	**argv;
{
    Arg			arg[8];
    int			i, j, d, s;
    unsigned char	*p;
    unsigned char	mask;
    extern void		wakeup();

    toplevel = XtInitialize( argv[0], "Xmoon",
				options, XtNumber(options), &argc, argv);

    XtGetApplicationResources( toplevel, (caddr_t)NULL,
				resources, XtNumber(resources),
				NULL, 0 );
    dpy = XtDisplay(toplevel);
    scrNum = DefaultScreen( dpy );

    if( small_flag ) {
	XtSetArg ( arg[0], XtNarrign, ARRIGN_CHAR );
	XtSetArg ( arg[1], XtNcwidth, 15 );
	XtSetArg ( arg[3], XtNcheight, 8 );
	XtSetArg ( arg[4], XtNstrings, &small );
	XtSetArg ( arg[5], XtNimagecx, 2 );
	XtSetArg ( arg[6], XtNimagecy, 0 );
    }
    else {
	XtSetArg ( arg[0], XtNarrign, ARRIGN_CHAR );
	XtSetArg ( arg[1], XtNcwidth, 65 );
	XtSetArg ( arg[2], XtNcheight, 19 );
	XtSetArg ( arg[3], XtNstrings, &boad );
	XtSetArg ( arg[4], XtNimagecx, 50 );
	XtSetArg ( arg[5], XtNimagecy, 1 );
    }
    XtSetArg ( arg[6], XtNinternalHeight, 4 );
    MoonPixmap = XCreatePixmapFromBitmapData( dpy,
	 XtScreen(toplevel)->root, FullMoon, MoonWidth, MoonHeight,
	 BlackPixel(dpy, scrNum), WhitePixel(dpy, scrNum),
	 DefaultDepth(dpy, scrNum) );
    XtSetArg ( arg[7], XtNbitmap, MoonPixmap );
    text = XtCreateManagedWidget( "moon", imageWidgetClass, toplevel,
				   arg, 8 );

    for( i = 0; i < sizeof(MoonPhase); i ++ ) {
	MoonPhase[i] = FullMoon[i];
	for ( s = FullMoon[i], j = d = 0; j < 8; j++, s >>= 1 ) {
	    d <<= 1;
	    if (s & 1)	d |= 1;
	}
	MoonPhase[i] = d;
    }

    p = MoonShade; mask = 0xaa;
    for( i = 0; i < MoonHeight; i ++ ) {
	mask = ~mask;
	for( j = 0; j < MoonWidth/8; j ++ ) *p ++ = mask;
    }

    XtRealizeWidget( toplevel );

    kickme = test_flag ? 1 : update*1000;
    XtAppAddTimeOut( XtWidgetToApplicationContext( text ),
		     1, (XtInputCallbackProc *)wakeup, (caddr_t)text );

    (void) time(&times);
    redraw_moon = 1;
    ringgg();

    XtMainLoop();
}

void
wakeup( client_data, source, id )
caddr_t		client_data;
int		*source;
XtInputId	*id;
{
    Arg		arg;

    ringgg();

    if( redraw_moon ) {
	XFreePixmap( dpy, MoonPixmap );
	MoonPixmap = XCreatePixmapFromBitmapData( dpy,
	    XtScreen(toplevel)->root, MoonShape, MoonWidth, MoonHeight,
	    BlackPixel(dpy, scrNum), WhitePixel(dpy, scrNum),
	    DefaultDepth(dpy, scrNum) );

	XtSetArg ( arg, XtNbitmap, (XtArgVal)MoonPixmap );
	XtSetValues ( text, &arg, 1 );
    }

    ImageRedisplay( text, (XEvent *)NULL, (Region *)NULL );

    XtAppAddTimeOut( XtWidgetToApplicationContext( text ),
		     kickme, (XtInputCallbackProc *)wakeup, (caddr_t)text );
}

/*  DRAWMOON  --  Construct given phase of moon.  */
void
RemakeMoon( ph )
double	ph;
{
    int			i, j, lx, rx;
    double		cp, xscale;
    unsigned short	lb[4];
    unsigned short	*Ptr = (unsigned short *)MoonPhase;

    xscale = cos(2 * PI * ph);

    for (i = 0; i < 24; i++) {
	lb[0] = lb[1] = lb[2] = lb[3] = 0xFFFF;
	cp = 24.0 * cos(asin(i / 24.0));
	if (ph < 0.5) {
	    rx = 32 + cp;
	    lx = 32 + xscale * cp;
	} else {
	    lx = 33 - cp;
	    rx = 33 - xscale * cp;
	}
	if ((1.0 - xscale) * cp < 0.3) {
	    lx = 0; rx = -1;
	}
	for (j = lx; j <= rx; j++) {
	    lb[j >> 4] &= (0x8000 >> (j & 0xF)) ^ 0xFFFF;
	}
	for (j = 0; j < 4; j++)
	    *( Ptr + (32 + i)*(MoonWidth/16) + j )
		= *( Ptr + (31 - i)*(MoonWidth/16) + j ) = lb[j];
    }
}

static
RedrawMoon()
{
    register int	 	n, s, j, d;
    register unsigned char	*sp, *dp, *fp, *ap;

    bcopy( MoonPhase, MoonLast, sizeof(FullMoon));

    sp = MoonPhase;
    dp = MoonShape;
    fp = FullMoon;
    ap = MoonShade;

    for( n = 0; n < sizeof( FullMoon ); n ++ ) {
	for ( s = *sp ++, j = d = 0; j < 8; j++, s >>= 1 ) {
	    d <<= 1;
	    if (s & 1)	d |= 1;
	}
	*dp++ = (~d & *ap & *fp) | d;
	ap ++; fp ++;
    }
}

/*  RINGGG  --	Update status on interval timer ticks and redraw
		window if needed.  */
void
ringgg()
{
	int lunation;
	double jd, p, aom, cphase, cdist, cangdia, csund, csuang, lptime;
	double phasar[5];
	static double nptime = 0.0; /* Next new moon time */
	char amsg[12], tbuf[80];
	static double faketime = 0.0;
	static double last_phase = 0.0;
	int yy, mm, dd, hh, mmm, ss;
	struct tm *gm;
        static char *moname[] = {"Jan", "Feb", "Mar",
           "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	if( test_flag ) {
	    times += 60*60*2;
	}
	else	(void) time(&times);
	    
	gm = localtime(&times);

	jd = jtime((gm = gmtime(&times)));

	p = phase(jd, &cphase, &aom, &cdist, &cangdia, &csund, &csuang);
	if (fabs(last_phase-p)*10000.0 > 1.0) {
	    RemakeMoon( p );
	    if( bcmp(MoonPhase, MoonLast, sizeof(FullMoon)) ) {
		redraw_moon = 1;
		RedrawMoon();
	    }
	    last_phase = p;
	}
		
/* Update textual information for open window */
        sprintf(boadstr[1].str, "Julian date:     %.5f", jd + 0.5 );
	if (test_flag) {
	   jyear(jd, &yy, &mm, &dd);
	   jhms(jd, &hh, &mmm, &ss);
	   sprintf(boadstr[2].str, 
              "Universal time:  %02d:%02d:%02d %2d %s %d",
	      hh, mmm, ss, dd, moname[mm - 1], yy);
	} else {
	   sprintf(boadstr[2].str, 
              "Universal time:  %02d:%02d:%02d %2d %s %d",
	      gm->tm_hour, gm->tm_min, gm->tm_sec,
	      gm->tm_mday, moname[gm->tm_mon], gm->tm_year + 1900);
	}
	gm = localtime(&times);

	if( small_flag ) {
	    sprintf(smallstr[0].str, "%02d:%02d:%02d",
			    gm->tm_hour, gm->tm_min, gm->tm_sec );
	    sprintf(smallstr[1].str, "%2d %s %d",
			gm->tm_mday, moname[gm->tm_mon], gm->tm_year + 1900 );
	}
	else {
	    sprintf(boadstr[3].str,
		    "Local time:      %02d:%02d:%02d %2d %s %d",
	       gm->tm_hour, gm->tm_min, gm->tm_sec,
	       gm->tm_mday, moname[gm->tm_mon], gm->tm_year + 1900);
	    sprintf(boadstr[4].str,
		    "Moon phase:      %d%% (0%% = New; 100%% = Full)",
					       (int) (cphase * 100));
	}

	/* Information about the Moon */
	if( small_flag ) {
	    sprintf(smallstr[2].str, "%2d.%2d.%2d(%3d%%)",
	       (int) aom, (int) (24 * (aom - floor(aom))),
	       ((int) (1440 * (aom - floor(aom)))) % 60,
	       (int) (cphase * 100) );
	    return;
	}

#define EPL(x) (x), (x) == 1 ? "" : "s"
	sprintf(boadstr[5].str,
           "Age of moon:     %2d day%s, %2d hour%s, %2d minute%s.    ",
	   EPL((int) aom), EPL(((int) (24 * (aom - floor(aom))))),
	   EPL(((int) (1440 * (aom - floor(aom)))) % 60));
	sprintf( boadstr[6].str, 
           "Moon's distance: %ld kilometres, %.1f Earth radii.",
	   (long) cdist, cdist / earthrad);
	sprintf( boadstr[7].str,
           "Moon subtends:   %.4f degrees (%d'%4.1f\").  ", cangdia,
	   (int)(cangdia*60.0), (cangdia*3600.0 - ((int)(cangdia*60.0))*60.0));

	/* Edit information about the Sun */

	sprintf( boadstr[8].str,
           "Sun's distance:  %.0f kilometres, %.3f astronomical units.",
	    csund, csund / sunsmax);
	sprintf( boadstr[9].str,
           "Sun subtends:    %.4f degrees (%d'%4.1f\").  ", csuang,
	   (int)(csuang*60.0), (csuang*3600.0 - ((int)(csuang*60.0))*60.0));

	/* Calculate times of phases of this lunation.	This is sufficiently
	   time-consuming that we only do it once a month. */

	if (jd > nptime) {
	   phasehunt(jd, phasar);
	   lptime = phasar[0];
	   lunation = floor(((lptime + 7) - lunatbase) / synmonth) + 1;
	   jyear(lptime, &yy, &mm, &dd);
	   jhms(lptime, &hh, &mmm, &ss);
	   sprintf( boadstr[10].str,
              "Last new moon:   %02d:%02d UTC %2d %s %d    Lunation %d",
	      hh, mmm, dd, moname[mm - 1], yy , lunation);

	   lptime = phasar[1];
	   jyear(lptime, &yy, &mm, &dd);
	   jhms(lptime, &hh, &mmm, &ss);
	   sprintf( boadstr[11].str, 
              "First quarter:   %02d:%02d UTC %2d %s %d",
	      hh, mmm, dd, moname[mm - 1], yy);

	   lptime = phasar[2];
	   jyear(lptime, &yy, &mm, &dd);
	   jhms(lptime, &hh, &mmm, &ss);
	   sprintf(boadstr[12].str, 
              "Full moon:       %02d:%02d UTC %2d %s %d",
	      hh, mmm, dd, moname[mm - 1], yy);

	   lptime = phasar[3];
	   jyear(lptime, &yy, &mm, &dd);
	   jhms(lptime, &hh, &mmm, &ss);
	   sprintf( boadstr[13].str,
              "Last quarter:    %02d:%02d UTC %2d %s %d",
	      hh, mmm, dd, moname[mm - 1], yy);

	   nptime = phasar[4];
	   jyear(nptime, &yy, &mm, &dd);
	   jhms(nptime, &hh, &mmm, &ss);
	   sprintf( boadstr[14].str,
              "Next new moon:   %02d:%02d UTC %2d %s %d    Lunation %d",
	      hh, mmm, dd, moname[mm - 1], yy, lunation + 1);
	}
}
