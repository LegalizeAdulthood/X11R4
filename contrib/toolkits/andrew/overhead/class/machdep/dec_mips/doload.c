/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* 
	doload.c - dynamic loader for class system

	Author:  Zalman Stern July 1989
 */

#include <stdio.h>
#include <a.out.h>
#include <setjmp.h>
#include <doload.h>
#include <mips/cachectl.h>

#include <andrewos.h> /* sys/types.h */

char *malloc();
char *realloc();
long lseek();

int doload_trace=0;		/* nonzero if debugging */

#include "../common/safe.h"

char *read_section(e, offset, buffer, length)
    struct doload_environment *e;
    long offset;
    char *buffer;
    long length;
{

    if (buffer == NULL) {
        buffer = (char *) safe_malloc(e, length);
    }
    safe_lseek(e, offset, L_SET);
    safe_read(e, buffer, length);
    return buffer;
}

extern struct globaltab {
    long entrypoint;	/* entry point value */
    char *entryname;	/* symbolic name */
} globals[];
extern long globalcount;

/* initialize state */
void doload_setup(e, inFD, mode)
struct doload_environment *e;
int inFD;
doload_mode mode;
{
    e->mode = mode;
    e->fd = inFD;
    e->problems = 0;
    e->sections = NULL;
    e->rtab = NULL;
    e->symtab = NULL;
    e->stringtab = NULL;
    e->newsym = NULL;
    e->newsymcount = 0;
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
    safe_free((char *)e->sections);
    safe_free((char *)e->rtab);
    safe_free((char *)e->symtab);
    safe_free(e->stringtab);
    safe_free((char *)e->newsym);
    return ;
}

struct doload_section_description {
    char *name;
    int read;
} sectionDescs[] = {
    _TEXT, 1,
    _RDATA, 1,
    _DATA, 1,
    _SDATA, 1,
    _SBSS, 0,
    _BSS, 0
};
    
/* read module into memory */

/* This routine reads all of the interesting sections of a object module into memory so they can be worked on by other parts of the code. Some of these sections are permanent while others are only sued during the loading phase and are freed afterwards. (List specific sections used by this code.) */
doload_read(e)
struct doload_environment *e;
{
    long stringlen;	/* length of string table */
    long totalSize = 0;
    long totalReadSize = 0;
    long totalRelocs = 0;
    int count;
    char *thisStart;

    /* read header */
    safe_read(e, (char *)&(e->filehdr), (long)sizeof e->filehdr);
    safe_read(e, (char *)&(e->aouthdr), (long)sizeof e->aouthdr);

    e->sections = (struct doload_section *) safe_malloc(e, e->filehdr.f_nscns *
                                                   sizeof(struct doload_section));
    bzero(e->sections, e->filehdr.f_nscns * sizeof(struct doload_section));

    for (count = 0; count < e->filehdr.f_nscns; count++) {

        struct scnhdr *tempSection = &e->sections[count].header;
        struct doload_section_description *thisDesc;

        safe_read(e, (char *)tempSection, (long)sizeof(*tempSection));
        for (thisDesc = sectionDescs; thisDesc < sectionDescs +
             sizeof(sectionDescs) && strcmp(tempSection->s_name,
                                            thisDesc->name) != 0; thisDesc++)
            ;

        if (thisDesc < sectionDescs +
             sizeof(sectionDescs)) {
            totalSize += tempSection->s_size;
            if (thisDesc->read) {
                totalReadSize += tempSection->s_size;
                totalRelocs += tempSection->s_nreloc;
            }
        }
        else if (tempSection->s_size != 0) {

            char buffer[200];

            sprintf(buffer, "doload: Unexpected non-zero section named %.100s encountered.", tempSection->s_name);
            doload_punt(e, buffer);
        }
    }

