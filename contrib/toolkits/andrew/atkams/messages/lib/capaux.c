/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/capaux.c,v 1.13 89/09/22 09:26:15 ajp Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/capaux.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/capaux.c,v 1.13 89/09/22 09:26:15 ajp Exp $ ";
#endif /* lint */

#include <andrewos.h>
#include <sys/param.h>
#include <cui.h>
#include <fdphack.h>
#include <class.h>
#include <keymap.ih>
#include <keystate.ih>
#include <menulist.ih>
#include <bind.ih>
#include <message.ih>
#include <complete.ih>
#include <im.ih>
#include <proctbl.ih>
#include <text.ih>
#include <textv.ih>
#include <style.ih>
#include <envrment.ih>
#include <environ.ih>
#include <cursor.ih>
#include <fontdesc.ih>

#include <ams.ih>
#include <hdrparse.h>
#include <amsutil.ih>
#include <folders.ih>
#include <sendmsg.ih>
#include <text822v.ih>

#define AUXMODULE 1
#include <captions.eh>
#undef AUXMODULE


extern void PreviousCaptionLine(), CapScrollBack(), CapGlitchDown(), CapBeginText(), captions__SimulateClick();

extern char *index();

static struct keymap *captions_privkeymap;
static struct menulist *captions_privmenulist;

extern char *GetLastResendName();

extern int (*textview_PreviousLineCmd)(),
	    (*textview_ReverseSearchCmd)(),
	    (*textview_ScrollScreenFwdCmd)(),
	    (*textview_ScrollScreenBackCmd)(),
	    (*textview_BeginningOfTextCmd)(),
	    (*textview_BeginningOfLineCmd)(),
	    (*textview_EndOfLineCmd)(),
	    (*textview_GlitchDownCmd)();

boolean captions__InitializeObject(c, captions)
struct classheader *c;
struct captions *captions;  
{
    char *fontname;
    int fontsize, mailfontbloat = (amsutil_GetOptBit(EXP_WHITESPACE)) ? 2 : 0;

    captions_SetWhatIAm(captions, WHATIAM_CAPTIONS);
    InitKeysMenus(captions);
    captions->CommentText = NULL;
    captions->myframe = NULL;
    captions->MenusExpanded = FALSE;
    captions->mycursor = NULL;
    captions->MarkCount = 0;
    captions->FolderSize = captions->FetchedFromStart = captions->FetchedFromEnd = 0;

    captions->CaptText = text_New();
    captions_SetDataObject(captions, captions->CaptText);
    captions->textscrollinterface = (struct scrollfns *) super_GetInterface(captions, "scroll,vertical");

    captions->ActiveCaptionStyle = style_New();
    captions->ActiveDeletedStyle = style_New();
    captions->DeletedStyle = style_New();
    captions->FixedStyle = style_New();
    captions->GlobalCapStyle = style_New();
    captions->HighlightStyle = style_New();
    captions->IconicStyle = style_New();
    captions->UnderlinedIconicStyle = style_New();
    captions->MailStyle = style_New();
    captions->NormalCaptionStyle = style_New();

