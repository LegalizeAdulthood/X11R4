/* ********************************************************************** *\
 *         Copyright IBM Corporation 1989 - All Rights Reserved           *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

#include <stdio.h>
#include <class.h>
#include <pshbttnv.eh>
#include <pshbttn.ih>
#include <andrewos.h>
#include <buffer.ih>
#include <complete.ih>
#include <cursor.ih>
#include <environ.ih>
#include <fontdesc.ih>
#include <frame.ih>
#include <graphic.ih>
#include <im.ih>
#include <menulist.ih>
#include <message.ih>
#include <observe.ih>
#include <proctbl.ih>
#include <view.ih>

/* Defined constants and macros */
#define NO_MSG "Pushbutton (v10)"
#define PLAIN 0
#define BOXEDRECT 1
#define THREEDEE 2
#define ROUNDRECT 3

#define PROMPTFONT "andy12b"
#define FONT "andy"
#define FONTTYPE fontdesc_Bold
#define FONTSIZE 12
#define BUTTONDEPTH 4
#define BUTTONPRESSDEPTH 2
#define TEXTPAD 2

/* External Declarations */

/* Forward Declarations */

/* Global Variables */
static struct atom *pushedtrigger;


boolean
pushbuttonview__InitializeClass(c)
struct classheader *c;
{
/* 
  Initialize all the class data.
*/

  pushedtrigger = atom_Intern("buttonpushed");
  return(pushedtrigger != NULL);
}


boolean
pushbuttonview__InitializeObject(c, self)
struct classheader *c;
struct pushbuttonview *self;
{
/*
  Set up the data for each instance of the object.
*/
  self->lit = 0;
  if (!(self->cursor = cursor_Create(self))) return(FALSE);
  cursor_SetStandard(self->cursor, Cursor_Gunsight);
  observable_DefineTrigger(self, pushedtrigger);
  return(TRUE);
}


void
pushbuttonview__FinalizeObject(c, self)
struct classheader *c;
struct pushbuttonview *self;
{
  if (self->cursor) cursor_Destroy(self->cursor);
  self->cursor = NULL;
  return;
}


