/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/captions.c,v 2.28 89/10/09 22:04:00 wjh Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/captions.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/captions.c,v 2.28 89/10/09 22:04:00 wjh Exp $ ";
#endif /* lint */

#include <cui.h>
#include <fdphack.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <andrewos.h> /* sys/file.h */
#include <netinet/in.h>  /* for htonl, etc. */
#include <errprntf.h>
#include <ctype.h>
#include <rect.h>
#include <class.h>
#include <view.ih>
#include <im.ih>
#include <textv.ih>
#include <text822v.ih>
#include <text822.ih>
#include <message.ih>
/* #include <lpair.ih> */
#include <mark.ih>
#include <envrment.ih>
#include <frame.ih>
#include <scroll.ih>
#include <fontdesc.ih>
#include <environ.ih>
#include <search.ih>
#include <text.ih>
#include <ams.ih>
#include <amsutil.ih>
#include <sendmsg.ih>
#include <folders.ih>
#include <captions.eh>

AbsentProcedure(self) 
struct view *self;
{
    message_DisplayString(NULL, 75, "Absent procedure - did not find a normal BE2 command in the proctable!");
}

int (*textview_PreviousLineCmd)() = AbsentProcedure,
	    (*textview_ReverseSearchCmd)() = AbsentProcedure,
	    (*textview_ScrollScreenFwdCmd)() = AbsentProcedure,
	    (*textview_ScrollScreenBackCmd)() = AbsentProcedure,
	    (*textview_BeginningOfTextCmd)() = AbsentProcedure,
	    (*textview_BeginningOfLineCmd)() = AbsentProcedure,
	    (*textview_EndOfLineCmd)() = AbsentProcedure,
	    (*textview_GlitchDownCmd)() = AbsentProcedure;

extern char *malloc(), *index(), *rindex();

void captions__ShowHelp(self)
struct captions *self;
{
    int doprefix = 0;
    char *motdfile;
    FILE *fp;

    text_ClearCompletely(self->CaptText);
    motdfile = (char *) environ_GetProfile("messages.motdfile");
    if (!motdfile) {
	doprefix = 1;
	motdfile = "/etc/motd";
    }
    fp = fopen(motdfile, "r");
    if (!fp) {
	char ErrMsg[100];

	sprintf(ErrMsg, "Cannot open file %s (%d).", motdfile, errno);
	text_AlwaysInsertCharacters(self->CaptText, 0, ErrMsg, strlen(ErrMsg));
    } else {
	int inslen;
	struct environment *et;

	if (doprefix) {
	    unsigned long clock;
	    char *timestr, InsStr[500];
	    struct stat SBlock;

	    if (fstat(fileno(fp), &SBlock) == 0) {
		clock = (unsigned long) SBlock.st_mtime;
	    } else {
		clock = osi_GetSecs();
	    }
	    timestr = ctime(&clock);
	    inslen = strlen(timestr);
	    timestr[inslen-1] = '\0';
	    sprintf(InsStr, "Message of the day as of %s:\n\n", timestr);
	    inslen = strlen(InsStr);
	    text_AlwaysInsertCharacters(self->CaptText, 0, InsStr, inslen);
	    et = environment_InsertStyle(self->CaptText->rootEnvironment, 0, self->ActiveCaptionStyle, 1);
	    environment_SetLength(et, inslen - 1);
	} else {
	    inslen = 0;
	}
	text_SetReadOnly(self->CaptText, FALSE); /* BOGUS -- AlwaysInsertFile doesn't work for read-only text, 12/28/88 */
	text_AlwaysInsertFile(self->CaptText, fp, motdfile, inslen);
	text_SetReadOnly(self->CaptText, TRUE); /* BOGUS -- DITTO */
	fclose(fp);
    }
    captions_WantUpdate(self, self);
}    

void captions__SetBodies(captions, bv)
struct captions *captions;
struct t822view *bv;
{
    captions->BodView = bv;
    if (bv) t822view_SetCaptions(bv, captions);
}

struct view *
captions__Hit(captions, action, x, y, nclicks)
struct captions *captions;
int x, y, nclicks;  
enum view_MouseAction action;
{
    int thisdot, linelen, whichthis, whichdown;
    struct environment *env;

    if (action == view_LeftDown || action == view_RightDown) {
	super_Hit(captions, view_LeftDown, x, y, 1);
	captions->downdot = captions_GetDotPosition(captions);
    } else if (action == view_LeftUp) {
	super_Hit(captions, view_LeftUp, x, y, 1);
	captions_SimulateClick(captions, TRUE);
    } else if (action == view_RightUp)  {
	super_Hit(captions, view_LeftUp, x, y, 1);
	if (amsutil_GetOptBit(EXP_MARKING)) {
	    thisdot = captions_GetDotPosition(captions);
	    if (thisdot == captions->downdot) thisdot += captions_GetDotLength(captions);
	    if (thisdot != captions->downdot) {
		captions_FindCUIDByDocLocation(captions, &thisdot, &linelen, &env, &whichthis);
		captions_FindCUIDByDocLocation(captions, &captions->downdot, &linelen, &env, &whichdown);
		if (whichthis != whichdown) {
		    int i, low, high;

		    if (whichthis < whichdown) {
			low = whichthis;
			high = whichdown;
		    } else {
			low = whichdown;
			high = whichthis;
		    }
		    for (i=low; i<= high; ++i) {
			if (!captions->capcache[i].IsMarked) {
			    captions_ToggleMark(captions, &captions->capcache[i], captions->capcache[i].offset);
			}
		    }
		    captions_PostMenus(captions, NULL);
		    captions_ReportMarkedMessageCount(captions);
		    return((struct view *)captions);
		}
	    }
	}
	captions_SimulateClick(captions, FALSE);
    } else if (action == view_LeftMovement || action == view_RightMovement) {
	super_Hit(captions, view_LeftMovement, x, y, 1);
    }
    return((struct view *) captions);
}

void captions__SimulateClick(captions, IsLeftClick)
struct captions *captions;
boolean IsLeftClick;
{
    int linestart, linelen, thisCUID, whichcaption;
    struct environment *env;

    captions_WantInputFocus(captions, captions);
    if (captions->captioncachecount <= 0) {
	/* This code is kind of silly; it says if there are no captions on display, but there is some kind of text in the captions area, we will search through the bodies area for the contents of the current line and will scroll the body to start with that same line.  This is currently used only by the Set Options code, but it could in theory be used to make Messages able to view all sorts of things besides mail and bulletin boards... */
	struct textview *tv = (struct textview *)captions;
	struct text *t = (struct text *) captions_GetDataObject(captions);
	int dot, len, tmpdot, retlen;
	char *str, *tp, BodyBuf[1000];
	struct SearchPattern *Pattern = NULL;
	struct text *bod;

	ams_WaitCursor(TRUE);
	textview_BeginningOfLineCmd(tv);
	dot = textview_GetDotPosition(tv);
	textview_EndOfLineCmd(tv);
	tmpdot = textview_GetDotPosition(tv);
	len = tmpdot - dot;
	if (len > 0) {
	    str = text_GetBuf(t, dot, len, &retlen);
	    if (retlen >= sizeof(BodyBuf)) retlen = sizeof(BodyBuf) - 1;
	    strncpy(BodyBuf, str, retlen);
	    BodyBuf[retlen] = '\0';
	    tp = search_CompilePattern(BodyBuf, &Pattern);
	    if (tp) {
		message_DisplayString(NULL, 10, tp);
	    } else {
		bod = captions_GetBodDoc(captions);
		if ((tmpdot = search_MatchPattern(bod, dot, Pattern)) < 0) {
		    tmpdot = search_MatchPattern(bod, 0, Pattern);
		}
		if (tmpdot < 0) {
		    message_DisplayString(NULL, 10, "Option not found.");
		} else {
		    textview_SetTopPosition((struct textview *) captions_GetBodView(captions), tmpdot);
		}
	    }
	}
	ams_WaitCursor(FALSE);
	return;
    }
    linestart = captions_GetDotPosition(captions);
    thisCUID =  captions_FindCUIDByDocLocation(captions, &linestart, &linelen, &env, &whichcaption);
    captions_SetDotPosition(captions, linestart);
    captions_SetDotLength(captions, 0);

    if (IsLeftClick || (!amsutil_GetOptBit(EXP_MARKING))) { 
	captions_DisplayNewBody(captions, thisCUID, linestart, linelen, env);
    } else {
	captions_ToggleMark(captions, &captions->capcache[whichcaption], linestart);
	captions_PostMenus(captions, NULL);
	captions_ReportMarkedMessageCount(captions);
    }
}


int captions__DisplayNewBody(captions, thisCUID, linestart, linelen, env)
struct captions *captions;
int thisCUID, linestart, linelen;
struct environment *env;
{
    char *id, *dir;
    int WasDeleted, IsDeleted, checkvis;

    if (captions->captioncachecount <= 0) return;
    ams_WaitCursor(TRUE);
    /* Want to increase the likelihood the next line is visible unless this is the last line, in which case we simply want to ensure this line is visible. */
    if ((linestart + linelen + 1) < text_GetLength(captions->CaptText)) {
	checkvis = linestart+linelen+1;
    } else {
	checkvis = linestart;
    }
    if (!captions_Visible(captions, checkvis)) {
	captions_SetTopPosition(captions, linestart);
    }
    if (captions->HighlightLen) {
	RemoveHighlighting(captions);
    }
    captions->HighlightStartPos = linestart;
    captions->HighlightLen = linelen;
    captions->HighlightEnv = env;
    if (captions->HighlightEnv->data.style == captions->ActiveDeletedStyle
    || captions->HighlightEnv->data.style == captions->DeletedStyle) {
	text_SetEnvironmentStyle(captions->CaptText, captions->HighlightEnv, captions->ActiveDeletedStyle);
	WasDeleted = 1;
    } else {
	text_SetEnvironmentStyle(captions->CaptText, captions->HighlightEnv, captions->ActiveCaptionStyle);
	WasDeleted = 0;
    }
    t822view_WantUpdate(captions_GetBodView(captions), captions_GetBodView(captions));
    captions_WantUpdate(captions, captions);
    im_ForceUpdate();
    if (thisCUID == captions->VisibleCUID) {
	SetSouthernmostPoint(captions, linestart);
	ams_WaitCursor(FALSE);
	return(0);
    }
    captions->VisibleCUID = thisCUID;
    if (captions_GetBodyFromCUID(captions, thisCUID, MODE822_NORMAL, NULL)) {
	/* error was reported */
	ams_WaitCursor(FALSE);
	ResetCaptionNotBody(captions);
	return(-1);
    }
    IsDeleted = AMS_GET_ATTRIBUTE(captions->VisibleSnapshot, AMS_ATT_DELETED) ? 1 : 0;
    if (IsDeleted != WasDeleted) {
	/* Someone else just deleted or undeleted it! */
	if (IsDeleted) {
	    message_DisplayString(NULL, 25, "Someone has just deleted this message, but not yet purged it.");
	    text_SetEnvironmentStyle(captions->CaptText, captions->HighlightEnv, captions->ActiveDeletedStyle);
	} else {
	    text_SetEnvironmentStyle(captions->CaptText, captions->HighlightEnv, captions->ActiveCaptionStyle);
	    message_DisplayString(NULL, 25, "Someone has just undeleted this message.");
	}	
	captions_AlterDeletedIcon(captions, captions->HighlightStartPos, IsDeleted);
	captions_WantUpdate(captions, captions);
    }
    SetSouthernmostPoint(captions, linestart);
    im_ForceUpdate();
    if (!ams_CUI_GetAMSID(ams_GetAMS(), thisCUID, &id, &dir)) {
	ams_MS_PrefetchMessage(ams_GetAMS(), dir, id, 1);
    }
    ams_WaitCursor(FALSE);
    return(0);
}

