
#ifndef lint
static char XRNrcsid[] = "$Header: /users/ricks/xrn-11/R4/RCS/newsrcfile.c,v 1.12 89/11/17 12:51:47 ricks Rel $";
#endif

/*
 * xrn - an X-based NNTP news reader
 *
 * Copyright (c) 1988, 1989, Ellen M. Sentovich and Rick L. Spickelmier.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * newsrcfile.c: routines for reading and updating the newsrc file
 *
 */

#include "copyright.h"
#include <X11/Xos.h>
#include <stdio.h>
#include "config.h"
#include "utils.h"
#include <sys/stat.h>
#ifdef VMS
#ifndef R_OK
#define F_OK            0       /* does file exist */
#define X_OK            1       /* is it executable by caller */
#define W_OK            2       /* writable by caller */
#define R_OK            4       /* readable by caller */
#endif R_OK
#define index strchr
#endif VMS
#ifdef aiws
#include <unistd.h>
#endif /* aiws */
#include "news.h"
#include "error_hnds.h"
#include "mesg.h"
#include <X11/Intrinsic.h>
#include "xrn.h"
#include "dialogs.h"
#include "resources.h"
#include "newsrcfile.h"

char *NewsrcFile;    /* newsrc file name    */
FILE *Newsrcfp;      /* newsrc file pointer */
char *optionsLine;   /*`options' line       */

#define OKAY  1
#define FATAL 0


/*
 * copy newsrc file to .oldnewsrc file
 */

static void
copyNewsrcFile(old, save)
char *old;   /* name of file to save */
char *save;  /* name of file to save to */
{
    FILE *orig, *new;
    char *newFile;
    int c;

    if ((orig = fopen(old, "r")) == NULL) {
	(void) sprintf(error_buffer, "Can not open the .newsrc file (%s) for copying\n",
			       old);
	ehErrorExitXRN(error_buffer);
    }

    if ((newFile = utTildeExpand(save)) == NIL(char)) {
	(void) sprintf(error_buffer, "Can not tilde expand the .newsrc save file (%s)\n",
			       save);
	ehErrorExitXRN(error_buffer);
    }

    if (STREQ(newFile, "")) {
	ehErrorExitXRN("hmmm.... file name is \"\"\n");
    }

    /* if .oldnewsrc is a link to .newsrc we could have trouble, so unlink it */
    (void) unlink(newFile);
    
    if ((new = fopen(newFile, "w")) == NULL) {
	(void) sprintf(error_buffer, "Can not open the .newsrc save file (%s)\n", save);
	ehErrorExitXRN(error_buffer);
    }

    while ((c = getc(orig)) != EOF) {
	(void) putc((char) c, new);
    }

    (void) fclose(orig);
    (void) fclose(new);

    return;
}


int
readnewsrc(newsrcfile, savenewsrcfile)
char *newsrcfile;
char *savenewsrcfile;
/*
 * read, parse, and process the .newsrc file
 *
 *   returns: 0 for fatal error, non-zero for okay
 *
 */
{
    struct stat buf;
    char *name;
    char *nntp;
    int nameLth;
#ifdef VMS
    int nntp_len;
    char *nntp_end;
#endif
    
    /* create the Newsrc array structure */
    Newsrc = ARRAYALLOC(struct newsgroup *, ActiveGroupsCount);

    optionsLine = NIL(char);

    if ((NewsrcFile = utTildeExpand(newsrcfile)) == NIL(char)) {
	mesgPane("Can not tilde expand the newsrc file name (%s)",
		       newsrcfile);
	return(FATAL);
    }

    /* check for .newsrc-NNTPSERVER */

    if (app_resources.nntpServer) {
	nntp = app_resources.nntpServer;
    } else {
        nntp = getenv("NNTPSERVER");
    }

    if (nntp) {
#ifdef VMS
	if (nntp_end = index(nntp,':')) {  /* Look for the : in the name */
	    nntp_len = nntp_end-nntp;	   /* Get the length of the first
					      part */
	} else nntp_len = strlen(nntp);	   /* If no colon, use the whole
					      string */
	nameLth = strlen(NewsrcFile) + nntp_len;
#else
        nameLth = strlen(NewsrcFile) + strlen(nntp);
#endif
        name = ARRAYALLOC(char, nameLth + 20);
        (void) strcpy(name, NewsrcFile);
        (void) strcat(name, "-");
#ifdef VMS
	(void) strncat(name, nntp, nntp_len);
#else
        (void) strcat(name, nntp);
#endif
      
        if (access(name, R_OK) != 0) {
	    NewsrcFile = XtNewString(NewsrcFile);
	} else {
	    NewsrcFile = XtNewString(name);
        }
	FREE(name);
    } else {
        NewsrcFile = XtNewString(NewsrcFile);
    }

    if (access(NewsrcFile, R_OK) != 0) {

	mesgPane("Creating a newsrc (%s) file for you", NewsrcFile);
	if ((Newsrcfp = fopen(NewsrcFile, "w")) == NULL) {
	    mesgPane("Can not create a newsrc file (%s)", NewsrcFile);
	    return(FATAL);
	}
	(void) fprintf(Newsrcfp, "news.announce.newusers:\n");
	(void) fclose(Newsrcfp);
    }

    if ((stat(NewsrcFile, &buf) == -1) || (buf.st_size == 0)) {
	(void) sprintf(error_buffer, "%s is zero length, aborting\n", NewsrcFile);
	ehErrorExitXRN(error_buffer);
    }

    if ((Newsrcfp = fopen(NewsrcFile, "r")) == NULL) {
	mesgPane("can not open the newsrc file for reading, %s", NewsrcFile);
	(void) perror("fopen");
	return(FATAL);
    }

    if (yyparse() != 0) {
	mesgPane("can not parse the newsrc file, %s", NewsrcFile);
	mesgPane("error on line %ld", MaxGroupNumber + 1);
	return(FATAL);
    }

    copyNewsrcFile(NewsrcFile, savenewsrcfile);

    (void) fclose(Newsrcfp);
    
    Newsrcfp = NIL(FILE);

    return(OKAY);
}


