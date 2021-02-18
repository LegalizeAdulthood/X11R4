/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/dump.c,v 1.11 89/09/17 08:54:39 wjh Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/dump.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/dump.c,v 1.11 89/09/17 08:54:39 wjh Exp $";
#endif

/* dump.c
	debugging output for ness environment

	Entry points:

	dumpStack(file) - dumps the stack to the indicated FILE
	dumpFuncDef(file, f) - dump a function definition
	dumpAttrList(file, attrs) - dump each definition on attribute list

	ds() == dumpStack(stdout)   (for gdb)
	da() == dumpAttrList(stdout, *curNess->AttrDest)    (for gdb)
*/

/*
 * $Log:	dump.c,v $
 * Revision 1.11  89/09/17  08:54:39  wjh
 * avoid crash when disabled event encountered
 * 
 * Revision 1.10  89/09/03  22:47:15  wjh
 * newness
 * 
 * Revision 1.9  89/06/01  15:59:19  wjh
 * campus release version
 * 
 * Revision 1.4  88/12/20  19:45:32  wjh
 * fixed various bugs
 * 
 * Revision 1.3  88/12/07  22:42:24  wjh
 * 
 * 9 Nov
 * implemented access level control
 * skip first line of script if starts with # (for shell script)
 * changed so the name of all initialization functions is init()
 * added ness-load
 * moved execution of init() from compilation to first execution
 * deferred compilation to first FullUpdate time
 * 
 * 22 Nov
 * proc table calls now work correctly with type-free procs  (the first arg can be anything)
 * added "cheat_" functions which will remain undocumented
 * changed inset() to a function
 * fixed some bugs in initial access
 * 
 * 25 November
 * added long strings
 * added Next Error menu option
 * made replace() work correctly in all cases
 * added class() and new()
 * 
 * 29 Nov
 * added ^<upper-case> and \e as characters in strings
 * added nextn() and length()
 * 
 * 6 Dec
 * added functions: parseint(), parsereal(), firstobject(), whereitwas(), replacewithobject(), addstyles(), nextstylegroup(), enclosingstylegroup(), clearstyles(), hasstyles()
 * catch bus and segmentation errors
 * 
 * 
 * Revision 1.2  88/11/02  14:41:05  wjh
 * fixed bugs with byte swapping
 * added some more corrected copyrights
 * 
 * Revision 1.1  88/10/21  10:56:53  wjh
 * Initial revision
 * 
 * Creation 0.0  88/05/09 19:57:00  wjh
 * Initial creation by WJHansen
 * 
*/

#include <stdio.h>
#include <class.h>
#include <nodeclss.h>
#include <nessmark.ih>
#include <xgetchar.h>
#include <proctbl.ih>
#include <nesssym.ih>

#include <interp.h>
#include <envt.h>
#include <call.h>
#include <gen.h>		/* for curNess, for da */
#include <nesscomp.h>
#include <compdefs.h>
#include <nevent.h>


	static void
printstaddr(f, a, NSP)
	FILE *f;
	union stackelement *a;
	union stackelement *NSP;
{
	long v = (unsigned long)a-(unsigned long)NSP;
	if (a == NULL)
		fprintf(f, "NULL    ");
	else if (v < -100  || v > 1000)
		fprintf(f, "0x%lx", a);
	else 
		fprintf (f, "NSP%-+5ld", v);
}

	void
dumpStack(f, NSP) 
	FILE *f;
	union stackelement *NSP;
{
	union stackelement *tsp, *newtsp;
	struct basicobject *bo;
	struct basicobject_methods *bom;
	long i, pos, len;
	struct nessmark *m;
	struct simpletext *t;
	long size;

