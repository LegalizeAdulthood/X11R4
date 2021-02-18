/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/textv.ch,v 2.8 89/09/08 16:52:22 ghoti Exp $ */
/* $ACIS:textv.ch 1.7$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/textv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidtextview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/textv.ch,v 2.8 89/09/08 16:52:22 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#define textview_VERSION 1

enum textview_MovementUnits  {
    textview_MoveByPixels,
    textview_MoveByLines
};

enum textview_ScrollDirection  {
    textview_NoScroll,
    textview_ScrollForward,
    textview_ScrollBackward,
    textview_MultipleScroll
};

enum textview_LineRedrawType {
    textview_FullLineRedraw,
    textview_GetPosition,
    textview_GetCoordinate,
    textview_GetHeight,
    textview_MoveView,
    textview_PartialLineRedraw
};

struct linedesc  {
    short y;				/* last position drawn */
    short height;			/* height used during last draw */
    short textheight;                   /* textheight used during last draw */
    short xMax;				/* farthest printed */
    short nChars;			/* length of the line in chars */
    boolean containsView;		/* true if this line contains a view */
    struct mark *data;			/* the range of the text storing the data */
};

#define textview_UNKNOWNVIEW 1
#define textview_REMOVEVIEW 0L

/* Menu mask bits. */
#define textview_NoMenus -1         /* Used as a flag indicating no menus are posted. Minor hack. */
#define textview_SelectionMenus 1   /* Menus to be posted when there is a selection region. */
#define textview_NoSelectionMenus 2 /* Menus to be posted only when there is no
                                     * selection. Handles the bogus case of the
                                     * paste item which should show up all the
                                     * time.
                                     */
#define textview_NotReadOnlyMenus 4 /* Menus to be posted for writable textviews. */

/* Key (as in third argument to menulist_ChainML) under which style menus are
 * chained to normal menulist.
 */
#define textview_StyleMenusKey 1

class textview[textv] : view {
overrides:
    WantUpdate(struct view *requestor);
    SetDataObject(struct dataobject *dataObject);
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long height);
    Update();
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    ReceiveInputFocus();
    LoseInputFocus();
    GetApplicationLayer() returns struct view *;
    DeleteApplicationLayer(struct view *scrollbar);
    GetInterface(char *interfaceName) returns char *;
    Print(FILE *file, char *processor, char *finalFormat, boolean topLevel);
    WantNewSize(struct view *requestor);
    ObservedChanged (struct observable *changed, long value);
    LinkTree(struct view *parent);
    DesiredSize(long width, long height, enum view_DSpass pass, long *dWidth, long *dheight) returns enum view_DSattributes;
    GetOrigin(long width, long height, long *originX, long *originY);
    InitChildren();
    CanView(char *TypeName) returns boolean;
methods:
    SetDotPosition(long newPosition);
    SetDotLength(long newLength);
    GetDotPosition() returns long;
    GetDotLength() returns long;
    GetTopPosition() returns long;
    SetTopPosition(long newTopPosition);
    SetBorder(long xBorder, long yBorder);
    SetEmbeddedBorder(long xBorder, long yBorder);
    CollapseDot();
    GetClickPosition(long position, long numberOfClicks, enum view_MouseAction action, long startLeft, long startRight, long *leftPos, long *rightPos);
    Visible(long pos) returns boolean;
    Locate(long x, long y, struct view **view) returns long;
    GetTextSize(long *width, long *height);
    MoveBack(long pos, long units, enum textview_MovementUnits type, long *distMoved, long *linesAdded) returns long;
    MoveForward(long pos, long units, enum textview_MovementUnits type, long *distMoved, long *linesAdded) returns long;
    FrameDot(long pos);
    FindLineNumber(long pos) returns long;
    LineRedraw(enum textview_LineRedrawType type, struct mark currentLine, long x, long y, long xSize, long ySize, long search, boolean *cont,long *textheight, struct formattinginfo *info) returns long;
    LookCmd(int look);
    SetDefaultStyle(struct style *styelptr);
    GetDefaultStyle() returns struct style *;
    ViewMove(struct mark *currentLine,long movement);
classprocedures:
    FinalizeObject(struct textview *self);
    InitializeClass() returns boolean;
    InitializeObject(struct textview *self) returns boolean;
data:
    short aLines;				/* allocated lines */
    short nLines;				/* number of lines currently shown */
    struct linedesc *lines;			/* the view image */
    short bx;				/* the border size - when using application layer*/
    short by;
    short ebx;				/* the border size when embedded */
    short eby;
    boolean hasApplicationLayer;
    boolean hasInputFocus;			
    boolean zapRest;				/* true if hit file's eof in redisplay */
    boolean force;				/* should we force a full redraw? */
    struct mark *dot;
    struct mark *top;
    struct mark *frameDot;
    struct keystate *keystate;
    struct menulist *styleMenus, *menus;	/* style menus are only present when dot is non-zero length. */
    short clearFrom;				/* y last cleared from */
    short csxPos;				/* old cursor x position */
    short csyPos;				/* old cursor y posn (-1 -> never shown) */
    short cshPos;				/* old cursor line height */
    short csbPos;				/* old cursor linebelow */
    short cexPos;				/* old cursor x position */
    short ceyPos;				/* old cursor y posn (-1 -> never shown) */
    short cehPos;				/* old cursor line height */
    short cebPos;				/* old cursor linebelow */
    enum textview_ScrollDirection scroll;
    int scrollLine;				/* line to use for scrolling. */
    int scrollDist;				/* distance to scroll */
    boolean needUpdate;
    int movePosition;
    long lastStyleMenuVersion;			/* Version number of stylesheet from which menus were last posted. */
    struct mark *atMarker;
    struct style *defaultStyle;			/* style from preferences */
    struct viewref *currentViewreference; /* Used for creating new views on existing dataobjects */
    long displayLength;
    struct environment *displayEnvironment;
    long displayEnvironmentPosition;
    long tabWidth;
    boolean exposeStyles;
    long predrawnY,predrawnX; 
    struct mark	*predrawn;			/* text preceding the dot that may avoid redrawing */
    struct mark	*prepredrawn;			/* text preceeding predrawn that may avoid redrawing,
							 used if the predrawn mark is modified */

};

