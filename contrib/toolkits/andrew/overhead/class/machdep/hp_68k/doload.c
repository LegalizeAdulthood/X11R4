/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* 
	doload.c - dynamic loader for class system

	Author:  John H Howard - April 4, 1987
 */

#include <stdio.h>
#include <andrewos.h>
#define a_syms a_lesyms 
#define relocation_info r_info
#include <a.out.h>
#include <setjmp.h>
#include <doload.h>

#include <aixfix.h>
#include <andrewos.h> /* sys/types.h */
#include <sys/stat.h>

char *malloc();
char *realloc();
long lseek();

int doload_trace=0;		/* nonzero if debugging */

#include "../common/safe.h"

/* initialize state */

void doload_setup(e, inFD, mode)
struct doload_environment *e;
int inFD;
doload_mode mode;
{
    e->mode = mode;
    e->fd = inFD;
    e->problems = 0;
    e->text = NULL;
    e->rtab = NULL;
    e->symtab = NULL;
    e->stringtab = NULL;
    e->newsym = NULL;
    e->newsymcount = 0;
    e->newsymsize = 0;
    e->stab_entries = NULL;
    e->alloced_stab_entries = 0;
    e->filled_stab_entries = 0;
    return;
}

/* tear down environment */

void doload_cleanup(e)
struct doload_environment *e;
{
    if (e->problems > 0) {
	e->problems = 0;
	doload_punt(e, "Errors while processing");
    }
    safe_free((char *)e->rtab);
    safe_free((char *)e->symtab);
    safe_free(e->stringtab);
    safe_free((char *)e->newsym);
    safe_free((char *)e->stab_entries);
    return ;
}

/* read module into memory */

doload_read(e)
struct doload_environment *e;
{
    long stringlen;	/* length of string table */

    /* read header */

    safe_read(e, (char *)&(e->header), (long)sizeof e->header);
    if (e->mode == List)
	printf( "\nHEADER\n  magic= %x\n  text = %x\n  data = %x\n\
  bss  = %x\n  syms = %x\n  entry= %x\n  trsize=%x\n  drsize=%x\n",
		e->header.a_magic, e->header.a_text, e->header.a_data,
		e->header.a_bss, e->header.a_syms, e->header.a_entry,
		e->header.a_trsize, e->header.a_drsize);
    if (N_BADMAG(e->header))
	doload_punt(e, "file not in loader format");

    /* read text plus data */
    /* Loaded files should be contiguous storage with internal sections packed one against the next.
     * There are sharable versions of HPUX a.out files that page-allign the text, data and pascal
     * sections. The Andrew Makefiles do not generate such files. This is a fundamental assumption
     * made by dofix and doload; in particular, dofix.c:FixIt and doload.c:doload_read. See the
     * include file /usr/include/a.out.h for definitions of TEXT_OFFSET, DATA_OFFSET and
     * MODCAL_OFFSET. 
     */

    /* read text plus data */

    e->text = safe_malloc(e, (long)(e->header.a_text + e->header.a_data + e->header.a_bss));
    e->data = e->text + e->header.a_text;
    safe_read(e, e->text, (long)(e->header.a_text + e->header.a_data));
    bzero(e->data + e->header.a_data, e->header.a_bss); /* bss storage is allocated at run time */

    /* We do not expect a PASCAL section. */
    if (e->header.a_pasint)
      doload_punt(e, "Unexpected Pascal data area.");

    /* read symbol table */

    e->symtab = (struct nlist *)safe_malloc(e, (long)e->header.a_syms);
    safe_read(e, (char *)e->symtab, (long)e->header.a_syms);

