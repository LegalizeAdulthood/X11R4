#ifndef lint
static char *rcsid = "$Header: xtp.c,v 1.4 89/07/02 16:00:20 moraes Exp $";
#endif

#include <X11/Xos.h>
#include <stdio.h>
#include "defs.h"
#include "xtp.h"

char *def_font = "Roman";

char *
ChangeFont(style, size)
char *style;
int size;
{
	extern char *HashSearch();
	extern char *strsave();
	struct fontinfo *fp;
	struct fontinfo *FindClosestFont();
	char buf[64];

	(void) sprintf(buf, "%s.%d", style, size);
	fp = (struct fontinfo *) HashSearch(buf);

	if (fp != NULL) {
		/* We used it before, so it is cached in the hash table */
#ifdef DEBUG
		(void) fprintf(stderr, "%s cached as %s\n", buf, fp->texname);
#endif
		return(fp->texname);
	}
		
	/* 
	 *  We haven't cached the font, so we search for it in the list,
	 *  cache the nearest font we find under it's name. Also increase
	 *  the reference count so we know that it's been used
	 */
	fp = FindClosestFont(style, size);
	HashInsert(strsave(buf), (char *) fp);
	fp->refcnt++;
#ifdef DEBUG
	(void) fprintf(stderr, "%s maps to %s\n", buf, fp->texname);
#endif
	return(fp->texname);
}


struct fontinfo *
FindClosestFont(style, size)
char *style;
int size;
{
	register struct fontinfo *fp = availfonts;
	struct fontinfo *closest = NULL;
	register int n = navailfonts;
	register int bestdelta = 99999;
	register int delta;

	while(n-- > 0) {
		delta = ABS(fp->size - size);
		if (delta <= bestdelta) {
			if (STREQ(style, fp->style) || 
			 (delta < bestdelta && STREQ(def_font, fp->style))) {
			 	closest = fp;
				bestdelta = delta;
			 }
		}
		fp++;
	}
	if (bestdelta == 0 && STREQ(closest->style, style)) {
		return(closest);
	} else if (closest != NULL) {
		(void) fprintf(stderr,
		 "Couldn't find font \"%s\" in size %d - using \"%s\" in %d instead\n",
		 style, size, closest->style, closest->size);
	} else {
		(void) fprintf(stderr, "Help - couldn't find any fonts.\n");
		exit(-1);
	}
	return(closest);
}

void
PutTexFontPostScript( fd)
FILE *fd;
{
	register struct fontinfo *fp = availfonts;
	register int n = navailfonts;
	
	while(n-- > 0) {
		if (fp->refcnt > 0)
			(void)fprintf(fd, "\\font%s = %s\n", fp->texname, fp->texfontinit);
		fp++;
	}
}
