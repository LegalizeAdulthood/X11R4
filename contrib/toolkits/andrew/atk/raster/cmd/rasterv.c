/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/raster/cmd/RCS/rasterv.c,v 2.13 89/10/13 16:07:24 msp Exp $ */
/* $ACIS:rasterv.c 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/raster/cmd/RCS/rasterv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/raster/cmd/RCS/rasterv.c,v 2.13 89/10/13 16:07:24 msp Exp $";
#endif /* lint */

/* Some comments on changes checked in on 8/10/88
>> SetPixel:
>>    The pix local variable is modified (somehow) by the
>>    call to graphic_SetBitAtLoc. Line after this call
>>    changed to use the value of raster_GetPix rather then pix.
>> Changed all occurances of:
>>    pixelimage_DATACHANGED --> raster_BITSCHANGED
>>    rasterimage_DATACHANGED --> raster_BITSCHANGED
>> ScaleCommand:
>>    Keeps copy of original pixels when scaling. Thus if one scales
>>    and then back up, the code extrapolates from the original rather
>>    then the scaled down version --> eliminates massive loss of
>>    information.
>> Post Menus:
>>    Reorganized to avoid duplication of code and to include several
>>    items when InMessages.
>> rasterview_FinalizeObject:
>>    Added: If original is non-NULL then
>> 		Destroy original and set original equal to NULL
>> rasterview_SetDataObject:
>>    Added Debug statement.
>> rasterview_ObservedChanged:
>>    When BITSCHANGED or BOUNDSCHANGED
>> 	if original is non-NULL then
>> 		Destroy original and set original equal to NULL
>>    Added: rasterview_SCALECHANGED.
>> rasterview_FullUpdate:
>>    if the ViewSelection is Empty then
>> 	set both the ViewSelection and DesiredSelection to the subraster.
*/

/* rasterv.c	

	The view module for the raster dataobject

*/
/* Work needed
	Needs input/output of X rasters.
*/

#include <class.h>
#include <im.ih>
#include <graphic.ih>
#include <frame.ih>
#include <buffer.ih>

#include <view.ih>
#include <fontdesc.ih>
#include <andrewos.h> /* strings.h */
#include <sys/param.h> /* Defines MAXPATHLEN among other things */

#include <menulist.ih>
#include <keymap.ih>
#include <keystate.ih>
#include <cursor.ih>
/* #include <bind.ih> */
#include <proctbl.ih>
#include <message.ih>
#include <complete.ih>
#include <scroll.ih>
#include <environ.ih>

#include <rect.h>
#include <txttroff.ih>

#include <rastimg.ih>
#include <raster.ih>
#include <rasterv.eh>
#include <heximage.ih>
#include <paint.ih>
#include <oldrf.ih>

#include <dispbox.h>

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *\
 *
 *	Supporting procedures
 *	
\* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#define	RegionSelectMode    0
#define	TouchUpMode	    1
#define	PanMode		    2

#define RegionSelect(self) ((self)->Mode == RegionSelectMode)
#define TouchUp(self) ((self)->Mode == TouchUpMode)
#define Pan(self) ((self)->Mode == PanMode)

#define FullSize(self) ((self)->Scale == 1)
#define NotFullSize(self) ((self)->Scale != 1)

static void ToggleDebug();

boolean FindFrameHelp(frame, im)
struct frame *frame;
struct im *im;
{
    return (frame_GetIM(frame) == im);
}

void CurrentDirectory(self, f)
struct rasterview *self;
char *f;
{
    struct frame *frame = frame_Enumerate(FindFrameHelp, rasterview_GetIM(self));
    struct buffer *buffer = frame_GetBuffer(frame);
    if (frame != NULL && buffer != NULL) {
	strcpy(f, buffer_GetFilename(buffer));
	DEBUG(("CurrrentFilename: '%s'\n", f));
	if (strlen(f) != 0) {
	    char *slash;
	    slash = rindex(f, '/');
	    if (slash != NULL)
		slash[1] = '\0'; }
	else {
	    im_GetDirectory(f);
	    strcat(f, "/"); } }
    else {
	im_GetDirectory(f);
	strcat(f, "/"); }

    DEBUG(("Return value: '%s'\n", f));
}

/* Uncropped(self) 
	tests to see if the rasterview is an uncropped version of the whole
	that is, the ViewSelection selects the entire rastimg.
*/
#define Cropped(self)							\
 raster_GetWidth((struct raster *)rasterview_GetDataObject(self)) !=	\
     rectangle_Width(&self->ViewSelection)				\
 || raster_GetHeight((struct raster *)rasterview_GetDataObject(self)) !=	\
	rectangle_Height(&self->ViewSelection)

#define Uncropped(self)							\
 raster_GetWidth((struct raster *)rasterview_GetDataObject(self)) ==	\
     rectangle_Width(&self->ViewSelection)				\
 && raster_GetHeight((struct raster *)rasterview_GetDataObject(self)) ==	\
	rectangle_Height(&self->ViewSelection)

/* FullySelected(self) 
	tests to see if the DesiredSelection selects the entire current ViewSelection.
*/
#define FullySelected(self)						\
 rectangle_IsEqualRect(&self->ViewSelection, &self->DesiredSelection)

void CenterViewSelection(self)
struct rasterview *self;
{
    struct rectangle VB;
    rasterview_GetVisualBounds(self, &VB);
    DEBUG(("Centering VB: (%d,%d,%d,%d)\n",
	    rectangle_Left(&VB), rectangle_Top(&VB),
	    rectangle_Width(&VB), rectangle_Height(&VB)));
    self->Xscroll =
      (rectangle_Width(&self->ViewSelection)*self->Scale - rectangle_Width(&VB))/2;
    self->Yscroll =
      (rectangle_Height(&self->ViewSelection)*self->Scale - rectangle_Height(&VB))/2;
}

/* selection
	The selected subrectangle is highlighted with a two pixel border,
	the inner pixel is white and the outer is black.
	
	To make room for this, the requested space for the image
	leaves room for two pixels of white around the image on all sides.
*/

/* ViewHideHighlight(self)
	repaints the selection area with the correct bits.
	paints white if painting in the two pixel border
	sets CurSelection to empty,
		but leaves DesiredSelection alone

	Strategy: paint the two pixel borders as though they did not overlap 
	the two pixel white space surrounding the image.
	Then test for overlap and, if any, repaint the entire 
	surrounding whitespace.
*/

static void ViewHideHighlight(self)
struct rasterview *self;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    struct rasterimage *pix;
    struct graphic *G = rasterview_GetDrawable(self);
    long Xoff = self->Xoff, Yoff = self->Yoff;
    long clipw, cliph;

    if (ras == NULL || (pix = raster_GetPix(ras)) == NULL) return;
    if (IsEmptyRect(&self->CurSelection)) return;
    if (NotFullSize(self)) pix = self->Expansion;

    clipw = rasterimage_GetWidth(pix);
    cliph = rasterimage_GetHeight(pix);

    if (self->DisplayBoxOverlapped)
	DisplayBoxHideOverlappingHighlight(self, G, pix);
    else {
	long l, t, w, h;			/* dimensions of selection */
	long leftThick, rightThick;		/* thickness of line to repair */
	long topThick, botThick;
	long vsl, vst, vsw, vsh;		/* dimensions of self->ViewSelection. */
	struct rectangle CS;

	CS = self->CurSelection;

	/* restore bits from image */

	if (NotFullSize(self)) {
	    struct rectangle R;
	    R = self->DisplayBoxSelection;
	    rectangle_IntersectRect(&R, &R, &CS);
	    if (IsEmptyRect(&R)) return;
	    l = (rectangle_Left(&R) - rectangle_Left(&self->DisplayBoxSelection)) * self->Scale;
	    t = (rectangle_Top(&R) - rectangle_Top(&self->DisplayBoxSelection)) * self->Scale;
	    w = rectangle_Width(&R)*self->Scale;
	    h = rectangle_Height(&R)*self->Scale;
	    vsl = vst = 0;
	    vsw = rectangle_Width(&self->DisplayBoxSelection)*self->Scale;
	    vsh = rectangle_Height(&self->DisplayBoxSelection)*self->Scale;
	    rectangle_SetRectSize(&CS, l, t, w, h);
	}
	else {
	    rectangle_GetRectSize(&CS, &l, &t, &w, &h);
	    rectangle_GetRectSize(&self->ViewSelection, &vsl, &vst, &vsw, &vsh); }

	/* we have to do white first, because writepiximage is bogus (because defined transfer modes are bogus). Draw one white line overlapping the black border */
	DrawHighlight(self, G, CS, graphic_WHITE, -1);

	DEBUG(("CurSel: (%d,%d,%d,%d)\n", l, t, w, h));
	DEBUG(("VSel: (%d,%d,%d,%d)\n", vsl, vst, vsw, vsh));

	rasterview_SetTransferMode(self, graphic_COPY);

	/* repair left edge */
	leftThick =  (l-vsl <= 1) ? l - vsl : 2;
	if (leftThick) 
	    /* repaint left edge with pixels (do not do top and bottom corners) */
	    ClipAndWritePixImage(clipw, cliph,
				 G, l-leftThick-Xoff, t-Yoff, pix,
				 l-leftThick, t, leftThick, h);

	/* repair right edge */
	rightThick = (vsl+vsw) - (l+w);
	if (rightThick > 2) rightThick = 2;
	if (rightThick) 
	    /* redraw pixels of right edge (excluding corners) */
	    ClipAndWritePixImage(clipw, cliph,
				 G, l+w-Xoff, t-Yoff, pix, l+w, t, rightThick, h);

	/* repair top edge */
	topThick =  (t-vst <= 1) ? t-vst : 2;
	if (topThick) 
	    /* redraw pixels of top edge (including corners) */
	    ClipAndWritePixImage(clipw, cliph,
				 G, l-leftThick-Xoff, t-topThick-Yoff, pix,
				 l-leftThick, t-topThick,
				 w+leftThick+rightThick, topThick);

	/* repair bottom edge */
	botThick = (vst+vsh) - (t+h);
	if (botThick > 2) botThick = 2;
	if (botThick) 
	    /* redraw pixels of bottom edge (including corners) */
	    ClipAndWritePixImage(clipw, cliph,
				 G, l-leftThick-Xoff, t+h-Yoff, pix,
				 l-leftThick, t+h,
				 w+leftThick+rightThick, botThick);
    }
    if (NotFullSize(self))
	DisplayBoxHideHighlight(self, G);

    rectangle_EmptyRect(&self->CurSelection);
}

/* CorrectHighlight(self) 
	checks the selection highlighting parameters, CurSelection and DesiredSelection.
	If they differ, hides the old highlight and draws the new

	Knows about input focus and uses a grey highlight if we haven't got it
*/
static void CorrectHighlight(self) 
struct rasterview *self;
{
    struct graphic *G = rasterview_GetDrawable(self);
    struct rectangle *CS = &self->CurSelection;
    struct rectangle *DS = &self->DesiredSelection;

    if (rectangle_IsEqualRect(CS, DS)
	 && self->HighlightIsGrey == self->HasInputFocus)
	/* highlight is correct */
	return;

    if (IsNotEmptyRect(&self->CurSelection))
	ViewHideHighlight(self);	/* remove the old highlight */

    self->HighlightIsGrey = self->HasInputFocus;
    *CS = *DS;
    if (IsEmptyRect(CS)) {
	/* There is no current selection.  This happens when there is no raster.  To show the user where we are, draw a rect.  Use Visual Bounds. */
	struct rectangle VB;
	rasterview_GetVisualBounds(self, &VB);
	rasterview_SetTransferMode(self, graphic_BLACK);
	SetWidthRect(&VB, rectangle_Width(&VB)-1);
	SetHeightRect(&VB, rectangle_Height(&VB)-1);
	graphic_DrawRect(G, &VB);
	return; }

    if (! TouchUp(self)) {
	if (self->HasInputFocus) {
	    /* draw black and white lines */
	    if (NotFullSize(self)) {
		DrawHighlightBehindDisplayBox(self, G, 0);
		DisplayBoxDrawHighlight(self, G); }
	    else
		DrawHighlightBlackAndWhite(self, G, self->DesiredSelection);
	}
	else {
	    /* draw grey border */
	    if (NotFullSize(self)) {
		DrawHighlightBehindDisplayBox(self, G, 1);
		DisplayBoxDrawHighlightGray(self, G); }
	    else {
		long l, t, w, h;
		rectangle_GetRectSize(&self->DesiredSelection, &l, &t, &w, &h);
		l -= self->Xoff;  /* convert to screen coords */
		t -= self->Yoff;
		rasterview_SetTransferMode(self, graphic_COPY);
		graphic_FillRectSize(G, l-2, t-2, w+4, 2, self->GreyPattern);
		graphic_FillRectSize(G, l-2, t+h, w+4, 2, self->GreyPattern);
		graphic_FillRectSize(G, l-2, t, 2, h, self->GreyPattern);
		graphic_FillRectSize(G, l+w, t, 2, h, self->GreyPattern); }
	}
    }
}

#define CalculatePanHighlight(self, R) {					\
if (FullSize((self)))								\
    *(R) = (self)->ViewSelection;						\
else {										\
    *(R) = (self)->ViewSelection;						\
    rectangle_SetRectSize((R),							\
			   (rectangle_Left((R)) - rectangle_Left(&(self)->DisplayBoxSelection)) * (self)->Scale,					\
			   (rectangle_Top((R)) - rectangle_Top(&(self)->DisplayBoxSelection)) * (self)->Scale,					\
			   rectangle_Width((R)) * (self)->Scale,		\
			   rectangle_Height((R)) * (self)->Scale); } }

void DrawPanHighlight(self, g)
struct rasterview *self;
short g;
{
    struct graphic *G = rasterview_GetDrawable(self);

    DEBUG(("Drawing Pan Highlight\n"));
    if (g == graphic_BLACK) {
	if (self->DisplayBoxHidden) {
	    struct rectangle PH;
	    CalculatePanHighlight(self, &PH);
	    DrawHighlightBlackAndWhite(self, G, PH); }
	else {
	    struct rectangle savedDS;
	    savedDS = self->DesiredSelection;
	    self->DesiredSelection = self->ViewSelection;
	    DrawHighlightBehindDisplayBox(self, G, 0);
	    DisplayBoxDrawHighlight(self, G);
	    self->DesiredSelection = savedDS; } }
    else {
	struct rectangle R;
	CalculatePanHighlight(self, &R);
	DrawHighlightWhite(self, G, R);
	if (! self->DisplayBoxHidden) {
	    DisplayBoxWritePixImageFull(self, G,
					raster_GetPix((struct raster *) rasterview_GetDataObject(self))); }
    }
    if (self->embedded) {
	struct rectangle VB;
	rasterview_GetVisualBounds(self, &VB);
	InsetRect(&VB, BORDER, BORDER);
	DrawHighlightScreenCoordinates(self, rasterview_GetDrawable(self), VB,
				       graphic_BLACK, -1); }
}

/* Draw a Target across the entire Visual Bounds less a BORDER all around. Target is three pixels wide: a black line surrounded by two white lines.
*/
void DrawTarget(self, x, y)
struct rasterview *self;
long x, y;
{
    struct graphic *G = rasterview_GetDrawable(self);
    struct rectangle VB;
    struct rectangle PH;
    long w, h;

    rasterview_GetVisualBounds(self, &VB);
    InsetRect(&VB, BORDER, BORDER);
    w = rectangle_Width(&VB);
    h = rectangle_Height(&VB);

    rasterview_SetTransferMode(self, graphic_BLACK);
    graphic_MoveTo(G, x, BORDER);
    graphic_DrawLineTo(G, x, h);
    graphic_MoveTo(G, BORDER, y);
    graphic_DrawLineTo(G, w, y);

    CorrectHighlight(self);

    CalculatePanHighlight(self, &PH);
    OffsetRect(&PH, - self->Xoff - BORDER,- self->Yoff - BORDER)

    rectangle_IntersectRect(&VB, &VB, &PH);
    if (IsNotEmptyRect(&VB)) {
	long l, t, w, h, r, b;
	rectangle_GetRectSize(&VB, &l, &t, &w, &h);
	l++; t++;
	r = l + w;
	b = t + h;
	DEBUG(("White Target: (%d,%d,%d,%d)\n", l, t, w, h));
	rasterview_SetTransferMode(self, graphic_WHITE);
	graphic_MoveTo(G, x+1, t);
	graphic_DrawLineTo(G, x+1, y-1);
	graphic_DrawLineTo(G, r, y-1);
	graphic_MoveTo(G, r, y+1);
	graphic_DrawLineTo(G, x+1, y+1);
	graphic_DrawLineTo(G, x+1, b);
	graphic_MoveTo(G, x-1, b);
	graphic_DrawLineTo(G, x-1, y+1);
	graphic_DrawLineTo(G, l, y+1);
	graphic_MoveTo(G, l, y-1);
	graphic_DrawLineTo(G, x-1, y-1);
	graphic_DrawLineTo(G, x-1, t);

    }

    /* Just in case any of the above white lines or the preceeding Hide overlapped the Start. */
    rasterview_SetTransferMode(self, graphic_BLACK);
    graphic_MoveTo(G, self->StartPanX, BORDER);
    graphic_DrawLineTo(G, self->StartPanX, h);
    graphic_MoveTo(G, BORDER, self->StartPanY);
    graphic_DrawLineTo(G, w, self->StartPanY);
}