	fprintf(f, "Stack bounds: 0x%lx ... 0x%lx    NSP: 0x%lx\n",
		NSLowEnd, NSHiEnd, NSP);
	fprintf(f, "FramePtr: ");
	printstaddr(f, (union stackelement *)FramePtr, NSP);
	fputc('\n', f);
	for (tsp = NSP-3; tsp < NSHiEnd; ) {
		printstaddr(f, tsp, NSP);  
		switch (tsp->l.hdr) {
		    case (longHdr):
			fprintf(f, ":  long   0x%lx  %d\n", tsp->l.v, tsp->l.v);
			size = sizeof(struct longstkelt);
			break;
		    case (boolHdr):
			fprintf(f, ":  bool   0x%lx  %d\n", tsp->b.v, tsp->b.v);
			size = sizeof(struct boolstkelt);
			break;
		    case (dblHdr):
			fprintf(f, ":  dbl    %10g\n", tsp->d.v);
			size = sizeof(struct dblstkelt);
			break;
		    case (ptrHdr):
			fprintf(f, ":  ptr    0x%lx  %d", tsp->p.v);
			bo = tsp->p.v;
			if (bo && (bom=bo->methods)
					&& bom->info
					&& bom->info->name)
				fprintf(f, "   (%s)", bom->info->name);
			fputc('\n', f);
			size = sizeof(struct ptrstkelt);
			break;
		    case (frameHdr):
			fprintf(f, ":  frame  ret %d   prev ", tsp->f.returnaddress);
			printstaddr(f, (union stackelement *)tsp->f.prevframe, NSP);
			fputc('\n', f);
			size = sizeof(struct frameelt);
			break;
		    default:
		    	if (tsp->l.hdr == nessmarkHdr) {
				fprintf(f, ":  mark   ");
				m = &tsp->m;
				t = nessmark_GetText(m);
				printstaddr(f, 
					(union stackelement *)nessmark_GetNext(m), 
					NSP);
				pos = nessmark_GetPos(m);
				len = nessmark_GetLength(m);
				fprintf(f, "  0x%lx[%d,%d]  ", t, pos, len);
				if (tsp >= NSP) {
					fprintf(f, "\"");
					for (i = 0; i < 20 && i < len; i++)
						fputc(simpletext_CorrectGetChar(t, pos+i), f);
					fprintf(f, "%s", (i < len) ? "...\"\n" : "\"\n");
				}
				else fprintf(f, "(%d,%d)\n", pos, len);
				size = sizeof(struct nessmark);
			}
			else {
				/* UNKNOWN stack element */
				fprintf(f, ":  other  0x%lx  %d\n", tsp->l.hdr, tsp->l.hdr);
				size = sizeof(long);
			}
			break;
		}
		newtsp = (union stackelement *) (((unsigned long)tsp) + size);
		if (tsp < NSP && newtsp > NSP)
			tsp = NSP;
		else	tsp = newtsp;
	}
	if (tsp != NSHiEnd)
		fprintf(f, "Stack not sychronized at bottom.\n");
}



