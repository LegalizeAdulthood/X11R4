/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: help.c,v 1.2 89/12/06 21:40:27 xguest Exp $ */
/* $ACIS$ */
/* $Source: /xsrc/contrib/toolkits/andrew/atk/help/src/RCS/help.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: help.c,v 1.2 89/12/06 21:40:27 xguest Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


/*---------------------------------------------------------------------------*/
/*									     */
/*		          	ATK Help Program			     */
/*									     */
/*	History:							     */
/*		original be2 version: Mike Kazar, c. 1985		     */
/*									     */
/*		complete ATK rewrite: Marc Pawliger, 2/89		     */
/*									     */
/*	See README for programmer details				     */
/*									     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*	MODULE: help.c							     */
/*		Subclass of view, includes all major lookup and display      */
/*		routines.						     */
/*---------------------------------------------------------------------------*/

#include <class.h>

#include <andyenv.h>
/* sys/types.h in AIX PS2 defines "struct label", causing a type name clash.
   Avoid this by temporarily redefining "label" to be something else in the preprocessor. */
#define label gezornenplatz
#include <andrewos.h> /* sys/types.h sys/file.h */
#undef label

#include <bind.ih>
#include <complete.ih>
#include <cursor.ih>
#include <dataobj.ih>
#include <environ.ih>
#include <filetype.ih>
#include <frame.ih>
#include <graphic.ih>
#include <im.ih>
#include <keymap.ih>
#include <keystate.ih>
#include <label.ih>
#include <labelv.ih>
#include <lpair.ih>
#include <menulist.ih>
#include <message.ih>
#include <panel.ih>
#include <print.ih>
#include <proctbl.ih>
#include <rect.h>
#include <rofftext.ih>
#include <scroll.ih>
#include <text.ih>
#include <textv.ih>
#include <attribs.h>
#include <view.ih>

#include <errno.h>
#include <ctype.h>
#include <sys/dir.h>
#include <sys/errno.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <index.h>
#include <regexp.h> /* use the reg expression routines in overhead */

#include "config.h"
#include "helpsys.h"
#include "help.h"
#include <helpdb.ih>
#include <help.eh>

/*---------------------------------------------------------------------------*/
/*				GLOBALS					     */
/*---------------------------------------------------------------------------*/

/* the new menu and key states */
static struct keymap *Help_Map;
static struct menulist *Help_Menus;

/* statics representing information cache */
static char tutorialDir[MAXPATHLEN];
static char changesDir[MAXPATHLEN];

/* a list of instances of help */
static struct self_help *ego = (struct self_help *)NULL;

static struct cursor *waitCursor; /* the watch cursor */

static char **panelList = NULL; /* used for enumerating across the help index */
static int panelIndex = 0;

/* hooks to textview and frame procs */
static void (*textSearch)() = (void (*)())NULL;
static void (*textRevSearch)() = (void (*)())NULL;
static void (*textSearchAgain)() = (void (*)())NULL;
static void (*textCopyRegion)() = (void (*)())NULL;
static void (*textPageDown)() = (void (*)())NULL;
static void (*textPageUp)() = (void (*)())NULL;
static void (*frameSetPrinter)() = (void (*)())NULL;

/*---------------------------------------------------------------------------*/
/*			CONDITIONAL DEBUGGING				     */
/*---------------------------------------------------------------------------*/

#ifdef DEBUGGING
/*
 * debugging statements can be included by compiling add modules with
 * -DDEBUGGING.  Debugging output is toggled by existance/nonexistance
 * of the environment variable HELPDEBUG.
 */
int HELPDEBUG = 0;
#undef DEBUG
#define DEBUG(arg) if (HELPDEBUG != 0) { printf arg; fflush(stdout); }
#else
#define DEBUG(arg)
#endif /* DEBUGGING */


/*---------------------------------------------------------------------------*/
/*				UTILITY FUNCTIONS			     */
/*---------------------------------------------------------------------------*/

/*
 * copy protoname into aresult, prepending /usr/andy or whatever,
 * as appropriate
 */
static char *AndyCopy(aproto, aresult)
register char *aproto, *aresult;
{
    register char *tp;

    tp = environ_AndrewDir(aproto);
    strcpy(aresult, tp);
    return aresult;
}

/*
 * allocate new string
 */
static char *CopyString(as)
register char *as;
{
    register char *tp;
    tp = (char *) malloc(strlen(as)+1);
    if (!tp)
	return NULL;
    strcpy(tp, as);
    return tp;
}

/*
 * parse comma terminated field followed by new-line terminated field
 */
static int ScanLine(afile, ae1, ae2)
register FILE *afile;
char *ae1, *ae2;
{
    register int state;		/* 0->reading ae1, 1->reading ae2 */
    register int tc;		/* char we're reading */
    register char *tp;		/* points to string */

    state = 0;
    tp = ae1;
    while(1) {
        tc = getc(afile);
        if (tc < 0 || tc == '\n') {
            /* done */
            if (state == 1) {
                *tp++ = 0;
                return 2;
            } else return 0;
        }
        if (state == 0 && tc == ',') {
            state = 1;
            *tp++ = 0;
            tp = ae2;
            continue;
        }
        *tp++ = tc;
    }
}

/*
 * just like system(3) only closes fds 3..., and doesn't wait
 */
static int mysystem (acmd)
register char *acmd;
{
    register long pid;
    pid = osi_vfork();
    if (pid < 0) return -1;
    else if (pid == 0) {
        /* child, next close window mgr's fd, so that parent window can be killed */
        for(pid = 3; pid < getdtablesize(); pid++) close(pid);
        execl("/bin/sh", "sh", "-c", acmd, NULL);
        _exit(127);
	/*NOTREACHED*/
    }
    else return 0;      /* parent, success */
}


/*
 * lowercases's a string.
 */
static char *LowerCase(astring)
register char *astring;
{
    register char *tp = astring;

    while (tp && *tp != NULL)
	if (isupper(*tp)) {
	    *tp = tolower(*tp);
	    tp++;
	} else
	    tp++;
    return astring;
}


/*
 * maps string(n) to string.n in place
 */
static char *MapParens(s)
char *s;
{
    char *lpp, *rpp;

    if (((lpp = rindex(s, '(')) != NULL) &&  /* has a ( */
	((rpp = rindex(s, ')')) != NULL) &&  /* and a ) */
	(*(rpp+1) == '\0')) {			     /* ) is the last char */
	*lpp = '.';
	*rpp = '\0';
	return s;
    }
}


/*
 * stolen from libcs.  Returns the index of string small in big, 0 otherwise
 */
char *sindex (big,small) char *big,*small;
{
    register char *bp, *bp1, *sp;
    register char c = *small++;

    if (c==0) return(0);
    for (bp=big;  *bp;  bp++)
	if (*bp == c) {
	    for (sp=small,bp1=bp+1;   *sp && *sp == *bp1++;  sp++)
		;
	    if (*sp==0) return(bp);
	}
    return 0;
}


/*---------------------------------------------------------------------------*/
/*				MAIN FUNCTIONS				     */
/*---------------------------------------------------------------------------*/

/*
 * Help's file displayer - given a filename, attempts to find out if
 * it's a roff file (1st char '.' or '#') and read it in using rofftext.
 * Otherwise, assumes it's text or ATK format and lets text_read take care
 * of the dirty work of dealing with that.
 *
 * Can display any file that uses a textview or a subclass of textview.
 * Creates a new view and dataobject for the file and reads the file into the
 * new object.
 *
 * Also takes care of adding elements to the history panel for the last-shown
 * help file.
 */
static int ShowFile(self, afilename, amoreFlag, hist)
register struct help *self;
register char *afilename;	/* the file */
int amoreFlag;			/* put "(more)" in the titlebar?
				   TRUE - yes, FALSE - no,
				   help_USE_OLD_TITLE - use last setting */
