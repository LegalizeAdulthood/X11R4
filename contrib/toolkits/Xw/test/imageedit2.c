#include <stdio.h>
#include <signal.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xw/Xw.h>
#include <Xw/ImageEdit.h>


void main (argc, argv)
unsigned int argc;
char **argv;

{
    Display * display;
    Widget toplevel, imageEdit;
    Arg args[10];
    XImage * image;
    register int i;
   extern void myQuit();

   signal (SIGINT, myQuit);


    toplevel = XtInitialize (argv[0], "XImageEdit2", NULL, 0, &argc, argv);
    display = XtDisplay (toplevel);


    /*  Fill out the image structure for the new tile and create the pixmap */

    image = XCreateImage (display, 
                          DefaultVisual (display, DefaultScreen (display)),
                          1, XYPixmap, 0, NULL, 32, 10, BitmapPad(display),
                          4);

    image -> data = (char*) XtMalloc (4000);
    image -> byte_order = ImageByteOrder(display);
    image -> bitmap_bit_order = BitmapBitOrder(display);
    image -> bitmap_unit = BitmapUnit(display);
    image -> bits_per_pixel = 1;
    image -> obdata = NULL;

    for (i=0; i<4000; i++)  
       image->data[i]=0xff;

    i = 0;
    XtSetArg (args[i], XtNimage, image); i++;
    XtSetArg (args[i], XtNdrawColor, 0); i++;
    XtSetArg (args[i], XtNeraseColor, 1); i++;
    imageEdit = XtCreateWidget ("image", XwimageEditWidgetClass, 
                                toplevel, (ArgList) args, i);

    XtManageChild (imageEdit);

    XtRealizeWidget (toplevel);

    XtMainLoop ();
}


void myQuit()
{
     printf("Begin exiting ... please standby ... \n");
     fflush(stdout);
     exit(0);
}

