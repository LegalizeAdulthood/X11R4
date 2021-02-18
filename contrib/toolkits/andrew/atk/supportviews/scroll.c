/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/supportviews/RCS/scroll.c,v 2.12 89/06/10 17:23:08 wjh Exp $ */
/* $ACIS:scroll.c 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/supportviews/RCS/scroll.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/supportviews/RCS/scroll.c,v 2.12 89/06/10 17:23:08 wjh Exp $";
#endif /* lint */

/* Scrollbar code for be2. */


#include <class.h>

#include <scroll.eh>

#include <graphic.ih>
#include <view.ih>
#include <updlist.ih>
#include <im.ih>
#include <cursor.ih>
#include <event.ih>
#include <environ.ih>

#include <point.h>
#include <rect.h>


/* The physical aspects of the scrollbar that cannot be changed dynamically */
#define ENDZONELENGTH 12
#define ENDTOBARSPACE 4
#define BARWIDTH 20
#define DOTLEFTEDGE 6
#define DOTRIGHTEDGE 12
#define MINDOTLENGTH 6
#define SMALLDIST 5

#define PIXELSPERINCH 75

/* The descriptions of the different types of scrollbars */
static int Type[scroll_SIDES] = {scroll_VERT, scroll_VERT, scroll_HORIZ, scroll_HORIZ},
    LeftChange[scroll_SIDES] = {BARWIDTH, 0, 0, 0},
    TopChange[scroll_SIDES] = {0, 0, BARWIDTH, 0},
    WidthChange[scroll_SIDES] = {-BARWIDTH, -BARWIDTH, 0, 0},
    HeightChange[scroll_SIDES] = {0, 0, -BARWIDTH, -BARWIDTH};
static char *InterfaceName[scroll_TYPES] = {"scroll,vertical", "scroll,horizontal"};


/* Icon infomation */
static short CursorIcon[scroll_TYPES], ThumbIcon;

static long startScrollTime;
static long minContScrollTime;
static long maxContScrollTime;
static boolean adjustScroll;
static boolean thumbScroll;

/* status codes for from_bar_to_range */
#define OUTSIDEBAR 0
#define INTOPZONE scroll_TOPENDZONE
#define INSIDEBAR scroll_BOTTOMENDZONE
#define INBOTZONE 3


/* The different buttons that can be down */
#define NEITHER 0
#define LEFT 1
#define RIGHT 2
#define SMOOTH 4


/* The different states the thumb can be in */
#define NOPE 0
#define MAYBE 1
#define YES 2


/* Useful macros */
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ABS(a) ((a) < 0 ? -(a) : (a))
#define SWAP(a,b) (temp)=(a);(a)=(b);(b)=(temp)


#define INTERSECT(top1, bot1, top2, bot2) ((bot1) >= (top2) && (top1) <= (bot2))

#if 0
static boolean INTERSECT(top1, bot1, top2, bot2)
long top1, top2, bot2, bot2;{
    /* which ordering of rectangle? */
    if (top1<top2) return bot1 > top2;
    else return bot2>top1;
}
#endif /* 0 */
#if 0
#define INTERSECT(top1, bot1, top2, bot2) ((bot1) > (top2) && (top1) < (bot2))
#endif /* 0 */

/* Variables used by elevator redraw code. Statics instead to parameters for speed. */

static struct {
    long top, bot;
    boolean whitep;
} range[2];
static long seen_top, seen_bot, left, right;

/* Creation and Destruction routines. */

boolean scroll__InitializeClass(classID)
struct classheader *classID;
{  
    CursorIcon[scroll_VERT] = Cursor_VerticalArrows;
    CursorIcon[scroll_HORIZ] = Cursor_HorizontalArrows;
    ThumbIcon = Cursor_Octagon;

    startScrollTime = environ_GetProfileInt("StartScrollTime", 0);
    minContScrollTime = environ_GetProfileInt("ContScrollTime", 500);
    maxContScrollTime = environ_GetProfileInt("MaxContScrollTime", minContScrollTime);
    adjustScroll = environ_GetProfileSwitch("AdjustScroll", FALSE);
    thumbScroll = environ_GetProfileSwitch("ThumbScroll", FALSE);

    return TRUE;
}

boolean scroll__InitializeObject(classID, self)
struct classheader *classID;
struct scroll *self;
{
    int i;

    self->current.location = 0;
    self->child = self->scrollee = NULL;
    for (i = 0; i < scroll_TYPES; i++) {
        struct scrollbar *bar = &self->current.bar[i];
        bar->total.beg = 0;
        bar->total.end = 1;
        bar->seen.beg = bar->seen.end = bar->dot.beg = bar->dot.end = -1;
    }

    self->desired = self->current;

    for (i = 0; i < scroll_TYPES; i++)
        self->fns[i] = NULL;

    self->pending_update = 0;
    self->updatelist = updatelist_New();
    self->left = self->top = self->width = self->height = 0;
    self->ideal_location = scroll_LEFT;
    self->endzone_threshold = 80;
    self->bar_threshold = 0;
    self->min_elevator[0] = 5;
    self->min_elevator[1] = 18;
    self->button = NEITHER;
    self->force_full_update = FALSE;
    self->force_get_interface = FALSE;
    for (i = 0; i < scroll_SIDES; i++) {
        self->cursor[i] = cursor_Create(self);
	cursor_SetStandard(self->cursor[i], CursorIcon[Type[i]]);
    }
    self->scrollEvent = NULL;

    return TRUE;
}
    
void scroll__FinalizeObject(classID, self)
struct classheader *classID;
struct scroll *self;
{
    int i;

