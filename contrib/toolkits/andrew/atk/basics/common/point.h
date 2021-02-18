/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/point.h,v 2.3 89/02/17 18:39:43 ghoti Exp $ */
/* $ACIS:point.h 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/point.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidpoint = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/point.h,v 2.3 89/02/17 18:39:43 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */



#ifndef POINT_DEFINED

#define POINT_DEFINED

#include <class.h>

struct point {
    long y,x;
};

#define point_X(thePoint) ((thePoint)->x)
#define point_Y(thePoint) ((thePoint)->y)
/* original unmacroified versions */
/*
long point_X();
long point_Y();
*/
void point_SetX(/*Pt,Value*/);
void point_SetY(/*Pt,Value*/);

void point_OffsetPoint(/*Pt,DeltaX, DeltaY*/);
void point_AddPt(/*LHS,RHS*/);
void point_SubPt(/*LHS,RHS*/);
/*void point_SetPt(LHS, NewX, NewY); */
#define point_SetPt(PtToSet,NewX, NewY) \
(    ((PtToSet)->y = (NewY)), \
     ((PtToSet)->x = (NewX)) )

boolean point_ArePtsEqual(/*LHS,RHS*/);
struct point * point_CreatePoint(/*InitX,InitY*/);
#endif /* POINT_DEFINED */
