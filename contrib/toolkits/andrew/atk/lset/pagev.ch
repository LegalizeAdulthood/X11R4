/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/pagev.ch,v 1.1 89/06/21 20:36:23 tpn Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/pagev.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_pagev_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/pagev.ch,v 1.1 89/06/21 20:36:23 tpn Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

struct pagev_switcheroo {
    struct page_switchee *switchee;
    struct view *v;
    struct pagev_switcheroo *next;
};

class pagev:view {
    classprocedures:
      InitializeClass() returns boolean;
      InitializeObject(struct pagev *self) returns boolean;
      FinalizeObject(struct pagev *self);
    overrides:
      FullUpdate(enum view_UpdateType type, long left,
		  long top, long width, long height);
      Update();
      Hit (enum view_MouseAction action, long x, long y,
	    long numberOfClicks) returns struct view *;
      PostKeyState(struct keystate *keystate);
      PostMenus(struct menulist *ml);
      LinkTree(struct view *parent);
      WantInputFocus(struct view *v);
      InitChildren() ;
    DesiredSize(long width, long height, enum view_DSpass pass, long *dWidth, long *dheight) returns enum view_DSattributes;
    Print(FILE *file, char *processor, char *finalFormat, boolean topLevel);
    ObservedChanged (struct observable *changed, long value);

    data:
      struct keystate *ks;
      struct menulist *ml;
      struct pagev_switcheroo *Firstswitcheroo, *NowPlaying;
};