int hist;			/* add this previous file to the history?
				   help_HIST_NOADD - none at all,
				   help_HIST_NAME - aname,
				   help_HIST_TAIL - tail of the found filename
				   */
{
    register FILE *fd;
    long objectID;
    static char *manfiles[] = MACFILES;
    char **manptr = manfiles;
    char tbuffer[MAXPATHLEN];
    char isTroffFormat = 0;
    char tc;
    static int lastmore;

    struct attributes attrs, *attr;

    char *viewName, *objName, *tmp;

    struct cache *c = self->info; /* view and dataobject to replace with new file object */

    struct view *newview;
    struct dataobject *newdata;
    struct view *oldview = c->view;
    struct dataobject *olddata = c->data;

    if (message_Asking(ego->this->info->view)) {
	DEBUG(("retracting\n"));
	message_CancelQuestion(ego->this->info->view);
    }

    /* Check if file exists */
    if (!(fd = fopen(afilename, "r"))) {
	char buf[HELP_MAX_ERR_LENGTH *2];
	
        if (access(afilename, 4) < 0) {
            if (errno == ETIMEDOUT) {
		sprintf(buf, "%s %s", err_server, err_read);
		ERRORBOX(c->view, buf);
            } else
		ERRORBOX(c->view, err_missing);
        } else ERRORBOX(c->view, err_read);
	return 0;
    }

    /* is it troff ? */
    tc = getc(fd);
    ungetc(tc, fd);
    isTroffFormat = (tc == '.' || tc == '#');

    /* what kind of object does this file want ? (don't touch attributes) */
    objName = filetype_Lookup(NULL, afilename, NULL, NULL);
    if ((newdata = (struct dataobject *) class_NewObject(objName)) ==
	(struct dataobject *)NULL) {
	sprintf(tbuffer, err_dobj, objName);
	ERRORBOX(c->view, tbuffer);
	return 0;
    }

    /* what kind of view do we need ? */
    viewName = dataobject_ViewName(newdata);
    if ((newview = (struct view *)class_NewObject(viewName)) ==
	(struct view *)NULL) {
	sprintf(tbuffer, err_view, viewName);
	ERRORBOX(c->view, tbuffer);
	dataobject_Destroy(newdata);
	return 0;
    }

    /* is it a subclass of textview? */
    if (!class_IsTypeByName(class_GetTypeName(newview), "textview")) {
	/* not text, punt */
	sprintf(tbuffer, err_nontext, viewName);
	ERRORBOX(c->view, tbuffer);
	view_Destroy(newview);
	dataobject_Destroy(newdata);
	return 0;
    }

    /* hook 'em up */
    view_SetDataObject(newview, newdata);

    /* since now committed, take care of the history for previous item, if we
       haven't already done so, and we should */

    if (c->histat != help_HIST_NOADD) {

	DEBUG(("hist... getting hist on %d\n", (int)c->view));
	
	/* save position of last view's top, dot and dotlen */
	
	AddHistoryItem(self, help_HE_HISTORY, help_IGNORE_TOGGLE);
	
	/* only add this entry once */
	c->histat = help_HIST_NOADD;
    }

    attrs.next = (struct attributes *)NULL;
    attrs.key = "readonly";
    attrs.value.integer = 0;
    text_SetAttributes((struct text *)newdata, &attrs);
    fseek(fd, 0 ,0);	/* somebody moved the pointer */
    attr = NULL;
    filetype_Lookup(fd, afilename, &objectID, attr);
    if (attr != NULL)
	text_SetAttributes((struct text *)newdata, attr);
    if (!isTroffFormat) {
	if (text_Read((struct text *)newdata, fd, objectID) != dataobject_NOREADERROR) {
	    ERRORBOX(c->view, err_read);
	    view_Destroy(newview);
	    dataobject_Destroy(newdata);
	    return 0;
	}
    } else {			/* it's troff */
	/* check if the manfiles are absolute or relative paths.
	   If 1st char is '/', path is absolute, OW, we andycopy it
	   into a temp buffer, since it will be relative to ANDREWDIR */
	while (*manptr) {
	    if (**manptr != '/') {
		tmp = (char *)malloc(strlen(*manptr)+1);
		tmp[0] = '/';
		strcat(tmp, *manptr);
		*manptr = AndyCopy(tmp, (char *)malloc(MAXPATHLEN));
		free(tmp);
	    }
	    manptr++;
	}
	message_DisplayString(c->view, 0, msg_converting);
	im_ForceUpdate();
	if (rofftext_ReadRoffIntoText((struct text *)newdata, fd, 0, manfiles) != dataobject_NOREADERROR) {
	    ERRORBOX(c->view, err_read);
	    view_Destroy(newview);
	    dataobject_Destroy(newdata);
	    return 0;
	}
	message_DisplayString(c->view, 0, "");
    }
    textview_SetTopPosition((struct textview *)newview, 0);
    textview_SetDotPosition((struct textview *)newview, 0);
    textview_SetDotLength((struct textview *)newview, 0);
    attrs.value.integer = 1;
    text_SetAttributes((struct text *)newdata, &attrs);

    /* now swap the old with the new */
    DEBUG(("swapping..."));
    view_UnlinkTree(c->view);
    c->view = newview;
    c->data = newdata;

    /* now set up the history state */

    c->histat = hist;

    switch (hist) {
      case help_HIST_NAME:
	strcpy(c->histent, c->name);
	break;
      case help_HIST_NOADD:	/* do this so that bookmarks will work correctly */
      case help_HIST_TAIL:
	tmp = rindex(afilename,'/');
	strcpy(c->histent, (tmp != NULL) ? tmp+1 : afilename);
    }
    DEBUG(("setting histent to: %s\n", c->histent));

    /* now hook the new view up to the scrollbar */
    scroll_SetView(c->scroll, c->view); /* does a linktree */
    textview_SetEmbeddedBorder((struct textview *) c->view, 20, 5);
    dataobject_NotifyObservers(c->data, (long)0);
    scroll_WantUpdate(c->scroll, c->scroll);

    /* get rid of the old view and dataobject*/
    DEBUG(("destroying... %d ",(int)oldview));
    view_Destroy(oldview);
    dataobject_Destroy(olddata);

    strcpy(tbuffer, afilename);

    DEBUG(("titling\n"));

    if ((amoreFlag == TRUE) || ((amoreFlag == help_USE_OLD_TITLE) && lastmore == TRUE))
	strcat(tbuffer, " (more)");
    if (lastmore != help_USE_OLD_TITLE)
	lastmore = amoreFlag;
    im_SetTitle(view_GetIM((struct view *)c->view), tbuffer);
    
    super_WantInputFocus(c->view, c->view);
    
    fclose(fd);

    DEBUG(("showfile returning\n"));
    return 1;
    /* phew! */
}


/*
 * Main function for getting help on a string
 *
 * Returns:
 *	-1: if a server was down while trying to get help on this topic
 *	 0: if no help found for this topic
 *	 1: if successful

    Pseudo-code:
{
    if (index not open)
	return error;
    lowercase(topic);
    map topic(n) -> topic.n;
    if (first time for this topic)
	code = setuphelp for this topic;
    if (code != error)
	setupmenus if this topic has tutorial or changes files;
    if (empty file list)
	out of files, return error;
    else if (code != error) {
	show current file;
	check if we should erase the "show more files" menu item;
    }
    if (code == error)
	show error message;
    else
	advance current file pointer to next file;
    return code;
}
*/
static int GetHelpOn(self, aname, isnew, ahistory, errmsg)
register struct help *self;
char *aname;	/* what topic */
long isnew;	/* is this a new topic? */
int ahistory;	/* show in history log under what name?
		   help_HIST_NOADD - none at all,
		   help_HIST_NAME - aname,
		   help_HIST_TAIL - tail of the found filename
		   */
char *errmsg;	/* error to print if failure. "Error" if this is NULL */
{
    register struct cache *c = self->info;
    char tbuffer[MAXPATHLEN];
    char helpBuffer[HNSIZE + 200];
    int code = 1;

    DEBUG(("%s get help: setup(%s)\n",(isnew) ? "new" : "old", aname));

    if (!helpdb_CheckIndex(c->view))
	return -1;

    LowerCase(aname);
    MapParens(aname);

     if (isnew) {		/* if the first time through, setup c->all */
  	code = helpdb_SetupHelp(c, aname, TRUE);

#ifdef DEBUGGING
	if (code != 0) {
	    DEBUG(("set found something\n"));
	} else {
	    DEBUG(("code to 0, tmplist == NULL\n"));
	}
#endif /*DEBUGGING*/

	if (code != 0) {
	    /* initially turn off changes and tutorial menus */
	    c->flags &= ~(MENU_SwitchChangesMenu | MENU_SwitchTutorialMenu | MENU_SwitchMoreMenu);
	
	    /* should we display "show tutorial" or "show changes" menus? */
	    sprintf(tbuffer, "%s/%s%s", tutorialDir, c->name, TUTORIAL_EXT);
	    if (access(tbuffer, 0) == 0 && !(c->flags & MENU_SwitchTutorialMenu))
		c->flags |= MENU_SwitchTutorialMenu;

	    sprintf(tbuffer, "%s/%s%s", changesDir, c->name, CHANGE_EXT);
	    if (access(tbuffer, 0) == 0 && !(c->flags & MENU_SwitchChangesMenu))
		c->flags |= MENU_SwitchChangesMenu;
	}
    }

    im_SetProcessCursor(waitCursor);

    if (c->cur == (struct helpFile *)NULL)	{ /* no more entries */
	DEBUG(("code to 0, curlist == NULL\n"));
	code = 0;
    } else if (code != 0) {
	DEBUG(("showfiling\n"));
	code = ShowFile(self, c->cur->fileName,
			(c->cur->next != (struct helpFile *)NULL),
			ahistory);

	DEBUG(("helpName: %s\n",c->name));

	/* see if we should display "show more" menu item */
	if (c->cur && c->cur->next)
	    c->flags |= MENU_SwitchMoreMenu; /* turn it on */
	else
	    c->flags &= ~MENU_SwitchMoreMenu; /* turn it off */

	/* now set up the menus */
	SetupMenus(c);
    }

    im_SetProcessCursor((struct cursor *)NULL);
    
    if (code == 0) {		/* error */
	DEBUG(("ERROR\n"));
        sprintf(helpBuffer, (errmsg == NULL) ? err_generic : errmsg);
	ERRORBOX(c->view, helpBuffer);
	DEBUG(("want focus\n"));
	super_WantInputFocus(c->view, c->view);
    } else {			/* no error, advance to next file on list */
	c->cur = c->cur->next;
    }
    DEBUG(("Help on returning %d\n",code));
    return code;
}


/*---------------------------------------------------------------------------*/
/*			HELP OBJECT ULTILITY FUNCTIONS			     */
/*---------------------------------------------------------------------------*/

/*
 * Allows help to just delete one window
 */
