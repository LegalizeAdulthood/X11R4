/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#include <sys/param.h>	/* for MAXPATHLEN */
#include <linkv.eh>
#include <link.ih>
#include <andrewos.h>
#include <atom.ih>
#include <buffer.ih>
#include <complete.ih>
#include <fontdesc.ih>
#include <frame.ih>
#include <im.ih>
#include <menulist.ih>
#include <message.ih>
#include <observe.ih>
#include <proctbl.ih>
#include <pshbttnv.ih>
#include <view.ih>

/* Defined constants and macros */
#define DBG(x) fprintf(stderr, "\nDebug: %s.", x);fflush(stderr);

#define PROMPTFONT "andy12b"
#define FONT "andy"
#define FONTSIZE 12
#define BUTTONDEPTH 4
#define BUTTONPRESSDEPTH 2
#define TEXTPAD 2

/* External Declarations */

/* Forward Declarations */
static void LabelProc(), LinkProc(), FontProc(), 
  WarpLink(), TargetProc(), AutolinkProc();
static boolean FindBuffer();
static char *FileName();
static struct view *FindViewofBuffer();

/* Global Variables */
static struct menulist *linkview_menulist = NULL;
static struct linkview *autolink_source = NULL;
static struct classinfo *proctype = NULL;


boolean
linkview__InitializeClass(c)
struct classheader *c;
{
/* 
  Initialize all the class data, particularly, set up the proc table entries 
  and the menu list (which is cloned for each instance of this class).
*/

  struct proctable_Entry *proc = NULL;

  proctype = class_Load("frame");
  linkview_menulist = menulist_New();

  proc = proctable_DefineProc("linkview-set-target", TargetProc, proctype, NULL, "Execute this proc from the frame of the the buffer for the target file of a link.  To be called after linkview-autolink.");

  proc = proctable_DefineProc("linkview-autolink", AutolinkProc, &linkview_classinfo, NULL, "Starts the autolink process.  Waits for linkview-set-target to be invoked, which tells this link what file to link to.");
  menulist_AddToML(linkview_menulist, "Link~1,Autolink~1", proc, NULL, 0);

  proc = proctable_DefineProc("linkview-set-link", LinkProc, &linkview_classinfo, NULL, "Prompts for user to set target filename of the link button.");
  menulist_AddToML(linkview_menulist, "Link~1,Set Link~11", proc, NULL, 0);

  proc = proctable_DefineProc("linkview-set-label-text", LabelProc, &linkview_classinfo, NULL, "Prompts for user to set the text string of the link button.");
  menulist_AddToML(linkview_menulist, "Link~1,Set Label~12", proc, NULL, 0);

  proc = proctable_DefineProc("linkview-set-font", FontProc, &linkview_classinfo, NULL, "Prompts for user to set the font of the link button.");
  menulist_AddToML(linkview_menulist, "Link~1,Set Font~13", proc, NULL, 0);

  return(TRUE);
}


boolean
linkview__InitializeObject(c, self)
struct classheader *c;
struct linkview *self;
{
/*
  Set up the data for each instance of the object (i.e.: clone the menu
  list, add the trigger recipient).
*/

  self->ml = menulist_DuplicateML(linkview_menulist, self);
  return(linkview_AddRecipient(self, atom_Intern("buttonpushed"), self, WarpLink, 0L));
}


void
linkview__FinalizeObject(c, self)
struct classheader *c;
struct linkview *self;
{
  return;
}



void
linkview__PostMenus(self, ml)
struct linkview *self;
struct menulist *ml;
{
/*
  Enable the menus for this object.
*/

  menulist_ClearChain(self->ml);
  if (ml) menulist_ChainAfterML(self->ml, ml, ml);
  super_PostMenus(self, self->ml);
}


static void
LabelProc(self, param)
struct linkview *self;
long param;
{
/*
  This is the routine which asks the user for a new text label.
  (BUG) Should not allow newlines (or should it?)
*/

  char buf[MAXPATHLEN];
  struct link *l = (struct link *)linkview_GetDataObject(self);
  char *oldtext;

  oldtext = link_LabelSetP(l) ? link_GetText(l) : NULL;
  if (message_AskForString(self,50,"Enter new text for button: ",
			   oldtext, buf, sizeof(buf)) >= 0) {
    link_SetText(l, buf);
    link_NotifyObservers(l, observable_OBJECTCHANGED);
    linkview_WantNewSize(self,self);
    linkview_WantUpdate(self, self);
    message_DisplayString(self, 10, "Changed link text.");
  }
}


	static void
