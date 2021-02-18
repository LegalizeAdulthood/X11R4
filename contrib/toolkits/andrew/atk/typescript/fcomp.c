/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* fcomp.c -- typescript package to perform filename completion
  * David Anderson
  * August 1988
  */

#include <andyenv.h>
#include <andrewos.h>
#include <class.h>
#include <text.ih>
#include <mark.ih>
#include <proctbl.ih>
#include <smpltext.ih>
#include <dataobj.ih>
#include <tscript.ih>
#include <filetype.ih>
#include <keystate.ih>
#include <keymap.ih>
#include <im.ih>
#include <environ.ih>
#include <envrment.ih>
#include <message.ih>
#include <cursor.ih>
#include <style.ih>
#include <stylesht.ih>
#include <fontdesc.ih>
#include <complete.ih>
#include <search.ih>

#include <fcomp.eh>

#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/dir.h>

extern char *rindex();
static struct cursor *waitCursor;

static int (*typescript_GrabLastCmd) ();

#define Text(self) ((struct text *) ((self)->header.view.dataobject))
#define MyEnvinfo(text,pos) environment_GetInnerMost(text->rootEnvironment, pos)
#define min(x, y) (((x) < (y)) ? (x) : (y))

CompleteFname(ts, key)
register struct typescript *ts;
long key;
{
    register long pos;
    register long fname;
    register struct text *theText;
    char pathname[MAXPATHLEN];
    char buffer[MAXPATHLEN];
    char canonical[MAXPATHLEN];
    register char *insert;
    char *msg = NULL;

    pos = typescript_GetDotPosition(ts);
    theText = Text(ts);
    if (pos == text_GetLength(theText)) {
        fname = pos;
        while(text_GetChar(theText, fname) != ' ') fname--;
        fname++;
        text_CopySubString(theText, fname, pos-fname, pathname, FALSE);
        pathname[pos-fname] = 0;
        switch (completion_FileComplete(pathname, FALSE, buffer, MAXPATHLEN)) {
            case message_Complete:
                msg = "[Unique]";
                break;
            case message_CompleteValid:
                msg = "[Others]";
                break;
            case message_Valid:
                msg = "[Incomplete]";
                break;
            case message_Invalid:
                msg = "[No Match]";
                break;
        }
        message_DisplayString(ts, 0, msg);
        filetype_CanonicalizeFilename(canonical, pathname, MAXPATHLEN);
        insert = buffer + strlen(canonical);
        text_InsertCharacters(theText, pos, insert, strlen(insert));
        typescript_SetDotPosition(ts, pos+strlen(insert));
        text_NotifyObservers(theText, 0);
    }
}


struct helpstat {
    boolean refusedot;
    long itemcnt;
    long maxitemlen;
    long errorcnt;
};

#define acceptitem(rock, item, itemtype) \
  (itemtype == message_HelpListItem) && \
  strcmp(item, "./") && strcmp(item, "../") && \
  (item[0] != '.' || !rock->refusedot)

static GatherStats(rock, itemtype, item, dummy)
struct helpstat *rock;
enum message_HelpItem itemtype;
char *item;
long dummy; /* along for the ride */
{
    if (acceptitem(rock, item, itemtype)) {
        long itemlen = strlen(item);

        rock->itemcnt++;
        if (itemlen > rock->maxitemlen) {
            rock->maxitemlen = itemlen;
        }
    }
    else if (itemtype != message_HelpListItem) rock->errorcnt++;
}


struct repCookie {
    boolean refusedot;
    char *report;
    long columns;
    long colwidth;
    long rows;
    long rowlen;
    long repcolwidth;
    long reprowlen;
    long widthinchar;
    long count;
};

#define COLSPACE    1
#define repspot(cookie, s) ((s) * cookie->repcolwidth)

static MakeReport(cookie, itemtype, item, dummy)
struct repCookie *cookie;
enum message_HelpItem itemtype;
char *item;
long dummy; /* along for the ride */
{
    if (acceptitem(cookie, item, itemtype)) {
        strcpy(&cookie->report[repspot(cookie, cookie->count)], item);
        cookie->count++;
    }
}


