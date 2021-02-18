/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lookz/RCS/lookz.ch,v 2.6 89/04/04 19:53:03 wjh Exp $ */
/* $ACIS:lookz.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lookz/RCS/lookz.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidlookz_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lookz/RCS/lookz.ch,v 2.6 89/04/04 19:53:03 wjh Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
lookz.H

	lookz data object

	This inset provides for editing the styles of its parent text

	The file format for a lookz has a single line between
	\begindata and \enddata.  The only text on this line is one of the 
	words "visible" or "hidden", determining whether the 
	style editor is exposed or hidden in the text.

*/

#define  lookz_PROGRAMMERVERSION    1

/* status values for NotifyObservers */
#define lookz_VISIBILITYCHANGED 1

class lookz : dataobject[dataobj]  {

overrides:

	Read( /* struct lookz *self, */ FILE *file, long id ) returns long;
	Write( /* struct lookz *self, */ FILE *file, long id, long level )
			returns long;

methods:
	
	SetVisibility( /* struct lookz *self, */ boolean visible ); 

macromethods:
	GetVisibility( /* struct lookz *self */ ) (((struct lookz *)self)->visible + 0)

classprocedures:

	InitializeObject(/* struct classhdr *ClassID;*/ struct lookz *self) returns boolean;
	FinalizeObject(/* struct classhdr *ClassID;*/ struct lookz *self);

data:

	boolean visible;		/* if FALSE, only an icon is displayed
						if TRUE, the editor is displayed 
						this field is constrained to be 
						TRUE or FALSE  */

};

