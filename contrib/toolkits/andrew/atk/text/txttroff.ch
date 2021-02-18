/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/txttroff.ch,v 2.5 89/02/23 09:21:44 ghoti Exp $ */
/* $ACIS:txttroff.ch 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/txttroff.ch,v $ */

#ifndef lint
static char *rcsidtexttroff_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/txttroff.ch,v 2.5 89/02/23 09:21:44 ghoti Exp $";
#endif /* lint */


package texttroff[txttroff] {
classprocedures:
    WriteTroff(struct view *view,struct dataobject *dd,FILE * f,int toplevel);

    /* BeginDoc / EndDoc
	These functions are called by standalone views which wish to use the
	troff environment as is established by text.
	They are called if the "toplevel" (fifth) parameter to xxx_Print is TRUE.
	The text generated before calling EndDoc should end with a newline;
	a .br should have been issued to send the last line to the output.
    */
    BeginDoc(FILE *f);
    EndDoc(FILE *f);


    /* BeginPS / EndPS
	These functions supply the transition for including a postscript rectangle
		in a troff page.  
	BeginPS sets up a postscript coordinate system for an image of size 
		(width x height) in postscript units (1/72 in.) 
		with the origin in the lower left corner.
	At the end of the postscript, the routine EndPS must be called.
	Each line between the two must begin with the two characters:  \!
		(backslash followed by exclamation mark)
*/
    BeginPS(FILE *f, long width, height);
    EndPS(FILE *f, long width, height);

};
