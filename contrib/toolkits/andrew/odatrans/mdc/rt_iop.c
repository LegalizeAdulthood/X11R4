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
                                  rt_iop.c
                    TO INTERLEAF FROM ODA -- PRESENTATION ATTRIBUTES
*/
#include <stdio.h>
#include <ctype.h>
#include <odatk.h>
#include <rx_dat.h>
#include <rx_con.h>

INT_type rt_iop(curBASIC,curContent,li,lo)
   CONSTITUENT			curBASIC;
   SEQUENCE_CONSTITUENT_type	curContent;
   ILEAF_RASTER_type		*li;
   ODA_RASTER_type		*lo;
{
    int  field1, *item, i, breaks, value, rval = {0};
           
    /* SET up Ileaf defaults */
    li->zcoord = 1;
    li->locks = 0;
    li->sub_e = 0;
    li->nbreaks = 0;
    li->rid = 1;
    li->bpp = 1;
    li->format = 1;
    
    /* get oda attrib */
    if(ERROR_INT == rt_goattrib(curBASIC,curContent,lo,0))  {
        printf("Cannot get attr's of raster\n");
        rval=ERROR_INT;
        goto LEAVE;
    }

    /* CALCULATE add'l values from ODA */
    /* set calculated values */


    /* pels per line */
    li->orig_ppl = lo->ppl;

    li->bitmap_ppl = ((int) (li->orig_ppl/8))*8;
    if((float) li->orig_ppl/8 > (int) li->orig_ppl/8)
       li->bitmap_ppl += 8;

    li->ileaf_ppl = ((int) (li->orig_ppl/16))*16;
    if((float) li->orig_ppl/16 > (int) li->orig_ppl/16)
       li->ileaf_ppl += 16;

    /* clipping rectangle */
    li->culx = lo->clip.ul.x;
    li->culy = lo->clip.ul.y;
    li->clrx = lo->clip.lr.x;
    li->clry = lo->clip.lr.y;

    /* number of lines */
    li->lines = lo->nlines;

    /* inches per line and inches per pel */
#ifdef RASTER_HACK
    li->inches_per_pel = lo->pel_sp.length;
    li->inches_per_pel /= lo->pel_sp.pel_spaces * 1200;
#else
    li -> inches_per_pel = 1.0 / 72.0;
#endif

    li->inches_per_line = li->inches_per_pel * lo->sp_rat.second;
    li->inches_per_line /= lo->sp_rat.first;

    /* parallelogram dimensions */
    li->pulx = UP_LEFT_X_START;
    li->pllx = UP_LEFT_X_START;
    li->puly = UP_LEFT_Y_START;
    li->pury = UP_LEFT_Y_START;


    li->purx = li->pulx + (RSUS_PER_INCH * 
                  (float) ((li->clrx - li->culx + 1) * li->inches_per_pel));

    li->plly = li->puly + (RSUS_PER_INCH * 
                  (float) ((li->clry - li->culy + 1) * li->inches_per_line));   

    li->wid_inches = li->purx / RSUS_PER_INCH;
    li->hei_inches = li->plly / RSUS_PER_INCH;



    /* ignored(?) supposed original dimensions */
    li->igd_width = 3*RSUS_PER_INCH;
    li->igd_height =3*RSUS_PER_INCH; 

LEAVE:
    return(rval);   
}

