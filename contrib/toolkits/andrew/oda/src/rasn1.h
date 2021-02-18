
#ifndef TK_INCLUDED_RASN1
#define	TK_INCLUDED_RASN1

#ifdef TK_TKI_RASN1
#define TK_RASN1
#else
#ifdef TK_IMPL_RASN1
#define	TK_RASN1
#else
#define AP_RASN1
#endif
#endif

#include <lcsite.h>
#include <tkerr.h>
#include <values.h>
#include <io.h>
#include <acd.h>
#include <tags.h>
#include <parms.h>
#include <seq.h>

#include <asn1.h>


#define	EOFILE			    ((INT_type)	0x00)
/* for use in the 'String2Constituent'  when it's to set
 the CONSTITUENT_type attr to a construction expression. */
#define	IDINSTRUCT		    ((INT_type)	-1)

#ifdef  TK_RASN1
#endif

/*
    Put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.

*/

#ifdef TK_IMPL_RASN1

/* The type definition of the hash table */
typedef struct hashtab {
    STRING_type	    id;
    CONSTITUENT	    constituent;
    struct hashtab  *next;
}   HASH_type;

/* The size of hash table for the constituents. Can't cast it since
use to define array bounds, and VMS won't take casted values  */
#define	HASHSIZE	    (100)

/* The type definition of the STRING_PAIR_type */
typedef struct {
    INT_type	    tag[2];    /* When either turns 1, it means the corresponding id is present */
    STRING_type	    id[2];
} STRING_PAIR_type;

#endif

/* Special hacking for determining what routine to call for reading in 
    a negative integer.
    */
#ifdef ONES_COMPLEMENT	    
#define RASN1_GetInteger	    Read1sComplement
#else
#define	RASN1_GetInteger	    Read2sComplement
#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/

#include <rasn11.h>
#include <rasn12.h>
#include <rasn13.h>
#include <rasn14.h>

#endif
