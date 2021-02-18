/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/odaatk.h,v 1.2 89/09/08 17:30:01 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/odaatk.h,v $ */

/*
  $Log:	odaatk.h,v $
 * Revision 1.2  89/09/08  17:30:01  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/09/07  16:27:19  mss
 * Initial revision
 * 
 * Revision 1.6  88/11/08  15:18:17  mss
 * Added global state for turning off raster scaling
 * 
 * Revision 1.5  88/11/04  17:05:31  mss
 * Fixed state for figure tracking
 * 
 * Revision 1.4  88/10/31  21:32:37  mss
 * More state for global SV and fonts
 * 
 * Revision 1.3  88/10/18  14:04:58  mss
 * Added SMU global data
 * 
 * Revision 1.2  88/09/22  11:10:28  mss
 * Added more global state
 * 
 * Revision 1.1  88/09/07  11:14:37  mss
 * Initial revision
 * 


*/

/* Include file for odaatkapp.c */

typedef struct odainfo_type_struct {
    INIT_PARMS_type ODAParams;    
    char * inputFileName;
    char * outputFileName;
    FILE * outputFileDescriptor;
    DOCUMENT_type odaDocument;
    struct text * topLevelDocument;
    struct view * topLevelView;
    CONSTITUENT specificLogicalRoot;
    CONSTITUENT genericLogicalRoot;
    CONSTITUENT genericLayoutRoot;
    CONSTITUENT documentProfile;
    CONSTITUENT initialBLO;	    /* Last non-concatenated text BLO */
    cc_CHARACTER_FONTS_type initialBLOCFonts;	/* Mapping for SGR codes */
    BOOL_type firstParagraphInDocument;	/* seen any paragraphs yet? */
    struct style	    * curStyle;
    struct text_statevector * curSV;
    CONSTITUENT		    curPage;
    char		    * curTextStream;
    char		    * curFNoteStream;
    struct style	    * globalStyle;
    struct text_statevector globalSV;
    BOOL_type		    doDebugging;
    INT_type		    SMU_Numerator;
    INT_type		    SMU_Denominator;
    dp_FONTS_LIST_type	    fonts;
    BOOL_type		    LastInsertWasFigure;
    BOOL_type		    ScaleRasters;
} odainfo_type, * p_odainfo_type;
