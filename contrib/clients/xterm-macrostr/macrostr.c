/* macrostr.c - handle macro string capability
 *
 * 26.Oct.89  Jim McBeath	Initial definition
 */

/*
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Jim McBeath, Silicon Compiler Systems
 *          sci!jimmc@decwrl.dec.com
 */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <ctype.h>
#include "ptyx.h"

#define TOS (stackCount-1)
#define PUSHINT(n) if (!StackPushInt(n)) return FALSE

/* inverse of CursorX and CursorY macros (except no topline used in Row) */
#define CursorRow(screen,Y) (((Y) - screen->border)/FontHeight(screen))
#define CursorCol(screen,X) (((X) - screen->scrollbar - screen->border) \
	/ FontWidth(screen))

extern char *malloc(), *realloc();

enum MacStackType { MacStackNull, MacStackInt, MacStackString };

typedef struct _macstackentry {
	enum MacStackType type;
	union {
		char *s;
		int n;
	} d;
} MacStackEntry;

/* The static variables in this module have a lifetime which is limited
 * to a single call to HandleMacroString.
 */

static Widget stackWidget;
static TScreen *stackScreen;
static XEvent *stackEvent;

/* The value stack used by all of the macro processing functions */
static int stackAlloc;
static int stackCount;
static MacStackEntry *stack;

/* A string buffer used by various parsing functions */
static int stackQAlloc;
static int stackQCount;
static char *Qstr;

/* forward references for PrimTab */
extern int PrimToBool();
extern int PrimNot();
extern int PrimIfTrue();
extern int PrimIfElse();
extern int PrimPop();
extern int PrimPush();
extern int PrimExch();
extern int PrimLimit();
extern int PrimOrd();
extern int PrimReturn();
extern int PrimError();
extern int PrimTekScale();

/* Table of named primitive functions */
static struct {
	char *name;	/* name of the primitive function */
	int (*func)();	/* returns TRUE if all OK, FALSE if problem */
} PrimTab[] = {
	{"error",	PrimError},
	{"exch",	PrimExch},
	{"if",		PrimIfTrue},
	{"ifElse",	PrimIfElse},
	{"limit",	PrimLimit},
	{"ord",		PrimOrd},
	{"pop",		PrimPop},
	{"push",	PrimPush},
	{"tekScale",	PrimTekScale},
	{"toBool",	PrimToBool},
};

/* Table of built-in macro strings */
static struct {
	char *name;	/* name of the built-in macro string */
	char *str;	/* contents of the macro string */
} BuiltinTab[] = {
	{"SeikoButton",
"'\033J'i'SeikoGetX'M'SeikoSub1'M'SeikoGetY'M'SeikoSub1'M'SeikoEnd'M"},
	{"SeikoGetX",	"x48*w/1890-"},
	{"SeikoGetY",	"1537y64*h/-"},
	{"SeikoSub1",
"'push'M'push'M10>>037&0140|%ci5>>037&0100|%ci037&040|%ci"},
	{"SeikoEnd",	"' 'i31b3&+%ci'! !\r'i"},
	{"X10Button",	"'\033[M'i31b+%ci33c+%ci33r+%ci"},
	{"VT200KeyState",	"s5&s8&2>>+"},
	{"VT200ButtonPress", "'\033['iM31'VT200KeyState'M+b+%ci33c+%ci33r+%ci"},
	{"VT200ButtonOther", "'\033['iM32'VT200KeyState'M+3+%ci33c+%ci33r+%ci"},
	{"TekButton",	"'TekGetBchar'M%ci\
x'tekScale'M/0 4096 1- 'limit'M'TekSu1'M \
3072 34+ y'tekScale'/- 0 3072 1- 'limit'M'TekSub1'M"},
	{"TekGetBchar",	"'TekGetlbchar'M s 1 & 'TekUcase' 'if'M 0x80 |"},
	{"TekGetlbchar", "\
'r' 'ord'M b 1 == Z 'pop'M \
'm' 'ord'M b 2 == Z 'pop'M \
'l' 'ord'M b 3 == Z 'error'M"},
	{"TekUcase",	"32 -"},
	{"TekSub1",	"'push'M 7>>037&040|%ci 2>>037&040|%ci"},
	{"TestConst",
"'h='ih%di' w='iw%di' H='iH%di' W='iW%di' R='iR%di' C='iC%di'\n'i"},
	{"TestButton",
"'x='ix%di' y='iy%di' X='iX%di' Y='iY%di\
' r='ir%di' c='ic%di' b='ib%di' s='is%xi'\n'i"},
	{"TestKey",
"'x='ix%di' y='iy%di' X='iX%di' Y='iY%di\
' r='ir%di' c='ic%di' k='ik%di' s='is%xi' l=\"'ili'\"\n'i"},
};


