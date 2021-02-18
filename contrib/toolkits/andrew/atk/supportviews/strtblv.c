/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/supportviews/RCS/strtblv.c,v 2.5 89/02/23 09:18:09 ghoti Exp $ */
/* $ACIS:strtblv.c 1.6$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/supportviews/RCS/strtblv.c,v $ */

#ifndef lint
	char *strtblview_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/supportviews/RCS/strtblv.c,v 2.5 89/02/23 09:18:09 ghoti Exp $";
#endif /* lint */

/* strtblv.c	

	The view module for the stringtbl dataobject

known problems:
	As the height gets too small, the number of columns reduces, 
		even though the objects could still fit in more columns
	the inset fails if invoked via .ezinit for file.stringtbl
*/

#include <class.h>
#include <rect.h>
#include <graphic.ih>
#include <im.ih>
#include <view.ih>
#include <fontdesc.ih>

#include <strtbl.ih>
#include <strtblv.eh>


/*
#include <menulist.ih>
#include <keymap.ih>
#include <keystate.ih>
*/

/*
static struct menulist  *class_menulist;
static struct keymap  *class_keymap;
static struct keystate  *class_keystate;
*/

#define BORDER 3		/* surrounding space */
#define INTER 3		/* interline spacing */

/* compute the parameters which say how big the largest item is */
static void
ComputeItemSize(self)
	register struct strtblview *self;
{
	register struct stringtbl *st 
			= (struct stringtbl *)self->header.view.dataobject;
	short n = stringtbl_NStrings(st);
	register short maxwidth = 0;
	short i;
	register struct graphic *g;
	struct fontdesc *fd;
	struct FontSummary *FS;
	g = strtblview_GetDrawable(self);
	fd = strtblview_GetFont(self);
	FS = fontdesc_FontSummary(fd, g);
	for (i = n; i--; ) {
		long w, h;
		fontdesc_StringSize(fd, g, stringtbl_IthString(st, i), &w, &h);
		if (w > maxwidth) maxwidth = w;
	}
	self->itemheight = FS->maxHeight;
	self->maxwidth = maxwidth;
	self->sizeknown = TRUE;
}

/* ComputeOrganization determines a good number of rows and columns
	for a given width and height of area */
static
ComputeOrganization (self, width, height)
	register struct strtblview *self;
	short width, height;
{
	/* the algorithm is to minimize the difference between inter-column white space
		and inter-row whitespace.  This is done by starting with one row, a situation
		with maximum inter-row whitespace and minimum inter-column spacing.
		Then we add one row at a time until the intercolumn spacing exceeds the
		inter-row spacing.  
			XXX this is a lousy algorithm.  It will compute with two columns for 
			about half the entries.  */
	short r, c;
	register struct stringtbl *st 
			= (struct stringtbl *)self->header.view.dataobject;
	short n = stringtbl_NStrings(st);
	if ( ! self->sizeknown) 
		ComputeItemSize(self);
	if (n <= 0) {
		self->rows = self->cols = 1;  /* avoid divide by zero */
		return;
	}
	r = 1;	/* start with one row of all elements */
	while (TRUE) {
		short colwhite, rowwhite;
		c = (n+r-1) / r;			/* compute number of columns */
		if (c <= 1) break;
		colwhite = (width - c*self->maxwidth) / c;
		rowwhite = (height - r*self->itemheight) / r;
		if (colwhite > rowwhite) 
			break;
		r ++;
	}
	self->rows = r;
	self->cols = (c<1) ? 1 : c;
}

/* iRect converts its argument rectangle from the 
		entire logical image to the image for the i'th string */