static void ExitProc(self)
register struct help *self;
{
    struct self_help *t, *p;

    DEBUG(("IN exit\n"));

    DEBUG(("ego: %d self: %d\n",(int)ego, (int)self));
    for (t=ego;t;t=t->next) {
	DEBUG(("t: %d this: %d next: %d\n", (int)t, (int)t->this, (int)t->next));
    }

    /* delete self from the instance list */
    p = NULL;
    t = ego;
    while (t) {
	if (t->this == self) {
	    if (p == NULL) {	/* deleting head */
		ego = t->next;
		free(t);
		t = ego;
	    } else {
		p->next = t->next;
		free(t);
		t = p->next;
	    }
	} else {
	    p = t;
	    t = t->next;
	}
    }
    DEBUG(("ego: %d self: %d\n",(int)ego, (int)self));
    for (t=ego;t;t=t->next) {
	DEBUG(("t: %d this: %d next: %d\n", (int)t, (int)t->this, (int)t->next));
    }

    /* take care of the cases with 0 and 1 instances left in the list */
    if (ego) {			/* something in the list */
	if (ego->next == (struct self_help *)NULL) { /* only one left */
	    /* remove "Delete this window" menu item */
	    DEBUG(("one left, Delete delete\n"));
	    ego->this->info->flags &= ~MENU_SwitchDeleteMenu;
	    SetupMenus(ego->this->info);
	}
    } else {			/* nobody left, all gone, we be outta here */
	im_KeyboardExit();
    }

    help_FinalizeObject(self);

    DEBUG(("OUT exit\n"));
}


/*
 * random key hit proc to chastise the user
 */
static void nono(self)
register struct help *self;
{
    message_DisplayString(self, 0, err_readonly);
}


/*
 * setup the menumask based on internal flags
 */
static void SetupMenus(c)
register struct cache *c;
{
    DEBUG(("IN setupmenus.."));
    menulist_SetMask(c->menus, c->flags);
    view_PostMenus(c->view, c->menus);
    DEBUG(("OUT setupmenus\n"));
}


/*
 * Do a search in the overview panel
 */
static void SearchOverviews(self)
register struct help* self;
{
    if (!self->showPanels)
	TogglePanels(self, help_ALWAYS_TOGGLE);
    if (!self->showOverview)
	ToggleOverviews(self, help_ALWAYS_TOGGLE);
    (*textSearch) ((struct textview *)self->overviewPanel);
}


/*
 * Do a search in the programs panel
 */
static void SearchPrograms(self)
register struct help* self;
{
    if (!self->showPanels)
	TogglePanels(self, help_ALWAYS_TOGGLE);
    DEBUG(("panels\n"));
    if (!self->showList)
	TogglePrograms(self, help_ALWAYS_TOGGLE);
    DEBUG(("search\n"));
    (*textSearch) ((struct textview *)self->listPanel);
}
    

/*
 * because textview's and frame's procs are bound at runtime, we must call them
 * like this, rather than just binding in a call to proctable_GetProc(...)
 */

static void TextviewProc(self, rock)
register struct help* self;
long rock;
{
    switch (rock) {
      case help_SEARCH:
	(*textSearch) ((struct textview *)self->info->view);
	break;
      case help_RSEARCH:
	(*textRevSearch) ((struct textview *)self->info->view);
	break;
      case help_SEARCH_AGAIN:
	(*textSearchAgain) ((struct textview *)self->info->view);
	break;
      case help_COPY_REGION:
	(*textCopyRegion) ((struct textview *)self->info->view);
	break;
      case help_PAGE_DOWN:
	(*textPageDown) ((struct textview *)self->info->view);
	break;
      case help_PAGE_UP:
	(*textPageUp) ((struct textview *)self->info->view);
	break;
      case help_SET_PRINTER:
	if (frameSetPrinter)
	    (*frameSetPrinter) (self);
	else
	    ERRORBOX(self->info->view, err_noproc);
    }
}

/*
 * print a help file
 */
static void Print(self)
register struct help *self;
{
    message_DisplayString(self, 0, msg_print_queue);
    im_ForceUpdate();
    im_SetProcessCursor(waitCursor);
    print_ProcessView((struct textview *)self->info->view, 1, 0, "", "");
    message_DisplayString(self, 0, msg_queue_done);
    im_SetProcessCursor((struct cursor *) NULL);
}

/*
 * send gripes/kudos to the help maintainers
 */
static void SendComments(self)
register struct help *self;
{
    char cmd[MAXPATHLEN], *prof;
    
    message_DisplayString(self, 0, msg_comment);
    im_ForceUpdate();
    im_SetProcessCursor(waitCursor);
    prof = environ_GetConfiguration(SETUP_GRIPE_ADDRESS);
    if (prof == NULL)
	prof = DEFAULT_GRIPE_ADDRESS;
    sprintf(cmd, "%s %s", COMMENTCMD, prof);
    mysystem(cmd);
    im_SetProcessCursor((struct cursor *) NULL);
}

/*
 * show a tutorial, if it exists
 */
static void ShowTutorial(self)
register struct help *self;
{
    static char tbuffer[MAXPATHLEN];

    sprintf(tbuffer, "%s/%s%s", tutorialDir, self->info->name, TUTORIAL_EXT);
    self->info->flags &= ~MENU_SwitchTutorialMenu; /* turn off menu item */
    SetupMenus(self->info);
    ShowFile(self, tbuffer, help_USE_OLD_TITLE, help_HIST_TAIL);
}

/*
 * show changes doc
 */
static void ShowChanges(self)
register struct help *self;
{
    static char tbuffer[MAXPATHLEN];
    
    sprintf(tbuffer, "%s/%s%s", changesDir, self->info->name, CHANGE_EXT);
    self->info->flags &= ~MENU_SwitchChangesMenu; /* turn off menu item */
    SetupMenus(self->info);
    ShowFile(self, tbuffer, help_USE_OLD_TITLE, help_HIST_TAIL);
}

/*
 * show next file in list
 */
static void NextHelp (self)
register struct help *self;
{
    if (GetHelpOn(self, self->info->name, help_OLD, help_HIST_TAIL, err_no_more) != 0)
	panel_ClearSelection(self->historyPanel);
}


/*
 * Add a history item for a given file with given dot, dotlen and top
 */
static void AddBookmark(self)
register struct help *self;
{
    AddHistoryItem(self, help_HE_BOOKMARK, help_SHOW_HIST);
}


/*
 * add an item to the history buffer
 */
static void AddHistoryItem (self, marcp, flash)
register struct help *self;
int marcp;			/* is this a bookmark? */
int flash;			/* should we expose the history panel? */
{
    struct history_entry *ent;
    register struct cache *c = self->info;

    DEBUG(("addhist\n"));
    if (c->histent == NULL || c->histent[0] == '\0')
	return;

    ent = (struct history_entry *) malloc (sizeof(struct history_entry));
    if (ent == NULL)
	return;

    ent->pos = textview_GetDotPosition((struct textview *)c->view),
    ent->top = textview_GetTopPosition((struct textview *)c->view),
    ent->sellen = textview_GetDotLength((struct textview *)c->view),
    ent->fname = (char *) malloc (strlen(c->histent) + 1);
    strcpy(ent->fname, c->histent);

    if (marcp == help_HE_HISTORY) { /* normal history add */
	
	/* now add the item */
	c->lastHist = panel_Add(c->hist, c->histent, ent, TRUE);
    } else {			/* a bookmark */
	char tfname[HNSIZE + 11];

	sprintf(tfname, "%s @ %d", c->histent, ent->top);
	/* now add the item */
	c->lastHist = panel_Add(c->hist, tfname, ent, TRUE);
    }

    if (flash == help_SHOW_HIST)
	ToggleHistory(self, help_SHOW_HIST);

    DEBUG(("OUT addhist\n"));
}

/*
 * get help on a clicked-on history item
 */
static void HistoryHelp (self, ent, apanel)
struct help *self;		/* callback rock */
struct history_entry *ent;	/* panelEntry rock */
struct panel *apanel;		/* appropriate panel */
{
    char buf[HNSIZE + HELP_MAX_ERR_LENGTH];
    char fnbuf[MAXPATHLEN];
    char *tmp, *dir;
    int code;
    int file = FALSE;

    DEBUG(("histhelp: %s\n", ent->fname));
    im_SetProcessCursor(waitCursor);
    sprintf(buf, err_unexpected, ent->fname);

    /* if it's a change or tutorial file, showfile it, since most of the time,
       the CHANGEDIR and TUTORIALDIR aren't indexed */
    tmp = rindex(ent->fname, '.');
    if (tmp) {
	if (!strcmp(tmp, CHANGE_EXT)) {
	    file = TRUE;
	    dir = changesDir;
	}
	if (!strcmp(tmp, TUTORIAL_EXT)) {
	    file = TRUE;
	    dir = tutorialDir;
	}
    }

    if (file) {
	strcpy(fnbuf, dir);
	strcat(fnbuf, "/");
	strcat(fnbuf, ent->fname);
	DEBUG(("history file: %s\n",fnbuf));
	code = ShowFile(self, fnbuf, FALSE, help_HIST_NOADD);
    } else
	code = GetHelpOn(self, ent->fname, help_NEW, help_HIST_NOADD, buf);

    if (code == 0) {
	DEBUG(("history help = 0\n"));
	/* error, so restore the old selection */
	if (self->info->lastHist != NULL) {
	    panel_MakeSelection(self->historyPanel, self->info->lastHist);
	}
    } else {
	panel_ClearSelection(self->overviewPanel);
	panel_ClearSelection(self->listPanel);
	textview_SetDotPosition((struct textview *)self->info->view, ent->pos);
	textview_SetDotLength((struct textview *)self->info->view, ent->sellen);
	textview_SetTopPosition((struct textview *)self->info->view, ent->top);
    }
    im_SetProcessCursor((struct cursor *)NULL);
    self->info->lastHist = panel_CurrentHighlight(self->historyPanel);
}