void HideTarget(self, x, y)
struct rasterview *self;
long x, y;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    struct rasterimage *pix;
    struct graphic *G = rasterview_GetDrawable(self);
    long clipw, cliph;
    struct rectangle VB;
    struct rectangle VS;
    long vsl, vst, vsw, vsh, vsr, vsb;

    if (ras == NULL || (pix = raster_GetPix(ras)) == NULL) return;
    if (NotFullSize(self)) pix = self->Expansion;

    clipw = rasterimage_GetWidth(pix);
    cliph = rasterimage_GetHeight(pix);

    rasterview_GetVisualBounds(self, &VB);
    InsetRect(&VB, BORDER, BORDER);
    rasterview_SetTransferMode(self, graphic_WHITE);
    graphic_MoveTo(G, x, BORDER);
    graphic_DrawLineTo(G, x, rectangle_Height(&VB));
    graphic_MoveTo(G, BORDER, y);
    graphic_DrawLineTo(G, rectangle_Width(&VB), y);
    DrawPanHighlight(self, graphic_BLACK);

    if (FullSize(self)) {
	VS = self->ViewSelection;
	OffsetRect(&VS,
		   -self->Xscroll + BORDER - rectangle_Left(&VS),
		   -self->Yscroll + BORDER - rectangle_Top(&VS)); }
    else {
	VS = self->DisplayBoxSelection;
	rectangle_SetRectSize(&VS,
			      BORDER + ((self->Xscroll < 0) ?
					-self->Xscroll : 0),
			      BORDER + ((self->Yscroll < 0) ?
					-self->Yscroll : 0),
			      rectangle_Width(&VS)*self->Scale,
			      rectangle_Height(&VS)*self->Scale); }
    rectangle_IntersectRect(&VS, &VS, &VB);
    rectangle_GetRectSize(&VS, &vsl, &vst, &vsw, &vsh);
    vsr = vsl + vsw;
    vsb = vst + vsh;
    DEBUG(("Hide Area: (%d,%d,%d,%d) Scl:(%d,%d) \n",
	    vsl, vst, vsw, vsh, self->Xscroll, self->Yscroll));

    rasterview_SetTransferMode(self, graphic_COPY);

    if (x+1 >= vsl && x-1 <= vsr) {
	/* Repair Vertical strip */
	long l = (x-1 < vsl) ? vsl : x-1;
	l = (l+3 > vsr) ? vsr-3 : l;
	DEBUG(("Ver:(%d,%d,%d,%d) Off:(%d,%d)\n",
	       l, vst, 3, vsh, self->Xoff, self->Yoff));
	ClipAndWritePixImage(clipw, cliph, G, l, vst, pix,
			     l+self->Xoff, vst+self->Yoff, 3, vsh);
    }
    if (y+1 >= vst && y-1 <= vsb) {
	/* Repair Horizontal strip */
	long t = (y-1 < vst) ? vst : y-1;
	t = (t+3 > vsb) ? vsb-3 : t;
	DEBUG(("Hor:(%d,%d,%d,%d) Off:(%d,%d)\n",
	       vsl, t, vsw, 3, self->Xoff, self->Yoff));
	ClipAndWritePixImage(clipw, cliph, G, vsl, t, pix,
			     vsl+self->Xoff, t+self->Yoff, vsw, 3);
    }
    DrawPanHighlight(self, graphic_BLACK);

    if (! self->DisplayBoxHidden && IsNotEmptyRect(&self->DisplayBox)
	 && ((x-1 <= rectangle_Left(&self->DisplayBox)-2*BORDER
	      && x+1 >= rectangle_Right(&self->DisplayBox)+2*BORDER)
	     || (x+1 >= rectangle_Left(&self->DisplayBox)-2*BORDER
		 && x-1 <= rectangle_Right(&self->DisplayBox)+2*BORDER)
	     || (y-1 <= rectangle_Top(&self->DisplayBox)
		 && y+1 >= rectangle_Bottom(&self->DisplayBox))
	     || (y+1 >= rectangle_Top(&self->DisplayBox)-2*BORDER
		 && y-1 <= rectangle_Bottom(&self->DisplayBox)+2*BORDER)))
	DisplayBoxWritePixImage(self, G);
}

void StartPanning(self, x, y)
struct rasterview *self;
long x, y;
{
    DEBUG(("Start Panning at: (%d,%d)\n", x, y));
    self->StartPanX = x;
    self->StartPanY = y;
    self->PanX = x;
    self->PanY = y;
    self->SavedDesiredSelection = self->DesiredSelection;
    self->DesiredSelection = self->ViewSelection;
    /* the following hide forces a calculation of the PanHighlight */
    HideTarget(self, x, y);
    DrawTarget(self, x, y);
}

void ContinuePanning(self, x, y)
struct rasterview *self;
long x, y;
{
    DEBUG(("Continue Panning at: (%d,%d)\n", x, y));
    if (x == self->PanX && y == self->PanY) return;

    HideTarget(self, self->PanX, self->PanY);
    DrawTarget(self, self->StartPanX, self->StartPanY);
    DrawTarget(self, x, y);
    self->PanX = x;
    self->PanY = y;
    if (self->embedded) {
	struct rectangle VB;
	rasterview_GetVisualBounds(self, &VB);
	InsetRect(&VB, BORDER, BORDER);
	DrawHighlightScreenCoordinates(self, rasterview_GetDrawable(self), VB,
				       graphic_BLACK, -1); }
}

void ClipScroll(self)
struct rasterview *self;
{
    long minLeft, maxRight;
    long minTop, maxBottom;
    struct rectangle VB;

    rasterview_GetVisualBounds(self, &VB);

    if (NotFullSize(self)) {
	self->Xscroll -= (self->Xscroll % self->Scale);
	minLeft = - (rectangle_Width(&VB) - 40);
	maxRight = (rectangle_Width(&self->ViewSelection) - 20) * self->Scale;
	self->Yscroll -= (self->Yscroll % self->Scale);
	minTop = - (rectangle_Height(&VB) - 40);
	maxBottom = (rectangle_Height(&self->ViewSelection) - 20) * self->Scale;
    }
    else {
	minLeft = - (rectangle_Width(&VB) - 18);
	maxRight = rectangle_Width(&self->ViewSelection) - 18;
	minTop = - (rectangle_Height(&VB) - 18);
	maxBottom = rectangle_Height(&self->ViewSelection) - 18;
    }

    DEBUG(("ClipScroll:\n   Visual: (%d,%d,%d,%d)\n   VS: (%d,%d,%d,%d)\n   Min: (%d,%d) Max: (%d,%d)\n",
	    rectangle_Left(&VB), rectangle_Top(&VB),
	    rectangle_Width(&VB), rectangle_Height(&VB),
	    rectangle_Left(&self->ViewSelection), rectangle_Top(&self->ViewSelection),
	    rectangle_Width(&self->ViewSelection), rectangle_Height(&self->ViewSelection),
	    minLeft, minTop, maxRight, maxBottom));

    if (self->Xscroll < minLeft) self->Xscroll = minLeft;
    if (self->Xscroll > maxRight) self->Xscroll = maxRight;
    if (self->Yscroll < minTop) self->Yscroll = minTop;
    if (self->Yscroll > maxBottom) self->Yscroll = maxBottom;
    DEBUG(("   Scroll: (%d,%d)\n", self->Xscroll, self->Yscroll));
}


void UpdateZoomedSelection(/* self, x, y */);

void FinishPanning(self, x, y)
struct rasterview *self;
long x, y;
{
    DEBUG(("Finish Panning at: (%d,%d)\n", x, y));
    DEBUG(("Started at: (%d,%d)\n", self->StartPanX, self->StartPanY));
    if (x == self->StartPanX && y == self->StartPanY)
	HideTarget(self, x, y);
    else {
	if (FullSize(self)) {
	    self->Xscroll += self->StartPanX - x;
	    self->Yscroll += self->StartPanY - y;
	    ClipScroll(self); }
	else {
	    long dx, dy;
	    dx = self->StartPanX - (self->StartPanX % self->Scale);
	    dy = self->StartPanY - (self->StartPanY % self->Scale);
	    dx -= x;
	    dy -= y;
	    dx -= (dx % self->Scale);
	    dy -= (dy % self->Scale);
	    DEBUG(("Delta: (%d,%d)\n", dx, dy));
	    self->Xscroll += dx;
	    self->Yscroll += dy;
	    ClipScroll(self);
	    if (dx !=0 || dy != 0) {
		rectangle_EmptyRect(&self->DisplayBoxSelection);
		UpdateZoomedSelection(self,
				      self->Xscroll/self->Scale,
				      self->Yscroll/self->Scale); }
	}

	self->DesiredSelection = self->SavedDesiredSelection;

	self->needsFullUpdate = TRUE;
	rasterview_WantUpdate(self, self); }
}

void SetPixel(self, x, y, bit)
struct rasterview *self;
long x, y;
boolean bit;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    struct rasterimage *pix;

    if ((ras == NULL) || ((pix = raster_GetPix(ras)) == NULL)) return;
    DEBUG(("Set Pixel in 0x%x to %s at (%d,%d)\n", pix, ((bit) ? "Black" : "White"), x, y));
    if (NotFullSize(self))
	SetPixelBehindDisplayBox(self, self->Expansion, x, y, bit);
    else {
	/* The following line does not work in X windows currently.
	graphic_SetBitAtLoc(rasterview_GetDrawable(self),
			    x - self->Xoff, y - self->Yoff, bit); */
	struct rectangle sub;
	rectangle_SetRectSize(&sub, x - self->Xoff, y - self->Yoff, 1, 1);
	rasterview_SetTransferMode(self, graphic_COPY);
	graphic_FillRect(rasterview_GetDrawable(self), &sub,
			 ((bit) ? self->BlackPattern : self->WhitePattern));
	rasterimage_SetPixel(raster_GetPix(ras), x, y, ((bit) ? 1 : 0)); }
}

/* the following algorithm is taken from "Fundamentals of Interactive Graphics" by J. D. Foley and A. Van Dam, page 435.*/
void DrawLineTo(self, x1, y1, bit)
struct rasterview  *self;
long x1, y1;
boolean bit;
/* draw the line in the unzoomed version and reflect the changes up. */
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);

    long x2 = self->TouchUpX, y2 = self->TouchUpY;

    /* start algorithm */
    long dx, dy;
    register long incr1, incr2, d, x, y;
    float slope;

    if (ras == NULL || raster_GetPix(ras) == NULL) return;

    dx = x2-x1;
    dy = y2-y1;
 
    if (dx == 0) {
       register long yend;
       if (y1 < y2) {
          y = y1; yend = y2; }
        else {
           y = y2; yend = y1; }
       for (; y<= yend; y++)
          SetPixel(self, x1, y, bit);
       return; }

    slope = (float)dy/dx;
    dx = (dx>=0) ? dx : -dx;
    dy = (dy>=0) ? dy : -dy;

    /* both x and y are either increasing or decreasing */
    if (slope > 0) {
	if (slope <= 1) {
	    register long xend;
	    /* Slope is between 0 and 1. */
	    if (x1 > x2) {
		x = x2;  y = y2;  xend = x1; }
	    else {
		x = x1;  y = y1;  xend = x2; }
	    SetPixel(self, x, y, bit);
	    d = 2*dy - dx;
	    incr1 = 2*dy;
	    incr2 = 2*(dy-dx);
	    while (x < xend) {
		x++;
		if (d < 0) d += incr1;
		else {
		    y++;
		    d += incr2; }
		SetPixel(self, x, y, bit); } }
	else {
	    register long yend;
	    /* Slope is greater than 1. */
	    if (y1 > y2) {
		x = x2;  y = y2;  yend = y1; }
	    else {
		x = x1;  y = y1;  yend = y2; }
	    SetPixel(self, x, y, bit);
	    d = 2*dx - dy;
	    incr1 = 2*dx;
	    incr2 = 2*(dx-dy);
	    while (y < yend) {
		y++;
		if (d < 0) d += incr1;
		else {
		    x++;
		    d += incr2; }
		SetPixel(self, x, y, bit); } }
    }
    else {
	if (slope >= -1) {
	    register long xend;
	    /* Slope is between 0 and -1. */
	    if (x1 < x2) {
		x = x2;  y = y2;  xend = x1; }
	    else {
		x = x1;  y = y1;  xend = x2; }
	    SetPixel(self, x, y, bit);
	    d = 2*dy - dx;
	    incr1 = 2*dy;
	    incr2 = 2*(dy-dx);
	    while (x > xend) {
		x--;
		if (d < 0) d += incr1;
		else {
		    y++;
		    d += incr2; }
		SetPixel(self, x, y, bit); } }
	else {
	    register long yend;
	    /* Slope is less than -1. */
	    if (y1 < y2) {
		x = x2;  y = y2;  yend = y1; }
	    else {
		x = x1;  y = y1;  yend = y2; }
	    SetPixel(self, x, y, bit);
	    d = 2*dx - dy;
	    incr1 = 2*dx;
	    incr2 = 2*(dx-dy);
	    while (y > yend) {
		y--;
		if (d < 0) d += incr1;
		else {
		    x++;
		    d += incr2; }
		SetPixel(self, x, y, bit); } }
    } }

/* The (x, y) argument here is the upper left bit of the current View Selection to be displayed on the screen. If the x value is negative then display (0, y) at a negative xscroll. If the y value is negative then display (x, 0) at a negative yscroll. */
void ZoomToVisualBounds(self, x, y)
struct rasterview *self;
long x, y;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    struct rasterimage *pix = raster_GetPix(ras);
    struct rectangle VB;
    struct rectangle OldDisplayBoxSelection;
    long UseableW, SmallW, ZoomW;
    long UseableH, SmallH, ZoomH;
    long diff;

    OldDisplayBoxSelection = self->DisplayBoxSelection;

    DEBUG(("Enter ZoomToVisualBounds: (%d, %d)\n", x, y));

    /* Update the scroll values given the new (x, y). */
    self->Xscroll = x*self->Scale;
    self->Yscroll = y*self->Scale;
    ClipScroll(self);
    /* Use clipped values for (x, y) */
    x = self->Xscroll/self->Scale;
    y = self->Yscroll/self->Scale;

    self->DBXscroll = (self->Xscroll < 0) ? self->Xscroll/self->Scale : 0;
    self->DBYscroll = (self->Yscroll < 0) ? self->Yscroll/self->Scale : 0;
    self->Xoff = ((self->Xscroll < 0) ? self->Xscroll : 0) - BORDER;
    self->Yoff = ((self->Yscroll < 0) ? self->Yscroll : 0) - BORDER;

    /* Useable Width is the number of pixels we can use for display of the pixelimg.  Small Width is the number of pixels in the original raster which we are going to Zoom.  The Small Width is the smaller of the Useable Width divided by the scale of expansion and the number of pixels between the left edge of the selected region and the right edge of the image.  The Height can be determined in a similar manner. Note that the Display Box is always shown in the lower left corner whether or not it overlaps the Zoomed version of the image.  If there is an overlap, blit into the pixelimage the portion of the Display Box (including the border) which overlaps so that the updating of the selected region rectangle on the screen will not mis-paint the screen with the wrong information. */

    rasterview_GetVisualBounds(self, &VB);

    UseableW = rectangle_Width(&VB) - 2*BORDER;
    UseableW = UseableW - (UseableW % self->Scale);
    SmallW = UseableW/self->Scale;
    SetWidthRect(&self->DisplayBox, SmallW);
    if (x < 0) {    
	SmallW += x;
	x = 0; }
    diff = rectangle_Right(&self->ViewSelection) - x;
    if (SmallW > diff && diff > 0) SmallW = diff;
    SetWidthRect(&self->DisplayBoxSelection, SmallW);

    UseableH = rectangle_Height(&VB) - 2*BORDER;
    UseableH = UseableH - (UseableH % self->Scale);
    SmallH = UseableH/self->Scale;
    SetHeightRect(&self->DisplayBox, SmallH);
    if (y < 0) {    
	SmallH += y;
	y = 0; }
    diff = rectangle_Bottom(&self->ViewSelection) - y;
    if (SmallH > diff && diff > 0) SmallH = diff;
    SetHeightRect(&self->DisplayBoxSelection, SmallH);

    SetLeftRect(&self->DisplayBoxSelection, x);
    SetTopRect(&self->DisplayBoxSelection, y);

    if (self->DisplayBoxHidden) {
	SetLeftRect(&self->DisplayBox, -3*rectangle_Width(&self->DisplayBox)); }
    else {
	/* Force the Display Box to be located within the Visual Bounds. */
	if (rectangle_Left(&self->DisplayBox) < rectangle_Left(&VB)
	    || rectangle_Right(&self->DisplayBox) > rectangle_Right(&VB))
	    SetLeftRect(&self->DisplayBox, 5*self->Scale);
	if (rectangle_Top(&self->DisplayBox) < rectangle_Top(&VB)
	    || rectangle_Bottom(&self->DisplayBox) > rectangle_Bottom(&VB))
	    SetTopRect(&self->DisplayBox,
		       rectangle_Bottom(&VB) - rectangle_Height(&self->DisplayBox) - 5*self->Scale); }

    DEBUG(("ZoomVB: (%d,%d,%d,%d)\n",
	    rectangle_Left(&VB),
	    rectangle_Top(&VB),
	    rectangle_Width(&VB),
	    rectangle_Height(&VB)));
    DEBUG(("ZoomDB: (%d,%d,%d,%d)\n",
	    rectangle_Left(&self->DisplayBox),
	    rectangle_Top(&self->DisplayBox),
	    rectangle_Width(&self->DisplayBox),
	    rectangle_Height(&self->DisplayBox)));
    DEBUG(("ZoomDBS: (%d,%d,%d,%d)\n",
	    rectangle_Left(&self->DisplayBoxSelection),
	    rectangle_Top(&self->DisplayBoxSelection),
	    rectangle_Width(&self->DisplayBoxSelection),
	    rectangle_Height(&self->DisplayBoxSelection)));

    if (! rectangle_IsEqualRect(&OldDisplayBoxSelection, &self->DisplayBoxSelection)) {

	ZoomW = SmallW*self->Scale;
	ZoomH = SmallH*self->Scale;

	rasterimage_Resize(self->Expansion, ZoomW, ZoomH);
	rasterimage_GetScaledSubraster(pix, &self->DisplayBoxSelection,
				       ZoomW, ZoomH, self->Expansion);

	DEBUG(("ZoomSize: %d x %d\n", ZoomW, ZoomH));

	DisplayBoxBlitOverlap(self, pix); }

    LEAVE(ZoomToVisualBounds);

}

