/*
 *	wstring.c  -- BSD string(3) for wchar_t
 */

/*
 * Copyright (c) 1989  Software Research Associates, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Software Research
 * Associates makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * Author:  Makoto Ishisone, Software Research Associates, Inc., Japan
 *		ishisone@sra.co.jp
 */

/* $Header: /private/usr/mmb/ishisone/job/x11/xlocal/RCS/wstring.c,v 1.3 89/09/12 10:14:06 ishisone Rel $ */

#include "WStr.h"

wchar_t *
wstrcat(s1, s2)
wchar_t *s1, *s2;
{
	wchar_t	*ret = s1;

	while (*s1++)
		;
	s1--;

	while (*s1++ = *s2++)
		;

	return ret;
}

wchar_t *
wstrncat(s1, s2, n)
wchar_t *s1, *s2;
int n;
{
	wchar_t	*ret = s1;

	while (*s1++)
		;
	s1--;

	while (n-- > 0 && (*s1++ = *s2++))
		;
	if (n < 0)
		*s1 = 0;

	return ret;
}

wstrcmp(s1, s2)
wchar_t *s1, *s2;
{
	while (*s1 && *s1 == *s2)
		s1++, s2++;
	return (*s1 - *s2);
}

wstrncmp(s1, s2, n)
wchar_t *s1, *s2;
int n;
{
	while (n-- > 0 && *s1 && *s1 == *s2)
		s1++, s2++;
	if (n < 0)
		return 0;
	return (*s1 - *s2);
}

wchar_t *
wstrcpy(s1, s2)
wchar_t *s1, *s2;
{
	wchar_t	*ret = s1;

	while (*s1++ = *s2++)
		;
}

wchar_t *
wstrncpy(s1, s2, n)
wchar_t *s1, *s2;
int n;
{
	wchar_t	*ret = s1;

	while (n-- > 0 && (*s1++ = *s2++))
		;
	while (n-- > 0)
		*s1++ = 0;

	return ret;
}

wstrlen(s)
wchar_t *s;
{
	int	n = 0;

	while (*s++)
	    n++;
	return n;
}

wchar_t *
#if __STDC__ == 1
/* to avoid current GCC bug */
windex(wchar_t *s, wchar_t c)
#else
windex(s, c)
wchar_t *s, c;
#endif
{
	int	x;

	while (x = *s++) {
		if (x == c)
			return s - 1;
	}

	return 0;
}

wchar_t *
#if __STDC__ == 1
/* to avoid current GCC bug */
wrindex(wchar_t *s, wchar_t c)
#else
wrindex(s, c)
wchar_t *s, c;
#endif
{
	wchar_t	*r = 0;
	int	x;

	while (x = *s++) {
		if (x == c)
			r = s - 1;
	}

	return r;
}