    style_AddNewFontFace(captions->FixedStyle, (long) fontdesc_Fixed);
    style_AddNewFontFace(captions->HighlightStyle, (long) fontdesc_Italic);
    style_SetFontFamily(captions->FixedStyle, "andytype");
    style_SetFontFamily(captions->IconicStyle, "msgs");
    style_SetFontFamily(captions->UnderlinedIconicStyle, "msgs");
    style_SetFontSize(captions->IconicStyle, style_ConstantFontSize, 10);
    style_SetFontSize(captions->UnderlinedIconicStyle, style_ConstantFontSize, 10);
    style_SetFontSize(captions->MailStyle, style_PreviousFontSize, mailfontbloat);
    style_SetJustification(captions->GlobalCapStyle, style_LeftJustified);
    style_SetJustification(captions->HighlightStyle, style_LeftJustified);
    style_SetName(captions->FixedStyle, "Typewriter");
    style_SetNewIndentation(captions->ActiveCaptionStyle, style_ConstantMargin, -10, style_RawDots);
    style_SetNewIndentation(captions->ActiveDeletedStyle, style_ConstantMargin, -10, style_RawDots);
    style_SetNewIndentation(captions->GlobalCapStyle, style_ConstantMargin, -10, style_RawDots);
    style_SetNewInterlineSpacing(captions->GlobalCapStyle, style_ConstantSpacing,10, style_RawDots);
    style_SetNewLeftMargin(captions->ActiveCaptionStyle, style_ConstantMargin, 20, style_RawDots);
    style_SetNewLeftMargin(captions->ActiveDeletedStyle, style_ConstantMargin, 20, style_RawDots);
    style_SetNewLeftMargin(captions->GlobalCapStyle, style_ConstantMargin, 20, style_RawDots);
    style_AddFlag(captions->UnderlinedIconicStyle, style_Underline);
    if (amsutil_GetOptBit(EXP_FIXCAPTIONS)) {
	style_AddNewFontFace(captions->ActiveCaptionStyle, (long) fontdesc_Bold | fontdesc_Fixed);
	style_AddNewFontFace(captions->ActiveDeletedStyle, (long) fontdesc_Bold | fontdesc_Fixed);
	style_AddNewFontFace(captions->DeletedStyle, (long) fontdesc_Fixed);
	style_AddNewFontFace(captions->GlobalCapStyle, (long) fontdesc_Fixed);
	style_AddNewFontFace(captions->HighlightStyle, (long) fontdesc_Fixed);
	style_AddNewFontFace(captions->MailStyle, (long) fontdesc_Fixed);
	style_AddNewFontFace(captions->NormalCaptionStyle, (long) fontdesc_Fixed);
    } else {
	style_AddNewFontFace(captions->ActiveCaptionStyle, (long) fontdesc_Bold);
	style_AddNewFontFace(captions->ActiveDeletedStyle, (long) fontdesc_Bold);
    }
    text_SetGlobalStyle(captions->CaptText, captions->GlobalCapStyle);

    captions->SouthPoint = NULL;

    captions->VisibleCUID = -1;
    bzero(captions->VisibleSnapshot, AMS_SNAPSHOTSIZE);
    captions->HighlightStartPos = 0;
    captions->HighlightLen = 0;
    captions->HighlightEnv = NULL;
    captions->OldMarkCount = 0;
    captions->OldMarks = NULL;
    captions->capcache = (struct CaptionCache *) malloc(25 *sizeof(struct CaptionCache));
    captions->captioncachesize = 25;
    captions->captioncachecount = 0;
    captions->FullName = NULL;
    captions->ShortName = NULL;
    captions->firstcuid = -1;
    ams_AddCheckpointCaption(captions);

    captions->myfold = NULL;

    fontsize = environ_GetProfileInt("messages.fontsize", 12);
    style_SetFontSize(captions->DeletedStyle, style_ConstantFontSize, fontsize - 4);
    style_SetFontSize(captions->ActiveDeletedStyle, style_ConstantFontSize, fontsize - 4);
    style_SetFontSize(captions->GlobalCapStyle, style_ConstantFontSize, fontsize);

    fontname = amsutil_GetDefaultFontName();
    if (amsutil_GetOptBit(EXP_FIXCAPTIONS)) fontname = "andytype";
    style_SetFontFamily(captions->ActiveCaptionStyle, fontname);
    style_SetFontFamily(captions->ActiveDeletedStyle, fontname);
    style_SetFontFamily(captions->DeletedStyle, fontname);
    style_SetFontFamily(captions->GlobalCapStyle, fontname);
    style_SetFontFamily(captions->HighlightStyle, fontname);
    style_SetFontFamily(captions->MailStyle, fontname);
    style_SetFontFamily(captions->NormalCaptionStyle, fontname);

