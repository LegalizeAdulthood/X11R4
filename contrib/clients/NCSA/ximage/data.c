/*
 * This NCSA software product is public domain software.  Permission
 * is granted to do whatever you like with it. Should you feel compelled 
 * to make a contribution for the use of this software, please send 
 * comments as to what you like or don't like about the software, 
 * how you use it, how it helps your work, etc. to softdev@ncsa.uiuc.edu.
 *
 * Please send bug reports to bugs@ncsa.uiuc.edu
 *
 * Author: Dave Thompson, National Center for Supercomputing Applications
 *         davet@ncsa.uiuc.edu
 */

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/List.h>

#include <math.h>

#include "xstuff.h"
/*#include "list.h"*/
#include "xstuff2.h"
#include "i.h"

#define ROUND(x)        ((int) (x + 0.5))
#define	PI	3.1415927

#define	FRACTSIZE	3
#define	WIDTH		5


#define	XLOC		300
#define	YLOC		300
#define	XDIM		350
#define	YDIM		350

extern  Widget  toplevelform;

static	Widget	topDataWidget;
static	Widget	dataBaseWidget;
static	Widget	dataListWidget;

static void MakeDataString();
void CBDoneData();
void CBSelectData();


int InitData(dpy)
Display *dpy;
{

	return(1);	/* return NULL if fatal */
}


static int MakeScalarString(datastring,length,width,ptr)
char	*datastring[];
int	length;
char	**ptr;	/* return value: storage area for strings */
{
int	x,y,z;
char	format[20];

	if(!(*ptr = (char *) XIMalloc(length*(width+1)))) { 
		printf("Can't allocate enough memory for data display\n");
		return(0);
		}
	if (width > 6)
		sprintf(format,"%%%dd   ",width-3);
	else
		sprintf(format,"%%%dd",width);
	for (z = 0 ; z < length; z++) {
		sprintf(*ptr,format,z);
		datastring[z] = *ptr;
		(*ptr) += (strlen(*ptr)+1);
		}

	datastring[length] = NULL;
	return(1);

} /* MakeScalarString() */


static void CBDataVScroll(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct IMAGE *i = (struct IMAGE *) client_data;

	gr_VPortVertMove(i->dws.box4,(int) call_data);
}


static void CBDataHScroll(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct IMAGE *i = (struct IMAGE *) client_data;

	gr_VPortHoriMove(i->dws.box3,(int) call_data);
}




static void DisplayData(i)
struct IMAGE *i;
{
int	x,y;
Widget	theVPort;
	if (i->data_showing)
		return;

	/* allocate space for data display strings */
	if (i->isFloat)
	    i->dws.pdata = (char *)XICalloc(i->xdim*i->ydim+1,
				i->fwidth+3);/*3=[+-].\0 */
	else
	    i->dws.pdata = (char *)XICalloc(i->xdim*i->ydim+1,i->fwidth+2);

	if (!i->dws.pdata) {
		PrintMesg("Can't allocate enough memory to display data\n");
		return;
		}
		

	/* allocate space for index of strings (for list widget) */
	i->dws.pdata2 = (char *) XICalloc(i->xdim*i->ydim + 1,sizeof(char *));
	if (!i->dws.pdata2) {
		PrintMesg("Can't allocate enough memory to display data\n");
		XIFree(i->dws.pdata);
		return;
		}

	if (i->isFloat)
		MakeDataString(i->dws.pdata2,i->dws.pdata,i->xdim,
				i->ydim,i->fdataset, TRUE,i->format);
	else
		MakeDataString(i->dws.pdata2,i->dws.pdata,i->xdim,
				i->ydim,i->ri,FALSE,i->format);
	/* Done creating string list of data for data window */

	/* Make Widgets */
	i->dws.toplevel = MakeATopLevel("X Image [Image.DataSet]",toplevelform,
				XLOC,YLOC,XDIM,YDIM);

	i->dws.composite = Make_Board(i->name,i->dws.toplevel,
				5,5,XDIM,YDIM);

	i->dws.box5 = NewMakeList("DataSet",i->dws.composite,&(i->dws.box2),
				VERTHORIZ,i->xdim,5,
				CBDataVScroll,CBDataHScroll,CBSelectData,i,
				i->dws.pdata2,40,60,XDIM-50,YDIM-65);


	/* Create X and Y string lists for data window ticks */
	i->dws.pdata3 = (char *) XICalloc(i->xdim + 2,sizeof(char *));
	if (!i->dws.pdata3) {
		PrintMesg("Can't allocate enough memory to display data\n");
		XIFree(i->dws.pdata);
		XIFree(i->dws.pdata2);
		return;
		}

