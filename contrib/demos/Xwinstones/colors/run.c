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

extern int entries;
extern Colormap cmap;

static int *currcount;
static int *currlimit;

int
run(ops)
optionsPtr ops;
{
int i;
int j = 0;
XColor color[1];
/* unsigned red, green, blue; */

currcount = &i;
currlimit = &(ops->count);

for(i=0; i<ops->count; i++)
	{
	color->pixel = j;
	color->flags = DoRed | DoGreen | DoBlue;
	color->red +=0x0030;
	color->green += 0x0700;
	color->blue += 0x0500;
	/* color->red = red +=0x0030; */
	/* color->green = green += 0x0700; */
	/* color->blue = blue += 0x0500; */
	XStoreColor(ops->display, cmap, color);
	if (color->pixel != j) 
	{
		fprintf(stderr,"Optimized or failed store: ");
		fprintf(stderr,"%d vs %d\n",color->pixel, j);
		printf("%s: stones not meaningful\n");
		return 0;
	}
	if (++j >= entries) j=0;
}
return i;
}


static beenhere = 0;
void
timeout()
{
if (beenhere)
	{
	fprintf(stderr,"TIMEOUT! Second time called in %s. Aborting. \n",prog);
	exit(1);
	}
printf("TIMEOUT! Maximum time elapsed.  %s value may be skewed\n",prog);
*currlimit = *currcount-1;
beenhere = -1;
stonesperturbed = -1;
}
