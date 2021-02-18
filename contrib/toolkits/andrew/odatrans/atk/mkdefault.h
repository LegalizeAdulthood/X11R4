/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/


#ifndef mkdefaultIncluded
#define mkdefaultIncluded

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkdefault.h,v 1.2 89/09/08 17:28:14 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkdefault.h,v $ */


/*
  $Log:	mkdefault.h,v $
 * Revision 1.2  89/09/08  17:28:14  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:22:32  mss
 * Initial revision
 * 
 * Revision 1.1  88/10/26  09:17:23  mss
 * Initial revision
 * 
 * Revision 1.1  88/10/17  12:01:15  mss
 * Initial revision
 * 

*/


/* External entry points */
INT_type MakeDeafultState(/*gD, styleBlk*/);

/* Forward declarations */
INT_type CreateAuxiliaryMargins(/*gD, actualSV, styleBlk*/);
INT_type CreteStyleAttrLMandIndent(/*gD*/);
INT_type CreteStyleAttrRM(/*gD*/);
INT_type CreteStyleAttrPage(/*gD*/);
INT_type CreteStyleAttrJust(/*gD*/);
INT_type CreteStyleAttrLSpacing(/*gD*/);
INT_type CreteStyleAttrPSpread(/*gD*/);
INT_type CreteStyleAttrFont(/*gD*/);
INT_type CreteStyleAttrTabs(/*gD*/);
INT_type CreteStyleAttrMisc(/*gD*/);
INT_type CreteStyleAttrCounter(/*gD*/);
INT_type CreteStyleAttrInformation(/*gD*/);


#endif
