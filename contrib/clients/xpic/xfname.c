#include <stdio.h>
#include <string.h>

#define DELIM '-'
#define size_t unsigned long

#ifdef XWINDOWS
# define emalloc	XtMalloc
#endif

extern char *emalloc();

char *
nstrsave(s, n)
char *s;
size_t n;
{
    char *cp = strncpy(emalloc(n + 1), s, n);
    *(cp + n) = '\0';
    return cp;
}

static char *
nexttok(cpp)
char **cpp;
{
    char *cp = *cpp;
    char *new;

    if (!*cp)
	return NULL;
    while(*cp && *cp != DELIM)
	*cp++;
    new = nstrsave(*cpp, (size_t) (cp - (*cpp)));
    *cpp = cp + (*cp ? 1 : 0);
    return new;
}

#include "xfname.h"

char **
getfontname(s)
char *s;
{
    char *cp = s;
    char **fname = (char **) emalloc(NCOMPONENTS * sizeof(char *));
    int i;

    for(i = 0; i < NCOMPONENTS && *cp; i++) {
	fname[i] = nexttok(&cp);
    }
    return fname;
}

#ifdef TEST
char *fmt = "FONTNAME_REGISTRY \"%s\"\n\
FAMILY_NAME \"%s\"\n\
FOUNDRY \"%s\"\n\
WEIGHT_NAME \"%s\"\n\
SETWIDTH_NAME \"%s\"\n\
SLANT \"%s\"\n\
ADD_STYLE_NAME \"%s\"\n\
PIXEL_SIZE %s\n\
POINT_SIZE %s\n\
RESOLUTION_X %s\n\
RESOLUTION_Y %s\n\
SPACING \"%s\"\n\
AVERAGE_WIDTH %s\n\
CHARSET_REGISTRY \"%s\"\n\
CHARSET_ENCODING \"%s\"\n\n";

int
main()
{
    char buf[512];
    char **fn;

    while(fgets(buf, sizeof buf, stdin) != NULL) {
	char *cp = strrchr(buf, '\n');
	if (cp)
	    *cp = '\0';
	fn = getfontname(buf);
	(void) printf(fmt, fn[FONTNAME_REGISTRY], fn[FAMILY_NAME],
	fn[FOUNDRY], fn[WEIGHT_NAME], fn[SETWIDTH_NAME], fn[SLANT],
	fn[ADD_STYLE_NAME], fn[PIXEL_SIZE], fn[POINT_SIZE],
	fn[RESOLUTION_X], fn[RESOLUTION_Y], fn[SPACING],
	fn[AVERAGE_WIDTH], fn[CHARSET_REGISTRY], fn[CHARSET_ENCODING]);
    }
    return 0;
}
#endif TEST
