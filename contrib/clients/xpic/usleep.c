/* A cheap subsecond delay call for Un*xs that don't have one. */

#ifndef lint
static char *rcsid="$Header: usleep.c,v 1.4 89/07/02 15:59:19 moraes Exp $";
#endif

#include <X11/Xos.h>

usleep(n)
{
	struct timeval value;
	
	if (n <= 0)
		return;

	value.tv_usec = n % 1000000;
	value.tv_sec = n / 1000000;

	(void) select(1, 0, 0, 0, &value);
}
