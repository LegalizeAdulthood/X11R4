
#ifndef TK_INCLUDED_FMT1
#define TK_INCLUDED_FMT1

#undef EXTERN

#ifdef TK_GVARS_FMT1
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_FMT

#ifndef USE_LONG_NAMES

#define FMT_Init	gfmini
#define	FMT_Final	gfmfnl
#define FMT_FormatInt	gfmint

#define FormatInt	    gfmfnt
#define RecursiveFormatInt  gfmrfi

#endif

EXTERN INT_type FMT_Final();
EXTERN INT_type FMT_Init();
EXTERN INT_type FMT_FormatInt();

#endif

/*
    Declarations seen within the FMT module go here.
*/

#ifdef TK_IMPL_FMT

#endif

#endif