void
pushbuttonview__FullUpdate(self, type, left, top, width, height)
struct pushbuttonview *self;
enum view_UpdateType type;
long left, top, width, height;
{
/*
  Redisplay this object.  Specifically, set my font, and put my text label
  in the center of my display box.
*/

  struct rectangle Rect, Rect2;
  struct pushbutton *b = (struct pushbutton *) pushbuttonview_GetDataObject(self);
  struct fontdesc *my_fontdesc;
  struct graphic *my_graphic;
  struct FontSummary *my_FontSummary;
  int r2_bot, r_bot;
  int tx = 0, ty = 0;
  short t_op;
  char *text;
  int style;

  if (b) {
    style = pushbutton_GetStyle(b);
    pushbuttonview_GetLogicalBounds(self, &Rect);
    pushbuttonview_PostCursor(self, &Rect, self->cursor);
    my_graphic = (struct graphic *)pushbuttonview_GetDrawable(self);
    if (!(my_fontdesc = pushbutton_GetButtonFont(b))) {
      my_fontdesc= fontdesc_Create(FONT, FONTTYPE, FONTSIZE);
    }
    if (my_fontdesc) {
      pushbuttonview_SetFont(self, my_fontdesc);
      my_FontSummary =  fontdesc_FontSummary(my_fontdesc, my_graphic);
    }

    pushbuttonview_SetTransferMode(self, graphic_SOURCE);

    t_op = graphic_BETWEENLEFTANDRIGHT | graphic_ATBASELINE;
    text = pushbutton_GetText(b) ? pushbutton_GetText(b) : NO_MSG;

    switch (style) {
    case BOXEDRECT:
      Rect.width--;
      Rect.height--;
      /* Rect2 is the inner rect */
      Rect2.top = Rect.top + BUTTONDEPTH;
      Rect2.left = Rect.left + BUTTONDEPTH;
      Rect2.width = Rect.width - 2*BUTTONDEPTH;
      Rect2.height = Rect.height - 2*BUTTONDEPTH;
      tx = TEXTPAD + (Rect2.left + Rect2.width) / 2;
      if (my_FontSummary)
	ty = TEXTPAD + (Rect2.top + my_FontSummary->maxHeight - my_FontSummary->maxBelow);

      pushbuttonview_DrawRect(self, &Rect);
      pushbuttonview_DrawRect(self, &Rect2);
      pushbuttonview_MoveTo(self, tx, ty);
      pushbuttonview_DrawString(self, text, t_op);
      break;
    case ROUNDRECT:
      /* Rect2 is the inner rect for DrawRRect */
      Rect2.top = Rect.top;
      Rect2.left = Rect.left;
      Rect2.height = Rect2.top + BUTTONDEPTH;
      Rect2.width = Rect2.left + BUTTONDEPTH;
      tx = TEXTPAD + (Rect.left + Rect.width) / 2;
      if (my_FontSummary)
	ty = TEXTPAD + (Rect.top + my_FontSummary->maxHeight - my_FontSummary->maxBelow);

      pushbuttonview_DrawRRect(self, &Rect, &Rect2);
      pushbuttonview_MoveTo(self, tx, ty);
      pushbuttonview_DrawString(self, text, t_op);
      break;
    case THREEDEE:
      /* Rect2 is the inner (Text) region */
      Rect2.top = Rect.top + BUTTONDEPTH;
      Rect2.left = Rect.left + BUTTONDEPTH;
      Rect2.width = Rect.width - 2*BUTTONDEPTH;
      Rect2.height = Rect.height - 2*BUTTONDEPTH;
      r2_bot = (Rect2.top)+(Rect2.height);
      r_bot = (Rect.top)+(Rect.height);
      tx = TEXTPAD + (Rect2.left + Rect2.width) / 2;
      if (my_FontSummary)
	ty = TEXTPAD + (Rect2.top + my_FontSummary->maxHeight - my_FontSummary->maxBelow);

      pushbuttonview_FillRectSize(self, Rect.left, Rect.top, BUTTONDEPTH, Rect.height, pushbuttonview_GrayPattern(self, 1, 4));	/* left bar */

      pushbuttonview_FillRectSize(self, Rect.left + Rect.width - BUTTONDEPTH, Rect.top, BUTTONDEPTH, Rect.height, pushbuttonview_GrayPattern(self, 3, 4)); /* right bar */

      pushbuttonview_FillTrapezoid(self, Rect2.left, r2_bot, Rect2.width, Rect.left, r_bot, Rect.width, pushbuttonview_GrayPattern(self, 3, 4)); /* lower trapz */

      pushbuttonview_FillTrapezoid(self, Rect.left, Rect.top, Rect.width, Rect2.left, Rect2.top, Rect2.width, pushbuttonview_GrayPattern(self, 1, 4)); /* upper trapz */

      pushbuttonview_FillRect(self, &Rect2, pushbuttonview_GrayPattern(self,1,2)); /* the middle box */

      pushbuttonview_SetTransferMode(self, graphic_WHITE);
      pushbuttonview_MoveTo(self, tx+1, ty);
      pushbuttonview_DrawString(self, text, t_op);
      pushbuttonview_MoveTo(self, tx, ty+1);
      pushbuttonview_DrawString(self, text, t_op);
      pushbuttonview_MoveTo(self, tx+1, ty+1);
      pushbuttonview_DrawString(self, text, t_op);
      pushbuttonview_SetTransferMode(self, graphic_BLACK);
      pushbuttonview_MoveTo(self, tx, ty);
      pushbuttonview_DrawString(self, text, t_op);
      break;
    default:
      tx = (Rect.left + Rect.width) / 2;
      if (my_FontSummary)
	ty = (Rect.top + my_FontSummary->maxHeight - my_FontSummary->maxBelow);

      pushbuttonview_SetTransferMode(self, graphic_SOURCE);
      pushbuttonview_MoveTo(self, tx, ty);
      pushbuttonview_DrawString(self, text, t_op);
      break;
    } /* switch (style) */

  } /* if (b) */
  self->lit = 0;
}


void
pushbuttonview__Update(self)
struct pushbuttonview *self;  
{
/*
  Do an update.  Just set up the call to FullUpdate method.
*/
    struct rectangle r;

