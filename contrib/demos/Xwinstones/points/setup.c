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

#define MAXPTS 16380   /* 16380 * sizeof(XPoint) <= 65536(64K) */

char *units = "pixels";
int numpts = MAXPTS;
XPoint xpoints[MAXPTS];


int
setup(argc, argv, ops)
int argc;
char **argv;
optionsPtr ops;
{
XWindowAttributes att[1];
int winW; 
int winH;
int row;
int col;
int horiz = 1;
int vert = 0;
int top = 1;
int bottom = 0;
int left = 1;
int right = 0;
int i;

for(i=1; i<argc; i++)
{
	if (strcmp(argv[i], "-help")==0)  return(1);
	else if (strcmp(argv[i],"-h")==0)	
	{
		printf("\n");
		printf("\tPointstones uses XDrawPoints \n");
		printf("\tPointstones' options are: \n");
		printf("\n");
	
		printf("\t-npts   (int)         number of lines (default=%d) \n",numpts);
		printf("\t-maxpts (int)         %d  pts per call \n",MAXPTS);
		printf("\t-horiz                horizonal lines across window (default=%d) \n",horiz);
		printf("\t-vert                 vertical lines across window  (default=%d) \n",vert);
		printf("\t-left                 left to right across window   (default=%d) \n",left);
		printf("\t-right                right to left across window   (default=%d) \n",right);
		printf("\t-top                  top to bottom across window   (default=%d) \n",top);
		printf("\t-bottom               bottom to top across window   (default=%d) \n",bottom);
		printf("\t-help                 stones' help page \n");
		printf("\t-h                    this listing \n");
	
		printf("\n");
		return(1);
	}
	else if (strcmp(argv[i],"-npts")==0) numpts = atoi(argv[++i]);
	else if (strcmp(argv[i],"-maxpts")==0) numpts = MAXPTS;
	else if (strcmp(argv[i],"-horiz")==0) horiz = 1;
	else if (strcmp(argv[i],"-vert")==0) vert = 1;
	else if (strcmp(argv[i],"-left")==0) left = 1;
	else if (strcmp(argv[i],"-right")==0) right = 1;
	else if (strcmp(argv[i],"-top")==0) top = 1;
	else if (strcmp(argv[i],"-bottom")==0) bottom = 1;
}

if (numpts > MAXPTS) numpts = MAXPTS;
	
XSync( ops->display, 0);
XGetWindowAttributes(ops->display, ops->window, att);

winW = att->width;
winH = att->height;

if ((bottom) && (right) && (vert))
{
	/* printf("bottom right vert \n"); */
	row = winH - 2;
	col = winW - 2;

	for (i = 0; i < numpts; i++)
	{
		xpoints[i].x = col;
		xpoints[i].y = row;
		if (--row < 0) 
		{
			row = winH - 2;
			if (--col < 0) col =  winW - 2;
		}
	}
}
else if ((bottom) && (left) && (vert))
{
	/* printf("bottom left vert \n"); */
	row = winH - 2;
	col = 1;

	for (i = 0; i < numpts; i++)
	{
		xpoints[i].x = col;
		xpoints[i].y = row;
		/* printf(" xpoint[%d] = (%d,%d) \n",i,col,row); */
		if (--row < 0) 
		{
			row = winH - 2;
			if (++col > winH) col = 1;
		}
	}
}
else if ((top) && (right) && (vert))
{
	/* printf("top right vert \n"); */
	row = 1;
	col = winW - 2;

	for (i = 0; i < numpts; i++)
	{
		xpoints[i].x = col;
		xpoints[i].y = row;
		/* printf(" xpoint[%d] = (%d,%d) \n",i,col,row); */
		if (++row > winH) 
		{
			row = 1;
			if (--col < 0) col = winW - 2;
		}
	}
}
else if ((top) & (left) && (vert))
{
	/* printf("top left vert \n"); */
	row = 1;
	col = 1;

	for (i = 0; i < numpts; i++)
	{
		xpoints[i].x = (short)col;
		xpoints[i].y = (short)row;
		/* printf(" xpoint[%d] = (%d,%d) \n",i,col,row); */
		if (++row > winH) 
		{
			row = 1;
			if (++col > winW) col = 1;
		}
	}
}
else if ((bottom) && (right) && (horiz))
{
	/* printf("bottom right horiz \n"); */
	row = winH - 2;
	col = winW - 2;
	/* row = 200; */
	/* col = 300; */

	for (i = 0; i < numpts; i++)
	{
		xpoints[i].x = col;
		xpoints[i].y = row;
		/* printf(" xpoint[%d] = (%d,%d) \n",i,col,row); */
		if (--col < 0) 
		{
			col = winW - 2;
			/* col = 300; */
			/* if (--row < 0) row = 200; */
			if (--row < 0) row =  winH - 2;
		}
	}
}
else if ((bottom) && (left) && (horiz))
{
	/* printf("bottom left horiz \n"); */
	row = winH - 2;
	col = 1;
	/* row = 200; */
	/* col = 300; */

	for (i = 0; i < numpts; i++)
	{
		xpoints[i].x = col;
		xpoints[i].y = row;
		/* printf(" xpoint[%d] = (%d,%d) \n",i,col,row); */
		if (++col > winW) 
		{
			/* col = 300; */
			col = 1;
			if (--row < 0) row = winH - 2;
		}
	}
}
else if ((top) && (right) && (horiz))
{
	/* printf("top right horiz \n"); */
	row = 1;
	col = winW - 2;

	for (i = 0; i < numpts; i++)
	{
		xpoints[i].x = col;
		xpoints[i].y = row;
		/* printf(" xpoint[%d] = (%d,%d) \n",i,col,row); */
		if (--col < 0) 
		{
			col = winW - 2;
			if (++row > winH) row = 1;
		}
	}
}
else if ((top) & (left) && (horiz))     /* defualt */
{
	/* printf("top left horiz \n"); */
	row = 1;
	col = 1;

	for (i = 0; i < numpts; i++)
	{
		xpoints[i].x = (short)col;
		xpoints[i].y = (short)row;
		/* printf(" xpoint[%d] = (%d,%d) \n",i,col,row); */
		if (++col > winW) 
		{
			col = 1;
			if (++row > winH) row = 1;
		}
	}
}
else
{
	printf("unknown direction \n");
	return 1;
}
/* printf("finished setup \n"); */

XSync(ops->display,0);
return 0;
}

/***********************************************/

