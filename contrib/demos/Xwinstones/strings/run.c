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

#define MAXASCENT 	22         /* ascent(Rom22.500) = 22 */
#define MAXDESCENT 	12         /* descent(Rom22.500) = 8 */
#define MINWIDTH	0
#define MAXWIDTH 	12         /* width(Rom22.500) = 12 */

static int *currcount;
static int *currlimit;

extern void (*DrawString)();
extern char *alphabet;
extern int numchars;

int
run(ops)
optionsPtr ops;
{
int i;
int x = MINWIDTH;
int y = MAXASCENT;
int bottom;

bottom = ops->winH - MAXDESCENT;

currcount = &i;
currlimit = &(ops->count);

for(i=0; i<ops->count; i++)
{
	(*DrawString)(ops->display,ops->window,ops->fggc, x, y, alphabet, numchars);
	if (++x >= MAXWIDTH) x = MINWIDTH;
	if (++y >= bottom) y = MAXASCENT;
}
return (i * numchars);
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
