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
===============================================================================
=======                                                                 =======
=======		Program/Module Exhibit  --  Exhibit Images		=======
=======			     File exhibit.c				=======
=======                                                                 =======
===============================================================================


  Functions:

	Exhibit a collection of images, one at a time.

	Normally this covers the screen with a black background
	window for the images.  It will optionally lock the
	workstation, requiring the user to type either his own
	password or the root user's password to unlock it.


  Public Routines:

	main		Main program


  Local Routines:

	None


  Module History:

	11-28-89	Paul Raveling
	Updated copyright/legal disclaimer notice to suit
	X Consortium requirements

	11-27-89	Paul Raveling
	Finalized version 1.1 for contribution to X11R4.

	Summer '89	Paul Raveling
	Created module as a hack.
*/



/************************************************************************/
/*	Minimum delay times, to restrict loading on file servers	*/
/*	and networks due to multiple users displaying lots of images	*/
/************************************************************************/

#ifndef	MIN_DELAY_LOCKED
#define	MIN_DELAY_LOCKED   30	/*  Min delay with system locked	*/
#endif

#ifndef	MIN_DELAY_UNLOCKED
#define	MIN_DELAY_UNLOCKED 10	/*  Min delay with system unlocked	*/
#endif

#ifndef	DEFAULT_DELAY
#define	DEFAULT_DELAY	   45	/*  Default delay period		*/
#endif




#include <stdio.h>
#include <signal.h>

#include <imglib/imglib.h>	/*  Include sys-independent defs	*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>		/*  Include X11 definitions		*/
#include <X11/Xos.h>

#include <imglib/imgXlib.h>	/*  Include sys-dependent defs for X11	*/


#ifndef	hpux
#define	volatile	""
#endif

extern		rnd_init ();	/*  Initialize random number generator	*/
extern	long	rnd_i ();	/*  Get random integer  (xrand)		*/
extern	unsigned long rnd_u ();	/*  Get random integer  (xrand)		*/



	Window	ourparent;	/*  Either root or background window	*/

static	int	 caughtsig;	/*  Bleepin' bypass for UNIX stupidity	*/


static	dumdum (what)
	int	what;
{
caughtsig = what;
}

static	X11env	*X11_env;

static	fold	(what)
	int	 what;
{
stop_X11 (X11_env);
exit(0);
}

static	int	 imgnum = 0;
static	int	 consecfail = 0;



main(argc,argv)

	int	 argc;
	char	*argv[];

