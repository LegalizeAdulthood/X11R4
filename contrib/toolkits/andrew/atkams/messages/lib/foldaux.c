/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/foldaux.c,v 1.10 89/10/04 18:10:19 cfe Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/foldaux.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/foldaux.c,v 1.10 89/10/04 18:10:19 cfe Exp $ ";
#endif /* lint */

#include <sys/param.h>
#include <andrewos.h>
#include <errprntf.h>
#include <cui.h>
#include <fdphack.h>
#include <class.h>
#include <keymap.ih>
#include <keystate.ih>
#include <proctbl.ih>
#include <menulist.ih>
#include <bind.ih>
#include <im.ih>
#include <message.ih>
#include <environ.ih>
#include <style.ih>
#include <fontdesc.ih>
#include <cursor.ih>
#include <text.ih>
#include <filetype.ih>
#include <envrment.ih>

#include <ams.ih>
#include <amsutil.ih>
#include <captions.ih>
#include <sendmsg.ih>
#include <text822v.ih>
#include <options.ih>

#define AUXMODULE 1
#include <folders.eh>
#undef AUXMODULE

extern void folders__SimulateClick();
extern char *rindex();

void TextviewCompound(tv, cmds)
struct textview *tv;
char *cmds;
{
    ams_GenericCompoundAction(ams_GetAMS(), tv, "textview", cmds);
}

void FoldersCompound(self, cmds)
struct folders *self;
char *cmds;
{
    ams_GenericCompoundAction(ams_GetAMS(), self, "folders", cmds);
}

void FoldersTextviewCommand(self, cmds)
struct folders *self;
char *cmds;
{
    ams_GenericCompoundAction(ams_GetAMS(), self, "textview", cmds);
}

void FoldersMessagesCommand(self, cmds)
struct folders *self;
char *cmds;
{
    ams_GenericCompoundAction(ams_GetAMS(), self, "messages", cmds);
}

void FoldersSendmessageCommand(self, cmds)
struct folders *self;
char *cmds;
{
    if (self->sm) {
	ams_GenericCompoundAction(ams_GetAMS(), self->sm, "sendmessage", cmds);
    }
}

void FoldersCaptionsCommand(self, cmds)
struct folders *self;
char *cmds;
{
    ams_GenericCompoundAction(ams_GetAMS(), folders_GetCaptions(self), "captions", cmds);
}

static void DownFocus(self)
struct folders *self;
{
    ams_Focus(folders_GetCaptions(self));
}

static void UpFocus(self)
struct folders *self;
{
    if (self->sm) {
	ams_Focus(self->sm->BodyTextview);
    } else {
	ams_Focus(captions_GetBodView(folders_GetCaptions(self)));
    }
}

static void SimulateLeftClick(self)
struct folders *self;
{
    DoClick(self, TRUE, TRUE);
}

static struct keymap *folders_standardkeymap;
static struct menulist *folders_standardmenulist;

static struct bind_Description folders_standardbindings [] = {
    /* procname, keysequence, key rock, menu string, menu rock, proc, docstring, dynamic autoload */
    {"folders-left-click-here", "!", TRUE, NULL, NULL, 0, SimulateLeftClick, "Display what I am pointing at"},
    {"folders-right-click-here", "@", FALSE, NULL, NULL, 0, folders__SimulateClick, "Simulate right click on what I am pointing at"},
    {"folder-down-focus", "\030n", NULL, NULL, NULL, 0, DownFocus, "Move input focus to captions"},
    {"folders-up-focus", "\030p", NULL, NULL, NULL, 0, UpFocus, "Move input focus to bodies or sendmessage"},
    {"folder-down-focus", "\030\016", NULL, NULL, NULL, 0, DownFocus, "Move input focus to captions"},
    {"folders-up-focus", "\030\020", NULL, NULL, NULL, 0, UpFocus, "Move input focus to bodies or sendmessage"},
    {"textview-compound", NULL, NULL, NULL, NULL, NULL, TextviewCompound, "Execute a compound textview operation"},
    {"folders-compound-operation", NULL, NULL, NULL, NULL, 0, FoldersCompound, "Execute a compound folders operation"},
    {"folders-textview-compound", NULL, NULL, NULL, NULL, 0, FoldersTextviewCommand, "Execute a compound 'textview' operation on the folders"},
    {"folders-sendmessage-compound", NULL, NULL, NULL, NULL, 0, FoldersSendmessageCommand, "Execute a compound 'sendmessage' operation."},
    {"folders-captions-compound", NULL, NULL, NULL, NULL, 0, FoldersCaptionsCommand, "Execute a compound 'captions' operation."},
    {NULL, "\033~", NULL, NULL, NULL, 0, NULL, NULL, NULL}, /* Preserve read onliness */
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
};

