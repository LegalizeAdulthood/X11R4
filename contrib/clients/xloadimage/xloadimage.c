/* loadimage.c:
 *
 * generic image loader for X11
 *
 * jim frost 09.27.89
 *
 * Copyright 1989 Jim Frost.  See included file "copyright.h" for complete
 * copyright information.
 */

#include "copyright.h"
#include "xloadimage.h"

/* options array and definitions
 */

char *Options[] = {
  "onroot", /* global options */
  "border",
  "display",
  "geometry",
  "help",
  "identify",
  "list",
  "install",
  "path",
  "quiet",
  "supported",
  "verbose",
  "view",

  "at", /* image options */
  "background",
  "brighten",
  "center",
  "clip",
  "colors",
  "dither",
  "foreground",
  "name",
  "xzoom",
  "yzoom",
  "zoom",
  NULL
};

#define ONROOT    0
#define BORDER    1
#define DISPLAY   2
#define GEOMETRY  3
#define HELP      4
#define IDENTIFY  5
#define LIST      6
#define INSTALL   7
#define PATH      8
#define QUIET     9
#define SUPPORTED 10
#define VERBOSE   11
#define VIEW      12

#define AT         13
#define BACKGROUND 14
#define BRIGHT     15
#define CENTER     16
#define CLIP       17
#define COLORS     18
#define DITHER     19
#define FOREGROUND 20
#define NAME       21
#define XZOOM      22
#define YZOOM      23
#define ZOOM       24

/* the real thing
 */