    if (self->child != NULL)
        view_UnlinkTree(self->child);
    updatelist_Destroy(self->updatelist);
    for (i = 0; i < scroll_SIDES; i++)
        cursor_Destroy(self->cursor[i]);
}

struct scroll *scroll__Create(classID, scrollee, location)
struct classheader *classID;
struct view *scrollee;
int location;
{
    struct scroll *retval = scroll_New();

    scroll_SetView(retval, scrollee);
    scroll_SetLocation(retval, location);

    return retval;
}



/* State modification routines. */

void scroll__SetView(self, view)
struct scroll *self;
struct view *view;
{

    scroll_SetChild(self, view);
    scroll_SetScrollee(self, view);
}

void scroll__SetChild(self, child)
struct scroll *self;
struct view *child;
{
    if (self->child != child) {
        self->force_full_update = TRUE;
        if (self->child)
            view_UnlinkTree(self->child);
        self->child = child;
        if (child)
            view_LinkTree(child, self);
        self->thumbing = NOPE;
        scroll_WantUpdate(self, self);
    }
}

void scroll__SetScrollee(self, scrollee)
struct scroll *self;
struct view *scrollee;
{
    if (self->scrollee != scrollee) {
        self->force_get_interface = TRUE;
        self->scrollee = scrollee;
        self->thumbing = NOPE;
        scroll_WantUpdate(self, self);
    }
}

struct view *scroll__GetChild(self)
struct scroll *self;
{
    return self->child;
}

struct view *scroll__GetScrollee(self)
struct scroll *self;
{
    return self->scrollee;
}

void scroll__SetLocation(self, location)
struct scroll *self;
int location;
{
    self->ideal_location = location;
    scroll_WantUpdate(self, self);
}

int scroll__GetLocation(self)
struct scroll *self;
{
    return self->ideal_location;
}

int scroll__GetCurrentLocation(self)
struct scroll *self;
{
    return self->current.location;
}

void scroll__SetParameters(self, endzone, bar, without, with)
struct scroll *self;
long endzone, bar;
int without, with;
{
    self->endzone_threshold = endzone;
    self->bar_threshold = bar;
    self->min_elevator[0] = without;
    self->min_elevator[1] = with;
    scroll_WantUpdate(self, self);
}

void scroll__GetParameters(self, endzone, bar, without, with)
struct scroll *self;
long *endzone, *bar;
int *without, *with;
{
    *endzone = self->endzone_threshold;
    *bar = self->bar_threshold;
    *without = self->min_elevator[0];
    *with = self->min_elevator[1];
}

/* Interface routines. */

static void get_interface(self, type)
struct scroll *self;
int type;
{
    self->force_get_interface = FALSE;
    if (self->fns[type] == NULL)
        self->fns[type] = (struct scrollfns *)view_GetInterface(self->scrollee, InterfaceName[type]);
}

static void getinfo(self, type, total, seen, dot)
struct scroll *self;
int type;
struct range *total, *seen, *dot;
{
    void (*real_getinfo)();

    get_interface(self, type);

    if (self->fns[type] != NULL && (real_getinfo = self->fns[type]->GetInfo) != NULL)
        real_getinfo(self->scrollee, total, seen, dot);

    if (total->beg == total->end) {
        total->end++;
        *seen = *total;
        dot->beg = dot->end = total->beg;
    }
}

static void set_frame(self, side, posn, coord)
struct scroll *self;
int side;
int posn;
long coord;
{
    void (*real_setframe)();
    int type = Type[side];

    get_interface(self, type);

    if (self->fns[type] != NULL && (real_setframe = self->fns[type]->SetFrame) != NULL)
        real_setframe(self->scrollee, posn, coord, type == scroll_VERT ? self->height : self->width);
}

/* Calculation routines. */

static long bar_height(self, side)
struct scroll *self;
int side;
{
    switch (Type[side]) {
        case scroll_VERT: return self->height;
        case scroll_HORIZ: return self->width;
    }
}

static void endzone(self, side, end, action)
struct scroll *self;
int side;
int end;
enum view_MouseAction action;
{
    void (*real_endzone)();
    int type = Type[side];

    get_interface(self, type);

    if (self->fns[type] != NULL && (real_endzone = self->fns[type]->EndZone) != NULL)
        real_endzone(self->scrollee, end, action);
    else {
        if (end == scroll_TOPENDZONE)
            set_frame(self, side, self->desired.bar[type].total.beg, 0);
        else
            set_frame(self, side, self->desired.bar[type].total.end, bar_height(self, side) >> 2);
    }
}

static int what_is_at(self, side, coord)
struct scroll *self;
int side;
int coord;
{
    long (*real_what)();
    int type = Type[side];

    get_interface(self, type);

    if (self->fns[type] != NULL && (real_what = self->fns[type]->WhatIsAt) != NULL)
        return real_what(self->scrollee, coord, type == scroll_VERT ? self->height : self->width);
    else
        return 0;
}

static void rotate(self, side, x, y, scroll_x, scroll_y)
struct scroll *self;
int side;
long x, y, *scroll_x, *scroll_y;
{
    switch (side) {
        case scroll__LEFT:
            *scroll_x = x;
            *scroll_y = self->top + y;
            break;
        case scroll__RIGHT:
            *scroll_x = scroll_GetLogicalWidth(self) - 1 - x;
            *scroll_y = self->top + y;
            break;
        case scroll__TOP:
            *scroll_x = self->left + y;
            *scroll_y = x;
            break;
        case scroll__BOTTOM:
            *scroll_x = self->left + y;
            *scroll_y = scroll_GetLogicalHeight(self) - 1 - x;
            break;
    }
}

