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
typedef int INT_type;
typedef unsigned char BYTE_type;
typedef BYTE_type BOOL_type;

   typedef struct {
      INT_type           first,second;
   }INT_PAIR_type ;

typedef struct {
    INT_PAIR_type coord1, coord2;
} rg_CLIPPING_type;

typedef INT_type rg_NUMBER_OF_LINES_type;

typedef INT_type rg_NUMBER_OF_PELS_PER_LINE_type;

typedef struct {
    BOOL_type   null;
    INT_type    length;
    INT_type    pel_spaces;
} rg_PEL_SPACING_type;

typedef INT_PAIR_type rg_SPACING_RATIO_type;

#include <rx_dat.h>
