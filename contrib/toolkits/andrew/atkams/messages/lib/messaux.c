/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/messaux.c,v 1.3 89/10/09 22:04:30 wjh Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/messaux.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/messaux.c,v 1.3 89/10/09 22:04:30 wjh Exp $ ";
#endif /* lint */

#include <andrewos.h>
#include <sys/param.h>
#include <cui.h>
#include <fdphack.h>
#include <errprntf.h>
#include <class.h>
#include <keymap.ih>
#include <keystate.ih>
#include <menulist.ih>
#include <bind.ih>
#include <text.ih>
#include <textv.ih>
#include <environ.ih>

#include <ams.ih>
#include <amsutil.ih>
#include <text822.ih>
#include <text822v.ih>
#include <captions.ih>
#include <folders.ih>
#include <sendmsg.ih>
#define AUXMODULE 1
#include <messages.eh>
#undef AUXMODULE

extern char *index(), *rindex();

extern         void AlterSubByName();
extern         void AppendMarked();
extern         void AppendMarkedToFile();
extern         AppendMessageToFile();
extern         void BS_AppendDelPlease();
extern         void BS_AppendOnto();
extern         void BS_AppendPlease();
extern         void BS_CheckNewPlease();
extern         void BS_ClassifyPlease();
extern         void BS_CopyPlease();
extern         void BS_CreatePlease();
extern         void BS_DeleteFolder();
extern         void BS_DeletePlease();
extern         void BS_DifferentContentType();
extern         void BS_DummyQuit();
extern         void BS_FileInto();
extern         void BS_MarkCurrent();
extern         void BS_ModifiableBody();
extern         void BS_MorePlease();
extern         void BS_NextPlease();
extern         void BS_ReSendPlease();
extern         void BS_ReadMailPlease();
extern         void BS_RedisplayFixedWidth();
extern         void BS_RedisplayFormat();
extern         void BS_RedisplayNormal();
extern         void BS_RedisplayRot13();
extern         void BS_RefreshDisplayedFolder();
extern         void BS_RenamePlease();
extern         void BS_ReplyToAll();
extern         void BS_ReplyToBoth();
extern         void BS_ReplyToReaders();
extern         void BS_ReplyToSender();
extern         void BS_RestoreAsDraft();
extern         void BS_ScrollBackBody();
extern         void BS_ScrollForwardBody();
extern         void BS_SelectBody();
extern         void BS_SendForward();
extern         void BS_SendFresh();
extern         void BS_SetPrinter();
extern         void BS_ShowAllPlease();
extern         void BS_ShowHelp();
extern         void BS_ShowNewPlease();
extern         void BS_ShowPersonalPlease();
extern         void BS_ShowSubscribedPlease();
extern         void BS_ShowTreePlease();
extern         void BS_UndeletePlease();
extern         void BSearchFPlease();
extern         void BSearchRPlease();
extern         void BackUp();
extern         void CSearchFPlease();
extern         void CSearchRPlease();
extern         void ClassifyMarked();
extern         void ClearMarks();
extern         boolean ClearSM();
extern         void CopyMarked();
extern         void CountMarks();
extern         void DeleteMarked();
extern         void DeleteWindow();
extern         DirectlyClassify();
extern         void DuplicateWindow();
extern         void ExcerptMarked();
extern         void ExpandFileIntoMenus();
extern         void FSearchFPlease();
extern         void FSearchRPlease();
extern         void FileByName();
extern         void FileIntoByName();
extern         void FileOntoByName();
extern         void FindAllCaptions();
extern         void FindRelatedMessages();
extern         void GSearchFPlease();
extern         void GSearchRPlease();
extern         GenNodeName();
extern         struct t822view *GetBodies();
extern         struct captions *GetCaptions();
extern         struct captions *GetCaptionsNoCreate();
extern         GetFolderName();
extern         struct folders *GetFolders();
extern         char *GetLastResendName();
extern         void MarkRange();
extern         void MarkVisibleMessageUnseen();
extern         void MessagesBodiesCommand();
extern         void MessagesCaptionsCommand();
extern         void MessagesCompound();
extern         void MessagesFocusBodies();
extern         void MessagesFocusCaptions();
extern         void MessagesFocusFolders();
extern         void MessagesFoldersCommand();
extern         void MessagesSendmessageCommand();
extern         void MessagesTextviewCommand();
extern         void NextMarked();
extern         NoOp();
extern         OrgHit();
extern         void PrevMarked();
extern         void PrintMarked();
extern         void PrintVisibleMessage();
extern         void PuntCurrent();
extern         void PurgeAllDeletions();
extern         void QuitMessages();
extern         ReadByName();
extern         ReadNamedFolder();
extern         void ReplyAllMarked();
extern         void ReplySendersMarked();
extern         void ResendMarked();
extern         void RestoreOldMarks();
extern         void SetMessagesOptions();
extern         SetSubStatus();
extern         void ShrinkFileIntoMenus();
extern         void SubscribeByName();
extern         void TextviewCompound();
extern         void ThisIsFlorida();
extern         void UnSubscribeByName();
extern         void UndeleteMarked();
extern         countdots();
extern         int messages__AppendOneMessageToFile();
extern         void messages__SetWhatIAm();

