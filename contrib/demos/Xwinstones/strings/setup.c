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

extern	void	XDrawString();
extern	void	XDrawImageString();

void	(*DrawString)()= XDrawString;

char *alphabet;
int numchars;

char *units = "chars";

extern int fg;
extern int bg;

int
setup(argc, argv, ops)
int argc;
char **argv;
optionsPtr ops;
{
XGCValues xgcv[1];
Pixmap pmap;
XWindowAttributes att[1];
XFontStruct *fontstruct;
XColor installedcolor[1];
XColor databasecolor[1];
unsigned int osbg;
int textWidth;
int stip = 0;
int tile = 0;
int opaqstip = 0;
int i;
int nchars;

alphabet = "ABCDEFGHIHJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
numchars = strlen(alphabet);
nchars = numchars;

for(i=1; i<argc; i++)
{
	if (strcmp(argv[i], "-help")==0)  return(1);
	else if (strcmp(argv[i],"-h")==0 ) 
	{
		printf("\n");
		printf("\tStringstones uses XDrawString or XDrawImageString \n");
		printf("\tStringstones' options are: \n");
		printf("\n");

		printf("\t-nchars                number of characters per line if in window (default=%d) \n",numchars);
		printf("\t-image                 uses XDrawImageString instead of XDrawString (default) \n");
		printf("\t-tile                  tile fill \n");
		printf("\t-stip                  stipple fill \n");
		printf("\t-opaqstip              opaque stipple fill \n");
		printf("\t-opaqstipbg (string)   opaque stipple fill background \n");
		printf("\t-string     (string)   string to use other than default \n");
		printf("\t-help                  stones' help page \n");
		printf("\t-h                     this listing \n");
	
		printf("\n");

		return(1);
	}
	else if (strcmp(argv[i],"-stip") ==0 )	stip = -1;
	else if (strcmp(argv[i],"-tile") ==0 )	tile = -1;
	else if (strcmp(argv[i],"-opaqstip") ==0 )	opaqstip = -1;
	else if (strcmp(argv[i],"-opaqstipbg") ==0 )   
	{
			GETCOLOR(osbg, argv[++i], WhitePixel);
	}
	else if (strcmp(argv[i],"-nchars") ==0 ) nchars = atoi(argv[++i]);
	else if (strcmp(argv[i],"-image") ==0 )	DrawString= XDrawImageString;
	else if (strcmp(argv[i],"-string")==0) strcpy(alphabet,argv[++i]);
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

XSync(ops->display,0);
fontstruct = XLoadQueryFont(ops->display, ops->font);
if (fontstruct == 0)
{
	printf("Fatal: %s: Font %s failed\n", prog, ops->font);
	return -1;
}
XSync(ops->display,0);
if (numchars > nchars)
	numchars = nchars;
textWidth = XTextWidth(fontstruct, alphabet, numchars);
XGetWindowAttributes(ops->display, ops->window, att);

while(textWidth+15 > att->width)
	{
	numchars--;
	if (numchars < 1) 
		{
		printf("Fatal: %s: Font %s too wide for %s\n", 
				prog, ops->font, ops->server);
		return -1;
		}
	textWidth = XTextWidth(fontstruct, alphabet, numchars);
	}

(*DrawString)(ops->display,ops->window,ops->fggc, 10, 20, alphabet, numchars);
XSync(ops->display,0);
return 0;
}

/*********************************************************/