int captions__GetBodyFromCUID(ci, cuid, Mode, ContentTypeOverride)
struct captions *ci;
int cuid, Mode;
char *ContentTypeOverride;
{
    struct text *d;
    struct t822view *bv;
    char FileName[1+MAXPATHLEN], ErrorText[500];
    int ShouldDelete, IgnorePos, len;
    FILE *fp;
    struct im *im;

    d = captions_GetBodDoc(ci);
    bv = captions_GetBodView(ci);
    text_ClearCompletely(d);
    text822_ResetGlobalStyle(d);
    im = t822view_GetIM(bv);
    if (im && im_GetInputFocus(im) == NULL) t822view_WantInputFocus(bv, bv);
    if (ams_CUI_ReallyGetBodyToLocalFile(ams_GetAMS(), cuid, FileName, &ShouldDelete, !ams_CUI_SnapIsRunning(ams_GetAMS()))
    || ams_CUI_GetSnapshotFromCUID(ams_GetAMS(), cuid, ci->VisibleSnapshot)) {
	return(-1); /* Error message already reported */
    }
    fp = fopen(FileName, "r");
    if (!fp) {
	ams_ReportError(ams_GetAMS(), "Cannot read message body file", ERR_WARNING, FALSE, 0);
	return(-1);
    }
    if (!text822_ReadIntoText(d, fp, Mode, ContentTypeOverride, &len, TRUE, &ci->StartOfRealBody, &IgnorePos, NULL)) {
	ams_ReportError(ams_GetAMS(), "Cannot read text822 object", ERR_WARNING, FALSE, 0);
	fclose(fp);
	return(-1);
    }
    fclose(fp);
    ci->CurrentFormatting = Mode;
    if (ShouldDelete) {
	if (unlink(FileName)) {
	    sprintf(ErrorText, "Cannot unlink local file %s (%d)", FileName, errno);
	    ams_ReportError(ams_GetAMS(), ErrorText, ERR_WARNING, FALSE, 0);
	}
    }
    t822view_SetTopPosition(bv, IgnorePos+1);
    t822view_SetDotPosition(bv, IgnorePos+1);
    t822view_SetDotLength(bv, 0);

    text_SetReadOnly(d, TRUE);
    MarkVisibleMessageSeen(ci);
    t822view_WantUpdate(bv, bv);
    captions_WantUpdate(ci, ci);
    im = t822view_GetIM(bv);
#ifdef NOWAYJOSE
    if (im) {
	im_ExposeWindow(im);
    }
#endif /* NOWAYJOSE */
    im_ForceUpdate();
    ams_SubtleDialogs(ams_GetAMS(), TRUE);
    ams_CUI_ProcessMessageAttributes(ams_GetAMS(), cuid, ci->VisibleSnapshot);
    ams_SubtleDialogs(ams_GetAMS(), FALSE);
    return (0);
}


void GetInfo(ci, total, seen, dot)
struct captions *ci;
struct range *total, *seen, *dot;
{
    int pos, mylen, whichcaption, gap, mystart;
    struct environment *envptr;

    ci->textscrollinterface->GetInfo(ci, total, seen, dot);
    if (ci->captioncachecount <= 0) return;
    gap = ci->FolderSize - ci->FetchedFromEnd - ci->FetchedFromStart;
    total->beg = 0;
    total->end = ci->FolderSize;
    pos = seen->beg;
    captions_FindCUIDByDocLocation(ci, &pos, &mylen, &envptr, &whichcaption);
    if (pos < 0) {
	total->beg = total->end = 0;
	seen->beg = seen->end = 0;
	dot->beg = dot->end = 0;
	return;
    }
    mystart = (whichcaption >= ci->FetchedFromStart) ? (whichcaption + gap) : whichcaption;
    pos = seen->end - 1;
    captions_FindCUIDByDocLocation(ci, &pos, &mylen, &envptr, &whichcaption);
    seen->beg = mystart;
    seen->end = ((whichcaption >= ci->FetchedFromStart) ? (whichcaption + gap) : whichcaption) + 1;
    pos = ci->HighlightStartPos;
    captions_FindCUIDByDocLocation(ci, &pos, &mylen, &envptr, &whichcaption);
    dot->beg = (whichcaption >= ci->FetchedFromStart) ? (whichcaption + gap) : whichcaption;
    dot->end = dot->beg;
}

static void SetFrame(ci, p, n, d)
struct captions *ci;
long p, n, d;
{
    int min, max, w;

    if (ci->captioncachecount <= 0) {
	ci->textscrollinterface->SetFrame(ci, p, n, d);
	return;
    }
    w = (n * captions_GetLogicalHeight(ci)) / d;
    /* The following conservatively assume a 6 pt font, 4 pts spacing */
    min = p - (w / 10);
    max = p + ((captions_GetLogicalHeight(ci) - w) / 10);
    if (min < 0) min = 0;
    if (max > ci->FolderSize) max = ci->FolderSize;
    if (captions_GuaranteeFetchedRange(ci, min, max)) return; /* error reported */

    if (p >= ci->FolderSize) p = ci->FolderSize - 1;
    if (p >= ci->FetchedFromStart) {
	p -= ci->FolderSize - ci->FetchedFromEnd - ci->FetchedFromStart;
    }
    if (p >= 0) {
	ci->textscrollinterface->SetFrame(ci, ci->capcache[p].offset, n, d);
    } else {
	ci->textscrollinterface->SetFrame(ci, 0, n, d);
    }
}

static long WhatIsAt(ci, n, d)
struct captions *ci;
long n,d;
{
    int pos, len, whichcaption;
    struct environment *envptr;

    pos = ci->textscrollinterface->WhatIsAt(ci, n, d);
    if (ci->captioncachecount > 0) {
	captions_FindCUIDByDocLocation(ci, &pos, &len, &envptr, &whichcaption);
	pos = (whichcaption >= ci->FetchedFromStart) ? (whichcaption + ci->FolderSize - ci->FetchedFromEnd - ci->FetchedFromStart) : whichcaption;
    }
    return(pos);
}

static struct scrollfns scrollInterface = {GetInfo, SetFrame, NULL, WhatIsAt};

char * captions__GetInterface(self, interfaceName) 
struct captions *self;
char *interfaceName;
{
    if (strcmp(interfaceName, "scroll,vertical") == 0)
        return (char *) &scrollInterface;
    return NULL;
}

void captions__ResetVisibleCaption(h)
struct captions *h;
{
    struct t822view *bv = captions_GetBodView(h);

    ResetCaptionNotBody(h);
    text_ClearCompletely(captions_GetBodDoc(h));
    text822_ResetGlobalStyle(captions_GetBodDoc(h));
    if (t822view_GetIM(bv) && im_GetInputFocus(t822view_GetIM(bv)) == NULL) t822view_WantInputFocus(bv, bv);
    t822view_SetDotPosition(bv, 0);
    t822view_SetDotLength(bv, 0);
    t822view_FrameDot(bv, 0);
    t822view_WantUpdate(bv, bv);
}

ResetCaptionNotBody(ci)
struct captions *ci;
{
    RemoveHighlighting(ci);
    ci->VisibleCUID = -1;
    captions_WantUpdate(ci, ci);
    captions_PostMenus(ci, NULL);
}

RemoveHighlighting(h) 
struct captions *h;
{
    if (h->HighlightEnv) {
	text_SetEnvironmentStyle(h->CaptText, h->HighlightEnv, AMS_GET_ATTRIBUTE(h->VisibleSnapshot, AMS_ATT_DELETED) ? h->DeletedStyle : h->NormalCaptionStyle);
	h->HighlightStartPos = 0;
	h->HighlightLen = 0;
	h->HighlightEnv = NULL;
    }
    return(0);
}

int captions__DeleteVisibleMessage(ci, Delete)
struct captions *ci;
Boolean Delete;
{
    int cuid;

    cuid = ci->VisibleCUID;    
    if (cuid < 1 || !ci->HighlightEnv) {
	message_DisplayString(NULL, 10, "There is no message on display.");
	return(0);
    }
    ams_WaitCursor(TRUE);
    if (Delete) {
	if (ams_CUI_DeleteMessage(ams_GetAMS(), cuid) == 0) { /* Everything worked Fine */
	    text_SetEnvironmentStyle(ci->CaptText, ci->HighlightEnv, ci->ActiveDeletedStyle);
	    AMS_SET_ATTRIBUTE(ci->VisibleSnapshot, AMS_ATT_DELETED);
	    captions_WantUpdate(ci, ci);
	    captions_AlterDeletedIcon(ci, ci->HighlightStartPos, TRUE);
	    message_DisplayString(NULL, 10, "Message deleted.");
	} /* The cui routine reported the errors itself */
    } else {
	if (ams_CUI_UndeleteMessage(ams_GetAMS(), cuid) == 0) { /* Everything worked Fine */
	    text_SetEnvironmentStyle(ci->CaptText, ci->HighlightEnv, ci->ActiveCaptionStyle);
	    AMS_UNSET_ATTRIBUTE(ci->VisibleSnapshot, AMS_ATT_DELETED);
	    captions_WantUpdate(ci, ci);
	    captions_AlterDeletedIcon(ci, ci->HighlightStartPos, FALSE);
	    message_DisplayString(NULL, 10, "Message undeleted.");
	} /* The cui routine reported the errors itself */
    }
    captions_PostMenus(ci, NULL);
    ams_WaitCursor(FALSE);
    return(0);
}