/*
 * show overview or a help file from the program list panel
 */
static void OverviewHelp (self, name, apanel)
struct panel *apanel;
register char *name;		/* which topic to request - panelEntry rock */
register struct help *self;
{
    char buf[HNSIZE + HELP_MAX_ERR_LENGTH];
    
    sprintf(buf, err_sorry, name);
    if (GetHelpOn(self, name, help_NEW, help_HIST_NAME, buf) != 0) {
	panel_ClearSelection(self->historyPanel);
	/* clear the selection in the other panel */
	if (apanel == self->overviewPanel) {
	    panel_ClearSelection(self->listPanel);
	} else {		/* it was self->listPanel */
	    panel_ClearSelection(self->overviewPanel);
	}
    }
}


/*
 * get help on a prompted-for topic or a selected word, bringing up a
 * new window if necessary
 */
static void NewHelp(self, type)
register struct help *self;
long type;		/* help_ON 			if Help On... */
			/* help_ON & help_NEW_WIN	if New Help On... */
			/* help_SEL			if Help On Selected */
			/* help_SEL & help_NEW_WIN	if New Help On Selected */
{
    register int i, pos, len, code;
    char tc;
    char helpName[HNSIZE];
    char buf[HNSIZE + HELP_MAX_ERR_LENGTH];
    struct help *hv;

    if (type & help_SEL) {
	pos = textview_GetDotPosition((struct textview *)self->info->view);
	len = textview_GetDotLength((struct textview *)self->info->view);

	/*  only shows menu item if a selection has been made */
	/*  but, since can bind the procedure, must handle this */
	/*  case anyway. */
	if (len == 0) {
	    ERRORBOX(self->info->view, err_no_sel);
	    return;
	} else {
	    if (len >= HNSIZE) {
		ERRORBOX(self->info->view, err_sel_too_long);
		return;
	    }
	}
	i = 0;			/* position in text object */
	code = 0;               /* position in helpName */
	while(i<len) {
	    tc = text_GetChar((struct text *)self->info->data, i+pos);
	    i++;
	    if (tc == ' ' || tc == '\t' || tc == '\n') continue;
	    helpName[code++] = tc;
	}
	helpName[code++] = '\0';
    } else {			/* prompt for topic */
	code = message_AskForString(self, 0, msg_ask_prompt,
				    0, helpName, HNSIZE);
	if ((code < 0) || (helpName[0] == '\0')) return;
    }

    if (type & help_NEW_WIN) {
	struct im *im;
	struct frame *frame;

	im = im_Create(NULL);
	frame = frame_New();
	hv = help_New();
	if (!hv || !im || !frame) {
	    ERRORBOX(self->info->view, err_no_new_view);
	    return;
	}

	/* since we exits, ego must exist now, too */
	if (ego->next != (struct self_help *)NULL) { /* > 1 instances */
	    struct self_help *tmp;

	    for (tmp = ego; tmp; tmp = tmp->next) {
		/* add "delete this window" menu item" */
		DEBUG(("Add delete\n"));
		tmp->this->info->flags |= MENU_SwitchDeleteMenu;
		SetupMenus(tmp->this->info);
	    }
	}
	
	/* frame for frame_SetView must have associated im */
	frame_SetView(frame, help_GetApplicationLayer(hv));
	im_SetView(im, frame);

	/* add in a message handler */
	frame_PostDefaultHandler(frame, "message",
				 frame_WantHandler(frame, "message"));

    }
    
    sprintf(buf, err_sorry, helpName);
    if (GetHelpOn((!(type & help_NEW_WIN)) ? self : hv
		  , helpName, help_NEW, help_HIST_NAME, buf) != 0) {
	panel_ClearSelection(self->overviewPanel);
	panel_ClearSelection(self->listPanel);
	panel_ClearSelection(self->historyPanel);
    }
}


/*
 * Adds a search directory to the searchpath
 */
static void AddSearchDir(self)
struct help *self;
{
    char buf[MAXPATHLEN];
    char buf2[MAXPATHLEN+100];
    int code;

    sprintf(buf2, "%s", "Added directory: ");
    code = completion_GetFilename((struct view *)self, msg_dir_prompt, NULL,
				  buf, sizeof(buf), TRUE, TRUE);
    if (code != -1) {
	buf[strlen(buf)-1] = '\0'; /* remove trailing '/' */
	helpdb_AddSearchDir(buf);
    strcat(buf2, buf);
    message_DisplayString(self, 0, buf2);
    }
}


/*---------------------------------------------------------------------------*/
/*			     PANEL FUNCTIONS				     */
/*---------------------------------------------------------------------------*/

/*
 * setup panels and default entries.
	This function had an ifdef to map it between cmu and non-cmu.
	But the non-cm,u functionality is needed for overviews, so
	I converted it to a runtime choice.   -- wjh

	readpairs:
		fname is the name of a file containing pair lines:
			panel-entry,help-key
		def is NULL, but could be an array of keys
	non-readpairs:
		fname is the name of a  directory of help files
		def is a list of extensions of files to be used in the panel
 
	returns the number of entries added to the panel
 */
	static long
SetupPanel(readpairs, fname, panel, def)
	boolean readpairs;
	char *fname;
	struct panel *panel;		/* the panel to add entries to */
	char **def;
{
	char **defptr;
	register FILE *tfile;
	char elt1[HNSIZE], elt2[HNSIZE];
	register long code;
	char *tmp;
	DIR *helpdir;
	struct direct *ent;
	char tf[MAXPATHLEN];
	char *tfp;
	long entriesadded = 0;

	if (readpairs) {
		tfile = fopen(fname, "r");		/* try opening file */
		if (tfile) {
			while (1) {
				code = ScanLine(tfile, elt1, elt2);
				if (code != 2) break;
				if (elt1[0] != '#') {
					panel_Add(panel, elt1, CopyString(elt2), FALSE);
					entriesadded++;
				}
			}
			fclose(tfile);
		}
		else {			/* if that don't woik, use da backup */
			defptr = def;
			while (defptr && *defptr != (char *) NULL) {
				panel_Add(panel, *defptr++, *defptr++, FALSE);
				entriesadded++;
			}
		}
	}
	else if ((helpdir = opendir(fname)) == (DIR *)NULL)
		ERRORBOX(panel, err_openhelpdir);
	else {
		if (!panelList)
			panelList = (char **)malloc(sizeof(char *) * help_MAXPANEL);

		panelIndex = 0;
		
		strcpy(tf, fname); /* make a base for filenames dir/dir/ */
		tfp = tf + strlen(tf);
		*tfp++ = '/';	/* tfp points just after the last '/' */

		while((ent = readdir(helpdir)) != (struct direct *)NULL) {
			strcpy(tfp, ent->d_name); /* finish the filename */
			defptr = def;
			tmp = rindex(ent->d_name, '.');
			if (tmp != NULL) {
				while (defptr && *defptr != (char *) NULL) {
					if (strcmp(tmp, *defptr++) == 0) {
						*tmp = '\0'; /* chop off extension */
						/* found a good candidate filename */
						AddToPanelList(ent->d_name);
						*tmp = '.';   /* replace period */
						entriesadded++;
					}
				}
			}
		}
		closedir(helpdir);

		/* now make the list */
		SortAndMakePanel(panel);
	}
	return entriesadded;
}


/*
 * Setup the lpairs for the side panel(s)
 */
static struct view *SetupLpairs(self)
register struct help *self;
{
    long which = 0;

    lpair_UnlinkTree(self->panelLpair1);
    lpair_UnlinkTree(self->panelLpair2);

    if (self->showOverview) which |= help_SHOW_OVER;
    if (self->showList) which |= help_SHOW_LIST;
    if (self->showHistory) which |= help_SHOW_HIST;

    DEBUG(("lpair which: %d\n",which));

    switch (which) {
        case 0:
	    return (struct view *)NULL;

	case help_SHOW_OVER:
	    return (struct view *)self->overviewLpair;

	case help_SHOW_LIST:
	    return (struct view *)self->listLpair;

	case help_SHOW_HIST:
	    return (struct view *)self->historyLpair;

	case help_SHOW_OVER | help_SHOW_LIST:
	    lpair_SetNth(self->panelLpair1, 0, self->overviewLpair);
	    lpair_SetNth(self->panelLpair1, 1, self->listLpair);
	    self->panelLpair1->needsfull = 3;
	    return (struct view *)self->panelLpair1;

	case help_SHOW_OVER | help_SHOW_HIST:
	    lpair_SetNth(self->panelLpair1, 0, self->overviewLpair);
	    lpair_SetNth(self->panelLpair1, 1, self->historyLpair);
	    self->panelLpair1->needsfull = 3;
	    return (struct view *)self->panelLpair1;

	case help_SHOW_LIST | help_SHOW_HIST:
	    lpair_SetNth(self->panelLpair1, 0, self->listLpair);
	    lpair_SetNth(self->panelLpair1, 1, self->historyLpair);
	    self->panelLpair1->needsfull = 3;
	    return (struct view *)self->panelLpair1;

	case help_SHOW_OVER | help_SHOW_LIST | help_SHOW_HIST:
	    lpair_SetNth(self->panelLpair1, 0, self->overviewLpair);
	    lpair_SetNth(self->panelLpair1, 1, self->listLpair);
	    self->panelLpair1->needsfull = 3;

	    lpair_SetNth(self->panelLpair2, 0, self->panelLpair1);
	    lpair_SetNth(self->panelLpair2, 1, self->historyLpair);
	    self->panelLpair2->needsfull = 2;
	    return (struct view *)self->panelLpair2;
	}
}


