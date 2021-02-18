/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/messages.ch,v 2.6 89/02/20 13:16:19 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/messages.ch,v $ */

#ifndef lint
static char *rcsid_messages_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atkams/messages/lib/RCS/messages.ch,v 2.6 89/02/20 13:16:19 ghoti Exp $ ";
#endif /* lint */

#define WHATIAM_UNDEFINED -1
#define WHATIAM_FOLDERS 0
#define WHATIAM_CAPTIONS 1
#define WHATIAM_BODIES 3

class messages: textview[textv] {
    classprocedures:
      InitializeObject(struct messwind *self) returns boolean;
      InitializeClass() returns boolean;
      FinalizeObject(struct messwind *self);
    overrides:
      PostKeyState(struct keystate *keystate);
      PostMenus(struct menulist *ml);
    methods:
      SetWhatIAm(int WhatIAm);
      ResetFileIntoMenus();
      AppendOneMessageToFile(int cuid, char *fname) returns int;
    data:
      struct menulist *mymenulist, *fileintomenulist;
      struct keystate *mykeys;
      int WhatIAm;
};

