/************************************************************
(c) Copyright 1988, 1989 McDonnell Douglas Corporation
McDonnell Douglas Corporation (MDC) is allowing this Computer
Software to be provided at no cost. MDC does not warrant that the
operation of the Software will be uninterrupted or error free.

MDC makes no warranties of any kind, expressed or implied, including
any implied warranty of merchantability or fitness for a particular purpose.

In no event shall MDC be liable under any legal or equitable theory
(including, but not limited to, contract, negligence, unintentional
misrepresentation or strict liability) for any damages whatsoever,
including, but not limited to direct, indirect or consequential damages,
even if MDC has notice of the possibility of such damages,
or for any claim against any party, inclduing any claim for patent,
copyright, or trade secret infringement.
************************************************************/
/*

                          odawalk


*/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/mdc/RCS/fontwalk.c,v 1.4 89/09/08 18:41:42 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/mdc/RCS/fontwalk.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/mdc/RCS/fontwalk.c,v 1.4 89/09/08 18:41:42 mss Exp $";
#endif

/*
  $Log:	fontwalk.c,v $
 * Revision 1.4  89/09/08  18:41:42  mss
 * added finalization
 * 
 * Revision 1.3  89/09/08  17:37:01  mss
 * /tmp/msg
 * 
 * Revision 1.2  89/09/05  15:12:37  mss
 * Added local initizalization
 * 
 * Revision 1.1  89/08/30  16:27:05  mss
 * Initial revision
 * 
 * Revision 1.3  89/05/23  16:11:14  mss
 * Fixed problems with deleting already deleted or nonexistant
 * sequences
 * 
 * Revision 1.2  89/01/10  13:05:59  mss
 * Changed call of FindDocumentRoot to conform to new
 * procedure interface
 * 
 * Revision 1.1  88/12/19  14:33:52  mss
 * Initial revision
 * 
 * Revision 1.1  88/08/29  15:01:50  mss
 * Initial revision
 * 


*/

#define Inherited    BOOL_true
#define NoInherited  BOOL_false
#define Local_Value  BOOL_false
#define Ignore_Parms (PARM_MASK_type *) 0
#define Layout       BOOL_false
#define Specific     BOOL_false

#include <stdio.h>
#include <odatk.h>
#include <ckcontent.h>
/*
#include <sample.h>
*/
extern cc_CHARACTER_FONTS_type   FONT;


/* Forward declarations */

INT_type DoSingleLO(/*oneLO*/);
INT_type GetText(/*whichDoc*/);

extern char *malloc();

int DEBUG = 0;  /* Turn on debugging */

/* Random global data */
CONSTITUENT        DocumentProfile ;
INIT_PARMS_type    parms;
FILE               *fopen(),*out,*in;
int                ii,newchar,zero = 0;
char               *object_id;

static char ia_ver[1][80]={
"<!OPS, Version = 5.2>\n"};

static char ia_doc[3][80]={
"<!Document,\n",
"	Final Output Device =	\"cx\",\n",
"	Default Printer =	\"INTERLEAF_PSPRINT\">\n"};

static char *ia_page[] = {
    "<!Page,\n",
    "	Left Margin =		0 inches,\n",
    "	Right Margin =		0 inches,\n",
    "	Top Margin =		0 inches,\n",
    "	Bottom Margin =		0 inches,\n",
    "	Starting Page # = 	Inherit,\n",
    "	Hyphenation =		off,\n",
    "	Revision Bar Placement = Left>\n"};
#define NPAGE	(sizeof(ia_page) / sizeof(ia_page[0]))

static char ia_clas_para[4][80]={
"<!Class, paragraph,\n",
"	Top Margin =		0.07 inches,\n",
"	Bottom Margin =		0.07 inches,\n",
"	Line Spacing =		1.308 lines>\n"};

#ifdef DONT_NEED_THIS

static char raster_hdr[9][80]={
"<Frame,Name =                  \"Following Text\",\n",
"	Placement =		Following Text,\n",
"	Horizontal Alignment =	Center,\n",
"	Width =			6.50 inches,\n",
"	Height =		3.25 inches,\n",
"	Page # =		1,\n",
"	Diagram =\n",
"V4,\n",
"(g9,1,0\n"};

#endif


