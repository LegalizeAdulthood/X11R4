
#ifndef lint
static char XRNrcsid[] = "$Header: /users/ricks/xrn-11/R4/RCS/save.c,v 1.9 89/11/17 12:51:57 ricks Rel $";
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
 * save.c: routines for saving articles and sending articles to processes
 */

#include "copyright.h"
#include <X11/Xos.h>
#include <ctype.h>
#include <stdio.h>
#include "utils.h"
#include "config.h"
#ifndef VMS
#include <sys/param.h>
#else
#define MAXPATHLEN 512
#ifndef R_OK
#define F_OK            0       /* does file exist */
#define X_OK            1       /* is it executable by caller */
#define W_OK            2       /* writable by caller */
#define R_OK            4       /* readable by caller */
#endif R_OK
#define index strchr
#endif
#include <errno.h>
#include <sys/stat.h>
#include "news.h"
#include "resources.h"
#include "server.h"
#include "mesg.h"
#include "save.h"

extern int errno;

#define BUFFER_SIZE 1024


#ifndef VMS
static int
processArticle(command, artfile)
char *command;
char *artfile;
/*
 * send the current article to 'command'
 *
 *   returns: 1 for OKAY, 0 for FAILURE
 *
 */
{
    FILE *process, *fromfp;
    extern FILE *popen();	/* sequent */
    int c;

    if ((fromfp = fopen(artfile, "r")) == NULL) {
	mesgPane("can not open article file `%s' for reading", artfile);
	return(0);
    }
    
    if ((process = popen(command, "w")) == NULL) {
	mesgPane("can not start `%s'", command);
	(void) fclose(fromfp);
	return(0);
    }

    while ((c = getc(fromfp)) != EOF) {
	(void) putc((char) c, process);
    }

    (void) pclose(process);
    (void) fclose(fromfp);

    return(1);
}
#endif /* VMS */

int
createNewsDir()
/*
 * make sure that the news directory exists before trying to update it
 *
 *   returns: 0 for failure, 1 for okay
 */
{
    static int done = 0;
    char *newdir;

    if (done) {
	return(1);
    }
    if ((newdir = utTildeExpand(app_resources.saveDir)) == NIL(char)) {
	mesgPane("Cannot create save directory `%s'", app_resources.saveDir);
	return(0);
    }
    if ((mkdir(newdir, 0777) == -1) && (errno != EEXIST)) {
	mesgPane("Cannot create save directory `%s'",
		       app_resources.saveDir);
	return(0);
    }
    done = 1;
    app_resources.expandedSaveDir = XtNewString(newdir);

    return(1);
}


#ifdef VMS
static void
VmsMakeDirName(dummy, savedir, Group)
    char *dummy;
    char *savedir;
    char *Group;
{
    int temp;
    (void) strcpy(dummy, savedir);	/* Copy save directory name */
    temp = strlen(dummy);		/* Fetch the length */
    if (dummy[temp-1] = ']') {		/* If a directory spec, */
        dummy[temp-1] = '.';		/*  Convert to a .  */
    } else {
	(void) strcat(dummy, "[.");	/* Else, start directory */
    }
    (void) strcat(dummy, Group);	/* Add the group name */
    (void) strcat(dummy, "]");		/* Now terminate the string */
}
#endif /* VMS */


