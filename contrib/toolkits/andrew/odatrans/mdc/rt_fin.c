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

#include <stdio.h>
#include <ctype.h>
#include <rx_ful.h>
#include <rx_con.h>


static char raster_header[][80] = {
    "\n<Frame,\n",
    "	Name =			\"At Anchor\"\n",
    "	Placement =		At Anchor,\n",
#ifdef NO_ALIGNMENT
    "	Horizontal Alignment =	Left,\n",
#endif
    "	Width =			",
    " inches,\n",
    "	Height =		",
    " inches,\n",
    "	Page # =		1,\n",
    "	Diagram =\n",
    "V4,\n(g9,1,0\n (i13,1,0\n(X0,44,\n"};



INT_type rt_fin(li,ir)
   ILEAF_RASTER_type *li;
   FILE *ir;
{
    INT_type     input_exists,
                 raster_data,
                 rval = {0},
                 bytes_per_line,
                 extrabytes,
                 i;

    for(i=0;i<4;i++)
       fprintf(ir,"%s",raster_header[i]);
    
    /* raster width in inches */
    fprintf(ir,"%5.2f",li->wid_inches);

    for(i=4;i<6;i++)
       fprintf(ir,"%s",raster_header[i]);

    /* raster height in inches */
    fprintf(ir,"%5.2f",li->hei_inches);

    for(i=6;i<10;i++)
       fprintf(ir,"%s",raster_header[i]);

    rval = rt_pinumber(8,HEX,ir,li->igd_width);
    rval = rt_pinumber(8,HEX,ir,li->igd_height);
    rval = rt_pinumber(8,HEX,ir,li->pulx);
    rval = rt_pinumber(8,HEX,ir,li->puly);
    rval = rt_pinumber(8,HEX,ir,li->purx);
    rval = rt_pinumber(8,HEX,ir,li->pury);
    rval = rt_pinumber(8,HEX,ir,li->pllx);
    rval = rt_pinumber(8,HEX,ir,li->plly);
    rval = rt_pinumber(4,HEX,ir,li->sub_e);
    fprintf(ir,"\n");
    rval = rt_pinumber(4,HEX,ir,li->culx);
    rval = rt_pinumber(4,HEX,ir,li->culy);
    rval = rt_pinumber(4,HEX,ir,li->clrx);
    rval = rt_pinumber(4,HEX,ir,li->clry);
    rval = rt_pinumber(4,HEX,ir,li->nbreaks);
    fprintf(ir,")\n(X0,2,");
    rval = rt_pinumber(4,DEC,ir,li->rid);
    fprintf(ir,")\n(X0,30,");
    rval = rt_pinumber(4,HEX,ir,li->ileaf_ppl);
    rval = rt_pinumber(4,HEX,ir,li->lines);
    rval = rt_pinumber(4,HEX,ir,li->bpp);
    rval = rt_pinumber(4,HEX,ir,li->ileaf_ppl-1);
    rval = rt_pinumber(4,HEX,ir,0);   /* zero */
    rval = rt_pinumber(4,HEX,ir,0);   /* zero */
    rval = rt_pinumber(4,HEX,ir,li->lines-1);
    rval = rt_pinumber(4,HEX,ir,0);   /* zero */
    rval = rt_pinumber(4,HEX,ir,0);   /* zero */
    rval = rt_pinumber(4,HEX,ir,li->ileaf_ppl-1);
    rval = rt_pinumber(4,HEX,ir,li->lines-1);
    rval = rt_pinumber(2,HEX,ir,0);   /* zero */
    rval = rt_pinumber(2,HEX,ir,0);   /* zero */
    rval = rt_pinumber(4,HEX,ir,li->format);

    /* calculate raster bytes */
    bytes_per_line = li->bitmap_ppl/8;
    extrabytes = !(bytes_per_line % 2 == 0);
    li->bytes = ((bytes_per_line+1) * li->lines)+1;
    if(extrabytes)
       li->bytes += li->lines;

    rval = rt_pinumber(8,HEX,ir,li->bytes);
    fprintf(ir,")\n(X0,%d,",li->bytes);
    input_exists=TRUE;

LEAVE:
   return(rval);
}
