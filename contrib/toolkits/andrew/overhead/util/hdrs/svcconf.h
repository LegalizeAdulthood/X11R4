/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/hdrs/RCS/svcconf.h,v 2.8 89/04/24 15:34:03 cfe Exp $ */
/* $ACIS: $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/hdrs/RCS/svcconf.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_h = "$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/hdrs/RCS/svcconf.h,v 2.8 89/04/24 15:34:03 cfe Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* ************************************************************ *\
	svcconf.h
	Definitions for cell-based services.
\* ************************************************************ */

extern char *ThisDomain;
extern int ThisDomainLen;
extern char *ViceFile;

extern int AMS_ViceIsRunning;

extern int AMS_DeliverySystem;

extern int AMS_UseWP;

extern int AMS_OnAIX;
extern int AMS_LocalMailSystemExists, AMS_DefaultToAFSCellMail, AMS_ThisDomainAuthFromWS;

extern char *CellCommonPrefix, *CellCommonRWSuffix, *CellCommonSuffix,
	*CellCommonWPDirSuffix;
extern char *UserPrimaryCell, *WorkstationCell, *WorkstationName;
