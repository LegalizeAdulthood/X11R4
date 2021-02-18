/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: nullv.ch,v 1.2 89/12/05 11:33:54 xguest Exp $ */
/* $ACIS:$ */
/* $Source: /xsrc/contrib/toolkits/andrew/atk/createinset/null/RCS/nullv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
	char *nullv_ch_rcsid = "$Header: nullv.ch,v 1.2 89/12/05 11:33:54 xguest Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
nullv.H

	Class definitions for null inset view

*/
/*
 *    $Log:	nullv.ch,v $
Revision 1.2  89/12/05  11:33:54  xguest
change instances of nullv to nullview

Revision 1.1  89/12/04  20:27:58  xguest
Initial revision

Revision 1.1  89/07/31  15:35:27  wjh
Initial revision


Revision 1.0  88/05/14  15:40:34  wjh
Copied from /usr/andrew/lib/genericinset
 */


class nullview [nullv] : view
{
overrides:

	FullUpdate(/* struct nullview *self, */ enum view_UpdateType type, 
			long left, long top, long width, long height);
	Update(/* struct nullview *self, */);
	Hit(/* struct nullview *self, */ enum view_MouseAction action, 
			long x, long y, long n)	returns struct view *;
	DesiredSize(/* struct nullview *self, */ long width, long height, 
			enum view_DSpass pass, long *desiredWidth, long *desiredHeight) 
			returns enum view_DSattributes;
	ReceiveInputFocus(/* struct nullview *self */);
	LoseInputFocus(/* struct nullview *self */);
	ObservedChanged(/* struct nullview *self, */ struct null *dobj, long status);
	Print(/* struct nullview *self, */ FILE *file, 
			char *processor, char *finalFormat, boolean topLevel);
	GetApplicationLayer(/* struct nullview *self */) returns struct nullview *;
	

methods:

	/* $$$ methods unique to this view.
		These would be defined if a parent view were to be given 
		greater control over this view. */

classprocedures:

	InitializeClass() returns boolean; 	/* Create default Keymap & Menus */
	InitializeObject(struct nullview *self) returns boolean;
	FinalizeObject(struct nullview *self);

data:
	struct menulist  *Menus;
	struct keystate *Keystate;

	struct graphic  *BlackPattern;	/* these might differ between instances . . . */
	struct graphic  *WhitePattern;	/*		. . . on different displays */

	boolean OnScreen;			/* if not view_Removed */
	boolean embedded;		/* TRUE if no call to GetApplicationLayer */
	boolean HasInputFocus;		/* T if received input focus */
	boolean ignoreUp;			/* T iff have just asked for InputFocus */
	boolean sizeknown;		/* T if haven't called ComputeArea */

	/* $$$ some facts basic to computing desired size
		null chooses to have a given area if the parent
		constrains height or width */
	long DesiredArea;			/* total space desirable in square pixels */

	/* $$$ save desired size if the computation of it was expensive */
	long DesiredHeight, DesiredWidth;	/* requested dimensions; set by 
						ComputeArea*/
};