void UpdateZoomedSelection(self, x, y)
struct rasterview *self;
long x, y;
{
    /* XXX Can be made MUCH more efficient. */
    ZoomToVisualBounds(self, x, y);
}

void PostMenus();

static RedrawRaster(self, type, left, top, width, height)
struct rasterview *self;
enum view_UpdateType type;
long left, top, width, height;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    struct rasterimage *pix;
    struct rectangle SRC;	/* image rectangle to redraw */
    struct rectangle DEST;	/* the screen rectangle to be redrawn */
    struct rectangle LB;	/* logical rectangle, inset by BORDER */
    struct rectangle VB;	/* visible rectangle inside border */

    DEBUG(("RedrawRaster(%d (%d,%d,%d,%d))\n", type, left, top, width, height));

    self->UpdateWanted = FALSE;
    if (ras == NULL) return;

    if (self->NeedsCentering) {
	self->NeedsCentering = FALSE;
	if (! self->embedded) {
	    CenterViewSelection(self);
	    PostMenus(self); } }

    /* Just in case the window has been resized. */
    ClipScroll(self);

    pix = (NotFullSize(self)) ? self->Expansion : raster_GetPix(ras);
    if (pix == NULL) {
	/* XXX Kludge: If there is no rasterimage, we create one */
	raster_SetPix(ras, pix = rasterimage_Create(91, 91));
	rectangle_SetRectSize(&self->DesiredSelection, 0, 0, 91, 91);
	self->ViewSelection = self->DesiredSelection;
	self->Shrunken = FALSE;
	/* This is to include the execution of the code which would be executed in raster_SetPix which is special cased to NOT inform the upper layers that the raster has changed.  This is a kludge to fix the problem when raster is run with a new file one should be able to quit without the message 'Are you sure you want to Quit?' Posting Menus should be sufficient. */
	PostMenus(self); }

    /* compute LB, the entire allocated rectangle of pixels, but inside by BORDER pixels on all edges. */
    rasterview_GetLogicalBounds(self, &LB);
    InsetRect(&LB, BORDER, BORDER);
    DEBUG(("LB: (%d,%d,%d,%d) Scroll: (%d,%d)\n",
	    rectangle_Left(&LB), rectangle_Top(&LB), rectangle_Width(&LB),
	    rectangle_Height(&LB), self->Xscroll, self->Yscroll));

    /* compute Xoff,Yoff, increments which map a point in the logical view rectangle to a point in the stored raster image. */

    self->Xoff = - rectangle_Left(&LB);
    self->Yoff = - rectangle_Top(&LB);
    if (FullSize(self)) {
	self->Xoff += rectangle_Left(&self->ViewSelection) + self->Xscroll;
	self->Yoff += rectangle_Top(&self->ViewSelection) + self->Yscroll; }
    else {
	if (self->Xscroll < 0)
	    self->Xoff += self->Xscroll;
	if (self->Yscroll < 0)
	    self->Yoff += self->Yscroll;
    }

    if (type == view_FullRedraw) {
	rectangle_EmptyRect(&self->CurSelection); }
    else ViewHideHighlight(self);	/* (requires Xoff and Yoff) */

    /* compute VB, the displayed portion of the image.  It is the intersection of the visual rectangle with LB */
    rasterview_GetVisualBounds(self, &VB);
    rectangle_IntersectRect(&VB, &VB, &LB);

    DEBUG(("VB: (%d,%d,%d,%d) Offset: (%d,%d)\n",
	    rectangle_Left(&VB), rectangle_Top(&VB), rectangle_Width(&VB),
	    rectangle_Height(&VB), self->Xoff, self->Yoff));

    /* compute SRC, a rectangle showing which bits of the image need to be replotted.  First we map the parameters given above into the pixel image coordinates, offsetting by the subraster amount.  Then we take the union with the PixChanged rect. */
    if (type == view_FullRedraw)
	SRC = VB;
    else {
	/* set T to pixels changed in view coords */
	struct rectangle T;
	T = self->PixChanged;
	DEBUG(("Changed: (%d,%d,%d,%d)\n",
	       rectangle_Left(&T), rectangle_Top(&T),
	       rectangle_Width(&T), rectangle_Height(&T)));
	OffsetRect(&T, -self->Xoff, -self->Yoff);
	/* set SRC to rectangle arg to Update() */
	rectangle_SetRectSize(&SRC, left, top, width, height);
	/* merge with changes noted in the data (PixChanged) */
	rectangle_UnionRect(&SRC, &SRC, &T);
	/* restrict the SRC to viewable portion */
	rectangle_IntersectRect(&SRC, &SRC, &VB);
    }
    DEBUG(("preSRC: (%d,%d,%d,%d)\n",
	    rectangle_Left(&SRC), rectangle_Top(&SRC),
	    rectangle_Width(&SRC), rectangle_Height(&SRC)));
    /* map SRC into the raster coords */
    OffsetRect(&SRC, self->Xoff, self->Yoff);
    /* and finally, restrict to the portion selected for this view */
    if (FullSize(self))
	rectangle_IntersectRect(&SRC, &SRC, &self->ViewSelection);
    else {
	struct rectangle NS;
	rectangle_SetRectSize(&NS, 0, 0,
			      rectangle_Width(&self->ViewSelection)*self->Scale,
			      rectangle_Height(&self->ViewSelection)*self->Scale);
	rectangle_IntersectRect(&SRC, &SRC, &NS); }

    DEBUG(("SRC: (%d,%d,%d,%d)\n",
	    rectangle_Left(&SRC), rectangle_Top(&SRC),
	    rectangle_Width(&SRC), rectangle_Height(&SRC)));

    /* compute DEST by mapping SRC into the logical rectangle coordinates 
      and then take the intersection with the visible rect. */
    DEST = SRC;
    OffsetRect(&DEST, -self->Xoff, -self->Yoff);
    rectangle_IntersectRect(&DEST, &DEST, &VB);

    /* SRC is the source in the image space.  DEST is the destination, in
      screen coordinates.  DEST also gives the visible width and height. */

    rectangle_GetRectSize(&DEST, &left, &top, &width, &height);

    DEBUG(("DEST: (%d,%d,%d,%d)\n", left, top, width, height));

    /* XXX need to deal with ras->options */

    rasterview_SetTransferMode(self, graphic_WHITE);
    /* There must have been some reason to special case pan mode when embedded but I (pasieka) can not remember why.  Maybe someone will point out a bug.
	if (self->embedded && (Pan(self) || type == view_FullRedraw)
	    ) {
	    rasterview_GetVisualBounds(self, &VB);
	    InsetRect(&VB, BORDER, BORDER);
	    rasterview_FillRect(self, &VB, self->WhitePattern);
	}
	else */
    if (type == view_FullRedraw) {
	rasterview_GetVisualBounds(self, &VB);
	if (self->embedded) InsetRect(&VB, BORDER, BORDER);
	rasterview_FillRect(self, &VB, self->WhitePattern);
    }
    else if (width > 0  &&  height > 0) {
	/* erase the DEST because only the black bits get painted */
	rasterview_FillRect(self, &DEST, self->WhitePattern);
    }

    /* XXX Is a TransferMode needed with WritePixImage ??? */
    rasterview_SetTransferMode(self, graphic_COPY);
    if (width > 0  &&  height > 0) {
	struct graphic *G = rasterview_GetDrawable(self);
	graphic_WritePixImage(G, left, top,
			      pix, rectangle_Left(&SRC), rectangle_Top(&SRC),
			      width, height);

	/*if Zoomed draw the display box in the right place */
	if (NotFullSize(self) && ! self->DisplayBoxHidden)
	    DisplayBoxWritePixImage(self, G);
    }

    if (RegionSelect(self))
	CorrectHighlight(self);
    if (Pan(self)) {
	DrawPanHighlight(self, graphic_BLACK); }

    if (self->embedded) {
	rasterview_GetVisualBounds(self, &VB);
	InsetRect(&VB, BORDER, BORDER);
	DrawHighlightScreenCoordinates(self, rasterview_GetDrawable(self), VB,
				       graphic_BLACK, graphic_WHITE);
    }

    rectangle_EmptyRect(&self->PixChanged);
    self->needsFullUpdate = FALSE;

    LEAVE(RedrawRaster);
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *\
 *	User Interface 
 *	
 *	Routines called from keystrokes or menu

	XXX  when first created, there is no rasterimage hanging from the struct raster.
	Some menu operations will have to cause one to exist 

\* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* masks for menu items */

#define	menuCopy	(1<<0)	/* Copy, usually, and even when read only */
#define	menuRead	(1<<1)	/* ReadRaster, usually, and also if no pix */
#define	menuOps 	(1<<2)	/* Replace, MirrorXX */
#define	menuCrop	(1<<3)	/* Crop, usual, when selection < subraster */
#define	menuUncrop	(1<<4)	/* Uncrop, only when cropped */
#define	menuRotate	(1<<5)	/* Rotate, only when whole rasterimage selected */
#define	menuNegative	(1<<6)	/* Negative */
#define	menuExtend	(1<<7)	/* ExtendToMat, when matte exceeds image */
#define	menuShrink	(1<<8)	/* ShrinkToButton, when RasterInitiallyShrunk */
#define menuMoveDB	(1<<9)	/* Move the Display Box */
#define menuHideDB	(1<<10)	/* Hide the Display Box */
#define menuTouchUp	(1<<11)	/* Touch Up */
#define menuSelect	(1<<12)	/* Selection Mode */
#define menuPan		(1<<13)
#define menuWrite	(1<<14)
#define menuZoomIn	(1<<15)
#define menuZoomOut	(1<<16)
#define menuScale	(1<<17)
#define menuCenter	(1<<18)
#define menuUpperLeft	(1<<19)
#define menuSelectAll	(1<<20)

/* masks for menus */

#define	menuNoPix	(menuRead | menuWrite)
#define	menuReadOnly	menuCopy
#define	menuReadWrite	(menuCopy | menuOps | menuNegative | menuRead | menuWrite)

static struct menulist  *EmbeddedMenus, *ApplicationMenus;
static struct keymap  *EmbeddedKeymap, *ApplicationKeymap;

static void ToggleDebug()
{
    debug = ! debug;
    printf("Debuging is now %s\n", (debug) ? "On" : "Off");  fflush (stdout);
}

static void CropCommand(self, rock)
struct rasterview *self;
long rock;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    if (ras == NULL) return;

    /* Until Crop works when Zoomed. */
    if (NotFullSize(self)) return;

    self->ViewSelection = self->DesiredSelection;
    ras->subraster = self->ViewSelection;

    if (self->embedded) {
	self->Xscroll = self->Yscroll = 0;
	ras->subraster = self->ViewSelection;
	raster_NotifyObservers(ras, raster_BOUNDSCHANGED);
    }
    else {
	CenterViewSelection(self);
	self->needsFullUpdate = TRUE;
	rasterview_WantUpdate(self, self);
    }
}

static void UncropCommand(self, rock)
struct rasterview *self;
long rock;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    if (ras == NULL) return;

    if (! self->embedded) {
	self->Xscroll += rectangle_Left(&self->ViewSelection);
	self->Yscroll += rectangle_Top(&self->ViewSelection); }

    rectangle_SetRectSize(&self->ViewSelection, 0, 0,
			   raster_GetWidth(ras), raster_GetHeight(ras));
    ras->subraster = self->ViewSelection;

    if (self->embedded) {
	self->Xscroll = self->Yscroll = 0;
	ras->subraster = self->ViewSelection;
	raster_NotifyObservers(ras, raster_BOUNDSCHANGED);
    }
    else {
	self->needsFullUpdate = TRUE;
	rasterview_WantUpdate(self, self);
    }
}

static void ShrinkCommand(self, rock)
struct rasterview *self;
long rock;
{
    self->Shrunken = TRUE;
    self->needsFullUpdate = TRUE;
    rasterview_WantNewSize(self, self);
    /* the new size will force FullUpdate, so we don't call WantUpdate */
    /* XXX WantNewSize does nothing if is an ApplicationLayer */
    if ( ! self->embedded)
	rasterview_WantUpdate(self, self);
}

void CenterCommand(self, rock)
struct rasterview *self;
long rock;
{
    struct rectangle VB;
    rasterview_GetVisualBounds(self, &VB);

    if (RegionSelect(self)) {
	self->Xscroll = rectangle_Left(&self->DesiredSelection) - rectangle_Left(&self->ViewSelection) -
	  (rectangle_Width(&VB)/self->Scale - rectangle_Width(&self->DesiredSelection) - 1)/2;
	self->Yscroll = rectangle_Top(&self->DesiredSelection) - rectangle_Top(&self->ViewSelection) -
	  (rectangle_Height(&VB)/self->Scale - rectangle_Height(&self->DesiredSelection) - 1)/2;
	if (NotFullSize(self))
	    ZoomToVisualBounds(self, self->Xscroll, self->Yscroll); }
	else {
	    CenterViewSelection(self);
	    if (NotFullSize(self))
		ZoomToVisualBounds(self,
				   self->Xscroll/self->Scale,
				   self->Yscroll/self->Scale); }

    self->needsFullUpdate = TRUE;
    rasterview_WantUpdate(self, self);
}

void UpperLeftCommand(self, rock)
struct rasterview *self;
long rock;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);

    if (! RegionSelect(self) || ras == NULL) return;

    if (FullSize(self)) {
	self->Xscroll = rectangle_Left(&self->DesiredSelection);
	self->Yscroll = rectangle_Top(&self->DesiredSelection); }
    else
	UpdateZoomedSelection(self,
			      rectangle_Left(&self->DesiredSelection),
			      rectangle_Top(&self->DesiredSelection));

    self->needsFullUpdate = TRUE;
    rasterview_WantUpdate(self, self);
}

void SelectAllCommand(self, rock)
struct rasterview *self;
long rock;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);

    if (! RegionSelect(self) || ras == NULL) return;

    self->DesiredSelection = self->ViewSelection;
    CorrectHighlight(self);
    self->needsFullUpdate=FALSE;
    rasterview_WantUpdate(self, self);
}


void ZoomInCommand(self, rock)
struct rasterview *self;
long rock;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    long x, y;
    struct rectangle VB;

    if (ras == NULL || raster_GetPix(ras) == NULL) return;

    /* Until zoom works with cropping... */
    if (Cropped(self)) return;

    rasterview_GetVisualBounds(self, &VB);

    if (rectangle_Width(&VB)+2*BORDER < self->Scale*2
	 || rectangle_Height(&VB)+2*BORDER < self->Scale*2) {
	message_DisplayString(self, 0, "Zoom too large to fit current screen");
	return; }

    if (FullSize(self)) {
	/* Create the pixelimage which will hold the expanded version of the dataobj. */
	self->Expansion = rasterimage_Create(0,0); }

    self->Scale *= 2;

    DEBUG(("Scroll: (%d,%d)\n", self->Xscroll, self->Yscroll));
    DEBUG(("Zoom In Scale: %d VS: (%d,%d,%d,%d)\n", self->Scale,
	    rectangle_Left(&self->ViewSelection),
	    rectangle_Top(&self->ViewSelection),
	    rectangle_Width(&self->ViewSelection),
	    rectangle_Height(&self->ViewSelection)));

    self->DisplayBoxHidden = (self->Scale < 4);

    /* Force ZoomToVisualBounds to place the Display Box in the default location and to determine the zoomed selection. */
    SetLeftRect(&self->DisplayBox, -1);
    SetTopRect(&self->DisplayBox, -1);
    rectangle_EmptyRect(&self->DisplayBoxSelection);

    /* The scrolling of the zoomed in raster can be determined by considering the vector between the center of the screen and the upper left corner of the current View Selection. That vector is scaled (in this case) by a factor of two. The x component of the vector can be calculated as the sum of the current Xscroll and one half the visual width. The new Xscroll can then be calculated as two times the x component of the vector minus one half the visual width. The new Yscroll can be calculated in a similar fashion. The (x, y) to pass to ZoomToVisualBounds is then the scroll divided by the current scale. */
    x = (2*self->Xscroll + rectangle_Width(&VB)/2)/self->Scale;
    y = (2*self->Yscroll + rectangle_Height(&VB)/2)/self->Scale;

    ZoomToVisualBounds(self, x, y);

    PostMenus(self);
    self->needsFullUpdate = TRUE;
    rasterview_WantUpdate(self, self);
}

void ReflectChangesInExpansion(self, R)
struct rasterview *self;
struct rectangle *R;
/* R is the rectangle within the full image which has been changed */
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    struct rasterimage *pix, *new;
    struct rectangle change;
    long x, y, w, h;

    if ((ras == NULL) || ((pix = raster_GetPix(ras)) == NULL))  return;

    rectangle_IntersectRect(&change, R, &self->DisplayBoxSelection);
    if (IsEmptyRect(&change)) return;

    rectangle_GetRectSize(&change, &x, &y, &w, &h);
    w *= self->Scale;
    h *= self->Scale;

    new = rasterimage_Create(w, h);

    rasterimage_GetScaledSubraster(pix, &change, w, h, new);

    x = rectangle_Left(&change) - rectangle_Left(&self->DisplayBoxSelection);
    y = rectangle_Top(&change) - rectangle_Top(&self->DisplayBoxSelection);
    rectangle_SetRectSize(&change, 0, 0, w, h);

    rasterimage_BlitSubraster(self->Expansion, x*self->Scale, y*self->Scale,
			       new, &change, 0);
    rasterimage_Destroy(new);

    /* If the Display Box is Empty then do not blit anything into the image. */
    if (IsNotEmptyRect(&self->DisplayBox))
	DisplayBoxBlitOverlap(self, pix);
}

