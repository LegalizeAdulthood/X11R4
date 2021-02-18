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
#include <math.h>

#define MAXSEGS 8190  /* 1890 * sizeof(XSegments) <= 65536 (64K) */

char *units = "lines";
int numsegs = MAXSEGS;
XSegment xsegs[MAXSEGS];
int single = 0;


/*******************************************************/

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
	int dash;
	int width;
	int hspace = 10;
	int vspace = 5;
	int count = 0;
	int xhead, xtail;
	int yhead, ytail;
	int xh, yh;
	int xt, yt;
	int dx, dy;
	int i, imin, imax, iplus;
	int j, jmin, jmax, jplus;
	double pels = 40.0;
	double degrees = 45.0;
	double radians;
	
	width = 0;
	dash = 0;
	
	for(i=1; i<argc; i++)
	{
		if (strcmp(argv[i], "-help")==0)  return(1);
		else if (strcmp(argv[i],"-h")==0)	
		{
			printf("\n");
			printf("\tVectstones uses XDrawline or XDrawSegments \n");
			printf("\tVectstones' options are: \n");
			printf("\n");
		
			printf("\t-nvects    (int)     number of vectors for window (default=%d) \n",numsegs);
			printf("\t-maxvects  (int)     %d vectors if they fit in window \n",MAXSEGS);
			printf("\t-single              uses XDrawline instead of XDrawSegments(default) \n");
			printf("\t-width     (int)     width of vector (default=%d) \n",width);
			printf("\t-length    (int)     length of vector in pels (default=%4.0f) \n",pels);
			printf("\t-degrees   (int)     angle of vector (default=%4.0f) \n",degrees);
			printf("\t-vspace    (int)     vertical space between vectors (default=%d) \n",vspace);
			printf("\t-hspace    (int)     horizonal space between vectors (default=%d) \n",hspace);
			printf("\t-dash      (1) (2) (3) ... (N) specified pattern line \n");
			printf("\t                       1 is the number of parameters \n");
			printf("\t                       2 is number of pels on \n");
			printf("\t                       3 is number of pels off \n");
			printf("\t-help                stones' help page \n");
			printf("\t-h                   this listing \n");
		
			printf("\n");
			return(1);
		}
		else if	(strcmp(argv[i],"-width")==0) width = atoi(argv[++i]);
		else if (strcmp(argv[i],"-length")==0)	pels = atof(argv[++i]);
		else if (strcmp(argv[i],"-degrees")==0)	degrees = atof(argv[++i]);
		else if (strcmp(argv[i],"-nlines")==0) numsegs = atoi(argv[++i]);
		else if (strcmp(argv[i],"-single")==0) single = 1;
		else if (strcmp(argv[i],"-vspace")==0) vspace = atoi(argv[++i]);
		else if (strcmp(argv[i],"-hspace")==0) hspace = atoi(argv[++i]);
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
		
	XSync( ops->display, 0);
	XGetWindowAttributes(ops->display, ops->window, att);
	
	winW = att->width;
	winH = att->height;

	/* CREATE THE INITIAL VECTOR WITH TAIL AT THE ORIGIN
	OF THE CARTESIAN PLANE. */
	radians = M_2PI * degrees / 360.0;
	dx = (int)(pels * cos(radians));
	dy = (int)(pels * sin(radians));

	xtail = 0;
	ytail = 0;
	xhead = dx;
	yhead = dy;

	/* REMEMBER THAT:     PELS(LENGTH(VECTOR)) != SQRT(DX^2 + DY^2).
	CERTAIN TRANSLATIONS AND REFLECTIONS ARE NEEDED TO 
	TRANSFORM THE COORDINATES TO XWINDOW COORDINATES.
	ALSO INITIALIZES LOOP PARAMETERS. */
	if ((0.0 <= degrees) && (degrees < 90.0)) 
	{
		if (dx > dy) dx = (int)pels;
		else dy = (int)pels;

		yhead = yhead - dy; /* translate vector down */
		ytail = ytail - dy;

		yhead = -yhead;     /* reflect vector over x-axis */
		ytail = -ytail;
          
        imin = 0;
	    iplus = dy + vspace;
		if (dy != 0) imax = iplus * (winH / iplus); 
		else imax = winH; 

		jmin = 0;
        jmax = winW - dx;
		jplus = hspace;
	}
	else if ((90.0 <= degrees) && (degrees < 180.0)) 
	{
		if (-dx > dy) dx = -((int)pels);
		else dy = (int)pels;

		xhead = xhead - dx;      /* translate vector to rigth */
		xtail = xtail - dx;
		
		yhead = yhead - dy;      /* translate vector down */
		ytail = ytail - dy;
	
        yhead = -yhead;    /* reflect vector over x_axis */
		ytail = -ytail;
       
		imin = 0;
	    iplus = dy + vspace;
		if (dy != 0) imax = iplus * (winH / iplus); 
        else imax = winH; 

        jmin = 0;
        jmax = winW + dx; 
		jplus = hspace;
	}
	else if ((180.0 <= degrees) && (degrees < 270.0)) 
	{
		if (-dx > -dy) dx = -(int)pels;
		else dy = -((int)pels);

		xhead = xhead - dx;      /* translate vector to right */
		xtail = xtail - dx;

		yhead = -yhead;    /* reflect vector over x-axis */
		ytail = -ytail;

        imin = 0;
	    iplus = vspace - dy;
		if (dy != 0) imax = iplus * (winH / iplus); 
		else imax = winH; 

        jmin = 0;
        jmax = winW + dx; 
		jplus = hspace;
	}
	else if ((270.0 <= degrees) && (degrees <= 360.0))
	{
		if (dx > -dy) dx = (int)pels;
		else dy = -((int)pels);

		yhead = -yhead;    /* reflect vector over x-axis */
		ytail = -ytail;

        imin = 0;
	    iplus = vspace - dy;
		if (dy != 0) imax = iplus * (winH / iplus); 
		else imax = winH; 

        jmin = 0;
        jmax = winW - dx;
		jplus = hspace;
	}
	else 
	{
		printf(" ERROR, ANGLE MORE THAN %f > 360.0 DEGREES \n",degrees);
		exit(-1);
	}

    for (i = imin; i < imax; i += iplus) 
	{
		yh = yhead + i;
		yt = ytail + i;

		for (j = jmin; j < jmax; j += jplus)
		{
			xh = xhead + j; 
			xt = xtail + j; 
			xsegs[count].x1 = xh;     /* short := int */
			xsegs[count].y1 = yh;
			xsegs[count].x2 = xt;
			xsegs[count].y2 = yt;
			count++; 
			if (count > numsegs)
			{
				XSync(ops->display,0);
				return 0;
			}

		 } 
    } 
	numsegs = count;
	XSync(ops->display,0);
	return 0;
}

/***********************************************/

