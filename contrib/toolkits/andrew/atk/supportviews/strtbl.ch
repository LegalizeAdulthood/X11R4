/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/supportviews/RCS/strtbl.ch,v 2.6 89/02/22 14:02:54 ghoti Exp $ */
/* $ACIS:strtbl.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/supportviews/RCS/strtbl.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidstringtbl_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/supportviews/RCS/strtbl.ch,v 2.6 89/02/22 14:02:54 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
stringtbl.H

String table data object

	This module supports storage for a set of string values.  (At most 32 strings)
	A bit vector associated with the set specifies which are "selected" (0, 1, or many)
	Strings are checked for backslash and brackets and they are deleted.
	Strings are truncated to 255 character.
	All operations are case sensitive.
	(AddString checks for length and backslash, but the others do not.  Thus
		a string may be added and then not found.)

	An "accession number" is associated with each string.  This will be a small integer;
	counting restarts each time the table is cleared.  The functions with OfEntry in their
	names have the accession number as argument.

	The character string arguments to the functions are tested for NULL or '\0' and
	ignored if so.   In this case, AddString returns 0;  it also returns zero if the table is full.

*/

#define  stringtbl_PROGRAMMERVERSION    1

/* XXX ??? I think there is as yet no way to say I want these only in .eh */
#define MAXSTRINGSINTABLE 32
#define MAXSTRINGENTRYLENGTH 255

#define stringtbl_STRINGSCHANGED 1
#define stringtbl_BITSCHANGED 2

class stringtbl[strtbl] : dataobject[dataobj] {

overrides:

	Read( /* struct stringtbl *self, */ FILE *file, long id ) returns long;
	Write( /* struct stringtbl *self, */ FILE *file, long id, long level )
	returns long;

	/* The file format for a stringtbl is the bitvector in hex, 
		a newline, 	and then a sequence of the strings, each 
		followed by a newline.
	*/
	ViewName() returns char *;
methods:

	Clear( /* struct stringtbl *self */ );		
		/* Clears the string table of existing strings */
	AddString( /* struct stringtbl *self, */ unsigned char *s) returns short;
		/* Adds a string to the set of strings in the table, 
			returns its accession number 
			returns -1 if table is full */
	RemoveString( /* struct stringtbl *self, */ unsigned char *s);
			/* Removes the specified string from the table.
			If the string is absent, the call is ignored */
	SetBit( /* struct stringtbl *self, */ unsigned char *s, boolean val);
		/* finds the named string in the table 
			and sets its associated bit to the given val */
	GetBit( /* struct stringtbl *self, */ unsigned char *s) returns boolean;
		/* finds the named string in the table and 
			returns the current value of its bit 
		   if s is absent, returns FALSE anyway */
	ClearBits( /* struct stringtbl *self */ );
		/* sets all bits to zero */
	RemoveEntry( /* struct stringtbl *self, */ short accnum );
		/* remove the entry with the given accnum 
			If the string is absent, the call is ignored */
	SetBitOfEntry( /* struct stringtbl *self, */  short accnum, boolean val );
		/* set the bit associated with the given accnum 
			If the string is absent, the call is ignored */
	GetBitOfEntry( /* struct stringtbl *self, */  short accnum ) returns boolean;
		/* return the bit for the given accnum 
			If the string is absent, returns FALSE */
	GetStringOfEntry( /* struct stringtbl *self, */  short accnum ) returns unsigned char *;
		/* return the string for the given accnum 
			If the string is absent, returns NULL */


macromethods:

	/* (the instances of " + 0 " prevent assignment to these fields) 
		It is a gross error to modify the string pointed at by IthString
		or to retain the pointer beyond the next AddString or RemoveString */

	NStrings( /* struct stringtbl *self */ ) ((short)(self->used + 0))
	IthString( /* struct stringtbl *self, */  short i )  ((unsigned char *)(self->item[i] + 0))
	NumAcc( /* struct stringtbl *self */ ) ((short)(self->numacc+0))

classprocedures:

	InitializeObject(/* struct classhdr *ClassID;*/ struct stringtbl *self) returns boolean;
	FinalizeObject(/* struct classhdr *ClassID;*/ struct stringtbl *self);

data:

	unsigned char *(item[MAXSTRINGSINTABLE]);	/* pointers to the strings */
	unsigned long highlight;		/* the bits.  bit (1<<i) is for item[i] */
	short used;		/* the first -used- of the elements in -item- have values */
	short numacc;		/* number of accessions */
	short accmap[32];	/* the accession number for each entry */
	boolean ContainsInitialStrings;	/* TRUE until client first changes table */
};

