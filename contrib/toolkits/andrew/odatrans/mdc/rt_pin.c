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

INT_type rt_pinumber(bytes,hex,outfile,value)
   int bytes;
   int hex;
   FILE *outfile;
   int value;
{
   INT_type rval = {0};
   static char *hexs[9] = {"%0X","%01X","%02X",
                          "%03X","%04X","%05X",
                          "%06X","%07X","%08X"};

   static char *decs[9] = {"%0d","%01d","%02d",
                          "%03d","%04d","%05d",
                          "%06d","%07d","%08d"};

   if(hex)   
      fprintf(outfile,hexs[bytes],value);
   else 
      fprintf(outfile,decs[bytes],value);

LEAVE:
   return(rval);
}
