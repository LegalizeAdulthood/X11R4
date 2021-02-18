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

int entries;
Colormap cmap;

char *units = "colors";

int
setup(argc, argv, ops)
int argc;
char **argv;
optionsPtr ops;
{
	int i,j;
	unsigned long k=0;
	int dx,dy;
	int xcount, ycount;
	XWindowAttributes xwa;
	GC	gc;

	for (i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "-help")==0) return(1);
		else if (strcmp(argv[i], "-h")==0)
		{
			printf("\n");
			printf("\tColorstones uses XStoreColors. \n");
			printf("\tColorstones options are: \n");
			printf("\n");
		
			printf("\t-help          stones' help page \n");
			printf("\t-h             this listing \n");
		
			printf("\n");
			return(1);
		}
	}

	switch (ops->dVisual->class)
	{
		case PseudoColor:
		case TrueColor:
		case DirectColor:
		case GrayScale:
			break;
		case StaticColor:
		case StaticGray:
		default:
			printf("Display %s has static colormaps",ops->server);
			printf(" in default visual.  %s not run\n", prog);
			return -1;	
	}

	XGetWindowAttributes(ops->display, ops->window, &xwa);
	cmap = XCreateColormap(ops->display, ops->window, ops->dVisual,
				AllocAll);
	
	entries = ops->dVisual->map_entries;
	xcount = ((int) (0.5 + sqrt((double)entries)));
	ycount = entries/xcount;
	dx = xwa.width / xcount;
	dy = xwa.height / ycount;
	gc = ops->fggc;

	for(i=0; i<xcount; i++)
		for(j=0; j<ycount; j++)
		{
			XSetForeground(ops->display, gc, k++);
			XFillRectangle(ops->display, ops->window,
				gc, i*dx, j*dy, dx, dy);
		}

	XInstallColormap(ops->display, cmap);
	XSync(ops->display,0);

	return 0;
}

/**************************************************/
