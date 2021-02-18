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
                     file = rt_prc.c

                    PROCESS RASTER CONTENT
*/
#include <stdio.h>
#include <ctype.h>
#include <odatk.h>
#include <rx_dat.h>
#include <rx_con.h>

extern int DEBUG;

INT_type ProcessRasterCP(curBASIC, contents, contentType, contentMode, out)
CONSTITUENT  curBASIC;                    /* basic logical object */
SEQUENCE_CONSTITUENT_type	contents; /* content portions of blos */
   INT_type contentType, contentMode;
   FILE *out;                                 /* Ileaf stream */
  {

    ILEAF_RASTER_type *li;
    ODA_RASTER_type *lo;
    char *fileroot,*file[3],root[50];
    int find,x[3],*err_data,ineof,i;
    FILE *fopen();
    INT_type rval = 0;
    SEQUENCE_BYTE_type curRasterData;

    li = (ILEAF_RASTER_type *) malloc(sizeof(ILEAF_RASTER_type));
    lo = (ODA_RASTER_type *) malloc(sizeof(ODA_RASTER_type));

    /* GET content info */
    if (ERROR_INT == GetAttr(
               contents->sequence_value.constituents[0],
               at_CONTENT_INFORMATION, 
               (POINTER_type) &curRasterData, 
               BOOL_false, 
               ((PARM_MASK_type *) 0))) {
        printf("Missing content information (only checking here)", 
                   "(%s)\n", ErrorMsg(TKError));
	rval = ERROR_INT;
	goto LEAVE;
    }

    lo->raster_area = curRasterData->sequence_value.bytes;
    lo->bitmap_bytes = curRasterData->length;

    if (DEBUG)
	printf("RT_IOP:  Convert Raster Pres Attr's (Oda -> Ileaf)\n");
    if(ERROR_INT == rt_iop(curBASIC,contents,li,lo)){
       printf("cannot convert raster pres attr's...");
       rval=ERROR_INT;
       goto LEAVE;
    }

    if (DEBUG)
	printf("RT_FIN:  Write Raster header to Interleaf stream\n");
    if(ERROR_INT==rt_fin(li,out))  { 
       printf("Cannot write raster header to output.....\n");
       rval=ERROR_INT;
       goto LEAVE;
    }

    if (DEBUG)
	printf("RT_IOR:  Convert Raster object (Oda -> Ileaf)\n");
    if(ERROR_INT == 
        rt_ior(
           out,                                     /* ileaf output file */  
           li,                                     /* ileaf variables */  
           lo)){                                   /* oda variables */
       printf("cannot convert raster pres attr's...");
       rval=ERROR_INT;
       goto LEAVE;
    }

#if 0
    /* need to free ...... done by caller! */ 
    if(ERROR_INT == DeleteSequence(contents))  {
       printf("Cannot delete raster contents...\n");
       rval=ERROR_INT;
       goto LEAVE;
    }
#endif

    if(DEBUG)
	printf("RT_FIN2:  Write Raster trailer to Interleaf stream\n");
    if(ERROR_INT==rt_fin2(li,out))  { 
       printf("Cannot finish raster output.....\n");
       rval=ERROR_INT;
       goto LEAVE;
    }

LEAVE:
    return(rval);
}