static long from_range_to_bar(self, side, bar, posn)
struct scroll *self;
int side;
struct scrollbar *bar;
long posn;
{
    int endzones = bar->endzones ? ENDZONELENGTH : 0;
    long cords = bar_height(self, side) - 2*(endzones + ENDTOBARSPACE) - 1;
    long retval = endzones + ENDTOBARSPACE 
		+ (posn - bar->total.beg) * cords / (bar->total.end - bar->total.beg);
    return retval;
}

static long from_bar_to_range(self, side, bar, posn, status)
struct scroll *self;
int side, *status;
struct scrollbar *bar;
long posn;
{
    int endzones = bar->endzones ? ENDZONELENGTH : 0;
    long height = bar_height(self, side),
        cords = height - 2*(endzones + ENDTOBARSPACE),
        retval = bar->total.beg + (bar->total.end - bar->total.beg) * (posn - ENDTOBARSPACE - endzones) / cords;

    if (posn < 0 || posn >= height)
        *status = OUTSIDEBAR;
    else if (posn < endzones)
        *status = INTOPZONE;
    else if (posn >= height - endzones)
        *status = INBOTZONE;
    else
        *status = INSIDEBAR;

    return retval;
}

static void calc_location(self)
struct scroll *self;
{
    int i, lastlocation;

    /* This routine generates the desired.location from the ideal_location. 
	In doing so, it destroys the left, top, width, and height fields. 
	This works because the two places this routine is called 
	(scroll__FullUpdate and scroll__Update) force a full update 
	of the child if  they have changed. */

    /* start with the ideal configuration. */
    self->desired.location = self->ideal_location;
    do {
        self->left = 0;
        self->top = 0;
        self->width = scroll_GetLogicalWidth(self);
        self->height = scroll_GetLogicalHeight(self);
        lastlocation = self->desired.location;

        for (i = 0; i < scroll_SIDES; i++)
            if (self->desired.location & (1<<i)) {
                if (bar_height(self, i) < self->bar_threshold) {
                    /* If the scrollbar doesn't fit, turn it off. */
                    self->desired.location &= ~(1<<i);
                }
                else {
                    /* Otherwise allocate it's space. */
                    self->left += LeftChange[i];
                    self->top += TopChange[i];
                    self->width += WidthChange[i];
                    self->height += HeightChange[i];
                }
            }

        if (self->width < 0 || self->height < 0) {
            /* Turn off all scrollbars if there is no area left over. Happens when you have a very tall and narrow window, for example */
            self->width = scroll_GetLogicalWidth(self);
            self->height = scroll_GetLogicalHeight(self);
            self->left = self->top = 0;
            self->desired.location = 0;
        }
    } while (lastlocation != self->desired.location);
    /* Keep turning more and more off until something works. 
	Guaranteed to stop sometime because there are a finite number 
	of scrollbars that can be turned off (since we never turn any back on...) */
}

static void calc_desired(self)
struct scroll *self;
{
    int i, exists[scroll_TYPES];

    for (i = 0; i < scroll_TYPES; i++) 
        exists[i] = 0;

    for (i = 0; i < scroll_SIDES; i++) 
        if (self->desired.location & (1<<i)) 
            exists[Type[i]] = 1;

    for (i = 0; i < scroll_TYPES; i++) 
        if (exists[i]) {
            struct scrollbar *bar = &self->desired.bar[i];
            getinfo(self, i, &bar->total, &bar->seen, &bar->dot);
        }
}


static int calc_dot(self, side, bar, x1, y1, x2, y2)
struct scroll *self;
int side;
struct scrollbar *bar;
long *x1, *y1, *x2, *y2;
{

    if (bar->dot.end < bar->dot.beg || bar->dot.end < bar->total.beg || bar->dot.beg > bar->total.end)
        return 0;

    *y1 = from_range_to_bar(self, side, bar, bar->dot.beg);
    *y2 = from_range_to_bar(self, side, bar, bar->dot.end);

    *x1 = DOTLEFTEDGE;
    *x2 = DOTRIGHTEDGE;

    if (bar->dot.beg == bar->dot.end) {
        *y1--;
        *y2 += 2;
    }
    else if (*y2 - *y1 < MINDOTLENGTH) {
        *y1 = (*y1 + *y2 - MINDOTLENGTH) / 2;
        *y2 = (*y1 + *y2 + MINDOTLENGTH) / 2;
    }

    return 1;
}

static int calc_elevator(self, side, bar, x1, y1, x2, y2)
struct scroll *self;
int side;
struct scrollbar *bar;
long *x1, *y1, *x2, *y2;
{
    int min,
        height = bar_height(self, side),
        extra = ENDTOBARSPACE + (bar->endzones ? ENDZONELENGTH : 0);

    if (bar->seen.end < bar->seen.beg || bar->seen.end < bar->total.beg || bar->seen.beg > bar->total.end)
        return 0;

    *y1 = from_range_to_bar(self, side, bar, bar->seen.beg);
    *y2 = from_range_to_bar(self, side, bar, bar->seen.end);

    *x1 = 0;
    *x2 = BARWIDTH - 2;

    if (bar->endzones)
        min = self->min_elevator[1];
    else
        min = self->min_elevator[0];

    if (min > height - 2*extra)
        min = height - 2*extra;

    if (*y2 - *y1 < min) {
        *y1 = (*y1 + *y2 - min) / 2;
        if (*y1 < extra) {
            *y1 = extra;
            *y2 = *y1 + min - 1;
        }
        else if (*y1 + min >= height - extra) {
            *y2 = height - extra - 1;
            *y1 = *y2 - min + 1;
        }
        else
            *y2 = *y1 + min - 1;
    }
    return 1;
} 