/*
 * turn side panels on and off
 */
static void TogglePanels(self, rock)
register struct help *self;
long rock;
{
    if ((self->showPanels && (rock == help_SHOW_PANEL)) ||
	(!self->showPanels && (rock == help_HIDE_PANEL)))
	return;
    /* could fall through here if rock is ALWAYS_TOGGLE */
    self->showPanels = 1 - self->showPanels;    	/* toggle it */
    self->info->flags ^= (MENU_TogglePanelShow | MENU_TogglePanelHide); /* toggle menus */

    if (self->showPanels) {
	scroll_UnlinkTree(self->info->scroll);
	lpair_LinkTree(self->mainLpair, self);
    } else {
	lpair_UnlinkTree(self->mainLpair);
	scroll_LinkTree(self->info->scroll, self);
    }
    SetupMenus(self->info);
    /* here we want to update everything */
    help_Update(self);
}


/*
 * toggle overview panel on and off
 */
static void ToggleOverviews(self, rock)
register struct help *self;
long rock;
{
    struct view *v;
    
    if ((self->showOverview && (rock == help_SHOW_OVER)) ||
	(!self->showOverview && (rock == help_HIDE_OVER)))
	return;
    /* could fall through here if rock is ALWAYS_TOGGLE */

    self->showOverview = 1 - self->showOverview; 		/* toggle */
    v = SetupLpairs(self);

    if (v) {
	self->info->flags ^= (MENU_ToggleOverHide | MENU_ToggleOverShow); /* toggle menus */
	if (!self->showPanels)
	    TogglePanels(self, help_ALWAYS_TOGGLE); /* turn them on */
	lpair_SetNth(self->mainLpair, 1, v);
	self->mainLpair->needsfull = 2; 		/* -- hack to get redraw to work */
	super_WantInputFocus(self, self->info->view);
	SetupMenus(self->info);
    } else {
	self->showOverview = 1 - self->showOverview; 		/* toggle */
	if (self->showPanels)
	    TogglePanels(self, help_ALWAYS_TOGGLE); /* turn them off */
    }
}


/*
 * toggle program list panel on and off
 */
static void TogglePrograms(self, rock)
register struct help *self;
long rock;
{
    struct view *v;
    
    if ((self->showList && (rock == help_SHOW_LIST)) ||
	(!self->showList && (rock == help_HIDE_LIST)))
	return;
    /* could fall through here if rock is ALWAYS_TOGGLE */

    self->showList = 1 - self->showList; 		/* toggle */
    v = SetupLpairs(self);

    if (v) {
	self->info->flags ^= (MENU_ToggleListHide | MENU_ToggleListShow); /* toggle menus */
	if (!self->showPanels)
	    TogglePanels(self, help_ALWAYS_TOGGLE); /* turn them on */
	lpair_SetNth(self->mainLpair, 1, v);
	self->mainLpair->needsfull = 2; 		/* -- hack to get redraw to work */
	super_WantInputFocus(self, self->info->view);
	SetupMenus(self->info);
    } else {
	self->showList = 1 - self->showList; 		/* toggle */
	if (self->showPanels)
	    TogglePanels(self, help_ALWAYS_TOGGLE); /* turn them off */
    }
}



/*
 * toggle history panel on and off
 */
static void ToggleHistory(self, rock)
register struct help *self;
long rock;
{
    struct view *v;
    
    if ((self->showHistory && (rock == help_SHOW_HIST)) ||
	(!self->showHistory && (rock == help_HIDE_HIST)))
	return;
    /* could fall through here if rock is ALWAYS_TOGGLE */

    self->showHistory = 1 - self->showHistory; 		/* toggle */
    v = SetupLpairs(self);

    if (v) {
	self->info->flags ^= (MENU_ToggleHistHide | MENU_ToggleHistShow); /* toggle menus */
	if (!self->showPanels)
	    TogglePanels(self, help_ALWAYS_TOGGLE); /* turn them on */
	lpair_SetNth(self->mainLpair, 1, v);
	self->mainLpair->needsfull = 2; 		/* -- hack to get redraw to work */
	super_WantInputFocus(self, self->info->view);
	SetupMenus(self->info);
    } else {
	self->info->flags ^= (MENU_ToggleHistHide | MENU_ToggleHistShow); /* toggle menus */
	self->showHistory = 1 - self->showHistory; 		/* toggle */
	if (self->showPanels)
	    TogglePanels(self, help_ALWAYS_TOGGLE); /* turn them off */
 	SetupMenus(self->info);
   }
}


/*
 * Used to add all files in a directory to the expanded program list
 */
static void ExpanderAux(dname)
char *dname;
{
    DIR *tmpdir;
    struct direct *tde;
    char tf[MAXPATHLEN], *tfp, *tfp2;
    
    tmpdir = opendir(dname);
    
    if (tmpdir == (DIR *)NULL)
	return; /* don't use unopened directory */
    strcpy(tf, dname); /* make a base for filenames dir/dir/ */
    tfp = tf + strlen(tf);
    *tfp++ = '/';	/* tfp points just after the last '/' */
    
    while((tde=readdir(tmpdir)) != NULL) {
	if (*tde->d_name != '.') { /* no dot files */
	    strcpy(tfp, tde->d_name); /* finish the filename */
	    tfp2 = rindex(tde->d_name, '.');
	    if (tfp2 && !isdigit(*(tfp2+1)))
		*tfp2 = '\0';	/* chop off non-numeric extension */
	    AddToPanelList(tde->d_name);
	}
    }
    closedir(tmpdir);
}


/*
 * comparison function for qsort
 */
static int panelCompare(s1, s2)
char **s1, **s2;
{
    return (strcmp(*s1, *s2));
}


/*
 * terminates and then sorts the panelList
 */
static void SortAndMakePanel(p)
struct panel *p;
{
    char **ts;

    DEBUG(("sort and make..."));
    panelList[panelIndex] = NULL;
    DEBUG(("sort..."));
    if(panelIndex > 1) qsort(panelList, panelIndex, sizeof(char *), panelCompare);
    DEBUG(("removing..."));
    if(panelIndex > 0){	
	panel_RemoveAll(p);
	DEBUG(("adding..."));
	for (ts=panelList; *ts; ts++) {
	    panel_Add(p, *ts, *ts, FALSE);
	}
    }
    DEBUG(("done\n"));
}


/*
 * just adds a string to the global panelList
 */
static char *AddToPanelList(s)
char *s;
{
    if(!panelList)
	panelList = (char **)malloc(sizeof(char *) * help_MAXPANEL);
    if(!panelList)
	return((char *) NULL);
    panelList[panelIndex] = (char *)malloc(strlen(s) + 1);
    if (!panelList[panelIndex])
	return NULL;
    strcpy(panelList[panelIndex], s);
    panelIndex++;
    return panelList[(panelIndex - 1)];
}    


/*
 * Index library callback helper for 'expand the program list'
 */
static void Expander(aindex, ac, self)
struct Index *aindex;
struct indexComponent *ac;
struct help *self;
{
    AddToPanelList(ac->name);
}


/*
 * toggle programs list size.  Like da name dun say.
 */
static void ToggleProgramListSize(self, rock)
register struct help* self;
long rock;
{
    struct helpDir *thd;

    if ((self->expandedList && (rock == help_EXPAND)) ||
	(!self->expandedList && (rock == help_SHRINK)))
	return;
    /* could fall through here if rock is ALWAYS_TOGGLE */
    if (!self->showList) TogglePrograms(self, help_ALWAYS_TOGGLE);

    self->expandedList = 1 - self->expandedList; /* toggle */
    self->info->flags ^= (MENU_ToggleSizeExpand | MENU_ToggleSizeShrink); /* toggle menus */

    if (self->oldpanel)	/* using tmp, restore original */
	RestorePanel(self);

    if (!self->expandedList) { /* shrink it down */
	char *tmp, pathName[MAXPATHLEN];

	DEBUG(("shrinking "));
	
	self->expandedList = 0;

	panel_RemoveAll(self->listPanel); /* this frees all the strings in panelList, too */
	
	/* add only the small list of entries to listPanel */
	tmp = environ_GetConfiguration(SETUP_LIBDIR);
	if (tmp == NULL)
	    tmp = environ_AndrewDir(DEFAULT_LIBDIR);
	sprintf(pathName, "%s%s", tmp, PROGRAMFILE);

	if (0 == SetupPanel(TRUE, pathName, self->listPanel, NULL)) {
		/* we got nothing from the lib/help.programs file.  Try using extensions. */
		tmp = environ_GetConfiguration(SETUP_HELPDIR);
		if (tmp == NULL)
			tmp = environ_AndrewDir(DEFAULT_HELPDIR);
		strcpy(pathName, tmp);
	
		SetupPanel(FALSE, pathName, self->listPanel, program_ext_array);
	}

    } else {			/* pump it up */

	DEBUG(("expanding "));

	if (!panelList)
	    panelList = (char **)malloc(sizeof(char *) * help_MAXPANEL);

	panelIndex = 0;
	
	if (!helpdb_CheckIndex(self))
	    return;
	message_DisplayString(self, 0, msg_expanding);
	im_ForceUpdate();
	im_SetProcessCursor(waitCursor);
	self->expandedList = 1;

	/* get everything in the index */
	helpdb_Enumerate(Expander, self);

	/* now add in the auxiliary help dirs */
	for(thd = (struct helpDir *)helpdb_GetHelpDirs(); thd; thd = thd->next) {
	    char *tmp;
	    char *subdir = MANSUBS;
	    char dir[MAXPATHLEN];
	    
	    tmp = rindex(thd->dirName, '/');
	    if (tmp)
		tmp = tmp+1;
	    else
		tmp = thd->dirName;
	    if (!strcmp(tmp, "man")) {
		strcpy(dir, thd->dirName);
		strcat(dir, "/man");
		tmp = dir + strlen(dir);
		*(tmp + 1) = '\0';
		while (*subdir) {
		    *tmp = *subdir++;
		    ExpanderAux(dir);
		}
	    } else
		ExpanderAux(dir);
	}

	/* now make the list */
	SortAndMakePanel(self->listPanel);

	im_SetProcessCursor((struct cursor *) NULL);
    }
    label_SetText(self->listLab, (self->expandedList) ?
		  "Full Program List" : "Programs");
    SetupMenus(self->info);
    message_DisplayString(self, 0, "Done.");
}