LinkProc(self, param)
	struct linkview *self;
	char *param;
{
/*
  This is the routine which asks the user for the target of the link.
  Uses the (amazingly quick) completion package.
*/

	char buf[MAXPATHLEN];
	struct link *l = (struct link *)linkview_GetDataObject(self);

	if (param != NULL) 
		strcpy(buf, param);
	else if (completion_GetFilename(self, "New target file for link:  ",
			     link_GetLink(l) ? link_GetLink(l) : NULL,
			     buf, sizeof(buf), 0, 0) < 0) 
		return;

	link_SetLink(l, buf);
	if ( ! link_LabelSetP(l)) {
		link_SetText(l, FileName(buf));
		linkview_WantNewSize(self, self);
		linkview_WantUpdate(self, self);
		message_DisplayString(self, 10, "Changed link target (and label).");
	} else {
		message_DisplayString(self, 10, "Changed link target.");
	}
	link_NotifyObservers(l, observable_OBJECTCHANGED);
}


static void
FontProc(self, param)
struct linkview *self;
long param;
{
/*
  This is the routine which asks the user for a new font.
  It sucks, but I don't know how to smoothly integrate this button
  with a textview-like font change.  Oh well.  Font should really be 
  an indication of the *type* of the link.
*/

  char buf[MAXPATHLEN], name[MAXPATHLEN];
  long style, size;
  struct link *l = (struct link *)linkview_GetDataObject(self);
  struct fontdesc *fd;

  if (message_AskForString(self,50,"Enter new fontname for link: ", PROMPTFONT,
			   buf, sizeof(buf)) >= 0) {
    if (!fontdesc_ExplodeFontName(buf,name,sizeof(name),
				  &style, &size)) {
      message_DisplayString(self, 50, "Couldn't parse fontname.");
      return;
    }
    if ((fd = fontdesc_Create(name,style,size))!=NULL) {
      link_SetButtonFont(l, fd);
      link_NotifyObservers(l, observable_OBJECTCHANGED);
      linkview_WantNewSize(self,self);
      linkview_WantUpdate(self, self);
      message_DisplayString(self, 10, "Changed font.");
    } else {
      message_DisplayString(self, 50, "Font change failed.  Using old font.");
    }
  }
}


static boolean
FindBuffer(f,b)
struct frame *f;
struct buffer *b;
{
/*
  Little, dippy routine passed to frame_Enumerate to find the
  frame which contains the buffer we want.
*/

  return(frame_GetBuffer(f)==b);
}


static struct view *
FindViewofBuffer(b)
struct buffer *b;
{
/*
  I don't know why *I* have to do this, it should be a buffer method.
  Anyways, this tries to find the frame of our buffer.  If there is no
  such frame, make one, make a new IM for it (new window), and put the
  buffer in the frame in the IM.  *phew*
*/

  struct frame *f;
  struct im *im;

  if ((f = frame_Enumerate(FindBuffer, b))==NULL) {
    /* No frame--need to map buffer to new window */

    f = frame_New();
    frame_SetCommandEnable(f, TRUE);
    frame_PostDefaultHandler(f, "message", frame_WantHandler(f, "message"));
    frame_SetBuffer(f, b, TRUE);

    im = im_Create(NULL);
    im_SetView(im, f);
  }
  return((struct view *)f);
}


static void
WarpLink(self, triggerer, rock)
struct linkview *self;
struct observable *triggerer;
long rock;
{
/*
  Do the actual "warp".  The semantics I want are:
  If the file isn't in a buffer, put it in one.
  If the buffer isn't in a window, put it in one.
  Warp the cursor & set the input focus to the target window (file).
  (BUG) For some reason, the new window doesn't echo all messages
  (like "Cancelled." from ^G) like the parent window does.
*/

  char temp[MAXPATHLEN];
  struct buffer *buffer;
  struct view *view;
  struct im *im;
  char *filename;
  struct link *b = (struct link *)linkview_GetDataObject(self);

  if (!(filename=link_GetLink(b))) {
      message_DisplayString(self, 10, "No link");
      return;
  }

  if ((buffer = buffer_FindBufferByFile(filename)) == NULL) {
    /* don't have the file in memory, get it */
    if ((buffer = buffer_GetBufferOnFile(filename, 0)) == NULL) {
      sprintf(temp, "Couldn't get file %s.", filename);
      message_DisplayString(self, 50, temp);
      return;
    }
  }
  /* have the file in buffer buffer */

  /* get a view on the buffer */
  view = FindViewofBuffer(buffer);

  if ((im = view_GetIM(view)) == NULL) {
    message_DisplayString(self, 50, "Couldn't find window.");
    return;
  }

  view_WantInputFocus(view,view);
  /* pop to top window */
  im_ExposeWindow(im);
  /* warp cursor there */
  im_SetWMFocus(im);
}