static char *
buildFileName(filename, savedir, group)
char *filename;    /* file name, possibly with a '~' */
char *savedir;     /* save directory                 */
char *group;       /* name of the news group         */
/*
 * expand a file name for 'saving' a file
 *
 *   returns: the name of the file or NIL(char) if the filename is bad
 *            (i.e. ~user/xx where 'user' is not a valid user name)
 *            the area returned is static
 *
 */
{
    char Group[GROUP_NAME_SIZE];
#ifdef aiws
    static char dummy[MAXPATH];
#else
    static char dummy[MAXPATHLEN];
#endif /* aiws */
#ifdef VMS
    char *dot;	   /* Pointer to dots to convert to underscores */
#endif

#ifndef VMS
    /* Make a local copy of the group name for modification */
    (void) strncpy(Group, group, sizeof(Group));
    /* upcase the first letter of the group name (same as 'rn') */
    if (islower(Group[0])) {
	Group[0] = toupper(Group[0]);
    }
#else /* VMS */
    /* First, use the generic group to filename conversion routine and
       convert the group name to a filename */
    (void) utGroupToVmsFilename(Group, group);

    /* If we are in ONEDIR_SAVE mode, find the first underscore character
       and convert it (back) to a dot.  This is done for compatibility with
       VNEWS. */
    if (app_resources.saveMode & ONEDIR_SAVE) {
	if (dot = index(Group, '_')) {
	    *dot = '.';
	}
    }
#endif

    if ((filename == NIL(char)) || (*filename == '\0')) {
	if (app_resources.saveMode & ONEDIR_SAVE){
#ifndef VMS
	    (void) sprintf(dummy, "%s/%s", savedir, Group);
#else
	    (void) sprintf(dummy, "%s%s", savedir, Group);
#endif
	} else {
	    /* use "saveDir/group" */
#ifndef VMS
	    (void) sprintf(dummy, "%s/%s", savedir, group);
#else
	    VmsMakeDirName(dummy, savedir, Group);
#endif /* VMS */
	    (void) mkdir(utTildeExpand(dummy), 0777);
#ifndef VMS
	    (void) strcat(dummy, "/");
#endif /* VMS */
	    (void) strcat(dummy, Group);
#ifdef VMS
	    if (!(app_resources.saveMode & ONEDIR_SAVE)) {
		(void) strcat(dummy, ".ART");
	    }
#endif /* VMS */
	}
	return(utTildeExpand(dummy));
    }

#ifndef VMS
    if ((filename[0] == '/') || (filename[0] == '~')) {
	return(utTildeExpand(filename));
    }
#else
    if ((index(filename, ':')) || (index(filename, '['))
		|| (index(filename, '<'))) {
	return(utTildeExpand(filename));
    }
#endif /* VMS */

    if (app_resources.saveMode & ONEDIR_SAVE) {
#ifndef VMS
	(void) sprintf(dummy, "%s/%s", savedir, filename);
#else /* VMS */
	(void) sprintf(dummy, "%s%s", savedir, filename);
#endif /* VMS */
    } else {
	/* use "saveDir/group/filename" */
#ifndef VMS
	(void) sprintf(dummy, "%s/%s", savedir, group);
#else /* VMS */
	VmsMakeDirName(dummy, savedir, Group);
#endif /* VMS */
	(void) mkdir(utTildeExpand(dummy), 0777);
#ifndef VMS
	(void) strcat(dummy, "/");
#endif /* VMS */
	(void) strcat(dummy, filename);
    }
    return(utTildeExpand(dummy));
}


int
saveRangeOfArticles(filenameTemplate, start, end)
char *filenameTemplate;
art_num start;
art_num end;
/*
 * save the a range of articles from start to end,
 *
 * filenameTemplate is a sprintf compatible string
 *
 *   examples:  "mg.%d", "| save-it"
 *
 *     %d will be filled in with the article number
 *
 *   returns: 1 for OKAY, 0 for FAILURE
 *
 */
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    art_num save_current = newsgroup->current;
    art_num i;
    char buffer[1024];

    for (i = start; i <= end; i++) {
	(void) sprintf(buffer, filenameTemplate, i);
	if (!saveArticle(buffer, newsgroup, i)) {
	    return(0);
	}
    }
    return(1);
}


