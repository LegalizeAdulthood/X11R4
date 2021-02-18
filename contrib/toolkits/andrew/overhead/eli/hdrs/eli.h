/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/*
 * $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/eli/hdrs/RCS/eli.h,v 2.12 89/07/07 12:30:08 bobg Exp $
 *
 * $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/eli/hdrs/RCS/eli.h,v $
 */

#include  <stdio.h>
#include  <ctype.h>
#include  <fdplumb.h>

#define MAJORVERSION    (4)
#define MINORVERSION    (0)

#define IN                             /* Someday I'll use these to label
                                        * params in */
#define OUT                            /* function declarations */
#define INOUT

#define DQUOTE          '\"'
#define BACKSLASH       '\\'
#define LEFTPAREN       '('
#define RIGHTPAREN      ')'
#define PARSESTRING     1
#define UNPARSESTRING   2

#define ELI_ERR_CLIENT      0x0
#define ELI_ERR_OUT_OF_MEM  0x1
#define ELI_ERR_BAD_SYNTAX  0x2
#define ELI_ERR_UNBOUND     0x4
#define ELI_ERR_FN_UNDEF    0x8
#define ELI_ERR_BAD_PARAM   0x10
#define ELI_ERR_BAD_ARGS    0x20
#define ELI_ERR_NOSYM       0x40
#define ELI_ERR_ARGLISTSIZE 0x80
#define ELI_ERR_SYSERROR    0x100
#define ELI_ERR_USERERROR   0x200

#define BUCKETNODE_BLOCKSIZE    128    /* Arbitrary */
#define CONS_BLOCKSIZE  128
#define STR_BLOCKSIZE   128
#define SYM_BLOCKSIZE   128
#define NODE_BLOCKSIZE  128
#define FNNODE_BLOCKSIZE    128
#define NUMBUCKETS  64

#define GROW_STACK_SIZE 64

#define BITS_PER_BYTE 8

#define NUM_DEBUG_ENTRIES 64           /* Also arbitrary */

#ifndef TRUE
#define TRUE (1)
#define FALSE (0)
#endif                                 /* TRUE */

#include  <etypes.h>
#include  <elifns.h>
#include  <eglbvars.h>

#ifdef DEBUG
#define D(x)    x
#else                                  /* DEBUG */
#define D(x)
#endif                                 /* DEBUG */

extern int errno;
