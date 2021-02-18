/*  Copyright International Business Machines, 1989
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

int winW;
int winH;
int srcx;
int srcy;
unsigned int wd;
unsigned int ht;
char *units = "Kpixels";
int pixtowindow = 0;
Pixmap pixmap;

/****************************************************************************/

int
setup(argc, argv, ops)
int argc;
char **argv;
optionsPtr ops;
{
	XGCValues		xgcv[1];
	XWindowAttributes xwatt;
	XColor installedcolor[1];
	XColor databasecolor[1];
	Pixmap pmap;
	unsigned int osbg;
	int tile = 0;
	int stip = 0;
	int opaqstip = 0;
	int i;

	srcx = 0;
	srcy = 0;
	wd = 40;
	ht = 40;

	for(i=1; i<argc; i++)
	{
		if (strcmp(argv[i], "-help")==0) return(1);
		else if (strcmp(argv[i], "-h")==0) 
		{
			printf("\n");
			printf("\tBlitstones uses XCopyArea. \n");
			printf("\tBlitstones' options are: \n");
			printf("\n");
		
			printf("\t-wd       (int)         width of rectangle (default=%d) \n",wd);
			printf("\t-ht       (int)         heigth of rectangle (default=%d) \n",ht);
			printf("\t-pix                    pixmap to window blit \n");
			printf("\t-tile                   tile fill \n");
			printf("\t-stip                   stipple fill \n");
			printf("\t-opaqstip               opaque stipple fill \n");
			printf("\t-opaqstipbg (string)    background color for opaque stipple fill \n");
			printf("\t-help                   stones' help page \n");
			printf("\t-h                      this listing \n");
		
			printf("\n");
		
			return(1);
		}
		else if (strcmp(argv[i], "-ht")==0) ht = atoi(argv[++i]);
		else if (strcmp(argv[i], "-wd")==0) wd = atoi(argv[++i]);
		else if (strcmp(argv[i], "-pix")==0) pixtowindow = 1;
		else if (strcmp(argv[i], "-tile")==0) tile = 1;
		else if (strcmp(argv[i], "-stip")==0) stip = 1;
		else if (strcmp(argv[i], "-opaqstip")==0) opaqstip = 1;
		else if (strcmp(argv[i], "-opaqstipbg") ==0 )   
		{
			GETCOLOR(osbg, argv[++i], WhitePixel);
		}
	}

	if (pixtowindow)
	{
		pixmap = XCreatePixmap(ops->display, ops->window, 
			wd, ht, DefaultDepth(ops->display, ops->dScreen));
	}

	if (tile)
	{
		pmap = XCreatePixmapFromBitmapData(ops->display, ops->window, 
			bitmap_bits, bitmap_width, bitmap_height, ops->fggc, ops->bggc,
			DefaultDepth(ops->display, ops->dScreen));
		xgcv->fill_style = FillTiled;
		xgcv->tile = pmap;
		XChangeGC(ops->display, ops->fggc, GCTile | GCFillStyle, xgcv);
	}
	
	if (stip)
	{
		pmap = XCreateBitmapFromData(ops->display, ops->window, 
			bitmap_bits, bitmap_width, bitmap_height);
		xgcv->stipple = pmap;
		xgcv->fill_style = FillStippled;
		XChangeGC(ops->display, ops->fggc, GCStipple | GCFillStyle, xgcv);
	}
	
	if (opaqstip)
	{
		pmap = XCreateBitmapFromData(ops->display, ops->window, 
			bitmap_bits, bitmap_width, bitmap_height);
		xgcv->stipple = pmap;
		xgcv->fill_style = FillOpaqueStippled;
		xgcv->background = osbg;
		XChangeGC(ops->display, ops->fggc,
				GCStipple | GCFillStyle | GCBackground , xgcv);
	}

	XSync( ops->display, 0);
	XGetWindowAttributes(ops->display,ops->window,&xwatt);
	winW = xwatt.width;
	winH = xwatt.height;

	if (pixtowindow)
	{
		srcx = 0;
		srcy = 0;
		/* printf("pix x=%d y =%d wd=%d ht=%d \n",srcx,srcy,wd,ht); */
		XFillRectangle(ops->display,pixmap,ops->fggc,srcx,srcy,wd,ht);
		XSetGraphicsExposures(ops->display,ops->fggc,FALSE);
		XSync( ops->display, 0);
	}
	else
	{
		XFillRectangle(ops->display,ops->window,ops->fggc,srcx,srcy,wd,ht);
		XSetGraphicsExposures(ops->display,ops->fggc,FALSE);
		XSync( ops->display, 0);
	}

	return(0);
}


/***********************************************************************/

