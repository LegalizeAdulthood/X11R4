/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/rofftext.h,v 2.3 89/02/17 17:05:58 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/rofftext.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_h = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/rofftext.h,v 2.3 89/02/17 17:05:58 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* type checking */
SetIndent(/*struct rofftext *self,int u*/);
SetTempIndent(/*struct rofftext *self,int u*/);
Is_BOL(/*struct rofftext *self*/);
SetBOL(/*struct rofftext *self*/);
DestroyContext(/*IC c*/);
Trickle topen(/*struct rofftext *self, FILE *f, char *s*/);
tclose(/*struct rofftext *self, Trickle t*/);
g(/*struct rofftext *self, Trickle t*/);
ung(/*struct rofftext *self,char c,Trickle t*/);
tpush(/*struct rofftext *self, Trickle t, FILE *f, char *s, boolean push, int argc, char *argv[]*/);
munch(/*struct rofftext *self, Trickle t*/);
special(/*struct rofftext *self,Trickle t*/);
setfont(/*struct rofftext *self, Trickle t*/);
getwidth(/*struct rofftext *self, Trickle t*/);
munchmove(/*struct rofftext *self, Trickle t*/);
getname(/*struct rofftext *self, Trickle t, char *name*/);
char *getregister(/*struct rofftext *self, Trickle t*/);
putregister(/*struct rofftext *self, char *name, double value, enum RegFmt fmt, double inc, boolean relative*/);
char *getstring(/*struct rofftext *self, char *name*/);
putstring(/*struct rofftext *self, char *name, char *value*/);
getarg(/*struct rofftext *self, Trickle t, char *buf, int n, boolean copymode*/);
put(/*struct rofftext *self, char c*/);
DoBreak(/*struct rofftext *self*/);
get(/*struct rofftext *self, Trickle t*/);
DoCommand(/*struct rofftext *self, Trickle t, char *name, boolean br*/);
Scan(/*struct rofftext *self, Trickle t, char *cmd*/);