    captions->BodView = NULL;

    captions_ShowHelp(captions);
    text_SetReadOnly(captions->CaptText, TRUE);
    return(TRUE);
}

void captions__FinalizeObject(c, self)
struct classheader *c;
struct captions *self;
{
    ams_RemoveCheckpointCaption(self);
    text_Destroy(self->CaptText);
    if (self->CommentText) {
	free(self->CommentText);
	self->CommentText = NULL;
    }
    if (self->myfold) {
	folders_SetCaptions(self->myfold, NULL);
    }
    if (self->BodView) {
	t822view_SetCaptions(self->BodView, NULL);
    }
    style_Destroy(self->ActiveCaptionStyle);
    style_Destroy(self->NormalCaptionStyle);
    style_Destroy(self->HighlightStyle);
    style_Destroy(self->GlobalCapStyle);
    style_Destroy(self->DeletedStyle);
    style_Destroy(self->ActiveDeletedStyle);
    style_Destroy(self->IconicStyle);
    style_Destroy(self->UnderlinedIconicStyle);
    style_Destroy(self->MailStyle);
    style_Destroy(self->FixedStyle);
    FinalizeProcStuff(self);
    if (self->OldMarks) {
	free(self->OldMarks);
	self->OldMarks = NULL;
    }
    if (self->capcache) {
	free(self->capcache);
	self->capcache = NULL;
    }
    if (self->FullName) {
	free(self->FullName);
	self->FullName = NULL;
    }
    if (self->ShortName) {
	free(self->ShortName);
	self->ShortName = NULL;
    }
}

void CaptionsCompound(self, cmds)
struct captions *self;
char *cmds;
{
    ams_GenericCompoundAction(ams_GetAMS(), self, "captions", cmds);
}

void CaptionsTextviewCommand(self, cmds)
struct captions *self;
char *cmds;
{
    ams_GenericCompoundAction(ams_GetAMS(), self, "textview", cmds);
}

void CaptionsFoldersCommand(self, cmds)
struct captions *self;
char *cmds;
{
    ams_GenericCompoundAction(ams_GetAMS(), captions_GetFolders(self), "folders", cmds);
}

void CaptionsBodiesCommand(self, cmds)
struct captions *self;
char *cmds;
{
    ams_GenericCompoundAction(ams_GetAMS(), captions_GetBodView(self), "t822view", cmds);
}

static void DownFocus(self)
struct captions *self;
{
    ams_Focus(captions_GetBodView(self));
}

static void UpFocus(self)
struct captions *self;
{
    ams_Focus(captions_GetFolders(self));
}

void captions__ClearMarks(self)
struct captions *self;
{
    int j, whicholdmark = 0;
    struct CaptionCache *hc;

    if (self->MarkCount <= 0) {
	message_DisplayString(NULL, 10, "There are no marked messages");
	return;
    }
    ams_WaitCursor(TRUE);
    if (self->OldMarks) free(self->OldMarks);
    self->OldMarks = (int *) malloc((1+self->MarkCount)*sizeof(long));
    if (self->OldMarks) self->OldMarkCount = self->MarkCount;
    for (j = 0; j < self->captioncachecount; ++j) {
	hc = &self->capcache[j];
	if (hc->IsMarked) {
	    if (self->OldMarks) self->OldMarks[whicholdmark++] = hc->cuid;
	    captions_ToggleMark(self, hc, hc->offset);
	}
    }
    self->MarkCount = 0;
    captions_PostMenus(self, NULL);
    message_DisplayString(NULL, 10, "All marks cleared; there are now no messages marked.");
    captions_WantUpdate(self, self);
    ams_WaitCursor(FALSE);
}

