/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/breakup.h,v 1.2 89/09/08 17:25:48 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/breakup.h,v $ */

/*
  $Log:	breakup.h,v $
 * Revision 1.2  89/09/08  17:25:48  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:22:07  mss
 * Initial revision
 * 
 * Revision 1.3  88/11/04  17:02:32  mss
 * Track paragraph state (including para breaks in
 * the midst of styles)
 * 
 * Revision 1.2  88/10/25  15:34:48  mss
 * Added new entry points
 * 
 * Revision 1.1  88/09/09  16:45:33  mss
 * Initial revision
 * 


*/

/* Include file for breakup.c: breaking atk into nbs pieces support */

extern CONSTITUENT MakePassage(/*gD, curStyle, IsGeneric*/);
extern CONSTITUENT MakePhrase(/*gD, curStyle, IsGeneric*/);
extern INT_type ProcessStyleEntry(/*gD, curTextView, curEnv*/);
extern INT_type ProcessStyleExit(/*gD, curTextView, curEnv*/);
extern INT_type ProcessTextRun(/*gD, textdo, start, finish*/);
extern p_font_list_blk MakeNewFontFromSV(/*gD,SV*/);
extern BOOL_type IsViewAtPos(/*gD,textdo, pos*/);

#define MagicViewChar '\377'
