/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
#ifndef mkgenericIncluded
#define mkgenericIncluded

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkgeneric.h,v 1.2 89/09/08 17:28:36 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/mkgeneric.h,v $ */


/*
  $Log:	mkgeneric.h,v $
 * Revision 1.2  89/09/08  17:28:36  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:22:22  mss
 * Initial revision
 * 
 * Revision 1.1  88/09/09  16:45:09  mss
 * Initial revision
 * 


*/

extern INT_type MakeGenericParagraph(/*gD*/);
extern INT_type MakeGenericText(/*gD*/);
extern INT_type MakeGenericRaster(/*gD*/);
extern INT_type MakeGenericTable(/*gD*/);
extern INT_type MakeGenericEq(/*gD*/);
extern INT_type MakeGenericFigure(/*gD*/);

#endif
