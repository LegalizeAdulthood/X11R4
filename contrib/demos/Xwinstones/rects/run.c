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

static int *currcount;
static int *currlimit;

extern void (*XDoRects)();
extern XRectangle xrect[];
extern int num_rects;
extern unsigned int area;
extern int fill;

int
run(ops)
optionsPtr ops;
{
	int     i;

	currcount = &i;
	currlimit = &(ops->count);

	for(i = 0; i < ops->count; i++)
	{
		(*XDoRects)(ops->display,ops->window,ops->fggc,xrect,num_rects);
	}

	if (fill)
		return( ops->count * ( area / 1000 ) );
	else 
	{
		extern char *units;
		units = "rect_verts";
		return( 4 * ops->count * num_rects );
	}
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
