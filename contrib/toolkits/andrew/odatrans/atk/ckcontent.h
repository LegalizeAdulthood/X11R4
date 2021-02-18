/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/ckcontent.h,v 1.2 89/09/08 17:26:06 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/ckcontent.h,v $ */


/*
  $Log:	ckcontent.h,v $
 * Revision 1.2  89/09/08  17:26:06  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:16:47  mss
 * Initial revision
 * 
 * Revision 1.4  88/11/02  16:56:08  mss
 * Added equation and table content types
 * 
 * Revision 1.3  88/10/11  22:00:32  mss
 * CHanged name of GetContentType back
 * 
 * Revision 1.2  88/10/10  18:51:43  mss
 * renamed getcontenttype
 * 
 * Revision 1.1  88/09/07  11:10:53  mss
 * Initial revision
 * 


*/

#define CharContent (1)
#define RasterContent (2)
#define GeoContent (3)
#define FormattedContent (4)
#define ProcessableContent (5)
#define FormattedProcContent (6)
#define EquationContent (7)
#define TableContent (8)

extern void GetContentType(/*specConstituent, contentType, contentMode*/);