InitKeyMenusStyles(folders)
struct folders *folders;
{
    char *fontname;
    int fontsize;

    folders->mykeys = keystate_Create(folders, folders_standardkeymap);
    folders->mymenulist = menulist_DuplicateML(folders_standardmenulist, folders);

    folders->BigCenterStyle = style_New();
    style_SetFontSize(folders->BigCenterStyle, style_ConstantFontSize, 20);
    style_SetJustification(folders->BigCenterStyle, style_Centered);

    folders->CenterStyle = style_New();
    style_SetJustification(folders->CenterStyle, style_Centered);
    style_AddNewFontFace(folders->CenterStyle, (long) fontdesc_Italic);

    folders->SmallCenterStyle = style_New();
    style_SetJustification(folders->SmallCenterStyle, style_Centered);
    style_SetFontSize(folders->SmallCenterStyle, style_ConstantFontSize, 6);

    folders->BoldStyle = style_New();
    style_SetName(folders->BoldStyle, "Bold");
    style_AddNewFontFace(folders->BoldStyle, (long) fontdesc_Bold);

    folders->ItalicStyle = style_New();
    style_SetFontSize(folders->ItalicStyle, style_PreviousFontSize, 2);
    style_AddNewFontFace(folders->ItalicStyle, (long) fontdesc_Italic);

    folders->IconicStyle = style_New();
    style_SetFontFamily(folders->IconicStyle, "msgs");
    style_SetFontSize(folders->IconicStyle, style_ConstantFontSize, 10);

    folders->Activefolderstyle = style_New();
    if (amsutil_GetOptBit(EXP_FIXCAPTIONS)) {
	style_AddNewFontFace(folders->Activefolderstyle, (long) fontdesc_Bold | fontdesc_Fixed);
    } else {
	style_AddNewFontFace(folders->Activefolderstyle, (long) fontdesc_Bold);
    }
    style_SetNewLeftMargin(folders->Activefolderstyle, style_ConstantMargin, 20, style_RawDots);
    style_SetNewIndentation(folders->Activefolderstyle, style_ConstantMargin, -10, style_RawDots);

    folders->Normalfolderstyle = style_New();

    folders->GlobalCapStyle = style_New();
    style_SetJustification(folders->GlobalCapStyle, style_LeftJustified);
    style_SetNewInterlineSpacing(folders->GlobalCapStyle, style_ConstantSpacing,10, style_RawDots);
    style_SetNewLeftMargin(folders->GlobalCapStyle, style_ConstantMargin, 20, style_RawDots);
    style_SetNewIndentation(folders->GlobalCapStyle, style_ConstantMargin, -10, style_RawDots);

    fontsize = environ_GetProfileInt("messages.fontsize", 12);
    fontname = amsutil_GetDefaultFontName();
    style_SetFontSize(folders->GlobalCapStyle, style_ConstantFontSize, fontsize);
    style_SetFontFamily(folders->Activefolderstyle, fontname);
    style_SetFontFamily(folders->Normalfolderstyle, fontname);
    style_SetFontFamily(folders->GlobalCapStyle, fontname);
    style_SetFontFamily(folders->BoldStyle, fontname);
    style_SetFontFamily(folders->ItalicStyle, fontname);
}

OneTimeInitKeyMenus(c)
struct classheader *c;
{
    folders_standardkeymap = keymap_New();
    folders_standardmenulist = menulist_New();
    bind_BindList(folders_standardbindings, folders_standardkeymap, folders_standardmenulist, c);
}

void folders__PostKeyState(self, ks)
struct folders *self;
struct keystate *ks;
{
    self->mykeys->next = NULL;
    if (amsutil_GetOptBit(EXP_KEYSTROKES)) {
	if (ks) keystate_AddAfter(ks, self->mykeys);
	super_PostKeyState(self, self->mykeys);
    } else {
	super_PostKeyState(self, ks);
    }
}

void folders__PostMenus(self, ml)
struct folders *self;
struct menulist *ml;
{
    menulist_ClearChain(self->mymenulist);
    if (ml) menulist_ChainAfterML(self->mymenulist, ml, ml);
    super_PostMenus(self, self->mymenulist);
}