static struct bind_Description privbindings [] = {
    {"captions-previous-caption", "\020", NULL, NULL, 0, NULL, PreviousCaptionLine, "Go to previous caption"},
    {"captions-scroll-back", "\033v", NULL, NULL, NULL, 0, CapScrollBack, "Scroll back captions"},
    {"captions-glitch-down", "\033z", NULL, NULL, NULL, 0, CapGlitchDown, "Glitch down captions"},
    {"captions-glitch-down", "\021", NULL, NULL, NULL, 0, CapGlitchDown, "Glitch down captions"},
    {"captions-beginning-of-text", "\033<", NULL, NULL, NULL, 0, CapBeginText, "Move to top of captions"},
    {"captions-left-click-here", "!", TRUE, NULL, NULL, 0, captions__SimulateClick, "Display what I am pointing at"},
    {"captions-right-click-here", "@", FALSE, NULL, NULL, 0, captions__SimulateClick, "Simulate right click on what I am pointing at"},
    {"captions-compound-operation", NULL, NULL, NULL, NULL, 0, CaptionsCompound, "Execute a compound captions operation"},
    {"captions-textview-compound", NULL, NULL, NULL, NULL, 0, CaptionsTextviewCommand, "Execute a compound 'textview' operation on the captions"},
    {"captions-folders-compound", NULL, NULL, NULL, NULL, 0, CaptionsFoldersCommand, "Execute a compound 'folders' operation."},
    {"captions-bodies-compound", NULL, NULL, NULL, NULL, 0, CaptionsBodiesCommand, "Execute a compound 't822view' operation."},
    {"captions-down-focus", "\030n", NULL, NULL, NULL, 0, DownFocus, "Move input focus to bodies"},
    {"captions-up-focus", "\030p", NULL, NULL, NULL, 0, UpFocus, "Move input focus to folders"},
    {"captions-down-focus", "\030\016", NULL, NULL, NULL, 0, DownFocus, "Move input focus to bodies"},
    {"captions-up-focus", "\030\020", NULL, NULL, NULL, 0, UpFocus, "Move input focus to folders"},
    {NULL, "\033~", NULL, NULL, NULL, 0, NULL, NULL, NULL}, /* Preserve read onliness */
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
};

InitKeysMenus(captions)
struct captions *captions;
{
    captions->privkeys = keystate_Create(captions, captions_privkeymap);
    captions->privmenus = menulist_DuplicateML(captions_privmenulist, captions);
}

OneTimeInitKeyMenus(c)
struct classheader *c;
{
    struct proctable_Entry *tempProc;

    if ((tempProc = proctable_Lookup("textview-previous-line")) != NULL) {
        textview_PreviousLineCmd = proctable_GetFunction(tempProc);
    }
    if ((tempProc = proctable_Lookup("textview-reverse-search")) != NULL) {
        textview_ReverseSearchCmd = proctable_GetFunction(tempProc);
    }
    if ((tempProc = proctable_Lookup("textview-next-screen")) != NULL) {
        textview_ScrollScreenFwdCmd = proctable_GetFunction(tempProc);
    }
    if ((tempProc = proctable_Lookup("textview-prev-screen")) != NULL) {
        textview_ScrollScreenBackCmd = proctable_GetFunction(tempProc);
    }
    if ((tempProc = proctable_Lookup("textview-beginning-of-text")) != NULL) {
        textview_BeginningOfTextCmd = proctable_GetFunction(tempProc);
    }
    if ((tempProc = proctable_Lookup("textview-beginning-of-line")) != NULL) {
        textview_BeginningOfLineCmd = proctable_GetFunction(tempProc);
    }
    if ((tempProc = proctable_Lookup("textview-end-of-line")) != NULL) {
        textview_EndOfLineCmd = proctable_GetFunction(tempProc);
    }
    if ((tempProc = proctable_Lookup("textview-glitch-down")) != NULL) {
        textview_GlitchDownCmd = proctable_GetFunction(tempProc);
    }
    captions_privkeymap = keymap_New();
    captions_privmenulist = menulist_New();
    bind_BindList(privbindings, captions_privkeymap, captions_privmenulist, c);
}