PossibleCompletions(ts, key)
register struct typescript *ts;
long key;
{
    register long pos;
    register struct text *theText;

    pos = typescript_GetDotPosition(ts);
    theText = Text(ts);
    if (pos == text_GetLength(theText)) {
        register long fname;
        char pathname[MAXPATHLEN];
        long pathlen;
        boolean emptycmd;
        struct helpstat rock;

        emptycmd = (mark_GetPos(ts->cmdStart) == pos);
        fname = pos;
        while(text_GetChar(theText, fname) != ' ') fname--;
        fname++;
        text_CopySubString(theText, fname, pos-fname, pathname, FALSE);
        pathname[pos-fname] = 0;

        text_InsertCharacters(theText, pos++, "\n", 1);

        rock.itemcnt = rock.maxitemlen = rock.errorcnt = 0;
        pathlen = strlen(pathname);
        rock.refusedot = FALSE;
        if (pathlen == 0) rock.refusedot = TRUE;
        else {
            if (pathname[pathlen-1] == '/') rock.refusedot = TRUE;
        }
        completion_FileHelp((*pathname) ? pathname : "./", 0, GatherStats, &rock);

        if (rock.errorcnt || rock.itemcnt == 0) {
            message_DisplayString(ts, 0, "[No Match]");
        }
        else {
            struct style *tsStyle;
            enum style_FontSize dummy;
            char ffamily[MAXPATHLEN];
            long tssize;
            struct fontdesc *tsfont;
            char spaces[MAXPATHLEN];
            long windowwidth;
            long charwidth;
            struct repCookie cookie;
            register long i;
            register long j;

            tsStyle = typescript_GetDefaultStyle(ts);
            style_GetFontSize(tsStyle, &dummy, &tssize);
            style_GetFontFamily(tsStyle, ffamily, MAXPATHLEN);
            tsfont = fontdesc_Create(ffamily, fontdesc_Fixed, tssize);

            charwidth = fontdesc_FontSummary(tsfont, ts)->maxSpacing;
            windowwidth = typescript_GetVisualRight(ts);

            cookie.refusedot = rock.refusedot;
            cookie.widthinchar = windowwidth/charwidth - 1;
            cookie.colwidth = rock.maxitemlen + COLSPACE;
            /* how many columns could fit? */
            cookie.columns = (cookie.widthinchar + COLSPACE) / cookie.colwidth;
            if (cookie.columns == 0) cookie.columns = 1;
            /* min number of rows required */
            cookie.rows = (rock.itemcnt + cookie.columns - 1) / cookie.columns;
            /* how many columns are actually needed? */
            cookie.columns = (rock.itemcnt + cookie.rows - 1) / cookie.rows;
            /* round out the columns to use the available space */
            cookie.colwidth = (cookie.widthinchar + COLSPACE) / cookie.columns;
            cookie.repcolwidth = rock.maxitemlen + 1;
            cookie.reprowlen = cookie.repcolwidth * cookie.columns;
            cookie.count = 0;
            cookie.report = (char *) malloc(rock.itemcnt * (rock.maxitemlen+1));

            completion_FileHelp((*pathname) ? pathname : "./", 0, MakeReport, &cookie);

            qsort(cookie.report, rock.itemcnt, rock.maxitemlen+1, strcmp);

            for (i=0; i<cookie.colwidth; i++) spaces[i] = ' ';

            for (i=0; i<cookie.rows; i++) {
                for (j=0; j<cookie.columns && i+j*cookie.rows<rock.itemcnt; j++) {
                    int len;
                    char *thisfile;
                    struct repCookie *c = &cookie;

                    thisfile = &cookie.report[repspot(c, i+j*cookie.rows)];
                    len = strlen(thisfile);
                    text_InsertCharacters(theText, pos, thisfile, len);
                    pos += len;
                    if (j+1 != cookie.columns) {
                        text_InsertCharacters(theText, pos, spaces, cookie.colwidth-len);
                        pos += cookie.colwidth-len;
                    }
                }
                text_InsertCharacters(theText, pos++, "\n", 1);
            }

            free(cookie.report);
        }

        ts->readOnlyLen = -1;
        typescript_SetDotPosition(ts, pos);
        typescript_SetDotLength(ts, 0);
        ts->lastEnv = NULL;
        mark_SetPos(ts->cmdStart, pos);
        mark_SetLength(ts->cmdStart, 0);
        text_SetFence(theText, pos);
        typescript_FrameDot(ts, pos);

        text_NotifyObservers(theText, 0);

        write(ts->SubChannel, "\n", 1);
        if (!emptycmd) {
            typescript_GrabLastCmd(ts);
            typescript_CollapseDot(ts);
        }
    }
}


