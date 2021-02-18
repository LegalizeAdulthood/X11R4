#ifdef sgi
# ifdef mips
#  include "s-iris4d.c"
#  define s_included
# endif
#endif

/* This should work on most BSD machines */
#ifndef s_included
# include "s-bsd.c"
#endif
