/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/


#ifndef mkstylediffIncluded
#define mkstylediffIncluded

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkstylediff.h,v 1.2 89/09/08 17:29:51 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkstylediff.h,v $ */


/*
  $Log:	mkstylediff.h,v $
 * Revision 1.2  89/09/08  17:29:51  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:22:25  mss
 * Initial revision
 * 
 * Revision 1.1  88/10/17  12:03:06  mss
 * Initial revision
 * 

*/


/* Forward declarations */
INT_type CalcAuxiliaryLMandIndent(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcAuxiliaryRM(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcAuxiliaryPage(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcAuxiliaryJust(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcAuxiliaryLSpacing(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcAuxiliaryPSpread(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcAuxiliaryFont(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcAuxiliaryTabs(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcAuxiliaryMisc(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcAuxiliaryCounter(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcAuxiliaryInformation(/*gD, curNode, curStyle, styleSV, actualSV*/);


INT_type CalcAuxiliaryAttributes(/*gD, newStyleBlk, curStyle*/);

#endif
