/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/msclients/vui/RCS/andmpc.c,v 2.8 89/08/18 00:07:30 ghoti Exp $ */
/* $ACIS: $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/msclients/vui/RCS/andmpc.c,v 2.8 89/08/18 00:07:30 ghoti Exp $ ";
#endif /* lint */

/* 
 * This package contains more C library routines either 1.missing from those
 * supplied with the MSC/IBM compilers, or 2. included in libmail or libitc.
 *
 *  Andrew Version
 *
 */

#include <ctype.h>
#include <andrewos.h>
#include <util.h>
ULsubstr(s1, s2)
char *s1, *s2;
{
    while (*s2 && *s1 &&
	   ((isupper(*s1) ? tolower(*s1) : *s1) ==
	    (isupper(*s2) ? tolower(*s2) : *s2))) {
	++s1; ++s2;
    }
    if (!*s2) return(0);
    return((isupper(*s1) ? tolower(*s1) : *s1) - (isupper(*s2) ? tolower(*s2) : *s2));
}

char *memcpy_preserve_overlap(dest, source, len)
char *dest, *source;
int len;
{
#ifndef sun
    bcopy(source, dest, len);
#else /* sun */
    memcpy(dest, source, len);
#endif /* sun */

    return dest;
}

#if 0
int ULstrncmp(s1, s2,count)
int count;
register char *s1, *s2;
{
/* case INSENSITIVE:  Compare strings:  s1>s2: >0  s1==s2: 0  s1<s2: <0 */

    register char i,c1,c2;

    for(i=0;i<count;i++) {
	c1 = *s1++; if (c1 <= 'Z') if (c1 >= 'A') c1 += 040;
	c2 = *s2++; if (c2 <= 'Z') if (c2 >= 'A') c2 += 040;
	if (c1 != c2) break;
	if (c1 == '\0') return(0);
    }
    return(c1 - c2);
}
#endif /* 0 */