void captions__AlterDeletedIcon(ci, pos, Delete)
struct captions *ci;
int pos;
Boolean Delete;
{
    char c;
    char *newicon, ErrorText[256];

    c = text_GetChar(ci->CaptText, ++pos);
    switch(c) {
	case ICON_MAIL:
	case ICON_DELMAIL:
	    newicon = Delete ? SICON_DELMAIL : SICON_MAIL;
	    break;
	case ICON_READMAIL:
	case ICON_READDELMAIL:
	    newicon = Delete ? SICON_READDELMAIL : SICON_READMAIL;
	    break;
	case ICON_POST:
	case ICON_DELPOST:
	    newicon = Delete ? SICON_DELPOST : SICON_POST;
	    break;
	case ICON_READDELPOST:
	case ICON_READPOST:
	    newicon = Delete ? SICON_READDELPOST : SICON_READPOST;
	    break;
	default:
	    newicon = "X";
	    sprintf(ErrorText, "Found '%c' on '%sdelete' where a mail/post icon was expected", c, Delete ? "" : "un");
	    ams_ReportError(ams_GetAMS(), ErrorText, ERR_WARNING, FALSE, 0);
	    break;
    }
    text_AlwaysInsertCharacters(ci->CaptText, pos++, newicon, 1);
    text_AlwaysDeleteCharacters(ci->CaptText, pos, 1);
}

captions__FindCUIDByDocLocation(ci, position, len, envptr, whichcaption)
struct captions *ci;
int *position, *len;
struct environment **envptr;
int *whichcaption;
{
    int top, bottom, split;

    bottom = 0;
    top = ci->captioncachecount -1;
    if (top < 0) {
	*position = *len = *whichcaption = -1;
	*envptr = NULL;
	return(-1);
    }
    split = top/2;
    while (TRUE) {
	if (ci->capcache[split].offset > *position) {
	    top = split;
	} else {
	    bottom = split;
	}
	split = bottom + (top-bottom)/2;
	if (top == bottom) break;
	if (top == bottom + 1) {
	    if (*position < ci->capcache[top].offset) {
		split = bottom;
	    } else {
		split = top;
	    }
	    break;
	}
    }
    *position = ci->capcache[split].offset;
    if (split == ci->captioncachecount -1) {
	*len = text_GetLength(ci->CaptText) - *position;
    } else {
	*len = ci->capcache[split+1].offset - *position;
    }
    *envptr = ci->capcache[split].env;
    *whichcaption = split;
    return(ci->capcache[split].cuid);
}

void captions__ToggleMark(ci, hc,linestart)
struct captions *ci;
struct CaptionCache *hc;
int linestart;
{
    text_AlwaysInsertCharacters(ci->CaptText, linestart+1, hc->IsMarked ? " " : SICON_MARK, 1);
    text_AlwaysDeleteCharacters(ci->CaptText, linestart, 1);
    ci->MarkCount += hc->IsMarked ? -1 : 1;
    hc->IsMarked = hc->IsMarked ? 0 : 1;
    captions_WantUpdate(ci, ci);
}


AddCaptionToCacheEntry(ccache, ct, size, cuid, offset, env, iconenv, MayModify, snapshot, IsDup)
struct CaptionCache **ccache;
int *ct, *size;
int cuid, offset;
struct environment *env, *iconenv;
char *snapshot;
Boolean MayModify, IsDup;
{
    long dumint;
    struct CaptionCache *cc = &((*ccache)[*ct]);

    cc->offset = offset;
    cc->cuid = cuid;
    cc->env = env;
    cc->iconenv = iconenv;
    cc->IsMarked = 0;
    cc->MayModify = MayModify ? 1 : 0;
    cc->IsDup = IsDup ? 1 : 0;
    bcopy(AMS_CHAIN(snapshot), &dumint, sizeof(long));
    cc->Chain = ntohl(dumint);
    strcpy(cc->Date, AMS_DATE(snapshot));
    bcopy(AMS_ATTRIBUTES(snapshot), cc->Attributes, AMS_ATTRIBUTESIZE);
    if (++*ct >= *size) {
	int newsize;

	newsize = *size * 2;
	*ccache = (struct CaptionCache *) realloc(*ccache, newsize * sizeof(struct CaptionCache));
	*size = newsize;
    }
}

MergeTwoCacheEntries(ci, ccache, cct, csize, prefixend)
struct captions *ci;
struct CaptionCache *ccache;
int cct, csize, prefixend;
{
    int totalct, i;

    totalct = ci->captioncachecount + cct;
    if (totalct >= ci->captioncachesize) {
	ci->capcache = (struct CaptionCache *) realloc(ci->capcache, (1+totalct) * sizeof(struct CaptionCache));
	ci->captioncachesize = totalct+1;
    }
    bcopyfromback(ci->capcache, &ci->capcache[cct], ci->captioncachecount * sizeof(struct CaptionCache));
    bcopy(ccache, ci->capcache, cct * sizeof(struct CaptionCache));
    for (i = cct; i<totalct; ++i) {
	ci->capcache[i].offset += prefixend;
    }
    ci->captioncachecount = totalct;
}

int GetSouthernmostPoint(ci)
struct captions *ci;
{
    if (ci->SouthPoint) {
	return(environment_Eval(ci->SouthPoint));
    } else {
	return(0);
    }
}

SetSouthernmostPoint(ci, pos)
struct captions *ci;
int pos;
{
    int i;

    if (pos >= 0 && pos < GetSouthernmostPoint(ci)) return;
    if (ci->SouthPoint) {
	text_SetEnvironmentStyle(ci->CaptText, ci->SouthPoint, ci->IconicStyle);
	ci->SouthPoint = NULL;
    }
    if (pos < 0) return;
    for (i=0; i<ci->captioncachecount; ++i) {
	if (ci->capcache[i].offset > pos) break;
    }
    if (--i < 0) return;
    ci->SouthPoint = ci->capcache[i].iconenv;
    text_SetEnvironmentStyle(ci->CaptText, ci->SouthPoint, ci->UnderlinedIconicStyle);

    captions_WantUpdate(ci, ci);
}


MarkVisibleMessageSeen(ci)
struct captions *ci;
{
    if (AMS_GET_ATTRIBUTE(ci->VisibleSnapshot, AMS_ATT_MAYMODIFY) && AMS_GET_ATTRIBUTE(ci->VisibleSnapshot, AMS_ATT_UNSEEN) && ams_CUI_MarkAsRead(ams_GetAMS(), ci->VisibleCUID)) {
	return(-1); /* error was reported */
    }
    AMS_UNSET_ATTRIBUTE(ci->VisibleSnapshot, AMS_ATT_UNSEEN);
    captions_MarkVisibleMessageStateofSeeing(ci, TRUE);
}


void captions__MarkVisibleMessageStateofSeeing(ci, HasSeen)
struct captions *ci;
Boolean HasSeen;
{
    int c, pos;
    struct text *d;
    char *newicon, ErrorText[256];

    d = ci->CaptText;
    pos = ci->HighlightStartPos + 1;
    c = text_GetChar(d,  pos);
    switch(c) {
	case ICON_MAIL:
	case ICON_READMAIL:
	    newicon = HasSeen ? SICON_READMAIL : SICON_MAIL;
	    break;
	case ICON_READDELMAIL:
	case ICON_DELMAIL:
	    newicon = HasSeen ? SICON_READDELMAIL : SICON_DELMAIL;
	    break;
	case ICON_POST:
	case ICON_READPOST:
	    newicon = HasSeen ? SICON_READPOST : SICON_POST;
	    break;
	case ICON_READDELPOST:
	case ICON_DELPOST:
	    newicon = HasSeen ? SICON_READDELPOST : SICON_DELPOST;
	    break;
	default:
	    newicon = "X";
	    sprintf(ErrorText, "Found '%c' on 'mark %sseen' where a mail/post icon was expected", c, HasSeen ? "" : "un");
	    ams_ReportError(ams_GetAMS(), ErrorText, ERR_WARNING, FALSE, 0);
	    break;
    }
    text_AlwaysInsertCharacters(d, pos++, newicon, 1);
    text_AlwaysDeleteCharacters(d, pos, 1);
    captions_WantUpdate(ci, ci);
    captions_PostMenus(ci, NULL);
    if (!HasSeen) message_DisplayString(NULL, 10, "Marked message as unread");
}


int captions__ShowMore(ci, MayScroll, MayGoOn, InsistOnMark)
struct captions *ci;
Boolean MayScroll, MayGoOn, InsistOnMark;
{
    int whichcaption, pos, len, thisCUID;
    struct environment *env;

    if (captions_GetDotLength(ci) > 0) {
	pos = captions_GetDotPosition(ci);
	thisCUID = captions_FindCUIDByDocLocation(ci, &pos, &len, &env, &whichcaption);
	if (pos > 0) {
	    captions_SetDotPosition(ci, pos - 1);
	}
	captions_SetDotLength(ci, 0);
    }
    if (MayScroll && (len=text_GetLength(captions_GetBodDoc(ci))) > 0 && !t822view_Visible(captions_GetBodView(ci), len)  && (len != t822view_GetTopPosition(captions_GetBodView(ci)))) {
	/* Scroll it forward */
	NextTextviewScreen(captions_GetBodView(ci));
	return(0);
    } else {
	/* Go on to the next one */
	if (ci->HighlightLen > 0) {
	    pos = ci->HighlightStartPos;
	} else {
	    pos = captions_GetDotPosition(ci);
	}
	if (pos <= 0 && ci->HighlightLen <= 0) {
	    whichcaption = -1;
	} else {
	    thisCUID = captions_FindCUIDByDocLocation(ci, &pos, &len, &env, &whichcaption);
	}
	++whichcaption;
	while (whichcaption < ci->captioncachecount && ci->capcache[whichcaption].IsDup) {
	    ++whichcaption;
	}
	if (InsistOnMark) {
	    while (whichcaption < ci->captioncachecount && !ci->capcache[whichcaption].IsMarked) {
		++whichcaption;
	    }
	}
	if (whichcaption >= ci->captioncachecount) {
	    if (MayGoOn) {
		/* Have to set southernmost point in case skipped duplicates */
		if (ci->captioncachecount > 0) {
		    SetSouthernmostPoint(ci, ci->capcache[ci->captioncachecount-1].offset);
		}
		folders_NextFolder(captions_GetFolders(ci), TRUE);
	    } else {
		if (ci->captioncachecount > 0) {
		    message_DisplayString(NULL, 10, "You are at the end of the captions");
		}
		/* Else we assume another error is displayed already... */
	    }
	} else {
		pos = ci->capcache[whichcaption].offset;
		captions_SetDotPosition(ci, pos);
		thisCUID = captions_FindCUIDByDocLocation(ci, &pos, &len, &env, &whichcaption);
		captions_DisplayNewBody(ci, thisCUID, pos, len, env);
		return(0);
	}
    }
}


