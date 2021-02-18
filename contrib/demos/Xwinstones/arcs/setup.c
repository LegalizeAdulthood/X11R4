/*   Copyright International Business Machines, 1989
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

#define ELLIPSE 	100
#define PROBLEM		1
#define WITH 		==

extern void XDrawArcs();
extern void XFillArcs();

void (*XDoArcs)() = XDrawArcs;

#define MAXARCS		5460	/* 5460 * sizeof(XARC) ?= 65536 (=64K) */
XArc xarc[MAXARCS];
int num_arcs = 0;
char *units = "arcs";

/****************************************************************************/

int
setup(argc, argv, ops)
int argc;
char **argv;
optionsPtr ops;
{
	XGCValues		xgcv[1];
	Pixmap pmap;
	XColor installedcolor[1];
	XColor databasecolor[1];
	unsigned int osbg;
	int x = 2;
	int y = 2;
	int dx = 0;
	int dy = 0;
	int kind = ELLIPSE;
	int rad1 = 390;
	int rad2 = 390;
	int drad1 = 0;
	int drad2 = 0;
	int ang1 = 0;
	int ang2 = 360;
	int dang1 = 0;
	int dang2 = 0;
	int tile = 0;
	int stip = 0;
	int opaqstip = 0;
	int i;
	int j;
	int width = 0;
	extern char *malloc();
	int dash = 0;
	char *dash_list = 0;

	num_arcs = 190;

	for(i=1; i<argc; i++)
	{
		if (strcmp(argv[i], "-help")==0) return(1);
		else if (strcmp(argv[i], "-h")==0) 
		{
			printf("\n");
			printf("\tArcstones uses XDrawArcs or XFillArcs. \n");
			printf("\tArcstones' options are: \n");
			printf("\n");
		
			printf("\t-narcs (int)          number of arcs per call (default=%d) \n",num_arcs); 
			printf("\t-maxarcs              %d arcs per call \n",MAXARCS); 
			printf("\t-x     (int)          left extent of ellipse (default=%d) \n",x); 
			printf("\t-y     (int)          top extent of ellipse (default=%d) \n",y); 
			printf("\t-dx    (int)          change of left extent of ellipse (default=%d) \n",dx); 
			printf("\t-dy    (int)          change of top extent of ellipse (default=%d) \n",dy); 
			printf("\t-ellipse              uses XDrawArcs() (default)\n"); 
			printf("\t-chord                uses XFillArcs() in ArcChord mode \n"); 
			printf("\t-pieslice             uses XFillArcs() in ArcPieSlice mode \n");
			printf("\t-rad1  (int)          diameter of x-axis (default=%d) \n",rad1);
			printf("\t-rad2  (int)          diameter of y-axis (default=%d) \n",rad2);
			printf("\t-drad1 (int)          change of diameter of x-axis (default=%d) \n",drad1);
			printf("\t-drad2 (int)          change of diameter of y-axis (default=%d) \n",drad2);
			printf("\t-ang1  (int)          starting angle (default=%d) \n",ang1); 
			printf("\t-ang2  (int)          ending angle (default=%d) \n",ang2); 
			printf("\t-dang1 (int)          change of starting angle (default=%d) \n",dang1);
			printf("\t-dang2 (int)          change of ending angle (default=%d) \n",dang2);
			printf("\t-wide  (int)          width of arc (default=%d) \n",width);
			printf("\t-dash (1) (2) (3) .. (N)   specified pattern line \n");
			printf("\t                            1 is the number of parameters \n");
			printf("\t                            2 is number of pels on \n");
			printf("\t                            3 is number of pels off \n");
			printf("\t-tile                 tile fill for chords and pieslices \n");
			printf("\t-stip                 stipple fill for chords and pieslices \n");
			printf("\t-opaqstip             opaque stipple fill for chords and pieslices \n");
			printf("\t-opaqstipbg (string)  background color \n");
			printf("\t-help                 stones' help page \n");
			printf("\t-h                    this listing \n");
		
			printf("\t\n");
			return(1);
		}	
		else if (strcmp(argv[i], "-narcs")==0) num_arcs = atoi(argv[++i]); 
		else if (strcmp(argv[i], "-maxarcs")==0) num_arcs = MAXARCS; 
		else if (strcmp(argv[i], "-x")==0) x = atoi(argv[++i]); 
		else if (strcmp(argv[i], "-y")==0) y = atoi(argv[++i]); 
		else if (strcmp(argv[i], "-dx")==0) dx = atoi(argv[++i]); 
		else if (strcmp(argv[i], "-dy")==0) dy = atoi(argv[++i]); 
		else if (strcmp(argv[i], "-ellipse")==0) kind = ELLIPSE; 
		else if (strcmp(argv[i], "-chord")==0) kind = ArcChord;
		else if (strcmp(argv[i], "-pieslice")==0) kind = ArcPieSlice;
		else if (strcmp(argv[i], "-rad1")==0)  rad1 = atoi(argv[++i]);
		else if (strcmp(argv[i], "-rad2")==0)  rad2 = atoi(argv[++i]);
		else if (strcmp(argv[i], "-drad1")==0)  drad1 = atoi(argv[++i]);
		else if (strcmp(argv[i], "-drad2")==0)  drad2 = atoi(argv[++i]);
		else if (strcmp(argv[i], "-ang1")==0)  ang1 = atoi(argv[++i]);
		else if (strcmp(argv[i], "-ang2")==0)  ang2 = atoi(argv[++i]);
		else if (strcmp(argv[i], "-dang1")==0)  dang1 = atoi(argv[++i]);
		else if (strcmp(argv[i], "-dang2")==0)  dang2 = atoi(argv[++i]);
		else if	(strcmp(argv[i], "-wide")==0)	width = atoi(argv[++i]);
		else if (strcmp(argv[i], "-tile")==0) tile = 1;
		else if (strcmp(argv[i], "-stip")==0) stip = 1;
		else if (strcmp(argv[i], "-opaqstip")==0) opaqstip = 1;
		else if (strcmp(argv[i],"-opaqstipbg") ==0 )   
		{
			GETCOLOR(osbg, argv[++i], WhitePixel);
		}
		else if (strcmp(argv[i],"-dash")==0)	
		{
			dash  = atoi(argv[++i]);
			dash_list = malloc((unsigned)dash);
			if (dash_list == 0)
			{
				printf("%s: malloc for dash\n",prog);
				return -1;
			}
			for(j=0; j<dash; j++)
				dash_list[j]= (char) atoi(argv[++i]);
		}
	}

	XSync( ops->display, 0);

	if ((num_arcs == 0) || (num_arcs > MAXARCS))
		num_arcs = MAXARCS;

	if (PROBLEM WITH Making_Arcs(rad1,rad2,drad1,drad2,					ang1,ang2,dang1,dang2,x,y,dx,dy))
	{
		printf("Error in making arcs. \n");
		return(1);
	}

	if (ELLIPSE != kind)
	{
		XSetArcMode(ops->display, ops->fggc, kind );
		XDoArcs = XFillArcs;
	}

	if (width)
	{
		xgcv->line_width = width;
		XChangeGC(ops->display, ops->fggc,  GCLineWidth, xgcv);
	}

	if (dash_list)
	{
		xgcv->line_style = LineOnOffDash;
		XSetDashes(ops->display, ops->fggc, 0, dash_list, dash);
		XChangeGC(ops->display, ops->fggc,  GCLineStyle, xgcv);
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

	return(0);
}


/***********************************************************************/

int
Making_Arcs(rad1,rad2,drad1,drad2,ang1,ang2,dang1,dang2,x,y,dx,dy)
	int rad1;
	int rad2;
	int drad1;
	int drad2;
	int ang1;
	int ang2;
	int dang1;
	int dang2;
	int x;
	int y;
	int dx;
	int dy;
{
	int i = 1;

	xarc[0].x = (short)x;
	xarc[0].y = (short)y;
	xarc[0].width = (unsigned short)rad1;
	xarc[0].height = (unsigned short)rad2;
	xarc[0].angle1 = (short)(64 * ang1);
	xarc[0].angle2 = (short)(64 * ang2);

	while (i < num_arcs)
	{
		/* xarc[i].x = (short)x; */
		/* xarc[i].y = (short)y; */
		xarc[i].x = (short)dx + xarc[i-1].x;
		xarc[i].y = (short)dy + xarc[i-1].y;
		xarc[i].width = (unsigned short)(drad1) + xarc[i-1].width;
		xarc[i].height = (unsigned short)(drad2) + xarc[i-1].height;
		xarc[i].angle1 = (short)(64 * dang1) + xarc[i-1].angle1;
		xarc[i].angle2 = (short)(64 * dang2) + xarc[i-1].angle2;
		i++;
	}
	return (0);
}

/******************************************************************/

