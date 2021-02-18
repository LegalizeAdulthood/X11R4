/*	$Header: /u/04c/mnt/new_integ/utilities/include/RCS/u22_rtd.h,v 1.1 89/10/07 23:54:27 serpent Exp $ */

/*
$Log:	u22_rtd.h,v $
 * Revision 1.1  89/10/07  23:54:27  serpent
 * Initial revision
 * 
 * Revision 1.6  89/09/12  23:07:55  bmc
 * Added definitions for dro_less_than_or_equal and dro_greater_than_or_equal
 * 
 * Revision 1.5  89/05/31  13:22:04  little
 * jobs
 * add definition for dro_not_equal
 * 
 * Revision 1.4  89/05/30  11:37:40  serpent
 * fix up a define statement for NULL
 * 
 * Revision 1.3  89/05/26  10:51:51  little
 * add definition for NULL
 * 
 * Revision 1.2  89/05/25  13:26:01  little
 * add #define for dob_null_object
 * 
 * Revision 1.1  89/05/19  11:11:07  little
 * Initial revision
 * 
*/


/*--------------------------------------------------------------------------*\
|
|   Distribution: Approved for public release; distribution is unlimited.
|
|   Copyright (c) 1989  by  Carnegie  Mellon  University, Pittsburgh, PA.  The
|  Software Engineering  Institute  (SEI) is a federally  funded  research and
|  development center established  and  operated by Carnegie Mellon University
|  (CMU).  Sponsored  by  the  U.S.  Department  of   Defense  under  contract
|  F19628-85-C-0003,  the  SEI  is  supported  by  the  services  and  defense
|  agencies, with the U.S. Air Force as the executive contracting agent.
|
|    Permission to use,  copy,  modify, or  distribute  this  software and its
|  documentation for  any purpose and without fee  is hereby granted, provided
|  that  the above copyright notice appear  in  all copies and that both  that
|  copyright  notice  and   this  permission  notice  appear   in   supporting
|  documentation.   Further,  the  names  Software  Engineering  Institute  or
|  Carnegie  Mellon  University  may  not be used in  advertising or publicity
|  pertaining to distribution of the software without specific, written  prior
|  permission.  CMU makes no claims  or representations  about the suitability
|  of this software for any purpose.  This software is provided "as is" and no
|  warranty,  express  or  implied,  is  made  by  the  SEI  or CMU, as to the
|  accuracy  and  functioning of the program and related program material, nor
|  shall   the  fact of  distribution   constitute  any   such  warranty.   No
|  responsibility is assumed by the SEI or CMU in connection herewith.
|
\*--------------------------------------------------------------------------*/



#define stderr (&_iob[2])
#define LIST_DEFAULT_VALUE 0
#define NO_GENERIC_ROUTINE -1
#define NOT_NODES 0
#define NULLlist ((LIST) 0)
#define iid_null_id -1
#define DEBUG2(statement)\
if (d22_debug > 1)\
{\
statement;\
fprintf (stderr, " file %s, line %d\n", __FILE__, __LINE__);\
fflush (stderr);\
}
#define LISThead( list ) (((LIST) (list)) -> next)
#define LISTtail( list ) (((LIST) (list)) -> prev)
#define LISTvalue( list ) (((LIST) (list)) -> value)
#define make_list() _make_list()
#define free_list( list, flag ) \
_free_list( (LIST) (list), (int) (flag) )
#define list_length( list ) (int) (LISTvalue( list ))
#define list_empty( list ) (list_length( list ) == 0)
#define add_to_head_val( list, node, val ) \
(list_empty( list ) ? \
_list_add_node( list, &LISTtail(list), &LISThead(list), \
(NODE) node, val ) : \
_list_add_node( list, &LISThead(list)->prev, \
&LISThead(list), (NODE) node, val ))
#define add_to_head( list, node ) \
add_to_head_val( list, node, LIST_DEFAULT_VALUE )
#define add_to_tail_val( list, node, val ) \
(list_empty( list ) ? \
_list_add_node( list, &LISTtail(list), &LISThead(list), \
(NODE) node, val ) : \
_list_add_node( list, &LISTtail(list), \
&LISTtail(list)->next, (NODE) node, val ))
#define add_to_tail( list, node ) \
add_to_tail_val( list, node, LIST_DEFAULT_VALUE )
#define set_status(val) isc_status=val
#define dob_null_object ((dob_data_object) -1)
#define NULL 0
#define dro_not_equal(op1, op2) dro_not(dro_equal(op1, op2))
#define dro_less_than_or_equal(op1, op2) dro_not(dro_greater(op1, op2))
#define dro_greater_than_or_equal(op1, op2) dro_not(dro_less(op1, op2))

