/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/typescript/RCS/tscript.ch,v 2.8 89/11/06 11:31:21 tpn Exp $ */
/* $ACIS:tscript.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/typescript/RCS/tscript.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidtypescript_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/typescript/RCS/tscript.ch,v 2.8 89/11/06 11:31:21 tpn Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


#define READONLYMAX 65

/* Typescript menu masks. */
#define typescript_SelectionMenus 1   /* Menus to be posted when there is a selection region. */
#define typescript_NoSelectionMenus 2 /* Menus to be posted only when there is no
                                     * selection. Handles the bogus case of the
                                     * paste item which should show up all the
                                     * time.
                                     */
#define typescript_AfterFenceMenus 4 /* Menus to be posted for writable textviews. */

class typescript[tscript] : textview[textv] {
overrides:
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);
    Update();
    ReceiveInputFocus();
    ObservedChanged (struct thisobject *changed, long value);
    PostMenus(struct menulist *menulist);
    GetClickPosition(long position, long numberOfClicks, enum view_MouseAction action, long startLeft, long startRight, long *leftPos, long *rightPos);
    SetDataObject(struct dataobject *dataObject);

methods:
classprocedures:
    InitializeObject(struct typescript *self) returns boolean;
    Create(char **arglist,FILE *diskf,boolean filemenu) returns struct typescript *;
    CreatePipescript(FILE *indiskf, FILE *outdiskf, boolean filemenu) returns struct typescript *;
    InitializeClass() returns boolean;
    FinalizeObject(struct typescript *ap);
data:
    struct mark *cmdStart;	/* last place a new command was started */
    struct mark *lastCommand;	/* last place $= went to */
    struct environment *lastEnv; /* last environment $- went to */
    long lastPosition;		/* used for deciding to frame the dot when reading from the process */
    FILE *SCFile;               /* Sub Channel file * for BE 2file handler. */
    short SubChannel;           /* File descriptor of sub process i/o channel. */
    short readOnlyLen;		/* Number of charaters in read only buffer. -1 if not in read only mode. */
    char readOnlyBuf[READONLYMAX]; /* Place to gather characters when in read only (no echo) mode. */
    char OutputWait;
    char ChopLines;
    struct keystate *keystate;
    struct menulist *menulist;
#if 0
    struct menulist *selmenulist;
    struct menulist *fencedmenulist;
#endif /* 0 */
    char *progname;
    boolean pipescript;
#ifdef AIX
    int	pgrpid;	    /* process group id for signalling */
#endif
};