FinalizeProcStyleStuff(self)
struct folders *self;
{
    keystate_Destroy(self->mykeys);
    menulist_Destroy(self->mymenulist);
    style_Destroy(self->Activefolderstyle);
    style_Destroy(self->Normalfolderstyle);
    style_Destroy(self->GlobalCapStyle);
    style_Destroy(self->IconicStyle);
    style_Destroy(self->BoldStyle);
    style_Destroy(self->ItalicStyle);
    style_Destroy(self->CenterStyle);
    style_Destroy(self->BigCenterStyle);
    cursor_Destroy(self->mycursor);
}

CreateFoldersCursor(self)
struct folders *self;
{
    struct fontdesc *fd;

    fd = fontdesc_Create("icon", 0, 12);
    self->mycursor = cursor_Create(self);
    cursor_SetGlyph(self->mycursor, fd, 'R');
}

static int lastconfiguration = -999;

void folders__Reconfigure(self, listcode)
struct folders *self;
int listcode;
{
    if (lastconfiguration == listcode) return;
    ams_WaitCursor(TRUE);
    self->HasSetUp = 0;
    SetupList(self, listcode, NULL);
    lastconfiguration = self->CurrentConfiguration = listcode;
    folders_PostMenus(self, NULL);
    ams_WaitCursor(FALSE);
}

void folders__UpdateMsgs(self, mailonly, thingstoread, ShowHelp) 
struct folders *self;
int mailonly;
char *thingstoread[];
boolean ShowHelp;
{
    if (ShowHelp) {
	if (self->mycaps) {
	    if (self->mycaps->BodView) {
		t822view_ShowHelp(self->mycaps->BodView, NULL);
	    }
	    captions_ClearAndUpdate(self->mycaps, FALSE, TRUE);
	    captions_ShowHelp(self->mycaps);
	}
	folders_ShowHelp(self);
	im_ForceUpdate();
    }
    self->HasSetUp = 0;
    if (ams_OnlyMail(ams_GetAMS())) mailonly = 1;
    self->MailOnlyMode = mailonly;
    ams_WaitCursor(TRUE);
    if (thingstoread) {
	self->ShowingAsRequested = 1;
	SetupList(self, LIST_AS_REQUESTED, thingstoread);
	lastconfiguration = LIST_AS_REQUESTED;
    } else {
	ams_CUI_CheckMailboxes(ams_GetAMS(), mailonly ? ams_GetMailPath() : NULL);
	self->ShowingAsRequested = 0;
	SetupList(self, LIST_NEWONLY, NULL);
	lastconfiguration = LIST_NEWONLY;
    }
    if (!amsutil_GetOptBit(EXP_NOFIRSTFOLDER)) {
	if (self->MainDirCacheCount > 0) {
	    int which, lim;

	    lim = self->MainDirCacheCount;
	    for(which=0; which<lim && self->MainDirCache[which].SkipMe; ++which) {
		;
	    }
	    if (which < lim) {
		captions_InsertUpdatesInDocument(folders_GetCaptions(self), self->MainDirCache[which].ShortName, self->MainDirCache[which].FullName, FALSE);
	    }
	} else {
	    ClearFolders(self);
	    folders_ReadMail(self, FALSE);
	}
    }
    folders_WantInputFocus(self, self);
    ams_WaitCursor(FALSE);
}

static char *E1 = "   (NOT the currently-displayed folder)";
static char *E2 = "Folder name: ";
static char *E3 = "\nFolder type: ";
static char *E4 = "\nNumber of messages: ";
static char *E5 = "\nYour subscription status: ";
static char *E6 = "\n\nExplanation of this message folder:\n\n";
static char *E7 = "\n\nNo explanation of this folder is available, but here is the first message:\n\n";
static char *E8 = "\n\nNo explanation of this folder is available.";