static void rectangle(self, x1, y1, x2, y2, tile)
struct scroll *self;
int x1, y1, x2, y2;
struct graphic *tile;
{
   struct rectangle rect;

    rectangle_SetRectSize(&rect, MIN(x1,x2), MIN(y1,y2), ABS(x1-x2) + 1, ABS(y1-y2) + 1);
    scroll_SetTransferMode(self, graphic_SOURCE);
/*    scroll_FillRectSize(self, MIN(x1,x2), MIN(y1,y2), ABS(x1-x2) + 1, ABS(y1-y2)  + 1, tile); */
    scroll_FillRect(self, &rect, tile);
}

static void draw_elevator(self, side)
struct scroll *self;
int side;
{
    long x1, y1, x2, y2;
    long left, top, width, height;
    struct scrollbar *bar = &self->desired.bar[Type[side]];

    if (calc_elevator(self, side, bar, &x1, &y1, &x2, &y2)) {

        rotate(self, side, x1, y1, &x1, &y1);
        rotate(self, side, x2, y2, &x2, &y2);

        rectangle(self, x1, y1, x2, y2, self->elevatorFill);
        scroll_SetTransferMode(self, graphic_BLACK);
	left = (x1<x2 ? x1 : x2);
	top =  (y1<y2 ? y1 : y2);
	width = abs(x1-x2);
	height = abs(y1-y2);
	scroll_DrawRectSize(self,left,top,width,height);
/*
        scroll_MoveTo(self, x1, y1);
        scroll_DrawLineTo(self, x2, y1);
        scroll_DrawLineTo(self, x2, y2);
        scroll_DrawLineTo(self, x1, y2);
        scroll_DrawLineTo(self, x1, y1);
*/
    }
}
    
static void draw_dot(self, side)
struct scroll *self;
int side;
{
    long x1, y1, x2, y2;
    long left, top, width, height;
    struct scrollbar *bar = &self->desired.bar[Type[side]];

    if (calc_dot(self, side, bar, &x1, &y1, &x2, &y2)) {
        rotate(self, side, x1, y1, &x1, &y1);
        rotate(self, side, x2, y2, &x2, &y2);

        scroll_SetTransferMode(self, graphic_BLACK);
	scroll_SetLineWidth(self, 2); 
	/* Note: we offset the left and top coordinate and decrement the width and height values so that we have a rectangle "centered" or "framed" along the rectangle path rather than traced like an ordinary polygon would be drawn (thus extra lines appear to go on the "inside" rather than along the left and top edges of the poolygon/rectangle */
	left = (x1<x2 ? x1 : x2) + 1;
	top =  (y1<y2 ? y1 : y2) + 1;
	width = abs(x1-x2) - 1;
	height = abs(y1-y2) - 1;
	scroll_DrawRectSize(self,left,top,width,height);
	scroll_SetLineWidth(self,1); 
/*
        switch (side) {
            case scroll__LEFT:
            case scroll__TOP:
                x1++; y1++; x2--; y2--;
                break;
            case scroll__RIGHT:
                x1--; y1++; x2++; y2--;
                break;
            case scroll__BOTTOM:
                x1++; y1--; x2--; y2++;
                break;
        }

	left++; top++; width -= 2; height -= 2;
	scroll_DrawRectSize(self,left,top,width,height);
*/
    }
}    

static void draw_whole_bar(self, side)
struct scroll *self;
int side;
{
    struct scrollbar *bar = &self->desired.bar[Type[side]];
    int height = bar_height(self, side);
    long x1, y1, x2, y2;

    scroll_SetTransferMode(self, graphic_BLACK);

    /* line between bar and child */
    rotate(self, side, BARWIDTH - 1, 0, &x1, &y1);
    scroll_MoveTo(self, x1, y1);
    rotate(self, side, BARWIDTH - 1, height /*- 1*/, &x1, &y1);
    scroll_DrawLineTo(self, x1, y1);

    if (bar->endzones) {
        rotate(self, side, 0, 0, &x1, &y1);
        rotate(self, side, BARWIDTH - 2, ENDZONELENGTH - 1, &x2, &y2);
        rectangle(self, x1, y1, x2, y2, self->endzoneFill);

        rotate(self, side, 0, height - ENDZONELENGTH, &x1, &y1);
        rotate(self, side, BARWIDTH - 2, height - 1, &x2, &y2);
        rectangle(self, x1, y1, x2, y2, self->endzoneFill);
    }

    rotate(self, side, 0, bar->endzones ? ENDZONELENGTH : 0, &x1, &y1);
    rotate(self, side, BARWIDTH - 2, 
		height - (bar->endzones ? ENDZONELENGTH : 0) - 1, &x2, &y2);
    rectangle(self, x1, y1, x2, y2, self->barFill);

    draw_elevator(self, side);
    draw_dot(self, side);
}

static void erase_dot(self, side, top, bot)
struct scroll *self;
int side;
long top, bot;
{
    int i;
    long x1, x2, y1, y2;

    if (top > bot)
        return;

    for (i = 0; i < 2; i++)
	if (INTERSECT(top, bot, range[i].top, range[i].bot)) {
	    erase_dot(self, side, top, range[i].top-1);
	    erase_dot(self, side, range[i].bot+1, bot);
	    return;
	}

    rotate(self, side, left, top, &x1, &y1);
    rotate(self, side, right, bot, &x2, &y2);
    rectangle(self, x1, y1, x2, y2, 
		INTERSECT(top, bot, seen_top, seen_bot) ? self->elevatorFill : self->barFill);
}