INT_type main()
{
    dp_FONTS_LIST_type FONTS_LIST ;
    FONT_PAIR_type     *FPPTR ;
    int i, pointsize, dsclass ;
    float first, second ;
    INT_type weight, posture ;
    char *font, *emphasis ;

    DOCUMENT_type NewDoc;
    int od_num, rval, output = 0;
    char buffer[132];
    INT_type IntStatus;
    char *filename;

    filename = malloc(60) ;
    font = malloc (20) ;
    emphasis = malloc(10) ;

    printf("\nEnter Odif Stream Name    --->");
    scanf("%s",filename);
    if((in = fopen(filename,"rb"))==NULL)  {
      printf("\ncannot open input file");
      rval = ERROR_INT; 
      goto LEAVE;
    }

    printf("\nEnter Interleaf File Name --->");
    scanf("%s",filename);
    if((out = fopen(filename,"w"))==NULL)  {
      printf("\ncannot open output file");
      rval = ERROR_INT; 
      goto LEAVE;
    }    

    /* Start off everything */
    parms.automatic_label = BOOL_true;
    parms.error_od = fileno(stdout);
    parms.format_errors = BOOL_true;
    parms.output_filter = NULL_FILTER_ROUTINE;
    if (LCL_ERROR_INT == LOCAL_Init()){
	printf("Could not initialize localization facility\n");
    }

    IntStatus = InitToolKit(&parms);
    if (IntStatus == ERROR_INT) printf("Could not initialize toolkit\n");

    FONT = NULL_SEQUENCE;

    NewDoc = ReadODIF(fileno(in));
    if (NewDoc == ERROR_DOCUMENT) {
	printf("Error reading ODIF\n");
	exit(1);
    }

    /* print version stuff */
       rval = fprintf(out,&ia_ver[0][0]);
    
    /* print document stuff */
    for(ii=0;ii<3;ii++)
       rval = fprintf(out,&ia_doc[ii][0]);
    
    /* print document stuff */
    rval = fprintf(out,"%s","<!Font Definitions");

    DocumentProfile = FindDocumentProfile (NewDoc) ;
    if (DocumentProfile == ERROR_CONSTITUENT) {
	printf("Could not get Document Profile %s\n", ErrorMsg(TKError));
	rval = ERROR_INT;
	goto LEAVE;
    }

    if (ERROR_INT == GetAttr(DocumentProfile, dp_FONTS_LIST,
			     (POINTER_type) &FONTS_LIST, NoInherited, Ignore_Parms)) 
       printf("Couldn't access Document Profile Fonts List. (%s)\n",
	      ErrorMsg(TKError));
    else {
	if (FONTS_LIST != NULL_SEQUENCE) {
	    for(i=0; i<FONTS_LIST->length; i++) {
		FPPTR = &FONTS_LIST -> sequence_value.font_pairs[i] ;   

		/* Make sure size is specified */
		if (FPPTR->font_reference.dsnsize.second == 0) {
#ifdef DONT_COMPILE
		    /* No font size specified, use 12 pts as default */
		    printf(">>No size for font %d, using 12 pts\n", i);
		    FPPTR->font_reference.dsnsize.first = 1524;
		    FPPTR->font_reference.dsnsize.second = 360;
#else
		    continue;
#endif
		}
		first = (float) FPPTR->font_reference.dsnsize.first ;
		second = (float) FPPTR->font_reference.dsnsize.second ;
				    /* mm/inch * points/inch */
		pointsize = (int) ((((first / second) / 25.4) * 72.0) + 0.5) ;
		weight  = FPPTR->font_reference.weight ;
		posture = FPPTR->font_reference.posture ;

		/* Look at em,phasis & weight */
		emphasis = "" ;
		if (weight==ISO_WEIGHT_bold && posture==ISO_POSTURE_unspec)
		    emphasis = "Bold";
		if (weight==ISO_WEIGHT_bold && posture==ISO_POSTURE_upright)
		    emphasis = "Bold";
		if (weight==ISO_WEIGHT_unspec && posture==ISO_POSTURE_italic)
		    emphasis = "Italic" ;
		if (weight==ISO_WEIGHT_medium && posture==ISO_POSTURE_italic)
		    emphasis = "Italic" ;
		if (weight == ISO_WEIGHT_bold && posture==ISO_POSTURE_italic)
		    emphasis = "Bold" ;

		/* See if DSCLASS is specified */
		if (FPPTR->font_reference.dsclass != NULL_SEQUENCE) {
		    /* It is, use it to determine font family */
		    dsclass = 0 ;
		    for( ii = 0 ; ii < FPPTR->font_reference.dsclass->length ; ii++ )
		    dsclass = (dsclass * 10) +
			FPPTR->font_reference.dsclass->sequence_value.ints[ii] ;
		    if      (dsclass == 451) font = "Classic" ;
		    else if (dsclass == 465) font = "Typewriter" ;
		    else                     font = "Modern" ;
		} else {
		    STRUCTURED_NAME_type *family;
		    ISO9541_ID_type *idseq;
		    int nids;

		    /* There is no DSCLASS, look at family name */
		    family = &FPPTR->font_reference.family;
		    if (family->structured_name_tag == STRUCTURED_NAME_SEQ_tag) {
			idseq = family->structured_name_value.idseq->
				    sequence_value.iso9541_ids;
			nids = family->structured_name_value.idseq->length;
		    }
		    if (family->structured_name_tag ==
			STRUCTURED_NAME_UNSPEC_tag) continue;
		    if (family->structured_name_tag != STRUCTURED_NAME_SEQ_tag ||
			  nids == 0 ||
			  idseq[nids-1].iso9541_id_tag != ID_STR_tag) {
			printf(">>No/bad FAMILY for font %d, using CLASSIC\n", i);
			font = "Classic";
		    } else {
			BYTE_type *name;
			int length;

			/* Look at last element of family name */
			name = idseq[nids-1].iso9541_id_value.octet_string->
				    sequence_value.bytes;
			length = idseq[nids-1].iso9541_id_value.octet_string->
				    length;
			if (streql(name, length, "Times"))
			    font = "Classic";
			else
			if (streql(name, length, "Helvetica"))
			    font = "Modern";
			else
			if (streql(name, length, "Typewriter"))
			    font = "Typewriter";
			else {
			    printf(">>Unrecognized FAMILY, using CLASSIC\n");
			    font = "Classic";
			}
		    }
		}
		/* Print out this font */
		fprintf(out, ",\n\tF%d = %s %d %s",
		    FPPTR->font_id, font, pointsize, emphasis);
	    }
	    if (ERROR_INT == DeleteSequence(FONTS_LIST))
		printf("Delete failed for Document Profile fonts list (%s)\n",
		       ErrorMsg(TKError));
	}
	fprintf(out,">\n\n");   /* close out the attribute bundle */
    }
       
    
    /* print page stuff */
    for(ii=0;ii<NPAGE;ii++)
       rval = fprintf(out, ia_page[ii]);
    
    /* print document stuff */
    for(ii=0;ii<4;ii++)
       rval = fprintf(out,&ia_clas_para[ii][0]);

    /*
       Put out simple <paragraph> command -- this is in case the
       document starts off with a raster.
    */
    fputs("\n<Paragraph>\n", out);

    if (ERROR_INT == GetText(NewDoc)) {
	 printf("Could not extract text %s\n", ErrorMsg(TKError));
    }

    fflush(stdout);

#ifdef AlsoDumpDebugging
    if (ERROR_INT == WriteDocumentState(fileno(stdout), NewDoc)){
	printf("\nCould not provide debugging output: %s\n", ErrorMsg(TKError));
    }
#endif

    if (ERROR_INT == FinalToolKit()) {
	printf("Could not finalize the toolkit %s\n", ErrorMsg(TKError));
    }
    if (LCL_ERROR_INT == LOCAL_Final()) {
	printf("Could not finalize local %s\n", ErrorMsg(LCLError));
    }

LEAVE:
       ;

}