void folders__ExplainDir(self, FullName, nickname)
struct folders *self;
char *FullName, *nickname;
{
    int ProtCode, MsgCount;
    char ErrorText[100+MAXPATHLEN], *TypeStr, *SubsStr, ExpFileName[1+MAXPATHLEN], LocalFileName[1+MAXPATHLEN];
    struct text *d;
    int pos = 0, substatus, ShouldDelete, fpos;
    long mcode;

    captions_ResetVisibleCaption(folders_GetCaptions(self));
    t822view_SetDotPosition(captions_GetBodView(folders_GetCaptions(self)), 0);
    t822view_SetDotLength(captions_GetBodView(folders_GetCaptions(self)), 0);
    t822view_FrameDot(captions_GetBodView(folders_GetCaptions(self)), 0);

    d = captions_GetBodDoc(folders_GetCaptions(self));

    mcode = ams_MS_GetDirInfo(ams_GetAMS(), FullName, &ProtCode, &MsgCount);
    if (mcode) {
	if (ams_AMS_ERRNO(ams_GetAMS()) == EACCES) {
	    sprintf(ErrorText, "'%s' is private; you don't have read-access or are unauthenticated.", nickname);
	} else if (ams_vdown(ams_GetAMS(), ams_AMS_ERRNO(ams_GetAMS()))) {
	    sprintf(ErrorText, "%s: temporarily unavailable (net/server problem)", nickname);
	} else if (ams_AMS_ERRNO(ams_GetAMS()) == ENOENT) {
	    ams_CUI_HandleMissingFolder(ams_GetAMS(), FullName);
	    return;
	} else {
	    sprintf(ErrorText, "Cannot look up information about %s", FullName);
	    ams_ReportError(ams_GetAMS(), ErrorText, ERR_WARNING, TRUE, mcode);
	}
	message_DisplayString(NULL, 75, ErrorText);
	return;
    } else {
	TypeStr = ams_DescribeProt(ams_GetAMS(), ProtCode);
    }
    if (mcode = ams_MS_GetSubscriptionEntry(ams_GetAMS(), FullName, ErrorText, &substatus)) {
	ams_ReportError(ams_GetAMS(), "Cannot get subscription entry", ERR_WARNING, TRUE, mcode);
	SubsStr = "Lookup error";
    } else {
	switch(substatus) {
		case AMS_ALWAYSSUBSCRIBED:
		    SubsStr = "Subscribed";
		    break;
		case AMS_UNSUBSCRIBED:
		    SubsStr = "Not subscribed";
		    break;
		case AMS_ASKSUBSCRIBED:
		    SubsStr = "Ask-subscribed";
		    break;
		case AMS_SHOWALLSUBSCRIBED:
		    SubsStr = "Show-all subscribed";
		    break;
		case AMS_PRINTSUBSCRIBED:
		    SubsStr = "Print-subscribed";
		    break;
		default:
		    SubsStr = "unknown";
		    break;
	}
    }
    QAddToDoc(d, &pos, E2, strlen(E2), self->BoldStyle, strlen(E2) - 1);
    QAddToDoc(d, &pos, nickname, strlen(nickname), NULL, 0);
    if (folders_GetCaptions(self)->FullName && strcmp(FullName, folders_GetCaptions(self)->FullName)) {
	QAddToDoc(d, &pos, E1, strlen(E1), self->ItalicStyle, strlen(E1));
    }
    QAddToDoc(d, &pos, E3, strlen(E3), self->BoldStyle, strlen(E3) - 1);
    QAddToDoc(d, &pos, TypeStr, strlen(TypeStr), NULL, 0);
    QAddToDoc(d, &pos, E4, strlen(E4), self->BoldStyle, strlen(E4)-1);
    sprintf(ErrorText, "%d", MsgCount);
    QAddToDoc(d, &pos, ErrorText, strlen(ErrorText), NULL, 0);
    QAddToDoc(d, &pos, E5, strlen(E5), self->BoldStyle, strlen(E5)-1);
    QAddToDoc(d, &pos, SubsStr, strlen(SubsStr), NULL, 0);

    sprintf(ErrorText, "%s/%s", FullName, AMS_EXPLANATIONFILE);
    mcode = ams_MS_DisambiguateFile(ams_GetAMS(), ErrorText, ExpFileName, AMS_DISAMB_EXISTS);
    if (mcode) {
	if (ams_AMS_ERRNO(ams_GetAMS()) == ENOENT) {
	    if (MsgCount > 0) {
		FILE *fp;
		char SnapshotBuf[AMS_SNAPSHOTSIZE], LineBuf[2000], *objtype;
		int numbytes, bytesleft, cuid, IsDup, myid = 0;

		QAddToDoc(d, &pos, E7, strlen(E7), self->ItalicStyle, strlen(E7)-1);
		if (ams_CUI_GetHeaders(ams_GetAMS(), FullName, "000000", SnapshotBuf, AMS_SNAPSHOTSIZE, 0, &numbytes, &bytesleft, TRUE)) {
		    ams_ReportError(ams_GetAMS(), "Could not get first notice text", ERR_WARNING, TRUE, mcode);
		    return;
		}
		cuid = ams_CUI_GetCuid(ams_GetAMS(), AMS_ID(SnapshotBuf), FullName, &IsDup);
		if (ams_CUI_ReallyGetBodyToLocalFile(ams_GetAMS(), cuid, LocalFileName, &ShouldDelete, !ams_CUI_SnapIsRunning(ams_GetAMS()))) {
		    return; /* error already reported */
		}
		fp = fopen(LocalFileName, "r");
		if (!fp) {
		    ams_ReportError(ams_GetAMS(), "Could not open initial message to display it", ERR_WARNING, FALSE, 0);
		    return;
		}
		while (fgets(LineBuf, sizeof(LineBuf), fp)) {
		    if (LineBuf[0] == '\n') break;
		}
		fpos = ftell(fp);
		objtype = filetype_Lookup(fp, NULL, &myid, NULL);
		if (ftell(fp) == 0) {
		    fseek(fp, fpos, 0);
		}
		if (objtype && strcmp(objtype, "text")) {
		    myid = 0;
		    message_DisplayString(NULL, 80, "ATK message does not contain a top-level text object!");
		}
		text_SetReadOnly(d, FALSE);
		text_ReadSubString(d, pos, fp, 1);
		text_SetReadOnly(d, TRUE);
		fclose(fp);
		if (ShouldDelete) unlink(LocalFileName);
	    } else {
		QAddToDoc(d, &pos, E8, strlen(E8), self->ItalicStyle, strlen(E8)-1);
	    }
	} else {
	    ams_ReportError(ams_GetAMS(), "Cannot get explanation of messages folder", ERR_WARNING, TRUE, mcode);
	    QAddToDoc(d, &pos, E8, strlen(E8), self->ItalicStyle, strlen(E8)-1);
	}
    } else {
	int fd, bytes;
	char Splat[5000];

	QAddToDoc(d, &pos, E6, strlen(E6), self->ItalicStyle, strlen(E6)-1);
	ams_CUI_GenTmpFileName(ams_GetAMS(), LocalFileName);
	if (ams_CUI_GetFileFromVice(ams_GetAMS(), LocalFileName, ExpFileName)) {
	    ams_ReportError(ams_GetAMS(), "Cannot get explanation file from AFS", ERR_WARNING, TRUE, ams_mserrcode(ams_GetAMS()));
	    return;
	}

	fd = open(LocalFileName, O_RDONLY, 0644);
	if (fd<0) {
	    sprintf(ErrorText, "Cannot open local help file %s (%d)", LocalFileName, errno);
	    ams_ReportError(ams_GetAMS(), ErrorText, ERR_WARNING, FALSE, 0);
	    unlink(LocalFileName);
	    return;
	}
	while((bytes = read(fd, Splat, sizeof(Splat))) > 0) {
	    text_AlwaysInsertCharacters(d, pos, Splat, bytes);
	    pos += bytes;
	}
	close(fd);
	if (bytes<0) {
	    sprintf(ErrorText, "Cannot read from local help file %s (%d)", LocalFileName, errno);
	    ams_ReportError(ams_GetAMS(), ErrorText, ERR_WARNING, FALSE, 0);
	}
        unlink(LocalFileName);
    }
    ExposeCap(self);
}

