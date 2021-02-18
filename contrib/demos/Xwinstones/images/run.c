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

#define SRC_X 0
#define SRC_Y 0
#define STEP  3

static int *currcount;
static int *currlimit;

extern int winW;
extern int winH;
extern int wd;
extern int ht;
extern XImage *image;
extern int special8c;

int
run(ops)
optionsPtr ops;
{
int i;
int dst_x = 0;
int dst_y = 0;

currcount = &i;
currlimit = &(ops->count);

if (special8c)
{
	for(i=0; i<ops->count; i++ )
	{
#ifdef RTPC
		XPutXYblt(ops->display, ops->window, ops->fggc, image, 
				SRC_X, SRC_Y, dst_x, dst_y, wd, ht);
#endif
		dst_x += STEP;
		dst_y += STEP;
		if (dst_x + wd >= winW) dst_x = 0;
		if (dst_y + ht >= winH) dst_y = 0;
	}
}
else 
{
	for(i=0; i<ops->count; i++ )
	{
		XPutImage(ops->display, ops->window, ops->fggc, image, 
				SRC_X, SRC_Y, dst_x, dst_y, wd, ht);
		dst_x += STEP;
		dst_y += STEP;
		if (dst_x + wd >= winW) dst_x = 0;
		if (dst_y + ht >= winH) dst_y = 0;
	}
}

return( (wd * ht * ops->count) / 1000 );
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
