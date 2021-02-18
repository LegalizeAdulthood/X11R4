/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/fonts/lib/RCS/fntmanip.h,v 2.4 89/02/16 12:25:10 ghoti Exp $ */
/* $ACIS:fntmanip.h 1.2$ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/fonts/lib/RCS/fntmanip.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidfontmanip = "$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/fonts/lib/RCS/fntmanip.h,v 2.4 89/02/16 12:25:10 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* ************************************************************ *\

	fntmanip.h		


\* ************************************************************ */

#define MAXICONS 128
struct IconGenericPart  generic[MAXICONS];
struct BitmapIconSpecificPart   specific[MAXICONS];
unsigned short *bits[MAXICONS];
struct font fonthead;

#define MAXCOMMENTS 200
char *comments[MAXCOMMENTS];
int ncomments;
