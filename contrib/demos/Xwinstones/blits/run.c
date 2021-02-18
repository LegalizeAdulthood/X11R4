/*  Copyright International Business Machines, 1989
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
#define MARGIN 0

static int *currcount;
static int *currlimit;

extern int winW;
extern int winH;
extern int srcy;
extern int srcx;
extern unsigned int wd;
extern unsigned int ht;
extern int pixtowindow;
extern Pixmap pixmap;

int
run(ops)
optionsPtr ops;
{
	int     i;
	int     dstx = MARGIN;
	int     dsty = MARGIN;
	int     right = winW - wd;
	int     bottom = winH - ht;

	right -= MARGIN;
	bottom -= MARGIN;

	currcount = &i;
	currlimit = &(ops->count);

	/* printf("srcx=%d srcy=%d wd=%d ht=%d \n",srcx,srcy,wd,ht); */

	if (pixtowindow)
	{
		for(i = 0; i < ops->count; i++)
		{
			XCopyArea(ops->display,pixmap,ops->window,ops->fggc,
					srcx,srcy,wd,ht,dstx,dsty);
			if (++dstx >= right) dstx = MARGIN; 
			if (++dsty >= bottom) dsty = MARGIN;
		}
		
	}
	else 
	{
		for(i = 0; i < ops->count; i++)
		{
			XCopyArea(ops->display,ops->window,ops->window,ops->fggc,
					srcx,srcy,wd,ht,dstx,dsty);
			if (++dstx >= right) dstx = MARGIN; 
			if (++dsty >= bottom) dsty = MARGIN;
		}
	}

	return( ops->count * ((wd * ht) / 1000) );
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
