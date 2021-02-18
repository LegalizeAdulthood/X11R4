/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/getstyles.h,v 1.2 89/09/08 17:27:54 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/getstyles.h,v $ */

/*
  $Log:	getstyles.h,v $
 * Revision 1.2  89/09/08  17:27:54  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:15:43  mss
 * Initial revision
 * 
 * Revision 1.4  88/11/17  17:41:03  mss
 * Added state for seeing if style was ever applied
 * (in case style changing was required)
 * 
 * Revision 1.3  88/10/31  21:31:22  mss
 * Added more style function (fonts/margins)
 * 
 * Revision 1.2  88/09/22  11:14:22  mss
 * Added global state, rearrange style tables
 * 
 * Revision 1.1  88/09/07  11:09:32  mss
 * Initial revision
 * 


*/

/* Include file for getstyles.c */


typedef struct style_elt_struct {
    struct style_elt_struct *next;
    struct style	    *ATKStyle;
    char		    *styleName;
    struct text_statevector SV;
    CONSTITUENT		    presentationStyle;
    CONSTITUENT		    layoutStyle;
    at_NEW_LAYOUT_OBJECT_type *NLO;
    CONSTITUENT		    genericPage;
    BOOL_type		    styleEverApplied;
} style_elt, * p_style_elt;

/* Global state */

extern struct style *globalStyle;	    /* The global ATK style for document */
extern struct text_statevector *globalFSV;  /* The global FSV corresponding to the global style */
extern p_style_elt style_list;		    /* List of styles from ODA generics */

extern INT_type GetGlobalStyle(/*gd*/);
extern INT_type GetGenericStyles(/*gd*/);
extern struct style * FindATKStyleFromConst(/*node*/);
extern INT_type GetDenseDotsFromSMUs(/*gD, SizeInSMUs, DotDensityPerInch*/);
extern INT_type SetJustification(/*ATKStyle, alignmentValue*/);
