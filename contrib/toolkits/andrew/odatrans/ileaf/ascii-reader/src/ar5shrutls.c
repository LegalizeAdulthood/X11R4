
/*
 * Copyright 1989 by Interleaf, Inc, 10 Canal Park,  Cambridge, MA. 02141
 *
 *		All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Interleaf not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Interleaf, Inc. makes no representations 
 * about the suitability of this software for any purpose. 
 * It is provided "AS IS" without express or implied warranty.
 *
 * INTERLEAF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL INTERLEAF BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * 
 */

/***********************************************************************\
 *									*
 *									*
 *	- Short array utility routines.					*
 *	These are substitutes for the strxxxx() family			*
 *									*
 *	These are miscellaneous routines which are not machine		*
 *	dependent.
 *									*
 *	Interleaf, Inc. Copyright 1986					*
 *	Ten Canal Park, Cambridge MA 02141				*
 *      See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT                *
 *									*
\***********************************************************************/

#ifdef INTERLEAF_INCLUDE_FILES
#include "cdefs.h"
#else
#ifndef NULL
#define NULL (0)
#endif
#endif


/*  ArShrlen
 *	return the number of shorts in a 0 terminated string of shorts
 */
int
ArShrlen( shr)
short*	shr;
{
    register short*	s;

    if ( ( s = shr ) == NULL )
	return ( 0 );
    while ( *s++ != 0 );
    return ( s-1 - shr );
}
  

/*
 * Compare short strings:  s1>s2: >0  s1==s2: 0  s1<s2: <0
 */
int
ArShrcmp(s1, s2)
	register short *s1, *s2;
{

	while (*s1 == *s2++)
		if (*s1++==0)
			return (0);
	return (*s1 - *--s2);
}

/*
 * Compare short strings (at most n bytes):  s1>s2: >0  s1==s2: 0  s1<s2: <0
 */

int
ArShrncmp(s1, s2, n)
	register short *s1, *s2;
	register int n;
{

	while (--n >= 0 && *s1 == *s2++)
		if (*s1++ == '\0')
			return(0);
	return (n<0 ? 0 : *s1 - *--s2);
}


/*
 * Copy short string s2 to s1.  s1 must be large enough.
 * return s1
 */
short*
ArShrcpy( s1, s2)
register short*	s1;
register short*	s2;
{
    short*	os1 = s1;

    while ( *s1++ = *s2++ )
	;
    return ( os1 );
}

/*
 * Return the ptr in sp at which the character c appears;
 * NULL if not found
 */

short *
ArShrindex(sp, c)
register short *sp, c;
{

    do
    {
	if (*sp == c)	return (sp);
    } while (*sp++);
    return (NULL);
}

/* Return a ptr to the LAST occurence of a short in a shring
 */

short *
ArShrrindex(sp, sh)
register short *sp,sh;
{
  register short *result = NULL;
  
  do {
    if (*sp == sh)
      result = sp;
  } while (*sp++);
  
  return (result);
}


/*
 * Concatenate s2 on the end of s1.  S1's space must be large enough.
 * Return s1.
 */

short *
ArShrcat(s1, s2)
	register short *s1, *s2;
{
	register short *os1;

	os1 = s1;
	while (*s1++)
		;
	--s1;
	while (*s1++ = *s2++)
		;
	return (os1);
}
/*
 * Copy s2 to s1, truncating or null-padding to always copy n shorts
 * return s1
 */

short *
ArShrncpy(s1, s2, n)
	register short *s1, *s2;
	register int	n;
{
	register i;
	register short *os1;

	os1 = s1;
	for (i = 0; i < n; i++)
		if ((*s1++ = *s2++) == '\0') {
			while (++i < n)
				*s1++ = '\0';
			return (os1);
		}
	return (os1);
}

/*
 * Concatenate s2 on the end of s1.  s1's space must be large enough.
 * At most n characters are moved.
 * Return s1.
 */

short *
ArShrncat(s1, s2, n)
	register short *s1, *s2;
	register n;
{
	register short *os1;

	os1 = s1;
	while (*s1++)
		;
	--s1;
	while (*s1++ = *s2++)
		if (--n < 0) {
			*--s1 = '\0';
			break;
		}
	return (os1);
}

/*  Mixed char and short array operations. Formerly in ndg/dgEq/eqstubs.c */
short*
ArShrstrcat(to, from)
register short* to;
register char* from;
{
    short* tox = to;
    while(*to++);
    --to;
    while(*to++ = *from++);
    return(tox);
}

/*
 * Copy char string s2 to short string s1.  s1 must be large enough.
 * return s1
 */
short*
ArShrstrcpy( s1, s2)
register short*	s1;
register  char*	s2;
{
    short*	os1 = s1;

    while ( *s1++ = *s2++ )
	;
    return ( os1 );
}

int
ArShrstrcmp(s1, s2)
register short* s1;
register char* s2;
{
    while (*s1 == *s2++)
	if (*s1++==0)
	    return (0);
    return (*s1 - *--s2);
}

/*
 * Compare strings (at most n bytes):  s1>s2: >0  s1==s2: 0  s1<s2: <0
 */
int
ArShrstrncmp(s1, s2, n)
	register short *s1;
	register char  *s2;
	register int n;
{

	while (--n >= 0 && *s1 == *s2++)
		if (*s1++ == '\0')
			return(0);
	return (n<0 ? 0 : *s1 - *--s2);
}

int
ArStrtoshr(sP, cP)
  register short *sP;
  register char *cP;
{
    while (*sP++ = *cP++) {}
}

/* Convert ascii string to a long integer.  string may be signed by a '-';
 * This function is equivalent to c library function atoi().
 */
	
long
ArAtoi(str)			/* old Mdtoi	*/
register char	*str;
{
    char sign = 0;
    register long	n = 0;

    if (*str == '-') {
	sign++;  str++;
    }

    for (; *str >= '0' && *str <= '9'; str++) {
	n *= 10;
	n += *str - '0';
    }

/*  if (*str != 0) return(0);  Semantics changed 9/23/87 to be like atoi() */

    return(sign == 0 ? n : -n);
}

/* Same as shrtostr, but copy limited to n (total) characters */
char *
ArShrnstr(d, s, n)
register unsigned char *d;
register short *s;
register int n;
{
    unsigned char *ds = d;

    for (n--; n > 0 && *s; n--) *d++ = *s++;
    *d = 0;
    return((char *) ds);
}


int 
ArAtoiS(shr)
short *shr;
{
    char cbuf[100];
    return ArAtoi(ArShrnstr(cbuf,shr,sizeof(cbuf)));
}

