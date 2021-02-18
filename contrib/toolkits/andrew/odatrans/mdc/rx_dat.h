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
typedef struct {
   int zcoord, 
       locks,        
       igd_width,   
       igd_height,  
       pulx, 
       puly, 
       purx, 
       pury, 
       pllx, 
       plly, 
       sub_e,       
       culx,     
       culy,     
       clrx,     
       clry,     
       nbreaks,     
       rid,    
       orig_ppl,      /* ppl for the actual image */
       bitmap_ppl,    /* ppl for the bitmap (may be byte adjusted)  */
       ileaf_ppl,     /* ppl for the Ileaf stream (may be 2-byte adjusted)  */
       lines,    
       bpp,     
       format,        
       bytes;
float  inches_per_pel,
       inches_per_line,
       wid_inches,
       hei_inches;
} ILEAF_RASTER_type ;      


   typedef struct {
      INT_type                         x,
                                       y;
   }XY_type ;

   typedef struct {
      XY_type                     ul, lr;
   } rg_CLIP_type;

typedef INT_type rg_TYPE_OF_CODING_type;

   typedef struct {
      rg_CLIP_type                      clip;
      rg_SPACING_RATIO_type             sp_rat;   
      rg_PEL_SPACING_type               pel_sp;  
      rg_NUMBER_OF_LINES_type           nlines;  
      rg_NUMBER_OF_PELS_PER_LINE_type   ppl;     
      rg_TYPE_OF_CODING_type            type_coding;
      unsigned char                     *raster_area;
      INT_type                          bitmap_bytes;
   }      ODA_RASTER_type;
