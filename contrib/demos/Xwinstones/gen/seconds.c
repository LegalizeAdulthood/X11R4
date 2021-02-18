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
/********************************************/
/* Returns a float value representing time, */
/* since the begining of time.              */

#include "stones.h"

float 
seconds()
{
#ifdef AIX
#include <sys/types.h>
#include <sys/times.h>
#define TICKS_PER_SEC 60.0

	extern time_t times();
	time_t rtn_value;
	static struct tms tmb, *tmbp = &tmb;
	float flt_val;

	if ( (rtn_value=times(tmbp)) == -1 )
	{
		perror("times");
		return( -1.0 );
	}

	flt_val = (float)rtn_value / TICKS_PER_SEC ;
	return( flt_val );
#endif
}