int captions__InsertUpdatesInDocument(ci, shortname, dname, ShowFirst)
struct captions *ci;
char *shortname, *dname;
Boolean ShowFirst;
{
    int code, substatus;
    long errcode;
    char DumBuf[1+MAXPATHLEN];
    boolean HadFullName = TRUE;

    ams_WaitCursor(TRUE);
    captions_ClearAndUpdate(ci, FALSE, TRUE);
    if (!dname || !*dname) {
	HadFullName = FALSE;
	errcode = ams_CUI_DisambiguateDir(ams_GetAMS(), shortname, &dname);
	if (errcode) {
	    if (ams_vdown(ams_GetAMS(), ams_AMS_ERRNO(ams_GetAMS()))) {
		sprintf(DumBuf, "%s: temporarily unavailable (net/server problem)", shortname);
	    } else if (ams_AMS_ERRNO(ams_GetAMS()) == EACCES) {
		sprintf(DumBuf, "'%s' is private; you don't have read-access or are unauthenticated.", shortname);
	    } else {
		sprintf(DumBuf, "The folder %s is not readable.", shortname);
	    }
	    message_DisplayString(NULL, 75, DumBuf);
	    return(-1);
	}
    }
    errcode = ams_MS_GetSubscriptionEntry(ams_GetAMS(), dname, DumBuf, &substatus);
    if (errcode) {
	ams_WaitCursor(FALSE);
	ams_ReportError(ams_GetAMS(), "Could not get subscription entry", ERR_WARNING, TRUE, errcode);
	return(-1);
    }
    if (!HadFullName && ci->myfold) {
	folders_AlterSubscriptionStatus(ci->myfold, dname, substatus, shortname);
    }
    if (substatus == AMS_PRINTSUBSCRIBED) {
	ams_CUI_PrintUpdates(ams_GetAMS(), dname, shortname);
    }
    ci->CommentText = NULL;
    ci->firstcuid = -1;
    ci->FullName = malloc(1+strlen(dname));
    if (ci->FullName) strcpy(ci->FullName, dname);
    ci->ShortName = malloc(1+strlen(shortname));
    if (ci->ShortName) strcpy(ci->ShortName, shortname);
    ci->substatus = substatus;
    captions_SetLabel(ci, shortname);
    code = captions_InsertCaptions(ci, shortname, dname, NULL, substatus == AMS_SHOWALLSUBSCRIBED ? TRUE : FALSE);
    if (ci->CommentText) {
	folders_HighlightFolder(captions_GetFolders(ci), dname, ci->CommentText); 
	free(ci->CommentText);
	ci->CommentText = NULL;
    }

    im_ForceUpdate();
    if (ShowFirst) {
	if (substatus == AMS_SHOWALLSUBSCRIBED) {
	    int len, pos;

	    pos = ci->capcache[0].offset;
	    if (ci->captioncachecount > 1) {
		len = ci->capcache[1].offset - pos;
	    } else if (ci->captioncachecount > 0) {
		len = text_GetLength(ci->CaptText) - pos;
	    } else {
		len = 0;
	    }
	    if (len) captions_DisplayNewBody(ci, ci->capcache[0].cuid, pos, len, ci->capcache[0].env);
	} else {
	    captions_ShowMore(ci, FALSE, FALSE, FALSE);
	}
    } else {
	/* Pre-fetch the first message */
	if (ci->captioncachecount > 1) {
	    ams_CUI_PrefetchMessage(ams_GetAMS(), ci->capcache[1].cuid, 0);
	} else if (ci->captioncachecount > 0) {
	    ams_CUI_PrefetchMessage(ams_GetAMS(), ci->capcache[0].cuid, 0);
	}
    }
    captions_WantInputFocus(ci, ci);
    ams_WaitCursor(FALSE);
    return(code);
}

captions_InsertCaptions(ci, shortname, dname, StartTime, ShowAll)
struct captions *ci;
char *shortname, *dname, *StartTime;
Boolean ShowAll;
{
    int totalbytes, numbytes, cuid, status, addlen, highstart, highlen, envstart, inspos, insertct = 0, IsDup, myfirstcuid = 0;
    char date64[AMS_DATESIZE+1], olddate64[AMS_DATESIZE+1], newdate[AMS_DATESIZE+1], firstdate[AMS_DATESIZE+1], captionbuf[100*AMS_SNAPSHOTSIZE], ErrorText[256], *DirName, *ThisCaption, *s;
    Boolean UseHighDensity, MayModify, IsRead;
    struct environment *et, *et2;
    int PositionDot = 1, NewCt, TotalCt, ProtCode;
    long errcode;
    struct CaptionCache *tempcache = NULL;
    int tempcachesize, tempcachecount;

    if ((errcode = ams_CUI_DisambiguateDir(ams_GetAMS(), dname, &DirName)) != 0) {
	if (*dname == '/' && ams_AMS_ERRNO(ams_GetAMS()) == ENOENT) {
	    ams_CUI_HandleMissingFolder(ams_GetAMS(), dname);
	    return(-1);
	} else if (ams_vdown(ams_GetAMS(), ams_AMS_ERRNO(ams_GetAMS()))) {
	    sprintf(ErrorText, "%s: temporarily unavailable (net/server problem)", shortname);
	} else if (ams_AMS_ERRNO(ams_GetAMS()) == EACCES) {
	    sprintf(ErrorText, "'%s' is private; you don't have read-access or are unauthenticated.", shortname);
	} else {
	    sprintf(ErrorText, "The folder %s is not readable.", shortname);
	}
	message_DisplayString(NULL, 75, ErrorText);
	return(-1);
    }
    UseHighDensity = ! (amsutil_GetOptBit(EXP_WHITESPACE));
    if (!StartTime) {
	long mytime;

	im_ForceUpdate();
	errcode = ams_MS_GetDirInfo(ams_GetAMS(), DirName, &ProtCode, &TotalCt);
	if (errcode) {
	    if (ams_AMS_ERRNO(ams_GetAMS()) == ENOENT) {
		ams_CUI_HandleMissingFolder(ams_GetAMS(), dname);
		return(-1);
	    }
	    sprintf(ErrorText, "Cannot look up information about %s", DirName);
	    ams_ReportError(ams_GetAMS(), ErrorText, ERR_WARNING, TRUE, errcode);
	    return(-1);
	}
	ci->IsFullMail =  (ProtCode == AMS_DIRPROT_FULLMAIL) ? TRUE : FALSE;
	ci->FolderSize = TotalCt;
	if (errcode = ams_MS_GetNewMessageCount(ams_GetAMS(), DirName, &NewCt, &TotalCt, olddate64, TRUE)) {
	    sprintf(ErrorText, "Couldn't get date information for %s", DirName);
	    ams_ReportError(ams_GetAMS(), ErrorText, ERR_CRITICAL, TRUE, errcode);
	    return(-1);
	}
	if (NewCt == TotalCt) ShowAll = TRUE;
	if (ShowAll) {
	    date64[0] = '\0';
	    NewCt = TotalCt;
	} else {
	    mytime = amsutil_conv64tolong(olddate64);
	    if (mytime>0) {
		strcpy(date64, amsutil_convlongto64(mytime-1, 0));
	    }
	}
	inspos = insertct;
   } else {
	strcpy(date64, StartTime);
    }
    insertct = 0;
    inspos = 0;
    newdate[0] = '\0';
    totalbytes = 0;
    s = NULL;
    if (StartTime) {
	tempcache = (struct CaptionCache *) malloc(25 * sizeof(struct CaptionCache));
	tempcachesize = 25;
	tempcachecount = 0;
    }
    do {
	if ((errcode = ams_CUI_GetHeaders(ams_GetAMS(), DirName, date64, captionbuf, sizeof(captionbuf), totalbytes, &numbytes, &status, FALSE)) != 0) {
	    captions_WantUpdate(ci, ci);
	    if (ams_vdown(ams_GetAMS(), ams_AMS_ERRNO(ams_GetAMS()))) {
		sprintf(ErrorText, "%s: temporarily unavailable (net/server problem)", DirName);
		ams_ReportSuccess(ams_GetAMS(), ErrorText);
	    } else {
	        sprintf(ErrorText, "Could not get captions for %s", dname);
		ams_ReportError(ams_GetAMS(), ErrorText, ERR_WARNING, FALSE, 0);
	    }
	    if (tempcache) free(tempcache);
	    return(-1);
	}
	if (numbytes <= 0) break;
	totalbytes += numbytes;
	for (s=captionbuf; s-captionbuf  < numbytes; s+= AMS_SNAPSHOTSIZE) {
	    cuid = ams_CUI_GetCuid(ams_GetAMS(), AMS_ID(s), DirName, &IsDup);
	    if (ci->firstcuid == cuid) {
		/* All done -- we are back to where we started */
		status = 0;
		break;
	    }
	    MayModify = AMS_GET_ATTRIBUTE(s, AMS_ATT_MAYMODIFY) ? 1 : 0;
	    if (MayModify) {
		IsRead = AMS_GET_ATTRIBUTE(s, AMS_ATT_UNSEEN) ? FALSE : TRUE;
	    } else {
		IsRead = (!StartTime && (myfirstcuid || ShowAll)) ? FALSE : TRUE;
	    }
	    if (!myfirstcuid) {
		myfirstcuid = cuid;
		strcpy(firstdate, AMS_DATE(s));
	    }
	    MakeCaptionLine(&ThisCaption, cuid, s, amsutil_GetOptBit(EXP_FIXCAPTIONS), &highstart, &highlen, ci->IsFullMail, IsDup, IsRead);
	    if (!UseHighDensity) {
		strcat(ThisCaption, "\n");
	    }
	    addlen = strlen(ThisCaption);
	    text_AlwaysInsertCharacters(ci->CaptText, inspos, ThisCaption, addlen);
	    if (!StartTime && PositionDot) {
		captions_SetDotPosition(ci, inspos+addlen-1);
		captions_FrameDot(ci, inspos);
		PositionDot = 0;
	    }
	    insertct += addlen;
	    et2 = environment_InsertStyle(ci->CaptText->rootEnvironment, inspos, ci->IconicStyle, 1);
	    environment_SetLength(et2, 3);
	    envstart = inspos + 2;
	    et = environment_InsertStyle(ci->CaptText->rootEnvironment, envstart, AMS_GET_ATTRIBUTE(s, AMS_ATT_DELETED) ? ci->DeletedStyle : ci->NormalCaptionStyle, 1);
	    environment_SetLength(et, addlen -3);
	    if (StartTime) {
		AddCaptionToCacheEntry(&tempcache, &tempcachecount, &tempcachesize, cuid, inspos, et, et2, MayModify, s, IsDup);
	    } else {
		AddCaptionToCacheEntry(&ci->capcache, &ci->captioncachecount, &ci->captioncachesize, cuid, inspos, et, et2, MayModify, s, IsDup);
	    }
	    ++ci->FetchedFromEnd; /* sometimes from start? */
	    if (ci->IsFullMail) {
		et = environment_InsertStyle(et, 0, ci->MailStyle, 1);
		environment_SetLength(et, addlen - 3); 
	    }
	    if (highlen) {
		et = environment_InsertStyle(et, inspos+highstart - envstart, ci->HighlightStyle, 1);
		environment_SetLength(et, highlen);
	    }
	    inspos += addlen;
	}
    } while (status > 0);
    if (myfirstcuid) ci->firstcuid = myfirstcuid;
    if (GetSouthernmostPoint(ci) == 0) { /* No southernmost point yet */
	int newsouth;

	if ((ci->substatus == AMS_SHOWALLSUBSCRIBED)
	    || ShowAll
	    || (ci->captioncachecount <= 0)
	    || (!StartTime && myfirstcuid && (strcmp(firstdate, olddate64)< 0))) {
	    newsouth = -1;
	    captions_SetDotPosition(ci, 0);
	} else {
	    newsouth = ci->capcache[0].offset;
	}
	SetSouthernmostPoint(ci, newsouth);
    }
    if (status < 0) {
	sprintf(ErrorText, "Couldn't read all of the captions successfully");
	ams_ReportError(ams_GetAMS(), ErrorText, ERR_CRITICAL, TRUE, errcode);
	if (tempcache) free(tempcache);
	return(-1);
    }
    if (ci->firstcuid) {
	if (s && !StartTime) {
	    strcpy(newdate, AMS_DATE(s - AMS_SNAPSHOTSIZE));
	}
    }
    captions_WantUpdate(ci, ci);
    if (StartTime) {
	if (tempcache) {
	    MergeTwoCacheEntries(ci, tempcache, tempcachecount, tempcachesize, inspos);
	    free(tempcache);
	}
    } else {
	char CommText[500];

	if (ci->substatus == AMS_SHOWALLSUBSCRIBED) {
	    sprintf(CommText, " (%s; All %d shown) ", ams_DescribeProt(ams_GetAMS(), ProtCode), TotalCt);
	} else {
	    sprintf(CommText, " (%s; %d new of %d) ", ams_DescribeProt(ams_GetAMS(), ProtCode), NewCt, TotalCt);
	}
	ci->CommentText = malloc(1+strlen(CommText));
	if (ci->CommentText) {
	    strcpy(ci->CommentText, CommText);
	}
    }
    return(0);
}


