/* mftest.c
 *
 * Test multiple fonts API.
 */
/*****************/
/* include files */
/*****************/
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/Xutil.h>
#include <locale.h>
#include "mbX.h"

/************************/
/* constant definitions */
/************************/
#define MFWINDOWX 500
#define MFWINDOWY 100
#define LINES 20

/********************/
/* global variables */
/********************/
       Display	*display;
       Window	mfwindow;
       GC	mfgc;
       unsigned int display_width, display_height;
       unsigned long	myforeground, mybackground;
       unsigned int line_height ; 

static XFontStruct **fonts ; 

/*
 *	Initialize the UNIX International.
 *	return 1 : success
 *	       0 : fail
 */

main(argc, argv)
int argc;
char **argv;
{
	extern Display	*display;
	extern Window	window;
	extern GC	mygc;
	XSizeHints	myhint;
	int		screen;
	XEvent		ev;

	/* Run in the installed locale - sensitive to $LANG */
	setlocale(LC_ALL, "") ; 

	/******************/
	/* initialization */
	/******************/
	if (!(display = XOpenDisplay(""))) { /* sorry, hard coded */
		printf("XOpenDisplay error\n");
		return 0;
	}
 
	screen = DefaultScreen(display);

	/* default pixel values */
	mybackground = WhitePixel(display, screen);
	myforeground = BlackPixel(display, screen);

        /* screen size */
	display_width = DisplayWidth(display, screen);
	display_height = DisplayHeight(display, screen);

        /* load fonts for subsequent use */
	fonts = mbXLoadQueryFont(display, argv[1]);

	/*************************/
	/* window creation */
	/*************************/
	line_height = fonts[0]->max_bounds.ascent 
			+ fonts[0]->max_bounds.descent;
	myhint.x = MFWINDOWX;
	myhint.y = MFWINDOWY;
	myhint.width = fonts[0]->max_bounds.width * 80 ; 
	myhint.height = (line_height + 2 ) * (LINES + 1);
	myhint.flags = USPosition | USSize;

	/* window creation */
	mfwindow = XCreateSimpleWindow(display, DefaultRootWindow(display), 
		myhint.x, myhint.y, myhint.width, myhint.height, 
		5, myforeground, mybackground);

	/* GC creation and initialization */
	mfgc = XCreateGC(display, mfwindow, 0, 0);
	XSetBackground(display, mfgc, mybackground);
	XSetForeground(display, mfgc, myforeground);

	/* set properties */
	XSetStandardProperties(display, mfwindow, "mftest", "mftest",
			       None, None, None, &myhint);
	
	/* input event selection */
	XSelectInput(display, mfwindow, 
		       KeyPressMask | ButtonPressMask | ExposureMask);

	/* set border color and width */
	XSetWindowBorder(display, mfwindow, 0x54);
	XSetWindowBorderWidth(display, mfwindow, 0x2);

	/* window mapping */
	XMapRaised(display, mfwindow);

	while (1) { 
		/* this return zero on the event other than KeyPress */
		XNextEvent(display, &ev);
		switch(ev.type)  {
		    case Expose:
			if (ev.xexpose.window == mfwindow) {
				mfDrawIt(); 
			}
			break;
		    case KeyPress:
			XClearWindow( display, mfwindow );
			if ( !strcmp("Jp_JP.pc932",setlocale(LC_CTYPE,NULL)) )
				SJIS_FillUp();
			else
				FillUp(); 
	 		mfDrawIt();
			break;
		    default:
			mfClose();
			break;
		} /* end of switch event */
	}
}


/***************************************/
/* close down all resource we have had */
/***************************************/
mfClose()
{
	extern Display	*display;
	extern Window	window;
	extern GC	mygc;

	XFreeGC(display, mfgc);
	XDestroyWindow(display, mfwindow);
	XCloseDisplay(display);
	exit(1);
}

#define MAX_BUFFER_SIZE (LINES * 80 * 2)
	char mfbuffer[ MAX_BUFFER_SIZE ];
	char *mfline = mfbuffer; 