	i->dws.pdata4 = (char *) XICalloc(i->ydim + 2,sizeof(char *));
	if (!i->dws.pdata4) {
		PrintMesg("Can't allocate enough memory to display data\n");
		XIFree(i->dws.pdata);
		XIFree(i->dws.pdata2);
		XIFree(i->dws.pdata3);
		return;
		}
	if(! MakeScalarString((char *) i->dws.pdata3,i->xdim+1,i->fwidth,
				&(i->dws.pdata5)))
		return;
	if(! MakeScalarString((char *) i->dws.pdata4,i->ydim+1,
				(int)log10((double)i->ydim)+1,
				&(i->dws.pdata6)))
		return;	
	i->dws.box6 = NewMakeListStatic("staticList",i->dws.composite,
			&(i->dws.box3), i->xdim,NULL,NULL,
			(char *) i->dws.pdata3,40,35,XDIM-75,20);
	i->dws.box7 = NewMakeListStatic("OogaBooga",i->dws.composite,
			&(i->dws.box4), 1,NULL,NULL,
			(char *) i->dws.pdata4,5,60,30,YDIM-90);


	i->dws.box1 = Make_ButtonBox(i->dws.composite,5,5,XDIM-60,25);
	(void) Make_Label(i->label,i->dws.box1,2,2,30);
	(void) Make_Label(i->name,i->dws.box1,33,2,XDIM-100);
	(void) MakeButton("Done",i->dws.composite,XDIM-50,6,CBDoneData,i,0,0);


	i->data_showing = TRUE;
	
	return;
} /* DisplayData() */


static void MakeDataString(datastring,mbuff,xdim,ydim,dataset,isFloat,format)
char	*datastring[];	
char	*mbuff;	
int	xdim,ydim;
float	*dataset;
char	*format;
char	isFloat;
{
int	x,y;
char	*ptr;
float	*fdataset;	/* float data set */
unsigned char	*cdataset;	/* raster image data set */


	if (isFloat) {
		fdataset = (float *) dataset;
		ptr = mbuff;
		for(y = 0 ; y < ydim ; y++) {
			for(x = 0 ; x < xdim; x++ ) {
				sprintf(ptr,format,fdataset[y*xdim+x]);
				datastring[y*xdim+x] = ptr;
				ptr += (strlen(ptr)+1);
				}
			}
		}
	else {
		cdataset = (unsigned char *) dataset;
		ptr = mbuff;
		for(y = 0 ; y < ydim ; y++) {
			for(x = 0 ; x < xdim; x++ ) {
				sprintf(ptr,format,cdataset[y*xdim+x]);
				datastring[y*xdim+x] = ptr;
				ptr += (strlen(ptr)+1);
				}
			}
		}


	datastring[xdim*ydim]=NULL;

	return;

} /* MakeDataString() */


void CBDisplayData(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{

	DisplayData((struct IMAGE *) client_data);
}



void CBDoneData(w,client_data,call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct IMAGE *i;
	i = (struct IMAGE *) client_data;

	XIFree(i->dws.pdata);
	XIFree(i->dws.pdata2);
	XIFree(i->dws.pdata3);
	XIFree(i->dws.pdata4);
	XIFree(i->dws.pdata5);
	XIFree(i->dws.pdata6);
	XtDestroyWidget(i->dws.toplevel);
	i->data_showing = FALSE;
}


void SelectPointFromImage(i,x,y)
/* A point has been selected image, this proc highlights point in data */
struct IMAGE *i;
int x,y;	/* with respect to data set */
{
	if (! i->data_showing)
		return;
	if ((x < 0) || (x > i->xdim) || (y < 0) || (y > i->ydim))
		return;

	gr_ListHighlight(i->dws.box5,y*i->xdim+x);
	gr_ListHighlight(i->dws.box6,x);
	gr_ListHighlight(i->dws.box7,y);

	/* turn off old pixel highlight box */

	/* scroll the data window and update scales */
	gr_VPortMoveChild(i->dws.box2,&x,&y,i->xdim,i->ydim);
	gr_VPortVertMove(i->dws.box4,y);
	gr_VPortHoriMove(i->dws.box3,x);

} /* SelectPointFromImage() */

static void CBSelectData(w,client_data,call_data)
/* A point has been selected in Data window */
/* set X&Y ticks, and image */
Widget w;
caddr_t client_data;
caddr_t call_data;
{
struct IMAGE *i;
XtListReturnStruct *lr;
int x,y;

	i = (struct IMAGE *) client_data;
	lr = (XtListReturnStruct *) call_data;

	y = lr->index / i->xdim;
	x = lr->index % i->xdim;
	gr_ListHighlight(i->dws.box6,x);
	gr_ListHighlight(i->dws.box7,y);
	if (i->iws.amIShowing)
		CartHighlightSelectedImage(i,x,y);
	if (i->pws.amIShowing) {
		double theta;

		theta=(((double)x)/((double)i->xdim))
			*((double)(2.0*PI)) + PI/2.0;

		PolarHighlightSelectedImage(i,
			ROUND(((double)y)*sin(theta)) + i->ydim,
			ROUND(((double)y)*cos(theta)) + i->ydim);
		}
} /* CBSelectData() */

