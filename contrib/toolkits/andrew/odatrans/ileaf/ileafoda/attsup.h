/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/ileaf/ileafoda/RCS/attsup.h,v 1.2 89/09/08 17:20:04 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/ileaf/ileafoda/RCS/attsup.h,v $ */


/*
 *$Log:	attsup.h,v $
 * Revision 1.2  89/09/08  17:20:04  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/09/05  21:58:25  mss
 * Initial revision
 * 
 * Revision 1.3  89/07/12  12:40:18  jaap
 * margins, and justification in place (sort of)
 * 
 * Revision 1.2  89/07/10  12:21:02  jaap
 * First shot at doing styles
 * 
 * Revision 1.1  89/06/28  16:05:38  jaap
 * Initial revision
 * 
 */
 
/*
 * shamelessly stolen from mss
 */


#ifndef attsupIncluded
#define attsupIncluded

#define DoAsString  (BOOL_true)
#define	DoAsBytes   (BOOL_false)

extern INT_type SafeSetAttr(/*constituent, attribute, value, mask*/);
extern INT_type SafeSetIntAttr(/*constituent, attribute, value*/);
extern INT_type SafeSetValueAttr(/*constituent, attribute, value*/);
extern INT_type AddStringAttribute(/*CurCons, theAttr, textData, AsString*/);
extern INT_type AppendSubordinate(/*Parent,Child*/);
extern INT_type AppendGenForSub(/*Parent, Child*/);
extern INT_type AppendCharContent(/*CurBLO, CurCP, textData*/);
extern INT_type SafeGetAttr(/*curConstituent, attribute, attributevalue, inherited, parmmask*/);
extern char * ErrorMsg(/*errorNumber*/);
extern CONSTITUENT SafeMakeStyle(/*Doc, kind*/);

#define INHERIT	BOOL_true
#define NOINHERIT	BOOL_false

#endif
