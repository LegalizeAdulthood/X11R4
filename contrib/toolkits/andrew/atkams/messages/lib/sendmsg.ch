/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/sendmsg.ch,v 2.10 89/06/22 17:46:27 cfe Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/sendmsg.ch,v $ */

#ifndef lint
static char *rcsid_sendmessage_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/sendmsg.ch,v 2.10 89/06/22 17:46:27 cfe Exp $ ";
#endif /* lint */

class sendmessage[sendmsg]: view {
overrides:
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);
    Update();
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    PostKeyState(struct keystate *keystate);
    PostMenus(struct menulist *menulist);
    WantUpdate(struct view *v);
    LinkTree(struct view *parent);
    UnlinkTree();
classprocedures:
    InitializeObject(struct sendmessage *self) returns boolean;
    FinalizeObject(struct sendmessage *self);
    InitializeClass() returns boolean;
methods:
    Clear() returns int ;
    ReadFromFile(char *SourceFile, Boolean DeleteIt) returns int ;
    Reset();
    AddHeaderLine(char *line) returns int ;
    AddToToHeader(char *line) returns int ;
    ResetSendingDot() returns int ;
    ResetFromParameters(char *ToName, char *Subject, char *CC, char *IncludeFile, int Delete) returns int ;
    Checkpoint() returns int ;
    WriteFile(char *FileName) returns int ;
    SetFoldersView(struct messages *mess) returns int ;
    CheckButtons();
    HasChanged() returns int;
    SetCurrentState(int state);
    AppendBugInfoToBody(int IsMessagesBug) returns int;
    AskEraseUnsentMail() returns int;
    QuoteBody();
    CheckRecipients() returns int;
    NewFoldersInNewWindow() returns struct folders *;
    NeedsCheckpointing() returns int;
data:
    struct lpair *SendLpair;
    struct scroll *HeadScroll, *BodyScroll;
    struct textview *HeadTextview, *BodyTextview;
    struct text *HeadText, *BodyText;
    struct butter *bclear, *bhide, *bblind, *breset, *bsign;
    struct butterview *bcv, *bhv, *bbv, *brv, *bsv;
    struct style *DefaultStyle, *DefaultHeadStyle, *BoldStyle;
    struct folders *folders;
    struct menulist *mymenulist, *myheadmenulist;
    struct keystate *keys, *headkeys;
    long HeadModified, BodyModified; /* stored mod #'s for texts */
    int HeadCheckpoint, BodyCheckpoint;  /* Value of above at time of last checkpoint */
    int NeedsTemplate, CurrentState;
    char *CKPFileName, *PSMsg;
    struct frame *myframe;
    int HasSigned;
};

