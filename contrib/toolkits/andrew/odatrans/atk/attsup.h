/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/attsup.h,v 1.2 89/09/08 17:25:21 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/attsup.h,v $ */

/*
  $Log:	attsup.h,v $
 * Revision 1.2  89/09/08  17:25:21  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:15:56  mss
 * Initial revision
 * 
 * Revision 1.2  89/05/20  20:38:13  mss
 * Remove routines for setting pres/lay styles
 * 
 * Revision 1.1  88/09/07  11:08:19  mss
 * Initial revision
 * 


*/

/* Include file for attsup,c: attribute support */


#define DoAsString  (BOOL_true)
#define	DoAsBytes   (BOOL_false)
/* I hope this is a safe value */
#define NoSuchAttr ((INT_type) -23)

extern INT_type SafeSetAttr(/*constituent, attribute, value, mask*/);
extern INT_type SafeSetValueAttr(/*constituent, attribute, value*/);
extern INT_type AddStringAttribute(/*CurCons, theAttr, textData, AsString*/);
extern INT_type AppendSubordinate(/*Parent,Child*/);
extern INT_type AppendCharContent(/*CurBLO, CurCP, textData*/);
extern INT_type SafeGetAttr(/*curConstituent, attribute, attributevalue, inherited, parmmask*/);
extern INT_type CheckAndGetAttr(/*constituent, attribute, value,  mask*/);
extern INT_type GetOnObjOrStyle(/*curConstituent, attribute, value, parmmask*/);
extern INT_type GetBLOFromDVL(/*node, retBLO*/);