void captions__ClearAndUpdate(ci, ConsiderPurging, SaveState) 
struct captions *ci;
int ConsiderPurging, SaveState;
{
    struct text *d;

    d = ci->CaptText;
    if (SaveState) captions_MakeCachedUpdates(ci);
    if (ConsiderPurging
	 && ams_CUI_DoesDirNeedPurging(ams_GetAMS(), ci->FullName)
	 && (amsutil_GetOptBit(EXP_PURGEONQUIT)
	     || ams_GetBooleanFromUser(ams_GetAMS(), "Do you want to purge the deleted messages", FALSE))) {
	captions_PurgeDeletions(ci);
    }
    ResetCaptionNotBody(ci);
    if (ci->MarkCount > 0) {
	/* We clear the marks this way so that they can be restored later if so desired.  */
	captions_ClearMarks(ci);
	ci->MarkCount = 0;
    }
    if (ci->FullName) {
	free(ci->FullName);
	ci->FullName = NULL;
    }
    if (ci->ShortName) {
	free(ci->ShortName);
	ci->ShortName = NULL;
    }
    ci->captioncachecount = 0;
    ci->FetchedFromStart = ci->FetchedFromEnd = ci->FolderSize = 0;
    SetSouthernmostPoint(ci, -1);
    text_ClearCompletely(d);
    text_SetGlobalStyle(d, ci->GlobalCapStyle);
    captions_SetDotPosition(ci, 0);
    captions_SetDotLength(ci, 0);
    captions_PostMenus(ci, NULL);
    captions_WantUpdate(ci, ci);
    im_ForceUpdate();
}

void captions__MakeCachedUpdates(ci) 
struct captions *ci;
{
    char ErrorText[256], UpdateDate[AMS_DATESIZE];
    int south, j;
    long mserrcode;

    ams_TryDelayedUpdates();
    if (!ci->FullName) return;
    south = GetSouthernmostPoint(ci);
    if (south <= 0 && (ci->FolderSize > (ci->FetchedFromEnd + ci->FetchedFromStart))) {
	captions_GuaranteeFetchedRange(ci, ci->FolderSize - ci->FetchedFromEnd - 1, ci->FolderSize);
	south = GetSouthernmostPoint(ci);
    }
    for(j=0; j<ci->captioncachecount; ++j) {
	if (ci->capcache[j].offset > south) break;
    }
    if (j<=0) {
	folders_SetSkip(captions_GetFolders(ci), ci->FullName, TRUE);
	return;
    }
    strcpy(UpdateDate, ci->capcache[--j].Date);
    mserrcode = ams_MS_SetAssociatedTime(ams_GetAMS(), ci->FullName, UpdateDate);
    if (mserrcode) {
	if (ams_AMS_ERRNO(ams_GetAMS()) == ENOENT) {
	    sprintf(ErrorText, "Folder %s has recently been deleted -- profile not set", ci->ShortName);
	    ams_ReportError(ams_GetAMS(), ErrorText, ERR_WARNING, FALSE, mserrcode);
	} else if (ams_vdown(ams_GetAMS(), ams_AMS_ERRNO(ams_GetAMS())) || (ams_AMS_ERRNO(ams_GetAMS()) == EWOULDBLOCK)) {
	    sprintf(ErrorText, "Could not set profile for %s; will try again later.", ci->ShortName);
	    ams_ReportSuccess(ams_GetAMS(), ErrorText);
	    ams_CacheDelayedUpdate(ci->FullName, UpdateDate);
	} else {
	    sprintf(ErrorText, "Could not set profile for %s (%s, %d, %d, %d)", ci->ShortName, ci->FullName, ams_AMS_ERRNO(ams_GetAMS()), AMS_ERRCAUSE, AMS_ERRVIA);
	    /* UGH!  Above line only works because we've defined mserrcode locally to be the right thing... */
	    ams_ReportError(ams_GetAMS(), ErrorText, ERR_WARNING, TRUE, mserrcode);
	}
	return;
    }
    if (j >= (ci->captioncachecount-1)) {
	folders_SetSkip(captions_GetFolders(ci), ci->FullName, TRUE);
    }
}

captions_PurgeDeletions(ci)
struct captions *ci;
{
    message_DisplayString(NULL, 10, "Purging deletions; please wait...");
    im_ForceUpdate();
    ams_WaitCursor(TRUE);
    if (ams_CUI_PurgeDeletions(ams_GetAMS(), ci->FullName) == 0) {
	message_DisplayString(NULL, 10, "Purging complete.");
    }
    ams_WaitCursor(FALSE);
}

GetCUIDFromPosition(ci, pos)
struct captions *ci;
int pos;
{
    int j;

    for(j=0; j<ci->captioncachecount; ++j) {
	if (ci->capcache[j].offset > pos) break;
    }
    if (j<=0) {
	return(-1);
    } else {
	return(ci->capcache[j].cuid);
    }
}

GetPositionFromCUID(ci, cuid)
struct captions *ci;
int cuid;
{
    int j;

    for(j=0; j<ci->captioncachecount; ++j) {
	if (ci->capcache[j].cuid == cuid) return(ci->capcache[j].offset);
    }
    return(-1);
}