extern int (*textview_ForwardSearchCmd)(), (*textview_ReverseSearchCmd)(), (*textview_ScrollScreenBackCmd)(), (*textview_ScrollScreenForwardCmd)();

void
messages__PostKeyState(self, ks)
struct messages *self;
struct keystate *ks;
{
    if (!ks) return;
    if (amsutil_GetOptBit(EXP_KEYSTROKES) 
		&& (self->WhatIAm != WHATIAM_BODIES
		    || text_GetReadOnly(
			(struct text *) t822view_GetDataObject(GetBodies(self))
		))) {
	keystate_AddBefore(self->mykeys, ks);
	super_PostKeyState(self, self->mykeys);
    } else {
	super_PostKeyState(self, ks);
    }
}

void messages__FinalizeObject(c, self)
struct classheader *c;
struct messages *self;
{
    menulist_Destroy(self->mymenulist);
    keystate_Destroy(self->mykeys);
    if (self->fileintomenulist) menulist_Destroy(self->fileintomenulist);
}

void messages__PostMenus(mess, ml)
struct messages *mess;
struct menulist *ml;
{
    CheckMenuMasks(mess);
    menulist_ClearChain(mess->mymenulist);
    if (ml) menulist_ChainAfterML(mess->mymenulist, ml, ml);
    if (mess->fileintomenulist) {
	menulist_ClearChain(mess->fileintomenulist);
	menulist_ChainAfterML(mess->mymenulist, mess->fileintomenulist, mess->fileintomenulist);
    }
    super_PostMenus(mess, mess->mymenulist);
}

struct captions *GetCaptions(self)
struct messages *self;
{
    struct captions *c = NULL;
    switch(self->WhatIAm) {
	case WHATIAM_FOLDERS:
	    c = folders_GetCaptions((struct folders *) self);
	    break;
	case WHATIAM_CAPTIONS:
	    c = (struct captions *) self;
	    break;
	case WHATIAM_BODIES:
	    c = t822view_GetCaptions((struct t822view *) self);
	    break;
    }
    if (!c) {
	ams_ReportError(ams_GetAMS(), "Out of memory; a core dump is imminent.", ERR_FATAL, FALSE, 0);
    }
    return(c);
}

struct t822view *GetBodies(self)
struct messages *self;
{
    struct t822view *tv = NULL;
    switch(self->WhatIAm) {
	case WHATIAM_FOLDERS:
	    tv = captions_GetBodView(folders_GetCaptions((struct folders *) self));
	    break;
	case WHATIAM_CAPTIONS:
	    tv = captions_GetBodView((struct captions *) self);
	    break;
	case WHATIAM_BODIES:
	    tv = (struct t822view *) self;
	    break;
    }
    if (!tv) {
	ams_ReportError(ams_GetAMS(), "Out of memory; a core dump is imminent.", ERR_FATAL, FALSE, 0);
    }
    return(tv);
}

void MessagesFocusFolders(self)
struct messages *self;
{
    struct folders *f = GetFolders(self);
    folders_WantInputFocus(f, f);
}

void MessagesSendmessageCommand(self, cmds)
struct messages *self;
char *cmds;
{
    struct sendmessage *sm = folders_ExposeSend(GetFolders(self));
    if (sm) {
	ams_GenericCompoundAction(ams_GetAMS(), sm, "sendmessage", cmds);
    }
}

