/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* 
	doload.h - environment for dynamic loader

	Author:  Zalman Stern July 1989
 */



/* here is the state during a load */

typedef enum doload_mode_enum {
	Load,			/* .. loading .do file */
	List,			/* .. listing .do or .o file */
	Fix			/* .. converting .o to .do file */
} doload_mode;


struct doload_section {
    struct scnhdr header;
    char *contents;
};

struct doload_environment {
    doload_mode mode;		/* operating mode */
    int fd;			/* input file descriptor */
    jmp_buf errorJump;		/* error exit */
    int problems;		/* count of problems encountered */
    struct filehdr filehdr;		/* header at beginning of a.out file */
    struct aouthdr aouthdr; /* Where the entry point resides among other things. */
    char *text;			/* text segment */
    struct doload_section *sections;
    long totalsize;
    struct reloc *rtab; /* relocation table */
    HDRR symheader;
    pEXTR symtab;	/* symbol table */
    char *stringtab;		/* string table */
    pEXTR newsym;	/* replacement symbol table */
    int newsymcount;		/* number of new symbols */
};

extern int doload_trace;
#define doload_extension ".do"

extern void *(*doload() )();
