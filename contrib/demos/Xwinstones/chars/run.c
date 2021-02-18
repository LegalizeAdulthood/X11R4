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

extern void (*DrawString)();
extern char *alphabet;
extern int numchars;
extern char *font2;

int
run(ops)
optionsPtr ops;
{
int i;
char c[1];
int j = 0;
int x = 0;
int y = 70;

currcount = &i;
currlimit = &(ops->count);

for(i=0; i<ops->count; i++)
{
	c[0] = alphabet[j];
	(*DrawString)(ops->display,ops->window,ops->fggc, x, y, c, 1);
	if (++j >= numchars) j=0;
	if (++x >= 300) x=0;
	if (++y >= 350) y=70;
	if (font2)
	{
		c[0] = alphabet[j];
		(*DrawString)(ops->display,ops->window,ops->bggc, x, y, c, 1);
		if (++j >= numchars) j=0;
		if (++x >= 300) x=0;
		if (++y >= 350) y=70;
		i++;
	}
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
