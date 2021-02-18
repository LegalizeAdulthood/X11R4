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
/******************************************************/
/* tells operating system to create a signal event to */
/* occur in so many "secs".  When the event occurs,   */
/* the local "timeout" function which comes with the  */
/* local "run" function, attempts to kill "run".      */

#include "stones.h"

void
settimer(secs)
int secs;
{
	extern void timeout(); 
#ifdef AIX
	signal(SIGALRM, timeout);
	alarm((unsigned)secs); 
#endif

#ifdef BSD
	struct itimerval new[1], old[1];

	new->it_value.tv_sec = secs;
	new->it_value.tv_usec = 0;

	new->it_interval.tv_sec = secs;
	new->it_interval.tv_usec = 0;

	signal(SIGALRM, timeout);
	setitimer(ITIMER_REAL, new, old);
#endif
		
}

/****************************************************/