static struct {
	char *name;
	enum addrtype {none, stack, sysmark, branch, call, callnode, search, 
			event, realunary, realbinary, current, cheat, cheatcall} 
		randcode;
}
	opTbl[] = {		/* first operator is for char ' ' */
		{"ERR' '", none},		/* ' ' */
		{"ERR'!'", none},		/* '!' */
		{"ERR'\"'", none},		/* '\"' */
		{"ERR'#'", none},		/* '#' */
		{"ERR'$'", none},		/* '$' */
		{"%", none},		/* '\%' */
		{"ERR'&'", none},		/* '&' */
		{"ERR'\''", none},		/* '\'' */
		{"ERR'('", none},		/* '(' */
		{"ERR')'", none},		/* ')' */
		{"*", none},		/* '*' */
		{"+", none},		/* '+' */
		{"ERR','", none},		/* ',' */
		{"-", none},		/* '\-' */
		{"ERR'.'", none},		/* '.' */
		{"/", none},		/* '/' */
		{"ld0", none},		/* '0' */
		{"TRUE", none},		/* '1' */
		{"ERR'2'", none},		/* '2' */
		{"ERR'3'", none},		/* '3' */
		{"ERR'4'", none},		/* '4' */
		{"ERR'5'", none},		/* '5' */
		{"ERR'6'", none},		/* '6' */
		{"ERR'7'", none},		/* '7' */
		{"ERR'8'", none},		/* '8' */
		{"FALSE", none},		/* '9' */
		{"ERR':'", none},		/* ':' */
		{"ERR';'", none},		/* ';' */
		{"ERR'<'", none},		/* '<' */
		{"ERR'='", none},		/* '=' */
		{"ERR'>'", none},		/* '>' */
		{"ERR'?'", none},		/* '?' */
		{"ERR'@'", none},	/* '@' */
		{"append", none},		/* 'A' */
		{"bool", none},		/* 'B' */
		{"", callnode},		/* 'C' */
		{"", cheatcall},		/* 'D' */
		{"deftext", none},		/* 'E' */
		{"", search},		/* 'F' */
		{"ERR'G'", none},		/* 'G' */
		{"", realunary},		/* 'H' */
		{"int", none},		/* 'I' */
		{"", cheat},		/* 'J' */
		{"ERR'K'", none},		/* 'K' */
		{"txtimg", none},		/* 'L' */
		{"", realbinary},		/* 'M' */
		{"\\n", none},		/* 'N' */
		{"call", call},		/* 'O' */
		{"enter", stack},		/* 'P' */
		{"exit", stack},		/* 'Q' */
		{"real", none},		/* 'R' */
		{"", current},		/* 'S' */
		{"cmpT", none},		/* 'T' */
		{"", event},		/* 'U' */
		{"ptr", none},		/* 'V' */
		{"wrtfile", none},		/* 'W' */
		{"system", none},		/* 'X' */
		{"setsel", none},		/* 'Y' */
		{"last", none},		/* 'Z' */
		{"ERR'['", none},		/* '[' */
		{"ERR'\\'", none},		/* '\\' */
		{"ERR']'", none},		/* ']' */
		{"NULL", none},		/* '^' */
		{"neg", none},		/* '_' */
		{"ERR'`'", none},		/* '\`' */
		{"lt", branch},		/* 'a' */
		{"gt", branch},		/* 'b' */
		{"le", branch},		/* 'c' */
		{"ge", branch},		/* 'd' */
		{"eq", branch},		/* 'e' */
		{"ne", branch},		/* 'f' */
		{"goto", branch},		/* 'g' */
	  	{"brerr", branch},		/* 'h' */
		{"readf", none},		/* 'i' */
		{"print", none},		/* 'j' */
		{"ldsys", sysmark},	/* 'k' */
		{"ldstk", stack},		/* 'l' */
		{"dup", none},		/* 'm' */
		{"next", none},		/* 'n' */
		{"start", none},		/* 'o' */
		{"base", none},		/* 'p' */
		{"newbs", none},		/* 'q' */
		{"replace", none},		/* 'r' */
		{"ststk", stack},		/* 's' */
		{"strcmp", none},		/* 't' */
		{"eqempty", none},	/* 'u' */
		{"stsys", sysmark},	/* 'v' */
		{"prev", none},		/* 'w' */
		{"extent", none},		/* 'x' */
		{"pop", none},		/* 'y' */
		{"swap", none},		/* 'z' */
		{"ERR'{'", none},		/* '{' */
		{"ERR'|'", none},		/* '|' */
		{"ERR'}'", none},		/* '}' */
		{"ERR'~'", none},		/* '~' */
		{"ERR'\\177'", none},	/* '\177' */
};
struct srchtblelt {
	unsigned char code;
	char *name;
};

