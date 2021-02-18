/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/getcompsv.h,v 1.2 89/09/08 17:27:02 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/getcompsv.h,v $ */

/*
  $Log:	getcompsv.h,v $
 * Revision 1.2  89/09/08  17:27:02  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:16:20  mss
 * Initial revision
 * 
 * Revision 1.1  88/11/18  15:53:26  mss
 * Initial revision
 * 


*/

/* Include file for getcompsv.c */

/* Global state */

extern struct text_statevector * GetCompensatedSV(/* gD, specConstituent, curSV, styleToUse, compensatingStyle*/);
extern INT_type DoStyleApplication(/*gD, contentStart, contentLength, styleToUse*/);
extern INT_type FinishStyleApplications(/*gD*/);