void HideDisplayBox(self)
struct rasterview *self;
{
    if (RegionSelect(self))
	ViewHideHighlight(self);
    DisplayBoxHide(self);
}

void MoveDisplayBoxCommand(self, rock)
struct rasterview *self;
long rock;
{
    if (rock == 1 && FullSize(self)) return;

    if (rock >= 0)
	message_DisplayString(self, 0,
			      "Click Left to selection new position of Display Box");

    DEBUG(("Moving Display Box\n"));
    HideDisplayBox(self);
    self->MovingDisplayBox = TRUE;
}

void FinishMovingDisplayBox(self, x, y)
struct rasterview *self;
long x, y;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    struct rasterimage *pix;
    struct graphic *G = rasterview_GetDrawable(self);
    struct rectangle VB;

    ENTER(FinishMovingDisplayBox);

    message_DisplayString(self, 0, "");

    if ((ras == NULL) || ((pix = raster_GetPix(ras)) == NULL))  return;
    if (! self->MovingDisplayBox) return;

    rasterview_GetVisualBounds(self, &VB);

    SetLeftRect(&self->DisplayBox, x);
    SetTopRect(&self->DisplayBox, y);
    /* Keep the Display Box within the Visual Rectangle. */
    if (rectangle_Left(&self->DisplayBox) - 4*BORDER <= 0) {
	SetLeftRect(&self->DisplayBox, 4*BORDER); }
    if (rectangle_Top(&self->DisplayBox) - 4*BORDER <= 0) {
	SetTopRect(&self->DisplayBox, 4*BORDER); }
    if (rectangle_Right(&self->DisplayBox) + 3*BORDER > rectangle_Width(&VB)) {
	long left = rectangle_Right(&VB) - 2*BORDER;
	left -= (left % self->Scale);
	SetLeftRect(&self->DisplayBox,
		    left - rectangle_Width(&self->DisplayBox) - 3*BORDER); }
    if (rectangle_Bottom(&self->DisplayBox) + 3*BORDER > rectangle_Height(&VB)) {
	long top = rectangle_Bottom(&VB) - 2*BORDER;
	top -= (top % self->Scale);
	SetTopRect(&self->DisplayBox,
		   top - rectangle_Height(&self->DisplayBox) - 3*BORDER); }

    DEBUG(("Moving Display Box to: (%d,%d,%d,%d)\n",
	    rectangle_Left(&self->DisplayBox),
	    rectangle_Top(&self->DisplayBox),
	    rectangle_Width(&self->DisplayBox),
	    rectangle_Height(&self->DisplayBox)));

    self->DisplayBoxHidden = FALSE;
    self->MovingDisplayBox = FALSE;

    DisplayBoxBlitOverlap(self, pix);
    DisplayBoxWritePixImageFull(self, G, pix);

    if (RegionSelect(self))
	CorrectHighlight(self);
    if (Pan(self)) {
	DrawPanHighlight(self, graphic_BLACK); }

    PostMenus(self);
}

void HideDisplayBoxCommand(self, rock)
struct rasterview *self;
long rock;
{
    HideDisplayBox(self);
    SetLeftRect(&self->DisplayBox, -3*rectangle_Width(&self->DisplayBox));
    if (RegionSelect(self))
	CorrectHighlight(self);
    self->DisplayBoxHidden = TRUE;
    PostMenus(self);
}

void ZoomOutCommand(self, rock)
struct rasterview *self;
long rock;
{
    struct rectangle VB;
    long x, y;
    rasterview_GetVisualBounds(self, &VB);

    if (FullSize(self)) return;

    self->Scale /= 2;
    DEBUG(("Zoom Out Scale: %d\n", self->Scale));

    /* The scrolling of the zoomed out raster can be determined by considering the vector between the center of the screen and the upper left corner of the current View Selection. That vector is scaled (in this case) by a factor of one half. The x component of the vector can be calculated as the sum of the current Xscroll and one half the visual width. The new Xscroll can then be calculated as one half the x component of the vector minus one half the visual width. The new Yscroll can be calculated in a similar fashion. The (x, y) is then these values divided by the current Scale. */
    rasterview_GetVisualBounds(self, &VB);
    x = (self->Xscroll/2 - rectangle_Width(&VB)/4)/self->Scale;
    y = (self->Yscroll/2 - rectangle_Height(&VB)/4)/self->Scale;

    if (FullSize(self)) {
	rasterimage_Destroy(self->Expansion);
	self->Expansion = NULL;
	self->DisplayBoxOverlapped = FALSE;
	rectangle_EmptyRect(&self->DisplayBoxSelection);
	rectangle_EmptyRect(&self->DisplayBox);
	self->DisplayBoxHidden = TRUE;
	self->Xscroll = x;
	self->Yscroll = y;
    }
    else {
	self->DisplayBoxHidden = (self->Scale < 4);
	rectangle_EmptyRect(&self->DisplayBoxSelection);
	ZoomToVisualBounds(self, x, y);
    }

    self->needsFullUpdate = TRUE;
    rasterview_WantUpdate(self, self);
}

void NormalSizeCommand(self, rock)
struct rasterview *self;
long rock;
{
    if (NotFullSize(self)) {
	float factor = ((1.0/self->Scale - 1.0)/2.0);
	struct rectangle VB;
	rasterview_GetVisualBounds(self, &VB);

	self->Xscroll = self->Xscroll/self->Scale + rectangle_Width(&VB)*factor;
	self->Yscroll = self->Yscroll/self->Scale + rectangle_Height(&VB)*factor;
	rasterimage_Destroy(self->Expansion);
	self->Expansion = NULL;
	self->DisplayBoxOverlapped = FALSE;
	rectangle_EmptyRect(&self->DisplayBoxSelection);
	rectangle_EmptyRect(&self->DisplayBox);
	self->DisplayBoxHidden = TRUE;

	self->Scale = 1;

	PostMenus(self);
	self->needsFullUpdate = TRUE;
	rasterview_WantUpdate(self, self);
    }
}

void ExitTouchUp(self)
struct rasterview *self;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    struct rasterimage *pix;
    if (ras == NULL || (pix = raster_GetPix(ras)) == NULL) return;
    /* Notify Observers here instead of each pixel changed level. */
    rasterimage_NotifyObservers(pix, raster_BITSCHANGED);
}

void RegionSelectCommand(self, rock)
struct rasterview *self;
long rock;
{
    struct rectangle r;
    rasterview_GetVisualBounds(self, &r);

    message_DisplayString(self, 0, "Region Select Mode");
    rasterview_RetractCursor(self, self->Cursor[self->Mode]);

    if (TouchUp(self))
	ExitTouchUp(self);
    else if (Pan(self))
	DrawPanHighlight(self, graphic_WHITE);

    self->Mode = RegionSelectMode;
    CorrectHighlight(self);

    rasterview_PostCursor(self, &r, self->Cursor[self->Mode]);
    PostMenus(self);
}

void TouchUpCommand(self, rock)
struct rasterview *self;
long rock;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    struct rectangle r;
    rasterview_GetVisualBounds(self, &r);

    if (ras == NULL || raster_GetPix(ras) == NULL) return;

    message_DisplayString(self, 0, "Touch Up Mode");
    rasterview_RetractCursor(self, self->Cursor[self->Mode]);

    if (RegionSelect(self))
	ViewHideHighlight(self);
    else if (Pan(self))
	DrawPanHighlight(self, graphic_WHITE);

    self->Mode = TouchUpMode;

    rasterview_PostCursor(self, &r, self->Cursor[self->Mode]);
    PostMenus(self);
}

void PanCommand(self, rock)
struct rasterview *self;
long rock;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    struct rectangle r;
    rasterview_GetVisualBounds(self, &r);
    if (ras == NULL || raster_GetPix(ras) == NULL) return;

    message_DisplayString(self, 0, "Pan Mode");
    rasterview_RetractCursor(self, self->Cursor[self->Mode]);

    if (RegionSelect(self))
	ViewHideHighlight(self);
    if (TouchUp(self))
	ExitTouchUp(self);

    self->Mode = PanMode;
    DrawPanHighlight(self, graphic_BLACK);

    rasterview_PostCursor(self, &r, self->Cursor[self->Mode]);
    PostMenus(self);
}

void ModifyCommand(self, rock)
struct rasterview *self;
long rock;
{
    /* -1 = negative, 0 = white, 1 = black, 2 = Gray */
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    struct rasterimage *pix;
    struct rectangle R;

    R = self->DesiredSelection;

    if (ras == NULL || (pix = raster_GetPix(ras)) == NULL) return;

    if (rock == 0)
	rasterimage_PaintSubraster(pix, &R, WHITEBYTE);
    else if (rock == 1)
	rasterimage_PaintSubraster(pix, &R, BLACKBYTE);
    else if (rock == -1)
	rasterimage_InvertSubraster(pix, &R);
    else if (rock == 2) {
	char inbuf[50];
	long level;
	AskOrCancel(self, "Gray level (1..15)[8]: ", inbuf);
	/* If there is a value returned then if the value cannot be parsed then error. */
	if (*inbuf) {
	    if (sscanf(inbuf, "%d", &level) != 1)	
		DisplayAndReturn(self, "Value must be digits with no decimal point."); }
	else level = 8;
	rasterimage_GraySubraster(pix, &R, level); }
    else if (rock == 3)
	rasterimage_MirrorLRSubraster(pix, &R);
    else if (rock == 4)
	rasterimage_MirrorUDSubraster(pix, &R);
 
    if (NotFullSize(self))
	ReflectChangesInExpansion(self, &R);

    rasterimage_NotifyObservers(pix, raster_BITSCHANGED);
}

/* RotateCommand(self, rock)
	Rotates the entire raster image of self.

	Strategy is to clone the rasterimage, set the bit array pointer to null in 
	the original and resize it.  Then the Rotate operation is done by copying
	the bits from the clone (which has the original bit array) to the new
	bit array (which is attached to the original rasterimage).
	Finally, the clone and the old bit array are discarded.
*/
static void RotateCommand(self, rock)
struct rasterview *self;
long rock;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    struct rasterimage *pix;
    struct rasterimage *temp;
    long w, h;
    struct rectangle R;

    if ((ras == NULL) || ((pix = raster_GetPix(ras)) == NULL)) return;

    /* Until Rotation works while Zoomed... */
    if (NotFullSize(self)) return;

    w = rasterimage_GetWidth(pix);
    h = rasterimage_GetHeight(pix);

    temp = rasterimage_Clone(pix);
    rasterimage_SetBitsPtr(pix, NULL);
    rasterimage_Resize(pix, h, w);

    rectangle_SetRectSize(&R, 0, 0, w, h);
    pixelimage_GetRotatedSubraster((struct pixelimage *)temp, &R,
				    (struct pixelimage *)pix);
    rasterimage_Destroy(temp);
    rectangle_SetRectSize(&self->DesiredSelection, 0, 0, h, w);
    rectangle_SetRectSize(&self->ViewSelection, 0, 0, h, w);
    CenterViewSelection(self);
    rasterimage_NotifyObservers(pix, raster_BITSCHANGED);
}

void ReadRaster(self, ras, filename)
struct rasterview *self;
struct raster *ras;
char *filename;
{
    /* need to use FindFile XXX */
    long readresult = dataobject_OBJECTCREATIONFAILED;
    FILE *f;
    register c;

    f = fopen(filename, "r");
    if (f == NULL) {
	char err[MAXPATHLEN + 50];
	sprintf(err, "Could not read file %s", filename);
	message_DisplayString((struct view *)self, 50, err);
	return; }

    /* The raster_Read routine must assume the header is gone therefore we have to read it. For compatability, we do not read beyond an initial 0xF1, which raster_Read knows about */

    ungetc((c=getc(f)), f);
    if (c == 0xF1) 
	readresult = raster_Read(ras, f, 0);
    else {
	/* XXX If the file does not begin with a raster, we may as well scan until we find one */
	static char hdr[] = "\\begindata{raster,";
	char *hx = hdr;

	while ((c = getc(f)) != EOF && *hx)
	    if (c == *hx) hx++;
	    else hx = (c == *hdr) ? hdr+1 : hdr;
	if (*hx) {
	    char err[MAXPATHLEN + 50];
	    sprintf(err, "No raster found in file %s", filename);
	    message_DisplayString((struct view *)self, 50, err);
	    fclose(f);
	    return; }
	while ((c=getc(f)) != '\n' && c != EOF) 
	{}	/* skip to end of header */
	readresult = raster_Read(ras, f, 0); }
    fclose(f);
    if (! self->embedded)
	buffer_SetFilename(frame_GetBuffer(frame_Enumerate(FindFrameHelp, rasterview_GetIM(self))), filename);

    /* XXX need to inform observers of scroll change. */

    if (readresult == dataobject_NOREADERROR) {
	self->needsFullUpdate = TRUE;
	self->Xscroll = self->Yscroll = 0;
	/* select the entire raster */
	self->ViewSelection = self->DesiredSelection = ras->subraster;
	if (! self->embedded)
	    CenterViewSelection(self);
    }
    else {
	char err[MAXPATHLEN + 50];
	sprintf(err, "Error %d while reading file %s", readresult, filename);
	message_DisplayString((struct view *)self, 50, err); }
}

static void ReadFileCommand(self, rock)
struct rasterview *self;
long rock;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    char filename[MAXPATHLEN];
    enum message_CompletionCode result;

    ENTER(ReadFileCommand);
    if (ras == NULL) return;

    CurrentDirectory(self, filename);

    result = (enum message_CompletionCode)
      completion_GetFilename(self, 
			      "Read Raster File: ", 	/* prompt */
			      filename,			/* initial string */
			      filename,			/* working area */
			      MAXPATHLEN,		/* size of working area */
			      FALSE,			/* want a file, not a directory */
			      TRUE);			/* must be existing file */

    if (result != message_Complete && result != message_CompleteValid) {
	message_DisplayString((struct view *)self, 0, "Cancelled.");
	return; }

    ReadRaster(self, ras, filename);

    /* the Read will cause a NotifyObservers, which will call WantUpdate */
    LEAVE(ReadFileCommand);
}

void WriteFileCommand(self)
struct rasterview *self ;
{
    /* write using filename from read as default. */
    message_DisplayString((struct view *)self, 0, "Write Raster not yet implemented");
}

enum RasterIOType {
	InMacPaint, 
	OutMacPaint, 
	OutRasterFile, 
	OutPostscript, };

static char *prompts[] = {
	"Read MacPaint file: ",
	"Write MacPaint file: ",
	"Write RasterFile file: ",
	"Write Postscript file: ", };

static void RasterIOCommand(self, rock)
struct rasterview *self;
enum RasterIOType rock;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    FILE *f;
    boolean inputop;
    char filename[MAXPATHLEN];
    enum message_CompletionCode result;

    ENTER(RasterIOCommand);
    if (ras == NULL) return;

    inputop =  (rock == InMacPaint);  /* XXX adjust if add more input types */

    CurrentDirectory(self, filename);

    result = (enum message_CompletionCode)
      completion_GetFilename(self, 
			      prompts[(int)rock], 	/* prompt */
			      filename,	 		/* initial string */
			      filename,			/* working area */
			      MAXPATHLEN,		/* size of working area */
			      FALSE,			/* want a file, not a directory */
			      inputop);			/* must be existing file */

    if (result != message_Complete && result != message_CompleteValid) {
	message_DisplayString((struct view *)self, 0, 
			      "Cancelled.");
	return; }

    /* need to use FindFile XXX */
    /* need to check if output file exists XXX */

    f = fopen(filename, (inputop) ? "r" : "w");
    if (f == NULL) {
	char err[MAXPATHLEN + 50];
	sprintf(err, "Could not %s file %s", (inputop) ? "read" : "write", filename);
	message_DisplayString((struct view *)self, 0, err);
	return; }

    switch (rock) {
	case InMacPaint:
	    if (paint_ReadImage(f, raster_GetPix(ras)) 
		!= dataobject_NOREADERROR) {
		char msg[MAXPATHLEN + 50];
		sprintf(msg, "File %s apparently not in MacPaint format", filename);
		message_DisplayString((struct view *)self, 0, msg); }
	    else {
		self->Xscroll = self->Yscroll = 0;
		self->needsFullUpdate = TRUE;
		self->ViewSelection = self->DesiredSelection = ras->subraster; 
		/* the Read will cause a NotifyObservers,
		 which will call WantUpdate */
	    }
	    break;
	case OutMacPaint:
	    paint_WriteImage(f, raster_GetPix(ras), &self->ViewSelection);
	    break;
	case OutRasterFile:
	    oldRF_WriteImage(f, raster_GetPix(ras), &self->ViewSelection);
	    break;
	case OutPostscript:
	    heximage_WritePostscript(f, raster_GetPix(ras), &self->ViewSelection, 
				     (ras->xScale + 0e0) / raster_UNITSCALE, 
				     (ras->yScale + 0e0) / raster_UNITSCALE);
	    break;
    }
    fclose(f);
    if (! self->embedded && inputop)
	buffer_SetFilename(frame_GetBuffer(frame_Enumerate(FindFrameHelp, rasterview_GetIM(self))), filename);

    if ( ! inputop) {
	char msg[MAXPATHLEN + 50];
	sprintf(msg, "Wrote file %s", filename);
	message_DisplayString((struct view *)self, 0, msg); }

    LEAVE(RasterIOCommand);
}

