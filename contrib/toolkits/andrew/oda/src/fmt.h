
#ifndef TK_INCLUDED_FMT
#define	TK_INCLUDED_FMT

#ifdef TK_TKI_FMT
#define TK_FMT
#else
#ifdef TK_IMPL_FMT
#define	TK_FMT
#else
#define AP_FMT
#endif
#endif

#include <lcsite.h>
#include <errors.h>
#include <apirt.h>
#include <tkerr.h>

/* Everyone sees these definitions */

#ifdef  TK_FMT
#endif

/*
    put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.
*/

#ifdef TK_IMPL_FMT
#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter
*/

#include <fmt1.h>

#endif
