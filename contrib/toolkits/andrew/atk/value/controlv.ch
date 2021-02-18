/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/controlv.ch,v 2.7 89/04/19 09:47:48 tpn Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/controlv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_controlV_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/controlv.ch,v 2.7 89/04/19 09:47:48 tpn Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


class controlV[controlv] : buttonV[buttonv]{
    overrides:
      DoHit(enum view_MouseAction type, long x,y,hits) returns struct controlV *;
      LookupParameters();
      DrawFromScratch(long x,y,width,height);
    classprocedures:
      InitializeClass() returns boolean;
      InhibitAutoInit();
      SetAutoInit(boolean val);
    data:
      char *cclass;
      char *function;
      boolean autoinit,needsinit;
};