static
iRect(self, i, r)
	register struct strtblview   *self;
	short i;
	register struct rectangle *r;
{
	short col = i / self->rows;	/* strings run vertically */
	short row = i % self->rows;
	r->height /= self->rows;
	r->top += row * r->height;
	r->width /= self->cols;
	r->left += col * r->width;
	r->width -= 1;
	r->height -= 1;
}
#ifdef NOTUSED
static void
AddItem_Command( self )
	register struct strtblview  *self;
{

	/* XXX  prompt for string to add */

	strtblview_WantUpdate( self, self );
}
#endif /* NOTUSED ? */
boolean
strtblview__InitializeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct strtblview  *self;
{
	self->OnScreen = FALSE;
	self->GaveSize = FALSE;
	self->sizeknown = FALSE;
	self->tablechanged = FALSE;
	self->maxused = 0;
	self->ItemHitProc = NULL;
	return TRUE;
}

void 
strtblview__FinalizeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct strtblview  *self;
{
}

void 
strtblview__Clear(self)
	register struct strtblview  *self;
{
	register struct stringtbl *st 
			= (struct stringtbl *)self->header.view.dataobject;
	self->sizeknown = FALSE;
	self->GaveSize = FALSE;
	self->tablechanged = TRUE;
	self->maxused = 0;
	if (st)
		stringtbl_Clear(st);
}

void
strtblview__ObservedChanged(self, dobj, status)
	register struct strtblview  *self;
	struct stringtbl *dobj;
	long status;
{
	if (status == stringtbl_STRINGSCHANGED) {
		self->sizeknown = FALSE;
		self->GaveSize = FALSE;
		self->tablechanged = TRUE;
		strtblview_WantNewSize(self, self);
	}
	else if (status == observable_OBJECTDESTROYED) 
		return;	/* deleting it */
	strtblview_WantUpdate(self, self);
}


static boolean
BogusCallFromParent(self, where, msg)
	register struct strtblview *self;
	char *where, *msg;
{
	fprintf(stderr, "<strtblview>Bogus call to %s, %s\n", where, msg);
	return FALSE;
}

static boolean
CheckWindow(self, where)
	register struct strtblview *self;
	char *where;
{
	register struct graphic *g
		= (struct graphic *)strtblview_GetDrawable(self);
	if ( ! g) return BogusCallFromParent(self, where, "No Graphic");
	return TRUE;
}

static void 
AdjustHighlight(self)
	register struct strtblview *self;
{
	register struct stringtbl *st 
			= (struct stringtbl *)self->header.view.dataobject;
	unsigned long which = self->BlackOnes ^ st->highlight;
		/* WARNING: direct use of st->highlight could be a problem if the
		data object has changed asynchronously with the view */
	struct rectangle rectangle, r;
	short i, n;

	strtblview_SetTransferMode(self, graphic_INVERT);
	strtblview_GetLogicalBounds(self, &rectangle);
	n = stringtbl_NStrings(st);
	for (i = 0; i < n; i++) {
		if (which & (1<<i)) {
			r = rectangle;
			iRect(self, i, &r);
			strtblview_FillOval(self, &r, self->BlackPattern);
		}
	}
	self->BlackOnes = st->highlight;
}

static void
RedrawTable(self)
	register struct strtblview *self;
{
	register struct stringtbl *st 
			= (struct stringtbl *)self->header.view.dataobject;
	struct rectangle rectangle, r;
	short i, n;
	
	strtblview_GetLogicalBounds(self, &rectangle);
	if (rectangle.width == 0 || rectangle.height == 0) return;
	if ( ! self->sizeknown)
		ComputeOrganization(self, rectangle.width, rectangle.height);
	strtblview_SetTransferMode(self, graphic_COPY);
	strtblview_FillRect(self, &rectangle, self->WhitePattern);

	n = stringtbl_NStrings(st);
	for (i = 0; i < n; i++) {
		r = rectangle;
		iRect(self, i, &r);
		strtblview_MoveTo(self, 
				r.left + (r.width>>1), 
				r.top + (r.height>>1));
		strtblview_DrawString(self, stringtbl_IthString(st, i), 
				graphic_BETWEENLEFTANDRIGHT |
				   graphic_BETWEENTOPANDBASELINE);
	}
	self->tablechanged = FALSE;
	self->BlackOnes = 0;
	AdjustHighlight(self);
}

