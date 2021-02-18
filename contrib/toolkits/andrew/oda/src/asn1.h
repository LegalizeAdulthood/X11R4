
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/asn1.h,v 1.3 89/10/26 14:01:12 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/asn1.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	asn1.h,v $
 * Revision 1.3  89/10/26  14:01:12  mss
 * casted constants as needed for PC compatibilty
 * 
 * Revision 1.2  89/10/26  13:59:23  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/09/21  06:15:19  mss
 * Initial revision
 * 
 * Revision 1.6  89/09/06  16:26:21  annm
 * fixed bad defines for GENERALIZEDSTRING_TAG and VISIBLESTRING_TAG
 * 
 * Revision 1.5  88/10/03  17:08:13  mz0k
 * changed the INDEFINITE_LENGTH from (-1) to (-2) 
 * 
 * Revision 1.4  88/09/06  13:17:26  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.3  88/08/04  14:52:44  mz0k
 * added copyright notice
 * 
*/

/*******************************************************************\
* 								    *
* This is general purpose ASN.1 definitions for use within the TK.  *
* 								    *
\*******************************************************************/

#ifndef TK_INCLUDED_ASN1
#define	TK_INCLUDED_ASN1

/* Unkown or uncertain asn1 stuffs */
#define	UNKNOWNASN1

/* Definitions for identifier octets */

    /* Definitions for the CLASS field --  note that these are just integers */
#define UNIVERSAL_CLASS		((INT_type)0)
#define APPLICATION_CLASS	((INT_type)1)
#define CONTEXT_SPECIFIC_CLASS	((INT_type)2)
#define PRIVATE_CLASS		((INT_type)3)

    /* Definitions for the P/C field */
#define PRIMITIVE		((INT_type)0)
#define CONSTRUCTED		((INT_type)1)

    /* Definitions for TAG field */

/* This is the value that indicates a tag is >= 31 */
#define BIG_TAG			((INT_type)0x1F)

/* Special ID octet value indicating that default value should be used */
#define DEFAULT_ID		((INT_type)-1)



/* The UNIVERSAL types */
#define BOOLEAN_TAG		((INT_type)1)
#define INTEGER_TAG		((INT_type)2)
#define BITSTRING_TAG		((INT_type)3)
#define OCTETSTRING_TAG		((INT_type)4)
#define NULL_TAG		((INT_type)5)
#define OBJECT_IDENTIFIER_TAG	((INT_type)6)
#define OBJECT_DESCRIPTOR_TAG	((INT_type)7)
#define EXTERNAL_TAG		((INT_type)8)
#define SEQUENCE_TAG		((INT_type)16)
#define SET_TAG			((INT_type)17)
#define NUMERICSTRING_TAG	((INT_type)18)
#define PRINTABLESTRING_TAG	((INT_type)19)
#define TELETEXSTRING_TAG	((INT_type)20)
#define VIDEOTEXSTRING_TAG	((INT_type)21)
#define IA5STRING_TAG		((INT_type)22)
#define UNIVERSALTIME_TAG	((INT_type)23)
#define GENERALISEDTIME_TAG	((INT_type)24)
#define GRAPHICSTRING_TAG	((INT_type)25)
#define GENERALSTRING_TAG	((INT_type)28)
#define VISIBLESTRING_TAG	((INT_type)27)

    /* Definitions for length octets */

/* Value indicating that the unspecified form of length should be used */
#define INDEFINITE_LENGTH		((INT_type)-2)
#define	INDEFINITE_LENGTH_OCTET		((BYTE_type) 128)

/* Value indicating that short form of length can be used */
#define	SHORT_LENGTH			 ((BYTE_type)127)

#ifndef UNKNOWNASN1
#define UNKNOWNASN1
#endif

#endif