captions__GuaranteeFetchedRange(ci, min, max)
struct captions *ci;
int min, max;
{
    int frontgap, backgap, oldsize, olddot, added, oldtop, south, southcuid, dotcuid, topcuid, pos;
    char TimeBuf[AMS_DATESIZE+1], SBuf[AMS_SNAPSHOTSIZE], ErrorText[500];
    long errcode;
    int RetryCount = 0;
#define RETRY_MAX 5 /* Maximum number of times to believe the folder is changing underneath me in rapid sequence. */

    if (ci->FolderSize <= 0) return(0);
    if (min < 0) min = 0;
restart:
    frontgap = max - ci->FetchedFromStart;
    backgap = (ci->FolderSize - min) - ci->FetchedFromEnd;
    if (frontgap < 0 || backgap < 0) return(0); /* already there */
    oldtop = captions_GetTopPosition(ci);
    ams_WaitCursor(TRUE);
    if (errcode = ams_MS_GetNthSnapshot(ams_GetAMS(), ci->FullName, min, SBuf)) {
	if ((RetryCount < RETRY_MAX) && (ams_AMS_ERRNO(ams_GetAMS()) == EINVAL)) {
	    /* This is the case where you have had a folder purged underneath you! */
	    char nickname[1+MAXPATHLEN], fullname[1+MAXPATHLEN];

	    strcpy(nickname, ci->ShortName);
	    strcpy(fullname, ci->FullName);
	    southcuid = GetCUIDFromPosition(ci, GetSouthernmostPoint(ci));
	    topcuid = GetCUIDFromPosition(ci, captions_GetTopPosition(ci));
	    dotcuid = GetCUIDFromPosition(ci, captions_GetDotPosition(ci));
	    captions_ClearAndUpdate(ci, FALSE, FALSE); /* Both false to inhibit recursion into GuaranteeFetchedRange again */
	    captions_InsertUpdatesInDocument(ci, nickname, fullname, FALSE);
	    RetryCount++;
	    goto restart;
	} else {
	    sprintf(ErrorText, "Could not get %dth snapshot in folder %s (size %d)", min, ci->FullName, ci->FolderSize);
	    ams_ReportError(ams_GetAMS(), ErrorText, ERR_CRITICAL, TRUE, errcode);
	    ams_WaitCursor(FALSE);
	    return(-1);
	}
    }
    if (strcmp(AMS_DATE(SBuf), "zzzzzz")) {
	strcpy(TimeBuf, amsutil_convlongto64(amsutil_conv64tolong(AMS_DATE(SBuf)) -1, 0));
    } else {
	if (min <= 0) {
	    sprintf(ErrorText, "Bad first date for %s; probably needs reconstruction.", ci->ShortName);
	} else {
	    sprintf(ErrorText, "Bad %dth date for %s; probably needs reconstruction.", min, ci->ShortName);
	}
	ams_ReportError(ams_GetAMS(), ErrorText, ERR_WARNING, FALSE, 0);
	strcpy(TimeBuf, "000000");
    }
    oldsize = text_GetLength(ci->CaptText);
    olddot = captions_GetDotPosition(ci);
    south = GetSouthernmostPoint(ci);
    captions_InsertCaptions(ci, ci->ShortName, ci->FullName, TimeBuf, FALSE);
    added = text_GetLength(ci->CaptText) - oldsize;
    captions_SetDotPosition(ci, olddot + added-1);
    captions_SetTopPosition(ci, oldtop + added);
    if (ci->HighlightLen > 0) {
	ci->HighlightStartPos += added;
    }
    SetSouthernmostPoint(ci, south+added);
    if (RetryCount > 0) {
	pos = GetPositionFromCUID(ci, southcuid);
	if (pos >= 0) SetSouthernmostPoint(ci, pos);
	pos = GetPositionFromCUID(ci, dotcuid);
	if (pos >= 0) captions_SetDotPosition(ci, pos);
	pos = GetPositionFromCUID(ci, topcuid);
	if (pos >= 0) captions_SetTopPosition(ci, pos);
    }
    ams_WaitCursor(FALSE);
    return (0);
}

void captions__FileCurrent(ci, FullName, nickname)
struct captions *ci;
char *FullName, *nickname;
{
    int	    cuid, OpCode;
    Boolean DoAppend = FALSE;

    if (*FullName == '*') {
	++FullName;
	DoAppend = TRUE;
    }
    cuid = ci->VisibleCUID;
    if (cuid < 1) {
	message_DisplayString(NULL, 10, "There is nothing to classify.");
	return;
    }
    ams_WaitCursor(TRUE);
    if (DoAppend) {
	OpCode = AMS_GET_ATTRIBUTE(ci->VisibleSnapshot, AMS_ATT_MAYMODIFY) ? MS_CLONE_APPENDDEL : MS_CLONE_APPEND;
    } else {
	OpCode = AMS_GET_ATTRIBUTE(ci->VisibleSnapshot, AMS_ATT_MAYMODIFY) ? MS_CLONE_COPYDEL : MS_CLONE_COPY;
    }
    if (!ams_CUI_CloneMessage(ams_GetAMS(), cuid, FullName, OpCode)) {
	/* cuilib reports errors, here we deal with success */
	if (OpCode == MS_CLONE_COPYDEL || OpCode == MS_CLONE_APPENDDEL) {
	    if (ci->HighlightEnv) text_SetEnvironmentStyle(ci->CaptText, ci->HighlightEnv, ci->ActiveDeletedStyle);
	    AMS_SET_ATTRIBUTE(ci->VisibleSnapshot, AMS_ATT_DELETED);
	    captions_PostMenus(ci, NULL);
	}
	captions_WantUpdate(ci, ci);
    }
    ams_WaitCursor(FALSE);
    return;
}

void captions__FileMarked(ci, FullName, nickname)
struct captions *ci;
char *FullName, *nickname;
{
    ClassifyMarkedByName(ci, FullName);
    return;
}

void captions__AlterPrimaryFolderName(ci, addname, delname)
struct captions *ci;
char *addname, *delname;
{
    char Nick[1+MAXPATHLEN], *s;

    if (strcmp(delname, ci->FullName)) return;

    if (addname) {
	s = malloc(1+strlen(addname));
	if (!s) return;
	free (ci->FullName);
	ci->FullName = s; 
	strcpy(ci->FullName, addname);

	ams_CUI_BuildNickName(ams_GetAMS(), addname, Nick);
	s = malloc(1+strlen(Nick));
	if (!s) return;
	free (ci->ShortName);
	ci->ShortName = s;
	strcpy(ci->ShortName, Nick);
    } else {
	/* Just Deleting the sucker on display */
	captions_ClearAndUpdate(ci, FALSE, FALSE);
    }
}

void captions__FullUpdate(self, type, left, top, width, height)
struct captions *self;
enum view_UpdateType type;
long left, top, width, height;
{
    struct rectangle Rect;

    captions_GetLogicalBounds(self, &Rect);
    super_FullUpdate(self, type, left, top, width, height);
    if (!self->mycursor) {
	CreateCaptionsCursor(self);
    }
    captions_PostCursor(self, &Rect, self->mycursor);
}

void captions__ReportMarkedMessageCount(captions)
struct captions *captions;
{
    char ErrorText[100];
    if (captions->MarkCount <=0) {
	strcpy(ErrorText, "There are now no messages marked.");
    } else if (captions->MarkCount == 1) {
	strcpy(ErrorText, "There is now one message marked.");
    } else {
	sprintf(ErrorText, "There are now %s messages marked.", amsutil_cvEng(captions->MarkCount, 0, 1000));
    }
    message_DisplayString(NULL, 10, ErrorText);
}

void CapGlitchDown(self)
struct captions *self;
{
    int backupto = self->FolderSize - self->FetchedFromEnd -2;
    struct im *im = captions_GetIM(self);
    
    if (im && im_ArgProvided(im)) {
	backupto -= im_Argument(im);
    }
    if (captions_GuaranteeFetchedRange(self, backupto, self->FolderSize)) return;
    im_ForceUpdate();
    textview_GlitchDownCmd((struct textview *) self);
}

void PreviousCaptionLine(self)
struct captions *self;
{
    int backupto = self->FolderSize - self->FetchedFromEnd -2;
    struct im *im = captions_GetIM(self);

    if (im && im_ArgProvided(im)) {
	backupto -= im_Argument(im);
    }
    if (captions_GetDotPosition(self) < 150) { /* poor heuristic */
	if (captions_GuaranteeFetchedRange(self, backupto, self->FolderSize)) return; /* error reported */
    }
    textview_PreviousLineCmd((struct textview *) self);
}

void captions__CapReverseSearch(self)
struct captions *self;
{
    captions_GuaranteeFetchedRange(self, 0, self->FolderSize);
    im_ForceUpdate();
    textview_ReverseSearchCmd((struct textview *) self);
    captions_WantInputFocus(self, self);
}

void CapScrollBack(self)
struct captions *self;
{
    int min, pos, mylen, whichcaption;
    struct range total, seen, dot;
    struct environment *envptr;

    self->textscrollinterface->GetInfo(self, &total, &seen, &dot);
    pos = seen.beg;
    /* The following conservatively assume a 6 pt font, 4 pts spacing, maximum of one screen of scrolling */
    captions_FindCUIDByDocLocation(self, &pos, &mylen, &envptr, &whichcaption);
    min = self->FolderSize - self->FetchedFromEnd - (captions_GetLogicalHeight(self)/10) + whichcaption - 1;
    if (min < 0) min = 0;
    captions_GuaranteeFetchedRange(self, min, self->FolderSize);
    im_ForceUpdate();
    textview_ScrollScreenBackCmd((struct textview *) self);
}

void CapBeginText(self)
struct captions *self;
{
    captions_GuaranteeFetchedRange(self, 0, self->FolderSize);
    im_ForceUpdate();
    textview_BeginningOfTextCmd((struct textview *) self);
}

boolean captions__InitializeClass(classID) 
struct classheader *classID;
{
    class_Load("textview"); /* make sure the textview is loaded first */
    OneTimeInitKeyMenus(&captions_classinfo);
    return(TRUE);
}

NextTextviewScreen(tv)
struct textview *tv;
{
    textview_ScrollScreenFwdCmd(tv);
}
RSearchTextview(tv)
struct textview *tv;
{
    textview_ReverseSearchCmd(tv);
}

/* This is the same as bcopy, but copies from the back to be safe when you're 
    really just extending an array by pushing back elements */

bcopyfromback(from, to, length)
char *from, *to;
int length;
{
    register char *f, *t;

    f = from + length-1;
    t = to+length-1;
    while (f >=from) {
	*t-- = *f--;
    }
}


/* The next two used to be defines, but things are getting tight again... */
static int PADTOCOLUMNA = 11;
static int PADTOCOLUMNB = 45;
static char *LOTSASPACE="                                                                ";

/* The next call returns a pointer to a static area, overwritten
	on each call */
static char CaptionBuf[AMS_SNAPSHOTSIZE];