FillUp() 
{

	int n = MAX_BUFFER_SIZE;
	int lines = LINES;
	int len;

	mfline = mfbuffer;
	while ( n && lines-- && fgets(mfline, n, stdin) ) {
		len = strlen(mfline) ; 
		mfline += len + 1 ; n -= len + 1 ; 
	} 
}

mfDrawIt()
{
	int len, lines;
	char *line = mfbuffer;

 	if ( mfline == mfbuffer ) return ;

	for ( lines = 0 ; line < mfline && lines < LINES ; ++lines ) {
		len = strlen(line);
		if ( len == 1 ) { line += len + 1 ; continue;} 
		mbXDrawImageString( display, mfwindow, mfgc, 0,
			lines * (line_height + 2) + fonts[0]->max_bounds.ascent,
			line, len-1, fonts);
		line += len + 1 ;
	}
}

Fill_CS0()
{
	int i;
	static int cs0 = 1;

	for ( i = 0 ; i < 4 ; ++i ) { 
		*mfline++ = cs0++ ;
		if ( cs0 == 0x80 ) cs0 = 1;
	} 
	*mfline++ = ' ';
}

Fill_CS1() /* Shifted JIS Codes */
{
	int i;
	char str[3];
	static cs1_byte1 = 0x81;
	static cs1_byte2 = 0x40;

	str[2] = 0 ;
	for ( i = 0 ; i < 4 ; ++i ) { 

		if ( cs1_byte2 == 0x7f ) ++cs1_byte2 ;

		*mfline++ = cs1_byte1; *mfline++ = cs1_byte2++ ;

		if ( cs1_byte2 == 0xfd ) {
			cs1_byte2 = 0x40;
			if ( ++cs1_byte1 == 0xf0 ) {
				cs1_byte1 = 0x81;
				continue;
			}

			for ( str[0] = cs1_byte1, str[1] = cs1_byte2 ; 
					CSid(str,2,NULL) != 1 ;  ) {
				str[0] = ++cs1_byte1 ;
			}
		}
	} 
	*mfline++ = ' ';
}

Fill_CS2() /* Katakana Data GR */
{
	int i;
	char str[2];
	static cs2 = 0x80;

	str[1] = 0 ;
	for ( i = 0 ; i < 4 ; ++i ) { 
		*mfline++ = cs2++ ;
		if ( cs2 == 0x100 ) {
			cs2 = 0x80;
			continue;
		}
		for ( str[0] = cs2 ; (str[0] = CSid(str,1,NULL)) != 2 ;  ) {
			str[0] = ++cs2 ;
		}
	} 
	*mfline++ = ' ';
}

Fill_CS3() /* IBM Selected Codes 0xf040 to 0xfcfc */
{
	int i;
	char str[3];
	static cs3_byte1 = 0xf0;
	static cs3_byte2 = 0x40;

	str[2] = 0 ;
	for ( i = 0 ; i < 4 ; ++i ) { 

		if ( cs3_byte2 == 0x7f ) ++cs3_byte2 ;

		*mfline++ = cs3_byte1; *mfline++ = cs3_byte2++ ;

		if ( cs3_byte2 == 0xfd ) {
			cs3_byte2 = 0x40;
			if ( ++cs3_byte1 == 0xfd ) {
				cs3_byte1 = 0xf0;
				continue;
			}

			for ( str[0] = cs3_byte1, str[1] = cs3_byte2 ; 
					CSid(str,2,NULL) != 3 ;  ) {
				str[0] = ++cs3_byte1 ;
			}
		}
	} 
	*mfline++ = ' ';
}

SJIS_FillUp() 
{

	int i ;

	mfline = mfbuffer;

	for ( i = 0 ; i < LINES ; ++i ) {
		Fill_CS0(); 
		Fill_CS1(); 
		Fill_CS2(); 
		Fill_CS3(); 
		Fill_CS2(); 
		Fill_CS1(); 
		Fill_CS0(); 
		*mfline++ = '\n' ; 
		*mfline++ = '\0' ;
	}

}