void BS_ShowHelp(self)
struct messages *self;
{
    if (GetCaptions(self)->FullName) {
	folders_ExplainDir(GetFolders(self), GetCaptions(self)->FullName, GetCaptions(self)->ShortName);
    } else {
	t822view_ShowHelp(GetBodies(self), NULL);
    }
}

boolean ClearSM(self)
struct captions *self;
{
    struct sendmessage *sm = folders_ExposeSend(captions_GetFolders(self));
    if (!sm) return(TRUE);
    sendmessage_Reset(sm);
    if (sendmessage_HasChanged(sm)) return(TRUE);
    return(FALSE);
}

void BS_CheckNewPlease(self)
struct messages *self;
{
    folders_UpdateMsgs(GetFolders(self), 0, NULL, TRUE);
}

void BS_ReadMailPlease(self)
struct messages *self;
{
    folders_ReadMail(GetFolders(self), TRUE);
}

void BS_ShowNewPlease(self)
struct messages *self;
{
    folders_Reconfigure(GetFolders(self), LIST_NEWONLY);
}

void BS_ShowPersonalPlease(self)
struct messages *self;
{
    folders_Reconfigure(GetFolders(self), LIST_MAIL_FOLDERS);
}

void BS_ShowAllPlease(self)
struct messages *self;
{
    folders_Reconfigure(GetFolders(self), LIST_ALL_FOLDERS);
}

void BS_ShowSubscribedPlease(self)
struct messages *self;
{
    folders_Reconfigure(GetFolders(self), LIST_SUBSCRIBED);
}

void DuplicateWindow(self)
struct messages *self;
{
    if (self->WhatIAm == WHATIAM_FOLDERS) {
	struct folders *f = folders_New();

	ams_InstallInNewWindow(folders_GetApplicationLayer(f), "messages-folders", "Message Folders", environ_GetProfileInt("folders.width", 600), environ_GetProfileInt("folders.height", 120), f);
    } else if (self->WhatIAm == WHATIAM_CAPTIONS) {
	struct captions *f = captions_New();

	ams_InstallInNewWindow(captions_GetApplicationLayer(f), "messages-captions", "Message Captions", environ_GetProfileInt("captions.width", 600), environ_GetProfileInt("captions.height", 120), f);
    } else if (self->WhatIAm == WHATIAM_BODIES) {
	struct t822view *f = t822view_New();
	struct text *t = text_New();

	t822view_SetDataObject(f, t);
	ams_InstallInNewWindow(t822view_GetApplicationLayer(f), "messages-bodies", "Message Bodies", environ_GetProfileInt("bodies.width", 600), environ_GetProfileInt("bodies.height", 120), f);
    }
	
}

struct captions *GetCaptionsNoCreate(self)
struct messages *self;
{
    struct captions *c = NULL;
    switch(self->WhatIAm) {
	case WHATIAM_FOLDERS:
	    c = ((struct folders *) self)->mycaps;
	    break;
	case WHATIAM_CAPTIONS:
	    c = (struct captions *) self;
	    break;
	case WHATIAM_BODIES:
	    c = ((struct t822view *) self)->mycaps;
	    break;
    }
    return(c);
}

struct folders *GetFolders(self)
struct messages *self;
{
    struct folders *f = NULL;
    switch(self->WhatIAm) {
	case WHATIAM_FOLDERS:
	    f = (struct folders *) self;
	    break;
	case WHATIAM_CAPTIONS:
	    f = captions_GetFolders((struct captions *) self);
	    break;
	case WHATIAM_BODIES:
	    f = captions_GetFolders(t822view_GetCaptions((struct t822view *) self));
	    break;
    }
    if (!f) {
	ams_ReportError(ams_GetAMS(), "Out of memory; a core dump is imminent.", ERR_FATAL, FALSE, 0);
    }
    return(f);
}

void MessagesFoldersCommand(self, cmds)
struct messages *self;
char *cmds;
{
    ams_GenericCompoundAction(ams_GetAMS(), GetFolders(self), "folders", cmds);
}

void FSearchFPlease(self)
struct messages *self;
{
    textview_ForwardSearchCmd((struct textview *) GetFolders(self));
    messages_WantInputFocus(self, self);
}

void FSearchRPlease(self)
struct messages *self;
{
    textview_ReverseSearchCmd((struct textview *) GetFolders(self));
    messages_WantInputFocus(self, self);
}

