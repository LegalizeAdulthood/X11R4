/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/buttonv.ch,v 2.7 89/02/23 10:22:25 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/buttonv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_buttonV_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/buttonv.ch,v 2.7 89/02/23 10:22:25 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#include <fontdesc.ih>
#define buttonV_NONE 0
#define buttonV_SURROUND 1
#define buttonV_ADJACENT 2
#define buttonV_EITHER 3
class buttonV[buttonv] : valueview[valuev]
{
 overrides:
  DrawFromScratch(long x,y,width,height);
  DrawHighlight();
  DrawDehighlight();
  DrawNewValue();
  DoHit(enum view_MouseAction type, long x,y,hits)
    returns struct buttonV *;
  LookupParameters();
 classprocedures:
  InitializeClass() returns boolean;
macromethods:
  SetButtonType(TYPE) (self->buttontype = TYPE)
  GetButtonType() (self->buttontype)
 data:
  char * label;
  char * fontname;
  short fontsize;
  short buttontype;
  struct fontdesc * normalfont, * boldfont, * activefont;
};

/* parameters to spst
   label		string		Defaults to NULL
   bodyfont		string		Defaults to "andytype"
   bodyfont-size	long		Defaults to 10
*/