static struct srchtblelt 
searchTbl[] = {
	{'a', "search"},
	{'b', "match"},
	{'c', "anyof"},
	{'d', "span"},
	{'e', "token"},
	{'j', "AddStyle"},
	{'k', "HasStyles"},
	{'s', "NextStyleGroup"},
	{'t', "EnclosingStyleGroup"},
	{'u', "ClearStyles"},
	{'p', "ParseInt"},
	{'q', "ParseReal"},
	{'r', "FirstObj"},
	{'w', "WhereItWas"},
	{'x', "ReplaceWithObj"},
	{'\0', "*Unknown Search Code*"}
};
static struct srchtblelt 
eventTbl[] = {
	{'a', "evLdn"},
	{'b', "evLup"},
	{'c', "evLmv"},
	{'d', "evRdn"},
	{'e', "evRup"},
	{'f', "evRmv"},
	{'k', "evK"},
	{'m', "evMenu"},
	{'n', "valgV"},
	{'o', "valgSz"},
	{'p', "valgStr"},
	{'q', "valgA"},
	{'r', "valsV"},
	{'s', "valsSz"},
	{'t', "valsStr"},
	{'u', "valsA"},
	{'v', "valsNt"},
	{'w', "evMAct"},
	{'x', "evMx"},
	{'y', "evMy"},
	{'F', "evIF"},
	{'H', "DoHit"},
	{'I', "evCurInset"},
	{'R', "evIsR/O"},
	{'T', "evTell"},
	{'U', "evAsk"},
	{'\0', "*Unknown Event Code*"}
};
static struct srchtblelt 
realUnTbl[] = {
	{'a', "acos"},
	{'b', "acosh"},
	{'c', "asin"},
	{'d', "asinh"},
	{'e', "atan"},
	{'f', "atanh"},
	{'g', "cbrt"},
	{'i', "cos"},
	{'j', "cosh"},
	{'k', "erf"},
	{'l', "erfc"},
	{'m', "exp"},
	{'n', "expm1"},
	{'o', "fabs"},
	{'r', "j0"},
	{'s', "j1"},
	{'t', "lgamma"},
	{'u', "log"},
	{'v', "log10"},
	{'w', "log1p"},
	{'x', "pow"},
	{'y', "sin"},
	{'z', "sinh"},
	{'A', "sqrt"},
	{'B', "tan"},
	{'C', "tanh"},
	{'D', "y0"},
	{'E', "y1"},
	{'_', "-"},
	{'\0', "*Unknown real Unary Code*"}
};
static struct srchtblelt 
realBinTbl[] = {
	{'+', "R+"},
	{'-', "R-"},
	{'*', "R*"},
	{'/', "R/"},
	{'a', "atan2"},
	{'b', "hypot"},
	{'c', "round"},
	{'d', "floor"},
	{'e', "ceil"},
	{'f', "isnan"},
	{'g', "finite"},
	{'j', "float"},
	{'k', "jn"},
	{'l', "yn"},
	{'\0', "*Unknown real Binary Code*"}
};

static struct srchtblelt 
currentTbl[] = {
	{'m', "curmark"},
	{'s', "cursel"},
	{'\0', "*Unknown \"current\" Code*"}
};
static struct srchtblelt 
cheatcallTbl[] = {
	{'a', "CHcMeth "},
	{'b', "CHcProc "},
	{'\0', "*Unknown \"cheatcall\" Code*"}
};
static struct srchtblelt 
cheatTbl[] = {
	{'a', "CHgL"},
	{'b', "CHgD"},
	{'c', "CHgStr"},
	{'d', "CHgCh"},
	{'e', "CHgPtr"},
	{'f', "CHgB"},
	{'g', "CHsL"},
	{'h', "CHsD"},
	{'i', "CHsSP"},
	{'j', "CHsChA"},
	{'k', "CHsStrB"},
	{'l', "CHsCh"},
	{'m', "CHsPtr"},
	{'n', "CHsB"},
	{'q', "forceupdate"},
	{'r', "inset"},
	{'s', "new"},
	{'t', "class"},
	{'\0', "*Unknown \"cheat\" Code*"}
};

	static void 
PrintFromTbl(file, text, i, Tbl)
	FILE *file;
	register struct simpletext *text;
	register long i;
	struct srchtblelt *Tbl;
{
	register unsigned char c = simpletext_CorrectGetChar(text, i);
	register struct srchtblelt *e;
	for (e = Tbl; e->code && e->code != c; e++) {}
	fprintf(file, "%s", e->name);
}