    pushbuttonview_EraseVisualRect(self);
    pushbuttonview_GetLogicalBounds(self, &r);
    pushbuttonview_FullUpdate(self, 0, r.left, r.top, r.width, r.height);
}


static int
RectEnclosesXY(r, x, y)
struct rectangle *r;
long x, y;
{
  return(   ( ((r->top)  <= y) && ((r->top + r->height) >= y) )
	 && ( ((r->left) <= x) && ((r->left + r->width) >= x) )
	 );
}


static void
HighlightButton(self)
struct pushbuttonview *self;
{
  struct pushbutton *b = (struct pushbutton *) pushbuttonview_GetDataObject(self);
  struct rectangle Rect, Rect2;
  int style;
  struct fontdesc *my_fontdesc;
  struct graphic *my_graphic;
  struct FontSummary *my_FontSummary;
  int tx, ty;
  short t_op;
  char *text;
  
  if (!(self->lit)) {
    style = pushbutton_GetStyle(b);
    pushbuttonview_GetLogicalBounds(self, &Rect);
    
    switch(style) {
    case BOXEDRECT:
      /* Rect2 is the inner rect */
      Rect2.top = Rect.top + BUTTONDEPTH;
      Rect2.left = Rect.left + BUTTONDEPTH;
      Rect2.width = Rect.width - 2*BUTTONDEPTH;
      Rect2.height = Rect.height - 2*BUTTONDEPTH;

      pushbuttonview_SetTransferMode(self, graphic_INVERT);
      pushbuttonview_FillRect(self, &Rect,pushbuttonview_BlackPattern(self));
      pushbuttonview_FillRect(self, &Rect2,pushbuttonview_BlackPattern(self));

      break;
    case ROUNDRECT:
      /* Rect2 is the inner rect for DrawRRect */
      Rect2.top = Rect.top;
      Rect2.left = Rect.left;
      Rect2.height = Rect2.top + BUTTONDEPTH;
      Rect2.width = Rect2.left + BUTTONDEPTH;

      pushbuttonview_SetTransferMode(self, graphic_INVERT);
      pushbuttonview_FillRRect(self, &Rect, &Rect2, pushbuttonview_BlackPattern(self));

      break;
    case THREEDEE:
      Rect2.top = Rect.top + BUTTONDEPTH;
      Rect2.left = Rect.left + BUTTONDEPTH;
      Rect2.width = Rect.width - 2*BUTTONDEPTH;
      Rect2.height = Rect.height - 2*BUTTONDEPTH;
      my_graphic = (struct graphic *)pushbuttonview_GetDrawable(self);
      if (!(my_fontdesc = pushbutton_GetButtonFont(b))) {
	my_fontdesc= fontdesc_Create(FONT, FONTTYPE, FONTSIZE);
      }
      if (my_fontdesc) {
	pushbuttonview_SetFont(self, my_fontdesc);
	my_FontSummary =  fontdesc_FontSummary(my_fontdesc, my_graphic);
      }
      t_op = graphic_BETWEENLEFTANDRIGHT | graphic_ATBASELINE;
      text = pushbutton_GetText(b) ? pushbutton_GetText(b) : NO_MSG;
      tx = TEXTPAD + (Rect2.left + Rect2.width) / 2;
      if (my_FontSummary)
	ty = TEXTPAD + (Rect2.top + my_FontSummary->maxHeight - my_FontSummary->maxBelow);
	  
      pushbuttonview_SetTransferMode(self, graphic_WHITE);
      pushbuttonview_MoveTo(self, tx, ty);
      pushbuttonview_DrawString(self, text, t_op);
      
      break;
      
    default:
      pushbuttonview_SetTransferMode(self, graphic_INVERT);
      pushbuttonview_FillRect(self,&Rect,pushbuttonview_BlackPattern(self));
      break;
    }
  }
  self->lit = 1;
}


static void
UnhighlightButton(self)
struct pushbuttonview *self;
{
  struct pushbutton *b = (struct pushbutton *) pushbuttonview_GetDataObject(self);
  struct rectangle Rect, Rect2;
  int style;
  struct fontdesc *my_fontdesc;
  struct graphic *my_graphic;
  struct FontSummary *my_FontSummary;
  int tx, ty;
  short t_op;
  char *text;
  
