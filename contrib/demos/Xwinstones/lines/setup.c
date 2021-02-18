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

/* #define MAXPTS 16300 */
#define MAXPTS 8190

int numpts;
XPoint Epts[MAXPTS]; 
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
XGCValues xgcv[1];
int dash = 0; 
int i =  0; 
int j =  0; 
int x;
int y;
int minx;
int maxx;
int miny;
int maxy;
int cx;
int cy;
int length = 200;
int width = 0;
int default_segs = 1;
FILE *fp;

numpts = 3200;

for(i=1; i<argc; i++)
{
	if (strcmp(argv[i], "-help")==0)  return(1);
	else if (strcmp(argv[i],"-h")==0)	
	{
		printf("\n");
		printf("\tlinestones uses XDrawLines \n");
		printf("\tlinestones' options are: \n");
		printf("\n");
	
		printf("\t-npts   (int)              number of points per call (default=%d) \n",numpts);
		printf("\t-maxpts                    of points per call (default=%d) \n",MAXPTS);
		printf("\t-wide   (int)              width of lines, 0 being machine dependent (default=%d) \n",width);
		printf("\t-length (int)              length of each segment (default=%d) \n",length);
		printf("\t-dash   (1) (2) (3) .. (N) specified pattern line \n");
		printf("\t                            1 is the number of parameters \n");
		printf("\t                            2 is number of pels on \n");
		printf("\t                            3 is number of pels off \n");
		printf("\t-file   (string)           name of file(polygon) to use \n");
		printf("\t-help                      stones' help page \n");
		printf("\t-h                         this listing \n");
	
		printf("\n");
		return(1);
	}
	else if (strcmp(argv[i],"-length")==0)	length = atoi(argv[++i]);
	else if (strcmp(argv[i],"-wide")==0)	width = atoi(argv[++i]);
	else if (strcmp(argv[i],"-npts")==0)	numpts = atoi(argv[++i]);
	else if (strcmp(argv[i],"-maxpts")==0)  numpts = MAXPTS;
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
	else if (strcmp(argv[i], "-file")==0) 
	{
		int tempx;
		int tempy;
		int indx = 0;

		default_segs = 0;
		if ((fp = fopen(argv[++i], "r")) == NULL)
		{
				fprintf(stderr,"Error in opening %s .",argv[i]);
				fprintf(stderr,"Using default segs instead. \n");
				default_segs = 1;
		}
		else
		{
	 		while (fscanf(fp,"%d %d\n",&tempx,&tempy) != EOF)  
			{
				Epts[indx].x = tempx;
				Epts[indx].y = tempy;
				indx++;
				if (indx > numpts)         /* break; */
				fprintf(stderr,"Error, array indx overflow! Please abort. \n");
			}
			numpts = indx;
			fclose(fp);
		}
	}
}

if (dash_list)
{
	XSetDashes(ops->display, ops->fggc, 0, dash_list, dash);
	xgcv->line_style = LineOnOffDash;
	XChangeGC(ops->display, ops->fggc,  GCLineStyle , xgcv);
}

if (width)
{
	xgcv->line_width = width;
	XChangeGC(ops->display, ops->fggc,  GCLineWidth, xgcv);
}

	
XSync( ops->display, 0);

if (default_segs)
{
	XGetWindowAttributes(ops->display, ops->window, att);
	winW = att->width;
	winH = att->height;
	
	cx = winW / 2;
	cy = winH / 2;
	i = 0;

	while (1)
	{
		minx = cx - length;
		maxx = cx + length;
		miny = cy - length;
		maxy = cy + length;

		for (x = minx; x < maxx; x++)
		{
			Epts[i].x = cx;
			Epts[i].y = cy;
			i++;
			if (i > numpts) return (0);
			Epts[i].x = x;
			Epts[i].y = miny;
			i++;
			if (i > numpts) return (0);
		}
		for (y = miny; y < maxy; y++)
		{
			Epts[i].x = cx;
			Epts[i].y = cy;
			i++;
			if (i > numpts) return (0);
			Epts[i].x = maxx;
			Epts[i].y = y;
			i++;
			if (i > numpts) return (0);
		}
		for (x = maxx; x > minx; x--)
		{
			Epts[i].x = cx;
			Epts[i].y = cy;
			i++;
			if (i > numpts) return (0);
			Epts[i].x = x;
			Epts[i].y = maxy;
			i++;
			if (i > numpts) return (0);
		}
		for (y = maxy; y > miny; y--)
		{
			Epts[i].x = cx;
			Epts[i].y = cy;
			i++;
			if (i > numpts) return (0);
			Epts[i].x = minx;
			Epts[i].y = y;
			i++;
			if (i > numpts) return (0);
		}
	}

}
	
XSync(ops->display,0);
return 0;
}

/***********************************************/