main(argc, argv)
     int argc;
     char *argv[];
{ unsigned int  onroot= 1;
  char         *border;
  char         *dname;
  unsigned int  identify;
  unsigned int  install;
  unsigned int  verbose;
  Image        *dispimage;      /* image that will be sent to the display */
  Image        *newimage;       /* new image we're loading */
  Display      *disp;           /* display we're sending to */
  int           scrn;           /* screen we're sending to */
  XColor        xcolor;         /* color for border option */
  ImageOptions  images[MAXIMAGES]; /* list of image w/ options to load */
  unsigned int  imagecount;     /* number of images in ImageName array */
  unsigned int  a;
  unsigned int  winx, winy;     /* location of window */
  unsigned int  winwidth, winheight; /* geometry of window */

  if (argc < 2)
    usage(argv[0]);

  /* defaults and other initial settings.  some of these depend on what
   * our name was when invoked.
   */

  loadPathsAndExts();
  onroot= 0;
  verbose= 1;
  if (!strcmp(tail(argv[0]), "xview")) {
    onroot= 0;
    verbose= 1;
  }
  else if (!strcmp(tail(argv[0]), "xsetbg")) {
    onroot= 1;
    verbose= 0;
  }
  border= NULL;
  dname= NULL;
  identify= 0;
  install= 0;
  winx= winy= winwidth= winheight= 0;

  imagecount= 0;
  for (a= 1; a < argc; a++) {
    switch (optionNumber(argv[a], Options)) {
    case OPT_BADOPT:
      printf("%s: Bad option\n", argv[a]);
      usage(argv[0]);
      /* NOTREACHED */

    case OPT_NOTOPT:
      if (imagecount == MAXIMAGES)
	printf("%s: Too many images (ignoring)\n", argv[++a]);
      else
	images[imagecount++].name= argv[a];
      break;

    case OPT_SHORTOPT:
      printf("%s: Not enough characters to identify option\n", argv[a]);
      usage(argv[0]);
      /* NOTREACHED */

    /* process options global to everything
     */

    case ONROOT:
      onroot= 1;
      break;

    case BORDER:
      border= argv[++a];
      break;

    case DISPLAY:
      dname= argv[++a];
      break;

    case GEOMETRY:
      XParseGeometry(argv[++a], &winx, &winy, &winwidth, &winheight);
      break;

    case HELP:
      usage(argv[0]);
      exit(0);

    case IDENTIFY:
      identify= 1;
      break;

    case LIST:
      listImages();
      exit(0);

    case INSTALL:
      install= 1;
      break;

    case PATH:
      showPath();
      break;

    case QUIET:
      verbose= 0;
      break;

    case SUPPORTED:
      supportedImageTypes();
      break;

    case VERBOSE:
      verbose= 1;
      break;

    case VIEW:
      onroot= 0;

    /* process options local to an image
     */

    case AT:
      if (sscanf(argv[++a], "%d,%d",
		 &images[imagecount].atx, &images[imagecount].aty) != 2) {
	printf("Bad argument to -at\n");
	usage(argv[0]);
	/* NOTREACHED */
      }
      break;

    case BACKGROUND:
      images[imagecount].bg= argv[++a];
      break;

    case BRIGHT:
      images[imagecount].bright= atoi(argv[++a]);
      break;

    case CENTER:
      images[imagecount].center= 1;
      break;

    case CLIP:
      if (sscanf(argv[++a], "%d,%d,%d,%d",
		 &images[imagecount].clipx, &images[imagecount].clipy,
		 &images[imagecount].clipw, &images[imagecount].cliph) != 4) {
	printf("Bad argument to -clip\n");
	usage(argv[0]);
	/* NOTREACHED */
      }
      break;

    case COLORS:
      images[imagecount].colors= atoi(argv[++a]);
      if (images[imagecount].colors < 2) {
	printf("Argument to -colors is too low (ignored)\n");
	images[imagecount].colors= 0;
      }
      else if (images[imagecount].colors > 65536) {
	printf("Argument to -colors is too high (ignored)\n");
	images[imagecount].colors= 0;
      }
      break;

    case DITHER:
      images[imagecount].dither= 1;
      break;

    case FOREGROUND:
      images[imagecount].fg= argv[++a];
      break;

    case NAME:
      if (imagecount == MAXIMAGES)
	printf("%s: Too many images (ignoring)\n", argv[++a]);
      else
	images[imagecount++].name= argv[++a];
      break;

    case XZOOM:
      images[imagecount].xzoom= atoi(argv[++a]);
      break;

    case YZOOM:
      images[imagecount].yzoom= atoi(argv[++a]);
      break;

    case ZOOM:
      images[imagecount].xzoom= images[imagecount].yzoom= atoi(argv[++a]);
      break;

    default:

      /* this should not happen!
       */

      printf("%s: Internal error parsing arguments\n", argv[0]);
      exit(1);
    }
  }

  if (!imagecount) /* NO-OP from here on */
    exit(0);

  if (identify) {                    /* identify the named image(s) */
    for (a= 0; a < imagecount; a++)
      identifyImage(images[a].name);
    exit(0);
  }

  /* start talking to the display
   */

  if (! (disp= XOpenDisplay(dname))) {
    printf("%s: Cannot open display\n", XDisplayName(dname));
    exit(1);
  }
  scrn= DefaultScreen(disp);
  XSetIOErrorHandler(ioErrorHandler);

  dispimage= NULL;
  if (onroot && (winwidth || winheight || images[0].center ||
      images[a].atx || images[a].aty)) {
    if (!winwidth)
	winwidth= DisplayWidth(disp, scrn);
    if (!winheight)
      winheight= DisplayHeight(disp, scrn);
    if (DefaultDepth(disp, scrn) == 1)
      dispimage= newBitImage(winwidth, winheight);
    else {
      dispimage= newRGBImage(winwidth, winheight, DefaultDepth(disp, scrn));
      dispimage->rgb.used= 1;
    }
    *(dispimage->rgb.red)= 65535;   /* default border value is white */
    *(dispimage->rgb.green)= 65535;
    *(dispimage->rgb.blue)= 65535;
    if (border) {
      XParseColor(disp, DefaultColormap(disp, scrn), border, &xcolor);
      *dispimage->rgb.red= xcolor.red;
      *dispimage->rgb.green= xcolor.green;
      *dispimage->rgb.blue= xcolor.blue;
    }
    fill(dispimage, 0, 0, winwidth, winheight, 0, verbose);
  }

  /* load in each named image
   */

  for (a= 0; a < imagecount; a++) {
    if (! (newimage= loadImage(images[a].name, verbose)))
      continue;
    if ((dispimage && BITMAPP(dispimage)) || (DefaultDepth(disp, scrn) == 1))
      images[a].dither= 1;
    newimage= processImage(disp, scrn, newimage, &images[a], verbose);
    if (!images[a].clipw && !images[a].cliph) {
      images[a].clipw= newimage->width;
      images[a].cliph= newimage->height;
    }
    if (images[a].center) {
      images[a].atx= (dispimage->width - images[a].clipw) / 2;
      images[a].aty= (dispimage->height - images[a].cliph) / 2;
    }
    if (dispimage) {
      if (! dispimage->title)
	dispimage->title= dupString(newimage->title);
      merge(dispimage, newimage, images[a].atx, images[a].aty, verbose);
      freeImage(newimage);
    }
    else
      dispimage= newimage;
  }

  if (! dispimage) {
    printf("No images to display\n");
    exit(1);
  }

  if (onroot)
    imageOnRoot(disp, scrn, dispimage, install, verbose);
  else
    imageInWindow(disp, scrn, dispimage, winx, winy, winwidth, winheight,
		  install, verbose);
  XCloseDisplay(disp);
}
