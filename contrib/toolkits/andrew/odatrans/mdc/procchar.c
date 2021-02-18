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
                     file = procchar.c

                    PROCESS character content portions 
*/
#include <stdio.h>
#include <ctype.h>
#include <odatk.h>

extern CONSTITUENT DocumentProfile ;

#define Inherited    BOOL_true
#define NoInherited  BOOL_false
#define Local_Value  BOOL_false
#define Ignore_Parms (PARM_MASK_type *) 0

cc_CHARACTER_FONTS_type   FONT;

INT_type ProcessCharCP(out, curBasic, contents, contentType, contentMode)
FILE *out;
CONSTITUENT curBasic ;
SEQUENCE_CONSTITUENT_type	contents;	/* content portions of basic objects */
INT_type contentType, contentMode;
{
    at_CONCATENATION_type     CONCATENATION;
    at_OFFSET_type            *OFFSET;   
    at_SEPARATION_type        *SEPARATION;   

    cc_ALIGNMENT_type         ALIGNMENT;
    cc_1ST_LINE_OFFSET_type   FIRST_LINE_OFFSET;
	    /* Should be cc_1ST_LINE_OFFSET_type */
    cc_LINE_SPACING_type      LINE_SPACING;

    INT_PAIR_type             *IPPTR;
    INT_type	              i, rval;
    register int    fontsize;

    rval = SUCCESS_INT;

    /* Check to see if this is a paragraph (unconcatenated) beginning */

    if (ERROR_INT == GetValueAttr(curBasic, at_CONCATENATION, (VALUE_type *) &CONCATENATION, Inherited)) {
	printf("Could not get value for concatenation attribute and assuming non-conconcatented (%s)\n", ErrorMsg(TKError));
	CONCATENATION = at_CONCATENATION_non_concated;
    }
    if (CONCATENATION == at_CONCATENATION_non_concated) {

       /* Starting a new paragraph, place in separator (should really check to see if first, but this is simple) */

       fprintf(out,"\n\n<Paragraph");

       /* Get the SEPARATION values */

#ifdef SEPARATION

   /*****************************************************
    * SEPARATION not working in TK -- so ignore for now *
    *****************************************************/

       if (ERROR_INT == GetAttr(curBasic, at_SEPARATION, (POINTER_type) &SEPARATION, Inherited, Ignore_Parms)) 
          printf("Could not access SEPARATION values. (%s)\n", ErrorMsg(TKError));
       else {
          fprintf(out,",\n    Top Margin    = %f inches",
		  (float) SEPARATION->trailing_edge / 1200.0);
          fprintf(out,",\n    Bottom Margin = %f inches",
		  (float) SEPARATION->leading_edge  / 1200.0);
	  free(SEPARATION);
       }
#endif

       /* Get the OFFSET values */

       if (ERROR_INT == GetAttr(curBasic, at_OFFSET, (POINTER_type) &OFFSET, Inherited, Ignore_Parms)) 
          printf("Could not access OFFSET values. Assuming default (%s)\n", ErrorMsg(TKError));
       else {
          fprintf(out,",\n    Left Margin   = %f inches",
		  (float) OFFSET->right_hand_offset / 1200.0);
          fprintf(out,",\n    Right Margin  = %f inches",
		  (float) OFFSET->left_hand_offset  / 1200.0);
	  free(OFFSET);
       }

       /* Get the value for FIRST LINE OFFSET */

       if (ERROR_INT == GetIntAttr(curBasic, cc_1ST_LINE_OFFSET, (POINTER_type) &FIRST_LINE_OFFSET, Inherited))
          printf("Could not get value for FIRST LINE OFFSET. (%s)\n", ErrorMsg(TKError));
       else 
          fprintf(out,",\n    First Indent = %f inches",
		  (float) FIRST_LINE_OFFSET / 1200.0);

       /* Get the value of ALIGNMENT */

       if (ERROR_INT == GetValueAttr(curBasic, cc_ALIGNMENT, (POINTER_type) &ALIGNMENT, Inherited))
          printf("Could not get value for ALIGNMENT. (%s)\n", ErrorMsg(TKError));
       else if (ALIGNMENT == cc_ALIGNMENT_start_aligned) fprintf(out,",\n    Alignment = Left"  );
       else if (ALIGNMENT == cc_ALIGNMENT_end_aligned  ) fprintf(out,",\n    Alignment = Right" );
       else if (ALIGNMENT == cc_ALIGNMENT_centred      ) fprintf(out,",\n    Alignment = Center");
       else if (ALIGNMENT == cc_ALIGNMENT_justified    ) fprintf(out,",\n    Alignment = Both"  );

       /* Get the CHARACTER FONTS attribute (determine there use later) */

       if (FONT != NULL_SEQUENCE) {
	   DeleteSequence(FONT) ;  /* Delete any old font sequence */
       }
       if (ERROR_INT == GetAttr(curBasic, cc_CHARACTER_FONTS,
				(POINTER_type) &FONT, Inherited, Ignore_Parms)) 
          printf("Could not access CHARACTER FONTS. (%s)\n", ErrorMsg(TKError));
       else 
          if (FONT != NULL_SEQUENCE) {
		/* Get primary font only */
            IPPTR = &FONT -> sequence_value.int_pairs[0];
		/* Get font indentifier : point size in font definition */
            fprintf(out,",\n    Font = F%d", IPPTR->second);
	    fontsize = IPPTR -> first;
          } else
	    fontsize = 12;
       /* Get the value for LINE SPACING */

#ifdef LINE_SPACING_WORKS

       if (ERROR_INT == GetIntAttr(curBasic, cc_LINE_SPACING,
				   &LINE_SPACING, Inherited))
          printf("Could not get value for LINE SPACING. (%s)\n",
		 ErrorMsg(TKError));
       else
          fprintf(out,",\n    Line Spacing = %f lines",
		  (float) LINE_SPACING / (float) fontsize);

#endif

       fprintf(out,">\n\n");   /* close out the attribute bundle */
    }
    /* Go get all of the content portions (assuming ascii for now) */
    for (i=0; i< contents->length; i++ ){
        if (ERROR_INT == ProcessSingleCharCP(out,(POINTER_type) FONT, contents->sequence_value.constituents[i], contentMode)) {
	    rval = ERROR_INT;
	}
    }

LEAVE:
    return rval;
}

