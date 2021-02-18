/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/supportviews/RCS/scroll.ch,v 2.9 89/08/02 16:59:19 mss Exp $ */
/* $ACIS:scroll.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/supportviews/RCS/scroll.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidscroll_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/supportviews/RCS/scroll.ch,v 2.9 89/08/02 16:59:19 mss Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


#define scroll_VERSION 3

/* The different locations scrollbars can appear in. For more than one scrollbar, or the values of the single underscore constants together. */
#define scroll_SIDES 4
#define scroll__LEFT 0
#define scroll__RIGHT 1
#define scroll__TOP 2
#define scroll__BOTTOM 3
#define scroll_LEFT (1<<scroll__LEFT)
#define scroll_RIGHT (1<<scroll__RIGHT)
#define scroll_TOP (1<<scroll__TOP)
#define scroll_BOTTOM (1<<scroll__BOTTOM)

/* The different types of scrollbars supported. Currently only vertical and horizontal */
#define scroll_TYPES 2
#define scroll_VERT 0
#define scroll_HORIZ 1

/* The endzone codes */
#define scroll_TOPENDZONE 1
#define scroll_BOTTOMENDZONE 2


struct scrollfns {
    void (*GetInfo)(/* self, total, seen, dot */);
    void (*SetFrame)(/* self, posn, coord, outof */);
    void (*EndZone)(/* self, zone, action */);
    long (*WhatIsAt)(/* self, coord, outof */);
};

struct range {
    long beg, end;		/* The beggining and the end of the range. */
};

struct scrollbar {
    struct range
        total,			/* The total range of the scrollee */
        seen,			/* The portion that is seen */
        dot;			/* The portion that is selected. */
    int endzones;		/* TRUE iff the endzones are displayed. */
};

struct scrollstate {
    int location;
    struct scrollbar bar[scroll_TYPES];
};

class scroll : view {
  overrides:
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);
    Update();
    Hit(enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    WantUpdate(struct view *requestor);
    LinkTree(struct view *parent);
    UnlinkNotification(struct view *unlinkedTree);

  methods:
    /* A ``location'' is a bitmask of scroll_LEFT, etc. describing the places for the scrollbars to appear. The location returned by GetLocation is the desired location, while GetCurrentLocation returns the real condititions based on the size of the region were in. */
    SetLocation(int location);
    GetLocation() returns int;
    GetCurrentLocation() returns int;

    /* The parameters consist of:
         endzone: The minimum number of pixles the scrollbar must have to display the endzones.
         bar: The minimum number of pixles to try to put a scrollbar in.
         without: The minimum size for the elevator given that the scrollbar is currently without endzones.
         with: The minimum size of the elevator when the scrollbar is displayed with endzones. */
    SetParameters(long endzone, long bar, int without, int with);
    GetParameters(long *endzone, long *bar, int *without, int *with);

    /* The ``child'' is the view directly under the scrollbars, while the ``scrollee'' is the view being scrolled. Normally, both of these are the same, and set with the SetView method. SetChild and SetScrollee can be used to change them individually. */
    SetView(struct view *view);
    SetChild(struct view *child);
    SetScrollee(struct view *scrollee);
    GetChild() returns struct view *;
    GetScrollee() returns struct view *;

  classprocedures:
    InitializeClass() returns boolean;

    /* Allocates a new set of scrollbars, and sets the view and location */
    Create(struct view *view, int location) returns struct scroll *;
    InitializeObject(struct scroll *self) returns boolean;
    FinalizeObject(struct scroll *self);

  data:
    struct view *child;
    struct view *scrollee;
    struct scrollstate current, desired;/* The state currently displayed and the state to display on the next update */

    struct scrollfns *fns[scroll_TYPES];/* Pointers to the interface structures. */

    struct updatelist *updatelist;	/* The list of all decendents that want updates. */

    long left, top, width, height;	/* The coords of the child inset rel to me. Not in the state structure because they are calculated from things in the state structure. */

/* Excuse the structure packing. Efficiency never hurts... */
    short endzone_threshold;		/* if height < this val, endzones disapear. */
    short bar_threshold;		/* if height < this val, bar disapears. */
    short min_elevator[2];		/* min size for elevator w/o and w/ endzones. */

    char pending_update;		/* TRUE iff we have an update that is pending */
    char ideal_location;		/* The ideal location(s) for the bars */
    char thumbing;
    char button;
    char force_full_update;             /* TRUE if we must do a full update. */
    char force_get_interface;           /* TRUE if the scrollee has changed since the last time we got the scroll functions. */
    short side;
    int hitcoord;
    int hitposn;

    struct cursor *cursor[scroll_SIDES];
    
    struct graphic *cornerFill;
    struct graphic *endzoneFill;
    struct graphic *barFill;
    struct graphic *elevatorFill;
    struct graphic *whiteFill;
    struct event *scrollEvent;
};

/* Fake a get view routine. */
#define scroll_GetView scroll_GetScrollee
