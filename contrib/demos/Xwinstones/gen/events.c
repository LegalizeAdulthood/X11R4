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

void HandleEvents(dpy)
Display *dpy;
{
XEvent event;
XNextEvent(dpy,&event);
switch(event.type)
	{
	case ConfigureNotify:
	case MapNotify:
		if (stonesrunning)
			stonesperturbed = -1;
		break;
	case Expose:
		{
		int ct;

		ct = ((XExposeEvent *) &event)->count;
		while (ct--) XNextEvent(dpy,&event);
		if ( firstExposeExpected )
			firstExposeExpected = 0 ;
		else if (stonesrunning)
			stonesperturbed = -1;
		}
		break;
	case UnmapNotify: /*BLOCK UNTIL REMAPPED */
		do {
		   XNextEvent(dpy,&event);
		   }
		   while(event.type != MapNotify);
	default: (void) printf("%s: weird event %d\n",prog,event.type);
		if (stonesrunning)
			stonesperturbed = -1;
		break;
	}
}