  if (self->lit) {
    style = pushbutton_GetStyle(b);
    pushbuttonview_GetLogicalBounds(self, &Rect);
    
    switch(style) {
    case BOXEDRECT:
      /* Rect2 is the inner rect */
      Rect2.top = Rect.top + BUTTONDEPTH;
      Rect2.left = Rect.left + BUTTONDEPTH;
      Rect2.width = Rect.width - 2*BUTTONDEPTH;
      Rect2.height = Rect.height - 2*BUTTONDEPTH;

      pushbuttonview_SetTransferMode(self, graphic_INVERT);
      pushbuttonview_FillRect(self, &Rect,pushbuttonview_BlackPattern(self));
      pushbuttonview_FillRect(self, &Rect2,pushbuttonview_BlackPattern(self));

      break;
    case ROUNDRECT:
      /* Rect2 is the inner rect for DrawRRect */
      Rect2.top = Rect.top;
      Rect2.left = Rect.left;
      Rect2.height = Rect2.top + BUTTONDEPTH;
      Rect2.width = Rect2.left + BUTTONDEPTH;

      pushbuttonview_SetTransferMode(self, graphic_INVERT);
      pushbuttonview_FillRRect(self, &Rect, &Rect2, pushbuttonview_BlackPattern(self));

      break;
    case THREEDEE:
      /* Rect2 is the inner (Text) region */
      Rect2.top = Rect.top + BUTTONDEPTH;
      Rect2.left = Rect.left + BUTTONDEPTH;
      Rect2.width = Rect.width - 2*BUTTONDEPTH;
      Rect2.height = Rect.height - 2*BUTTONDEPTH;
      my_graphic = (struct graphic *)pushbuttonview_GetDrawable(self);
      if (!(my_fontdesc = pushbutton_GetButtonFont(b))) {
	my_fontdesc= fontdesc_Create(FONT, FONTTYPE, FONTSIZE);
      }
      if (my_fontdesc) {
	pushbuttonview_SetFont(self, my_fontdesc);
	my_FontSummary =  fontdesc_FontSummary(my_fontdesc, my_graphic);
      }
      t_op = graphic_BETWEENLEFTANDRIGHT | graphic_ATBASELINE;
      text = pushbutton_GetText(b) ? pushbutton_GetText(b) : NO_MSG;
      tx = TEXTPAD + (Rect2.left + Rect2.width) / 2;
      if (my_FontSummary)
	ty = TEXTPAD + (Rect2.top + my_FontSummary->maxHeight - my_FontSummary->maxBelow);
      
      pushbuttonview_SetTransferMode(self, graphic_BLACK);
      pushbuttonview_MoveTo(self, tx, ty);
      pushbuttonview_DrawString(self, text, t_op);

      break;
      
    default:
      pushbuttonview_SetTransferMode(self, graphic_INVERT);
      pushbuttonview_FillRect(self,&Rect,pushbuttonview_BlackPattern(self));
      break;
    }
  }
  self->lit = 0;
}


struct view *
pushbuttonview__Hit(self, action, x, y, numclicks)
struct pushbuttonview *self;
long x, y;
enum view_MouseAction action;
long numclicks;  
{
/*
  Handle the button event.  Currently, semantics are:
    
    Left Down  -- Draw button pressed
    Right Down -- select button (Receive input focus, for menuing without activating)
    Left Up    -- draw button at rest, pull trigger
    Right Up   -- No Op
    Left Movement     -- unhighlight if moved off, highlight if moved on
    Right Movement -- No Op
*/
  struct cursor *wait_cursor;
  
  switch (action) {
  case view_LeftDown: 
    HighlightButton(self);
    pushbuttonview_WantInputFocus(self,self);
    break;
  case view_LeftMovement:
    {
      struct rectangle r;

      pushbuttonview_GetVisualBounds(self, &r);
      if (RectEnclosesXY(&r, x, y))
	HighlightButton(self);
      else
	UnhighlightButton(self);
    }
    break;
  case view_LeftUp:
    {
      short litp = self->lit;

      UnhighlightButton(self);
      if (litp) {
	if (wait_cursor = cursor_Create(self)) {
	  cursor_SetStandard(wait_cursor, Cursor_Wait);
	  im_SetProcessCursor(wait_cursor);
	  pushbuttonview_PullTrigger(self, pushedtrigger);
	  im_SetProcessCursor(NULL);
	  cursor_Destroy(wait_cursor);
	}
      }
    }
    break;
  case view_RightDown:
    pushbuttonview_WantInputFocus(self, self);
    break;
  }
  return((struct view *)self);
}