static int
ishexalpha(c)
char c;
{
	return (c>='a'&&c<='f' || c>='A'&&c<='F');
}

/* Value stack manipulation functions */

static void
StackClear()	/* clear the stack */
{
	int i;

	for (i=0; i<stackCount; i++) {		/* free all strings */
		if (stack[i].type == MacStackString) {
			if (stack[i].d.s)
				free(stack[i].d.s);
		}
	}
	stackCount = 0;
}

static void
StackPop(n)
int n;		/* pop N items off the top of the stack */
{
	while (n>0 && stackCount>0) {
		if (stack[TOS].type == MacStackString) {
			if (stack[TOS].d.s)
				free(stack[TOS].d.s);
		}
		n--;
		stackCount--;
	}
}

static Bool	/* TRUE if successful, FALSE it not (no more memory) */
StackPush(e)
MacStackEntry *e;
{
	int nbytes;

	if (stackCount>=stackAlloc) {
		/* need more room */
		if (stackAlloc)
			stackAlloc *= 2;
		else
			stackAlloc = 15;
		nbytes = stackAlloc * sizeof(stack[0]);
		if (stack)
			stack = (MacStackEntry *)realloc((char *)stack,nbytes);
		else
			stack = (MacStackEntry *)malloc(nbytes);
		if (!stack)
			return FALSE;
	}
	stack[stackCount] = *e;	/* copy in structure */
		/* note that we do not do any string copies here */
	stackCount++;
	return TRUE;
}

static Bool	/* TRUE if successful, FALSE it not (no more memory) */
StackPushString(s)
char *s;	/* the string becomes the property of the stack */
{
	MacStackEntry e;
	Bool t;

	e.type = MacStackString;
	e.d.s = s;
	t = StackPush(&e);
	if (!t) {
		free(s);	/* it's our string - don't leak the memory */
	}
	return t;
}

static Bool	/* TRUE if successful, FALSE it not (no more memory) */
StackPushStringCopy(s)
char *s;	/* makes a copy of the string to put onto the stack */
{
	char *newstr;

	newstr = malloc(strlen(s)+1);
	if (!newstr)
		return FALSE;
	strcpy(newstr,s);
	return (StackPushString(newstr));
}

static Bool	/* TRUE if successful, FALSE it not (no more memory) */
StackPushInt(n)
int n;
{
	MacStackEntry e;
	Bool t;

	e.type = MacStackInt;
	e.d.n = n;
	t = StackPush(&e);
	return t;
}


/* String buffer manipulation functions */

static void
StackQclear()
{
	stackQCount = 0;
}

static Bool	/* TRUE is successful, FALSE if not (no more memory) */
StackQchar(c)
char c;
{
	int nbytes;

	if (stackQCount>=stackQAlloc) {
		/* need more room */
		if (stackQAlloc)
			stackQAlloc *= 2;
		else
			stackQAlloc = 120;
		nbytes = stackQAlloc;
		if (Qstr)
			Qstr = realloc(Qstr,nbytes);
		else
			Qstr = malloc(nbytes);
		if (!Qstr)
			return FALSE;
	}
	Qstr[stackQCount++] = c;
	return TRUE;
}

/* handles quoted strings; parses string and pushes it onto the stack */
/* Note that we do NOT do any backslash processing */
static char *	/* returns pointer to the closing quote char or NULL on error */
StackQstr(p,stopchar)
char *p;	/* pointer to first data char */
char stopchar;	/* the character to stop on */
{
	StackQclear();
	while (*p && *p!=stopchar) {
		if (!StackQchar(*p))
			return NULL;	/* no more memory */
		p++;
	}
	if (!*p)
		return NULL;	/* no terminating quote */
	if (!StackQchar(0))
		return NULL;
	if (!StackPushStringCopy(Qstr))
		return NULL;
	return p;
}