MakeCaptionLine(Buf, cuid, RawSnapshot, Fixed, HighStart, HighLen, IsMail, IsDup, IsRead)
char **Buf, *RawSnapshot;
int cuid, Fixed, *HighStart, *HighLen;
Boolean IsMail, IsDup, IsRead;
{
    char *s, *t, *RawCap;
    int len, len2, IconCode, IconCode2;

    *HighStart = *HighLen = 0;
    if (AMS_GET_ATTRIBUTE(RawSnapshot, AMS_ATT_DELETED)) {
	IconCode = IsMail ? (IsRead ? ICON_READDELMAIL : ICON_DELMAIL) : (IsRead ? ICON_READDELPOST : ICON_DELPOST);
    } else {
	IconCode = IsMail ? (IsRead ? ICON_READMAIL : ICON_MAIL) : (IsRead ? ICON_READPOST : ICON_POST);
    }
    IconCode2 = IsDup ? ICON_DUP : ' ';
    RawCap = AMS_CAPTION(RawSnapshot);
    *Buf = CaptionBuf;
    s = index(RawCap, '\t');
/* IF BOGUS ATK tabbing is fixed, the following line should be all we ever need. */
    if (!s || ((s-RawCap)>PADTOCOLUMNA)) {
	sprintf(CaptionBuf, " %c%c%s\n", IconCode, IconCode2, RawCap);
	return(0);
    }
    *s = '\0';
    sprintf(CaptionBuf, " %c%c%s ", IconCode, IconCode2, RawCap);
    if (Fixed) {
	len2 = strlen(CaptionBuf);
	len = PADTOCOLUMNA - len2;
	if (len > 0) {
	    strncat(CaptionBuf, LOTSASPACE, len);
	    CaptionBuf[len+len2] = '\0';
	}
    } else { /* This is bogus -- can NOT get tabbing right in ATK */
	strcat(CaptionBuf, " ");
    }
    t = index(++s, '\t');
    if (!t || ((t-s)>PADTOCOLUMNB)) {
	strcat(CaptionBuf, s);
	strcat(CaptionBuf, "\n");
	return(0);
    }
    *t = '\0';

    if (Fixed) {
	strcat(CaptionBuf, s);
	len2 = strlen(CaptionBuf);
	len = PADTOCOLUMNB - len2;
	if (len > 0) {
	    strncat(CaptionBuf, LOTSASPACE, len);
	    CaptionBuf[len+len2] = '\0';
	}
    } else { /* This is bogus -- can NOT get tabbing right in ATK */
	*HighStart = strlen(CaptionBuf);
	strcat(CaptionBuf, s);
	*HighLen = strlen(CaptionBuf) - *HighStart;
	strcat(CaptionBuf, " - ");
    }
    strcat(CaptionBuf, t+1);
    strcat(CaptionBuf, "\n");
    return(0);
}

void captions__SearchAll(ci)
struct captions *ci;
{
    char ShortName[1+MAXPATHLEN], *tp, ErrorText[256];
    struct SearchPattern *Pattern = NULL;
    int pos, numfound, len, orgpos, whichcaption, oldpos;
    struct environment *envdum;
    
    if (message_AskForString(NULL, 50, "Find all occurrences of: ", "", ShortName, sizeof(ShortName))< 0) return;
    captions_GuaranteeFetchedRange(ci, 0, ci->FolderSize);
    tp = search_CompilePattern(ShortName, &Pattern);
    if (tp) {
	message_DisplayString(NULL, 10, tp);
	return;
    }
    if (ci->MarkCount > 0) {
	captions_ClearMarks(ci);
    }
    numfound = 0;
    orgpos = captions_GetDotPosition(ci);
    pos = 0;
    captions_SetDotPosition(ci, 0);
    captions_SetDotLength(ci, 0);
    while ((pos = search_MatchPattern(ci->CaptText, pos, Pattern)) >= 0) {
	oldpos = pos;
	if (captions_FindCUIDByDocLocation(ci, &pos, &len, &envdum, &whichcaption)) {
	    if (numfound++ <= 0) {
		orgpos = pos;
	    }
	    if (!ci->capcache[whichcaption].IsMarked) {
		captions_ToggleMark(ci, &ci->capcache[whichcaption], pos);
	    }
	    pos += len;
	} else {
	    pos = oldpos + 1;
	}
    }
    captions_SetDotPosition(ci, orgpos);
    captions_FrameDot(ci, orgpos);
    captions_PostMenus(ci, NULL);
    sprintf(ErrorText, "Marked %s captions containing '%s'", amsutil_cvEng(numfound, 0, 1000), ShortName);
    message_DisplayString(NULL, 10, ErrorText);
    captions_WantInputFocus(ci, ci);
}

void captions__ThisIsFlorida(self)
struct captions *self;
{
    int pos, len, whichcaption;
    struct environment *env;

    im_ForceUpdate();
    /* The following line makes things work better if this caption is the first one fetched into the caption cache but not the first in the folder */
    captions_GuaranteeFetchedRange(self, self->FolderSize - self->FetchedFromEnd - 1, self->FolderSize);
    pos = self->HighlightStartPos;
    (void) captions_FindCUIDByDocLocation(self, &pos, &len, &env, &whichcaption);
    SetSouthernmostPoint(self, -1); /* Always needed */
    if (--whichcaption >= 0) {
	pos = self->capcache[whichcaption].offset;
	(void) captions_FindCUIDByDocLocation(self, &pos, &len, &env, &whichcaption);
	SetSouthernmostPoint(self, pos);
    }
    im_ForceUpdate();
}

void captions__PuntCurrent(self, GoToNext)
struct captions *self;
Boolean GoToNext;
{
    int loops = 1;
    struct im *im = captions_GetIM(self);
    char Scratch[100+MAXPATHLEN];

    if (im && im_ArgProvided(im)) {
	loops = im_Argument(im);
	if (loops < 0) {
	    loops = 1;
	} else if (loops > 1) {
	    sprintf(Scratch, "Do you really want to punt the next %d folders", loops);
	    if (!ams_GetBooleanFromUser(ams_GetAMS(), Scratch, FALSE)) {
		return;
	    }
	}
    }
    while (loops--) {
	if (self->captioncachecount > 0) {
	    SetSouthernmostPoint(self, self->capcache[self->captioncachecount-1].offset);
	}
	if (GoToNext) {
	    folders_NextFolder(captions_GetFolders(self), FALSE);
	}
	if (loops>0) im_ForceUpdate();
    }
}

void captions__FindRelatedMessages(self)
struct captions *self;
{
    char ErrorText[256];
    long numfound, i, j, mainchain, orgpos;
    struct CaptionCache *cc;
    
    if (self->VisibleCUID <= 0) {
	message_DisplayString(NULL, 10, "There is nothing on display.");
	return;
    }
    captions_GuaranteeFetchedRange(self, 0, self->FolderSize);
    if (self->MarkCount > 0) {
	captions_ClearMarks(self);
    }
    orgpos = captions_GetDotPosition(self);
    numfound = 0;
    bcopy(AMS_CHAIN(self->VisibleSnapshot), &i, sizeof(long));
    mainchain = ntohl(i);
    if (mainchain) {
	for (j = 0; j < self->captioncachecount; ++j) {
	    cc = &self->capcache[j];
	    if (cc->Chain == mainchain) {
		if (numfound++ <= 0) {
		    orgpos = cc->offset;
		}
		captions_ToggleMark(self, cc, cc->offset);
	    }
	}
    }
    captions_SetDotPosition(self, orgpos);
    captions_FrameDot(self, orgpos);
    captions_PostMenus(self, NULL);
    if (numfound) {
	sprintf(ErrorText, "Marked %s related messages.", amsutil_cvEng(numfound, 0, 1000));
    } else {
	strcpy(ErrorText, "There are no related messages.");
    }
    message_DisplayString(NULL, 10, ErrorText);
    captions_WantInputFocus(self, self);
}

void captions__MarkRangeOfMessages(self)
struct captions *self;
{
    char ErrorText[256], Sdate[1+AMS_DATESIZE], Edate[1+AMS_DATESIZE], DBuf[400];
    long numfound, j, orgpos;
    struct CaptionCache *cc;
    long year, month, day, hour, min, sec, wday, gtm;
    Boolean FoundStartDate = FALSE /*, FoundEndDate = FALSE */ ;

    captions_GuaranteeFetchedRange(self, 0, self->FolderSize);
    if (self->MarkCount > 0) {
	captions_ClearMarks(self);
    }
    if (message_AskForString(NULL, 50, "Mark messages since date [beginning of time]: ", "", DBuf, sizeof(DBuf)) < 0) {
	return;
    }
    if (DBuf[0]) {
	long code;
	code = ams_MS_ParseDate(ams_GetAMS(), DBuf, &year, &month, &day, &hour, &min, &sec, &wday, &gtm);
	if (code) {
	    message_DisplayString(NULL, 25, "Sorry; I don't understand the date you entered.");
	    return;
	}
	strcpy(Sdate, amsutil_convlongto64(gtm, 0));
    } else {
	strcpy(Sdate, "000000");
    }
    if (message_AskForString(NULL, 50, "Mark messages through date [end of time]: ", "", DBuf, sizeof(DBuf)) < 0) {
	return;
    }
    if (DBuf[0]) {
	long code;
	code = ams_MS_ParseDate(ams_GetAMS(), DBuf, &year, &month, &day, &hour, &min, &sec, &wday, &gtm);
	if (code) {
	    message_DisplayString(NULL, 25, "Sorry; I don't understand the date you entered.");
	    return;
	}
	strcpy(Edate, amsutil_convlongto64(gtm, 0));
    } else {
	strcpy(Edate, "zzzzzz");
    }
    orgpos = captions_GetDotPosition(self);
    numfound = 0;
    for (j = 0; j < self->captioncachecount; ++j) {
	cc = &self->capcache[j];
	if (FoundStartDate) {
	    if (strcmp(cc->Date, Edate) > 0) {
/*		FoundEndDate = TRUE; */
		break;
	    }
	} else {
	    if (strcmp(cc->Date, Sdate) > 0) {
		FoundStartDate = TRUE;
	    }
	}
	if (FoundStartDate) {
	    if (numfound++ <= 0) {
		orgpos = cc->offset;
	    }
	    captions_ToggleMark(self, cc, cc->offset);
	}
    }
    captions_SetDotPosition(self, orgpos);
    captions_FrameDot(self, orgpos);
    captions_PostMenus(self, NULL);
    if (numfound) {
	sprintf(ErrorText, "Marked %s messages in the date range requested.", amsutil_cvEng(numfound, 0, 1000));
    } else {
	strcpy(ErrorText, "There are no messages in that date range.");
    }
    message_DisplayString(NULL, 10, ErrorText);
    captions_WantInputFocus(self, self);
}