enum view_DSattributes 
pushbuttonview__DesiredSize(self, width, height, pass, desired_width, desired_height)
struct pushbuttonview *self;
long width;
long height;
enum view_DSpass pass;
long *desired_width;
long *desired_height;
{
/* 
  Tell parent that this object  wants to be as big as the box around its
  text string.  For some reason IM allows resizing of this object. (BUG)
*/

  struct fontdesc *my_fontdesc;
  struct FontSummary *my_FontSummary;
  struct graphic *my_graphic;
  struct pushbutton *b = (struct pushbutton *) pushbuttonview_GetDataObject(self);
  int style;

  style = pushbutton_GetStyle(b);

  my_graphic = (struct graphic *)pushbuttonview_GetDrawable(self);
  if (!(my_fontdesc = pushbutton_GetButtonFont(b))) {
    my_fontdesc= fontdesc_Create(FONT, FONTTYPE, FONTSIZE);
  }
  if (my_fontdesc) {
    fontdesc_StringSize(my_fontdesc, my_graphic, pushbutton_GetText(b) ? pushbutton_GetText(b) : NO_MSG, desired_width, desired_height);
    my_FontSummary =  fontdesc_FontSummary(my_fontdesc, my_graphic);
  }

  switch (style) {
  case BOXEDRECT:
  case ROUNDRECT:
  case THREEDEE:
    *desired_width = *desired_width + 2*TEXTPAD + 2*BUTTONDEPTH;
    if (my_FontSummary)
      *desired_height = my_FontSummary->maxHeight + 2*TEXTPAD + 2*BUTTONDEPTH;
    break;
  default:
    if (my_FontSummary)
      *desired_height = my_FontSummary->maxHeight;
    break;
  }

/*
  (BUG) I don't check to see if I can specify a size, I just do it.
  Will this break things?  What if I can't change my size?  Will I be
  Ugly?  What to do, what to do....
*/

  return(view_Fixed); /* (BUG) should disable user sizing, but this doesn't */
}


void
pushbuttonview__GetOrigin(self, width, height, originX, originY)
struct pushbuttonview *self;
long width, height;
long *originX, *originY;
{
/*
  We want this object to sit in-line with text, not below the baseline.
  Simply, we could negate the height as the originX, but then our
  text would be too high.  So, instead, we use the height of
  our font above the baseline
*/

  struct FontSummary *my_FontSummary;
  struct fontdesc *my_fontdesc;
  struct graphic *my_graphic;
  struct pushbutton *b = (struct pushbutton *) pushbuttonview_GetDataObject(self);
  int style;

  style = pushbutton_GetStyle(b);

  my_graphic = (struct graphic *)pushbuttonview_GetDrawable(self);
  if (!(my_fontdesc = pushbutton_GetButtonFont(b))) {
    my_fontdesc= fontdesc_Create(FONT, FONTTYPE, FONTSIZE);
  }
  if (my_fontdesc) {
    my_FontSummary =  fontdesc_FontSummary(my_fontdesc, my_graphic);
  }

  *originX = 0;
  switch(style) {
  case BOXEDRECT:
  case ROUNDRECT:
  case THREEDEE:
    if (my_FontSummary) 
      *originY = (my_FontSummary->maxHeight) - (my_FontSummary->maxBelow) + 1 + TEXTPAD + BUTTONDEPTH;
    break;
  default:
    if (my_FontSummary)
      *originY = (my_FontSummary->maxHeight) - (my_FontSummary->maxBelow) + 1;
    break;
  }
  return;
}

