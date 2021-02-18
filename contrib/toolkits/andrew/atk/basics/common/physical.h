/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/physical.h,v 2.3 89/02/17 18:39:17 ghoti Exp $ */
/* $ACIS:physical.h 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/physical.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidphysical = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/physical.h,v 2.3 89/02/17 18:39:17 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* Include file for cooridnate conversions */

long physical_LogicalPtToGlobalX(/*CoordinateSystem,LogicalPoint*/);
long physical_LogicalPtToGlobalY(/*CoordinateSystem,LogicalPoint*/);
void physical_LogicalPtToGlobalPt(/*CoordinateSystem,tempPoint*/);
void physical_GlobalPtToLogicalPt(/*CoordinateSystem,tempPoint*/);
long physical_LogicalXToGlobalX(/*CoordinateSystem,LogicalX*/);
long physical_LogicalYToGlobalY(/*CoordinateSystem,LogicalY*/);
long physical_GlobalXToLogicalX(/*CoordinateSpace,PhysicalX*/);
long physical_GlobalYToLogicalY(/*CoordinateSpace,PhysicalY*/);
void physical_LogicalToGlobalRect(/*CoordinateSystem,TempRect*/);
void physical_GlobalToLogicalRect(/*CoordinateSystem,TempRect*/);
