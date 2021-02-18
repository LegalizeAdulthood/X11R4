

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/odatk.h,v 1.2 89/09/29 11:40:56 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/odatk.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	odatk.h,v $
 * Revision 1.2  89/09/29  11:40:56  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:34:52  mss
 * Initial revision
 * 
 * Revision 1.18  89/09/10  11:37:02  mss
 * Got rid of duplicate tkerr.h
 * 
 * Revision 1.17  89/03/22  10:57:45  mss
 * Added atts.h inclusion
 * 
 * Revision 1.16  88/09/14  20:12:11  jr
 * Include font module now
 * 
 * Revision 1.15  88/09/06  13:21:53  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.14  88/09/06  12:06:04  annm
 * 
 * no change
 * 
 * Revision 1.13  88/07/15  15:37:56  mss
 * Added include of tags.h
 * 
 * Revision 1.12  88/06/08  16:57:49  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.11  88/06/08  14:36:55  annm
 * added Copyright notice
 * 
*/

/*

	This is include file may be included by all CMU ODA Tool Kit
	applications.  This include will include all other include
	files that are necessary.

	Applications that wish to be more selective may want to not
	include this file, but instead do the #include's by hand.
	This is not recommended practice, but ...

*/

#ifndef TK_INCLUDED_ODATK
#define TK_INCLUDED_ODATK

/*
    Just include everything (except for low-level module implementation
    include files).
*/
#include <lcsite.h>

#include <apityp.h>
#include <aunion.h>
#include <acd.h>
#include <attrid.h>
#include <atts.h>
#include <debug.h>
#include <errors.h>
#include <tkerr.h>
#include <expr.h>
#include <font.h>
#include <init.h>
#include <iter.h>
#include <label.h>
#include <local.h>
#include <mem.h>
#include <parms.h>
#include <prior.h>
#include <rodif.h>
#include <seq.h>
#include <str.h>
#include <tags.h>
#include <values.h>
#include <wodif.h>

#endif
