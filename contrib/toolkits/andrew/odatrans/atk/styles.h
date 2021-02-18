/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/

#ifndef stylesIncluded
#define stylesIncluded

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/styles.h,v 1.2 89/09/08 17:33:37 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/styles.h,v $ */


/*
  $Log:	styles.h,v $
 * Revision 1.2  89/09/08  17:33:37  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:22:17  mss
 * Initial revision
 * 
 * Revision 1.4  89/04/30  12:12:39  mss
 * Changed names to reflect new atk naming scheme
 * 
 * Revision 1.3  88/10/10  18:53:27  mss
 * revised tracing
 * 
 * Revision 1.2  88/09/09  16:43:41  mss
 * Added style creation for atk styles
 * 
 * Revision 1.1  88/09/07  11:19:30  mss
 * Initial revision
 * 
 * Revision 1.2  88/08/24  10:40:48  mss
 * COntains flags to ensure only only inclusion
 * 
 * Revision 1.1  88/08/10  09:54:39  mss
 * Initial revision
 * 

*/

/* This module is used for manipulating abstract stylesheet information between a native system and ODA */

/* A style is represented in ODA by having various pieces of information
   stored on the ODA tree. Some attributes apply to BLOs and are stored
   in layout and presentation styles referenced in a default value list
   in the CLO to which the style was applied. The layoutsSTyle and
   presStyle fields are used to hold those styles. Some attributes are
   held on a page (and its suborindates). For example, top and bottom
   margins are stored as position and dimension attributes in the header, body
   and footer frames, which are headerFrame, bodyFrame and footerFrame
   respectively. Headers and footers are logically sourced in the hdrFrame
   and ftrSourceFrame respectively. When using a style sheet, one can just place the styleList onto the desired generic. Finally, the structure can be tagged to ensure that the correct page is being used for formatting: a use of a style places the appropriate newLayObjStyle on the generic CLO and places the appropriate textLayCatStyle or fnoteLayCatStyle on the specific BLOs */

#include <txtstvec.h>
#include <style.ih>
#include <envrment.ih>
#include <odatk.h>

typedef struct styleSheetInfo_struct {
    /* Overhead for maintaining list information */
    struct styleSheetInfo_struct *  next;   /* For chaining style sheet blocks */

    /* Host specific information */
    char *	    name;		/* Name of style sheet in native system */
    CONSTITUENT	    passage;		/* "generic" passage for this style */
    CONSTITUENT	    phrase;		/* "generic" phrase for this style */
    struct style    *ATKStyle;		/* Andrew style being translated */
    struct text_statevector SV;		/* State vector associated with this style */

    /* ODA pieces where information held */
    CONSTITUENT	    layoutStyle;	/* Layout style associated with this style sheet */
    CONSTITUENT	    presStyle;		/* Presentation style associated with this style sheet */
    CONSTITUENT	    headerFrame;	/* Frame for page header */
    CONSTITUENT	    hdrSourceFrame;	/* Frame inside of page header (for sourcing headers) */
    CONSTITUENT	    bodyFrame;		/* Frame holding body area (text/fnotes) */
    CONSTITUENT	    footerFrame;	/* Frame for page footer */
    CONSTITUENT	    ftrSourceFrame;	/* Frame inside of page footer (for sourcing footers) */
    
    /* derived ODA information for use as needed */
    at_DEFAULT_VALUE_LISTS_type styleList;	/* A default value list that can be used on CLOs */
    char *	    textCategory;	/* String describing text category */
    CONSTITUENT	    textLayCatStyle;	/* Style with layout category for body text BLOs */
    char *	    fnoteCategory;	/* String describing fnote category */
    CONSTITUENT	    fnoteLayCatStyle;	/* Style with layout category for fnote text BLOs */
    at_NEW_LAYOUT_OBJECT_type    newLayObject;   /* Attribute specifying generic page object */
    CONSTITUENT	    newLayObjStyle;	/* Style with new layout object attribute */

    /* Intermediate pieces created, not normally manipulated directly, but kept for debugging */
    CONSTITUENT	    fakeBLO;		/* Fake BLO used to hold styles in DVL */
    CONSTITUENT	    curPage;		/* Page used to hold formatting information */
    DOCUMENT_type   curDoc;		/* which document this stuff lives in */

} styleSheetInfo, * p_styleSheetInfo;


/* This structure is used to link together documents and page sets */

typedef struct docPageSet_struct {
    struct docPageSet_struct * next;
    DOCUMENT_type   doc;
    CONSTITUENT	    curPageSet;
} docPageSet, *p_docPageSet;

extern p_styleSheetInfo	styleSheetList;			/* This is where all of the style sheets are linked together */
extern p_docPageSet pageSetList;			/* This is where all doc/pageset pairs are recorded */
extern at_NEW_LAYOUT_OBJECT_type nullNewLayoutObject;	/* null new layout object for overriding generic */
extern CONSTITUENT nullNLOLayoutStyle;			/* Style containing only null New Layout Object */

extern INT_type	CreateEmptyODADocStyle();		/* Initializes the style sheet support for a document */
extern p_styleSheetInfo	CreateEmptyODAStyleSheet();	/* Make a new set of style sheet pieces for a give style */
extern p_styleSheetInfo	LookupSSInfo(/*gD, whichStyle*/);	/* How to look up a style */

#endif
