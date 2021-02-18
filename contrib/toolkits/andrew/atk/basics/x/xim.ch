/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/x/RCS/xim.ch,v 1.9 89/06/12 16:55:14 wjh Exp $ */
/* $ACIS:im.ch 1.5$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/x/RCS/xim.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidxim_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/x/RCS/xim.ch,v 1.9 89/06/12 16:55:14 wjh Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* im.H
 * Class header file for the interface manager.
 *
 *
 */

#define xim_PRORGAMMERVERSION 1



struct pane {
	char * paneStr;
	long priority;
	struct selection *firstSelection;
	struct pane *next;
};

struct selection  {
	char * selectionStr;
	long priority;
	struct proctable_Entry *proc;
	struct basicobject *object;
	long data;
	struct selection *next;
};

class xim : im  {

overrides:
	WhichWS() returns unsigned char *;
	PostMenus(struct menulist *menulist);
	PostCursor(struct rectangle *rec,struct cursor *cursor) ;
	ClearCursors(struct xcursor *C);
	UpdateCursors();
	SetTitle(char *titleString);
	FromCutBuffer() returns FILE *;
	CloseToCutBuffer(FILE *writeFile);
	RotateCutBuffers(long count);
	AppendToCutBuffer(FILE *writeFile);

	/* These are window manager interface calls */
	SetWMFocus();
	ExposeWindow();
	HideWindow();
	VanishWindow();
	HandleRedraw();
	CreateWindow(char * host) returns boolean;
	RedrawWindow();

classprocedures:
	InitializeClass() returns boolean;
	InitializeObject(struct xim *self) returns boolean;
	FinalizeObject(struct xim *self);
	HandleFiles(long WaitTime, boolean beCheap) returns boolean;
	FlushAllWindows();
	AddAndrewFontPath(Display * DisplayPtr);

data:
	struct pane *topPane;
	struct mouseStatus *MouseFacts;
	struct cmenu  *menu;
	struct mlcacheNode *mlcache;
	struct cacheregion *freeRegions, *activeRegions;
	Cursor StandardXCursor;
	Cursor globalXCursor;
	Window globalCursorWindow;
	struct xcursor *XimCursor;
	long Xfileno;
	long backgroundPixel;
	boolean EverMapped;	/* hack for first XMapWindow (for messages) */
	boolean CurrentlyMapped;
};