INT_type ProcessSingleCharCP(out, FONT, curContent, contentMode)
FILE *out;
cc_CHARACTER_FONTS_type   FONT;
CONSTITUENT curContent ;
INT_type contentMode; {

    SEQUENCE_BYTE_type curTextData;
    INT_type		rval,ii;
    
    dp_FONTS_LIST_type        FONTS_LIST ;
    FONT_PAIR_type            *FPPTR ;
    int                       jj, font_number ;
    char                      pn[10], ESC = 27, SGR = 'm' ;
    INT_PAIR_type             *IPPTR;

    /* Look for content information */
    if (ERROR_INT == GetAttr(curContent, at_CONTENT_INFORMATION,(POINTER_type) &curTextData, Local_Value, Ignore_Parms )) {
	printf("Missing content information (not checking for other places) (%s)\n", ErrorMsg(TKError));
	rval = ERROR_INT;
	goto LEAVE;
    }

    /*
       Here's special hack for start of each content portion.  ODA says
       that for a content portion that doesn't start with an SGR, that
       the default SGR is used.  Easiest thing to do here is just output
       default font.
   */
    if (FONT != NULL_SEQUENCE) {
	IPPTR = &FONT -> sequence_value.int_pairs[0];
	fprintf(out, "<F%d>", IPPTR -> second);
    }

    /*
       Throw the bytes onto out (file) -- minimal checking, should
       really be parsed.
    */

    ii = 0 ;
    jj = 0 ;
    while (ii<curTextData->length) {
       if (curTextData->sequence_value.bytes[ii] == ESC) {
          ii++ ;
          if (curTextData->sequence_value.bytes[ii] == '[') {
             ii++ ;
	     jj = 0;
             while ((curTextData->sequence_value.bytes[ii] != SGR) &&
			(curTextData->sequence_value.bytes[ii] != '^'))
                pn[jj++] = curTextData->sequence_value.bytes[ii++] ;
	     pn[jj] = '\n' ;
	     if (curTextData->sequence_value.bytes[ii++] != '^'){
		 sscanf(pn,"%d",&font_number);
		 font_number = font_number - 10 ;
		 /* Font SGRs range from 10 - 19 sequence index is 0 - 9 */

		 if (FONT != NULL_SEQUENCE) {
		     IPPTR = &FONT -> sequence_value.int_pairs[font_number] ; 
			/* Get font indentifier */
		     fprintf(out,"<F%d",IPPTR->second);
		     if (ERROR_INT == GetAttr(DocumentProfile,
					      dp_FONTS_LIST,
					      (POINTER_type) &FONTS_LIST,
					      NoInherited, Ignore_Parms))
			 printf("Couldn't access \"Fonts List\". (%s)\n",
				ErrorMsg(TKError));
		     else 
			 if (FONTS_LIST != NULL_SEQUENCE) {
			     FPPTR = &FONTS_LIST->
					sequence_value.
					font_pairs[IPPTR->second-1] ;   
			     if (FPPTR->font_reference.modes.specified == BOOL_true)
				 if (FPPTR->font_reference.modes.iso_0.specified ==
				     BOOL_true )
				     if (FPPTR->font_reference.modes.iso_0.scores.specified == BOOL_true) {
					 if (FPPTR->font_reference.modes.iso_0.scores.rscore.specified == BOOL_true) fprintf(out,",@U") ;
					 if (FPPTR->font_reference.modes.iso_0.scores.tscore.specified == BOOL_true) fprintf(out,",@S") ;
				     }
			 }
		     if (ERROR_INT == DeleteSequence(FONTS_LIST))
			 printf("Error deleting sequence for Document Profile fonts list (%s)\n", ErrorMsg(TKError)) ;
		     fprintf(out,">") ;
		 }
	     } else {
		 /* Processed tab, not a font change */
		 fprintf(out, "\t");
	     }
	  } 
	} else {
	    if (curTextData->sequence_value.bytes[ii] == '\015')
	    /* Ignore <cr> */
		ii++;
	    else
		if (curTextData->sequence_value.bytes[ii] == '\012') {
		    fprintf(out, "<HR>");
		    ii++;
		} else {
		    if (curTextData->sequence_value.bytes[ii] == '<')
			fprintf(out, "<");
		    fputc(curTextData->sequence_value.bytes[ii++], out);
		}
	}
    }

    if (ERROR_INT == DeleteSequence(curTextData))
	printf("Error deleting sequence for text content information (%s)\n", ErrorMsg(TKError));

LEAVE:

    return rval;

}