static int streql(bytes, nbytes, string)
    BYTE_type	*bytes;
    int		nbytes;
    CHAR_type	*string;
{
    REGISTER char *c;
    REGISTER BYTE_type *b;
    REGISTER int len;

    for (c=string, b=bytes; *c!='\0'; c++, b++, nbytes--) {
	if (nbytes <= 0) return 0;  /* String longer than bytes */
	if (*c != *b) return 0;
    }
    if (nbytes <= 0) return 1;
    else	     return 0;
}

INT_type GetText(whichDoc)
DOCUMENT_type whichDoc;
{
    CONSTITUENT		specLogRoot;
    INT_type		rval;

    rval = SUCCESS_INT;

    specLogRoot = FindDocumentRoot(whichDoc, SPECIFIC_DOC_LOGICAL_ROOT);
    if (ERROR_CONSTITUENT == specLogRoot) {
	printf("Could not get specific logical root %s\n", ErrorMsg(TKError));
	rval = ERROR_INT;
	goto LEAVE;
    }
    
    if (ERROR_INT == DoSingleLO(specLogRoot)) {
	printf("Could not process logical root %s\n", ErrorMsg(TKError));
	rval = ERROR_INT;
	goto LEAVE;
    }

    fprintf(out,"\n"); /* Make sure file ends with a new line */
 
LEAVE:

    return rval;

}