QAddToDoc(d, pos, text, tlen, ss, stylelen)
struct text *d;
char *text;
int tlen, stylelen, *pos;
struct style *ss;
{
    struct environment *et;

    text_AlwaysInsertCharacters(d, *pos, text, tlen);
    if (ss) {
	et = environment_InsertStyle(((struct text *)d)->rootEnvironment, *pos, ss, 1);
	environment_SetLength(et, stylelen);
    }
    *pos += tlen;
}

folders__WriteFormattedBodyFile(self, fname, captbuf)
struct folders *self;
char *fname, *captbuf;
{
    FILE *fp;
    struct text *t;
    struct t822view *bv;
    struct captions *mycap;

    mycap = folders_GetCaptions(self);
    if (mycap->VisibleCUID >= 1) {
	strcpy(captbuf, AMS_CAPTION(mycap->VisibleSnapshot));
    } else {
	*captbuf = '\0';
    }
    ams_CUI_GenTmpFileName(ams_GetAMS(), fname);
    fp = fopen(fname, "w");
    if (!fp) return(-1);
    t = captions_GetBodDoc(mycap);
    bv = captions_GetBodView(mycap);
    text_SetWriteID(t, im_GetWriteID());
    if (t822view_GetDotLength(bv) > 0) {
	text_WriteSubString(t, t822view_GetDotPosition(bv), t822view_GetDotLength(bv), fp, TRUE);
    } else {
	text_WriteSubString(t, mycap->StartOfRealBody, text_GetLength(t) - mycap->StartOfRealBody, fp, TRUE);
    }
    return(fclose(fp));
}
