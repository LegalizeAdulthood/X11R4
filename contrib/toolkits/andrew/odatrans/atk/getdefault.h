/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/getdefault.h,v 1.2 89/09/08 17:27:27 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/getdefault.h,v $ */

/*
  $Log:	getdefault.h,v $
 * Revision 1.2  89/09/08  17:27:27  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:15:51  mss
 * Initial revision
 * 
 * Revision 1.1  88/10/31  21:36:04  mss
 * Initial revision
 * 


*/

/* Include file for getdefault.c */

typedef struct FontAsSV_struct {
    struct FontAsSV_struct  * next;	/* next block in list */
    INT_type		    font_id;	/* font id (in fonts list) this corresponds to */
    FONT_REFERENCE_type	    * theFont;	/* pointer back to block, for debugging */
    BOOL_type		    nameDefined;    /* Is a (tanslated) font name specified? */
    BOOL_type		    sizeDefined;    /*Is a size defined? */
    BOOL_type		    facesDefined;   /* Any face codes (italic, bold, shadow, outline, fixed) present? */
    BOOL_type		    flagsDefined;  /* Any underline/overbars, shadow, outline? */
    BOOL_type		    scriptsDefined; /* Any sub/superscripts specified? */
    char *		    newFamilyName;  /* Name to use, if any */
    long		    newFontSize;    /* Font size, if any */
    long		    newFaces;	    /* Any faces that are present */
    long		    newFlags;	    /* Any flags that are present (underline, overbar)*/
    long		    newScript;	    /* sub/super scripts, if any */
} FontAsSV_type, *p_FontAsSV_type;

/* Global state */


extern p_FontAsSV_type MappedSVFontList;

extern INT_type GetGlobalStyle(/*gd*/);
extern p_FontAsSV_type FindSVforFont(/* gD, fontID */);