/* PrintChar(text, j, file)
	print the j'th character of 'text' to 'file'
	print non-ASCII characters with care
*/
	static void
PrintChar(text, j, f)
	struct simpletext *text;
	long j;
	FILE *f;
{
	register long ic = simpletext_CorrectGetChar(text, j);
	if (ic < ' ') putc('^', f),  putc(ic+'@', f);
	else if (ic < '\177') putc (ic, f);
	else fprintf (f, "\\%03o", ic);
}


/* dumpObjectCode(file, offset)
	print to 'file' a representation of the function represented by the 
	nessmark at location 'offset' in SysMark
*/
	void
dumpObjectCode(file, offset)
	FILE *file;
	long offset;
{
	register long i, end;
	long start;
	register struct simpletext *text;
	long sylcnt;	/* count syllables printed on each line */
	struct nessmark *m = &SysMarkLowEnd[offset];

	text = nessmark_GetText(m);
	start = nessmark_GetPos(m);
	end = start + nessmark_GetLength(m);
	sylcnt = 9999;
	for (i = start;  i < end; ) {
		/* print syllable(s) for operator at i */
		long op;
		enum addrtype randcode;

		if (sylcnt > 6)	/* newline after 10 syllables */
			fprintf(file, "\n     %d:", i), sylcnt = 0;

		op = simpletext_CorrectGetChar(text, i);
		i++;
		if (op >= ' ' &&  op <= '\177') {
			fprintf(file, "   %s", opTbl[op-' '].name);
			randcode = opTbl[op - ' '].randcode;
		}
		else if (op == '\n') {
			fprintf(file, "   nop");
			randcode = none;
		}
		else {
			fprintf(file, "   ERR'\\%o'", op);
			randcode = none;
		}
		sylcnt++;

		switch (randcode) {
		case none:
			break;
		case branch: {
			long offset;
			offset = ExtendShortSign(
				(simpletext_CorrectGetChar(text, i) << 8)
				 + simpletext_CorrectGetChar(text, i+1)
			);
			fprintf(file, "->%d", (i-1) + offset);
			i += 2;
			sylcnt = 100;	/* put newline */
		}	break;
		case call:  {
			long index = (simpletext_CorrectGetChar(text, i)<<8)
					+simpletext_CorrectGetChar(text, i+1);
			fprintf(file, "->%d(@%d)", index,
				nessmark_GetPos(&SysMarkLowEnd[index]));
			i += 2;
			sylcnt++;
		}	break;
		case stack:
			fprintf(file, " %d", simpletext_CorrectGetChar(text, i));
			i++;
			sylcnt++;
			break;
		case sysmark:{
			TSysMarkRef t = (simpletext_CorrectGetChar(text, i)<<8)
					+simpletext_CorrectGetChar(text, i+1);
			struct nessmark *m = &SysMarkLowEnd[t];
			long j, end;
			struct simpletext *text = nessmark_GetText(m);
			fprintf(file, " (%d)\"", t);
			j = nessmark_GetPos(m);
			end = j + nessmark_GetLength(m);
			if (end - j < 20)
				for ( ; j < end; j++)
					PrintChar(text, j, file);
			else {
				/* print 10 chars, three dots, and 7 more chars */
				long frontend = j + 10;
				for ( ; j < frontend; j++)
					PrintChar(text, j, file);
				putc('.', file);  putc('.', file);  putc('.', file);
				for (j = end - 7 ; j < end; j++)
					PrintChar(text, j, file);
			}
			putc ('"', file);
			i += 2;
			sylcnt++;
		}	break;
		case callnode:{
			unsigned char c0, c1, c2, c3;
			struct callnode *cnode;
			c0 = simpletext_CorrectGetChar(text, i);
			c1 = simpletext_CorrectGetChar(text, i+1);
			c2 = simpletext_CorrectGetChar(text, i+2);
			c3 = simpletext_CorrectGetChar(text, i+3);
			cnode = (struct callnode *)
				((c0<<24) | (c1 << 16) | (c2 << 8) | c3);
			if (cnode != NULL) {
				fprintf(file, "%s", callvarietyname[(long)cnode->variety]);
				switch (cnode->variety) {
				case callSym:
					fprintf(file, "(%s)",
						nesssym_NGetName(cnode->where.Sym));
					break;
				case callNess:
					fprintf(file, "[%s]", cnode->where.Nproc);
					break;
				case callPE:
					fprintf(file, "(%s)", cnode->where.pe->name);
					break;
				case callC:
					fprintf(file, "0x%lx", cnode->where.Cproc);
					break;
				}
			}
			else 
				fprintf(file, "callnode(NULL)");
			i += 4;
			sylcnt += 3;
		}	break;
		case search:
			PrintFromTbl(file, text, i++, searchTbl);  break;
		case current:
			PrintFromTbl(file, text, i++, currentTbl);  break;
		case cheat:
			PrintFromTbl(file, text, i++, cheatTbl);  break;
		case cheatcall:
			PrintFromTbl(file, text, i++, cheatcallTbl); 
			/* print number of args */
			fprintf(file, "%d", simpletext_CorrectGetChar(text, i));
			i ++;
			sylcnt++;
			break;
		case event:
			PrintFromTbl(file, text, i++, eventTbl);  break;
		case realunary:
			PrintFromTbl(file, text, i++, realUnTbl);  break;
		case realbinary:
			PrintFromTbl(file, text, i++, realBinTbl);  break;
		} /* end switch */
	} /* end for */
	fprintf(file, "\n");
}

	void
