/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/shr/RCS/utils.c,v 2.4 89/02/10 09:16:02 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/shr/RCS/utils.c,v $
*/
#include <andrewos.h>
#include <stdio.h>
#include <ctype.h>
#include <cui.h> /* Just for debugging, could as easily be ms.h */

extern char *index(), *malloc();

/* Random utilities used by BOTH cui and ms */

/* This is done with a char * declaration instead of a macro because it
	gets used so many times -- this makes the binaries smaller */

char *ErrDebugString = "Returning error codes %d %d %d\n";

/* This routine takes a string and returns a pointer to a version of it in which neither
	the beginning nor ending have white space.  It is destructive in the sense that
	the old string will also appear with the trailing white space deleted. */

char *
StripWhiteEnds(string)
char *string;
{
    char *stripped;
    int len;

    if (!string) return(string);
    for (stripped = string; *stripped && (*stripped == ' ' || *stripped == '\t' || *stripped == '\n'); ++stripped) {
	;
    }
    len = strlen(stripped) - 1;
    while (len && (stripped[len] == ' ' || stripped[len] == '\t' || stripped[len] == '\n')) {
	stripped[len] = 0;
	--len;
    }
    return(stripped);
}

bone(buf, len)
register char *buf;
int len;
{
    register char *s;

    s = buf + len -1;
    while (s >= buf) *s-- = '\377';
}

ReduceWhiteSpace(string)
char *string;
{
    char *old = string, *new = string;
    int InWhite = 1;

    while (*old) {
	switch (*old) {
	    case ' ':
	    case '\t':
	    case '\n':
		if (InWhite) {
		    break;
		}
		*old = ' '; /* always should be a space */
		InWhite = 1;
		*new++ = *old;
		break;
	    default:
		InWhite = 0;
		*new++ = *old;
		break;
	}
	++old;
    }
    *new = '\0';
}


#define BIGDATESTR 250

/* like rindex, but finds any of chars in second arg */

char *
multrindex(s, t)
char *s, *t;
{
    char *u, *v;

    for (u = s; *u; ++u) {
	;
    }
    --u; /* Now points to last char in s */
    while (u >= s) {
	for (v = t; *v; ++v) {
	    if (*v == *u) {
		return(u);
	    }
	}
	--u;
    }
    return((char *) NULL);
}

#define MAGICNAME ".MESSAGES"  /* This matches .MESSAGES*  */

BuildNickName(FullName, NickName)
char   *FullName,
       *NickName;
{
    char   *s, *t;

    for (s = FullName; *s; ++s) {
	if (*s == '.' && !strncmp(s, MAGICNAME, sizeof(MAGICNAME) - 1))
	    break;
    }
    if (*s) {
	t = index(s, '/');
	if (t) {
	    strcpy(NickName, t+1);
	} else {
	    strcpy(NickName, FullName);
	}
    }
    else {
	strcpy(NickName, FullName);
    }
    for (s = NickName; *s; ++s) {
	if (*s == '/')
	    *s = '.';
    }
    return(0);
}

/* The following routine compares the first string, which is assumed
	to be all lower case, to a second string which is not, and
	returns 0 if they are a case insensitive match */

lc2strncmp(s1, s2, len)
char *s1, *s2;
int len;
{
    while (len && *s1 && *s2 && (*s1 == *s2 || *s1 == tolower(*s2))) {
	++s1; ++s2; --len;
    }
    return(len && (*s1 || *s2));
}

LowerStringInPlace(string, len)
char *string;
int len;
{
    while (len--) {
	if (isupper(*string)) {
	    *string = *string - 'A' + 'a';
	}
	++string;
    }
}

char *
NextAddress(add)
char *add;
{
    int parenlevel = 0, inquotes = 0;

    while (*add) {
	switch(*add) {
	    case '"':
		inquotes = !inquotes;
		break;
	    case '(':
		++parenlevel;
		break;
	    case ')':
		--parenlevel;
		break;
	    case ',':
		if (!inquotes && (parenlevel <= 0)) return(add);
		break;
	    default:
		break;
	}
	++add;
    }
    return(NULL);
}


/* Returns pointer to static storage describing the integer foo.  For small
integers, this is "zero", "one", and so forth; for integers outside the
range, it's a pointer to overwritable static storage.  If Capitalized is
non-zero, it's a pointer to overwritable "Zero", "One", or so forth. */

static char *Nms[] = {"zero", "one", "two", "three", "four", "five", "six",
"seven", "eight", "nine", "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen",
"sixteen", "seventeen", "eighteen", "nineteen"};
static char *Tens[] = {"twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety"};
static char NmBuff[22];

char *cvEng(foo, Capitalized, MaxToSpellOut)
int foo, Capitalized, MaxToSpellOut;
{
	if (foo < 0 || foo >= 100 || foo >= MaxToSpellOut) {
		sprintf(NmBuff, "%d", foo);
		return NmBuff;
	}
	if (foo < 20) {
	    strcpy(NmBuff, Nms[foo]);
	} else {
	    strcpy(NmBuff, Tens[(foo/10) - 2]);
	    if (foo%10) {
		strcat(NmBuff, "-");
		strcat(NmBuff, Nms[foo%10]);
	    }
	}
	if (Capitalized && islower(NmBuff[0])) NmBuff[0] = toupper(NmBuff[0]);
	return NmBuff;
}