    if (strcmp(e->sections[0].header.s_name, _TEXT) != 0 ||
        e->sections[0].header.s_size == 0)
        doload_punt(e, "Missing or empty text section!");


/* This code is wrong if the sizes don't include the rounding. In that *
case, one should use bssscnhdr.s_scnptr - textscnhdr.scnptr + *
bssscnhdr.s_size */
    e->text = safe_malloc(e, totalSize);
    e->totalsize = totalSize;
    e->text = read_section(e, e->sections[0].header.s_scnptr, e->text, totalReadSize);
    bzero(e->text + totalReadSize, totalSize - totalReadSize);

    thisStart = e->text;
    for (count = 0; count < e->filehdr.f_nscns; count++) {
        e->sections[count].contents = thisStart;
        thisStart += e->sections[count].header.s_size;
    }


/* Perhaps the reloactions should be read into two different areas? */
    e->rtab = (struct reloc *) read_section(e, e->sections[0].header.s_relptr, NULL,
                                            totalRelocs * sizeof(struct reloc));

    read_section(e, e->filehdr.f_symptr, &e->symheader, sizeof(e->symheader));

    e->symtab = (pEXTR) read_section(e, e->symheader.cbExtOffset, NULL, e->symheader.iextMax * sizeof(EXTR));
    e->stringtab = (char *) read_section(e, e->symheader.cbSsExtOffset, NULL, e->symheader.issExtMax);
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
    struct reloc *rp;
    int count;

    /* set up environment */

    doload_setup(e = &E, inFD, Load);
    if (setjmp(e->errorJump)) {
	doload_cleanup(e);
	return NULL;
    }

    /* read module into memory */

    doload_read(e);

    /* do relocation */

    if (e->symheader.iextMax != 0)
        doload_preset(e);

/* Adjust virtual addresses in relocation records to be zeroed where the text
 * segment was actually loaded into this process' VM.
 */ 
    rp = e->rtab;

    for (count = 0; count < e->filehdr.f_nscns; count++) {
        struct reloc *startRp = rp;
        long sectionOffset = e->sections[count].header.s_vaddr - (long)
            e->sections[count].contents;

        for (; rp < startRp + e->sections[count].header.s_nreloc; rp++) {
            rp->r_vaddr = rp->r_vaddr - sectionOffset;
        }

        rp = startRp;
        for (rp = startRp; rp < startRp + e->sections[count].header.s_nreloc; rp++) {
            doload_relocate(e, rp->r_vaddr, rp, startRp + e->sections[count].header.s_nreloc);
        }
    }

    /* all done */

    if (doload_trace)
	printf( " %s: text = 0x%.8x  data = 0x%.8x  entry = 0x%.8x\n",
		name, e->text, e->sections[1].contents, e->text +
               e->aouthdr.entry - e->aouthdr.text_start);