/*
 * restores the original program panel (unfiltered)
 */
static void RestorePanel(self)
register struct help *self;
{
    if (!self->oldpanel) {
	DEBUG(("already have old panel\n"));
	return;
    }

    self->info->flags ^= (MENU_ToggleRestoreShow | (MENU_ToggleFilterShow | MENU_ToggleReFilterShow));

    if (!self->showPanels)
	TogglePanels(self, help_ALWAYS_TOGGLE);

    panel_RemoveAll(self->tmpanel);
    scroll_SetView(self->listScroll, self->oldpanel);
    self->oldpanel = (struct panel *)NULL;

    label_SetText(self->listLab, (self->expandedList) ?
		  "Full Program List" : "Programs");
    SetupMenus(self->info);
}


/*
 * replaces the program panel with a new panel that contains only those
 * entries from the old panel specified by the user
 */
static void FilterPanel(self, rock)
register struct help *self;
long rock;
{
    int code;
    char buf[255];
    char lbuf[255];
    struct panel_Entry *pe, *list;
    regexp *pattern;    /* used to store compiled version of expression */

    /* if rock == sort && self->oldpanel, punt */
    /* if rock == resort && self->oldpanel, OK */
    if (self->oldpanel && (rock == help_FILTER_FILTER)) { /* it's already being used */
	DEBUG(("already using old panel\n"));
	return;
    }

    if (!self->showPanels)
	TogglePanels(self, help_ALWAYS_TOGGLE);

    code = message_AskForString(self, 0, msg_filter_prompt,
				0, buf, sizeof(buf)-1);
    if ((code < 0) || (buf[0] == '\0'))
	return;

    panelIndex = 0;

    if (rock == help_FILTER_FILTER)
	list = self->listPanel->panelList;
    else
	list = self->tmpanel->panelList;
    
    pattern = regcomp(buf); /* compile the pattern and save the results */
    if (pattern == NULL) {
	ERRORBOX(self->info->view, "Sorry; filter pattern was not understood");
	return;
    } else {
	for (pe = list; pe != NULL; pe = pe->next) {
	    if (regexec(pattern, pe->tag) != NULL)
		AddToPanelList(pe->tag);
	}
	DEBUG(("done\n"));
	free(pattern);	/* return the space */
    }
    if(panelIndex == 0){
	char bbuf[512];
	sprintf(bbuf,"Panel does not contain \"%s\"",buf);
	ERRORBOX(self->info->view,bbuf);
	return;
    }

    if (rock == help_FILTER_FILTER) { /* only the first time */
	self->oldpanel = self->listPanel;

	self->info->flags ^= (MENU_ToggleRestoreShow | (MENU_ToggleFilterShow | MENU_ToggleReFilterShow));
	SetupMenus(self->info);
    }

    im_SetProcessCursor(waitCursor);
    im_ForceUpdate();

    sprintf(lbuf, "%s '%s'", "filtered by ", buf);
    label_SetText(self->listLab, lbuf);

    /* now make the list */
    SortAndMakePanel(self->tmpanel);
    
    scroll_SetView(self->listScroll, self->tmpanel);
    DEBUG(("set data\n"));

    im_SetProcessCursor((struct cursor *)NULL);
}



/*---------------------------------------------------------------------------*/
/*			     HELP CLASSPROCEDURES			     */
/*---------------------------------------------------------------------------*/

/*
 * makes new instance of a help object.  Creates keylist and menus, binds procedures
 * to menus and keys, and adds a default filetype so that all files will have
 * at least the default template when they are displayed.
 */
boolean help__InitializeClass(classID)
struct classheader *classID;
{
    unsigned char c[2];
    struct proctable_Entry *pe;

#ifdef DEBUGGING
    if ((char *)getenv("HELPDEBUG") != (char *) NULL)
	HELPDEBUG = 1;
#endif /* DEBUGGING */
    
    DEBUG(("IN init class\n"));
    Help_Menus = menulist_New();
    Help_Map = keymap_New();
    if (!Help_Menus || !Help_Map)
	return FALSE;

    /* make all printing keys put a readonly message in the message line */
    c[1] = '\0';
    pe = proctable_DefineProc("help-readonly-key", nono, &help_classinfo, 0, "fake readonliness");
    for (c[0] = (unsigned char)0; c[0] <= (unsigned char)127; c[0]++)
	if (isprint(c[0]) || c[0] == '\n' || c[0] == ' ')
	    keymap_BindToKey(Help_Map, c, pe, NULL);

    bind_BindList(helpBindings, Help_Map, Help_Menus, &help_classinfo);

    /* what this is really doing is adding a catchall filetype and template */
    filetype_AddEntry("*", "text", "template=default");

    class_Load("helpdb");
    class_Load("framcmds");
    
    DEBUG(("OUT init class\n"));
    return TRUE;
}

/*
 * initializes a new help object.  Sets up the view hiearchy, sets up panels, etc
 */
