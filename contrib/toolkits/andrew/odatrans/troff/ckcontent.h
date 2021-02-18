/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/ckcontent.h,v 1.2 89/09/08 17:48:20 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/ckcontent.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
CMU-ITC-071) for full text & permitted uses.

$Log:	ckcontent.h,v $
 * Revision 1.2  89/09/08  17:48:20  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:36:33  mss
 * Initial revision
 * 
 * Revision 1.1  89/01/05  18:47:13  jaap
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