static void draw_end_line(self,x1,y1,x2,y2)
struct scroll * self;
long x1,y1,x2,y2; {
    long temp;
    /* first order everything */
    if (x1>x2) { temp = x1; x1 = x2; x2 = temp; }
    if (y1>y2) { temp = y1; y1 = y2; y2 = temp; }
    scroll_MoveTo(self,x1,y1);
    /* Horizontal or vertical? */
    if (x1 == x2) { /* vertical drawing */
	scroll_DrawLineTo(self,x2,y2+1);
    }
    else {  /* horizontal drawing */
	scroll_DrawLineTo(self,x2+1,y2);
    }
}

static void move_elevator(self, side)
struct scroll *self;
int side;
{
    struct scrollbar *des = &self->desired.bar[Type[side]],
        *cur = &self->current.bar[Type[side]];
    int dot, i;
    long temp, old_top, old_bot, top, bot;

    if (!calc_elevator(self, side, cur, &left, &range[0].top, &right, &range[0].bot)) {
        range[0].top = range[0].bot = -1;
    }
    range[0].whitep = 0;

    if (!calc_elevator(self, side, des, &left, &range[1].top, &right, &range[1].bot)) {
        range[1].top = range[1].bot = -1;
    }
    range[1].whitep = 1;
    seen_top = range[1].top;
    seen_bot = range[1].bot;

    if (INTERSECT(range[0].top, range[0].bot, range[1].top, range[1].bot)) {
        /* The old range and the new range overlap. We don't need to do all the work. */
        if (range[0].top < range[1].top) {
            SWAP(range[0].bot, range[1].top);
            if (range[1].bot < range[1].top) {
                SWAP(range[1].top, range[1].bot);
                range[1].whitep = 0;
            }
        }
        else {
            SWAP(range[0].top, range[1].bot);
            if (range[0].bot < range[0].top) {
                SWAP(range[0].top, range[0].bot);
                range[0].whitep = 1;
            }
        }
    }

    for (i = 0; i < 2; i++) {
        long ul_x, ul_y, ur_x, ur_y, ll_x, ll_y, lr_x, lr_y;
	struct point tmpPoints[4];

        if (range[i].top != range[i].bot) {
            rotate(self, side, left, range[i].top, &ul_x, &ul_y);
            rotate(self, side, right, range[i].top, &ur_x, &ur_y);
            rotate(self, side, left, range[i].bot, &ll_x, &ll_y);
            rotate(self, side, right, range[i].bot, &lr_x, &lr_y);
            if (range[i].whitep) {
                rectangle(self, ul_x, ul_y, lr_x, lr_y, self->elevatorFill);
                scroll_SetTransferMode(self, graphic_BLACK);
		/* One last pixel hack works for vertical scroll bars but I don't know about horizontal -- these calculations confuse me as to how they work horizontally -- maybe the rotations do work correctly, but it seems strange. */
#if 0
		/* Old way that almost works */
		OneLastYPixel = (seen_bot != range[i].bot);
                scroll_MoveTo(self, ul_x, ul_y);
                scroll_DrawLineTo(self, ll_x, ll_y + OneLastYPixel ); /* +OneLastYPixel X change */
                scroll_MoveTo(self, ur_x, ur_y);
                scroll_DrawLineTo(self, lr_x, lr_y + OneLastYPixel); /* +OneLastYPixel x change */
                if (seen_top == range[i].top) {
                    scroll_MoveTo(self, ul_x, ul_y);
                    scroll_DrawLineTo(self, ur_x, ur_y);
                }
                if (seen_bot == range[i].bot) {
                    scroll_MoveTo(self, ll_x, ll_y);
                    scroll_DrawLineTo(self, lr_x, lr_y);
                }
#endif /* 0 */
		/* end of old way that almost works */
		/* beginning of new way to be tested */
		/* See which of four cases we are interested in: two sides, top U, bottom U, all four -- note that because of the rotation transform, the hints "ul" for upper left, etc, have meaning only for vertical scrollbars */
		/* Vertical or horizontal? */
		if (ll_x != ul_x) { /* upper left and lower left not vertical => horizontal scroll bars */
		    if ((seen_top == range[i].top) && (seen_bot == range[i].bot) ) {
			/* All four, draw a rectangle */
			scroll_DrawRectSize(self,ur_x, ur_y, lr_x-ur_x, ul_y-ur_y);
		    }
		    else if (seen_top == range[i].top) {
			/* draw left U */
			point_SetPt(&tmpPoints[0],ll_x+1,ll_y);
			point_SetPt(&tmpPoints[1],ul_x,ul_y);
			point_SetPt(&tmpPoints[2],ur_x,ur_y);
			point_SetPt(&tmpPoints[3],lr_x+1,lr_y);
			scroll_DrawPath(self,tmpPoints,4);
		    }
		    else if (seen_bot == range[i].bot) {
			/* draw right U */
			point_SetPt(&tmpPoints[0],ur_x,ur_y);
			point_SetPt(&tmpPoints[1],lr_x,lr_y);
			point_SetPt(&tmpPoints[2],ll_x,ll_y);
			point_SetPt(&tmpPoints[3],ul_x,ul_y);
			scroll_DrawPath(self,tmpPoints,4);
		    }
		    else {
			/* draw two horizontal sides only */
			scroll_MoveTo(self, ul_x, ul_y);
			scroll_DrawLineTo(self, ll_x, ll_y);
			scroll_MoveTo(self, ur_x, ur_y);
			scroll_DrawLineTo(self, lr_x, lr_y);
		    }
		}
		else { /* vertical scroll bars */
		    if ((seen_top == range[i].top) && (seen_bot == range[i].bot) ) {
			/* All four, draw a rectangle  */
			scroll_DrawRectSize(self,ul_x, ul_y, ur_x-ul_x, ll_y-ul_y);
		    }
		    else if (seen_top == range[i].top) {
			/* draw top U */
			point_SetPt(&tmpPoints[0],ll_x,ll_y+1);
			point_SetPt(&tmpPoints[1],ul_x,ul_y);
			point_SetPt(&tmpPoints[2],ur_x,ur_y);
			point_SetPt(&tmpPoints[3],lr_x,lr_y+1);
			scroll_DrawPath(self,tmpPoints,4);
		    }
		    else if (seen_bot == range[i].bot) {
			/* draw bottom U */
			point_SetPt(&tmpPoints[0],ur_x,ur_y);
			point_SetPt(&tmpPoints[1],lr_x,lr_y);
			point_SetPt(&tmpPoints[2],ll_x,ll_y);
			point_SetPt(&tmpPoints[3],ul_x,ul_y);
			scroll_DrawPath(self,tmpPoints,4);
		    }
		    else {
			/* draw two vertical sides only */
			scroll_MoveTo(self, ul_x, ul_y);
			scroll_DrawLineTo(self, ll_x, ll_y + 1 );
			scroll_MoveTo(self, ur_x, ur_y);
			scroll_DrawLineTo(self, lr_x, lr_y + 1);
		    }
		}
	    } /* end of test for any white part to be drawn */
            else {
                rectangle(self, ul_x, ul_y, lr_x, lr_y, self->barFill);
                scroll_SetTransferMode(self, graphic_BLACK);
                if (seen_top == range[i].bot) {
		    draw_end_line(self,ll_x,ll_y,lr_x,lr_y);
/*                    scroll_MoveTo(self, ll_x, ll_y);
                    scroll_DrawLineTo(self, lr_x, lr_y);  */
                }
                if (seen_bot == range[i].top) {
		    draw_end_line(self,ul_x,ul_y,ur_x,ur_y);
/*                    scroll_MoveTo(self, ul_x, ul_y);
                    scroll_DrawLineTo(self, ur_x, ur_y); */
                }
            }
        }
    }

    dot = calc_dot(self, side, des, &left, &top, &right, &bot);
    if (calc_dot(self, side, cur, &left, &old_top, &right, &old_bot) && (!dot || top != old_top || bot != old_bot))
        erase_dot(self, side, old_top, old_bot);

    if (dot)
        draw_dot(self, side);
}


	static void 
