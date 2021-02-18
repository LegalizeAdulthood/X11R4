/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/txtstvec.h,v 2.4 89/02/17 17:33:35 ghoti Exp $ */
/* $ACIS:txtstvec.h 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/txtstvec.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidtextstatevector = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/txtstvec.h,v 2.4 89/02/17 17:33:35 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#ifndef TEXTSTATEVECTOR_DEFINED
#define TEXTSTATEVECTOR_DEFINED


#include <style.ih>

struct text_statevector {
    long CurLeftMargin;
    long CurRightMargin;
    long CurRightEdge;
    long CurLeftEdge;
    long CurTopMargin;
    long CurBottomMargin;
    long CurFontAttributes;
    long CurScriptMovement;
    long CurFontSize;
    long CurIndentation;
    enum style_Justification CurJustification;
    long CurSpacing;
    long CurSpread;
    long SpecialFlags;
    struct fontdesc *CurCachedFont;
    char *CurFontFamily;
/* 
    struct dfamily * CurFontFamily;
    struct dfont * CurCachedFont;
 */
    long * CurTabStops;			/* Pointer to array of tab stops in points */
    long * CurTabTypes;			/* Pointer to array of tab descriptors  (see below)*/
    long CurNumTabs;			/* Number of entries in the tab arrays */
    struct view * CurView;		/* Pointer to view wrapped in the environment */
};

/* Comment on CurTabTypes. The types are the same values as in the enumeration type for tabentry except when a character-around justifictain is used. In that case, the character to be used is the enumeration value + 128*character to use. Thus, to get the actual value, one should mask off the CurTabType with 0x177 */

#endif /* TEXTSTATEVECTOR_DEFINED */