    /* Build local table of pointers to symtab entries.
       Needed to allow rapid access to symtab with embedded strings. */
    {
      register struct nlist *sp;
      register struct nlist *sbound;
      int alloc_increment = 32; /* allocate pointer table in chunks */

      sp = e->symtab;
      sbound = (struct nlist *)((char *)sp + e->header.a_syms);
      while (sp < sbound) {
	if (e->filled_stab_entries >= e->alloced_stab_entries) {
          e->alloced_stab_entries += alloc_increment;
	  e->stab_entries = (struct nlist **)safe_realloc(e,
							   (char *)e->stab_entries,
							   (e->alloced_stab_entries * sizeof *(e->stab_entries)));
	  bzero((char *)(e->stab_entries + e->filled_stab_entries),
		(alloc_increment * sizeof *(e->stab_entries)));
	}
	e->stab_entries[e->filled_stab_entries++] = sp;
	sp = nextSym(sp);
      }
    }

    /* read relocation information */

    if (e->header.a_trsize + e->header.a_drsize > 0) {
	long rsize;		/* size of relocation info */
	/* seek necessary to skip over debug info during dofix pass */
	if (lseek(e->fd, (long)RTEXT_OFFSET(e->header), 0) < 0)
	  doload_punt(e, "seek to relocation information failed");
	rsize = e->header.a_trsize + e->header.a_drsize;
	e->rtab = (struct relocation_info *)safe_malloc(e, rsize);
	safe_read(e, (char *)e->rtab, rsize);
    }
}

/* read and relocate module */
void *(* doload(inFD, name, bp, lenP, path) )()
				/* or NULL if error */
int inFD;			/* open fd for package file */
char *name;			/* name of package being loaded */
char **bp;			/* base address of package */
long *lenP;			/* size of text segment */
char *path;			/* Pathname of package being loaded */
				/* Path is used by the MACH loader, not this one */
{
    struct doload_environment E;
    register struct doload_environment *e;
    unsigned long n;	/* number of relocation items */
    struct relocation_info *rp;

    /* set up environment */

    doload_setup(e = &E, inFD, Load);
    if (setjmp(e->errorJump)) {
	doload_cleanup(e);
	return NULL;
    }

    /* read module into memory */

    doload_read(e);

    /* do relocation */

    if (e->header.a_syms)
	doload_preset(e);
    rp = e->rtab;
    for (n = (e->header.a_trsize)/(sizeof *rp); n > 0; n--, rp++) {
	doload_relocate(e, e->text + rp->r_address, rp);
    }
    for (n = (e->header.a_drsize)/(sizeof *rp); n > 0; n--, rp++) {
	doload_relocate(e, e->data + rp->r_address, rp);
    }

    /* all done */

    if (doload_trace)
	printf( " %s: text = 0x%.8x  data = 0x%.8x  entry = 0x%.8x\n",
		name, e->text, e->data, e->text + e->header.a_entry);

    if(bp!=NULL) *bp = e->text;
    if(lenP!=NULL) *lenP = e->header.a_text;

    doload_cleanup(e);

    return (void *(*)()) (e->text + e->header.a_entry);
}

extern struct globaltab {
    long entrypoint;	/* entry point value */
    char *entryname;	/* symbolic name */
} globals[];
extern long globalcount;

/* preset global symbols */

static char *symtypename[] = {"UNDF", "ABS ", "TEXT", "DATA", "BSS ", "????" };


char *RelocType(i)
int i;
{
    i &= N_TYPE;
    return symtypename[i <= 4 ? i : 5];
}

static char *rptypename[] = {"TEXT", "DATA", "BSS ", "REXT", "????" };

char *rpRelocType(i)
int i;
{
    return rptypename[i <= 3 ? i : 4];
}

