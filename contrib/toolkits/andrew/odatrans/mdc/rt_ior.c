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
                               rt_ior.c

                       TO INTERLEAF FROM ODA -- RASTER CONTENT

*/
#include <stdio.h>
#include <ctype.h>
#include <rx_ful.h>
#include <rx_con.h>

extern int DEBUG;

INT_type rt_ior(ir,li,lo)
   FILE              *ir;          /* Interleaf output file */
   ILEAF_RASTER_type *li;          /* interleaf data area */
   ODA_RASTER_type   *lo;          /* oda data area */
{
    static unsigned char *input_format,*reverse,*file1,*file2;
    static int x,lines,ineof,xxx,yyy,ppl,extrabytes;
    static int i,bytestowrite,*fatal_err,bytecount;

    bytestowrite = li->bitmap_ppl/8;

    /* Use the original code */
#define OLD_CODE

#ifdef OLD_CODE
    extrabytes = !(bytestowrite % 2 == 0);
#else
    if (bytestowrite % 2) {
	bytestowrite++;
    }
#endif


    lines=0;
    while (lines<li->lines)  {
       
       lines++;
       x=!(lines % 25);
       if(x && DEBUG)
          printf("INFORMATION   ===> scan line : %d \015",lines);
       for(i=1;i<=bytestowrite;i++){  
          xxx = *(lo->raster_area);
          lo->raster_area++;
          bytecount++;
          if(i==1)  {
             /* write a cr and next line starter*/
             putc('\n',ir); 
             putc('F',ir); putc('A',ir);
          }   
          if(!(i % 35))
             putc('\n',ir);

          /* reverse the bits  */
          reverse_binary(&xxx);      

          yyy = (xxx >> 4) + '0';
          if(yyy>'9') 
             yyy += 7;
          putc(yyy,ir);
          xxx = (xxx &= 15) + '0';
          if(xxx>'9') 
             xxx += 7;
             putc(xxx,ir);
          }
#ifdef OLD_CODE
       if(extrabytes)   {
          putc('0',ir);
          putc('0',ir);
       }
#endif
    }
    /* CALCULATE bytes written */
    li->bytes = ((bytestowrite+1) * lines)+1;
    if(extrabytes)
       li->bytes += lines;

    /* write an eof marker */
    putc('F',ir); putc('F',ir);

    if (DEBUG)
	printf("          INFORMATION   ===> lines compressed             = %d \n",
	       lines);
}
