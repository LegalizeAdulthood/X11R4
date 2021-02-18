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

#define MAXSEGS 8190        /* sizeof(XSegment) * 8190 =< 65536 (64K) */

int numsegs = MAXSEGS;
XSegment Seg[MAXSEGS];
char *units = "lines";


int
setup(argc, argv, ops)
int argc;
char **argv;
optionsPtr ops;
{
	XWindowAttributes att[1];
	int winW, winH;
	char *dash_list = 0;
	extern char *malloc();
	XGCValues xgcv[1];
	int dash = 0;
	int i, j, cx, cy, x, y, minx, miny, maxx, maxy;
	int length = 200;
	int width = 0;


	for(i=1; i<argc; i++)
	{
		if (strcmp(argv[i], "-help")==0)  return(1);
		else if (strcmp(argv[i],"-h")==0)
		{
			printf("\n");
			printf("\tSweepstones uses XDrawSegments \n");
			printf("\tSweepstones' options are: \n");
			printf("\n");
		
			printf("\t-nsegs  (int)              number of segments per call (defualt=%d) \n",numsegs);
			printf("\t-maxsegs                   %d segments per call \n",MAXSEGS);
			printf("\t-length  (int)             initial length of line (default=%d) \n",length);
			printf("\t-wide    (int)             width of lines, 0 being machine dependent (default=%d) \n",width);
			printf("\t-dash (1) (2) (3) .. (N)   specified pattern line \n");
			printf("\t                            1 is the number of parameters \n");
			printf("\t                            2 is number of pels on \n");
			printf("\t                            3 is number of pels off \n");
			printf("\t-help                      stones' help page \n");
			printf("\t-h                         this listing \n");

			printf("\n");
			return(1);
		}
		else if (strcmp(argv[i],"-nsegs")==0)	numsegs = atoi(argv[++i]);
		else if (strcmp(argv[i],"-maxsegs")==0)	numsegs = MAXSEGS;
		else if	(strcmp(argv[i],"-wide")==0)	width = atoi(argv[++i]);
		else if (strcmp(argv[i],"-length")==0)	length = atoi(argv[++i]);
		else if (strcmp(argv[i],"-dash")==0)
		{
			dash  = atoi(argv[++i]);
			dash_list = malloc((unsigned int)dash);
			if (dash_list == 0)
			{
				printf("%s: malloc for dash\n",prog);
				return -1;
			}
			for(j=0; j<dash; j++)
				dash_list[j]= (char) atoi(argv[++i]);
			j=0;
		}
	}

	if (numsegs > MAXSEGS) numsegs = MAXSEGS;

	if (dash_list)
	{
		xgcv->line_style = LineOnOffDash;
		XSetDashes(ops->display, ops->fggc, 0, dash_list, dash);
		XChangeGC(ops->display, ops->fggc,  GCLineStyle, xgcv);
	}

	if (width)
	{
		xgcv->line_width = width;
		XChangeGC(ops->display, ops->fggc,  GCLineWidth, xgcv);
	}

	XSync(ops->display, 0);
	XGetWindowAttributes(ops->display, ops->window, att);

	winW = att->width;
	winH = att->height;

	cx = winW / 2;
	cy = winH / 2;
	minx = cx - length;
	maxx = cx + length;
	miny = cy - length;
	maxy = cy + length;

	while (1)
	{
		for (x = minx; x < maxx; x++)
		{
			Seg[i].x1 = x;
			Seg[i].y1 = miny;
			Seg[i].x2 = cx;
			Seg[i].y2 = cy;
			i++;
			if (i > numsegs) 
			{
				XSync(ops->display,0);
				return (0);
			}
		}
		for (y = miny; y < maxy; y++)
		{
			Seg[i].x1 = maxx;
			Seg[i].y1 = y;
			Seg[i].x2 = cx;
			Seg[i].y2 = cy;
			i++;
			if (i > numsegs) 
			{
				XSync(ops->display,0);
				return (0);
			}
		}
		for (x = maxx; x > minx; x--)
		{
			Seg[i].x1 = x;
			Seg[i].y1 = maxy;
			Seg[i].x2 = cx;
			Seg[i].y2 = cy;
			i++;
			if (i > numsegs) 
			{
				XSync(ops->display,0);
				return (0);
			}
		}
		for (y = maxy; y > miny; y--)
		{
			Seg[i].x1 = minx;
			Seg[i].y1 = y;
			Seg[i].x2 = cx;
			Seg[i].y2 = cy;
			i++;
			if (i > numsegs) 
			{
				XSync(ops->display,0);
				return (0);
			}
		}
	}

	/* while (i < numsegs) */
	/* { */
		/* Seg[i].x1 = Seg[j].x1; */
		/* Seg[i].y1 = Seg[j].y1; */
		/* Seg[i].x2 = Seg[j].x2; */
		/* Seg[i].y2 = Seg[j].y2; */
		/* i++; */
		/* j++; */
	/* } */
	/* 	 */
	/* XSync(ops->display,0); */
	/* return (0); */
}

/***********************************************/