static char *
FileName(path)
char *path;
{
/*
  Returns the filename portion of path (i.e.: strips leading
  directories and any extensions--unless the filename begins
  with a '.', in which case it strips the extensions after the 
  first '.' [so: ".cshrc" and ".cshrc.BAK" maps to ".cshrc"]);
  Warning:  destructively modifies the string path!
*/
  char *r, *s;

  if ((s = rindex(path, '/'))==NULL) {
    s = path;
  } else {
    ++s;
  }
  if ((r = index(s+1, '.'))!=NULL) *r = '\0';

  return(s);
}


static void
TargetProc(v, param)
struct view *v;
long param;
{
/*
  First, checks to see if there is a link object waiting for an
  autolink.  Then, if checks to make sure this frame has a buffer,
  and that the buffer has a filename.
*/
  struct buffer *fb;
  char *fn, name[255];
  struct link *l;

  if (!autolink_source) {
    message_DisplayString(v, 50, "Please set autolink source first.");
    return;
  }

  if (!class_IsTypeByName(class_GetTypeName(v),"frame")) {
    message_DisplayString(v, 10, "Autolink works on frames only");
    return;
  }
  if ((fb = (struct buffer *)frame_GetBuffer(((struct frame *)v)))==NULL) {
    message_DisplayString(v, 10, "Autolink works with buffers only");
    return;
  }

  if ((fn = buffer_GetFilename((struct buffer *)fb))==NULL) {
    message_DisplayString(v, 50, "Can only autolink to buffers on files.");
  } else {
    strcpy(name, fn);
    l = (struct link *)linkview_GetDataObject(autolink_source);
    link_SetLink(l, name);
    if (!link_LabelSetP(l)) {
      link_SetText(l,FileName(name));
      linkview_WantNewSize(autolink_source,autolink_source);
      linkview_WantUpdate(autolink_source, autolink_source);
      message_DisplayString(autolink_source, 10, "Changed link target (and label).");
    } else {
      message_DisplayString(autolink_source, 10, "Changed link target.");
    } /* if (!link_LabelSetP(...)) */
    link_NotifyObservers(l, observable_OBJECTCHANGED);
  } /* if (!(fn = ...)) */
  autolink_source = NULL;

  return;
}


static void
AutolinkProc(self, param)
struct linkview *self;
long param;
{
/*
  Start the autolink process.  Check to make sure we're not trouncing
  on another autolink first though....
*/
  static char *conflict[] = {
    "Use new autolink source",
    "Use old autolink source",
    "Cancel autolink",
    NULL
  };
  int answer;

  if (autolink_source) {
    if (message_MultipleChoiceQuestion(self,99,"Already autolinking!", 2, &answer, conflict, NULL)>= 0) {
      switch(answer) {
      case 0:
	message_DisplayString(self, 10, "Please indicate autolink target buffer.");
	message_DisplayString(autolink_source, 10, "Cancelled.  Using new source.");
	autolink_source = self;
	return;
      case 1:
	message_DisplayString(self, 10, "Cancelled.  Using old source");
	message_DisplayString(autolink_source, 10, "Please indicate autolink target buffer with the linkview-set-target proc.");
	return;
      case 2:
	message_DisplayString(self, 10, "Autolink cancelled.");
	message_DisplayString(autolink_source, 10, "Autolink cancelled.");
	autolink_source = NULL;
	return;
      }
    } else {
	message_DisplayString(self, 10, "Autolink cancelled.");
	message_DisplayString(autolink_source, 10, "Autolink cancelled.");
	autolink_source = NULL;
	return;
    }
  }
  message_DisplayString(self, 10, "Please indicate autolink target buffer.");
  autolink_source = self;

  return;
}