{
	int	 selection;		/*  Image selection mode:	*/
#define	   RANDOM	0		/*	Random order		*/
#define	   SEQUENTIAL	1		/*	Sequential order	*/

	int	 delay;			/*  Extra time delay, seconds	*/
	int	 forcemono;		/*  Force-monochrome flag	*/
	int	 quiet;			/*  Quiet flag			*/
	int	 showusage;		/*  Show usage flag		*/

	int	 back;			/*  Background type:		*/
#define	   NOBACK	0		/*	No background		*/
#define	   UNLOCKED	1		/*	Black, unlocked		*/
#define	   LOCKED	2		/*	Black, locked		*/

	int	 colors;		/*  Color settings:		*/
#define	   ORIGINAL	0		/*	Original colors		*/
#define	   ARTIFICIAL	1		/*	Artificial colors	*/

	Img	*newimg;		/*  New image			*/
	Img	*oldimg;		/*  Oldimage			*/
	int	 status;
/*	X11env	*X11_env;	*/
volatile XEvent	*event;
	XColor	 black;
	Colormap default_cmap;		/*  Default color map		*/
 XSetWindowAttributes	 winat;		/*  Crud for changing color map	*/

	int	 scrwidth;		/*  Width of screen		*/
	int	 scrheight;		/*  Height of screen		*/
	int	 olddisproc;		/*  Display process pid, old img*/
	int	 newdisproc;		/*  Display process pid, new img*/
	int	 conproc;		/*  Control process pid		*/

	int	 corner;
	int	 gxoff;
	int	 gyoff;

	Byte   **name_list;		/*  Pointer to image name list	*/
	int	 num_names;		/*  Number of names in list	*/
	int	 list_cap;		/*  List capacity		*/
	Byte	 cur_name[1000];	/*  Current name		*/
	Byte	*bp;			/*  Pointer to a byte		*/
	FILE	*list_file;		/*  Name list file		*/
	int	 ic;			/*  Input char from file	*/
	Byte	 b;			/*  Input byte from file	*/

	int	 i;			/*  Whatnot			*/




/*:::::::::::::::::::::::::::::::::::::::::*/
/*	Macro to abort with a message	   */
/*:::::::::::::::::::::::::::::::::::::::::*/

#define	abort(msg) {fprintf(stderr,"exhibit:  %s\n",msg); exit(2);}



/*-----------------------------------------*/
/*	Parse command line arguments	   */
/*-----------------------------------------*/

if ( argc < 2 )				/*  Be sure filename exists	*/
   abort("No image list filename on command line")

/*	Set default options	*/

delay     = DEFAULT_DELAY;		/*  Delay = DEFAULT_DELAY sec.	*/
selection = RANDOM;			/*  Random selection		*/
forcemono = 0;				/*  Don't force monochrome	*/
quiet     = 0;				/*  Verbose, not quiet		*/
back	  = UNLOCKED;			/*  Bg = black, unlocked	*/
colors    = ORIGINAL;			/*  Original colors		*/

if ( *argv[1] == '?' )
   showusage = 1;			/*  Show usage if asked		*/
else
   showusage = 0;

for ( i=2; i < argc; ++i )		/*  Check remaining arguments	*/
    {
    if ( (*argv[i] >= '0') && (*argv[i] <= '9') )
	delay = atoi(argv[i]);

   else if ( (*argv[i] == 'r') || (*argv[i] == 'R') )
      selection = RANDOM;
   else if ( (*argv[i] == 's') || (*argv[i] == 'S') )
      selection = SEQUENTIAL;
   else if ( (*argv[i] == 'm') || (*argv[i] == 'M') )
      forcemono = 1;
   else if ( (*argv[i] == 'c') || (*argv[i] == 'C') )
      forcemono = 0;
   else if ( (*argv[i] == 'q') || (*argv[i] == 'Q') )
      quiet = 1;
   else if ( (*argv[i] == 'v') || (*argv[i] == 'V') )
      quiet = 0;
   else if ( (*argv[i] == 'n') || (*argv[i] == 'N') )
      back = NOBACK;
   else if ( (*argv[i] == 'u') || (*argv[i] == 'U') )
      back = UNLOCKED;
   else if ( (*argv[i] == 'l') || (*argv[i] == 'L') )
      back = LOCKED;
   else if ( (*argv[i] == 'o') || (*argv[i] == 'O') )
      colors = ORIGINAL;
   else if ( (*argv[i] == 'a') || (*argv[i] == 'A') )
      colors = ARTIFICIAL;
   else if ( *argv[i] == '?' )
      showusage = 1;
   else
      {
      fprintf ( stderr,
	"Can't recognize argument \"%s\"\n", argv[i] );
      showusage = 1;
      }
    }

if ( showusage )
   {
   printf ("\n");
   printf ("Usage:\texhibit  file  [option1  [option2 [...]]]\n");
   printf ("\n");
   printf ("    \"file\" lists the path for each image on a separate line\n");
   printf ("\n");
   printf ("    Options, recognized by 1st letter (either upper or lower case), are:\n");
   printf ("\n");
   printf ("\tMinimum delay between images:\tDecimal number\t(seconds)\n");
   printf ("\tBackground (screen cover):\tN[one], U[nlocked], or L[ocked]\n");
   printf ("\tColor or monochrome:\t\tC[olor] or M[onochrome]\n");
   printf ("\tColors to use:\t\t\tO[riginal] or A[rtificial]\n");
   printf ("\tImage selection sequence:\tR[andom] or S[equential]\n");
   printf ("\tReporting:\t\t\tV[erbose] or Q[uiet]\n");
   printf ("\n");
   printf (
    "    Defaults:\t%d seconds, unlocked, color, original, random, verbose\n",
		DEFAULT_DELAY);
   printf ("\n");
   printf ("    ? (\"\\\?\" to most shells) in any operand => Just report usage\n");
   printf ("\n");
   exit (1);
   }


if ( back == LOCKED )
   if ( delay < MIN_DELAY_LOCKED )
      {
      fprintf ( stderr,
	"\nDelay of %d seconds is shorter than allowed; will use %d seconds\n",
	delay, MIN_DELAY_LOCKED );
      delay = MIN_DELAY_LOCKED;
      }
else
   if ( delay < MIN_DELAY_UNLOCKED )
      {
      fprintf ( stderr,
	"\nDelay of %d seconds is shorter than allowed; will use %d seconds\n",
	delay, MIN_DELAY_UNLOCKED );
      delay = MIN_DELAY_UNLOCKED;
      }

if ( quiet == 0 )
   {
   printf ( "\n    Options in effect:\n\n" );
   printf ( "\tDelay between images = %d seconds + processing time\n", delay );
   if ( back == NOBACK )
      printf ( "\tNone:\t\tNo background (screen cover)\n" );
   else if ( back == UNLOCKED )
      printf ( "\tUnlocked:\tScreen's covered but unlocked\n" );
   else
      printf ( "\tLocked:\t\tScreen's covered and locked\n" );
   if ( forcemono == 0 )
      {
      printf ( "\tColor:\t\tKeeping color (& grayscale) in color\n" );
      if ( colors == ORIGINAL )
	 printf ( "\tOriginal:\tUsing image's original colors\n" );
      else
         printf ( "\tArtificial:\tUsing semi-random artificial colors\n" );
      }
   else
      printf ( "\tMonochrome:\tDisplaying all images in monochrome\n" );
   if ( selection == RANDOM )
      printf ( "\tRandom:\t\tSelecting images randomly\n" );
   else
      printf ( "\tSequential:\tSelecting images sequentially\n" );
   printf ( "\n" );
   fflush (stdout);
   sleep (5);
   }



/*------------------------------------------------------*/
/*	Read list of image names (or filenames)		*/
/*------------------------------------------------------*/

num_names = 0;				/*  Start with 0 names	*/
list_cap  = 0;
name_list = NULL;

list_file = (FILE *) open_yafile ( argv[1], "r", NULL );
if ( list_file == NULL )
   abort("Couldn't open image-list file")




/*	Macro to get next byte:		*/

#define	gnb	{ic=getc(list_file); b=ic;}

gnb					/*  Preread 1st byte	*/

while ( ic != EOF )			/*  For entire file...	*/
      {
      if ( num_names == list_cap )	/*  If name list has no room	*/
	 {				/*	Enlarge it		*/
	 Byte  **new_list;

	 list_cap += 1000;
	 new_list = (Byte **) calloc ( list_cap, sizeof(Byte *) );
	 if ( new_list == NULL )
	    abort("Insufficient memory")

	 for ( i=0; i < num_names; ++i )
	     new_list[i] = name_list[i];
	 if ( name_list != NULL )
	    free (name_list);
	 name_list = new_list;
	 }

      /*	Skip leading white space on line	*/

      while ( (b == ' ') || (b == '\t') )
	    gnb


      if ( b == '#' )			/*  If line starts with '#'	*/
	 {				/*     skip it			*/
	 while ( (ic != EOF) && (b != '\n') )
	       gnb
         gnb				/*  Get 1st byte of next line	*/
	 continue;
	 }

      /*	Read 1 imagename/filename from line	  */

      bp = cur_name;			/*  Collect name in cur_name	*/
      i = 0;				/*  Size starts @ 0 bytes	*/

      while ( (ic != EOF) && (b != '\n') && (b != ' ') && (b != '\t') )
	    {
	    *bp++ = b;			/*  Buffer a byte	*/
	    ++ i;			/*  Count it		*/
	    gnb				/*  Get next byte	*/
	    }

      *bp = 0;				/*  Terminate name with a 0	*/
      while ( (ic != EOF) && (b != '\n') )	/*  Skip to end of line	*/
	    gnb
      gnb				/*  Preread 1st byte of next	*/

      if ( i == 0 )			/*  ... In case of EOF	*/ 
	 continue;

      bp = (Byte *) malloc ( i+1 );	/*  Allocate space for name	*/
      if ( bp == NULL )
	 abort("Insufficient memory")

      name_list[num_names++] = bp;	/*  Set pointer to name		*/
      strcpy (bp, cur_name);		/*  Set content of name		*/
      }

close_yafile ( list_file );		/*  Close the name list file	*/



/*--------------------------------------*/
/*	Read and display images		*/
/*--------------------------------------*/

rnd_init ( time(NULL) | 3 );	/*  Use time to seed random # generator	*/

X11_env = start_X11 ( argc, argv );	/*  Start X11 interface		*/
if ( X11_env == NULL )
   abort("Couldn't start X interface")

default_cmap = X11_env -> cmap;
X11_env -> cmap = XCreateColormap
	( X11_env->display, X11_env->rootwin, X11_env->visual, AllocNone );
black.red   = 0;
black.green = 0;
black.blue  = 0;
black.flags = DoRed | DoGreen | DoBlue;
XAllocColor ( X11_env->display, X11_env->cmap, &black );


scrwidth  = X11_env -> width;
scrheight = X11_env -> height;
ourparent = X11_env -> rootwin;
stop_X11 (X11_env);

caughtsig = 0;
signal (SIGHUP, dumdum);

conproc = getpid ();		/*  Save control process' pid		*/
if ( back != NOBACK )		/*  For background (screen cover)	*/
   maint_back (back, conproc);	/*	create & maintain the bg	*/
				/*	(May reset ourparent)		*/

#define exitseq	\
 { \
 XInstallColormap ( X11_env->display, default_cmap ); \
 if ( newdisproc != 0 ) \
    kill ( newdisproc, SIGKILL ); \
 if ( olddisproc != 0 ) \
    kill ( olddisproc, SIGKILL ); \
 exit (0); \
 }



i = -1;			/*  If using sequential display, do img 0 1st	*/
olddisproc = 0;
newdisproc = 0;
corner  = 0;

while (1)
	{
	if ( caughtsig == SIGHUP )
	   exitseq

	/*	Choose next image	*/

	if ( selection == SEQUENTIAL )
	   {
	   if ( ++i > num_names )
	      i = 0;
	   }
	else  /*  selection == RANDOM  */
	   i = rnd_i () % num_names;

	if ( caughtsig == SIGHUP )
	   exitseq

	caughtsig = 0;
	signal (SIGUSR1, dumdum);
	signal (SIGUSR2, dumdum);
	newdisproc = fork ();
	if ( newdisproc != 0 )
	   {			/*  Parent (control) process	*/
	   while (   (caughtsig != SIGUSR1)
		  && (caughtsig != SIGUSR2)
		  && (caughtsig != SIGHUP) )
	   	 pause();
	   if ( caughtsig == SIGHUP )
	      exitseq
	   if ( caughtsig == SIGUSR2 )
	      {
	      kill (newdisproc, SIGHUP);
	      wait (NULL);
	      continue;
	      }
	   caughtsig = 0;

	   if ( olddisproc != 0 )
	      {
	      kill ( olddisproc, SIGHUP );
	      olddisproc = 0;
	      wait (NULL);
	      }

	   olddisproc = newdisproc;
	   newdisproc = 0;
	   sleep (delay);
	   }
	else
	   {			/*  Child (display) process		*/
	   newimg = read_img ( name_list[i], any_image );
	   if ( newimg == NULL )
	      {
	      if ( ++consecfail >= 4 )
		 {
		 fprintf (stderr,
			"Something's wrong:  4 consecutive failures\n");
		 fprintf (stderr, "Aborting after %d images\n", imgnum);
system ("ps -ef");	/* &&&& */
		 kill (conproc, SIGHUP);
		 }
	      kill (conproc, SIGUSR2);
	      exit(1);
	      }
	   consecfail = 0;
	   ++ imgnum;

	   if ( forcemono && (newimg->num_colors > 2) )
	      {
	      status = color_to_mono (newimg);
	      if ( status == 0 )
		 {
		 fprintf(stderr, "Couldn't convert image to monochrome\n");
		 delete_img (newimg);
		 kill (conproc, SIGUSR2);
		 exit(1);
		 }
	      }

	   gxoff = rnd_i () % scrwidth;
	   if ( scrwidth < (gxoff + newimg->width) )
	      {
	      if ( corner < 2 )
		 gxoff = 5;
	      else
		 gxoff = -5;
	      }

	   gyoff = rnd_i () % scrheight;
	   if ( scrheight < (gyoff + newimg->height) )
	      {
	      if ( corner & 1 )
		 gyoff = 5;
	      else
		 gyoff = -5;
	      }

	   corner = ++corner & 3;



	   /*	Start X11 interface	*/

	   X11_env = start_X11 ( argc, argv );
	   if ( X11_env == NULL )
	      abort("Couldn't start X interface")

	   signal ( SIGHUP, fold );

	   X11_env -> cmap = XCreateColormap ( X11_env->display,
			 X11_env->rootwin, X11_env->visual, AllocNone );
	   XAllocColor ( X11_env->display, X11_env->cmap, &black );

	   if ( quiet == 0 )
	      printf ( "%s\n", name_list[i] );
	   fflush (stdout);


	   /*  	Next block is an inline variant of:

	   display_img ( newimg, X11_env, ButtonPressMask | KeyPressMask );

		but signals parent process when new image is ready
		for display, and uses geometry planned earlier.
	   */

{
volatile Geom	 wingeom;		/*  Window's geometry		*/
volatile Geom	 pixgeom;		/*  Pixmap's geometry		*/
	Sir	*sir;			/*  System interface resources	*/

	Window	 win;			/*  Window for image		*/
	Pixmap	 pix;			/*  Pixmap for image		*/
	int	 status;		/*  Returned status		*/

volatile XSizeHints	 bull;		/*  X malarky			*/



/*------------------------------------------------*/
/*	Initialize resources and geometries	  */
/*------------------------------------------------*/

if ( newimg->alloc & sys_resources )	/*  If image has sys resources	*/
   delete_sysres (newimg);		/*     free old ones		*/

status = create_sysres (newimg, X11_env);/*  Create sys resources	*/
if ( status == 0 )
   {
   fprintf ( stderr, "exhibit:  Couldn't allocate X resources\n" );
   exit(0);
   }
sir = (Sir *) newimg -> sysres;		/*  Keep pointer to sys resources  */

wingeom.xoff   = gxoff;
wingeom.yoff   = gyoff;			/*  Set window geometry		*/
wingeom.width  = newimg->width;
wingeom.height = newimg->height;

pixgeom.xoff   = 0;
pixgeom.yoff   = 0;			/*  Set pixmap geometry		*/
pixgeom.width  = newimg->width;
pixgeom.height = newimg->height;


/*--------------------------------------*/
/*	Create resources for image	*/
/*--------------------------------------*/

sir -> win = XCreateSimpleWindow ( X11_env->display, X11_env->rootwin,
	wingeom.xoff, wingeom.yoff, wingeom.width, wingeom.height, 2,
	BlackPixel(X11_env->display,X11_env->screen),
	BlackPixel(X11_env->display,X11_env->screen) );


sir -> pix = XCreatePixmap ( X11_env->display, sir->win,
	 newimg->width, newimg->height, X11_env->depth );

put_img ( newimg, sir->pix, &pixgeom, X11_env );/*  Put image to pixmap	*/



/*	Override the bleepin' window manager by setting hints	*/

bull.flags = USPosition | USSize;	/*  Tell it the user wants	*/
bull.x      = wingeom.xoff;		/*  this geometry		*/
bull.y      = wingeom.yoff;
bull.width  = wingeom.width;
bull.height = wingeom.height;
XSetNormalHints ( X11_env->display, sir->win, &bull );


kill (conproc, SIGUSR1);		/*  Tell control process to	*/
sleep (1);				/*  delete old window & process	*/

	/*  1 second delay improves chance for display server	*/
	/*  to release memory for old image's pixmap		*/
	/*  before it needs more for new image			*/


/*	If using artificial colors, generate random map	    */

if ( colors == ARTIFICIAL )
   {
   for ( i=0; i < sir->num_colors; ++i )
       {
	volatile XColor	 coldef;

       coldef.pixel = sir->pixels[i];
       coldef.flags = DoRed | DoGreen | DoBlue;
       coldef.red   = (rnd_u () & 0xff) << 8;
       coldef.green = (rnd_u () & 0xff) << 8;
       coldef.blue  = (rnd_u () & 0xff) << 8;

       XStoreColor ( X11_env->display, X11_env->cmap, &coldef );
       }
   }

winat.colormap = X11_env->cmap;
XChangeWindowAttributes ( X11_env->display, sir->win, CWColormap, &winat );
if ( back != NOBACK )
   XChangeWindowAttributes ( X11_env->display, ourparent, CWColormap, &winat );
XInstallColormap ( X11_env->display, X11_env->cmap );
XMapWindow ( X11_env->display, sir->win );	/*  Map the window	*/

XSelectInput ( X11_env->display, sir->win,
	ButtonPressMask | KeyPressMask | ExposureMask | ColormapChangeMask );
}

	   while (1)
		{
		volatile char	  keybuf[20];	/* &*@$#*&@$ keyboard paraphernalia */
		volatile KeySym		 keysym;
		volatile XComposeStatus	 compstat;
		int	 n;


		event = maint_img ( newimg, X11_env );	/*  Maintain image  */

		switch ( event->type )
		    {
		    case ButtonPress:

			/*  For now, exit on any mouse button	*/

			exit(0);

		    case ColormapNotify:

			/*	This event has no apparent value	*/
			break;


		    case KeyPress:

			/*  Exit on control-C, ignore anything else	*/

			n = XLookupString ( event, keybuf, 20,
						 &keysym, &compstat );
			if ( (n > 0) && (*keybuf == 0x03) )
			   exit (0);
			break;


		    default:		/*  Ignore anything else	*/
			break;
		    }
		}
	   }
	}
}