#ifdef COMMANDCOMPLETION
static struct SearchPattern *pattern = 0;
static char lastcmd[MAXPATHLEN];
static long lastbegin = 0;
static long lastmatch;

CompleteCmd(ts, key)
register struct typescript *ts;
long key;
{
    register long pos;
    register struct text *theText;
    register long begincmd;

    pos = typescript_GetDotPosition(ts);
    theText = Text(ts);
    begincmd = mark_GetPos(ts->cmdStart);
    if (pos == text_GetLength(theText) && begincmd < pos) {
        char cmd[MAXPATHLEN];
        register long match;
        struct style *boldStyle;
        char *patcode;

        boldStyle = stylesheet_Find(theText->styleSheet, "bold");
        text_CopySubString(theText, begincmd, pos-begincmd, cmd, FALSE);
        cmd[pos-begincmd] = 0;
        if (begincmd == lastbegin && strcmp(cmd, lastcmd) == 0) {
            match = lastmatch;
        }
        else {
            match = begincmd;
        }
        if (patcode = search_CompilePattern(cmd, &pattern)) {
            message_DisplayString(ts, 0, patcode);
        }
        else {
            while ((match = search_MatchPatternReverse(theText, match-1, pattern)) >= 0) {
                if (MyEnvinfo(theText, match)->data.style == boldStyle) {
                    if (MyEnvinfo(theText, match-1)->data.style != boldStyle) break;
                }
            }
            lastmatch = match;
            strcpy(lastcmd, cmd);
            lastbegin = begincmd;
            if (match >= 0) {
                register long endmatch;

                text_DeleteCharacters(theText, begincmd, pos-begincmd);
                pos = begincmd;
                for (endmatch = match; text_GetChar(theText, endmatch) != '\n'; endmatch++);
                text_CopySubString(theText, match, endmatch-match, cmd, FALSE);
                text_InsertCharacters(theText, pos, cmd, endmatch-match);
                pos += endmatch-match;
                typescript_SetDotPosition(ts, pos);
                text_NotifyObservers(theText, 0);
            }
            else message_DisplayString(ts, 0, "[No matching command]");
        }
    }
}
#endif /* COMMANDCOMPLETION */


boolean fcomp__InitializeClass(classID)
struct classheader *classID;
{
    struct classinfo *imc;

    waitCursor = cursor_Create(NULL);
    cursor_SetStandard(waitCursor, Cursor_Wait);

#ifdef COMMANDCOMPLETION
    lastcmd[0] = 0;
    class_Load("search");
#endif /* COMMANDCOMPLETION */

    class_Load("completion");

    imc = class_Load("typescript");

    proctable_DefineProc("fcomp-complete-filename", CompleteFname, imc, NULL, "Filename completion (typescript)");
    proctable_DefineProc("fcomp-possible-completions", PossibleCompletions, imc, NULL, "Display possible filename completions (typescript)");

#ifdef COMMANDCOMPLETION
    proctable_DefineProc("fcomp-complete-command", CompleteCmd, imc, NULL, "Complete partial command (typescript)");
#endif /* COMMANDCOMPLETION */

    typescript_GrabLastCmd = proctable_GetFunction(proctable_Lookup("typescript-Grab-Last-Cmd"));

    return TRUE;
}