dumpFuncDef(file, f)
	FILE *file;
	struct nesssym *f;
{
	long offset = nesssym_NGetINode(f, funcnode)->SysMarkOffset;

	fprintf(file, "%d:  Function %s", offset, nesssym_NGetName(f));
	dumpObjectCode(file, offset);
}

	void
dumpEventDef(file, f)
	FILE *file;
	struct nesssym *f;
{
	struct eventnode *enode = nesssym_NGetINode(f, eventnode);
	long offset = enode->SysMarkOffset;
	if (enode->enabled) {
		fprintf(file, "%d:  %s Event (%s)", offset, 
			nesssym_NGetName(enode->varsym), enode->spec);
		dumpObjectCode(file, offset);
	}
	else
		fprintf(file, "%d:  disabled Event (%s)", offset, enode->spec);
}


	void
dumpAttrList(file, symlist)
	FILE *file;
	struct nesssym *symlist;
{
	for ( ; symlist;  symlist = symlist->next)
		switch (symlist->flags) {
		case flag_function | flag_ness:
		case flag_function | flag_ness | flag_xfunc:
			dumpFuncDef(file, symlist);
			break;
		case flag_var | flag_globalvar:
			fprintf(file, "%d: %s %s\n", nesssym_NGetInfo(symlist, long),
					TypeName[symlist->type],
					nesssym_NGetName(symlist));
			break;
		case flag_xobj:
			fprintf(file, "\nEXTENDED OBJECT %s\n",
					nesssym_NGetName(symlist) + 1);
			dumpAttrList(file, (nesssym_NGetINode(symlist,objnode)->attrs));
			fprintf(file, "END OBJECT %s\n\n",
					nesssym_NGetName(symlist) + 1);
			break;
		case flag_event:
			dumpEventDef(file, symlist);
			break;
		default:
			fprintf(file, "Symbol %s has unknown flags 0x%lx\n", 
					nesssym_NGetName(symlist), symlist->flags);
			break;
		}
}




/* functions for calling from the debugger */

	void 
ds(NSP) 
	union stackelement *NSP;
{
	dumpStack(stdout, NSP);
	fflush(stdout);
}

	void
da()
{
	dumpAttrList(stdout, *curNess->AttrDest);
	fflush(stdout);
}

	void
dna(n)
	struct nesssym *n;
{
	dumpAttrList(stdout, n);
	fflush(stdout);
}