doload_preset(e)
register struct doload_environment *e;
{
    register struct nlist *sp;
    register struct nlist *sbound;

    sp = e->symtab;
    sbound = (struct nlist *)((char *)sp + e->header.a_syms);

    for (; sp < sbound; sp = nextSym(sp)) { 
        char np_str[maxString];
	char *np = np_str;
	if (sp->n_length)
	  stabStrCpy(np, sp)
	else
	  np = "<<noname>>";
	if (e->mode == List) {
	    printf(" %.2x %.8x  %s %s %s\n", sp->n_type, sp->n_value,
		   RelocType(sp->n_type), ((sp->n_type & N_EXT) ? "EXT " : "    "), np);
	}
	else if ( SYM_TYPE(sp) == N_UNDF) {
	    register int i;

	    for (i = globalcount;
		 --i >= 0 && strcmp(globals[i].entryname, np) != 0; ) ;
	    if (i >= 0)
		sp->n_value = globals[i].entrypoint;
	    else if (sp->n_value > 0) {

                unsigned long length = sp->n_value;

		sp->n_value = (unsigned long)safe_malloc(e, length);
                bzero(sp->n_value, length);
	    }
	    else {
		fprintf(stderr, "doload:  Undefined symbol: %s\n", np);
		e->problems++;
	    }
	    sp->n_type = N_ABS + N_EXT;
	} /* endif N_UNDF */
    }
}

/* compute relocation adjustment */

long adjust(e, tw, rp, format)
register struct doload_environment *e;
register long tw;
register struct relocation_info *rp;
char *format;
{
    if (e->mode == List)
	printf("  %s", format);
    if (rp->r_segment == REXT) {
	register struct nlist *sp = e->stab_entries[rp->r_symbolnum];
	char np_str[maxString];
	char *np = np_str;
	if (sp->n_length)
	  stabStrCpy(np, sp)
	else
	  np="<<noname>>";

	if (e->mode == List) {
	    if (tw)
		printf("%x+", tw);
	}
	if ((sp->n_type & N_TYPE) == N_UNDF && e->mode == Load)
	    doload_punt(e, "Internal botch should have resolved in doload_preset");
	if (e->mode == List)
	    printf("%s=%x<%s>", np, sp->n_value, RelocType(sp->n_type));
	else {
	    tw += sp->n_value;
	    switch (sp->n_type & N_TYPE) {
	    case N_TEXT:
	    case N_DATA:
	    case N_BSS:
		tw += (long) e->text;
	    case N_ABS:
		break;
	    case N_UNDF:
		if (sp->n_type & N_EXT)
		    break;
	    default:
		fprintf(stderr, "doload:  Unknown relocation in symbol.\n");
		fprintf(stderr, "  reltab:  %.8x %.6x %.2x\n", rp->r_address, rp->r_symbolnum, *((char *)rp + 7));
		fprintf(stderr, "  symtab[%.6x]: %.2x %.4x %.8x %s\n", rp->r_symbolnum,	sp->n_type, sp->n_almod, sp->n_value, np);
		e->problems++;
	    }
	}
    } /* endif (rp->r_segment == REXT) */
    else {
	if (e->mode == List)
	    printf("%x<%s>", tw, rpRelocType(rp->r_segment));
	switch (rp->r_segment) {
	case RTEXT:
	case RDATA:
	case RBSS:
	    tw += (long) e->text;
	    break;
	case RNOOP:
	    break;    
	default:
	    doload_punt(e, "unknown symbol type");
	} /* end switch */
    } /* end else */
    return tw;
}

/* relocate one item */

doload_relocate(e, cp, rp)
register struct doload_environment *e;
register char *cp;
register struct relocation_info *rp;
{
    register long tw;

    switch (rp->r_length) {
    case RBYTE:	/* 1 byte */
	tw = *cp;
	tw = adjust(e, tw, rp, "(char)");
	if (e->mode == Load) {
	    if (tw > 255)
		doload_punt(e, "byte displacement overflow");
	    *cp = tw;
	}
	break;
    case RWORD:	/* 2 bytes */
	tw = *(short *)cp;
	tw = adjust(e, tw, rp, "(short)");
	if (e->mode == Load) {
	    if (tw < -32768 || tw > 32767)
		doload_punt(e, "short displacement overflow");
	    *(short *)cp = tw;
	}
	break;
    case RLONG:	/* 4 bytes */
	tw = *(long *)cp;
	tw = adjust(e, tw, rp, "(word)");
	if (e->mode == Load) {
	    *(long *)cp = tw;
	}
	break;
    default:
	doload_punt(e, "unknown relocation length");
    }
    return ;
}