    if(bp != NULL)
        *bp = e->text;
    if(lenP != NULL)
        *lenP = e->totalsize;

#if 0
    if (cacheflush(e->text, e->totalsize, BCACHE) != 0) {
#else
/* Ultrix 3.0 does not have cacheflush in libc.a, however, the kernel on my
 * machine implements it and it is needed to make doindex work...
 */
#include <sys/syscall.h>
#include <sys/sysmips.h>

    if (syscall(SYS_sysmips, MIPS_CACHEFLUSH, e->text, e->totalsize,
                BCACHE, 0) == -1) {
#endif
        perror("doload - cacheflush");
        doload_punt(e, "Could not flush caches in doload.");
    }

    doload_cleanup(e);

    return (void *(*)()) (e->text + e->aouthdr.entry - e->aouthdr.text_start);
}

/* Lookup table used to give useful information in error messages. This
 * table comes from /usr/include/symconst.h on an Ultrix 3.0 system.
 */
static char *doload_symbolClasses[] = {
    "scNil",
    "scText",	/* text symbol */
    "scData",	/* initialized data symbol */
    "scBss",	/* un-initialized data symbol */
    "scRegister",	/* value of symbol is register number */
    "scAbs",	/* value of symbol is absolute */
    "scUndefined",	/* who knows? */
    "scCdbLocal",	/* variable's value is IN se->va.?? */
    "scBits",	/* this is a bit field */
    "scCdbSystem or scDbx",    /* variable's value is IN CDB's address space */
                               /* overlap dbx internal use */
    "scRegImage",	/* register value saved on stack */
    "scInfo",	/* symbol contains debugger information */
    "scUserStruct",	/* address in struct user for current process */
    "scSData",	/* load time only small data */
    "scSBss",	/* load time only small common */
    "scRData",	/* load time only read only data */
    "scVar",	/* Var parameter (fortran,pascal) */
    "scCommon",	/* common variable */
    "scSCommon",	/* small common */
    "scVarRegister",	/* Var parameter in a register */
    "scVariant",	/* Variant record */
    "scSUndefined",	/* small undefined(external) data */
    "scInit",	/* .init section symbol */
    "scMax",
};

/* This routine rolls down the symbol table and finds those symbols wich */
 /* also appear in the globals array. When it finds one, it fills in the */
 /* value with that from globals. If there are any undefined symbols left */
 /* after this routine executes, its an error. */
doload_preset(e)
register struct doload_environment *e;
{
    register pEXTR sp;
    register pEXTR sbound;

    for (sp = e->symtab; sp < e->symtab + e->symheader.iextMax; sp++) {

        char *np = e->stringtab + sp->asym.iss;


/* DO list stuff? */

        if (e->mode == List)
            printf("List mode is not supported");
	else if (sp->asym.sc == scUndefined || sp->asym.sc == scCommon || sp->asym.sc == scSCommon) {

            struct globaltab *thisGlobal;

	    for (thisGlobal = globals;
		 thisGlobal < globals + globalcount && strcmp(thisGlobal->entryname, np) != 0; thisGlobal++ )
                ;
	    if (thisGlobal < globals + globalcount)
		sp->asym.value = thisGlobal->entrypoint;
	    else if (sp->asym.sc == scCommon && sp->asym.value > 0) {

                unsigned long length = sp->asym.value;

		sp->asym.value = (unsigned long)safe_malloc(e, length);
                bzero(sp->asym.value, length);
	    }
	    else {
		fprintf(stderr, "doload:  Undefined symbol: %s\n", np);
		e->problems++;
	    }
            sp->asym.sc = scAbs;
        }
    }
}

/* compute relocation adjustment */

long adjust(e, tw, rp, format)
register struct doload_environment *e;
register long tw;
register struct reloc *rp;
char *format;
{
    if (e->mode == List)
	printf("  %s", format);
    if (rp->r_extern) {

        register pEXTR sp = e->symtab + rp->r_symndx;
        char *np = e->stringtab + sp->asym.iss;

	if (e->mode == List) {
	    if (tw)
		(void) printf("%x+", tw);
	}
	if (sp->asym.sc == scUndefined || sp->asym.sc == scCommon && e->mode == Load)
	    doload_punt(e,
		"Internal botch - should have resolved in doload_preset");
	if (e->mode == List)
	    (void) printf( "%s=%x<%s>", np, sp->asym.value,
                    doload_symbolClasses[sp->asym.sc]);
	else {
	    tw += sp->asym.value;
	    switch (sp->asym.sc) {
                case scAbs:
                    break;
                case scUndefined:
                    doload_punt(e, "Encountered an scUndefined in doload_adjust.\n");
                    break;
	    default:
		fprintf(stderr, "doload:  Unknown relocation in symbol.\n");
                fprintf(stderr, "    Symbol name: %s; symbol class %s\n", np, doload_symbolClasses[sp->asym.sc]);
		e->problems++;
	    }
	}
    } /* endif IS_RP_EXTERN( rp ) */
    else {
	switch (rp->r_symndx) {
	    case R_SN_TEXT:
            case R_SN_RDATA:
	    case R_SN_DATA:
            case R_SN_SDATA:
	    case R_SN_BSS:
            case R_SN_SBSS:
		tw += (long) e->text;
		break;
	    default:
		doload_punt(e, "unknown local (not external) relocation type");
	} /* end switch */
    } /* end else */
    return tw;
}

/* relocate one item */

#define HI16MASK    0xffff0000
#define LOW16MASK   0x0000ffff
#define LOW26MASK       0x03ffffff
#define HI6MASK     0xfc000000

doload_relocate(e, cp, rp, sectionRelocBound)
register struct doload_environment *e;
register char *cp;
register struct reloc *rp;
struct reloc *sectionRelocBound; /* Used for bounds checking below. */
{
    register long tw;
    static int prevType = 0;
/* Assumes maximum nuber of sections is 16. */
    static short lastHighStart[16];
    static short lastHighResult[16];
    static short lastExternHighStart;
    static short lastExternHighResult;
    static long  lastExternSymndx;