static char *
ngEntry(newsgroup)
struct newsgroup *newsgroup;
{
    int first = 1;
    struct list *item;
    char buffer[32], *end;
    static char line[2048];

    end = line;
    *end = '\0';

    /* process the .newsrc line */

    for (item = newsgroup->nglist; item != NIL(struct list); item = item->next) {
	switch (item->type) {
	    case SINGLE:
	    (void) sprintf(buffer, "%d", item->contents.single);
	    break;

	    case RANGE:
	    if (item->contents.range.start != item->contents.range.end) {
		(void) sprintf(buffer, "%d-%d",
			       item->contents.range.start,
			       item->contents.range.end);
	    } else {
		(void) sprintf(buffer, "%d", item->contents.range.start);
	    }
	    break;
	}
	if (!first) {
	    strcat(end++, ",");
	}
	first = 0;
	(void) strcat(end, buffer);
	end += utStrlen(buffer);
    }
    return(line);
}


int
updatenewsrc()
/*
 * write out an up to date copy of the .newsrc file
 *
 *   returns: 0 for fatal error, non-zero for okay
 *
 */
{
    ng_num indx;

    static FILE *newsrcfp;       /* file pointer for the newsc file      */
    static struct stat lastStat; /* last stat done on the file           */
    struct stat currentStat;     /* current stat                         */
    static int done = 0;
#ifndef VMS
    static char tempfile[4096];
#endif /* VMS */

    if (!done) {
        (void) stat(NewsrcFile, &lastStat);
	/* must be in the same filesystem so `rename' will work */
#ifndef VMS
	(void) sprintf(tempfile, "%s.temp", NewsrcFile);
#endif /* VMS */
	done = 1;
    }

    (void) stat(NewsrcFile, &currentStat);

    if (currentStat.st_mtime > lastStat.st_mtime) {
	if (ConfirmationBox(topLevel, ".newsrc file updated by another program, continue?") == XRN_CB_ABORT) {
	    ehErrorExitXRN("");
	}
    }

#ifndef VMS
    if ((newsrcfp = fopen(tempfile, "w")) == NULL) {
	mesgPane("Can not open the .newsrc temporary file: %s\n", tempfile);
	perror("fopen");
	return(FATAL);
    }
#else
    if ((newsrcfp = fopen(NewsrcFile, "w")) == NULL) {
	mesgPane("Can not create a new version of the .newsrc file %s\n", NewsrcFile);
	perror("fopen");
	return(FATAL);
    }
#endif /* VMS */

    /*
     * handle outputing the options line
     */
    if (optionsLine != NIL(char)) {
	(void) fprintf(newsrcfp, "%s\n", optionsLine);
    }

    for (indx = 0; indx < MaxGroupNumber; indx++) {
	struct newsgroup *newsgroup = Newsrc[indx];
	int nocomma = 1, inrange = 1;
	art_num lastread = 1, j;

	(void) fprintf(newsrcfp, "%s%c", newsgroup->name,
		       (IS_SUBSCRIBED(newsgroup) ? ':' : '!'));

	if (newsgroup->last == 0) {
	    (void) fprintf(newsrcfp, "\n");
	    continue;
	}

	if (newsgroup->articles == NIL(struct article)) {
	    if (newsgroup->nglist) {
		fprintf(newsrcfp, " %s\n", ngEntry(newsgroup));
	    } else {
		(void) fprintf(newsrcfp, " 1-%d\n", newsgroup->last);
	    }
	    continue;
	}
	
	if (newsgroup->last >= newsgroup->first) {

	    for (j = newsgroup->first; j <= newsgroup->last; j++) {
		if (inrange && IS_UNREAD(newsgroup->articles[INDEX(j)]) &&
		    !IS_UNAVAIL(newsgroup->articles[INDEX(j)])) {
		    if (lastread == j - 1) {
			(void) fprintf(newsrcfp, "%c%ld",
				       (nocomma ? ' ' : ','), lastread);
			nocomma = 0;
		    } else {
			if ((j - 1) > 0) {
			    (void) fprintf(newsrcfp, "%c%ld-%ld",
					   (nocomma ? ' ' : ','), lastread,
					   j - 1);
			    nocomma = 0;
			}
		    }

		    inrange = 0;
		} else if (!inrange && IS_READ(newsgroup->articles[INDEX(j)])) {
		    inrange = 1;
		    lastread = j;
		}
	    }
	    
	    if (inrange) {
		if (lastread == newsgroup->last) {
		    (void) fprintf(newsrcfp, "%c%ld",
				   (nocomma ? ' ' : ','), lastread);
		} else {
		    (void) fprintf(newsrcfp, "%c%ld-%ld",
				   (nocomma ? ' ' : ','), lastread,
				   newsgroup->last);
		}
	    }
	} else {
	    if (newsgroup->last > 1) {
		(void) fprintf(newsrcfp, " 1-%ld", newsgroup->last);
	    }
	}
	    
	(void) fprintf(newsrcfp, "\n");
    }
    (void) fclose(newsrcfp);

#ifndef VMS
    if (rename(tempfile, NewsrcFile) != 0) {
	mesgPane("rename of %s to %s failed\n", tempfile,
		       NewsrcFile);
	perror("rename");
	return(FATAL);
    }
#endif /* VMS */

    (void) stat(NewsrcFile, &lastStat);

    return(OKAY);
}