boolean help__InitializeObject(classID,self)
struct classheader *classID;
register struct help *self;
{
    char pathName[MAXPATHLEN], *tmp;
    struct proctable_Entry *pe;
    struct self_help *id;
    struct view *v;

    DEBUG(("IN init obj\n"));
    waitCursor = cursor_Create(0);
    cursor_SetStandard(waitCursor, Cursor_Wait);
    im_SetProcessCursor(waitCursor);

    tmp = environ_GetConfiguration(SETUP_CHANGEDIR);
    if (tmp == NULL) {
	tmp = environ_LocalDir(DEFAULT_CHANGEDIR);
    }
    strcpy(changesDir, tmp);

    tmp = environ_GetConfiguration(SETUP_TUTORIALDIR);
    if (tmp == NULL) {
	tmp = environ_AndrewDir(DEFAULT_TUTORIALDIR);
    }
    strcpy(tutorialDir, tmp);

    im_SetProcessCursor((struct cursor *) NULL);

    /* general variables */
    self->expandedList = 0;
    self->showPanels = environ_GetProfileSwitch("ShowPanels", TRUE);
    self->showOverview = environ_GetProfileSwitch("ShowOverviews", TRUE);
    self->showList = environ_GetProfileSwitch("ShowPrograms", TRUE);
    self->showHistory = environ_GetProfileSwitch("ShowHistory", FALSE);

    self->state = keystate_Create(self, Help_Map);
    self->info = (struct cache *)malloc(sizeof(struct cache));
    if (!self->info || !self->state)
	return FALSE;
    DEBUG(("info state "));
    self->info->scroll = NULL;
    self->info->menus = menulist_DuplicateML(Help_Menus, self);
    if (!self->info->menus)
	return FALSE;
    DEBUG(("menus "));
    self->info->flags = 0;
    self->info->flags = MENU_ToggleSizeExpand; /* start out shrunk initially */
    self->info->flags |= (self->showPanels) ? MENU_TogglePanelHide : MENU_TogglePanelShow;
    self->info->flags |= (self->showOverview) ? MENU_ToggleOverHide : MENU_ToggleOverShow;
    self->info->flags |= (self->showList) ? MENU_ToggleListHide : MENU_ToggleListShow;
    self->info->flags |= (self->showHistory) ? MENU_ToggleHistHide : MENU_ToggleHistShow;
    self->info->flags |= MENU_ToggleFilterShow;
    self->info->all = self->info->cur = (struct helpFile *)NULL;
    self->info->histat = help_HIST_NOADD;
    self->info->histent[0] = '\0';
    self->info->lastHist = NULL;

    self->mainLpair = NULL;

    /* the help text object */
    self->info->data = (struct dataobject *)text_New();
    self->info->view = (struct view *)textview_New();
    if (!self->info->data || !self->info->view)
	return FALSE;
    DEBUG(("data view "));

    view_SetDataObject(self->info->view, self->info->data);
    
    self->info->scroll = scroll_Create(self->info->view, scroll_LEFT);
    textview_SetEmbeddedBorder((struct textview *) self->info->view, 20, 5);
    if (!self->info->scroll)
	return FALSE;
    DEBUG(("scroll "));
    
    /* panels and their scrollbars */
    self->oldpanel = (struct panel *)NULL;
    self->tmpanel = panel_New();
    self->overviewPanel = panel_New();
    self->listPanel = panel_New();
    self->historyPanel = panel_New();
    if (!self->tmpanel || !self->overviewPanel || !self->listPanel || !self->historyPanel)
	return FALSE;
    DEBUG(("panels "));
    self->info->hist = self->historyPanel;

    self->overviewScroll = (struct scroll *) panel_GetApplicationLayer(self->overviewPanel);
    self->listScroll = (struct scroll *) panel_GetApplicationLayer(self->listPanel);
    self->historyScroll = (struct scroll *) panel_GetApplicationLayer(self->historyPanel);
    if (!self->overviewScroll || !self->listScroll)
	return FALSE;


    /* labels */
    self->overviewLab = label_New();
    self->overviewLabV = labelview_New();
    self->listLab = label_New();
    self->listLabV = labelview_New();
    self->historyLab = label_New();
    self->historyLabV = labelview_New();
    if (!self->overviewLab || !self->overviewLabV || !self->historyLab || !self->historyLabV
	|| !self->listLab || !self->listLabV)
	return FALSE;

    labelview_SetDataObject(self->overviewLabV, self->overviewLab);
    label_SetFlags(self->overviewLab, label_CENTERED | label_BOXED);
    label_SetText(self->overviewLab, "Overviews");

    labelview_SetDataObject(self->listLabV, self->listLab);
    label_SetFlags(self->listLab, label_CENTERED | label_BOXED);
    label_SetText(self->listLab, "Programs");
    
    labelview_SetDataObject(self->historyLabV, self->historyLab);
    label_SetFlags(self->historyLab, label_CENTERED | label_BOXED);
    label_SetText(self->historyLab, "Help History");


    /* lpairs */
    self->overviewLpair = lpair_Create(self->overviewLabV, self->overviewScroll, LABPCT);
    self->listLpair = lpair_Create(self->listLabV, self->listScroll, LABPCT);
    self->historyLpair = lpair_Create(self->historyLabV, self->historyScroll, LABPCT);
    self->panelLpair1 = lpair_Create(NULL, NULL, -PANELPCT);
    self->panelLpair2 = lpair_Create(NULL, NULL, -33);
    if (!self->overviewLpair || !self->listLpair || !self->historyLpair
	|| !self->panelLpair1 || !self->panelLpair2)
	return FALSE;
    
    lpair_VTFixed(self->overviewLpair, self->overviewLabV, self->overviewScroll, LABPCT, 0);
    lpair_VTFixed(self->listLpair, self->listLabV, self->listScroll, LABPCT, 0);
    lpair_VTFixed(self->historyLpair, self->historyLabV, self->historyScroll, LABPCT, 0);

    /* if the user has all panels off, use the overview panel by default */
    v = SetupLpairs(self);
    self->mainLpair = lpair_Create(self->info->scroll,
				   v ? v : (struct view *)self->overviewScroll, -MAINPCT);
    if (!self->mainLpair)
	return FALSE;
    DEBUG(("main lpair "));
    
    lpair_SetLPState(self->mainLpair, lpair_NOCHANGE, lpair_VERTICAL, 1);

    DEBUG(("handlers "));

    /* default panel handlers */
    panel_SetHandler(self->overviewPanel, OverviewHelp, (char *) self);
    panel_SetHandler(self->listPanel, OverviewHelp, (char *) self);
    panel_SetHandler(self->tmpanel, OverviewHelp, (char *) self);
    panel_SetHandler(self->historyPanel, HistoryHelp, (char *) self);

    if (self->showPanels)
	lpair_LinkTree(self->mainLpair, self);
    else
	scroll_LinkTree(self->info->scroll, self);

    /* add minimum set of entries to listPanel */
    tmp = environ_GetConfiguration(SETUP_LIBDIR);
    if (tmp == NULL)
	tmp = environ_AndrewDir(DEFAULT_LIBDIR);
    sprintf(pathName, "%s%s", tmp, PROGRAMFILE);
    
    if (0 == SetupPanel(TRUE, pathName, self->listPanel, NULL)) {
	/* got nothing from lib/help.programs;  use extensions */
	tmp = environ_GetConfiguration(SETUP_HELPDIR);
	if (tmp == NULL)
		tmp = environ_AndrewDir(DEFAULT_HELPDIR);
	strcpy(pathName, tmp);
	help_AddSearchDir(pathName);	/* ??? XXX */
   
	SetupPanel(FALSE, pathName, self->listPanel, program_ext_array);
    }

    DEBUG(("expanding "));
    if (environ_GetProfileSwitch("ExpandedList", FALSE))
	ToggleProgramListSize(self, help_EXPAND);

    /* get overviews entries */
    tmp = environ_GetConfiguration(SETUP_LIBDIR);
    if (tmp == NULL)
	tmp = environ_AndrewDir(DEFAULT_LIBDIR);
    sprintf(pathName, "%s%s", tmp, OVERVIEWFILE);
    if (0 == SetupPanel(TRUE, pathName, self->overviewPanel, NULL)) {
	tmp = environ_GetConfiguration(SETUP_HELPDIR);
	if (tmp == NULL)
		tmp = environ_AndrewDir(DEFAULT_HELPDIR);
	strcpy(pathName, tmp);
	help_AddSearchDir(pathName);	/* ??? XXX */

	SetupPanel(FALSE, pathName, self->overviewPanel, overview_ext_array);
    }

    /* post our menus */
    SetupMenus(self->info);

    /* runtime-bound procedures */
    pe = proctable_Lookup("textview-search");
    if (pe) textSearch = (void (*)()) proctable_GetFunction(pe);
    else return FALSE;
    pe = proctable_Lookup("textview-reverse-search");
    if (pe) textRevSearch = (void (*)()) proctable_GetFunction(pe);
    else return FALSE;
    pe = proctable_Lookup("textview-search-again");
    if (pe) textSearchAgain = (void (*)()) proctable_GetFunction(pe);
    else return FALSE;
    pe = proctable_Lookup("textview-copy-region");
    if (pe) textCopyRegion = (void (*)()) proctable_GetFunction(pe);
    else return FALSE;
    pe = proctable_Lookup("textview-next-screen");
    if (pe) textPageDown = (void (*)()) proctable_GetFunction(pe);
    else return FALSE;
    pe = proctable_Lookup("textview-prev-screen");
    if (pe) textPageUp = (void (*)()) proctable_GetFunction(pe);
    else return FALSE;
    pe = proctable_Lookup("frame-set-printer");
    if (pe) frameSetPrinter = (void (*)()) proctable_GetFunction(pe);
    else return FALSE;

    /* now add ourself to the head of the instance list */
    DEBUG(("ego: %d\n",(int)ego));
    id = (struct self_help *)malloc(sizeof(struct self_help));
    if (!id)
	return FALSE;
    id->this = self;
    id->next = ego;
    ego = id;
    DEBUG(("ego: %d\n",(int)ego));

    DEBUG(("OUT init obj\n"));
    return TRUE;
}


/*
 * destroys the resources of the help object
 */
void help__FinalizeObject(classID, self)
struct classhead *classID;
register struct help *self;
{
    DEBUG(("IN finalize\n"));
    /* the presence/absence of self->info indicates whether we are
       in the process of finalizing, so that in update we know we
       can't post menus */
    free(self->info);
    self->info = NULL;
    if(panelList!=NULL){
	free(panelList);
	panelList=NULL;
    }
    im_Destroy(view_GetIM((struct view *)self));
    DEBUG(("info\n"));
    DEBUG(("OUT finalize\n"));
}

     
/*
 * help__ method interface for using an alias file
 */
void help__SetAliasesFile(classID,alias)
struct classheader *classID;
register char *alias;
{
    helpdb_ReadAliasesFile(alias);
}

/*
 * help__ method for adding a help index directory
 */
void help__SetIndex(classID,aindex)
struct classheader *classID;
register char *aindex;
{
    DEBUG(("IN SetIndex: %s\n",aindex));
    helpdb_SetIndex(aindex);
}

/*
 * help__ method for adding a directory to the searchpath
 */
void help__AddSearchDir(classID,dirName)
struct classheader *classID;
char *dirName;
{
    if (access(dirName, 4) < 0) {
	fprintf(stderr, "help: cannot open search directory '%s'\n", dirName);
	return;
    }
    DEBUG(("adding dir: '%s'\n",dirName));
    helpdb_AddSearchDir(dirName);
}


/*
 * outside interface help__ method for GetHelpOn.  Used by helpapp
 *
 * Returns:
 *	-1: if a server was down while trying to get help on this topic
 *	 0: if no help found for this topic
 *	 1: if successful
 */
static int help__HelpappGetHelpOn(classID, aname, isnew, ahistory, errmsg)
struct classheader *classID;
char *aname;	/* what topic */
long isnew;	/* is this a new topic? */
int ahistory;	/* show in history log? 1-show aname 2-show tail of filename */
char *errmsg;	/* error to print if failure. "Error" if this is NULL */
{
    if (aname[0] == '\0') {
	/* so -e flag with no file shown works */
	return 1;
    }
    /* use the last instance added to the list */
    DEBUG(("using %d view %d asking %d\n", (int)ego->this,(int)ego->this->info->view,
	   message_Asking(ego->this->info->view)));
    if (message_Asking(ego->this->info->view)) {
	DEBUG(("retracting\n"));
	message_CancelQuestion(ego->this->info->view);
    }
    return GetHelpOn(ego->this, aname, isnew, ahistory, errmsg);
}