full_update(self, type, left, top, width, height)
	struct scroll *self;
	enum view_UpdateType type;
	long left, top, width, height;
{
    int i;
    struct rectangle rect, crect;

    self->force_full_update = FALSE;
    if (self->force_get_interface)
        for (i = 0; i < scroll_TYPES; i++)
            self->fns[i] = NULL;

    if (self->child) {
        rectangle_SetRectSize(&rect, self->left, self->top, self->width, self->height);
        view_InsertView(self->child, self, &rect);
        rectangle_SetRectSize(&crect, left, top, width, height);
        rectangle_IntersectRect(&crect, &crect, &rect);

        view_FullUpdate(self->child, type, crect.left - self->left, crect.top - self->top,
				crect.width, crect.height);
        /* Is it possible that the scrollee has changed his mind about the scrollbar locations now that he has been redrawn? If so, we need to account for his area changing, and recalling his full update. Then what if he changes his mind again? */
        calc_desired(self);
    }

    if (type != view_FullRedraw && type != view_LastPartialRedraw)
	return;

    if (self->left > 0) {
        if (self->top > 0) {
            rectangle(self, 0, 0, self->left - 1, self->top - 1, self->cornerFill);
        }
        if (self->height + self->top < scroll_GetLogicalHeight(self)) {
            rectangle(self, 0, self->height + self->top, self->left - 1, scroll_GetLogicalHeight(self) - 1, self->cornerFill);
        }
    }
    if (self->width + self->left < scroll_GetLogicalWidth(self)) {
        int left = self->width + self->left;

        if (self->top > 0) {
            rectangle(self, left, 0, scroll_GetLogicalWidth(self) - 1, self->top - 1, self->cornerFill);
        }
        if (self->height + self->top < scroll_GetLogicalHeight(self)) {
            rectangle(self, left, self->height + self->top, scroll_GetLogicalWidth(self) - 1, scroll_GetLogicalHeight(self) - 1, self->cornerFill);
        }
    }

    for (i = 0; i < scroll_SIDES; i++)
        if (self->desired.location & (1<<i)) {
            self->desired.bar[Type[i]].endzones = bar_height(self, i) >= self->endzone_threshold;
            draw_whole_bar(self, i);
            switch (i) {
                case scroll__LEFT:
                    rectangle_SetRectSize(&rect, 0, self->top, BARWIDTH, self->height);
                    break;
                case scroll__RIGHT:
                    rectangle_SetRectSize(&rect, self->left + self->width, self->top, BARWIDTH, self->height);
                    break;
                case scroll__TOP:
                    rectangle_SetRectSize(&rect, self->left, 0, self->width, BARWIDTH);
                    break;
                case scroll__BOTTOM:
                    rectangle_SetRectSize(&rect, self->left, self->top + self->height, self->width, BARWIDTH);
                    break;
            }
            scroll_PostCursor(self, &rect, self->cursor[i]);
        }
        else
            scroll_RetractCursor(self, self->cursor[i]);

    self->current = self->desired;
    scroll_FlushGraphics(self);
}

/* Overrides of the view routines: */