int
saveArticle(filename, newsgroup, art)
char *filename;
struct newsgroup *newsgroup;
art_num art;
{
    char timeString[BUFFER_SIZE];
    extern char *ctime();
    extern long time();
    long clock;
    long pos;
    char *artfile, *fullName;
    FILE *fpart, *fpsave;
    int rotation;
    char mode[2], string[256];
    int c;
    struct stat buf;

    if ((filename != NIL(char)) && (*filename != '\0')) {
	filename = utTrimSpaces(filename);
    }

    /* get the FULL article */
#ifdef PREFETCH
    if (!invalidatePrefetchedGroup(newsgroup)) {
	mesgPane("The newsgroup has been deleted out from underneath us");
	return(0);
    }
#endif /* PREFETCH */

    rotation = (IS_ROTATED(newsgroup->articles[INDEX(art)]) ? ROTATED : NOT_ROTATED);
    artfile = getarticle(art, &pos, FULL_HEADER, rotation);

#ifndef VMS
    /* see if the file name is actually a command specification */
    if ((filename != NIL(char)) && (filename[0] == '|')) {
	int status;
	(void) sprintf(error_buffer, "Piping article %ld into command `%s'...     ",
		       newsgroup->current, &filename[1]);
	infoNow(error_buffer);
    	status = processArticle(utTrimSpaces(&filename[1]), artfile);
	(void) unlink(artfile);
	FREE(artfile);
	(void) strcpy(&error_buffer[utStrlen(error_buffer) - 4], "done");
	info(error_buffer);
	return(status);
    }
#endif

    /* XXX not quite right, don't want to try to create it if not used... */
    if (!createNewsDir()) {
	(void) unlink(artfile);
	FREE(artfile);
	return(0);
    }
    
    if ((fullName = buildFileName(filename, app_resources.saveDir, newsgroup->name)) == NIL(char)) {
	mesgPane("Cannot figure out file name `%s'", filename);
	(void) unlink(artfile);
	FREE(artfile);
	return(0);
    }

    if ((fpart = fopen(artfile, "r")) == NULL) {
	mesgPane("Cannot open the article temporary file");
	(void) unlink(artfile);
	FREE(artfile);
	return(0);
    }

    if (stat(fullName, &buf) == 0) {
	(void) strcpy(mode, "a");
    } else {
	(void) strcpy(mode, "w");
    }
    
    if ((fpsave = fopen(fullName, mode)) == NULL) {
	if (mode[0] == 'w') {
	    (void) sprintf(error_buffer, "Cannot create file `%s'", fullName);
	} else {
	    (void) sprintf(error_buffer, "Cannot append to file `%s'", fullName);
	}
	(void) fclose(fpart);
	(void) unlink(artfile);
	FREE(artfile);
	mesgPane(error_buffer);
	return(0);
    }

    if (mode[0] == 'w') {
	(void) sprintf(error_buffer, "Saving article %ld in file `%s'...     ",
			   art, fullName);
    } else {
	(void) sprintf(error_buffer, "Appending article %ld to file `%s'...     ",
		       art, fullName);
    }	
     
    infoNow(error_buffer);

    if ((app_resources.saveMode & MAILBOX_SAVE) == MAILBOX_SAVE) {
	(void) time(&clock);
	(void) strcpy(timeString, ctime(&clock));
	timeString[utStrlen(timeString) - 1] = '\0';  /* get rid of the newline */

	while (fgets(string, sizeof(string), fpart) != NULL) {
	    if (STREQN(string, "Path:", 5)) {
		string[utStrlen(string) - 1] = '\0';    /* get rid of the newline */
		(void) fprintf(fpsave, "From %s %s\n", &string[6], timeString);
		break;
	    }
	}
	(void) rewind(fpart);
    }

    (void) fprintf(fpsave, "Article %ld of %s:\n", art, newsgroup->name);

    if ((app_resources.saveMode & MAILBOX_SAVE) == MAILBOX_SAVE) {
	if ((app_resources.saveMode & HEADERS_SAVE) == HEADERS_SAVE) {
	    int found = 0;
	    
	    while (fgets(string, sizeof(string), fpart) != NULL) {
		if (!found && STREQN(string, "From:", 5)) {
		    found = 1;
		    (void) fprintf(fpsave, ">From %s", &string[6]);
		} else {
		    (void) fputs(string, fpsave);
		}
	    }	    
	} else {
	    (void) fseek(fpart, pos, 0);
	    while ((c = getc(fpart)) != EOF) {
		(void) putc((char) c, fpsave);
	    }
	}
    } else {
	if ((app_resources.saveMode & HEADERS_SAVE) == HEADERS_SAVE) {
	    while ((c = getc(fpart)) != EOF) {
		(void) putc((char) c, fpsave);
	    }
	} else {
	    (void) fseek(fpart, pos, 0);
	    while ((c = getc(fpart)) != EOF) {
		(void) putc((char) c, fpsave);
	    }
	}
    }

    (void) fprintf(fpsave, "\n\n");
    (void) fclose(fpart);
    (void) fclose(fpsave);

    (void) unlink(artfile);
    FREE(artfile);

    (void) strcpy(&error_buffer[utStrlen(error_buffer) - 4], "done");
    info(error_buffer);

    SET_SAVED(newsgroup->articles[INDEX(art)]);

    return 1;
}


int
saveCurrentArticle(filename)
char *filename;
/*
 * save the current article in 'filename' (if it begins with '|', send it to
 *   a process)
 *
 *   returns: 1 for OKAY, 0 for FAILURE
 *
 */
{
    struct newsgroup *newsgroup = Newsrc[CurrentGroupNumber];
    return saveArticle(filename, newsgroup, newsgroup->current);
}