static void CopyCommand(self) 
struct rasterview *self;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    FILE *copyFile;

    if (ras == NULL) return;
    copyFile = im_ToCutBuffer(rasterview_GetIM(self));
    raster_WriteSubRaster(ras, copyFile, 91, &self->DesiredSelection);
    /* the 91 is any non-zero object id */
    im_CloseToCutBuffer(rasterview_GetIM(self), copyFile);
}

static void ReplaceCommand (self)
struct rasterview *self;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    FILE *pasteFile;
    static char hdr[] = "\\begindata{raster,";
    char *hx = hdr;
    register c;

    pasteFile = im_FromCutBuffer(rasterview_GetIM(self));

    /* if the file does not begin with a raster, 
	we may as well scan until we find one XXX */

    while ((c = getc(pasteFile)) != EOF && *hx)
	if (c == *hx) hx++;
	else hx = (c == *hdr) ? hdr+1 : hdr;
    if (*hx) 
	message_DisplayString((struct view *)self, 0, "No raster found in cut buffer");
    else {
	while ((c=getc(pasteFile)) != '\n' && c != EOF) 
	{}	/* skip to end of header */
	if (FullySelected(self) && Uncropped(self)) {
	    raster_Read(ras, pasteFile, 1);
	    self->ViewSelection = self->DesiredSelection = ras->subraster;
	    CenterViewSelection(self); }
	else
	    raster_ReadSubRaster(ras, pasteFile, &self->DesiredSelection);
    }
    im_CloseFromCutBuffer(rasterview_GetIM(self), pasteFile);
}

boolean MatExtendPossible(self)
struct rasterview *self;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    struct rectangle VB;

    rasterview_GetVisualBounds(self, &VB);
    InsetRect(&VB, BORDER, BORDER);
    DEBUG(("Ext-P: Scroll: (%d,%d) Raster: (%d,%d)\n",
	    self->Xscroll, self->Yscroll,
	    raster_GetWidth(ras), raster_GetHeight(ras)));
    DEBUG(("       VS: (%d,%d,%d,%d) VB: (%d,%d,%d,%d)\n",
	    rectangle_Left(&self->ViewSelection),
	    rectangle_Top(&self->ViewSelection),
	    rectangle_Width(&self->ViewSelection),
	    rectangle_Height(&self->ViewSelection),
	    rectangle_Left(&VB),
	    rectangle_Top(&VB),
	    rectangle_Width(&VB),
	    rectangle_Height(&VB)));
    return ((self->Xscroll + rectangle_Width(&VB))/self->Scale > raster_GetWidth(ras)
	     || (self->Yscroll + rectangle_Height(&VB))/self->Scale > raster_GetHeight(ras)
	     || self->Xscroll < 0
	     || self->Yscroll < 0);
}

/* ExtendToMatCommand(self)
	Extend the raster size to the size of the matte, if bigger 
*/
static void ExtendToMatCommand(self)
struct rasterview *self;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    long l, t, w, h, r, b;
    long vr, vb;
    long width, height;
    struct rectangle VB;	/* the visual bounds measure the matte size */

    if (! MatExtendPossible(self)) return;

    rasterview_GetVisualBounds(self, &VB);
    InsetRect(&VB, BORDER, BORDER);
    vr = rectangle_Width(&VB);
    vb = rectangle_Height(&VB);

    l = -self->Xscroll;
    t = -self->Yscroll;
    w = raster_GetWidth(ras)*self->Scale;
    h = raster_GetHeight(ras)*self->Scale;
    r = l + w;
    b = t + h;

    width = (w + ((vr > r) ? (vr - r) : 0) + ((l > 0) ? l : 0))/self->Scale;
    height = (h + ((vb > b) ? (vb - b) : 0) + ((t > 0) ? t : 0))/self->Scale;

    DEBUG(("Visual: %d x %d\nCurrent Sides: (%d,%d,%d,%d)\nNew Size: %d x %d\n",
	    vr, vb,
	    l/self->Scale, t/self->Scale, r/self->Scale, b/self->Scale,
	    width, height));

    raster_Resize(ras, width, height);

    self->Xscroll = (l > 0) ? 0 : self->Xscroll;
    self->Yscroll = (t > 0) ? 0 : self->Yscroll;

    if (l > 0 || t > 0) {
	struct rasterimage *pix = raster_GetPix(ras);
	struct rasterimage *new = rasterimage_Create(w/self->Scale, h/self->Scale);
	rasterimage_BlitSubraster(new, 0, 0, pix, &self->ViewSelection, 0);
	rasterimage_Clear(pix);
	rasterimage_BlitSubraster(pix,
				  ((l > 0) ? l/self->Scale : 0),
				  ((t > 0) ? t/self->Scale : 0),
				  new, &self->ViewSelection, 0);
	rasterimage_Destroy(new); }

    rectangle_SetRectSize(&self->ViewSelection, 0, 0, width, height);
    rectangle_SetRectSize(&self->DesiredSelection, 0, 0, width, height);

    if (NotFullSize(self)) {
	ZoomToVisualBounds(self,
			   self->Xscroll/self->Scale, self->Yscroll/self->Scale); }

    rasterimage_NotifyObservers(raster_GetPix(ras), raster_BITSCHANGED);
    rasterimage_NotifyObservers(raster_GetPix(ras), raster_BOUNDSCHANGED);
}

/* SetPrintSizeCommand(self)
	set scaling factors used for printing
	prompts for print size and sets scaling accordingly
	Note that the subraster is either the entire raster or the cropped raster.
*/
static void SetPrintSizeCommand(self)
struct rasterview *self;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    long w = rectangle_Width(&self->ViewSelection);
    long h = rectangle_Height(&self->ViewSelection);

    double newdw = 0.0, newdh = 0.0;
    long newyScale = 0, newxScale = 0;
    char newxsize[75], newysize[75], request[150];

    /* ask for height */
    sprintf(request, "Print height %0.2f  width %0.2f in.   New height [scale to width] ", 
	     h / 72.0 * ras->yScale / raster_UNITSCALE,
	     w / 72.0 * ras->xScale / raster_UNITSCALE);
    AskOrCancel(self, request, newysize);
    if (*newysize) {
	/* height specified.  parse it and set width request */
	if (sscanf(newysize, "%lf", &newdh) != 1)		
	    DisplayAndReturn(self,
			     "Value must be digits with at most one decimal point.");
	newyScale = (newdh * 72.0 * raster_UNITSCALE + (h/2)) / h;
	/* Change x scaling factor to correspond to the new y scaling factor */
	newxScale = newyScale;

	sprintf(request, "Print height %0.2f in.   New width [%0.2f] ", 
		h / 72.0 * newyScale / raster_UNITSCALE,
		w / 72.0 * newxScale / raster_UNITSCALE); }
    else {
	/* no height specified; scale to width, default original width */
	sprintf(request, "Print height %0.2f  width %0.2f in.   Scale to width [original:  %0.2f] ", 
		h / 72.0 * ras->yScale / raster_UNITSCALE,
		w / 72.0 * ras->xScale / raster_UNITSCALE,
		w / 72.0 / 2.0); }

    /* request new width */
    AskOrCancel(self, request, newxsize);
    if (*newxsize && sscanf(newxsize, "%lf", &newdw) != 1)
	DisplayAndReturn(self, "Value must be digits with at most one decimal point.");
    /* now analyze results.   set unspecified new..Scale  */
    if (*newxsize) {
	newxScale = (newdw * 72.0 * raster_UNITSCALE + (w/2)) / w;
	if (*newysize == '\0') 
	    newyScale = newxScale; }
    else if (*newysize == '\0') {
	/* no change specified, revert to 1/2 screen size */
	newxScale = raster_UNITSCALE / 2;
	newyScale = raster_UNITSCALE / 2; }

    /* Store new scaling factors. */
    ras->xScale = newxScale;
    ras->yScale = newyScale;

    DEBUG(("chosen size inches: %0.4f x %0.4f   points:  %ld x %ld   (scaled 0x%lx  0x%lx)  \n", 
	    newdw, newdh,
	    (w * ras->xScale + (raster_UNITSCALE/2)) / raster_UNITSCALE,
	    (h * ras->yScale + (raster_UNITSCALE/2)) / raster_UNITSCALE, 
	    newxScale, newyScale));

    /* display the new size */
    sprintf(request, "Print size is now height %0.2f width %0.2f in. ", 
	     h / 72.0 * ras->yScale / raster_UNITSCALE,
	     w / 72.0 * ras->xScale / raster_UNITSCALE);
    message_DisplayString(self, 0, request);
}

/* This keeps the orignal pixel image around so that
  one can extrapolate from the original rather then the
  scaled version of the image. */
static void ScaleCommand(self)
struct rasterview *self;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    struct rasterimage *pix;
    long x, y, w, h;
    long NewW, NewH;
    char c[10], inbuf[50], request[150];

    if (ras == NULL || (pix = raster_GetPix(ras)) == NULL) return;

    /* Until Scaling works while Zoomed... */
    if (NotFullSize(self)) return;

    rectangle_GetRectSize(&self->DesiredSelection, &x, &y, &w, &h);

    /* ask for scaling factor as a number of pixels or relative
	to current size */
    sprintf(request, "Change scale via relative size or absolute pixel [relative]: ",
	     inbuf);
    AskOrCancel(self, request, inbuf);
    sscanf(inbuf, "%[ar]", &c[0]);
    if (strcmp(c,"a")==0) {
	/* Ask for absolute values */
	sprintf(request, "New pixel width [%d]: ", w);
	AskOrCancel(self, request, inbuf);
	/* If there is a value returned then if the value cannot be parsed then error else calculate new height and default the new height correspondingly. Else default the new height and width to 1. */
	if (*inbuf) {
	    if (sscanf(inbuf, "%d", &NewW) != 1) {		
		DisplayAndReturn(self, "Value must be digits with no decimal point."); }
	    else NewH = (h * NewW)/w; }
	else {
	    NewW = w;
	    NewH = h; }
	sprintf(request, "New pixel height [%d]: ", NewH);

	AskOrCancel(self, request, inbuf);
	/* if there is a value returned and it cannot be parsed then error. */
	if (*inbuf && sscanf(inbuf, "%d", &NewH) != 1)		
	    DisplayAndReturn(self, "Value must be digits with no decimal point.");
	if (! *inbuf) NewH = (h * NewW)/w;
    }
    else {
	float ScaleW, ScaleH;
	/* Ask for relative values. */
	AskOrCancel(self, "New relative width [1.0000]: ", inbuf);
	if (*inbuf) {
	    if (sscanf(inbuf, "%f", &ScaleW) != 1) {
		DisplayAndReturn(self,
				 "Value must be digits with at most one decimal point."); }
	    else ScaleH = ScaleW; }
	else {
	    ScaleW = 1.0;
	    ScaleH = 1.0; }
	sprintf(request, "New relative height [%0.4f]: ", ScaleH);

	AskOrCancel(self, request, inbuf);
	if (*inbuf && sscanf(inbuf, "%f", &ScaleH) != 1) {
	    DisplayAndReturn(self,
			     "Value must be digits with at most one decimal point."); }
	if (! *inbuf) ScaleH = ScaleW;
	NewW = w * ScaleW;
	NewH = h * ScaleH;
    }

    sprintf(request, "New pixel (width, height): (%d, %d)", NewW, NewH);
    message_DisplayString(self, 0, request);

    DEBUG(("Original is%s NULL\n", ((self->Original == NULL) ? "" : " not")));
    DEBUG(("New Absolute: (%d,%d)\n", NewW, NewH));

    if (self->Original == NULL) {
	self->Original = rasterimage_Clone(pix);
	rasterimage_SetBitsPtr(pix, NULL);
	rasterimage_GetScaledSubraster(self->Original,
				       &self->DesiredSelection, NewW, NewH, pix);
    }
    else {
	struct rectangle sub, original;
	/* map Desired Selection within scaled version to selection within the Original */
	rectangle_SetRectSize(&original, 0, 0,
			      rasterimage_GetWidth(self->Original),
			      rasterimage_GetHeight(self->Original));
	if (FullySelected(self))
	    sub = original;
	else {
	    float wscale =
	      rasterimage_GetWidth(self->Original)/rasterimage_GetWidth(pix);
	    float hscale =
	      rasterimage_GetHeight(self->Original)/rasterimage_GetHeight(pix);
	    rectangle_SetRectSize(&sub,
				  rectangle_Left(&self->DesiredSelection)*wscale,
				  rectangle_Top(&self->DesiredSelection)*hscale,
				  rectangle_Width(&self->DesiredSelection)*wscale,
				  rectangle_Height(&self->DesiredSelection) * hscale);
	    rectangle_IntersectRect(&sub, &sub, &original); }
	DEBUG(("Original: (%d,%d,%d,%d)\n",
	       rectangle_Left(&original), rectangle_Top(&original),
	       rectangle_Width(&original), rectangle_Height(&original)));
	DEBUG(("Selection: (%d,%d,%d,%d)\n",
	       rectangle_Left(&sub), rectangle_Top(&sub),
	       rectangle_Width(&sub), rectangle_Height(&sub)));
	DEBUG(("%s: 0x%x\n", class_GetTypeName(self->Original), self->Original));

	rasterimage_GetScaledSubraster(self->Original, &sub, NewW, NewH, pix);
    }

    rectangle_SetRectSize(&self->DesiredSelection, 0, 0, NewW, NewH);
    rectangle_SetRectSize(&self->ViewSelection, 0, 0, NewW, NewH);

    if (NotFullSize(self))
	ZoomToVisualBounds(self, 0, 0);

    CenterViewSelection(self);

    rasterimage_NotifyObservers(pix, rasterview_SCALECHANGED);
}

static void ScaleReplaceCommand(self)
struct rasterview *self;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    struct rasterimage *pix;
    FILE *pasteFile;
    static char hdr[] = "\\begindata{raster,";
    char *hx = hdr;
    register c;

    if (ras == NULL || (pix = raster_GetPix(ras)) == NULL) return;

    /* Mostly taken from ReplaceCommand */
    pasteFile = im_FromCutBuffer(rasterview_GetIM(self));

    /* if the file does not begin with a raster, 
	we may as well scan until we find one XXX */

    while ((c = getc(pasteFile)) != EOF && *hx)
	if (c == *hx) hx++;
	else hx = (c == *hdr) ? hdr+1 : hdr;
    if (*hx) 
	message_DisplayString((struct view *)self, 0, "No raster found in cut buffer");
    else {
	register struct raster *RasFromPaste = raster_Create(0,0);
	struct rasterimage *NewPix;
	struct rectangle R;
	long x, y, w, h;

	while ((c=getc(pasteFile)) != '\n' && c != EOF) 
	{}	/* skip to end of header */

	rectangle_GetRectSize(&self->DesiredSelection, &x, &y, &w, &h);

	raster_Read(RasFromPaste, pasteFile, 1);
	NewPix = raster_GetPix(RasFromPaste);

	rasterimage_GetScaledSubraster(NewPix, &RasFromPaste->subraster,
				       w, h, NewPix);
	rectangle_SetRectSize(&R, 0, 0, w, h);
	rasterimage_BlitSubraster(pix, x, y, NewPix, &R, 0);

	raster_Destroy(RasFromPaste);

	rasterimage_NotifyObservers(pix, raster_BITSCHANGED);
    }
    im_CloseFromCutBuffer(rasterview_GetIM(self), pasteFile);
}

static void PostMenus(self)
struct rasterview *self;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    struct rasterimage *pix = NULL;
    long menumask;

    if ( ! self->HasInputFocus) return;
    if (ras != NULL) pix = raster_GetPix(ras);

    /* choose 'menumask' value */
    if (pix == NULL)
	menumask = menuNoPix;
    else {
	if (rasterimage_GetRO(pix)) {
	    /* XXX probably should have a readonly bit in the raster data object */
	    menumask = menuReadOnly;
	    if (self->InMessages) {
		/* XXX in  messages allow Crop, Uncrop, and, if fully selected, Negative */
		if (RegionSelect(self))
		    menumask |= menuPan | menuCenter;
		else if (Pan(self))
		    menumask |= menuSelect;

		if (Uncropped(self) && FullySelected(self))
		    /* Selection and subraster are both full image, include Negative */
		    menumask |= menuNegative;
	    }
	}
	else {
	    menumask = menuReadWrite;

	    if (RegionSelect(self))
		menumask |= menuTouchUp | menuPan | menuCenter;
	    else if (TouchUp(self))
		menumask |= menuSelect | menuPan;
	    else if (Pan(self))
		menumask |= menuSelect | menuTouchUp;

	    if (FullSize(self))
		menumask |= menuScale;

	    if (Uncropped(self) && FullySelected(self)) {
		/* Selection and subraster are both full image, include Rotate */
		/* Until this work in Zoomed mode: */
		if (FullSize(self)) {
		    menumask |= menuRotate; }

		if (! self->Shrunken && MatExtendPossible(self))
		    menumask |= menuExtend;
	    }
	}

	if (RegionSelect(self)) {
	    if (rectangle_Left(&self->DesiredSelection) != self->Xscroll/self->Scale
		|| rectangle_Top(&self->DesiredSelection) != self->Yscroll/self->Scale
		|| rectangle_IsEmptyRect(&self->DesiredSelection))
		menumask |= menuUpperLeft;
	    if (! rectangle_IsEqualRect(&self->ViewSelection, &self->DesiredSelection))
		menumask |= menuSelectAll; }

	if (Cropped(self)) {
	    /* Until this work in Zoomed mode: */
	    if (FullSize(self))
		menumask |= menuUncrop; }
	else {
	    /* Until Zooming while Cropped works. */
	    menumask |= menuZoomIn;
	    if (! FullySelected(self)) {
		/* Until this work in Zoomed mode: */
		if (FullSize(self))
		    menumask |= menuCrop; }

	if (NotFullSize(self)) {
	    menumask |= menuZoomOut;
	    menumask |= menuMoveDB;
	    if (! self->DisplayBoxHidden)
		menumask |= menuHideDB; }
	}
    }
    if ( ! self->Shrunken && RastersInitiallyShrunk && self->embedded) 
	menumask |= menuShrink;

    DEBUG(("MenuMask: 0x%lx\n", menumask));
    if (self->AppMenus)
	menulist_SetMask(self->AppMenus, menumask);
    menulist_SetMask(self->Menus, menumask);
    rasterview_PostMenus(self, self->Menus);
}
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *\
 *	
 *	Methods
 *	
\* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  -----------  Added  4/6/89 -------------- */
/* 
  Note:  for a lot of these I call the menu command functions.  At the
      end of these functions there is a PostMenus function call.  Since this
           function does nothing, I'm assuming that's not a problem.
   Note:  for FitToSize, I have no clue what the "Original" stuff is all
       about - I just copied the code from ScaleCommand.
*/
void rasterview__FitToSize(self, logicalrect )
register struct rasterview  *self;
struct rectangle logicalrect; 
{  
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    struct rasterimage *pix;
    long x, y, w, h;
    long NewX, NewY, NewW, NewH;

    if (ras == NULL || (pix = raster_GetPix(ras)) == NULL) return;

    /* Until Scaling works while Zoomed... */
    if (NotFullSize(self)) return;

    rectangle_GetRectSize(&self->DesiredSelection, &x, &y, &w, &h);
    rectangle_GetRectSize(&logicalrect, &NewX, &NewY, &NewW, &NewH);

 
    DEBUG(("Original is%s NULL\n", ((self->Original == NULL) ? "" : " not")));
    DEBUG(("New Absolute: (%d,%d)\n", NewW, NewH));

    if (self->Original == NULL) {
	self->Original = rasterimage_Clone(pix);
	rasterimage_SetBitsPtr(pix, NULL);
	rasterimage_GetScaledSubraster(self->Original,
				       &self->DesiredSelection, NewW, NewH, pix);
    }
    else {
	struct rectangle sub, original;
	/* map Desired Selection within scaled version to selection within the Original */
	rectangle_SetRectSize(&original, 0, 0,
			      rasterimage_GetWidth(self->Original),
			      rasterimage_GetHeight(self->Original));
	if (FullySelected(self))
	    sub = original;
	else {
	    float wscale =
	      rasterimage_GetWidth(self->Original)/rasterimage_GetWidth(pix);
	    float hscale =
	      rasterimage_GetHeight(self->Original)/rasterimage_GetHeight(pix);
	    rectangle_SetRectSize(&sub,
				  rectangle_Left(&self->DesiredSelection)*wscale,
				  rectangle_Top(&self->DesiredSelection)*hscale,
				  rectangle_Width(&self->DesiredSelection)*wscale,
				  rectangle_Height(&self->DesiredSelection) * hscale);
	    rectangle_IntersectRect(&sub, &sub, &original); }
	DEBUG(("Original: (%d,%d,%d,%d)\n",
	       rectangle_Left(&original), rectangle_Top(&original),
	       rectangle_Width(&original), rectangle_Height(&original)));
	DEBUG(("Selection: (%d,%d,%d,%d)\n",
	       rectangle_Left(&sub), rectangle_Top(&sub),
	       rectangle_Width(&sub), rectangle_Height(&sub)));
	DEBUG(("%s: 0x%x\n", class_GetTypeName(self->Original), self->Original));

	rasterimage_GetScaledSubraster(self->Original, &sub, NewW, NewH, pix);
    }

    rectangle_SetRectSize(&self->DesiredSelection, 0, 0, NewW, NewH);
    rectangle_SetRectSize(&self->ViewSelection, 0, 0, NewW, NewH);

    if (NotFullSize(self))
	ZoomToVisualBounds(self, 0, 0);

    CenterViewSelection(self);

    rasterimage_NotifyObservers(pix, rasterview_SCALECHANGED);
}  
  

void rasterview__AutoCenter(self)
register struct rasterview  *self;
{   
    CenterCommand(self, 0);
}

void rasterview__ZoomRaster(self, zoomIn )
register struct rasterview  *self;
boolean zoomIn; 
{ 
    if (zoomIn)
	ZoomInCommand(self, 0);
    else
	ZoomOutCommand(self, 0);
}

void rasterview__SetPan(self)
register struct rasterview  *self;
{   
    PanCommand(self, 0);
}
	
void rasterview__SetRegionSelect(self)
register struct rasterview  *self;
{   
    RegionSelectCommand(self, 0);
}
	
void rasterview__SetTouchUp(self)
register struct rasterview  *self;
{   
    TouchUpCommand(self, 0);
}
	
void rasterview__RotateRaster(self)
register struct rasterview  *self;
{   
    RotateCommand(self, 0);
}
/*  -----------  Added 4/6/89 -------------- */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *\
 *	
 *	Override methods
 *	
\* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

boolean rasterview__InitializeClass(ClassID)
struct classhdr *ClassID;
{
    struct proctable_Entry *proc = NULL;
    DEBUG(("ENTER rasterview__InitializeClass\n"));
    EmbeddedMenus = menulist_New();
    ApplicationMenus = menulist_New();
    EmbeddedKeymap = keymap_New();
    ApplicationKeymap = keymap_New();

/*    bind_BindList(EmbeddedBindings, EmbeddedKeymap, EmbeddedMenus,
		   &rasterview_classinfo);
    bind_BindList(ApplicationBindings, ApplicationKeymap, ApplicationMenus,
		   &rasterview_classinfo); */

    /* Embedded Items: */

    proc = proctable_DefineProc("raster-copy-subraster", CopyCommand,
				 &rasterview_classinfo, NULL,
				 "Copy selected subraster to kill-buffer.");
    keymap_BindToKey(EmbeddedKeymap, "\033w", proc, 0);	/* ESC - w */
    menulist_AddToML(EmbeddedMenus, "Copy~3", proc, 0, menuCopy);

    proc = proctable_DefineProc("raster-replace-subraster", ReplaceCommand,
				 &rasterview_classinfo, NULL,
				 "Replace selected subraster from kill-buffer.");
    keymap_BindToKey(EmbeddedKeymap, "\033y", proc, 0);	/* ESC - y */
    menulist_AddToML(EmbeddedMenus, "Replace~13", proc, 0, menuOps);

    proc = proctable_DefineProc("raster-replace-subraster", ReplaceCommand,
				 &rasterview_classinfo, NULL,
				 "Replace selected subraster from kill-buffer.");
    keymap_BindToKey(EmbeddedKeymap, "\031", proc, 0);	/* ^Y */
    menulist_AddToML(EmbeddedMenus, "Replace~13", proc, 0, menuOps);

    proc = proctable_DefineProc("raster-scale-replace", ScaleReplaceCommand,
				 &rasterview_classinfo, NULL,
				 "Scale image in paste buffer to fit in selected Region");
    keymap_BindToKey(EmbeddedKeymap, "\033\031", proc, 0);	/* ESC ^Y */
    menulist_AddToML(EmbeddedMenus, "Replace Scaled~15", proc, 0, menuOps);

    proc = proctable_DefineProc("raster-negative", ModifyCommand,
				 &rasterview_classinfo, NULL,
				 "Interchange black and white within selection");
    keymap_BindToKey(EmbeddedKeymap, "\033n", proc, -1);	/* ESC - n */
    menulist_AddToML(EmbeddedMenus, "Raster Ops~20,Negative~21", proc, -1, menuNegative);

    proc = proctable_DefineProc("raster-white", ModifyCommand,
				 &rasterview_classinfo, NULL,
				 "Set white within selection");
    keymap_BindToKey(EmbeddedKeymap, "\033W", proc, 0);	/* ESC - W */
    menulist_AddToML(EmbeddedMenus, "Raster Ops~20,White~22", proc, 0, menuOps);

    proc = proctable_DefineProc("raster-black", ModifyCommand,
				 &rasterview_classinfo, NULL,
				 "Set black within selection");
    keymap_BindToKey(EmbeddedKeymap, "\033B", proc, 1);	/* ESC - B */
    menulist_AddToML(EmbeddedMenus, "Raster Ops~20,Black~23", proc, 1, menuOps);

    proc = proctable_DefineProc("raster-gray", ModifyCommand,
				 &rasterview_classinfo, NULL,
				 "Set gray within selection");
    keymap_BindToKey(EmbeddedKeymap, "\033G", proc, 2);	/* ESC - G */
    menulist_AddToML(EmbeddedMenus, "Raster Ops~20,Gray~24", proc, 2, menuOps);

    proc = proctable_DefineProc("raster-mirror-left-right", ModifyCommand,
				 &rasterview_classinfo, NULL,
				 "Interchange left and right within selection");
    keymap_BindToKey(EmbeddedKeymap, "\033L", proc, 3);	/* ESC - L */
    menulist_AddToML(EmbeddedMenus, "Raster Ops~20,Mirror LR~25", proc, 3, menuOps);

    proc = proctable_DefineProc("raster-mirror-up-down", ModifyCommand,
				 &rasterview_classinfo, NULL,
				 "Interchange top and bottom within selection");
    keymap_BindToKey(EmbeddedKeymap, "\033U", proc, 4);	/* ESC - U */
    menulist_AddToML(EmbeddedMenus, "Raster Ops~20,Mirror UD~26", proc, 4, menuOps);

    proc = proctable_DefineProc("raster-rotate", RotateCommand,
				 &rasterview_classinfo, NULL,
				 "Rotate entire raster image one-quarter turn clockwise");
    keymap_BindToKey(EmbeddedKeymap, "\033R", proc, 0);	/* ESC - R */
    menulist_AddToML(EmbeddedMenus, "Raster Ops~20,Rotate~27", proc, 0, menuRotate);

    proc = proctable_DefineProc("raster-scale", ScaleCommand,
				 &rasterview_classinfo, NULL,
				 "Scale the image");
    keymap_BindToKey(EmbeddedKeymap, "\033S", proc, 0);	/* ESC - S */	
    menulist_AddToML(EmbeddedMenus, "Raster Ops~20,Scale~32", proc, 0, menuScale);

    proc = proctable_DefineProc("raster-shrink-image", ShrinkCommand,
				 &rasterview_classinfo, NULL,
				 "Shrink the image of the raster to a button");
    keymap_BindToKey(EmbeddedKeymap, "\033s", proc, 0);	/* ESC - s */
    menulist_AddToML(EmbeddedMenus, "Raster Ops~20,Shrink to Button~67", proc, 0, menuShrink);

    proc = proctable_DefineProc("raster-center-image", CenterCommand,
				 &rasterview_classinfo, NULL,
				 "Center the Selected Region");
    keymap_BindToKey(EmbeddedKeymap, "\033c", proc, 0);	/* ESC - c */
    menulist_AddToML(EmbeddedMenus, "Raster Ops~20,Center Region~72", proc, 0, menuCenter);

    proc = proctable_DefineProc("raster-upperleft-image", UpperLeftCommand,
				 &rasterview_classinfo, NULL,
				 "Upper Left the Selected Region");
    keymap_BindToKey(EmbeddedKeymap, "\033c", proc, 0);	/*  */
    menulist_AddToML(EmbeddedMenus, "Raster Ops~20,Upper Left Region~73", proc, 0, menuUpperLeft);

    proc = proctable_DefineProc("raster-zoom-in", ZoomInCommand,
				 &rasterview_classinfo, NULL,
				 "Zoom In using upper left of selected region as starting point.");
    keymap_BindToKey(EmbeddedKeymap, "\033Z", proc, 0);	/* ESC - Z */
    menulist_AddToML(EmbeddedMenus, "Raster~20,Zoom In~12", proc, 0, menuZoomIn);

    proc = proctable_DefineProc("raster-zoom-out", ZoomOutCommand,
				 &rasterview_classinfo, NULL,
				 "Zoom Out");
    keymap_BindToKey(EmbeddedKeymap, "\033z", proc, 0);	/* ESC - z */
    menulist_AddToML(EmbeddedMenus, "Raster~20,Zoom Out~13", proc, 0, menuZoomOut);

    proc = proctable_DefineProc("raster-zoom-out-to-normal-size", NormalSizeCommand,
				 &rasterview_classinfo, NULL,
				 "Normal Size");
    keymap_BindToKey(EmbeddedKeymap, "\033N", proc, 0);	/* ESC - N */
    menulist_AddToML(EmbeddedMenus, "Raster~20,Normal Size~15", proc, 0, menuZoomOut);

    proc = proctable_DefineProc("raster-select-entire", SelectAllCommand,
				 &rasterview_classinfo, NULL,
				 "Select entire raster");
    keymap_BindToKey(EmbeddedKeymap, "\033N", proc, 0);	/* */
    menulist_AddToML(EmbeddedMenus, "Raster~20,Select All~22", proc, 0, menuSelectAll);

    proc = proctable_DefineProc("raster-region-select", RegionSelectCommand,
				 &rasterview_classinfo, NULL,
				 "Region Select Mode");
    keymap_BindToKey(EmbeddedKeymap, "\033\030R", proc, 0);	/* ESC - ^X R */
    menulist_AddToML(EmbeddedMenus, "Raster~20,Region Select~22", proc, 0, menuSelect);

    proc = proctable_DefineProc("raster-touchup", TouchUpCommand,
				 &rasterview_classinfo, NULL,
				 "Touch Up Mode");
    keymap_BindToKey(EmbeddedKeymap, "\033\030T", proc, 0);	/* ESC - ^X T */
    menulist_AddToML(EmbeddedMenus, "Raster~20,Touch Up~23", proc, 0, menuTouchUp);

    proc = proctable_DefineProc("raster-pan", PanCommand,
				 &rasterview_classinfo, NULL,
				 "Pan Mode");
    keymap_BindToKey(EmbeddedKeymap, "\033\030P", proc, 0);	/* ESC - ^X P */
    menulist_AddToML(EmbeddedMenus, "Raster~20,Pan~24", proc, 0, menuPan);

    proc = proctable_DefineProc("raster-move-display-box", MoveDisplayBoxCommand,
				 &rasterview_classinfo, NULL,
				 "Move the location of the Display Box. ");
    keymap_BindToKey(EmbeddedKeymap, "\033M", proc, 1);	/* ESC - M */
    menulist_AddToML(EmbeddedMenus, "Raster~20,Move Display Box~32", proc, 0, menuMoveDB);

    proc = proctable_DefineProc("raster-hide-display-box", HideDisplayBoxCommand,
				 &rasterview_classinfo, NULL,
				 "Hide the Display Box. ");
    keymap_BindToKey(EmbeddedKeymap, "\033H", proc, 1);	/* ESC - H */
    menulist_AddToML(EmbeddedMenus, "Raster~20,Hide Display Box~34", proc, 0, menuHideDB);

    proc = proctable_DefineProc("raster-set-print-scaling", SetPrintSizeCommand,
				 &rasterview_classinfo, NULL,
				 "Set the scaling factors used when printing");
    keymap_BindToKey(EmbeddedKeymap, "\033P", proc, 0);	/* ESC - P */
    menulist_AddToML(EmbeddedMenus, "Raster~30,Set Print Size~53", proc, 0, menuOps);

    proc = proctable_DefineProc("raster-extend-to-mat", ExtendToMatCommand,
				 &rasterview_classinfo, NULL,
				 "Add pixels to image to reach the bordering matte");
    keymap_BindToKey(EmbeddedKeymap, "\033X", proc, 0);	/* ESC - X */
    menulist_AddToML(EmbeddedMenus, "Raster~30,Extend to Mat~55", proc, 0, menuExtend);

    proc = proctable_DefineProc("raster-crop", CropCommand,
				 &rasterview_classinfo, NULL,
				 "Crop image to the selection");
    keymap_BindToKey(EmbeddedKeymap, "\033C", proc, 0);	/* ESC - C */
    menulist_AddToML(EmbeddedMenus, "Raster~30,Crop~63", proc, 0, menuCrop);

    proc = proctable_DefineProc("raster-uncrop", UncropCommand,
				 &rasterview_classinfo, NULL,
				 "Revert image to entire raster");
    keymap_BindToKey(EmbeddedKeymap, "\033c", proc, 0);	/* ESC - c */
    menulist_AddToML(EmbeddedMenus, "Raster~30,Uncrop~65", proc, 0, menuUncrop);

    proc = proctable_DefineProc("raster-toggle-debug", ToggleDebug,
				 &rasterview_classinfo, NULL,
				 "Toggle the rasterview debug flag");
    keymap_BindToKey(EmbeddedKeymap, "\033D", proc, 0);	/* ESC - D */
    /* No Menu Item for Debuging. */

    proc = proctable_DefineProc("raster-read-file", ReadFileCommand,
				 &rasterview_classinfo, NULL,
				 "Read a raster file");
    keymap_BindToKey(EmbeddedKeymap, "\033\022r", proc, 0);	/* ESC - ^R - r */
    menulist_AddToML(EmbeddedMenus, "File,Read Raster~12", proc, 0, menuRead);

    /* Application Items: */

    proc = proctable_DefineProc("raster-read-file", ReadFileCommand,
				 &rasterview_classinfo, NULL,
				 "Read a raster file");
    keymap_BindToKey(ApplicationKeymap, "\033\022r", proc, 0);	/* ESC - ^R - r */
    menulist_AddToML(ApplicationMenus, "Raster I/O~42,Read Raster~12", proc, 0, menuRead);

    proc = proctable_DefineProc("raster-read-macpaint", RasterIOCommand,
				 &rasterview_classinfo, NULL,
				 "Read a file allegedly in MacPaint format");
    keymap_BindToKey(ApplicationKeymap, "\033\022Im", proc, (long)InMacPaint);	/* ESC - ^R - I - m */
    menulist_AddToML(ApplicationMenus, "Raster I/O~42,Read MacPaint~22", proc, (long)InMacPaint, menuRead);