static char LastClassification[1+MAXPATHLEN] = AMS_DEFAULTMAILDIR;

char *captions__GetLastClassification(self)
struct captions *self;
{
    if ((self->ShortName) && !strcmp(LastClassification, self->ShortName)) {
	return("");
    }
    return(LastClassification);
}

SetLastClassification(self, lc)
struct captions *self;
char *lc;
{
    strncpy(LastClassification, lc, sizeof(LastClassification));
}


void captions__BackUpCheckingMarks(ci, InsistOnMark)
struct captions *ci;
Boolean InsistOnMark;
{
	int pos, len, thisCUID, whichcaption;
	struct environment *env;
	Boolean FirstPass = TRUE;

restart:
	pos = ci->HighlightEnv ? environment_Eval(ci->HighlightEnv) : -1;
        if (pos <= 0) pos = captions_GetDotPosition(ci);
	thisCUID = captions_FindCUIDByDocLocation(ci, &pos, &len, &env, &whichcaption);
	if (captions_GetDotLength(ci) > 0) {
	    if (pos > 0) {
		captions_SetDotPosition(ci, pos + len + 1);
	    }
	    captions_SetDotLength(ci, 0);
	}
	--whichcaption;
	if (InsistOnMark) {
	     while (whichcaption >= 0 && !ci->capcache[whichcaption].IsMarked) {
		--whichcaption;
	     }
	}
	if (whichcaption < 0) {
	    if (FirstPass && ((ci->FetchedFromStart + ci->FetchedFromEnd) < ci->FolderSize)) {
		captions_GuaranteeFetchedRange(ci, InsistOnMark ? 0 : (ci->FolderSize - ci->FetchedFromEnd-5), ci->FolderSize);
		FirstPass = FALSE;
		goto restart;
	    } else {
		message_DisplayString(NULL, 10, "You are at the beginning of the captions");
		return;
	    }
	}
	pos = ci->capcache[whichcaption].offset;
	captions_SetDotPosition(ci, pos);
	captions_FrameDot(ci, pos + 5);
	thisCUID = captions_FindCUIDByDocLocation(ci, &pos, &len, &env, &whichcaption);
	captions_DisplayNewBody(ci, thisCUID, pos, len, env);
	return;
}

void captions__AlterFileIntoMenus(self, Shrink)
struct captions *self;
boolean Shrink;
{
    if (Shrink) {
	if (self->MenusExpanded) {
	    self->MenusExpanded = FALSE;
	    captions_PostMenus(self, NULL);
	}
    } else {
	if (!self->MenusExpanded) {
	    self->MenusExpanded = TRUE;
	    captions_PostMenus(self, NULL);
	}
    }
}

void captions__MarkCurrent(ci)
struct captions *ci;
{
    int whichcaption, linestart, linelen;
    char ErrorText[256];
    struct environment *env;

    if (!ci->HighlightEnv) {
	message_DisplayString(NULL, 10, "There is nothing to mark.");
	return;
    }
    linestart = environment_Eval(ci->HighlightEnv);
    captions_FindCUIDByDocLocation(ci, &linestart, &linelen, &env, &whichcaption);
    captions_ToggleMark(ci, &ci->capcache[whichcaption], linestart);
    captions_PostMenus(ci, NULL);
    if (ci->MarkCount <=0) {
	strcpy(ErrorText, "There are now no messages marked.");
    } else if (ci->MarkCount == 1) {
	strcpy(ErrorText, "There is now one message marked.");
    } else {
	sprintf(ErrorText, "There are now %s messages marked.", amsutil_cvEng(ci->MarkCount, 0, 1000));
    }
    message_DisplayString(NULL, 10, ErrorText);
}

void captions__PrintVisibleMessage(self) 
struct captions *self;
{
    int flags = 0;

    if (self->VisibleCUID <1) {
	message_DisplayString(NULL, 10, "There is nothing to print.");
	return;
    }
    ams_WaitCursor(TRUE);
    if (self->CurrentFormatting & MODE822_ROT13) {
	flags |= AMS_PRINT_ROT13;
    }
    if (self->CurrentFormatting & MODE822_FIXEDWIDTH) {
	flags |= AMS_PRINT_FIXED;
    }
    message_DisplayString(NULL, 10, "Printing message; please wait...");
    im_ForceUpdate();
    if (ams_CUI_PrintBodyFromCUIDWithFlags(ams_GetAMS(), self->VisibleCUID, flags, NULL) == 0) {
	message_DisplayString(NULL, 10, "Message queued for printing.");
    }
    ams_WaitCursor(FALSE);
}

void captions__Redisplay(self, Mode, contenttype)
struct captions *self;
int Mode;
char *contenttype;
{
    int dot, top, len;
    struct t822view *bod;

    if (self->VisibleCUID < 0) return;
    ams_WaitCursor(TRUE);
    bod = captions_GetBodView(self);
    dot = t822view_GetDotPosition(bod);
    top = t822view_GetTopPosition(bod);
    len = t822view_GetDotLength(bod);
    captions_GetBodyFromCUID(self, self->VisibleCUID, Mode, contenttype);
    captions_PostMenus(self, NULL);
    t822view_SetDotPosition(bod, dot);
    t822view_SetTopPosition(bod, top);
    t822view_SetDotLength(bod, len);
    ams_WaitCursor(FALSE);
}

void captions__CloneMessage(self, Code)
struct captions *self;
int Code;
{
    int cuid, MayModify;
    char NewDirName[1+MAXPATHLEN], SaveDirName[1+MAXPATHLEN], ErrorText[256];

    cuid = self->VisibleCUID;    
    if (cuid < 1) {
	message_DisplayString(NULL, 10, "There is no message on display.");
	return;
    }
    MayModify = AMS_GET_ATTRIBUTE(self->VisibleSnapshot, AMS_ATT_MAYMODIFY) ? 1 : 0;
    switch(Code) {
	case MS_CLONE_APPEND:
	case MS_CLONE_APPENDDEL:
	    sprintf(ErrorText, "Append this message to what folder [%s]:  ", captions_GetLastClassification(self));
	    if (!MayModify) Code = MS_CLONE_APPEND;
	    break;
	case MS_CLONE_COPY:
	case MS_CLONE_COPYDEL:
	    sprintf(ErrorText, "File this message into which folder [%s]: ", captions_GetLastClassification(self));
	    if (!MayModify) Code = MS_CLONE_COPY;
	    break;
	default:
	    ams_ReportError(ams_GetAMS(), "Invalid internal parameter to CloneMessage.", ERR_WARNING, FALSE, 0);
	    return;
    }
    if (ams_GetFolderName(ErrorText, NewDirName, MAXPATHLEN, "", FALSE)) return;
    if (NewDirName[0] == '\0') {
	strcpy(NewDirName, captions_GetLastClassification(self));
	if (NewDirName[0] == '\0') return;
    }
    strcpy(SaveDirName, NewDirName); /* Cuilib fiddles with the string */
    ams_WaitCursor(TRUE);
    if (!ams_CUI_CloneMessage(ams_GetAMS(), cuid, NewDirName, Code)) {
	SetLastClassification(self, SaveDirName);
	/* cuilib reports errors, here we deal with success */
	if (Code == MS_CLONE_COPYDEL || Code == MS_CLONE_APPENDDEL) {
	    if (self->HighlightEnv) text_SetEnvironmentStyle(self->CaptText, self->HighlightEnv, self->ActiveDeletedStyle);
	    AMS_SET_ATTRIBUTE(self->VisibleSnapshot, AMS_ATT_DELETED);
	    captions_AlterDeletedIcon(self, self->HighlightStartPos, TRUE);
	    captions_PostMenus(self, NULL);
	}
	captions_WantUpdate(self, self);
    }
    ams_WaitCursor(FALSE);
    return;
}

void captions__SendMessage(ci, code)
struct captions *ci;
int code; 
{
    char FileName[MAXPATHLEN+1];
    struct sendmessage *sm;

    if (code == AMS_REPLY_FRESH) {
	ams_WaitCursor(TRUE);
	sm = folders_ExposeSend(captions_GetFolders(ci));
	sendmessage_Reset(sm);
	ams_WaitCursor(FALSE);
	return;
    }
    if (ci->VisibleCUID <1) {
	message_DisplayString(NULL, 10, "There is no message on display.");
	return;
    }
    ams_WaitCursor(TRUE);
    if (ams_CUI_NameReplyFile(ams_GetAMS(), ci->VisibleCUID, code, FileName) != 0) {
	ams_WaitCursor(FALSE);
	return;
    } 
    sm = folders_ExposeSend(captions_GetFolders(ci));
    if (sm) sendmessage_ReadFromFile(sm, FileName, TRUE);
    ams_WaitCursor(FALSE);
    return;
}

void captions__SetFolders(self, f)
struct captions *self;
struct folders *f;
{
    self->myfold = f;
}

void captions__SetLabel(self, label)
struct captions *self;
char *label;
{
    if (self->myframe) {
	frame_SetTitle(self->myframe, label);
    }
}

struct t822view *
captions__NewBodiesInNewWindow(self)
struct captions *self;
{
    struct t822view *tv = t822view_New();
    struct text *t = text_New();

    text_SetReadOnly(t, TRUE);	/* -wjh */
    captions_SetBodies(self, tv);
    t822view_SetCaptions(tv, self);
    t822view_SetDataObject(tv, t);
    ams_InstallInNewWindow(t822view_GetApplicationLayer(tv), "messages-bodies", "Message Bodies", environ_GetProfileInt("bodies.width", 600), environ_GetProfileInt("bodies.height", 250), tv);
    return(tv);
}

struct folders *
captions__NewFoldersInNewWindow(self)
struct captions *self;
{
    struct folders *f = folders_New();

    captions_SetFolders(self, f);
    folders_SetCaptions(f, self);
    ams_InstallInNewWindow(folders_GetApplicationLayer(f), "messages-folders", "Message Folders", environ_GetProfileInt("folders.width", 600), environ_GetProfileInt("folders.height", 120), f);
    return(f);
}

struct folders *captions__GetFolders(self)
struct captions *self;
{
    if (!self->myfold) {
	captions_NewFoldersInNewWindow(self);
    }
    return(self->myfold);
}

struct t822view *captions__GetBodView(self)
struct captions *self;
{
    if (!self->BodView) {
	captions_NewBodiesInNewWindow(self);
    }
    return(self->BodView);
}
