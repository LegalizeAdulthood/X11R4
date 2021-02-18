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
#define FALSE 0
#define TRUE  1
#include <stdio.h>
#include <ctype.h>

void reverse_binary(indata)
   int *indata;
{
   int test_byte;

   test_byte = *indata;
   *indata=255;
   
   if(test_byte & 128)
      *indata &= 127;
   
   if(test_byte & 64)
      *indata &= 191; 
   
   if(test_byte & 32)
      *indata &= 223;
   
   if(test_byte & 16)
      *indata &= 239;
   
   if(test_byte & 8)
      *indata &= 247;
   
   if(test_byte & 4)
      *indata &= 251;
   
   if(test_byte & 2)
      *indata &= 253;
   
   if(test_byte & 1)
      *indata &= 254;
}
