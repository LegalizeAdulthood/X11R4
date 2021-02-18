/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/fourwayv.ch,v 2.7 89/08/03 11:54:13 tpn Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/fourwayv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_fourwayV_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/fourwayv.ch,v 2.7 89/08/03 11:54:13 tpn Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#include <fontdesc.ih>
#include <rect.h>
#define fourwayv_NUMLABELS 4
class fourwayV[fourwayv] : valueview[valuev]
{
 overrides:
  DrawFromScratch(long x,y,width,height);
  DrawHighlight();
  DrawDehighlight();
  DrawNewValue();
  DoHit(enum view_MouseAction type, long x,y,hits)
    returns struct fourwayV  *;
  LookupParameters();
macromethods:
  GetTmpVal() (self->tmpval)
classprocedures:
  InitializeClass() returns boolean;
data:
  char *label[fourwayv_NUMLABELS];
  int lsize[fourwayv_NUMLABELS];
  char *fontname;
  short fontsize;
  struct fontdesc * normalfont, * boldfont, * activefont, *valuefont;
  long  tmpval;
  long x,y,width,height;
};

/* parameters to spst
   label		string		Defaults to NULL
   bodyfont		string		Defaults to "andytype"
   bodyfont-size	long		Defaults to 10
   max-value		long		defaults to 100
  min-value		long		dafaults to 0
  increment		float		defaults to 1.0
*/
