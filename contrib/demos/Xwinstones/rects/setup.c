/* Copyright International Business Machines, 1989
 *
 *
 *
 *  IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 *  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR ANY
 *  PARTICULAR USE.  IN NO EVENT SHALL
 *  IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 *  ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 *  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 *  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 *  SOFTWARE
 *
 *
 *
*/
#include "stones.h"
#include "bitmap.h"
#include <string.h>

#define PROBLEM		1
#define WITH 		==
#define MAXRECTS		8190

extern	void	XDrawRectangles();
extern	void	XFillRectangles();
void	(*XDoRects)()= XDrawRectangles;

XRectangle xrect[MAXRECTS];
char *units = "Kpixels";
int fill = 0;
int num_rects = 0;
unsigned int area;

/****************************************************************************/

int
setup(argc, argv, ops)
int argc;
char **argv;
optionsPtr ops;
{
	XGCValues		xgcv[1];
	XColor installedcolor[1];
	XColor databasecolor[1];
	Pixmap pmap;
	unsigned int osbg;

	int x = 300;
	int y = 200;
	int dx = -1;
	int dy = -1;
	int wd = 1;
	int ht = 1;
	int dwd = 2;
	int dht = 2;
	int solid = 0;
	int tile = 0;
	int stip = 0;
	int opaqstip = 0;
	int i;

	num_rects = 190;

	for(i=1; i<argc; i++)
	{
		if (strcmp(argv[i], "-help")==0)  return(1);
		else if (strcmp(argv[i], "-h")==0) 
		{
			printf("\n");
			printf("\tRectstones uses XDrawRectangles or XFillRectangles \n");
			printf("\tRectstones' options are: \n");
			printf("\n");
		
			printf("\t-nrects   (int)        number of rectangles per call (default=%d) \n",num_rects);
			printf("\t-maxrects (int)        %d rectangles per call \n",MAXRECTS);
			printf("\t-x        (int)        left of first rectangle (default=%d ) \n",x);
			printf("\t-y        (int)        top of first rectangle (default=%d) \n",y);
			printf("\t-dx       (int)        space between rectangles (default=%d) \n",dx);
			printf("\t-dy       (int)        space between rectangles (default=%d) \n",dy);
			printf("\t-wd       (int)        width of rectangle (default=%d) \n",wd);
			printf("\t-ht       (int)        heigth of rectangle (default=%d) \n",ht);
			printf("\t-dwd      (int)        change of the width of rectangle (default=%d) \n",dwd);
			printf("\t-dht      (int)        change of the heigth of rectangle (default=%d) \n",dht);
			printf("\t-solid                 solid XFillRectangles \n");
			printf("\t-tile                  tile XFillRectangles \n");
			printf("\t-stip                  stipple XFillRectangles \n");
			printf("\t-opaqstip              opaque stipple XFillRectangles \n");
			printf("\t-opaqstipbg (string)   opaque stipple fill \n");
			printf("\t-help                  stones' help page \n");
			printf("\t-h                     this listing \n");
		
			printf("\n");
			return(1);
		}
		else if (strcmp(argv[i], "-nrects")==0) num_rects = atoi(argv[++i]);
		else if (strcmp(argv[i], "-maxrects")==0) num_rects = MAXRECTS;
		else if (strcmp(argv[i], "-x")==0) x = atoi(argv[++i]);
		else if (strcmp(argv[i], "-y")==0) y = atoi(argv[++i]);
		else if (strcmp(argv[i], "-dx")==0) dx = atoi(argv[++i]);
		else if (strcmp(argv[i], "-dy")==0) dy = atoi(argv[++i]);
		else if (strcmp(argv[i], "-ht")==0) ht = atoi(argv[++i]);
		else if (strcmp(argv[i], "-wd")==0) wd = atoi(argv[++i]);
		else if (strcmp(argv[i], "-dht")==0) dht = atoi(argv[++i]);
		else if (strcmp(argv[i], "-dwd")==0) dwd = atoi(argv[++i]);
		else if (strcmp(argv[i], "-solid")==0) solid = 1;
		else if (strcmp(argv[i], "-tile")==0) tile = 1;
		else if (strcmp(argv[i], "-stip")==0) stip = 1;
		else if (strcmp(argv[i], "-opaqstip")==0) opaqstip = 1;
		else if (strcmp(argv[i],"-opaqstipbg") ==0 )   
		{
				GETCOLOR(osbg, argv[++i], WhitePixel);
		}
	}

	XSync( ops->display, 0);

	if ((num_rects == 0) || (num_rects > MAXRECTS))
		num_rects = MAXRECTS;

	if (PROBLEM WITH Making_Rects(x,y,dx,dy,wd,ht,dwd,dht))
	{
		printf("Error in making rects. \n");
		return(1);
	}

	if (solid) 
	{
		fill = 1;
		XDoRects = XFillRectangles;
	}

	if (tile)
	{
		fill = 1;
		XDoRects = XFillRectangles;
		pmap = XCreatePixmapFromBitmapData(ops->display, ops->window, 
			bitmap_bits, bitmap_width, bitmap_height, ops->fggc, ops->bggc,
			DefaultDepth(ops->display, ops->dScreen));
		xgcv->fill_style = FillTiled;
		xgcv->tile = pmap;
		XChangeGC(ops->display, ops->fggc, GCTile | GCFillStyle, xgcv);
	}
	
	if (stip)
	{
		fill = 1;
		XDoRects = XFillRectangles;
		pmap = XCreateBitmapFromData(ops->display, ops->window, 
			bitmap_bits, bitmap_width, bitmap_height);
		xgcv->stipple = pmap;
		xgcv->fill_style = FillStippled;
		XChangeGC(ops->display, ops->fggc, GCStipple | GCFillStyle, xgcv);
	}
	
	if (opaqstip)
	{
		fill = 1;
		XDoRects = XFillRectangles;
		pmap = XCreateBitmapFromData(ops->display, ops->window, 
			bitmap_bits, bitmap_width, bitmap_height);
		xgcv->stipple = pmap;
		xgcv->fill_style = FillOpaqueStippled;
		xgcv->background = osbg;
		XChangeGC(ops->display, ops->fggc, 
				GCStipple | GCFillStyle | GCBackground , xgcv);
	}

	return(0);
}


/***********************************************************************/

int
Making_Rects(x,y,dx,dy,wd,ht,dwd,dht)
	int x;
	int y;
	int dx;
	int dy;
	int wd;
	int ht;
	int dwd;
 	int dht;

{
	extern unsigned int area;
	int i = 1;
	
	xrect[0].x = x;
	xrect[0].y = y;
	xrect[0].width = wd;
	xrect[0].height = ht;
	area += (unsigned int)(wd * ht);

	while (i < num_rects)
	{
		x += dx;
		y += dy;
		wd += dwd;
		ht += dht;

		xrect[i].x = x;
		xrect[i].y = y;
		xrect[i].width = wd;
		xrect[i].height = ht;
		area += (unsigned int)(wd * ht);
		i++;
	}
	return 0;
}

/**************************************************************/