    switch (rp->r_type) {
        case R_REFHALF:
            tw = *(short *)cp;
            tw = adjust(e, tw, rp, "(short)");
            *(short *)cp = tw;
            break;
        case R_REFWORD:
            tw = *(long *)cp;
            if (!rp->r_extern)
                tw -= e->sections[0].header.s_vaddr;
            tw = adjust(e, tw, rp, "(long)");
            *(long *)cp = tw;
            break;
        case R_JMPADDR:
            tw = (*(long *)cp & LOW26MASK) << 2;
            tw = adjust(e, tw, rp, "(R_JMPADDR)");
            if (tw & HI6MASK != 0)
                doload_punt(e, "JUMPADDR relocation overflow.");
            *(long *)cp = (*(long *)cp & HI6MASK) + ((tw & LOW26MASK) >> 2);
            break;
        case R_REFHI:
            if (rp < sectionRelocBound) {

                struct reloc *nextRp = rp + 1;

                if (nextRp->r_type == R_REFLO) {

                    short highStart; /* Temporary value */
                    short *lastHighResultPtr; /* Points to place to tuck */
                                              /* lastHighResult */
                    short highHalf; /* Temporary for high 16 bits of final value. */
                    short lowHalf; /* Temporary for low 16 bits of final value. */

/* I assume that the low half of this will be appropriately sign extended. */
#ifdef MIPSEB /* BIG ENDIAN */
                    tw = (*(((short *)rp->r_vaddr) + 1) << 16) +
                         *(((short *)nextRp->r_vaddr) + 1);
                    highStart = *(((short *)rp->r_vaddr) + 1);
#else /* LITTLE ENDIAN */
                    tw = (*(((short *)rp->r_vaddr)) << 16) +
                         *(((short *)nextRp->r_vaddr));
                    highStart = *(((short *)rp->r_vaddr));
#endif /* Byte order selection */

                    if (!rp->r_extern) {
                        tw -= e->sections[0].header.s_vaddr;

                        if (rp->r_symndx < sizeof(lastHighStart)) {
                            lastHighStart[rp->r_symndx] = highStart;
                            lastHighResultPtr = &lastHighResult[rp->r_symndx];
                        }
                        else
                            doload_punt(e, "Encountered section number higher than MAX_SECTION in doload_relocate.(1)");
                    }
                    else {
/* I doubt this code ever gets used, but I don't know... */
                        lastExternHighStart = highStart;
                        lastExternSymndx = rp->r_symndx;
                        lastHighResultPtr = &lastExternHighResult;
                    }

                    tw = adjust(e, tw, rp, "(R_REFHI/R_REFLOW)");
                    lowHalf = tw & LOW16MASK;
                    highHalf = tw >> 16;
                    if (lowHalf < 0)
                        ++highHalf;
#ifdef MIPSEB /* BIGENDIAN */
                    *(((short *)rp->r_vaddr) + 1) = highHalf;
                    *(((short *)nextRp->r_vaddr) + 1) = lowHalf;
#else /* LITTLE ENDIAN */
                    *(((short *)rp->r_vaddr)) = highHalf;
                    *(((short *)nextRp->r_vaddr)) = lowHalf;
#endif /* Byte order selection */
                    *lastHighResultPtr = highHalf;
                    break;
                }
            }
            doload_punt(e, "R_REFHI relocation record not followed by REFLO.");
            break;
        case R_REFLO:
/* Apparently, sometimes relocation records don't come in pairs. This code
 * assumes that the only time this happens is when there are multiple references
 * to very close addresses. (In one particular case I found, this
 * happens when loading a double which requires two load instructions. A
 * REFHI is only emitted for the first one apparently.)
 * This code simply checks to see if this REFLO will work with
 * the REFHI of the last REFHI/REFLO pair and if so it puts in the result
 * value that was computed for the last pair.
 *
 * In actuality, things are more complicated than the above indecates. One
 * must pair the REFLO with the last REFHI from the same section. This hairs
 * up the code a great deal.
 *
 * Incidentally, if anyone from MIPS reads this, this is about the point where I
 * started re-evaluating my admiration of the R2000/R3000 and accompanying
 * software :-)
 */

            if (prevType != R_REFHI) {

                long tw;
                short highStart;
                short highResult;
                short highHalf;
                short lowHalf;

                if (!rp->r_extern) {
                    if (rp->r_symndx < sizeof(lastHighStart)) {
                        highStart = lastHighStart[rp->r_symndx];
                        highResult = lastHighResult[rp->r_symndx];
                    }
                    else
                        doload_punt(e, "Encountered section number higher than MAX_SECTION in doload_relocate.(2)");
                }
                else {
                    if (rp->r_symndx == lastExternSymndx) {
                        highStart = lastExternHighStart;
                        highResult = lastExternHighResult;
                    }
                    else
                        doload_punt(e, "REFLO to an external symbol with different symbol index than last extern REFHI.");
                }

#ifdef MIPSEB /* BIG ENDIAN */
                tw = (highStart << 16) + *(((short *)rp->r_vaddr) + 1);
#else /* LITTLE ENDIAN */
                tw = (highStart << 16) + *(((short *)rp->r_vaddr));
#endif /* Byte order selection */


                if (!rp->r_extern)
                    tw -= e->sections[0].header.s_vaddr;
                tw = adjust(e, tw, rp, "(R_REFHI/R_REFLOW)");
                lowHalf = tw & LOW16MASK;
                highHalf = tw >> 16;
                if (lowHalf < 0)
                    ++highHalf;
                if (highHalf == highResult) {
#ifdef MIPSEB /* BIGENDIAN */
                    *(((short *)rp->r_vaddr) + 1) = lowHalf;
#else /* LITTLE ENDIAN */
                    *(((short *)rp->r_vaddr)) = lowHalf;
#endif /* Byte order selection */
                }
                else
                    doload_punt(e, "R_REFLO relocation record not immediately preceeded by REFHI.\n Situation cannot be resolved by using last REFHI.");
            }
            break;
        case R_GPREL:
            if (!rp->r_extern)
                doload_punt(e, "Non-external R_GPREL relocation type. Were all modules compiled with -G 0?");
            else {

                extern int _gp; /* MIPS' defines this label at the address in the gp register */
                long gpOffset;
                short lowHalf;

#ifdef MIPSEB /* BIG ENDIAN */
                tw = *(((short *)rp->r_vaddr) + 1);
#else /* LITTLE ENDIAN */
                tw = *(((short *)rp->r_vaddr));
#endif /* Byte order selection */
                tw = adjust(e, tw, rp, "(External R_GPREL)");
                gpOffset = (long)&_gp - tw;
                if (gpOffset > 32767 || gpOffset < -32768)
                    doload_punt(e, "External R_GPREL relocation does not result in a gp addresable value.");
                lowHalf = (short)gpOffset;
#ifdef MIPSEB /* BIGENDIAN */
                *(((short *)rp->r_vaddr) + 1) = lowHalf;
#else /* LITTLE ENDIAN */
                *(((short *)rp->r_vaddr)) = lowHalf;
#endif /* Byte order selection */
            }
            break;
        case R_LITERAL:
            doload_punt(e, "R_LITERAL relocation in doload_relocate.");
            break;
    }

    prevType = rp->r_type;
}