    proc = proctable_DefineProc("raster-write-macpaint", RasterIOCommand,
				 &rasterview_classinfo, NULL,
				 "Write file in MacPaint format");
    keymap_BindToKey(ApplicationKeymap, "\033\022IM", proc, (long)OutMacPaint);	/* ESC - ^R - I - M */
    menulist_AddToML(ApplicationMenus, "Raster I/O~42,Write MacPaint~24", proc, (long)OutMacPaint, menuWrite);

    proc = proctable_DefineProc("raster-write-postscript", RasterIOCommand,
				 &rasterview_classinfo, NULL,
				 "Write file in postscript format");
    keymap_BindToKey(ApplicationKeymap, "\033\022IP", proc, (long)OutPostscript);	/* ESC - ^R - I - P */
    menulist_AddToML(ApplicationMenus, "Raster I/O~42,Write Postscript~32", proc, (long)OutPostscript, menuWrite);

    proc = proctable_DefineProc("raster-read-rasterfile", ReadFileCommand,
				 &rasterview_classinfo, NULL,
				 "Read a file allegedly in the old ITC RasterFile format");
    keymap_BindToKey(ApplicationKeymap, "\033\022r", proc, 0);	/* ESC - ^R - r *//* same as Read Raster above */
    menulist_AddToML(ApplicationMenus, "Raster I/O~42,Read Old Raster~42", proc, 0, menuRead);


    RastersInitiallyShrunk = environ_GetProfileSwitch("RastersInitiallyShrunk", FALSE);
    DEBUG(("LEAVE rasterview__InitializeClass\n"));
    return TRUE;
}


boolean rasterview__InitializeObject(ClassID, self)
struct classhdr *ClassID;
register struct rasterview  *self;
{
    ENTER(rasterview__InitializeObject);
    self->Menus = menulist_DuplicateML(EmbeddedMenus, self);
    self->AppMenus = NULL;
    /* ApplicationMenus are added in GetApplicationLayer */

    self->Keystate = keystate_Create(self, EmbeddedKeymap);
    self->Cursor[0] = NULL;
    self->Cursor[1] = NULL;
    self->Cursor[2] = NULL;
    rectangle_EmptyRect(&self->PixChanged);
    self->needsFullUpdate = TRUE;
    self->OnScreen = FALSE;
    self->embedded = TRUE;
    self->DraggingEdge = 0;
    self->HasInputFocus = FALSE;
    self->IgnoreUntilNextUpTransition = FALSE;
    self->Mode = 0;
    rectangle_EmptyRect(&self->ViewSelection);
    self->Scale = 1;
    self->Original = NULL;
    self->Expansion = NULL;
    self->TouchUpX = self->TouchUpY = 0;
    self->HighlightIsGrey = FALSE;
    self->Xscroll = self->Yscroll = 0;
    self->InMessages = FALSE;
    self->CheckedParent = FALSE;
    rectangle_EmptyRect(&self->CurSelection);
    rectangle_EmptyRect(&self->DesiredSelection);
    self->Shrunken = RastersInitiallyShrunk;
    self->NeedsCentering = TRUE;

    rectangle_EmptyRect(&self->DisplayBox);
    rectangle_EmptyRect(&self->DisplayBoxSelection);
    self->DBXscroll = self->DBYscroll = 0;
    self->MovingDisplayBox = FALSE;
    self->DisplayBoxHidden = TRUE;
    self->DisplayBoxOverlapped = FALSE;

    LEAVE(rasterview__InitializeObject);
    return TRUE;
}

void rasterview__FinalizeObject(ClassID, self)
struct classhdr *ClassID;
register struct rasterview  *self;
{
    ENTER(rasterview__FinalizeObject);
    menulist_Destroy(self->Menus);
    if (self->Original != NULL) {
	rasterimage_Destroy(self->Original);
	self->Original = NULL; }
    if (self->AppMenus != NULL)
	menulist_Destroy(self->AppMenus);
    keystate_Destroy(self->Keystate);
    if (self->Cursor[0] != NULL)
	cursor_Destroy(self->Cursor[0]);
    if (self->Cursor[1] != NULL)
	cursor_Destroy(self->Cursor[1]);
    if (self->Cursor[2] != NULL)
	cursor_Destroy(self->Cursor[2]);
    LEAVE(rasterview__FinalizeObject);
}

void rasterview__SetDataObject(self, ras)
register struct rasterview  *self;
register struct raster *ras;
{
    register struct raster *oldras = (struct raster *)rasterview_GetDataObject(self);
    DEBUG(("rasterview__SetDataObject(0x%lx, 0x%lx) was 0x%lx\n", self, ras, oldras));
    if (oldras == ras) return;	/* this is needed to avoid
				 Destroy'ing oldras in RemoveObserver */
    super_SetDataObject(self, ras);
    if (ras != NULL) {
	self->ViewSelection = self->DesiredSelection = ras->subraster;
	DEBUG(("VS: (%d,%d,%d,%d)\n",
	       rectangle_Left(&self->ViewSelection),
	       rectangle_Top(&self->ViewSelection),
	       rectangle_Width(&self->ViewSelection),
	       rectangle_Height(&self->ViewSelection))); }
    else 
	rectangle_EmptyRect(&self->DesiredSelection);
    rasterview_WantNewSize(self, self);
    LEAVE(rasterview__SetDataObject);
}

void rasterview__ObservedChanged(self, dobj, status)
register struct rasterview  *self;
struct raster *dobj;
long status;
{
    register struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    register struct rasterimage *pix;

    DEBUG(("Enter rasterview__ObservedChanged(0x%lx, 0x%lx, %d)   ras: 0x%lx\n", self, dobj, status, ras));
    if (ras == NULL) return;
    pix = raster_GetPix(ras);

    if (status == observable_OBJECTDESTROYED) return;

    switch (status) {
	case raster_BITSCHANGED:
	case pixelimage_DATACHANGED:
	    DEBUG(("   BitsChanged\n"));
	    if (self->Original != NULL) {
		rasterimage_Destroy(self->Original);
		self->Original = NULL; }

	    if (rasterimage_GetResized(pix)) 
		self->PixChanged = self->ViewSelection;
	    else {
		if (NotFullSize(self)) {
		    struct rectangle *C = rasterimage_GetChanged(pix);
		    rectangle_SetRectSize(&self->PixChanged,
					  (rectangle_Left(C) -rectangle_Left(&self->DisplayBoxSelection)) *self->Scale,
					  (rectangle_Top(C) -rectangle_Top(&self->DisplayBoxSelection)) *self->Scale,
					  rectangle_Width(C)*self->Scale,
					  rectangle_Height(C)*self->Scale);
			ReflectChangesInExpansion(self, C); }
		else
		    rectangle_UnionRect(&self->PixChanged, &self->PixChanged,
					rasterimage_GetChanged(pix)); }
	    DEBUG(("PixChanged: (%d,%d,%d,%d)\n", 
		   self->PixChanged.left, self->PixChanged.top, 
		   self->PixChanged.width, self->PixChanged.height));
	    rasterview_WantUpdate(self, self);
	    break;
	case raster_BOUNDSCHANGED:
	    DEBUG(("   BoundsChanged\n"));
	    /* The following is in the wrong place. What is needed is to be able to determine if the bounds changed in such a manner that the Original's bounds could be changed to reflect the bounds change. Since the bounds can change such that white space is added to the left/top of the image, resizing the Original is not enough. I am not sure that one CAN determine what to do in the general case. For now... */
	    if (self->Original != NULL) {
		rasterimage_Destroy(self->Original);
		self->Original = NULL; }
	    /* Break is intensionally left out here. */
	case rasterview_SCALECHANGED:
	    DEBUG(("   ScaleChanged\n"));
	    self->PixChanged = self->ViewSelection;	/* copy rectangle */
	    /* make sure the selection is inside the subraster, but otherwise retain the selection */
	    rectangle_IntersectRect(&self->DesiredSelection,
				    &self->DesiredSelection,
				    &self->ViewSelection);
	    self->needsFullUpdate = TRUE;
	    rasterview_WantNewSize(self, self);
	    /* the new size will force FullUpdate,
	     so we don't call WantUpdate */
	    /* XXX WantNewSize does nothing if is an ApplicationLayer */
	    if ( ! self->embedded)
		rasterview_WantUpdate(self, self);
	    break;
    }
    if (dobj == ras)
	raster_SetModified(ras);    /* this is a bogus place for this  XXX */
    LEAVE(rasterview__ObservedChanged);
}

void rasterview__ReceiveInputFocus(self)
register struct rasterview  *self;
{
    ENTER(rasterview__ReceiveInputFocus);
    self->Keystate->next = NULL;
    rasterview_PostKeyState(self, self->Keystate);

    self->HasInputFocus = TRUE;
    PostMenus(self);
    rasterview_WantUpdate(self, self);

    LEAVE(rasterview__ReceiveInputFocus);
}

void rasterview__LoseInputFocus(self)
register struct rasterview  *self;
{
    ENTER(rasterview__LoseInputFocus);
    self->HasInputFocus = FALSE;
    rasterview_WantUpdate(self, self);
    /* this is the wrong place? Moved to within the Hit
      if (TouchUp(self)) {
	  struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
	  struct rasterimage *pix;
	  if (ras != NULL && (pix = raster_GetPix(ras)) != NULL)
	      rasterimage_NotifyObservers(pix, raster_BITSCHANGED); } */
    LEAVE(rasterview__LoseInputFocus);
}

void rasterview__WantUpdate(self, requestor)
register struct rasterview  *self, *requestor;
{
    ENTER(rasterview__WantUpdate);
    if ( ! self->UpdateWanted) {
	self->UpdateWanted = TRUE;
	super_WantUpdate(self, self); }
    LEAVE(rasterview__WantUpdate);
}


void rasterview__FullUpdate(self, type, left, top, width, height)
register struct rasterview  *self;
register enum view_UpdateType  type;
register long  left, top, width, height;
{
    register struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    ENTER(rasterview__FullUpdate);
    if (type == view_Remove) {
	self->OnScreen = FALSE;
	return; }

    /* fixes bug that the view selection is not updated when raster_Read is called. */
    if (rectangle_IsEmptyRect(&self->ViewSelection))
	self->ViewSelection = self->DesiredSelection = ras->subraster;

    PostMenus(self);
    if ((type != view_FullRedraw  
	  && type != view_PartialRedraw
	  && type != view_LastPartialRedraw)
	 || rasterview_GetLogicalWidth(self) == 0 
	 || rasterview_GetLogicalHeight(self) == 0) 
	return;
    self->OnScreen = TRUE;

    /* 
      XXX this code sets ReadOnly in the 
      rasterimage if the parent is 'messages'
	  */
    if (! self->CheckedParent) {
	struct rasterimage *pix = raster_GetPix(ras);
	register struct view *v;
	self->CheckedParent = TRUE;
	for (v = (struct view *)self; v != NULL; v = v->parent) {
	    register char *nm = class_GetTypeName(v);
	    DEBUG(("parent: %s\n", nm));
	    if (strcmp(nm, "messages") == 0) {
		self->InMessages = TRUE;
		if (pix != NULL)
		    rasterimage_SetRO(pix, TRUE);
		break; } }
	DEBUG(("Done Checking parents.\n")); }
    if (type == view_FullRedraw) {
	struct rectangle r;
	/* must recompute graphics info because image
	 may be on different display hardware */
	self->WhitePattern = rasterview_WhitePattern(self);
	self->BlackPattern = rasterview_BlackPattern(self);
	self->GreyPattern = rasterview_GrayPattern(self, 1, 2);

	if (self->Cursor[0] == NULL) {
	    struct fontdesc *fd;
	    long junk;
	    DEBUG(("Creating Cursors\n"));
	    self->Cursor[RegionSelectMode] = cursor_Create(self);
	    self->Cursor[TouchUpMode] = cursor_Create(self);
	    self->Cursor[PanMode] = cursor_Create(self);
	    fd = fontdesc_Create("icon", 0, 12);
	    cursor_SetStandard(self->Cursor[RegionSelectMode], Cursor_Gunsight);
	    if (fontdesc_StringSize(fd, rasterview_GetDrawable(self), "!", &junk, &junk)
		> 5)
		cursor_SetGlyph(self->Cursor[TouchUpMode], fd, '!');
	    else
		cursor_SetStandard(self->Cursor[TouchUpMode], Cursor_Arrow);
	    cursor_SetStandard(self->Cursor[PanMode], Cursor_CrossHairs); }
	/* reset the cursor */
	DEBUG(("Finished Posting Cursors\n")); fflush(stdout);
	rasterview_GetVisualBounds(self, &r);
	rasterview_PostCursor(self, &r, self->Cursor[self->Mode]); }

    if (NotFullSize(self)) {
	DEBUG(("Call ZoomToVisualBounds\n"));
	ZoomToVisualBounds(self, self->Xscroll/self->Scale, self->Yscroll/self->Scale); }

    DEBUG(("Call RedrawRaster\n"));
    RedrawRaster(self, type, left, top, width, height);
    LEAVE(rasterview__FullUpdate);
}

void rasterview__Update(self)
register struct rasterview *self;
{
    ENTER(rasterview__Update);
    if (! self->OnScreen) return;

    PostMenus(self);
    if (self->needsFullUpdate)
	RedrawRaster(self, view_FullRedraw, 0, 0, -1, -1);
    else
	RedrawRaster(self, view_LastPartialRedraw, 0, 0, -1, -1);

    LEAVE(rasterview__Update);
}

enum view_DSattributes rasterview__DesiredSize(self, width, height, pass,
						desiredWidth, desiredHeight) 
register struct rasterview *self;
long width;
long height;
enum view_DSpass pass;
long *desiredWidth;
long *desiredHeight;
{
    register struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    register struct rasterimage *pix;
    ENTER(rasterview__DesiredSize);
    pix = raster_GetPix(ras);
    if (pix == NULL) {
	/* XXX Kludge: If there is no rasterimage, we create one */
	raster_SetPix(ras, pix = rasterimage_Create(91, 91));
	rectangle_SetRectSize(&self->DesiredSelection, 0, 0, 91, 91);
	self->Shrunken = FALSE; }
    if (ras != NULL && IsNotEmptyRect(&ras->subraster)) {
	/* leave BORDER pixels all around the image */
	*desiredWidth = rectangle_Width(&ras->subraster) + (2*BORDER);
	*desiredHeight = rectangle_Height(&ras->subraster) + (2*BORDER); }
    else {
	*desiredWidth = 95;
	*desiredHeight = 95; }
    if (self->Shrunken && *desiredHeight > 20) 
	*desiredHeight = 20;

    DEBUG(("Leave DesiredSize: %d x %d\n", *desiredWidth, *desiredHeight));

    return view_Fixed;
}

static char *MouseEvent[] = {
    "No Mouse Event",
    "Left Down",
    "Left Up",
    "Left Movement",
    "Right Down",
    "Right Up",
    "Right Movement" };
enum { DragCorner, DragTop, DragBottom, DragLeft, DragRight };