void scroll__FullUpdate(self, type, left, top, width, height)
struct scroll *self;
enum view_UpdateType type;
long left, top, width, height;
{
    self->pending_update = 0;
    if (type == view_FullRedraw || type == view_LastPartialRedraw) {

	self->cornerFill = scroll_GrayPattern(self,8,16);
	self->endzoneFill = scroll_GrayPattern(self,2,16);
	self->barFill = scroll_GrayPattern(self,4,16);
	self->whiteFill =self->elevatorFill = scroll_WhitePattern(self);
	 
        calc_location(self);
        calc_desired(self);
    }
    full_update(self, type, left, top, width, height);
}

void scroll__Update(self)
struct scroll *self;
{
    int i;
    long l, t, w, h;

    l = scroll_GetVisualLeft(self);
    t = scroll_GetVisualRight(self);
    w = scroll_GetVisualWidth(self);
    h = scroll_GetVisualHeight(self);

    if (w == 0 || h == 0) return;

    self->pending_update = 0;
    /* Let the children modify their state however they want. */
    updatelist_Clear(self->updatelist);

    if (self->thumbing == YES)
	return;

    calc_location(self);
    calc_desired(self);

    /* Is the change so drastic that we need to start from scratch? */
    if (self->current.location != self->desired.location || self->force_full_update) {
        rectangle(self, l, t, w, h, self->whiteFill);
        full_update(self, view_FullRedraw, l, t, w, h);
    }
    else {
        for (i = 0; i < scroll_SIDES; i++)
            if (self->desired.location & (1<<i)) {
                int type = Type[i];
                struct scrollbar *des = &self->desired.bar[type], *cur = &self->current.bar[type];

                if (des->endzones != cur->endzones)
                    /* The entire scrollbar need redrawing? */
                    draw_whole_bar(self, i);
                else if (des->total.beg != cur->total.beg || des->total.end != cur->total.end || des->seen.beg != cur->seen.beg || des->dot.beg != cur->dot.beg || des->seen.end != cur->seen.end || des->dot.end != cur->dot.end) {
                    move_elevator(self, i);
                }
            }
        self->current = self->desired;
    }
}

void scroll__WantUpdate(self, requestor)
struct scroll *self;
struct view *requestor;
{
    if ((struct view *)self != requestor)
        updatelist_AddTo(self->updatelist, requestor);

    if (!self->pending_update) {
        self->pending_update = 1;
        super_WantUpdate((struct view *)self, self);
    }
}

static void RepeatScroll(self, cTime)
struct scroll *self;
long cTime;
{
    struct scrollbar *cur = NULL;
    long timeInterval;

    self->button |= SMOOTH;

    timeInterval = minContScrollTime + (((maxContScrollTime - minContScrollTime) * self->hitcoord) / (PIXELSPERINCH * 10));

    self->scrollEvent = im_EnqueueEvent(RepeatScroll, self, event_MSECtoTU(timeInterval));

    if (self->side != -1) {
        cur = &self->current.bar[Type[self->side]];
    }

    if (self->button == (LEFT | SMOOTH))
        set_frame(self, self->side, what_is_at(self, self->side, self->hitcoord), 0);
    else if (self->button == (RIGHT | SMOOTH))
	set_frame(self, self->side, cur->seen.beg, self->hitcoord);

    self->thumbing = NOPE;

    im_ForceUpdate();
}


