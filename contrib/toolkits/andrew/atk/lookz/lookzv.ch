/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lookz/RCS/lookzv.ch,v 2.5 89/02/23 09:58:07 ghoti Exp $ */
/* $ACIS:lookzv.ch 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lookz/RCS/lookzv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidlookzview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lookz/RCS/lookzv.ch,v 2.5 89/02/23 09:58:07 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
lookzview.H

	Class definitions for lookz view
*/


#define  lookzview_PROGRAMMERVERSION    1

class lookzview[lookzv] : view {
overrides:

	FullUpdate(/* struct lookzview *self, */ enum view_UpdateType type, 
		long left, long top, long width, long height);
	Update(/* struct lookzview *self */ );
	Hit(/* struct lookzview *self, */ enum view_MouseAction action, long x, long y, long n)
			returns struct view *;
	DesiredSize(/* struct lookzview *self, */ long width, long height, enum view_DSpass pass, 
				long *desiredWidth, long *desiredHeight) 
			returns enum view_DSattributes;
	WantInputFocus(/* struct lookzview *self, */ struct view *child);
	ReceiveInputFocus(/* struct lookzview *self */);
	LoseInputFocus(/* struct lookzview *self */);
	Print(/* struct lookzview *self, */ FILE *file, char *processor, char *finalFormat, boolean topLevel);
	ObservedChanged(/* struct lookzview *self, */ struct stylesheet *dobj, long status);
	GetApplicationLayer(/* struct lookzview *self */) returns struct lookzview *;
	LinkTree(/* struct lookzview *self, */ struct view *parent);

methods:

	SetVisibility( /* struct lookzview *self, */ boolean visibility );
	GetVisibility( /* struct lookzview *self */ ) returns boolean;
	SetStyleSheet( /* struct lookzview *self, */ struct stylesheet *ss );
	GetStyleSheet( /* struct lookzview *self */ ) returns struct stylesheet *;

classprocedures:

	InitializeClass(/* struct classhdr *ClassID*/) returns boolean;
			/* Create default Keymap & Menus */
	InitializeObject(/* struct classhdr *ClassID;*/ struct lookzview *self) returns boolean;
	FinalizeObject(/* struct classhdr *ClassID;*/ struct lookzview *self);

data:

	boolean embedded;
	struct stringtbl *(st[8]);
	struct strtblview *(stv[8]);
	struct lprruler *ruler;
	struct lprrulerview *rulerview;
	struct lpair *image;
	struct labelview *shrinkicon;
	struct lpair *shrinkparent;
	boolean Linked;

	boolean foundstylesheet;
	struct stylesheet *curss;		/* the stylesheet being edited */
	char *curcard;			/* points to the stringtbl name entry for
							current menucard */
	struct style *curstyle;		/* the style named by menucard/stylename */

	struct menulist *MyMenus;	/* copy of the class menus in MenuList */
	struct keystate *Keystate;		/* retain state for multi-keystroke inputs */
	boolean HasInputFocus;
	boolean NeedsUnpack;	/* specifically for change to stylesheet (via another lookz) */
	boolean OnScreen;
};
