/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/spreadoda.ch,v 1.2 89/09/08 17:33:10 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/spreadoda.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidspreadodaH = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/spreadoda.ch,v 1.2 89/09/08 17:33:10 mss Exp $";
#endif

#include "view.ih"


class spreadoda: describer[describe] {
overrides:
    Describe(struct view * viewToDescribe, char * format, FILE * file, p_odainfo_type rock) returns enum view_DescriberErrs;
};
