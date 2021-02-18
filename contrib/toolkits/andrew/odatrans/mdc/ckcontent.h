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
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/mdc/RCS/ckcontent.h,v 1.2 89/09/08 17:36:38 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/mdc/RCS/ckcontent.h,v $ */


/*
  $Log:	ckcontent.h,v $
 * Revision 1.2  89/09/08  17:36:38  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:29:41  mss
 * Initial revision
 * 
 * Revision 1.2  88/12/19  14:33:44  mss
 * Updated for demo version of MDC's interleaf translator
 * 
 * Revision 1.1  88/08/29  15:02:38  mss
 * Initial revision
 * 


*/

#define CharContent (1)
#define RasterContent (2)
#define GeoContent (3)
#define FormattedContent (4)
#define ProcessableContent (5)
#define FormattedProcContent (6)

extern INT_type GetContentType(/*specConstituent, contentType, contentMode*/);