/*
 * return the first view in the list of instances, so helpapp can expose
 * a hidden window.
 */
struct view *help__GetInstance(classID)
struct classheader *classID;
{
    return (struct view *)(ego->this);
}


/*---------------------------------------------------------------------------*/
/*				HELP METHODS				     */
/*---------------------------------------------------------------------------*/

/*
 * self explanatory
 */
void help__PostMenus(self, menuList)
register struct help *self;
struct menulist *menuList;
{
    DEBUG(("post menus\n"));
    if (self->info != NULL)
	super_PostMenus(self, self->info->menus);
}

/*
 * override parents' keybindings
 */
void help__PostKeyState(self, keyState)
register struct help *self;
struct keystate *keyState;
{
    DEBUG(("post keys\n"));
    keystate_AddBefore(self->state, keyState);
    super_PostKeyState(self, self->state);
}

/*
 * mouse action handler
 */
struct view *help__Hit(self, action, x, y, numberOfClicks)
struct help *self;
enum view_MouseAction action;
long x, y;
long numberOfClicks;
{
    if (self->showPanels)
	return lpair_Hit(self->mainLpair, action, x, y, numberOfClicks);
    else
	return scroll_Hit(self->info->scroll, action, x, y, numberOfClicks);
}

/*
 * set up the view tree based on whether panels or scrollbar is the top level view
 */
void help__LinkTree(self, parent)
struct help *self;
struct view *parent;
{
    DEBUG(("IN link tree\n"));
    DEBUG(("\tsuper..."));
    super_LinkTree(self, parent);

    if (self->showPanels) {
	if (self->mainLpair)
	    lpair_LinkTree(self->mainLpair, self);
    } else {
	if (self->info!=NULL && self->info->scroll)
	    scroll_LinkTree(self->info->scroll, self);
    }
    DEBUG(("OUT link tree\n"));
}

/*
 * refreshing!
 */
void help__FullUpdate(self, type, left, top, width, right)
register struct help *self;
enum view_UpdateType type;
long left, top, width, right;
{
    struct rectangle childRect;

    DEBUG(("IN fullupdate\n"));

    help_GetLogicalBounds(self, &childRect);

    if (self->showPanels) {	/* need the main lpair as the root */
	lpair_InsertView(self->mainLpair, self, &childRect);
	lpair_FullUpdate(self->mainLpair, type, left, top, width, right);
    } else {			/* we just have the textview to show */
	scroll_InsertView(self->info->scroll, self, &childRect);
	scroll_FullUpdate(self->info->scroll, type, left, top, width, right);
    }
    super_WantInputFocus(self, self->info->view);
    DEBUG(("OUT fullupdate\n"));
}

/*
 * erase and refresh the screen
 */
void help__Update(self)
register struct help *self;
{
    DEBUG(("IN update\n"));
    /* clear out the region, then do a full redraw */
    help_EraseVisualRect(self);
    help_FullUpdate(self, view_FullRedraw, help_GetLogicalTop(self), help_GetLogicalLeft(self),
        help_GetLogicalWidth(self), help_GetLogicalHeight(self));
    DEBUG(("OUT update\n"));
}

/*
 * update event handler
 */
void help__WantUpdate(self, requestor)
register struct help *self;
register struct view *requestor;
{
    /* if the scrollbar took the hit, check if the user has selected something,
       and turn on some menus items if so */
    if ((requestor == (struct view *)self->info->scroll)) {
	/* if the menumask changes, post menus */
	if (menulist_SetMask(self->info->menus,
			 (textview_GetDotLength((struct textview *)self->info->view) != 0) ?
			 menulist_GetMask(self->info->menus) | MENU_SwitchCopy :
			 menulist_GetMask(self->info->menus) & ~MENU_SwitchCopy))
	    help_PostMenus(self, self->info->menus);
    }
    super_WantUpdate(self, requestor);
}


/*
 * classproc to handle getting help using a terminal-based interface
 */
void help__GetHelpOnTerminal(classID,akey,list,print)
struct classheader *classID;
register char *akey;		/* topic string */
register int list;		/* do help on topic, or just list files? */
register int print;		/* prompt for printing each helpfile? */
{
    FILE *tfile;
    char *alias, *pager, *index, *tmp;
    char pathName[MAXPATHLEN], cmdLine[MAXPATHLEN];
    char hbuffer[HNSIZE], tbuffer[HNSIZE];
    long digitMode, digitValue, firstChar;
    struct helpFile *tmpf;
    struct cache c;
    int tc;
    int first = 0;

    DEBUG(("key: %s\n",akey));
    if (!helpdb_CheckIndex(NULL)) { /* if we haven't SetIndex */
	index = environ_GetConfiguration(SETUP_INDEXDIR);
	if (index == NULL) 
	    index = environ_AndrewDir(DEFAULT_INDEXDIR);
	help_SetIndex(index);
    }

    if ((pager = environ_Get(PAGERVAR)) == (char *) NULL)
	pager = DEFAULTPAGER;

    if (!helpdb_CheckIndex(NULL)) {
        fprintf(stderr, *err_index, pathName);
        return;
    }

    /* map helpKey */
    strncpy(tbuffer, LowerCase(akey), HNSIZE);
    tbuffer[HNSIZE-1] = 0;

    /* now see if it has an illegal, command running, alias */
    alias = (char *)helpdb_MapAlias(tbuffer);
    DEBUG(("map alias on: %s\n",tbuffer));
    if (alias) {
	DEBUG(("alias: %s\n", alias));
        if (alias[0] == '#') {
            fprintf(stderr,err_terminal);
	    putchar('\n');
            exit(1);
	    /*NOTREACHED*/
        }
    }

    alias = tbuffer;		/* src */
    tmp = hbuffer;		/* dst */
    digitMode = 0;
    digitValue = 0;
    firstChar = 1;
    /* copy alias (points to tbuffer) to hbuffer, suppressing spaces and
       computing digitValue */
    while((tc = *alias++) != NULL) {
        if (tc == ' ' || tc == '\n' || tc == '\t')
	    continue;
        if (tc == '(' || (!firstChar && tc == '.'))
	    digitMode = 1;
        else if (digitMode) {
            if (tc >= '0' && tc <= '9') {
                digitValue *= 10;
                digitValue += tc - '0';
            }
        } else {
            *tmp++ = tc;
	}
        firstChar = 1;
    }
    *tmp++ = 0;

    DEBUG(("tbuf: %s hbuf: %s\n",tbuffer,hbuffer));

    c.all = (struct helpFile *)NULL;

    if (helpdb_SetupHelp(&c, hbuffer, FALSE) == 0) {
        fprintf(stderr, err_sorry, akey);
	putchar('\n');
        exit(1);
	/*NOTREACHED*/
    }

    /* if non-0 return, c->all was set up for us */

    DEBUG(("setup done\n"));
    
    if (list) {
	printf("Using search directories: ");
	helpdb_PrintSearchDirs();
	printf("\n");
        printf("For topic %s the files are:\n",akey);
	for (tmpf = c.all; tmpf; tmpf = tmpf->next) {
	    printf("\t%s\n",tmpf->fileName);
	}
	exit(0);
	/*NOTREACHED*/
    }

    /* o.w. do real help on the topic */
    first = TRUE;
    for(tmpf = c.all; tmpf; tmpf = tmpf->next) {
	int in = 0;
	char *fn;
	int skipFlag = FALSE;

	tfile = fopen(tmpf->fileName, "r");
	if (!tfile) {
	    fprintf(stderr, err_file, tmpf->fileName);
	    continue;
	}

	if (!first) {
	    skipFlag = FALSE;
	    fn = rindex(tmpf->fileName, '/');
	    printf(msg_term_prompt, fn ? fn+1 : tmpf->fileName);
	    in = getchar();
	    DEBUG(("'%c'\n", in));
	    if (in < 0) exit(1);
	    else if (in == 'q' || in == 'Q') exit(0);
	    else if (in == 'n' || in == 'N') skipFlag = TRUE;
	    if (in != '\n')
		while ((in = getchar()) > 0 && in != '\n')
		    ;
	}

	first = FALSE;
	
	if (!skipFlag) {
	    /* show the file */
	    tc = fgetc(tfile);
	    ungetc(tc,tfile);
	    fclose(tfile);
	    if ((tc == '.') || (tc == '#')) { /* troff file */
		sprintf(cmdLine, ROFFCMD, tmpf->fileName, pager);
	    } else {
		sprintf(cmdLine, ATKROFFCMD, tmpf->fileName, pager);
	    }
	    DEBUG(("cmd: %s\n",cmdLine));
	    system(cmdLine);
	}

	if (print) {
	    /* see if they want to print it */
	    if (in < 0) exit(1);
	    fn = rindex(tmpf->fileName, '/');
	    printf(msg_print_prompt, fn ? fn+1 : tmpf->fileName);
 	    in = getchar();
	    DEBUG(("'%c'\n", in));
	    if (in < 0) exit(1);
	    else if (in != 'n' && in != 'N') { /* print it */
		if ((tc == '.') || (tc == '#')) { /* troff file */
		    sprintf(cmdLine, ROFFPRINTCMD, tmpf->fileName);
		} else {
		    sprintf(cmdLine, ATKPRINTCMD, tmpf->fileName);
		}
		DEBUG(("cmd: %s\n",cmdLine));
		system(cmdLine);
	    }
	    if (in != '\n')
		while ((in = getchar()) > 0 && in != '\n')
		    ;
	}
    }
}
