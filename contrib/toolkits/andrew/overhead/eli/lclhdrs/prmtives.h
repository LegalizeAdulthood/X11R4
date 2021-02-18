/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/*
 * $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/eli/lclhdrs/RCS/prmtives.h,v 2.6 89/05/09 17:47:42 bobg Exp $
 *
 * $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/eli/lclhdrs/RCS/prmtives.h,v $
 */

#include <eli.h>
#include <ctype.h>
#include <andrewos.h>                  /* strings.h sys/file.h */
#include <sys/param.h>
#include <sys/types.h>

/* andrewos.h includes this.  BOGUS!
#include <sys/time.h>
*/

#include <sys/wait.h>
#include <signal.h>
#include <setjmp.h>

#include <regexp.h>

#define GENSYM_NAMELEN      16
#define FILTERBUFSIZ        256        /* Arbitrary */
#define FILTERTIMEOUT       300        /* Real seconds before killing child */

extern char    *getenv(), *getprofile(), *malloc(), *realloc(), *index();
extern char    *AndrewDir();           /* from libutil.a */

extern void     Prim_AND();            /* prmtives.c, line 1133 */
extern void     Prim_APPEND();         /* prmtives.c, line 2064 */
extern void     Prim_ASSOC();          /* prmtives.c, line 935 */
extern void     Prim_ATOM();           /* prmtives.c, line 1523 */
extern void     Prim_BOUNDP();         /* prmtives.c, line 1606 */
extern void     Prim_CAR();            /* prmtives.c, line 617 */
extern void     Prim_CDR();            /* prmtives.c, line 667 */
extern void     Prim_COND();           /* prmtives.c, line 757 */
extern void     Prim_CONS();           /* prmtives.c, line 520 */
extern void     Prim_CONSP();          /* prmtives.c, line 1479 */
extern void     Prim_DEBUG();          /* prmtives.c, line 3145 */
extern void     Prim_DEFUN();          /* prmtives.c, line 389 */
extern void     Prim_DEFUNQ();         /* prmtives.c, line 454 */
extern void     Prim_DEFUNV();
extern void     Prim_DEFUNVQ();
extern void     Prim_DISCARD();        /* prmtives.c, line 3300 */
extern void     Prim_DIV();            /* prmtives.c, line 1716 */
extern void     Prim_DO();             /* prmtives.c, line 2742 */
extern void     Prim_DOSTAR();         /* prmtives.c, line 1211 */
extern void     Prim_EQ();             /* prmtives.c, line 840 */
extern void     Prim_EQUAL();          /* prmtives.c, line 3179 */
extern void     Prim_ERROR();          /* prmtives.c, line 3391 */
extern void     Prim_EVAL();           /* prmtives.c, line 592 */
extern void     Prim_FILTER();
extern void     Prim_FUNCTION();       /* prmtives.c, line 2466 */
extern void     Prim_GENSYM();         /* prmtives.c, line 2440 */
extern void     Prim_GETENV();         /* prmtives.c, line 3112 */
extern void     Prim_INDEX();          /* prmtives.c, line 1808 */
extern void     Prim_INT_TO_STR();     /* prmtives.c, line 2932 */
extern void     Prim_LCSTRING();       /* prmtives.c, line 2025 */
extern void     Prim_LESSP();          /* prmtives.c, line 1571 */
extern void     Prim_LET();            /* prmtives.c, line 2665 */
extern void     Prim_LETSTAR();        /* prmtives.c, line 1050 */
extern void     Prim_LIST();           /* prmtives.c, line 720 */
extern void     Prim_LOAD();           /* prmtives.c, line 2501 */
extern void     Prim_MINUS();          /* prmtives.c, line 1640 */
extern void     Prim_NOT();            /* prmtives.c, line 1189 */
extern void     Prim_NUMBERP();        /* prmtives.c, line 1549 */
extern void     Prim_OR();             /* prmtives.c, line 1161 */
extern void     Prim_PLUMBER();        /* prmtives.c, line 2291 */
extern void     Prim_PLUS();           /* prmtives.c, line 352 */
extern void     Prim_PRINT();          /* prmtives.c, line 799 */
extern void     Prim_PRINTF();         /* prmtives.c, line 2972 */
extern void     Prim_PROGN();          /* prmtives.c, line 566 */
extern void     Prim_PUTS();           /* prmtives.c, line 3060 */
extern void     Prim_READ();           /* prmtives.c, line 1404 */
extern void     Prim_RE_STRCONTAINS(); /* prmtives.c, line 1404 */
extern void     Prim_RE_STRDECOMPOSE();/* prmtives.c, line 1404 */
extern void     Prim_RE_STRDECOMPOSEPLUS();     /* prmtives.c, line 1404 */
extern void     Prim_RINDEX();         /* prmtives.c, line 1853 */
extern void     Prim_SETQ();           /* prmtives.c, line 294 */
extern void     Prim_STRCAT();         /* prmtives.c, line 1756 */
extern void     Prim_STRCONTAINS();    /* prmtives.c, line 873 */
extern void     Prim_STRDECOMPOSE();   /* prmtives.c, line 873 */
extern void     Prim_STRINGP();        /* prmtives.c, line 1501 */
extern void     Prim_STRLEN();         /* prmtives.c, line 1999 */
extern void     Prim_STRSTARTS();      /* prmtives.c, line 998 */
extern void     Prim_STR_TO_INT();     /* prmtives.c, line 2906 */
extern void     Prim_SUBSTRING();      /* prmtives.c, line 2185 */
extern void     Prim_SYMBOLP();        /* prmtives.c, line 2269 */
extern void     Prim_SYM_TO_STR();     /* prmtives.c, line 2881 */
extern void     Prim_SYSTEM();         /* prmtives.c, line 3087 */
extern void     Prim_TERPRI();         /* prmtives.c, line 831 */
extern void     Prim_TIMES();          /* prmtives.c, line 1679 */
extern void     Prim_TRACE();
extern void     Prim_UCSTRING();       /* prmtives.c, line 3183 */
extern void     Prim_UNBIND();         /* prmtives.c, line 3270 */
extern void     Prim_UNBINDFN();       /* prmtives.c, line 3240 */
extern void     Prim_VERSION();        /* prmtives.c, line 3539 */