struct view *scroll__Hit(self, action, x, y, num_clicks)
struct scroll *self;
enum view_MouseAction action;
long x, y, num_clicks;
{
    int posn = 0, status, side = 0, length, delta, i, endzones;
    long coord = 0, temp, y1, y2;
    struct scrollbar *cur = NULL, *des = NULL;

    if (action == view_LeftDown || action == view_RightDown) {
        if (self->button != NEITHER)
            return (struct view *)self;            /* We already are dealing with the other button. */

        if (x >= self->left 
		&& y >= self->top 
		&& x < self->left + self->width 
		&& y < self->top + self->height 
		&& self->child) {
            struct view *retval = view_Hit(self->child, action, x - self->left, y - self->top, num_clicks);
            return retval;
        }
        else if (x < self->left && (self->current.location & scroll_LEFT)) {
            cur = &self->current.bar[Type[side = scroll__LEFT]];
            posn = from_bar_to_range(self, side, cur, self->hitcoord = (y - self->top), &status);
        }
        else if (x >= self->left + self->width && (self->current.location & scroll_RIGHT)) {
            cur = &self->current.bar[Type[side = scroll__RIGHT]];
            posn = from_bar_to_range(self, side, cur, self->hitcoord = (y - self->top), &status);
        }
        else if (y < self->top && (self->current.location & scroll_TOP)) {
            cur = &self->current.bar[Type[side = scroll__TOP]];
            posn = from_bar_to_range(self, side, cur, self->hitcoord = (x - self->left), &status);
        }
        else if (y >= self->top + self->height && (self->current.location & scroll_BOTTOM)) {
            cur = &self->current.bar[Type[side = scroll__BOTTOM]];
            posn = from_bar_to_range(self, side, cur, self->hitcoord = (x - self->left), &status);
        }
        else
            status = OUTSIDEBAR;

        switch (status) {
            case OUTSIDEBAR:
                self->thumbing = NOPE;
/* These -1's are dangerous. I am fairly certain they don't get used outside
 * of this routine, and that they are handled properly within it. But you never
 * know...
 */
                self->side = -1;
                self->button = NEITHER;
                break;
            case INTOPZONE:
            case INBOTZONE:
                self->thumbing = NOPE;
                self->side = -1;
                self->button = NEITHER;
                endzone(self, side, status, action);
                break;
            case INSIDEBAR:
                if (calc_elevator(self, side, cur, &temp, &y1, &temp, &y2) && y1 <= self->hitcoord && self->hitcoord <= y2)
                    self->thumbing = MAYBE;
                else
                    self->thumbing = NOPE;
                self->side = side;

                if (action == view_LeftDown)
                    self->button = LEFT;
                else
                    self->button = RIGHT;
		self->hitposn = posn;

		if (startScrollTime > 0)  {
		    self->scrollEvent = im_EnqueueEvent(RepeatScroll, self, event_MSECtoTU(startScrollTime));
		}

                break;
        }
        return (struct view *)self;
    }

    /* Assure that we have the correct button */
    if (((action == view_LeftMovement || action == view_LeftUp) && ((self->button & LEFT) == 0)) || ((action == view_RightMovement || action == view_RightUp) && ((self->button & RIGHT) == 0)))
        return (struct view *)self;

    if (self->side != -1) {
        cur = &self->current.bar[Type[self->side]];
        des = &self->desired.bar[Type[self->side]];
	if (self->side == scroll__LEFT || self->side == scroll__RIGHT)
	    coord = y - self->top;
	else
	    coord = x - self->left;
    }

    if (action == view_LeftMovement || action == view_RightMovement) {
        switch (self->thumbing) {
            case YES:
                endzones = (cur->endzones ? ENDZONELENGTH : 0) + ENDTOBARSPACE;
                if (self->side == scroll__LEFT || self->side == scroll__RIGHT) {
                    if (y < self->top + endzones)
                        y = self->top + endzones;
                    if (y > self->top + self->height - endzones)
                        y = self->top + self->height - endzones;
                    posn = from_bar_to_range(self, self->side, cur, y - self->top, &status);
                }
                else {
                    if (x < self->left + endzones)
                        x = self->left + endzones;
                    if (x > self->left + self->width - endzones)
                        x = self->left + self->width - endzones;
                    posn = from_bar_to_range(self, self->side, cur, x - self->left, &status);
                }
                if (status == INSIDEBAR) {
                    delta = posn - self->hitposn;
                    length = des->seen.end - des->seen.beg;
                    des->seen.beg += delta;
                    if (des->seen.beg < des->total.beg) {
                        des->seen.beg = des->total.beg;
                        des->seen.end = MIN(des->total.end, des->seen.beg + length);
                    }
                    else if (des->seen.beg + length - 1 > des->total.end) {
                        des->seen.beg = MAX(des->total.end - length + 1, des->total.beg);
                        des->seen.end = des->total.end;
                    }
                    else
                        des->seen.end = des->seen.beg + length;

 
                    for (i = 0; i < scroll_SIDES; i++)  {
                        if ((self->current.location & (1<<i)) && Type[i] == Type[self->side])  {
			    move_elevator(self, i);
			    if (thumbScroll && self->button == LEFT)  {
				set_frame(self, self->side, des->seen.beg, 0);
			    }
			}
		    }
                    *cur = *des;
                    self->hitposn = posn;
                }
                break;
            case MAYBE:
                if (ABS(coord - self->hitcoord) > SMALLDIST) {
		    self->thumbing = YES;
		    if (self->scrollEvent != NULL) {
			event_Cancel(self->scrollEvent);
			self->scrollEvent = NULL;
		    }
                    cursor_SetStandard(self->cursor[self->side], ThumbIcon);
		    im_SetWindowCursor(self->header.view.imPtr, self->cursor[self->side]);
		}
		break;

	    case NOPE:
		if (adjustScroll)  {
		    self->hitcoord = coord;
		}
                break;
        }
        return (struct view *)self;
    }

    /* The action must be an up transition to get to here. */

    if (self->scrollEvent != NULL) {
	event_Cancel(self->scrollEvent);
	self->scrollEvent = NULL;
    }
    if (self->thumbing != NOPE) {
        cursor_SetStandard(self->cursor[self->side], CursorIcon[Type[self->side]]);
        im_SetWindowCursor(self->header.view.imPtr, NULL);

        if (ABS(coord - self->hitcoord) > SMALLDIST) {
            posn = from_bar_to_range(self, self->side, cur, coord, &status);
            if (status == INSIDEBAR) {
                delta = posn - self->hitposn;
                length = des->seen.end - des->seen.beg;
                des->seen.beg += delta;
                if (des->seen.beg < des->total.beg)
                    des->seen.beg = des->total.beg;
                else if (des->seen.beg + length - 1 > des->total.end)
                    des->seen.beg = MAX(des->total.end - length + 1, des->total.beg);
            }
            set_frame(self, self->side, des->seen.beg, 0);
            scroll_WantUpdate(self, self);

	    self->button = NEITHER;
	    self->thumbing = NOPE;

            return (struct view *)self;
        }
    }

    self->thumbing = NULL;

    if (adjustScroll)  {
	self->hitcoord = coord;
    }

    if (self->button == LEFT)
        set_frame(self, self->side, what_is_at(self, self->side, self->hitcoord), 0);
    else if (self->button == RIGHT)
        set_frame(self, self->side, cur->seen.beg, self->hitcoord);

    self->button = NEITHER;

    return (struct view *)self;
}

void scroll__LinkTree(self, parent)
    struct scroll *self;
    struct view *parent;
{

    super_LinkTree(self, parent);
    if (self->child)
        view_LinkTree(self->child, self);
}

void scroll__UnlinkNotification(self, unlinkedTree)
    struct scroll *self;
    struct view *unlinkedTree;
{

    updatelist_DeleteTree(self->updatelist, unlinkedTree);
    super_UnlinkNotification(self, unlinkedTree);
}
