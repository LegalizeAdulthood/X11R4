/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/itops.c,v 2.2 89/02/08 15:34:48 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/itops.c,v $
*/
/* This routine converts a long integer to a string padded or truncated to an
	exact length.  The length persists, though the value may not. */

static char Digits[] = "0123456789";

itops(num, buf, len)
long    num;
char   *buf;
int     len;
{
    int     i,
            mod,
            div,
            neg = 0;

    if (num < 0) {
	neg = 1;
	num = -num;
    }
    for (i = len - 1, mod = 10, div = 1; i >= 0; i--, mod *= 10, div *= 10) {
	buf[i] = Digits[(num % mod) / div];
	num -= (num % mod) / div;
    }
    buf[len] = '\0';
    if (neg)
	buf[0] = '-';
}