static Bool	/* returns TRUE if all OK, FALSE if any problems */
StackToBool(n)	/* converts stack item n to a bool (int 0/1) */
int n;
{
	int f;

	switch (stack[n].type) {
	case MacStackInt:
		stack[n].d.n = !!stack[n].d.n;
		break;
	case MacStackString:
		if (stack[n].d.s && stack[n].d.s[0]) {
			f = 1;
		} else {
			f = 0;
		}
		if (stack[n].d.s)
			free(stack[n].d.s);
		stack[n].type = MacStackInt;
		stack[n].d.n = f;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

/* Primitives */

static Bool	/* returns TRUE if all OK, FALSE if any problems */
PrimToBool()	/* converts TOS to a bool (int 0/1) */
{
	if (stackCount<1)
		return FALSE;
	return (StackToBool(TOS));
}

static Bool	/* returns TRUE if all OK, FALSE if any problems */
PrimNot()	/* converts TOS to Bool and then invert it */
{
	int f;

	if (stackCount<1)
		return FALSE;
	switch (stack[TOS].type) {
	case MacStackInt:
		stack[TOS].d.n = !stack[TOS].d.n;
		break;
	case MacStackString:
		if (stack[TOS].d.s && stack[TOS].d.s[0]) {
			f = 0;
		} else {
			f = 1;
		}
		StackPop(1);
		PUSHINT(f);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

/* The IfTrue primitive requires a macro name at TOS and a value at TOS-1.
 * Both values are first removed from the stack.
 * If the value is TRUE, then the macro is executed, otherwise nothing
 * else happens.
 */
static Bool	/* returns TRUE if all OK, FALSE if any problems */
PrimIfTrue()
{
	char *macroname;
	int b,t;

	if (stackCount<2)
		return FALSE;
	if (stack[TOS].type!=MacStackString)
		return FALSE;
	macroname = stack[TOS].d.s;
	stack[TOS].d.s = 0;
	StackPop(1);
	if (!PrimToBool())
		return FALSE;	/* error testing truth */
	b = stack[TOS].d.n;
	StackPop(1);
	if (b)
		t = StackDoNamedMacro(macroname);
	else
		t = TRUE;
	free(macroname);
	return t;
}

/* The IfElse primitive requires a macro name at TOS,
 * a macro name at TOS-1, and a condition value at TOS-2.
 * All three items are first removed from the stack.
 * If the condition is TRUE, then the macro is which was at TOS-1 is executed,
 * otherwise the macro which was at TOS is executed.
 * Thus, you first push the condition value, then the name of the TRUE macro,
 * then the name of the FALSE macro, then "ifElse", then M.
 */
static Bool	/* returns TRUE if all OK, FALSE if any problems */
PrimIfElse()
{
	char *truemacro, *falsemacro;
	int b,t;

	if (stackCount<3)
		return FALSE;
	if (stack[TOS].type!=MacStackString ||
	    stack[TOS-1].type!=MacStackString)
		return FALSE;
	falsemacro = stack[TOS].d.s;
	stack[TOS].d.s = 0;
	truemacro = stack[TOS-1].d.s;
	stack[TOS-1].d.s = 0;
	StackPop(2);
	if (!PrimToBool())
		return FALSE;	/* error testing truth */
	b = stack[TOS].d.n;
	StackPop(1);
	if (b)
		t = StackDoNamedMacro(truemacro);
	else
		t = StackDoNamedMacro(falsemacro);
	free(truemacro);
	free(falsemacro);
	return t;
}

static Bool	/* returns TRUE if all OK, FALSE if any problems */
PrimPop()	/* throws away the top item on the stack */
{
	if (stackCount<1)
		return FALSE;
	StackPop(1);
	return TRUE;
}

static Bool	/* returns TRUE if all OK, FALSE if any problems */
PrimPush()	/* duplicates the top item on the stack by pushing a copy */
{
	if (stackCount<1)
		return FALSE;
	switch (stack[TOS].type) {
	case MacStackInt:
		PUSHINT(stack[TOS].d.n);
		break;
	case MacStackString:
		if (!StackPushStringCopy(stack[TOS].d.s))
			return FALSE;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

static Bool	/* returns TRUE if all OK, FALSE if any problems */
PrimExch()	/* exchanges the top two items on the stack */
{
	MacStackEntry e;

	if (stackCount<2)
		return FALSE;
	e = stack[TOS];		/* structure copy */
	stack[TOS] = stack[TOS-1];
	stack[TOS-1] = e;
	return TRUE;
}

static Bool	/* returns TRUE if all OK, FALSE if any problems */
PrimLimit()	/* limits TOS-2 to be between TOS-1 and TOS */
{
	int high,low;

	if (stackCount<3)
		return FALSE;
	if (stack[TOS].type != MacStackInt ||
	    stack[TOS-1].type != MacStackInt ||
	    stack[TOS-2].type != MacStackInt)
		return FALSE;
	high = stack[TOS].d.n;
	low = stack[TOS-1].d.n;
	StackPop(2);
	if (stack[TOS].d.n>high)
		stack[TOS].d.n = high;
	else if (stack[TOS].d.n<low)
		stack[TOS].d.n = low;
	return TRUE;
}

static Bool	/* returns TRUE if all OK, FALSE if any problems */
PrimOrd()	/* converts a char (string of len 1) to an int */
{
	int ord;

	if (stackCount<1)
		return FALSE;
	if (stack[TOS].type != MacStackString ||
	    strlen(stack[TOS].d.s)!=1)
		return FALSE;
	ord = stack[TOS].d.s[0];
	StackPop(1);
	PUSHINT(ord);
	return TRUE;
}

static Bool	/* returns TRUE if all OK, FALSE if any problems */
PrimError()	/* generates an error (aborts) */
{
	return FALSE;	/* simple enough */
}

static Bool	/* returns TRUE if all OK, FALSE if any problems */
PrimTekScale()	/* generates an error (aborts) */
{
	PUSHINT(TekScale(stackScreen));
	return TRUE;
}


/* Macro processing functions */

typedef struct {
	char *value;
} UmInfo, *UmInfoPtr;
static XtResource umresource[] = {
	{ "", "", XtRString, sizeof(String),
		XtOffset(UmInfoPtr,value),XtRString,NULL},
};

static String		/* returns macro string or NULL if not found */
StackFindUserMacro(name)
String name;
{
	UserMacro *umlist;
	UmInfo uminfo;

	umlist = stackScreen->userMacros;
	for (; umlist; umlist=umlist->next) {
		if (strcmp(umlist->name,name)==0)
			return(umlist->value);
	}
	/* not a macro that is already loaded, see if we can load it */
	umresource[0].resource_name = name;
	umresource[0].resource_class = name;
	XtGetSubresources(stackWidget,(caddr_t)&uminfo,
		"macroString","MacroString",
		umresource,(Cardinal)1,(ArgList)NULL,(Cardinal)0);
	if (!uminfo.value)
		return NULL;	/* can't find the macro */
	umlist = (UserMacro *)malloc(sizeof(UserMacro));
	if (!umlist)
		return NULL;	/* can't get memory */
	umlist->name = malloc(strlen(name)+1);
	if (!umlist->name) {
		free((char *)umlist);
		return NULL;
	}
	umlist->value = malloc(strlen(uminfo.value)+1);
	if (!umlist->value) {
		free(umlist->name);
		free((char *)umlist);
		return NULL;
	}
	strcpy(umlist->name,name);
	strcpy(umlist->value,uminfo.value);
	umlist->next = stackScreen->userMacros;
	stackScreen->userMacros = umlist;
	return(umlist->value);
}

/* handles the "%" formatting; parses format string and operates on stack */
static char *	/* returns pointer to the last char of the fmt string or NULL */
StackFstr(p)
char *p;	/* pointer to first char of formatting string */
{
	static char *fmtchars="-+#*.0123456789";
	char *buf;

	if (stackCount<1)
		return NULL;
	StackQclear();
	if (!StackQchar('%'))
		return NULL;
	while (*p && (index(fmtchars,*p))) {
		if (!StackQchar(*p))
			return NULL;
		p++;
	}
	if (!StackQchar(*p))	/* add the final formatting char */
		return NULL;
	if (!StackQchar(0))	/* null terminate the format string */
		return NULL;
	switch (*p) {
	case 0:
		return NULL;
	case 'd': case 'i': case 'o': case 'u': case 'x': case 'X': case 'c':
		if (stack[TOS].type!=MacStackInt)
			return NULL;
		buf = malloc(100);
		if (!buf)
			return NULL;
		sprintf(buf,Qstr,stack[TOS].d.n);
		break;
	case 's':
		if (stack[TOS].type!=MacStackString)
			return NULL;
		buf = malloc(strlen(stack[TOS].d.s)+100);
		if (!buf)
			return NULL;
		sprintf(buf,Qstr,stack[TOS].d.s);
		break;
	default:
		return NULL;
	}
	StackPop(1);	/* get rid of converted value */
	if (!StackPushStringCopy(buf)) {
		free(buf);
		return NULL;
	}
	free(buf);
	return p;
}

/* handles primitives */
Bool		/* returns TRUE if OK, FALSE is any errors */
StackDoPrimitive(name)
char *name;
{
	int i;

	for (i=0; i<XtNumber(PrimTab); i++) {
		if (strcmp(PrimTab[i].name,name)==0) {
			return (*PrimTab[i].func)();
		}
	}
	return FALSE;
}

/* handles built-in macro strings */
Bool		/* returns TRUE if OK, FALSE is any errors */
StackDoBuiltin(name)
char *name;
{
	int i;

	for (i=0; i<XtNumber(BuiltinTab); i++) {
		if (strcmp(BuiltinTab[i].name,name)==0) {
			return ProcessMacroString(BuiltinTab[i].str);
		}
	}
	return FALSE;
}

/* handles user defined macro strings */
Bool		/* returns TRUE if OK, FALSE is any errors */
StackDoUserMacro(name)
char *name;
{
	String macrostr;

	macrostr = StackFindUserMacro(name);
	if (!macrostr)
		return FALSE;	/* no such macro */
	return ProcessMacroString(macrostr);
}

/* handles macro as named at TOS */
Bool		/* returns TRUE if OK, FALSE if any errors */
StackDoMacro()
{
	char *macroname;
	int t;

	if (stackCount<1 || stack[TOS].type!=MacStackString)
		return FALSE;	/* no macro name */
	macroname = stack[TOS].d.s;
	stack[TOS].d.s = 0;
	StackPop(1);	/* remove the macro name from the stack */
	t = StackDoNamedMacro(macroname);
	free(macroname);
	return t;
}

/* handles macros (primitives, built-in macros, or user macros) */
Bool		/* returns TRUE if OK, FALSE if any errors */
StackDoNamedMacro(macroname)
String macroname;
{
	/* User macros can override builtin macro strings, but not primitives */
	if (StackDoPrimitive(macroname) ||
	    StackDoUserMacro(macroname) ||
	    StackDoBuiltin(macroname)) {
		return TRUE;
	}
	return FALSE;	/* no such macro */
}

static int    /* returns TRUE if all OK, FALSE if problem (syntax, no memory) */
ProcessMacroString(macrostring)
char *macrostring;
{
	char *p;
	char *newstr;
	int newnum;
	int l;
	int n;
	Bool b;
	int radix, hc;
	KeySym ks;
	char buf[100];

	for (p=macrostring;*p;p++) {	/* process commands */
		switch (*p) {
		case ' ':	/* ignore spaces */
			break;
		case '"':	/* quoted string */
		case '\'':
			p = StackQstr(p+1,*p);
			if (!p)
				return FALSE;		/* error */
			break;
		case '%':	/* formatted print onto TOS */
			p = StackFstr(p+1);
			if (!p)
				return FALSE;		/* error */
			break;
		case '+':	/* add two numbers or cat two strings */
			if (stackCount<2)
				return FALSE;
			if (stack[TOS].type == MacStackString &&
			    stack[TOS-1].type == MacStackString) {
				l = strlen(stack[TOS].d.s) +
				    strlen(stack[TOS-1].d.s) + 1;
				newstr = malloc(l);
				if (!newstr)
					return FALSE;
				strcpy(newstr,stack[TOS-1].d.s);
				strcat(newstr,stack[TOS].d.s);
				StackPop(1);
				free(stack[TOS].d.s);
				stack[TOS].d.s = newstr;
			}
			else if (stack[TOS].type == MacStackInt ||
				 stack[TOS-1].type == MacStackInt) {
				newnum = stack[TOS-1].d.n + stack[TOS].d.n;
				StackPop(1);
				stack[TOS].d.n = newnum;
			}
			else
				return FALSE;
			break;
		case '-':	/* subtract two numbers */
			if (stackCount<2)
				return FALSE;
			if (stack[TOS].type == MacStackInt ||
				 stack[TOS-1].type == MacStackInt) {
				newnum = stack[TOS-1].d.n - stack[TOS].d.n;
				StackPop(1);
				stack[TOS].d.n = newnum;
			}
			else
				return FALSE;
			break;
		case '*':	/* multiply two numbers */
			if (stackCount<2)
				return FALSE;
			if (stack[TOS].type == MacStackInt ||
				 stack[TOS-1].type == MacStackInt) {
				newnum = stack[TOS-1].d.n * stack[TOS].d.n;
				StackPop(1);
				stack[TOS].d.n = newnum;
			}
			else
				return FALSE;
			break;
		case '/':	/* divide two numbers */
			if (stackCount<2)
				return FALSE;
			if (stack[TOS].type == MacStackInt ||
				 stack[TOS-1].type == MacStackInt) {
				newnum = stack[TOS-1].d.n / stack[TOS].d.n;
				StackPop(1);
				stack[TOS].d.n = newnum;
			}
			else
				return FALSE;
			break;
		case '<':	/* left shift (<<) */
			if ((*(++p))!='<')
				return FALSE;
			if (stackCount<2)
				return FALSE;
			if (stack[TOS].type == MacStackInt &&
				 stack[TOS-1].type == MacStackInt) {
				newnum = stack[TOS-1].d.n << stack[TOS].d.n;
				StackPop(1);
				stack[TOS].d.n = newnum;
			}
			else
				return FALSE;
			break;
		case '>':	/* right shift (>>) */
			if ((*(++p))!='>')
				return FALSE;
			if (stackCount<2)
				return FALSE;
			if (stack[TOS].type == MacStackInt &&
				 stack[TOS-1].type == MacStackInt) {
				newnum = stack[TOS-1].d.n >> stack[TOS].d.n;
				StackPop(1);
				stack[TOS].d.n = newnum;
			}
			else
				return FALSE;
			break;
		case '|':	/* logical or bitwise OR */
			if (stackCount<2)
				return FALSE;
			if (p[1]=='|') {	/* logical */
				p++;
				if (!(StackToBool(TOS) && StackToBool(TOS-1)))
					return FALSE;
				newnum = stack[TOS-1].d.n || stack[TOS].d.n;
			} else {		/* bitwise */
				if (stack[TOS].type != MacStackInt ||
				    stack[TOS-1].type != MacStackInt)
					return FALSE;
				newnum = stack[TOS-1].d.n | stack[TOS].d.n;
			}
			StackPop(1);
			stack[TOS].d.n = newnum;
			break;
		case '&':	/* logical or bitwise AND */
			if (stackCount<2)
				return FALSE;
			if (p[1]=='&') {	/* logical */
				p++;
				if (!(StackToBool(TOS) && StackToBool(TOS-1)))
					return FALSE;
				newnum = stack[TOS-1].d.n && stack[TOS].d.n;
			} else {		/* bitwise */
				if (stack[TOS].type != MacStackInt ||
				    stack[TOS-1].type != MacStackInt)
					return FALSE;
				newnum = stack[TOS-1].d.n & stack[TOS].d.n;
			}
			StackPop(1);
			stack[TOS].d.n = newnum;
			break;
		case '^':	/* logical or bitwise XOR */
			if (stackCount<2)
				return FALSE;
			if (p[1]=='^') {	/* logical */
				p++;
				if (!(StackToBool(TOS) && StackToBool(TOS-1)))
					return FALSE;
				newnum = stack[TOS-1].d.n ^ stack[TOS].d.n;
			} else {		/* bitwise */
				if (stack[TOS].type != MacStackInt ||
				    stack[TOS-1].type != MacStackInt)
					return FALSE;
				newnum = stack[TOS-1].d.n ^ stack[TOS].d.n;
			}
			StackPop(1);
			stack[TOS].d.n = newnum;
			break;
		case '!':	/* logical not */
			if (!PrimNot())
				return FALSE;
			break;
		case '~':	/* bitwise invert */
			if (stackCount<1)
				return FALSE;
			if (stack[TOS].type == MacStackInt) {
				newnum = ~stack[TOS-1].d.n;
				stack[TOS].d.n = newnum;
			}
			else
				return FALSE;
			break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			newnum = 0;
			if ((*p)=='0') {
				if (p[1]=='x' || p[1]=='X') {
					radix = 16;
					p+=2;
				}
				else
					radix = 8;
			}
			else
				radix = 10;
			while (isdigit(*p) || (radix==16&&ishexalpha(*p))) {
				newnum *= radix;
				if (radix==16&&ishexalpha(*p)) {
					hc = *p;
					if (islower(hc))
						hc = toupper(hc);
					newnum += hc - 'A' + 10;
				}
				else
					newnum += *p - '0';
				p++;
			}
			--p;	/* have to leave it pointing to last char */
			PUSHINT(newnum);
			break;
		case 'b':	/* put button field from event onto stack */
			PUSHINT(stackEvent->xbutton.button);
			break;
		case 'c':	/* put cursor col onto stack (0 is leftmost) */
			PUSHINT(CursorCol(stackScreen,stackEvent->xbutton.x));
			break;
		case 'C':	/* put number of cols on screen onto stack */
			PUSHINT(stackScreen->max_col+1);
			break;
		case 'h':	/* put pixel height of a char on stack */
			PUSHINT(FontHeight(stackScreen));
			break;
		case 'H':	/* put pixel height of screen on stack */
			PUSHINT(stackScreen->fullVwin.height);
			break;
		case 'i':	/* send TOS as input */
			if (stackCount<1)
				return FALSE;
			if (stack[TOS].type == MacStackString)
				StringInput(stackScreen,stack[TOS].d.s);
			else
				return FALSE;
			StackPop(1);
			break;
		case 'k':	/* put keycode field from event onto stack */
			PUSHINT(stackEvent->xkey.keycode);
			break;
		case 'l':	/* put results of XLookupString on stack */
			n = XLookupString(stackEvent,buf,sizeof(buf)-1,&ks,0);
			if (n>0) {
				newstr = malloc(n+1);
				if (!newstr)
					return FALSE;
				strcpy(newstr,buf);
				if (!StackPushString(newstr))
					return FALSE;
			} else {	/* no translation, use null string */
				if (!StackPushStringCopy(""))
					return FALSE;
			}
			break;
		case 'M':	/* execute macro */
			if (!StackDoMacro())
				return FALSE;
			break;
		case 'r':	/* put cursor row onto stack (0 is topmost) */
			PUSHINT(CursorRow(stackScreen,stackEvent->xbutton.y));
			break;
		case 'R':	/* put number of rows on screen onto stack */
			PUSHINT(stackScreen->max_row+1);
			break;
		case 's':	/* put state field from event onto stack */
			PUSHINT(stackEvent->xbutton.state);
			break;
		case 'w':	/* put pixel width of a char on stack */
			PUSHINT(FontWidth(stackScreen));
			break;
		case 'W':	/* put pixel width of screen on stack */
			PUSHINT(stackScreen->fullVwin.width);
			break;
		case 'x':	/* put x pixel value of cursor on stack */
			PUSHINT(stackEvent->xbutton.x - stackScreen->border - 
						stackScreen->scrollbar);
			break;
		case 'y':	/* put y pixel value of cursor on stack */
			PUSHINT(stackEvent->xbutton.y - stackScreen->border);
			break;
		case 'X':	/* put x_root pixel value of cursor on stack */
			PUSHINT(stackEvent->xbutton.x_root);
			break;
		case 'Y':	/* put y_root pixel value of cursor on stack */
			PUSHINT(stackEvent->xbutton.y_root);
			break;
		case 'Z':	/* conditional return/end of macro */
			if (stackCount<1)
				return FALSE;	/* error */
			if (!PrimToBool())
				return FALSE;	/* error testing truth */
			b = stack[TOS].d.n;
			StackPop(1);
			if (b)
				return TRUE;	/* if true, end of this macro */
			/* else do nothing */
			break;
		default:
			return FALSE;
		}
	}
	return TRUE;	/* finished without errors */
}

/* This is the action function called from the translation table */
void
HandleMacroString(w,event,params,param_count)
Widget w;
XEvent *event;
String *params;
int *param_count;
{

	if (*param_count != 1)
		return;
	StackClear();		/* clear the stack machine */
	stackScreen = &((XtermWidget)w)->screen;
	stackWidget = w;
	stackEvent = event;
	if (!ProcessMacroString(params[0])) {
		Bell();		/* some sort of error */
	}
}

/* end */
