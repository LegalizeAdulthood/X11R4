/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/


#ifndef mkgenstyleIncluded
#define mkgenstyleIncluded

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkgenstyle.h,v 1.2 89/09/08 17:29:04 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkgenstyle.h,v $ */


/*
  $Log:	mkgenstyle.h,v $
 * Revision 1.2  89/09/08  17:29:04  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:22:29  mss
 * Initial revision
 * 
 * Revision 1.1  88/10/17  12:01:15  mss
 * Initial revision
 * 

*/


/* Forward declarations */
INT_type CalcStyleAttrLMandIndent(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcStyleAttrRM(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcStyleAttrPage(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcStyleAttrJust(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcStyleAttrLSpacing(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcStyleAttrPSpread(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcStyleAttrFont(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcStyleAttrTabs(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcStyleAttrMisc(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcStyleAttrCounter(/*gD, curNode, curStyle, styleSV, actualSV*/);
INT_type CalcStyleAttrInformation(/*gD, curNode, curStyle, styleSV, actualSV*/);


INT_type CalcStyleAttributes(/*gD, newStyleBlk, curStyle*/);

#endif
