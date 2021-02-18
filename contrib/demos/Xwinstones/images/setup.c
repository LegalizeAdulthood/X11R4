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

XImage *image;
char *units = "Kpixels";
int special8c = 0;
int wd = 32; 
int ht = 32; 
int winW;
int winH;

int
setup(argc, argv, ops)
int argc;
char **argv;
optionsPtr ops;
{
/*
XGCValues xgcv[1];
*/
int i,j;
/*
Pixmap pmap;
*/
XWindowAttributes att[1];
unsigned char k = 0;
unsigned char data[768][512];
int format = ZPixmap;


for(i=1; i<argc; i++)
{
	if (strcmp(argv[i], "-help")==0) return(-1);
	else if (strcmp(argv[i], "-h")==0)
	{
		printf("\n");
		printf("\tImagestones uses XPutImage \n");
		printf("\tImagestones' options are: \n");
		printf("\n");
	
		printf("\t-wd (int)         width of image (default=%d) \n",wd);
		printf("\t-ht (int)         height of image (default=%d) \n",ht);
		printf("\t-8c               uses an 8c eXtention for XYPixmaps \n");
		printf("\t-XYformat         XYPimap format \n");
		printf("\t-Zformat          ZPixmap format (default) \n");
		printf("\t-help             stones' help page \n");
		printf("\t-h (int)          this listing \n");
	
		printf("\n");
		return(-1);
	}
	else if (strcmp(argv[i], "-wd")==0) wd = atoi(argv[++i]);
	else if (strcmp(argv[i], "-ht")==0) ht = atoi(argv[++i]);
	else if (strcmp(argv[i], "-8c")==0) special8c = 1;
	else if (strcmp(argv[i], "-Zformat")==0) format = ZPixmap;
	else if (strcmp(argv[i], "-XYformat")==0) format = XYPixmap;
}

#ifdef RTPC
if ( ( special8c ) && ( XCanUseXYblt(ops->display) )  )
	format = XYPixmap;
else special8c = 0;
#endif

for(i=0; i<wd; i++)
	for(j=0; j<ht; j++)
	{
		data[i][j] = k;
		if (k == (ops->dVisual->map_entries-1))
			k=0;
	   		else k++;
	}
/*	
pmap = XCreatePixmap(ops->display, ops->window, 
		wd, ht, DefaultDepth(ops->display, ops->dScreen));
*/
image = XCreateImage(ops->display, ops->dVisual,
		DefaultDepth(ops->display, ops->dScreen),
		format, 0, data, wd, ht, 32, 0);

XSync(ops->display,0);
XGetWindowAttributes(ops->display, ops->window, att);

winW = att->width;
winH = att->height;

XSync(ops->display,0);
return(0);
}

/************************************************************/