void 
strtblview__FullUpdate( self, type, left, top, width, height )
	register struct strtblview  *self;
	register enum view_UpdateType  type;
	register long  left, top, width, height;
{
	if (type == view_Remove) {
		self->OnScreen = FALSE;
		return;
	}
	if ( ! CheckWindow(self, "FullUpdate")) return;
	if ((type != view_FullRedraw 
				&& type != view_LastPartialRedraw)
			|| strtblview_GetLogicalWidth(self) == 0 
			|| strtblview_GetLogicalHeight(self) == 0) 
		return;
	self->OnScreen = TRUE;
	if (type == view_FullRedraw) {
		self->WhitePattern = strtblview_WhitePattern(self);
		self->BlackPattern = strtblview_BlackPattern(self);
	}
	if ( ! self->GaveSize)
		strtblview_WantNewSize(self, self);
	RedrawTable(self);
}


void 
strtblview__Update( self )
	register struct strtblview *self;
{
	if (! self->OnScreen || ! CheckWindow(self, "Update")) return;
	if (self->tablechanged)
		RedrawTable(self);
	else
		AdjustHighlight(self);
}

struct view *
strtblview__Hit(self, action, x, y, num_clicks)
	register struct strtblview  *self;
	register enum view_MouseAction  action;
	register long  x, y, num_clicks;
{
	if (action == view_NoMouseEvent)
		return (struct view *)self;
	if (! self->OnScreen || ! CheckWindow(self, "Hit")) return NULL;
	if ( action == view_LeftDown || action == view_RightDown ) {
		register struct stringtbl *st 
				= (struct stringtbl *)self->header.view.dataobject;
		short i, n = stringtbl_NStrings(st);
		struct rectangle r;
		strtblview_GetLogicalBounds(self, &r);
		if (r.width < self->cols) 
			i = 1;
		else
			i = x / (r.width / self->cols) * self->rows
				+ y / (r.height / self->rows);
/*
printf("i %d  for hit at (%d, %d)  in box at (%d,%d) of size %dx%d\n", 
	i, x, y, r.top, r.left, r.width, r.height);  fflush(stdout);
*/
		if (i < n) {
			if (self->ItemHitProc)
				(self->ItemHitProc)(st, self->ItemHitRock, st->accmap[i] + 0L);
			else  {
				/* Implement a ZeroOrOne strategy if no ItemHitProc */
				stringtbl_ClearBits(st);
				if (((self->BlackOnes>>i) & 1) == 0) 
					stringtbl_SetBit(st, stringtbl_IthString(st, i), 1);
			}
			AdjustHighlight(self);
		}
		strtblview_WantInputFocus(self, self);
	}
	return (struct view *)self;		/* where to send subsequent hits */
}

