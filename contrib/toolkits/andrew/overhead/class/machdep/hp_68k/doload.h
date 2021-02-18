/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* 
	doload.h - environment for dynamic loader

	Author:  John H Howard - April 9, 1987
 */



/* here is the state during a load */

#define nlist nlist_

typedef enum doload_mode_enum {
	Load,			/* .. loading .do file */
	List,			/* .. listing .do or .o file */
	Fix			/* .. converting .o to .do file */
} doload_mode;

struct doload_environment {
    doload_mode mode;		/* operating mode */
    int fd;			/* input file descriptor */
    jmp_buf errorJump;		/* error exit */
    int problems;		/* count of problems encountered */
    struct exec header;		/* header at beginning of a.out file */
    char *text;			/* text segment */
    char *data;			/* data segment */
    struct reloc *rtab;		/* relocation table */
    struct syment *symtab;	/* symbol table */
    char *stringtab;		/* string table */
    struct syment *newsym;	/* replacement symbol table */
    int newsymcount;		/* number of new symbols */
    int newsymsize;		/* size of new symbol table */
    struct nlist **stab_entries; /* list of pointers to symbol table entries */
    int alloced_stab_entries,     /* number of slots malloced */
        filled_stab_entries;      /* number of slots filled */
};



extern int doload_trace;

#define doload_extension ".do"

extern void *(*doload() )();

#define maxString 256 /* max size string expected in a symbol table entry */
#define symSize(pSym) \
  (sizeof *pSym + pSym->n_length) /* struct size + string length (NO TRAILING ZERO!) */
#define nextSym(pSym) \
  (struct nlist *)((char *)pSym + symSize(pSym))
#define symString(pSym) \
  ((char *)pSym + sizeof *pSym)
#define stabStrCmp(str, pSym) \
  ((pSym->n_length == strlen(str)) ? \
   strncmp(symString(pSym), str, pSym->n_length) \
   : -1)
#define stabStrCpy(str, pSym) \
  { \
  bcopy(symString(pSym), str, pSym->n_length); \
  str[pSym->n_length] = '\0'; \
  }