struct view * rasterview__Hit(self, action, x, y, num_clicks)
struct rasterview  *self;
enum view_MouseAction  action;
long  x, y, num_clicks;
{
    struct raster *ras = (struct raster *)rasterview_GetDataObject(self);

    DEBUG(("rasterview_Hit %s at (%d, %d)\n", MouseEvent[(long)action], x, y));
    if (ras == NULL || action == view_NoMouseEvent)
	return (struct view *)self;
    if (! self->OnScreen)
	return NULL;

    /* Ignore all hits until next up transition. */
    if (self->IgnoreUntilNextUpTransition) {
	if (action == view_LeftUp || action == view_RightUp) {
	    self->IgnoreUntilNextUpTransition = FALSE;
	    return NULL; }
	else
	    DEBUG(("Ignoring Mouse Movements until up transition.\n")); }

    else if (self->Shrunken) {
	self->Shrunken = FALSE;
	self->needsFullUpdate = TRUE;
	rasterview_WantNewSize(self, self);
	/* the new size will force FullUpdate, so we don't call WantUpdate */
	/* XXX WantNewSize does nothing if is an ApplicationLayer */
	if (! self->embedded)
	    rasterview_WantUpdate(self, self);
	/* This covers the case when the raster is shrunken and does not have the input focus. */
	if (! self->HasInputFocus)
	    rasterview_WantInputFocus(self, self);
	self->IgnoreUntilNextUpTransition = TRUE; }

    /* This covers the case when the raster is not shrunken and one just wants the input focus. */
    else if (! self->HasInputFocus) {
	rasterview_WantInputFocus(self, self);
	self->IgnoreUntilNextUpTransition = TRUE; }

    else if (self->MovingDisplayBox) {
	if (action == view_LeftUp) {
	    FinishMovingDisplayBox(self, x, y);
	    return NULL; }
	else
	    DEBUG(("Still Moving Display Box\n")); }

    /* This tests for clicking left within the Display Box. */
    else if (! self->DisplayBoxHidden
	      && NotFullSize(self) && action == view_LeftDown
	      && x > rectangle_Left(&self->DisplayBox) - 2*BORDER
	      && x < rectangle_Right(&self->DisplayBox) + 2*BORDER
	      && y > rectangle_Top(&self->DisplayBox) - 2*BORDER
	      && y < rectangle_Bottom(&self->DisplayBox) + 2*BORDER) {
	/* Begin moving the Display Box. */
	MoveDisplayBoxCommand(self, -1); }

    /* If in Pan Mode then do functions of panning. */
    else if (Pan(self)) {
	if (action == view_LeftDown || action == view_RightDown)
	    StartPanning(self, x, y);
	else if (action == view_LeftUp || action == view_RightUp) {
	    FinishPanning(self, x, y);
	    return NULL; }
	else
	    ContinuePanning(self, x, y); }

    /* At this point the hit is either for TouchUp or for Region Select. */
    else {
	struct rectangle SR;

	/* map coordinates to full size raster space. */
	x += self->Xoff;
	y += self->Yoff;
	if (NotFullSize(self)) {
	    x = rectangle_Left(&self->DisplayBoxSelection) + x/self->Scale;
	    y = rectangle_Top(&self->DisplayBoxSelection) + y/self->Scale;
	    SR = self->DisplayBoxSelection; }
	else
	    SR = self->ViewSelection;
	/* Note that coordinates are constrained to refer to actual pixels
	 (left+width and top+height are the addresses of pixels
	  just outside the raster) */
	if (x < rectangle_Left(&SR)) x = rectangle_Left(&SR);
	else if (x > rectangle_Left(&SR)+rectangle_Width(&SR)-1)
	    x = rectangle_Left(&SR)+rectangle_Width(&SR)-1;
	if (y < rectangle_Top(&SR)) y = rectangle_Top(&SR);
	else if (y > rectangle_Top(&SR)+rectangle_Height(&SR)-1)
	    y = rectangle_Top(&SR)+rectangle_Height(&SR)-1;

	if (TouchUp(self)) {
	    if (action == view_LeftDown)
		SetPixel(self, x, y, TRUE);
	    else if (action == view_LeftMovement)
		DrawLineTo(self, x, y, TRUE);
	    else if (action == view_RightDown)
		SetPixel(self, x, y, FALSE);
	    else if (action == view_RightMovement)
		DrawLineTo(self, x, y, FALSE);
	    else {
		struct rasterimage *pix;
		if ((pix = raster_GetPix(ras)) != NULL) {
		    DEBUG(("Notify Observers of Touchup\n"));
		    rasterimage_NotifyObservers(pix, raster_BITSCHANGED); } }
	    self->TouchUpX = x;
	    self->TouchUpY = y; }
	else {
	    long l, t, w, h;	/* top, left, width, height */
	    long farx, fary;

	    /* while mouse is down, have raster cursor throughout the window */
	    if (action == view_RightDown || action == view_LeftDown) {
		im_SetWindowCursor(rasterview_GetIM(self), self->Cursor[self->Mode]);
	    }
	    else if (action == view_RightUp || action == view_LeftUp)
		im_SetWindowCursor(rasterview_GetIM(self), NULL);

	    if (action == view_LeftUp) {
		if (num_clicks == 2) {
		    /* Double Left Click selects entire raster. */
		    rectangle_GetRectSize(&SR, &l, &t, &w, &h);
		    x = l;
		    y = t;
		    self->Xdown = x + w - 1;
		    self->Ydown = y + h - 1; } }
	    else if (action == view_LeftDown) {
		/* Single Left Click sets the desired selection to the single point under the mouse. */
		self->Xdown = x;
		self->Ydown = y; }
	    else if (action == view_RightDown) {
		/* Drag either a corner or an edge while right down. */
		struct rectangle *R = &self->DesiredSelection;
		long l, t, w, h;
		long r, b;
		rectangle_GetRectSize(R, &l, &t, &w, &h);
		r = l+w;
		b = t+h;
		if ((x < l || x > r) && y > t && y < b) {
		    if (y <= t+h/2)
			self->DraggingEdge = (int)DragTop;
		    else
			self->DraggingEdge = (int)DragBottom; }
		else if ((y < t || y > b) && x > l && x < r) {
		    if (x <= l+w/2)
			self->DraggingEdge = (int)DragLeft;
		    else 
			self->DraggingEdge = (int)DragRight; }
		else 
		    self->DraggingEdge = (int)DragCorner;
		/* set (Xdown, Ydown) to the furthest corner of old selection */
		farx = (x < l  ||  (x < r  &&  x-l < r-x)) ? r-1 : l;
		fary = (y < t  ||  (y < b  &&  y-t < b-y)) ? b-1 : t;
		self->Xdown = farx;
		self->Ydown = fary; }

	    switch (self->DraggingEdge) {
		case DragCorner:
		    break;
		case DragTop:
		    y = rectangle_Top(&self->DesiredSelection);
		    break;
		case DragBottom:
		    y = rectangle_Bottom(&self->DesiredSelection) - 1;
		    break;
		case DragLeft:
		    x = rectangle_Left(&self->DesiredSelection);
		    break;
		case DragRight:
		    x = rectangle_Right(&self->DesiredSelection) - 1; }

	    /* set DesiredSelection to have corners (x,y) and (self->Xdown,self->Ydown) */
	    farx = self->Xdown;
	    fary = self->Ydown;
	    l = (x<farx) ? x : farx;
	    t = (y<fary) ? y : fary;
	    w = x - farx;   if (w<0) w = -w;   w++;
	    h = y - fary;   if (h<0) h = -h;   h++;
	    rectangle_SetRectSize(&self->DesiredSelection, l, t, w, h);

	    /* if rightup or leftup then update the scroll bars. */
	    if (action == view_LeftUp  ||  action == view_RightUp) {
		self->DraggingEdge = 0;
		self->needsFullUpdate=FALSE;
		rasterview_WantUpdate(self, self); }
	    CorrectHighlight(self);
	} }

    if (action == view_LeftUp  ||  action == view_RightUp)
	PostMenus(self);

    /*  XXX when we have drawing operations,
      the first mouse hit without a self->dataobject->pix
      must create the pix */

    LEAVE(rasterview__Hit);
    return (struct view *)self;		/* where to send subsequent hits */
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *\
 *
 *	Printing
 *
\* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static char *PSheader[] = {
	"%s  /width %d def  /height %d def /xScale %0.4f def /yScale %0.4f def\n",
	"%s     width xScale mul height yScale mul scale\n",
	"%s     /picstr width 7 add 8 idiv string def\n",
	"%s        width height 1\n",
	"%s        [width    0    0    height neg    0    height]\n",
	"%s        {currentfile picstr readhexstring pop}\n",
	"%s     image\n",
	NULL };

void rasterview__Print(self, file, processor, format, toplevel)
register struct rasterview	*self;
register FILE   	*file;
register char	*processor;
register char	*format;
register boolean	toplevel;
{
    register struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    register struct rasterimage *pix;
    char **psx;
    long row;
    short buf[300];
    long left, top, width, height;
    long wpts, hpts;  /* image dimensions in points */
    double xdscale, ydscale;
    char *prefix;

    ENTER(rasterview__Print);

    if (ras == NULL || (pix=raster_GetPix(ras)) == NULL) return;

    rectangle_GetRectSize(&self->ViewSelection, &left, &top, &width, &height);

    /* compute size in points and scaling as float values 
      add half unit to numerator to achieve rounding */
    wpts = (width * ras->xScale + (raster_UNITSCALE/2)) / raster_UNITSCALE;
    hpts = (height * ras->yScale + (raster_UNITSCALE/2)) / raster_UNITSCALE;

    if (ras->xScale == raster_UNITSCALE/2 
	 &&  ras->yScale == raster_UNITSCALE/2) {
	/* restrict to 6" by 9"  (1 inch = 72 points) */
	if (wpts > 6 * 72) { 
	    ras->xScale = ras->yScale =
	      6 * 72 * raster_UNITSCALE / width;
	    hpts = (height * ras->yScale + (raster_UNITSCALE/2)) 
	      / raster_UNITSCALE; }
	if (hpts > 9 * 72) 
	    ras->xScale = ras->yScale = 9 * 72 * raster_UNITSCALE / height;
	wpts = (width * ras->xScale + (raster_UNITSCALE/2)) / raster_UNITSCALE;
	hpts = (height * ras->yScale + (raster_UNITSCALE/2)) / raster_UNITSCALE; }
    xdscale = ((float) ras->xScale) / raster_UNITSCALE;
    ydscale = ((float) ras->yScale) / raster_UNITSCALE;

    if (strcmp(processor, "troff") == 0) {
	/* output to troff */
	if (toplevel)
	    /* take care of initial troff stream */
	    texttroff_BeginDoc(file);
	/*  Put macro to interface to postscript */
	texttroff_BeginPS(file, wpts, hpts);
	prefix = "\\!  ";
    }
    else if (strcmp(format, "troff") == 0)
	prefix = "\\!  ";
    else prefix = "";

    for (psx = PSheader; *psx; psx++)
	fprintf(file, *psx, prefix, width, height, xdscale, ydscale);

    /* generate bits of image
      XXX punt rotation and reflection  */
    for (row = top;  row < top+height; row++) {
	rasterimage_GetRow(pix, left, row, width, buf); 
	fprintf(file, "%s  ", prefix);
	heximage_WriteRow(file, buf, width, ! (ras->options & raster_INVERT)); }

    if (strcmp(processor, "troff") == 0) {
	texttroff_EndPS(file, wpts, hpts);
	if (toplevel)
	    texttroff_EndDoc(file); }

    LEAVE(rasterview__Print);
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *\
 *
 *	Scrolling
 *	
 *	The coordinate space defined for the scrollbars by getinfo uses the 
 *	self->dataobject->subraster.  Its upper left is 0,0.  The ends of the scroll bar
 *	correspond to the width and height of the subraster.
 *	
\* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static void	y_getinfo(), y_setframe(), x_getinfo(), x_setframe();
static long	y_whatisat(), x_whatisat();
static struct scrollfns	vertical_scroll_interface =
		{y_getinfo, y_setframe, NULL, y_whatisat};
static struct scrollfns	horizontal_scroll_interface =
		{x_getinfo, x_setframe, NULL, x_whatisat};

static void x_getinfo(self, total, seen, dot)
register struct rasterview  *self;
register struct range  *total, *seen, *dot;
{
    register struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    if (ras == NULL) {
	total->beg = seen->beg = seen->end = dot->beg = dot->end = 0;
	total->end = 1;
	return; }

    dot->beg = rectangle_Left(&self->DesiredSelection);
    dot->end = rectangle_Right(&self->DesiredSelection);
    if (NotFullSize(self)) {
	total->beg = rectangle_Left(&self->ViewSelection)*self->Scale;
	total->end = rectangle_Right(&self->ViewSelection)*self->Scale;
	seen->beg = rectangle_Left(&self->DisplayBoxSelection)*self->Scale;
	seen->end = rectangle_Right(&self->DisplayBoxSelection)*self->Scale;
    	dot->beg *= self->Scale;
	dot->end *= self->Scale;
    }
    else {
	total->beg = rectangle_Left(&self->ViewSelection);
	total->end = rectangle_Right(&self->ViewSelection);
	seen->beg = rectangle_Left(&self->ViewSelection) + self->Xscroll;
	seen->end = seen->beg + rasterview_GetLogicalWidth(self) - (2*BORDER);
    }

    if (seen->beg < total->beg) seen->beg = total->beg;
    if (seen->end > total->end) seen->end = total->end;
    if (dot->end > total->end) dot->end = total->end;
    DEBUG(("X info => total (%d, %d) seen (%d, %d) dot (%d, %d) Scr: %d\n", 
	    total->beg, total->end, seen->beg, seen->end, dot->beg, dot->end,
	    self->Xscroll));
}

static long x_whatisat(self, coordinate, outof)
register struct rasterview  *self;
register long  coordinate, outof;
{
    register long  value;
    register struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    if (ras == NULL) return 0;
    if (NotFullSize(self))
	value = (coordinate - (coordinate % self->Scale)) +
	  rectangle_Left(&self->DisplayBoxSelection)*self->Scale +
	  ((self->Xscroll < 0) ? self->Xscroll : 0);
    else
	value = coordinate + rectangle_Left(&self->ViewSelection) + self->Xscroll;
    DEBUG(("X what (%d, %d) => %d\n", coordinate, outof, value));
    return value;
}

static void  x_setframe(self, position, coordinate, outof) 
register struct rasterview *self;
int  position;
long  coordinate, outof;
{
    register struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    long oldscroll = self->Xscroll;
    if (ras == NULL) return;

    DEBUG(("Enter x_setframe(%d,%d,%d)\n", position, coordinate, outof));

    if (coordinate != 0 && position == rectangle_Left(&self->ViewSelection))
	position = self->Xscroll + rectangle_Left(&self->ViewSelection);
    if (Cropped(self))
	position -= rectangle_Left(&self->ViewSelection);

    self->Xscroll = position - coordinate;
    ClipScroll(self);

    if (self->Xscroll != oldscroll) {
	if (NotFullSize(self)) {
	    rectangle_EmptyRect(&self->DisplayBoxSelection);
	    UpdateZoomedSelection(self,
				  self->Xscroll/self->Scale,
				  self->Yscroll/self->Scale); }
	self->needsFullUpdate = TRUE;
	rasterview_WantUpdate(self, self); }

    DEBUG(("X set (%d, %d, %d) => %d Old: %d\n", 
	    position, coordinate, outof, self->Xscroll, oldscroll));
    DEBUG(("Scroll: (%d,%d)\n", self->Xscroll, self->Yscroll));
}

static void y_getinfo(self, total, seen, dot)
register struct rasterview  *self;
register struct range  *total, *seen, *dot;
{
    register struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    if (ras == NULL) {
	total->beg = seen->beg = seen->end = dot->beg = dot->end = 0;
	total->end = 1;
	return; }

    dot->beg = rectangle_Top(&self->DesiredSelection);
    dot->end = rectangle_Bottom(&self->DesiredSelection);
    if (NotFullSize(self)) {
	total->beg = rectangle_Top(&self->ViewSelection)*self->Scale;
	total->end = rectangle_Bottom(&self->ViewSelection)*self->Scale;
	seen->beg = rectangle_Top(&self->DisplayBoxSelection)*self->Scale;
	seen->end = rectangle_Bottom(&self->DisplayBoxSelection)*self->Scale;
    	dot->beg *= self->Scale;
	dot->end *= self->Scale;
    }
    else {
	total->beg = rectangle_Top(&self->ViewSelection);
	total->end = rectangle_Bottom(&self->ViewSelection);
	seen->beg = rectangle_Top(&self->ViewSelection) + self->Yscroll;
	seen->end = seen->beg + rasterview_GetLogicalHeight(self) - (2*BORDER);
    }

    if (seen->beg < total->beg) seen->beg = total->beg;
    if (seen->end > total->end) seen->end = total->end;
    if (dot->end > total->end) dot->end = total->end;
    DEBUG(("Y info => total (%d, %d) seen (%d, %d) dot (%d, %d) Scr: %d\n", 
	    total->beg, total->end, seen->beg, seen->end, dot->beg, dot->end,
	    self->Yscroll));
}

static long y_whatisat(self, coordinate, outof)
register struct rasterview *self;
register long  coordinate, outof;
{
    register long  value;
    register struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    if (ras == NULL) return 0;
    if (NotFullSize(self))
	value = (coordinate - (coordinate % self->Scale)) +
	  rectangle_Top(&self->DisplayBoxSelection)*self->Scale +
	  ((self->Yscroll < 0) ? self->Yscroll : 0);
    else
	value = coordinate + rectangle_Top(&self->ViewSelection) + self->Yscroll;
    DEBUG(("Y what (%d, %d) => %d\n", coordinate, outof, value));
    return value;
}

static void  y_setframe(self, position, coordinate, outof)
register struct rasterview *self;
int  position;
long  coordinate, outof;
{
    register struct raster *ras = (struct raster *)rasterview_GetDataObject(self);
    long oldscroll = self->Yscroll;
    if (ras == NULL) return;

    DEBUG(("Enter y_setframe(%d,%d,%d)\n", position, coordinate, outof));

    if (coordinate != 0 && position == rectangle_Top(&self->ViewSelection))
	position = self->Yscroll + rectangle_Top(&self->ViewSelection);
    if (Cropped(self))
	position -= rectangle_Top(&self->ViewSelection);

    self->Yscroll = position - coordinate;
    ClipScroll(self);

    if (self->Yscroll != oldscroll) {
	if (NotFullSize(self)) {
	    rectangle_EmptyRect(&self->DisplayBoxSelection);
	    UpdateZoomedSelection(self,
				  self->Xscroll/self->Scale,
				  self->Yscroll/self->Scale); }
	self->needsFullUpdate = TRUE;
	rasterview_WantUpdate(self, self); }

    DEBUG(("Y set (%d, %d, %d) => %d Old: %d\n",
	    position, coordinate, outof, self->Yscroll, oldscroll));
    DEBUG(("Scroll: (%d,%d)\n", self->Xscroll, self->Yscroll));
}


struct view * rasterview__GetApplicationLayer(self)
register struct rasterview *self;
{
    register struct scroll *view;
    ENTER(rasterview__GetApplicationLayer);
    view = scroll_Create(self, scroll_LEFT | scroll_BOTTOM);
    scroll_SetView(view, self);
    self->embedded = FALSE;
    self->Shrunken=FALSE;
    menulist_ChainBeforeML(self->Menus, 
			    self->AppMenus=menulist_DuplicateML(ApplicationMenus, self), NULL);
    keystate_AddBefore(keystate_Create(self, ApplicationKeymap), self->Keystate);
    rasterview_WantInputFocus(self, self);
    LEAVE(rasterview__GetApplicationLayer);
    return	(struct view *) view;
}

struct scrollfns * rasterview__GetInterface(self, interface_name)
register struct rasterview *self;
register char  *interface_name;
{
    register struct scrollfns *interface;
    DEBUG(("GetInterface(%s)\n", interface_name));
    if (strcmp(interface_name, "scroll,vertical") == 0)
	interface = &vertical_scroll_interface;
    else if (strcmp(interface_name, "scroll,horizontal") == 0)
	interface = &horizontal_scroll_interface;
    else
	interface = NULL;
    return interface;
}
