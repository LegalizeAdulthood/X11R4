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
                                 rt_goa.c
*/
#include <stdio.h>
#include <ctype.h>
#include <odatk.h>
#include <rx_dat.h>
#include <rx_con.h>

extern int DEBUG;

INT_type rt_goattrib(curBASIC,curContent,lo)
   CONSTITUENT			curBASIC;
   SEQUENCE_CONSTITUENT_type	curContent;
   ODA_RASTER_type		*lo;
{
/*  ------------------------------------------------------------ 
                      declare statements....                     */

   rg_SPACING_RATIO_type        	*oda_SPACING_RATIO;
   rg_PEL_SPACING_type	                *oda_PEL_SPACING;
   rg_CLIPPING_type                     *oda_CLIPPING;
   rg_NUMBER_OF_PELS_PER_LINE_type      oda_PPL;
   rg_NUMBER_OF_LINES_type              oda_NUMLINES;
   at_TYPE_OF_CODING_type               oda_TYPECODING;
   VALUE_type		                RasterEncoding;
   INT_type                             rval;

   rval=0;
   
/*  ------------------------------------------------------------ 
                          spacing ratio                           */

   if (ERROR_INT == GetAttr(curBASIC,
                            rg_SPACING_RATIO, 
                           (POINTER_type) &oda_SPACING_RATIO, 
                           BOOL_true, /* Inherit */ 
                           0)) {
       /* error!!! */
       printf("Cannot get raster SPACING RATIO!\n");
       rval = ERROR_INT;
       goto LEAVE;
   }
   
    if (DEBUG) {
	printf ("The line spacing value is %d\n", oda_SPACING_RATIO->first);
	printf ("The pel spacing value is %d\n", oda_SPACING_RATIO->second);
    }


     lo->sp_rat.first            = oda_SPACING_RATIO->first;
     lo->sp_rat.second           = oda_SPACING_RATIO->second;  

  free(oda_SPACING_RATIO); 
/*  ------------------------------------------------------------ 
                          pel spacing                            */


   if (ERROR_INT == GetAttr(curBASIC, 
                            rg_PEL_SPACING, 
                           (POINTER_type) &oda_PEL_SPACING, 
                            BOOL_true, /* Inherit */ 
                            0)) {

       /* error!!! */
       printf("Cannot get raster PEL SPACING!\n");
       rval = ERROR_INT;
       goto LEAVE;
   }

    if (DEBUG) {
	printf("The pel spacing length value is %d\n", oda_PEL_SPACING->length);
	printf("The pel spacing pel spaces value is %d\n",
	       oda_PEL_SPACING->pel_spaces);
    }

   lo->pel_sp.length           = oda_PEL_SPACING->length;
   lo->pel_sp.pel_spaces       = oda_PEL_SPACING->pel_spaces;

   free(oda_PEL_SPACING);   


/* -----------------------------------------------------------------
                          pels per line                             */


   if(ERROR_INT == GetIntAttr(
                curContent->sequence_value.constituents[0], 
                rg_NUMBER_OF_PELS_PER_LINE, 
                &oda_PPL, 
                BOOL_true)) {

       /*  ERROR  */
       printf("Cannot get raster spacing ratio!\n");
       rval = ERROR_INT;
       goto LEAVE;
   }

    if (DEBUG) printf("the value of pels per line is %d\n",oda_PPL);

   lo->ppl  = oda_PPL;

/* ----------------------------------------------------------------------
                            number of lines                          */

   if(ERROR_INT == GetIntAttr(
                      curContent->sequence_value.constituents[0], 
                      rg_NUMBER_OF_LINES, 
                      &oda_NUMLINES, 
                      BOOL_true)) {

       /*  ERROR */
       printf("Cannot get raster NUMBER OF LINES!\n");
       rval = ERROR_INT;
       goto LEAVE;
   }

    if (DEBUG)
	printf("the value of number of lines of raster data is %d\n",oda_NUMLINES);

   lo->nlines  = oda_NUMLINES;

/*    ------------------------------------------------------------
                          clipping                            

              Clipping is not yet implemented into the toolkit routines ...
*/

      if (ERROR_INT == GetAttr(
                   curBASIC, 
                   rg_CLIPPING,
                   (POINTER_type) &oda_CLIPPING, 
                   BOOL_true, 
                   0)) {

         lo->clip.ul.x          = 0;
         lo->clip.ul.y          = 0;
         lo->clip.lr.x          = lo->ppl - 1;
         lo->clip.lr.y          = lo->nlines - 1;

/* assume clipping is implemented...if not replace the foll. 4 lines with
   above...*/

         printf("Cannot get clipping information...\n");
         rval = ERROR_INT;
         goto LEAVE;
      }
      else  {

         lo->clip.ul.x          = oda_CLIPPING->coord1.first;
         lo->clip.ul.y          = oda_CLIPPING->coord1.second;
         lo->clip.lr.x          = oda_CLIPPING->coord2.first;
         lo->clip.lr.y          = oda_CLIPPING->coord2.second;

         free(oda_CLIPPING);  
      } 

    if (DEBUG) {
	printf("The clipping upper left coord pair x = %d  and y = %d \n",
	       lo->clip.ul.x, lo->clip.ul.y);
	printf("The clipping lower right coord pair x = %d  and y = %d \n",
	       lo->clip.lr.x, lo->clip.lr.y);
    }


/* ----------------------------------------------------------------------
                            type of encoding                          */

   if(ERROR_INT == GetAttr(
            curContent->sequence_value.constituents[0], 
            at_TYPE_OF_CODING, 
            (POINTER_type) &oda_TYPECODING,
            BOOL_true, 
            0))  {

       /*  ERROR */
       printf("Cannot get raster TYPE OF CODSING!\n");
       rval = ERROR_INT;
       goto LEAVE;
   }

   RasterEncoding = LookUpObjectId(oda_TYPECODING);

    if (DEBUG) {
	if (RasterEncoding == RG_BITMAP_CODING_id)
	    printf("type of encoding is bitmap\n");
	else if (RasterEncoding == RG_T6_CODING_id)       
	    printf("type of encoding is T6\n");
	else if (RasterEncoding == RG_T4_1_CODING_id)     
	    printf("type of encoding is T41\n");
	else if (RasterEncoding == RG_T4_2_CODING_id) 
	    printf("type of encoding is T42\n");
    }


   if (RasterEncoding == RG_BITMAP_CODING_id)   
        lo->type_coding  = B8;
   else if (RasterEncoding == RG_T6_CODING_id)       
        lo->type_coding  = T6;
   else if (RasterEncoding == RG_T4_1_CODING_id)     
        lo->type_coding  = T41;
   else if (RasterEncoding == RG_T4_2_CODING_id) 
        lo->type_coding  = T42;


   DeleteSequence(oda_TYPECODING);  
/* ---------------------------------------------------------------------- */

LEAVE:
   return(rval);

   }
