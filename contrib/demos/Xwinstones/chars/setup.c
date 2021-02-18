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

extern	void	XDrawString();
extern	void	XDrawImageString();

void	(*DrawString)()= XDrawString;

char *alphabet= NULL;
int numchars= 0;
char *units = "chars";
char *font2 = NULL;

extern int fg;
extern int bg;

int
setup(argc, argv, ops)
int argc;
char **argv;
optionsPtr ops;
{
XGCValues xgcv[1];
XColor installedcolor[1];
XColor databasecolor[1];
Pixmap pmap;
XFontStruct *fontstruct;
int tile=0;
int stip = 0;
int opaqstip= 0;
unsigned int osbg;
int i,nLeft,y;
char *pLeft;

alphabet = "ABCDEFGHIHJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

for(i=1; i<argc; i++)
{
	if (strcmp(argv[i], "-help")==0)  return(1);
	else if (strcmp(argv[i],"-h") ==0 )	
	{
		printf("\n");
		printf("\tCharstones uses XDrawString or XDrawImageString. \n");
		printf("\tCharstones' options are: \n");
		printf("\n");
	
		printf("\t-stip                stipple fill \n");
		printf("\t-tile                tile fill \n");
		printf("\t-string (string)     alphabet to use (default string=\"A-Za-z0-9\") \n");
		printf("\t-image               uses XDrawImageString instead of XDrawString(default) \n"); 
		printf("\t-fn2    (string)     font to use (default=%s) \n",ops->font);
		printf("\t-help                stones' help page \n");
		printf("\t-h                   this listing \n");
	
		printf("\n");
		return(1);
	}
	else if (strcmp(argv[i],"-tile") ==0 )	tile = 1;
	else if (strcmp(argv[i],"-stip") ==0 )	stip = 1;
	else if (strcmp(argv[i],"-opaqstipbg") ==0 )   
	{
			GETCOLOR(osbg, argv[++i], WhitePixel);
	}
	else if (strcmp(argv[i],"-string") ==0 ) alphabet= argv[++i];
	else if (strcmp(argv[i],"-image") ==0 )	DrawString= XDrawImageString;
	else if (strcmp(argv[i],"-fn2") ==0 )	font2 = argv[++i];
}

if (font2)
{
	if ((fontstruct = XLoadQueryFont(ops->display, font2))==(0))
	{
		printf("%s: Cannot find font %s\n", prog, font2);
		return(-1);
	}
	XSetFont(ops->display, ops->bggc, fontstruct->fid);
	XSetForeground(ops->display, ops->bggc, fg);
	XSetBackground(ops->display, ops->bggc, bg);
}

if (tile)
{
	pmap = XCreateBitmapFromData(ops->display, ops->window, 
		bitmap_bits, bitmap_width, bitmap_height);
	xgcv->fill_style = FillTiled;
	xgcv->tile = pmap;
	XChangeGC(ops->display, ops->fggc, GCFillStyle, xgcv);
}

if (stip)
{
	pmap = XCreateBitmapFromData(ops->display, ops->window, 
		bitmap_bits, bitmap_width, bitmap_height);
	xgcv->fill_style = FillStippled;
	xgcv->stipple = pmap;
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

numchars = strlen(alphabet);
nLeft= numchars; 
pLeft= alphabet; 
y= 10;
while (nLeft>0)
{
    if (nLeft>26)
	{
		(*DrawString)(ops->display,ops->window,ops->fggc, 0, y, pLeft, 26);
		nLeft-= 26; pLeft+= 26; y+= 10;
    }
    else
	{
		(*DrawString)(ops->display,ops->window,ops->fggc, 0, y, pLeft, nLeft);
		nLeft= 0;
    }
}
XSync(ops->display,0);
return 0;
}

/************************************************************/

