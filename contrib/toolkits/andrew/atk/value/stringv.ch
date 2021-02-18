/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/stringv.ch,v 2.6 89/02/23 10:23:28 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/stringv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_stringV_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/stringv.ch,v 2.6 89/02/23 10:23:28 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#include <fontdesc.ih>
#include <rect.h>

class stringV[stringv] : valueview[valuev] {
 overrides:
  DrawFromScratch(long x,y,width,height);
  DrawHighlight();
  DrawDehighlight();
  LookupParameters();
  DrawNewValue();
 classprocedures:
  InitializeClass() returns boolean;
 macromethods:
   GetUseAlt()(self->UseAlt)
   SetUseAlt(boolean val) (self->UseAlt = val)
 data:
  char * label;
  char * fontname;
  short fontsize;
  struct fontdesc * normalfont, * boldfont, * activefont;
  struct rectangle labelrec;
  long x,y,width,height;
  boolean UseAlt;
  char *plabel;
};

/* parameters to spst
   label		string		Defaults to NULL
   bodyfont		string		Defaults to "andytype"
   bodyfont-size	long		Defaults to 10
*/
