/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/search.c,v 2.5 89/02/24 08:43:45 ghoti Exp $ */
/* $ACIS:search.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/search.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/search.c,v 2.5 89/02/24 08:43:45 ghoti Exp $";
#endif /* lint */


#include <ctype.h>

#include <class.h>
#include <smpltext.ih>
#include <search.eh>

static char FoldTRT[128] = {
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, ' ', '!', '"', '#', '$', '%',
'&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4',
'5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', 'a', 'b', 'c',
'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '[', '\\', ']', '^', '_', '`', 'a',
'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', 127
};

struct SearchPattern {
    short size, used;
    unsigned char *body;
};
int MatchLength;

/* Special operators in the search pattern. */
#define STAR 0200	/* what follows may be repeated zero or more times */
#define BPAR 0201	/* begin parenthesized expression */
#define EPAR 0202	/* end parenthesized expression */
#define SET  0203	/* match any of the following set (16 chars of bits follow to define the set) */
#define ANY1 0204	/* match any single character */
#define isop(c)	((c) & 0200)	/* true if c is a special op */


search__GetMatchLength (classID)
struct classheader *classID;
{
    return MatchLength;
}

static unsigned char *
SkipOp (s)
register unsigned char *s; {
    switch (*s++) {
	default: 
	    return s;
	case STAR: 
	    return SkipOp (s);
	case SET: 
	    return s + 16;
	case BPAR: 
	    while (*s)
		if (*s == EPAR)
		    return s + 1;
		else
		    s = SkipOp (s);
	    return s;
    }
}

char *
search__CompilePattern (classID, string, result)
struct classheader *classID;
register char *string;
struct SearchPattern  **result; {
    register struct SearchPattern  *p;
    register    used = 0;
    int     LastStart = -1;
    int     ParenStack[20];
    int     ParenDepth = 0;
    if ((p = *result) == 0) {
	p = (struct SearchPattern  *) malloc (sizeof (struct SearchPattern));
	p -> body = (unsigned char *) malloc (p -> size = 100);
	*result = p;
    }
    p -> used = 0;
    while (*string) {
	int     ThisStart = used;
	if (used + 20 >= p -> size)
	    p -> body = (unsigned char *) realloc (p -> body, p -> size += 50);
	switch (*string) {
	    case '.': 
		p -> body[used++] = ANY1;
		break;
	    case '\\': 
		switch (*++string) {
		    case '(': 
			if (ParenDepth>19)
			    return "Too many \\('s in regular expression";
			p -> body[used++] = BPAR;
			ParenStack[ParenDepth++] = ThisStart;
			break;
		    case ')': 
			p -> body[used++] = EPAR;
			if (ParenDepth <= 0)
			    return "Unmatched \\) in regular expression";
			ThisStart = ParenStack[--ParenDepth];
			break;
		    default: 
			p -> body[used++] = *string;
			break;
		}
		break;
	    case '[': {
		    register unsigned char *set = p -> body + used;
		    register    inverted = 0;
		    *set++ = SET;
		    if (*++string == '^')
			inverted = -1, string++;
		    {
			register    n;
			for (n = 16; --n >= 0;)
			    *set++ = inverted;
		    }
		    set = p -> body + used + 1;
		    used += 17;
		    while (*string) {
			register    min = *string;
			register    max;
			if (string[1] == '-')
			    max = string[2], string += 2;
			else
			    max = min;
			while (min <= max) {
			    if (isalpha(min)) {
				register FlippedMin = min ^ 040;
			    if (inverted)
				set[FlippedMin >> 3] &= ~(1 << (FlippedMin & 7));
			    else
				set[FlippedMin >> 3] |= 1 << (FlippedMin & 7);
			    }
			    if (inverted)
				set[min >> 3] &= ~(1 << (min & 7));
			    else
				set[min >> 3] |= 1 << (min & 7);
			    min++;
			}
			if (*++string == ']')
			    break;
		    }
		    if (*string == 0)
			return "Unmatched '[' in regular expression";
		    break;
		}
	    case '*': 
		if (LastStart >= 0) {
		    register unsigned char *d = p -> body + used;
		    register    n = used - LastStart;
		    while (--n >= 0)
			d[0] = d[-1], d--;
		    *d = STAR;
		    used++;
		    ThisStart = -1;   /* disallow another * */
		    break;
		}
		else return "'*' not after pattern in regular expression";
	    default: 
		p -> body[used++] = FoldTRT[*string];
		break;
	}
	string++;
	LastStart = ThisStart;
    }  /* end of while loop */
    if (ParenDepth != 0)
	return "Unmatched \\( in regular expression";
    p -> body[used] = 0;
    p -> used = used;
    return 0;
}

search__MatchPattern (classID, d, pos, p)
struct classheader *classID;
register struct simpletext *d;
register pos;
register struct SearchPattern *p; {
    unsigned char *s;
    register int canopt;
    register unsigned char optchar;
    register int dl;
    long bufLen;
    char *buf = NULL;

    MatchLength = 0;
    if (p == 0 || p -> used <= 0) return -1;
    optchar = p->body[0];
    canopt = !isop(optchar);
    dl = simpletext_GetLength(d);
    bufLen = 0;
    for (; pos < dl; ++pos) {
	int n;
        if (bufLen == 0)
            buf = simpletext_GetBuf(d, pos, 1024, &bufLen);
	if (canopt)
            if (optchar != FoldTRT[bufLen--, *buf++])
                continue;
	s = p -> body;
	n = TryMatch (d, pos, &s, 1);
	if (n>=0) {
	    MatchLength = n - pos;
	    return pos;
	}
    }
    return -1;
}

search__MatchPatternReverse (classID, d, pos, p)
struct classheader *classID;
register struct simpletext *d;
register pos;
register struct SearchPattern *p; {
    unsigned char *s;
    register int canopt;
    register unsigned char optchar;
    long bufLen;
    char *buf = NULL;

    MatchLength = 0;
    if (p == 0 || p -> used <= 0) return -1;
    optchar = p->body[0];
    canopt = !isop(optchar);
    bufLen = 0;
    for (; pos >= 0; --pos) {
	int n;
        if (bufLen == 0)
            buf = simpletext_GetBufEnd(d, pos + 1, 1024, &bufLen);
	if (canopt)
            if (optchar != FoldTRT[bufLen--, *--buf])
                continue;
	s = p -> body;
	n = TryMatch (d, pos, &s, 1);
	if (n>=0) {
	    MatchLength = n - pos;
	    return pos;
	}
    }
    return -1;
}

static TryMatch (d, pos, s, loop)
register struct simpletext *d;
register pos;
register unsigned char **s; {
    register unsigned char  c,
                            dc;
    char *buf = NULL;
    long bufLen = 0;
    long length = simpletext_GetLength(d);

    do {
	if (c = *(*s)++) {
	    switch (c) {
		case BPAR:
		    if ((pos = TryMatch (d, pos, s, 1)) < 0)
			return pos;
		    break;
		case EPAR:
		    return pos;
		case ANY1:
                    if (bufLen == 0)
                        buf = simpletext_GetBuf(d, pos, 1024, &bufLen);
		    dc = *buf++, bufLen--;
		    if (pos >= length || dc == '\n')
			return -1;
		    pos++;
		    break;
		case SET:
                    if (bufLen == 0)
                        buf = simpletext_GetBuf(d, pos, 1024, &bufLen);
		    dc = *buf++, bufLen--;
		    if (((*s)[dc >> 3] & (1 << (dc & 7))) == 0)
			return -1;
		    *s += 16;
		    pos++;
		    break;
		case STAR:
		    {
			unsigned char  *st = *s;
			int     npos, code;
			/* first check if we've got another in a row */
			if ((npos = TryMatch (d, pos, &st, 0)) >= 0) {
			    (*s)--;
			    /* try to continue wildcard matching */
			    code = TryMatch (d, npos, s, 1);
			    /* if that fails, see if the rest of the pattern matches here */
			    if (code < 0) return TryMatch(d, pos, &st, 1);
			    else return code;
			}
			else
			    *s = SkipOp (*s);
		    }
		    break;
		default:
		    if (pos >= length) return -1;
                    if (bufLen == 0)
                        buf = simpletext_GetBuf(d, pos, 1024, &bufLen);
		    dc = *buf++, bufLen--;
		    if (c != FoldTRT[dc])
			return - 1;
		    pos++;
		    break;
	    }
	}
	else
	    break;
    } while (loop);
    return pos;
}
