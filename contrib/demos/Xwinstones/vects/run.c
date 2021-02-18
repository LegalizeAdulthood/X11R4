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

extern int width;
extern int numsegs;
extern int single;
extern XSegment xsegs[];

int
run(ops)
optionsPtr ops;
{
int i;
int j;

currcount = &i;
currlimit = &(ops->count);

#define X1 xsegs[j].x1
#define Y1 xsegs[j].y1
#define X2 xsegs[j].x2
#define Y2 xsegs[j].y2

if (single)
{
	
	j=0;
	for(i=0; i<ops->count; i++)
	{
		XDrawLine(ops->display,ops->window,ops->fggc, X1,Y1,X2,Y2);
		if (++j >= numsegs) j=0;
	}
	return i;
}
else
{
	for(i=0; i<ops->count; i++)
	{
		XDrawSegments(ops->display,ops->window,ops->fggc, xsegs, numsegs);
		if (++j >= numsegs) j=0;
	}
	return(i * numsegs);
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