INT_type DoSingleLO(oneLO)
CONSTITUENT oneLO;
{
    INT_type			rval;		/* Standard return value */
    SEQUENCE_CONSTITUENT_type	contents;	/* content portions of basic objects */
    SEQUENCE_CONSTITUENT_type	subordinates;	/* subordinates of the composite */
    VALUE_type			curConType;	/* Type of the specific Constituent */
    INT_type			contentType, contentMode;   /* What kind of content in specific */
    INT_type			i;		/* Loop index */

    rval = SUCCESS_INT;

    if (ERROR_INT == GetValueAttr(oneLO, at_OBJECT_TYPE, (POINTER_type) &curConType, Local_Value )){
	printf("Missing object type on constituent(%s)\n", ErrorMsg(TKError));
	rval = ERROR_INT;
	goto LEAVE;
    }

    switch(curConType) {
	    case at_OBJECT_TYPE_doc_logical_root:
	    case at_OBJECT_TYPE_comp_logical_obj:

		/* Any children? (no guarantee they exist) */
		switch (HasAttr(oneLO, at_SUBORDINATES)) {
			case HAS_ATTR:
			    break;	/* Go get it */
			case NO_ATTR:
			    goto LEAVE; /* nothing to do */
			case ERROR_INT:
			default:
			    printf("Error locating subordinates attribute %s\n", ErrorMsg(TKError));
			    rval = ERROR_INT;
			    goto LEAVE;
		}

		/* Walk over children */
		if (ERROR_INT == GetAttr(oneLO, at_SUBORDINATES, (POINTER_type) &subordinates, Local_Value, Ignore_Parms )) {
		    printf("Could not get subordinates attribute for composite logical object %s\n", ErrorMsg(TKError));
		    rval = ERROR_INT;
		    goto LEAVE;
		}
		for (i=0; i< subordinates->length; i++ ){
		    if (ERROR_INT == DoSingleLO(subordinates->sequence_value.constituents[i])) {
			rval = ERROR_INT;
		    }
		}
		if (ERROR_INT == DeleteSequence(subordinates)) {
		    printf("Cannot delete sequence for subordinates (%s)\n", ErrorMsg(TKError));
		}
		break;

	    case at_OBJECT_TYPE_bas_logical_obj:
		/* Look for content portions, so pull them off and process */
		if (ERROR_INT == GetAttr(oneLO, at_CONTENT_PORTIONS, (POINTER_type) &contents, Local_Value, Ignore_Parms )) {
		    printf("Could not get content portion from logical object %s\n", ErrorMsg(TKError));
		    rval = ERROR_INT;
		    goto LEAVE;
		}
		GetContentType(oneLO, &contentType, &contentMode);
		/* Figure out which architecture class and handle appropriately */
		switch(contentType) {
		    case CharContent:
			ProcessCharCP(out, oneLO, contents, contentType, contentMode);
			break;
		    case RasterContent:
                        /* file = rt_prc.c   */
                        if(ERROR_INT==ProcessRasterCP
                              (oneLO,          /* logical object */
                               contents,       /* contents */
                               contentType,    /* content type */
                               contentMode,    /* mode ? */
                               out))  {        /* output file */
                           printf("Cannot process raster to Ileaf\n");
                           rval=ERROR_INT;
                           goto LEAVE;
                        }
                        break;
		    case GeoContent:
			printf("Neither rasters nor geometric graphics processed yet\n");
			break;
		    default:
			printf("Unknown content type %d\n", contentType);
			break;
		}
		if (ERROR_INT == DeleteSequence(contents)) {
		    printf("Cannot delete sequence for content portions (%s)\n", ErrorMsg(TKError));
		}
		break;

	    default:
		printf("Unknown object type %d\n", curConType);
		break;
    }


LEAVE:

    return rval;

}