enum view_DSattributes
strtblview__DesiredSize( self, width, height, pass, 
				desiredWidth, desiredHeight ) 
	register struct strtblview *self;
	long width;
	long height;
	enum view_DSpass pass;
	long *desiredWidth;
	long *desiredHeight;
{
	register struct stringtbl *st 
			= (struct stringtbl *)self->header.view.dataobject;
	short n = stringtbl_NStrings(st);
	if (n > self->maxused)  self->maxused = n;
	if ( ! CheckWindow(self, "DSize")) {
		/* can't compute size without a window (we need a font)
			give dummy values */
		*desiredWidth = 85;
		*desiredHeight = 60;
		return view_HeightFlexible | view_WidthFlexible;
	}

	/* compute the dimensions of the biggest string */
	if ( ! self->sizeknown ) 
		ComputeItemSize(self);

	/* the following three if sections set self->rows and self->cols */
	if (pass == view_NoSet) {
		if (30<width && width<1000 && 30<height && height<1000)
			ComputeOrganization(self, width, height);
		else if (self->itemheight < 5)
			self->rows = self->cols = 1;
		else {
			/* invalid width or height, choose good ones */
			register long r, t = n * self->maxwidth / self->itemheight;
			register short i;
			/* square root of t is a good number of rows */
			for (i = 1, r = t>>2; i<6; i++)
				/* iterate newton-raphson */
				r = (r + t/r)>>1;
			if (r < 1) r = 1;
			if (r > self->maxused) r = self->maxused;
			self->rows = r;
			self->cols = (self->maxused + r - 1) / r;
		}
	}
	else if (pass == view_WidthSet) {
		self->cols = (width - 2*BORDER) / (self->maxwidth + INTER);
		if (self->cols < 1) self->cols = 1;
		if (self->cols > self->maxused) self->cols = self->maxused;
		self->rows = (self->maxused + self->cols - 1) / self->cols;
	}
	else /* pass == view_HeightSet */ {
		self->rows = (height - 2*BORDER) / (self->itemheight + INTER);
		if (self->rows < 1) self->rows = 1;
		if (self->rows > self->maxused) self->rows = self->maxused;
		self->cols = (self->maxused + self->rows - 1) / self->rows;
	}
	/* now determine height, width, and return value from rows & cols */
	*desiredHeight = self->rows * (self->itemheight + INTER)
					 + 2*BORDER - INTER;
	*desiredWidth = self->cols * (self->maxwidth + INTER)
					 + 2*BORDER - INTER;
/*
fprintf(stderr, "input %dx%d    rows: %d   cols: %d   output %dx%d\n", 
width, height, self->rows, self->cols, *desiredWidth, *desiredHeight); fflush(stderr);
*/
	self->GaveSize = TRUE;
	{
/* with the following, the compiler complains about incompatible operands to |= 
*		enum view_DSattributes ret = view_HeightLarger | view_WidthLarger;
*		if (self->rows > 1) ret |= view_HeightSmaller;
*		if (self->cols >1) ret |= view_WidthSmaller;
*		return ret;
*/
		if (self->rows > 1 && self->cols > 1) 
			return view_HeightFlexible | view_WidthFlexible;
		if (self->rows > 1) 
			return view_HeightFlexible | view_WidthLarger;
		if (self->cols > 1)  
			return view_HeightLarger | view_WidthFlexible;
		return  view_HeightLarger | view_WidthLarger;
	}
}

void
strtblview__Print( self, file, processor, format, level )
	register struct strtblview 	 *self;
	register FILE   file;
	register char  	 *processor;
	register char  	 *format;
	register boolean  	level;
{
	
}

/* OneOnly - is a generic hitproc for a string table which guarantees that exactly
	one bit is always on in the table */
void
strtblview__OneOnly(ClassID, st, stv, accnum)
	struct classhdr *ClassID;
	struct stringtbl *st;
	struct strtblview *stv;
	short accnum;
{
	if (0 == stringtbl_GetBitOfEntry(st, accnum)) {
		stringtbl_ClearBits(st);
		stringtbl_SetBitOfEntry(st, accnum, 1);
	}
}

/* ZeroOrMany - is a generic hitproc for a string table 
		which allows any number of bits to be on */
void
strtblview__ZeroOrMany(ClassID, st, stv, accnum)
	struct classhdr *ClassID;
	struct stringtbl *st;
	struct strtblview *stv;
	short accnum;
{
	stringtbl_SetBitOfEntry(st, accnum, ! stringtbl_GetBitOfEntry(st, accnum));
}


/* ZeroOrOne - is a generic hitproc for a string table 
		which guarantees that at most one bit is on */
void
strtblview__ZeroOrOne(ClassID, st, stv, accnum)
	struct classhdr *ClassID;
	struct stringtbl *st;
	struct strtblview *stv;
	short accnum;
{
	if (stringtbl_GetBitOfEntry(st, accnum))
		stringtbl_ClearBits(st);
	else {
		stringtbl_ClearBits(st);
		stringtbl_SetBitOfEntry(st, accnum, 1);
	}
}