void captions__PostMenus(self, ml)
struct captions *self;
struct menulist *ml;
{
    menulist_ClearChain(self->privmenus);
    if (ml) menulist_ChainAfterML(self->privmenus, ml, ml);
    super_PostMenus(self, self->privmenus);
}

void captions__PostKeyState(self, ks)
struct captions *self;
struct keystate *ks;
{
    self->privkeys->next = NULL;
    if (amsutil_GetOptBit(EXP_KEYSTROKES)) {
	if (ks) keystate_AddAfter(ks, self->privkeys);
	super_PostKeyState(self, self->privkeys);
    } else {
	super_PostKeyState(self, ks);
    }
}

ClassifyMarkedByName(self, NameGiven)
struct captions *self;
char *NameGiven;
{
    int code;

    if (*NameGiven == '*') {
	++NameGiven;
	code = MARKACTION_APPENDBYNAME;
    } else {
	code = MARKACTION_CLASSIFYBYNAME;
    }
    captions_ActOnMarkedMessages(self, code, NameGiven);
}

captions__ActOnMarkedMessages(ci, Code, GivenName)
struct captions *ci;
int Code;
char *GivenName; /* Not always supplied */
{
    char ErrorText[256];
    struct CaptionCache *hc;
    int j, k, resultcode, OpCode, goodct = 0, len;
    Boolean IsActiveCaption, HadDisaster, errct;
    char HeaderBuf[2000];
    char *HeadAccum = NULL;
    struct sendmessage *sm = NULL;

    if ((ci->MarkCount <= 0) && (Code != MARKACTION_RESTORE)) {
	message_DisplayString(NULL, 10, "There are no marked messages");
	return;
    }
    ams_WaitCursor(TRUE);
    HadDisaster = FALSE;
    errct = 0;
    for (j = 0; j < ci->captioncachecount && !HadDisaster; ++j) {
	hc = &ci->capcache[j];
	if (hc->IsMarked || (Code == MARKACTION_RESTORE)) {
	    if (hc->env->data.style == ci->ActiveDeletedStyle || hc->env->data.style == ci->ActiveCaptionStyle) {
		IsActiveCaption = TRUE;
	    } else {
		IsActiveCaption = FALSE;
	    }
	    switch(Code) {
		case MARKACTION_APPENDTOFILE:
		    if (captions_AppendOneMessageToFile(ci, hc->cuid, GivenName)) {
			errct++;
			HadDisaster = TRUE;
		    } else ++goodct;
		    break;
		case MARKACTION_CLASSIFYBYNAME:
		case MARKACTION_APPENDBYNAME:
		case MARKACTION_COPYBYNAME:
		    if (Code == MARKACTION_APPENDBYNAME) {
			OpCode = hc->MayModify ? MS_CLONE_APPENDDEL : MS_CLONE_APPEND;
		    } else if (Code == MARKACTION_COPYBYNAME) {
			OpCode = MS_CLONE_COPY;
		    } else {
			OpCode = hc->MayModify ? MS_CLONE_COPYDEL : MS_CLONE_COPY;
		    }
		    resultcode = ams_CUI_CloneMessage(ams_GetAMS(), hc->cuid, GivenName, OpCode);
		    if (resultcode) {
			errct++;
			HadDisaster = TRUE;
			break;
		    } 		    
		    if (OpCode == MS_CLONE_APPEND || OpCode == MS_CLONE_COPY) {
			++goodct;
			break;
		    }
		    /* FALL THROUGH to delete */
		case MARKACTION_DELETE:
		    if (ams_CUI_DeleteMessage(ams_GetAMS(), hc->cuid) == 0) {
			text_SetEnvironmentStyle(ci->CaptText, hc->env, IsActiveCaption ? ci->ActiveDeletedStyle : ci->DeletedStyle);
			if (IsActiveCaption) {
			    AMS_SET_ATTRIBUTE(ci->VisibleSnapshot, AMS_ATT_DELETED);
			}
			captions_AlterDeletedIcon(ci, hc->offset, TRUE);
			++goodct;
		    } else {
			++errct;
			HadDisaster = TRUE;
			/* Errors were reported by cui routine */
		    }
		    break;
		case MARKACTION_UNDELETE:
		    if (ams_CUI_UndeleteMessage(ams_GetAMS(), hc->cuid) == 0) {
			text_SetEnvironmentStyle(ci->CaptText, hc->env, IsActiveCaption ? ci->ActiveCaptionStyle : ci->NormalCaptionStyle);
			if (IsActiveCaption) {
			    AMS_UNSET_ATTRIBUTE(ci->VisibleSnapshot, AMS_ATT_DELETED);
			}
			captions_AlterDeletedIcon(ci, hc->offset, FALSE);
			++goodct;
		    } else {
			++errct;
			HadDisaster = TRUE;
			/* Errors were reported by cui routine */
		    }
		    break;
		case MARKACTION_PRINT:
		    if (ams_CUI_PrintBodyFromCUIDWithFlags(ams_GetAMS(), hc->cuid, 0, NULL)) {
			++errct;
		    } else {
			++goodct;
		    }
		    /* Errors were reported by cui routine */
		    break;
		case MARKACTION_RESTORE:
		    for (k=0; k<ci->OldMarkCount; ++k) {
			if (hc->cuid == ci->OldMarks[k] && (!hc->IsMarked)) {
			    captions_ToggleMark(ci, hc, hc->offset);
			    ++goodct;
			    break;
			}
		    }
		    break;
		case MARKACTION_EXCERPT:
		    if (!sm) {
			sm = folders_ExposeSend(captions_GetFolders(ci));
			if (!sm) return;
		    }
		    if (j == (ci->captioncachecount - 1)) {
			len = text_GetLength(ci->CaptText) - hc->offset;
		    } else {
			len = ci->capcache[j+1].offset - hc->offset;
		    }
		    captions_DisplayNewBody(ci, hc->cuid, hc->offset, len, hc->env);
		    textview_SetDotPosition(sm->BodyTextview, text_GetLength((struct text *) textview_GetDataObject(sm->BodyTextview)));
		    sendmessage_QuoteBody(sm);
		    ++goodct;
		    break;
		case MARKACTION_REPLYALL:
		case MARKACTION_REPLYSENDERS:
		    HeaderBuf[0] = '\0';
		    ams_CUI_GetHeaderContents(ams_GetAMS(), hc->cuid, NULL, (Code == MARKACTION_REPLYSENDERS) ? HP_REPLY_TO : HP_ALLREPLY, HeaderBuf, sizeof(HeaderBuf) - 2);
		    if (HeadAccum) {
			HeadAccum = realloc(HeadAccum, strlen(HeadAccum) + strlen(HeaderBuf) + 5);
			strcat(HeadAccum, ",\n\t");
			strcat(HeadAccum, HeaderBuf);
		    } else {
			HeadAccum = malloc(1+strlen(HeaderBuf));
			strcpy(HeadAccum, HeaderBuf);
		    }
		    ++goodct;
		    break;
		case MARKACTION_RESEND:
		    if (ams_CUI_ResendMessage(ams_GetAMS(), hc->cuid, GivenName)) {
			HadDisaster = TRUE;
		    } else {
			++goodct;
		    }
		    break;
	    }
	    if (HadDisaster) {
		if (ams_GetBooleanFromUser(ams_GetAMS(), "Do you want to continue with the other marked messages", FALSE)) {
		    HadDisaster = FALSE;
		}
	    }
	}
    }
    if (HeadAccum) {
	if (!sm) {
	    sm = folders_ExposeSend(captions_GetFolders(ci));
	    if (!sm) return;
	}
	sendmessage_AddToToHeader(sm, HeadAccum);
	free(HeadAccum);
	HeadAccum = NULL;
    }
    if (errct) {
	char Foobar[100];
	sprintf(ErrorText, "Errors were encountered on %s ", amsutil_cvEng(errct, 0, 1000));
	sprintf(Foobar, "of the %s marked messages.", amsutil_cvEng(ci->MarkCount, 0, 1000));
	strcat(ErrorText, Foobar);
	message_DisplayString(NULL, 50, ErrorText);
    } else {
	switch (Code) {
	    case MARKACTION_RESTORE:
		sprintf(ErrorText, "Restored %s old marks.", amsutil_cvEng(goodct, 0, 1000));
		break;
	    case MARKACTION_RESEND:
		sprintf(ErrorText, "Resent %s messages to %s.", amsutil_cvEng(goodct, 0, 1000), GivenName);
		break;
	    case MARKACTION_DELETE:
		sprintf(ErrorText, "Deleted %s messages.", amsutil_cvEng(goodct, 0, 1000));
		break;
	    case MARKACTION_UNDELETE:
		sprintf(ErrorText, "Undeleted %s messages.", amsutil_cvEng(goodct, 0, 1000));
		break;
	    case MARKACTION_CLASSIFYBYNAME:
		sprintf(ErrorText, "Classified %s messages into %s.", amsutil_cvEng(goodct, 0, 1000), GivenName);
		break;
	    case MARKACTION_PRINT:
		sprintf(ErrorText, "Printed %s messages.", amsutil_cvEng(goodct, 0, 1000));
		break;
	    case MARKACTION_APPENDBYNAME:
		sprintf(ErrorText, "Appended %s messages to folder %s.", amsutil_cvEng(goodct, 0, 1000), GivenName);
		break;
	    case MARKACTION_EXCERPT:
		sprintf(ErrorText, "Excerpted %s messages.", amsutil_cvEng(goodct, 0, 1000));
		break;
	    case MARKACTION_REPLYSENDERS:
		sprintf(ErrorText, "Replying to senders of %s messages.", amsutil_cvEng(goodct, 0, 1000));
		break;
	    case MARKACTION_REPLYALL:
		sprintf(ErrorText, "Replying widely to %s messages.", amsutil_cvEng(goodct, 0, 1000));
		break;
	    case MARKACTION_COPYBYNAME:
		sprintf(ErrorText, "Copied %s messages into folder %s.", amsutil_cvEng(goodct, 0, 1000), GivenName);
		break;
	    case MARKACTION_APPENDTOFILE:
		sprintf(ErrorText, "Appended %s messages to file %s.", amsutil_cvEng(goodct, 0, 1000), GivenName);
		break;
	    default:
		sprintf(ErrorText, "Did something to %s messages.", amsutil_cvEng(goodct, 0, 1000));
		break;
	}
	message_DisplayString(NULL, 10, ErrorText);
	captions_PostMenus(ci, NULL);
	if (Code == MARKACTION_CLASSIFYBYNAME
		|| Code ==  MARKACTION_APPENDBYNAME
	    || Code ==  MARKACTION_COPYBYNAME) {
	    char Nick[1+MAXPATHLEN];
	    ams_CUI_BuildNickName(ams_GetAMS(), GivenName, Nick);
	    SetLastClassification(ci, Nick);
	}
    }
    captions_WantUpdate(ci, ci);
    ams_WaitCursor(FALSE);
    return;
}

FinalizeProcStuff(self)
struct captions *self;
{
    keystate_Destroy(self->privkeys);
    menulist_Destroy(self->privmenus);
    cursor_Destroy(self->mycursor);
}

CreateCaptionsCursor(self)
struct captions *self;
{
        struct fontdesc *fd;

	fd = fontdesc_Create("icon", 0, 12);
	self->mycursor = cursor_Create(self);
	cursor_SetGlyph(self->mycursor, fd, 'R');
}

