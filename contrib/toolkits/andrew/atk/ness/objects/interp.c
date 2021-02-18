/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/interp.c,v 1.28 89/11/21 11:56:33 wjh Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/interp.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/interp.c,v 1.28 89/11/21 11:56:33 wjh Exp $";
#endif

/* interp.c
	interpret byte code representation of ness programs

	Entry points:

	initializeEnvt()  -  initialize the run-time environment
	stackArg(argvalue, type) - puts an argument on the stack
	interpretNess(func, ness)  -  interpret a call on func
*/

/*
 * $Log:	interp.c,v $
 * Revision 1.28  89/11/21  11:56:33  wjh
 * Correct the decision in writefile() of whether and what template to use.  
 * This fixes the bug in files written by gendoc.  They were getting style menus for only
 * a few styles.  Now they get all styles.
 * 
 * Revision 1.27  89/11/04  17:24:51  wjh
 * patch from Guy Harris to ness.c:
 * Do initializeEnvt() in ness_EstablishViews() to be sure the pointers are initialized.  This avoids "!!! Disasterous error..."
 * 
 * patch from Guy Harris to ness.c:
 * Change call.c so it passes the long variant of anytype to proc in callCfunc.  This again avoids a "!!! Disasterous error."  Similarly change the argument to free() in FreeTheMallocs.
 * 
 * Changed delimiter in origin line to double backslash to avoid error warning in sendmessage about illegal character in text.
 * 
 * Added ness_GetOriginData to extract fields from origin data.
 * 
 * ESC-~ gave funny results in a nessview if text wass initially read-only, but had no warning.  It is now fixed to keep read-onliness and the warning message in synch.
 * 
 * Implemented  pragmat:  --$Ness <level>   
 * Initial syntax level is CURRENTSYNTAXLEVEL.
 * Level before read is set to UNSPECIFIEDSYNTAXLEVEL   (i.e., -1)
 * After compile check syntax level and give error if too HIGH.
 * 
 * If the left operand of a catenation has a style, that style was imposed on the right operand, but should not have been.   It no longer is.
 * 
 * 
 * Revision 1.26  89/10/25  23:37:42  zs01
 * Yet again.
 * 
 * Revision 1.25  89/10/25  23:16:03  zs01
 * Extended VAX ifdefs to cover PMAX on isnan crap.
 * 
 * Revision 1.24  89/10/06  16:28:28  ghoti
 * changed defined(SYSV_ENV) to SY_U5x
 * 
 * Revision 1.23  89/10/06  11:33:19  ghoti
 * changed #ifndef HPUX_ENV's around things like 'isnan' and 'lgamma' to be:
 * #if (! defined(SYSV_ENV) && ! SY_AIXx)
 * 
 * Revision 1.22  89/09/18  20:05:08  wjh
 * convert for _ENV
 * 
 * Revision 1.21  89/09/17  08:53:19  wjh
 * make sure textimage generates real values with a "."
 * 	and null pointers are printed as NULL
 * 
 * Revision 1.20  89/09/10  17:51:20  wjh
 * change method of defining key and mouse bindings for views
 * 
 * Revision 1.19  89/09/03  22:47:49  wjh
 * newness
 * 
 * Revision 1.18  89/06/23  17:20:09  wjh
 * (Items that should not be sent to downstairs are marked "R4".)
 * 
 * Added a call to CheckForInterrupt in the function calls and internal gotos.  Thus loops can be terminated with ^G.   (interp.c)  R4
 * 
 * Changed dokeys() so it uses im_DoKeySequence.  This means it is now possible to send a key sequence which involves the message line.  (interp.c)  R4
 * 
 * Implemented DoMenu(object, menustring).   This function causes the same behavior as if the user selected the menu option.  At present the menu string must be exactly as originally defined;  see the warning above for im_DoMenu().  (interp.c, call.c)  R4
 * 
 * Changed access to Ness library routines so they are always compiled.  (They used ot get the default of NoCompilation, so they were useless.)  (call.c)  
 * 
 * Removed a superflous {} pair.  {This is purely a cosmetic change.}  (nessmark.c) 
 * 
 * Fixed value_GetStringValue.  Formerly it was getting an invalid initial value.  {The fix was done by adding a call to nessmark_Initialize() in the stackString section.}  (nevent.c) 
 * 
 * Modified the data stream so errors will not occur when a ness object is the outermost object.  The fix was to add two bytes, "00", at the end of the origin string to prevent the former occurence of a spurious "\}".  (ness.c) 
 * 
 * Fixed menu handling so Ness and child menus get posted when there is a mouse click in an inset within the Ness.  Formerly neither set of menus was posted. (nessv.c) 
 * 
 * Fixed dostmt, the function called from ness-load, which is recommended to be bound to ESC-ESC.  It was using a NULL pointer, so people were getting core dumps if they typed ESC-ESC before doing a compile.  (ness.c) 
 * 
 * Avoided an infinite loop which occurred if a library function referred to a non-existent entry point within itself.  Did this by checking to see if the library function is already Compiling just before trying to compile it.  (call.c call.hn, call.h) 
 * 
 * Revised system marker allocation so the compilation will not get a subsequent error.  (gen.c)
 * 
 * Revised system marker allocation so it expands the space available if necessary. This will make it possible to compile larger programs.  (gen.c)
 * 
 * Changed the type of TType to long from struct object *.  This will allow compilation on stricter compilers.  (interp.h) 
 * 
 * Fixed nessmark_FinalizeObject so it would not reference a NULL pointer.  {Somehow the assembler noticed this bug!}  (nessmark.c) 
 * 
 * Changed functions which deal with constant strings to have (char *) as there argument type (SaveError, exprnode_Create, ReportError, ExprError, RunError, LocateErrorFunc, QueryReadOnly, makeConst, printallerrors) or return type (Freeze, argcounterr, argtypeerr).  This prevents compile errors on picky compilers.  (interp.c, error.c, call.c, ness.c, nessv.c, search.c, nevent.c, nessmark.c, nessrun.ci)  R4 
 * 
 * Changed Imakefile to store Imakefile in checkin rule.  (Imakefile)
 * 
 * 
 * Revision 1.17  89/06/01  15:59:55  wjh
 * campus release version
 * 
 * Revision 1.11  89/01/04  16:27:52  wjh
 * fix another stupid coredump
 * 
 * Revision 1.10  89/01/04  12:57:05  wjh
 * fix stupid incompatible type messages for unsigned char *
 * 
 * Revision 1.9  88/12/20  19:46:00  wjh
 * fixed various bugs
 * 
 * Revision 1.8  88/12/18  17:40:11  wjh
 * fixed a number of bugs
 * added im_ForceUpdate()
 * changed order of args to DoHit()
 * 
 * Revision 1.7  88/12/07  22:43:01  wjh
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
 * Revision 1.6  88/11/23  14:43:27  wjh
 * proc table calls now work correctly with type-free procs  (the first arg can be anything)
 * added "cheat_" functions which will remain undocumented
 * changed inset() to a function
 * fixed some bugs in initial access
 * 
 * Revision 1.5  88/11/16  12:00:04  wjh
 * 
 * implemented access level control
 * skip first line of script if starts with # (for shell script)
 * changed so the name of all initialization functions is init()
 * added ness-load
 * moved execution of init() from compilation to first execution
 * deferred compilation to first FullUpdate time
 * 
 * Revision 1.4  88/11/02  14:41:41  wjh
 * fixed bugs with byte swapping
 * added some more corrected copyrights
 * 
 * Revision 1.3  88/11/01  17:41:53  wjh
 * 
 * an empty Ness is now given a template (ness.template or default.template)
 * implemented:  extend proctable on proc "name" ...
 * added currentmark()
 * fixed a core dump by not recompile during UnlinkTree()
 * 
 * Revision 1.2  88/10/26  11:54:00  wjh
 * fixed a malloc abort bug that happened when certain error messages were discarded
 * added call to init() when initial compilation is done
 * fixed so events are posted to views whenever the view name is registered with the arbiter
 * 
 * Revision 1.1  88/10/21  10:58:35  wjh
 * Initial revision
 * 
 * Creation 0.0  88/03/29 10:16:00  wjh
 * Initial creation by WJHansen
 * 
*/

#include <andrewos.h>
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>


#include <view.ih>
#include <textv.ih>
#include <stylesht.ih>
#include <envrment.ih>
#include <keystate.ih>
#include <im.ih>
#include <proctbl.ih>
#include <arbiterv.ih>
#include <celv.ih>
#include <message.ih>
#include <attribs.h>

#include <ness.ih>
#include <nessmark.ih>

#include <xgetchar.h>
#include <nodeclss.h>
#include <error.h>
#include <interp.h>
#include <envt.h>
#include <nevent.h>
#include <call.h>

#define APPENDSLOP 100		/* bytes to increase append rcvr beyond required */
#define FUNCSTACKSLOP 55	/* number of markers to leave space for after entering
					a function */


/* defining instances of globals  from envt.h */

struct simpletext *ObjectCode = NULL;	/* the compiled bytecode for the functions */
struct simpletext *EmptyText = NULL;	/* points to the special mark for error extent */
struct nessmark *SysMarkFree = NULL, *SysMarkLowEnd = NULL, *SysMarkHiEnd = NULL;
long SysMarkSize = InitialSysMarkSize;
TType nessmarkHdr = NULL;
struct frameelt *FramePtr = NULL;	/* points to current stack frame */
union stackelement *NSPstore = NULL, 
		*NSPbase = NULL, *NSLowEnd = NULL, *NSHiEnd = NULL;
long NSSize = InitialNSSize;		/* the number of words in the stack. */
/* long nArgsGiven = 0;		number of arguments on stack when interpreter is called */
struct classinfo *celClass = NULL;
struct classinfo *arbiterviewClass = NULL;
struct classinfo *textClass = NULL;
struct classinfo *textviewClass = NULL;
struct classinfo *dataobjectClass = NULL;
struct classinfo *lpairClass = NULL;
struct classinfo *scrollClass = NULL;
struct classinfo *matteClass = NULL;
struct classinfo *viewClass = NULL;
struct classinfo *celviewClass = NULL;
struct classinfo *valueviewClass = NULL;
struct classinfo *imClass = NULL;
struct ness *InterpretationInProgress = NULL;	/* used in ness.c */

static jmp_buf ExecutionExit;
static unsigned char *errIAR;
static unsigned char *FuncIAR;



/*RunError(msg, iar)
	execution errors call this routine, which passes the error along.
	'msg' describes the error.  If 'msg' begins with ':' it is a constant;
	if with a '*', it has been malloced especially for this error and
	will be freed when the error is discarded.
	'iar' is the index of the opcode that failed (it has been adjusted backward).
*/
	boolean	/* type is to fool the conditional expr in PTSTOMARK */
RunError(msg, iar) 
	char *msg;
	unsigned char *iar;
{
	errIAR = iar;
	longjmp (ExecutionExit, msg);
	return TRUE;		/* more stuff to fool compiler */
}

	void
SigHandler()
{
	RunError(":! ! !   Disastrous Ness error in this function.   Quit soon.   !!!!!!!!!!",
			FuncIAR); 
}



/* initializeEnvt()
	initialize all facets of the Ness environment
*/
	void
initializeEnvt()
{
	short i;
	static boolean Initialized = FALSE;
	if (Initialized) 
		return;
	Initialized  = TRUE;

	celClass = class_Load("cel");
	arbiterviewClass = class_Load("arbiterview");
	textClass = class_Load("text");
	textviewClass = class_Load("textview");
	dataobjectClass = class_Load("dataobject");
	lpairClass = class_Load("lpair");
	scrollClass = class_Load("scroll");
	matteClass = class_Load("matte");
	viewClass = class_Load("view");
	celviewClass = class_Load("celview");
	valueviewClass = class_Load("valueview");
	imClass = class_Load("im");

	ObjectCode = simpletext_New();

	/* SysMark array */
	SysMarkSize = InitialSysMarkSize;
	SysMarkLowEnd = (struct nessmark *)calloc(SysMarkSize, sizeof(struct nessmark));
	SysMarkHiEnd = SysMarkLowEnd + SysMarkSize;
	for (i = 0; i < SysMarkSize; i++)
		nessmark_SetNext(SysMarkLowEnd+i, (struct mark *)(SysMarkLowEnd+i+1));
	nessmark_SetNext(SysMarkHiEnd-1, NULL);
	SysMarkFree = SysMarkLowEnd + 1;
		/* discard SysMark[SysMarkLowEnd] so no function
		will have index 0 */

	/* allocate EmptyText. */
	nessmark_Initialize(SysMarkLowEnd);
	nessmark_MakeConst(SysMarkLowEnd, "");
	EmptyText = nessmark_GetText(SysMarkLowEnd);

	/* XXX Ugh.  We save the first word of a nessmark to identify other nessmarks */
	nessmarkHdr = (TType)(SysMarkLowEnd->header.nessmark_methods);


	/* Ness Execution Stack */
	NSSize = InitialNSSize;
	NSLowEnd = (union stackelement *)calloc(NSSize, sizeof(union stackelement));
	NSHiEnd = NSLowEnd + NSSize;
	NSPstore = NSHiEnd;

	/* create an initial frame record */
	NSPstore = (union stackelement *)((unsigned long)NSPstore - sizeof(struct frameelt));
	NSPstore->f.hdr = frameHdr;
	NSPstore->f.returnaddress = NULL;  
	NSPstore->f.prevframe = NULL;

	NSPbase = NSPstore;
	FramePtr = &NSPbase->f;
	/*  nArgsGiven = 0; */
}

/* popValue(NSP)
	removes topmost value from stack
	unlinks marks and frames
*/
	union stackelement *
popValue(NSP)
	union stackelement *NSP;
{
	switch (NSP->l.hdr) {
	      case (longHdr):
		NSP = (union stackelement *)(((unsigned long)NSP) + sizeof(struct longstkelt));
		break;
	      case (boolHdr):
		NSP = (union stackelement *)(((unsigned long)NSP) + sizeof(struct boolstkelt));
		break;
	      case (dblHdr):
		NSP = (union stackelement *)(((unsigned long)NSP) + sizeof(struct dblstkelt));
		break;
	      case (ptrHdr):
		NSP = (union stackelement *)(((unsigned long)NSP) + sizeof(struct ptrstkelt));
		break;
	      case (frameHdr):
		if (((long)NSP) >= ((long)FramePtr))
			FramePtr = NSP->f.prevframe;
		else {};	/* XXX illegal framepointer skipped ??? */
		NSP = (union stackelement *)(((unsigned long)NSP) + sizeof(struct frameelt));
		break;
	      default:
	      	if (NSP->l.hdr == nessmarkHdr) {
			/* remove link to this mark.  */
			nessmark_Finalize(&NSP->m);
			NSP = (union stackelement *)
				(((unsigned long)NSP) + sizeof(struct nessmark));
		}
		else
			/* ERROR: just discard one long word */
			NSP = (union stackelement *)(((unsigned long)NSP) + sizeof(long));
		break;
	}
	return NSP;
}


#if notdef
/* we do not need stackArg for now.  The single arg passed to interpretNess is enough for nessrun
the code is useful because it shows how to stack things */

union  argType {
	long l;
	unsigned long b;
	double d;
	struct basicobject *p;
	struct nessmark *m;
};


/* stackArg(arg, type)
	 puts the argument onto NSP and keeps track of the number of arguments supplied
*/
	boolean
stackArg(arg, type)
	union  argType *arg;
	TType type;
{
	if (NSPstore-1 <= NSLowEnd) {
		/* stack overflow */
		/* XXX */
		return FALSE;
	}

	switch (type) {
	case (longHdr):
	case (boolHdr):
		NSPstore = (union stackelement *)((unsigned long)NSPstore 
				- sizeof(struct longstkelt));
		NSPstore->l.hdr = type;
		NSPstore->l.v = arg->l; 
		break; 
	case (dblHdr):
		NSPstore = (union stackelement *)((unsigned long)NSPstore 
				- sizeof(struct dblstkelt));
		NSPstore->d.hdr = type;
		NSPstore->d.v = arg->d; 
		break;
	case (ptrHdr):
		if ((TType)arg->m->header.nessmark_methods
				== nessmarkHdr) {
			NSPstore = (union stackelement *)((unsigned long)NSPstore 
				- sizeof(struct nessmark)); 
			nessmark_InitFrom(&NSPstore->m, arg->m);
		}
		else {
			/* some other kind of pointer */
			NSPstore = (union stackelement *)((unsigned long)NSPstore 
						- sizeof(struct ptrstkelt));
			NSPstore->p.hdr = type;
			NSPstore->p.v = arg->p; 
		}
		break;
	}
	nArgsGiven ++;
	return TRUE;
}
#endif

	static void
QueryReadOnly(ness, s, msg, iar)
	struct ness *ness;
	struct simpletext *s;
	char *msg;
	unsigned char *iar;
{
	if (s == (struct simpletext *)ness_GetDefaultText(ness)) {
		/* see if user wants to make defaulttext read/write */
		long choice;
		static char *choices[] = {
			"Cancel - Leave text read-only", 
			"Read-Write - Let the Ness script modify the text", 
			NULL
		};
		if (message_MultipleChoiceQuestion(NULL, 50, 
			"The main text is read-only, but the script is trying to modify it.  Okay?",
				0, &choice, choices, "cr") >= 0 
				&& choice == 1) {
			/* ok, make it read write */
			simpletext_SetReadOnly(s, FALSE);
			return;
		}
	}
	RunError(msg, iar);
}


#define sRunError(msg, iar) (NSPstore = NSP, RunError(msg, iar))

#define PTSTOMARK(arg,iar)  ((TType)((struct nessmark *)arg)->header.nessmark_methods \
		== nessmarkHdr) ? TRUE :   \
		sRunError(":Not a pointer to a mark (uninitialized variable?)", iar);

#define PUSHMARK(src) { \
		NSP = (union stackelement *)((unsigned long)NSP - sizeof(struct nessmark));  \
		nessmark_InitFrom(&NSP->m, src); }


/* interpretNess(func, ness)  -- interpret the code for the ness function func

	'func' is the location value returned by makeFunction
	presumably a compiler has put bytecodes within this mark.

	A single marker argument is supplied to the top level function.

	'ness' is used for GetArbiter,  GetDefaultText,  and GetCelScope
*/
	struct errornode *
interpretNess(func, arg, ness)
	short func;
	struct nessmark *arg;
	struct ness *ness;
{
	register union stackelement *NSP;	/* stack pointer */
	register unsigned char *iar;	/* next opcode to execute */
	unsigned char *iarzero;	/* point to first byte of object code */
	unsigned char *PrevAddr;	/* former value of iar before goto or call */
	long objend;		/* position of next opcode to interpret */
	long nextiar;		/* beginning of func */
	long lengthgotten;
	long CondCode;		/* set by compares; tested by branches
					0: EQ    1:GT   -1:LT   -2:error */
	int (*oldBus)(), (*oldSeg)(); /* save signal handlers */
	boolean Success;
	char *ErrMsg;
	long gocount;

	if (InterpretationInProgress != NULL) 
		/* XXX recursive entries don't work yet */
		return NULL;	
	
	/* XXX The next three lines move the gap to the end of ObjectCode */
	objend = simpletext_GetLength(ObjectCode);
	simpletext_InsertCharacters(ObjectCode, objend, " ", 1);
	simpletext_DeleteCharacters(ObjectCode, objend, 1);
	NSP = NSPbase;
	FramePtr = &NSPbase->f;

	InterpretationInProgress = ness;
	NSPstore = NSP;

	nextiar = nessmark_GetPos(&SysMarkLowEnd[func]);
	iar = (unsigned char *)simpletext_GetBuf(ObjectCode, nextiar, 
		objend - nextiar, &lengthgotten);
	iarzero = iar - nextiar;		/* for debugging */
	if (lengthgotten < objend-nextiar) 
		return LocateErrorFunc(iar, iarzero, 
			":Object code management failure !!  Quit soon!", ness);

	PrevAddr = NULL;	/* this will terminate the while loop */
	/* the first frame will be built by the EnterFunction operator at the beginning
		of the function */

	if (arg != NULL) 
		PUSHMARK(arg);
	
	if ((ErrMsg=(char *)setjmp(ExecutionExit)) != 0) {

		/* return here from longjmp */
		/* ( even a normal exit uses longjmp) */

		struct errornode *msg;
		signal(SIGBUS, oldBus);
		signal(SIGSEGV, oldSeg);

		InterpretationInProgress = NULL;

		if (ErrMsg == (char *)-1)  
			/* normal exit */
			msg = NULL;
		else 
			msg = LocateErrorFunc(errIAR, iarzero, ErrMsg, ness);

		/* pop stack.  Necessary to free up marks to text */
		NSP = NSPstore;
		while (FramePtr != NULL)
			NSP = popValue(NSP);
		return msg;
	}
	else {
		/* normal setjmp return location */

gocount = 0;

/* catch disasterous errors */
oldBus = signal(SIGBUS, SigHandler);
oldSeg = signal(SIGSEGV, SigHandler);

while (TRUE)  {
switch(*iar++) {
	default:  sRunError(":Illegal Opcode.  Compiler failure!", iar-1);   break;
	case '\n':	break;	/* No-Op */
	case '^': 		/* push NULL */
		NSP = (union stackelement *)(((unsigned long)NSP) - sizeof(struct ptrstkelt));
		NSP->p.hdr = ptrHdr;
		NSP->p.v = NULL;
		break;
	case '_':		/* unary minus */
		if (NSP->l.hdr != longHdr)
			sRunError(":operand is not an integer value", iar-1);
		NSP->l.v = - NSP->l.v; 
		break;
	case '+':
	case '-':
	case '*':	
	case '/':
	case '%':  {
		register struct longstkelt *left ;
		if (NSP->l.hdr != longHdr)
			sRunError(":right operand is not an integer value", iar-1);
		left = &(&(NSP->l))[1];
		if (left->hdr != longHdr)
			sRunError(":left operand is not an integer value", iar-1);
		switch (*(iar-1)) {
		case '+':  left->v += NSP->l.v;   break;
		case '-':  left->v -= NSP->l.v;   break;
		case '*':  left->v *= NSP->l.v;   break;
		case '/':  if (NSP->l.v == 0) sRunError(":divide by zero", iar-1); 
			else left->v /= NSP->l.v;   break;
		case '%':  if (NSP->l.v == 0) sRunError(":divide by zero", iar-1); 
			else left->v %= NSP->l.v;   break;
		}
		NSP = popValue(NSP);	/* discard right operand */
	}	break;
	case '0': 		/* push zero long */
		NSP = (union stackelement *)(((unsigned long)NSP) - sizeof(struct longstkelt));
		NSP->l.hdr = longHdr;
		NSP->l.v = 0;
		break;
	case '1': 		/* push TRUE */
		NSP = (union stackelement *)(((unsigned long)NSP) - sizeof(struct boolstkelt));
		NSP->b.hdr = boolHdr;
		NSP->b.v = TRUE;	/* use C version for now */
		break;
	case '9': 		/* push FALSE */
		NSP = (union stackelement *)(((unsigned long)NSP) - sizeof(struct boolstkelt));
		NSP->b.hdr = boolHdr;
		NSP->b.v = FALSE;	/* use C version for now */
		break;

	case 'a':		/* branch LT */
		Success = (CondCode == -1);
		goto brancher;
	case 'b':		/* branch GT */
		Success = (CondCode == 1);
		goto brancher;
	case 'c':		/* branch LE */
		Success = (CondCode != 1);
		goto brancher;
	case 'd':		/* branch GE */
		Success = (CondCode != -1);
		goto brancher;
	case 'e':		/* branch EQ */
		Success = (CondCode == 0);
		goto brancher;
	case 'f':		/* branch NE */
		Success = (CondCode != 0);
		goto brancher;
	case 'g':		/* goto */
		Success = TRUE;
		goto brancher;
	case 'h':		/* branch error */
		Success = (CondCode == -2);
		goto brancher;

brancher: {
		long offset;
		unsigned char chi, clo;
		if (gocount++ >1000) {
			if (im_CheckForInterrupt()) 
				sRunError(":Interrupted !", iar - 3);
			else gocount = 0;
		}
		chi = *iar++;
		clo = *iar++;
		offset = ExtendShortSign((chi<<8) + clo);
		if (Success) {
			/* set iar to loc indicated by offset from addr of branch */
			PrevAddr = iar - 3;
			iar = PrevAddr + offset;
		}
	}	break;

	case 'i':	{	/* read named file to stack top */
		struct text *t;
		FILE *f;
		unsigned char *s;
		long val;

		PTSTOMARK(NSP, iar-1);	/* file name */
		/* open the file */
		s = nessmark_ToC(&NSP->m);
		f = fopen(s, "r");
		if (f == NULL) {
			char *msg = malloc(strlen(s)+40);
			sprintf(msg, "*readfile is unable to open file \"%s\"", s);
			free(s);
			sRunError(msg, iar-1);
		}
		NSP = popValue(NSP);		/* discard filename */
		t = text_New();		

		/* create a new mark for the file to read into */
		NSP = (union stackelement *)((unsigned long)NSP - sizeof(struct nessmark));
		nessmark_Initialize(&NSP->m);
		nessmark_Set(&NSP->m, t, 0, 0);

		/* now read the file into mark on top of stack */
		val = ReadTextFileStream(t, s, f);
		if (val != dataobject_NOREADERROR) {
			char *msg = malloc(strlen(s)+70);
			sprintf(msg, "*readfile format failure %d for file \"%s\"", val, s);
			free(s);
			sRunError(msg, iar-1);
		}
		fclose(f);
		free(s);
		nessmark_SetLength(&NSP->m, text_GetLength(t));
	}	break;
	case 'j':	{	/* print string */
		register long i, end;
		register struct simpletext *t;
		PTSTOMARK(NSP, iar-1);
		t = nessmark_GetText(&NSP->m);
		i = nessmark_GetPos(&NSP->m);
		end = i + nessmark_GetLength(&NSP->m);
		while (i < end)
			putchar(simpletext_CorrectGetChar(t, i)), i++;
		/* leave value on stack because print is a function and all such return values. */
	}	break;
	case 'k':	{	/* load string from SysMark */
		register unsigned long strloc;
		strloc = (unsigned long)*iar++ << 8;
		strloc += (unsigned long)*iar++;

		PTSTOMARK(&SysMarkLowEnd[strloc], iar-3);
		PUSHMARK(&SysMarkLowEnd[strloc]);
	}	break;
	case 'l':	{	/* load string from stack */
			/* operand is index of arg from FramePtr */
		register struct nessmark *m 
			= (struct nessmark *)((unsigned long)FramePtr
					+ sizeof(struct frameelt) 
					+ (unsigned long)*iar++);
		PTSTOMARK(m, iar-2);
		PUSHMARK(m);
	}	break;
	case 'm':	{	/* dup */
		register struct nessmark *m;
		PTSTOMARK(NSP, iar-1);
		m = &NSP->m;	/* copy pointer to mark before NSP changes */
		PUSHMARK(m);	
	}	break;
	case 'n':	{	/* string next() */
		register long pos;
		PTSTOMARK(NSP, iar-1);
		nessmark_SetPos(&NSP->m, pos = (nessmark_GetPos(&NSP->m) +
			nessmark_GetLength(&NSP->m)));
		nessmark_SetLength(&NSP->m, 
			(pos < simpletext_GetLength(
				nessmark_GetText(&NSP->m))) 
			? 1 : 0);
	}	break;
	case 'o':		/* string start() */
		PTSTOMARK(NSP, iar-1);
		nessmark_SetLength(&NSP->m, 0);
		break;
	case 'p':		/* string base() */
		PTSTOMARK(NSP, iar-1);
		nessmark_SetPos(&NSP->m, 0);
		nessmark_SetLength(&NSP->m,
			simpletext_GetLength(nessmark_GetText(&NSP->m)));
		break;
	case 'q':	{	/* string newbase() */
		NSP = (union stackelement *)((unsigned long)NSP - sizeof(struct nessmark));
		nessmark_Initialize(&NSP->m);
		nessmark_SetText(&NSP->m, text_New());
	}	break;
	case 'r':	{	/* string replace()  top arg is 2nd operand*/
		register struct nessmark *left, *right;
		struct simpletext *stext;
		right = &NSP->m;
		left = &(&(NSP->m))[1];
		PTSTOMARK(right, iar-1);
		PTSTOMARK(left, iar-1);
		stext = nessmark_GetText(left);
		if (simpletext_GetReadOnly(stext)) {
			NSPstore = NSP;
			QueryReadOnly(ness, stext, 
				":cannot replace any part of constant", iar-1);
		}

		nessmark_Replace(left, right);

		NSP = popValue(NSP);	/* discard right operand */
	}	break;
	case 's':	{	/* store string to stack */
			/* operand is index of arg from FramePtr */
		register struct nessmark *m		/* where to store */
			= (struct nessmark *)((unsigned long)FramePtr 
					+ sizeof(struct frameelt)
					+ (unsigned long)*iar++);
		PTSTOMARK(&NSP->m, iar-2);

		if ((TType)m->header.nessmark_methods != nessmarkHdr) {
			/* we are storing into an area which was not a mark */	
			nessmark_InitFrom(m, &NSP->m);
		}
		else 
			nessmark_SetFrom(m, &NSP->m);
		NSP = popValue(NSP);
	}	break;
	case 't':	{	/* compare strings */
		register long i, j, k;
		long len, ilen, jlen, d;
		register struct simpletext *itext, *jtext;
		register struct nessmark *left, *right;

		right = &NSP->m;
		left = &(&(NSP->m))[1];
		PTSTOMARK(right, iar-1);
		PTSTOMARK(left, iar-1);

		i = nessmark_GetPos(left);
		ilen = nessmark_GetLength(left);
		j = nessmark_GetPos(right);
		jlen = nessmark_GetLength(right);
		len = ilen;  if (jlen < len) len = jlen;	/* len = MIN(ilen, jlen); */
		itext = nessmark_GetText(left);
		jtext = nessmark_GetText(right);

		for (k=0  ;  k < len;  k++, i++, j++)
			if ((d = ((long)simpletext_CorrectGetChar(itext, i)) 
					- ((long)simpletext_CorrectGetChar(jtext, j))) != 0)
				break;
		if (k == len) 
			/* both equal until go off the end of one 
				the longer is the greater */
			CondCode = (ilen > jlen)  ?  1  :  (ilen == jlen)  ?  0  :  -1;
		else 
			/* reached an unequal character */
			CondCode = (d>0)  ?  1  :  -1;

		NSP = popValue(NSP);	/* discard right operand */
		NSP = popValue(NSP);	/* discard left operand */
	}	break;
	case 'u':		/* compare string to EMPTY  (is = or >) */
		PTSTOMARK(&NSP->m, iar-1);
		CondCode = (nessmark_GetLength(&NSP->m) == 0)  ?  0  :  1;
		NSP = popValue(NSP);	/* discard operand */
		break;
	case 'v':	{	/*store string to SysMark */
		register struct nessmark *m;
		register unsigned long strloc;
		strloc = (unsigned long)*iar++ << 8;
		strloc += (unsigned long)*iar++;
		m = &SysMarkLowEnd[strloc];	/* where to store */

		PTSTOMARK(&NSP->m, iar-3);	/* check the value to be stored */

		if ((TType)m->header.nessmark_methods != nessmarkHdr) {
			/* we are storing into an area which was not a nessmark */
			nessmark_InitFrom(m, &NSP->m);
		}
		else
			nessmark_SetFrom(m, &NSP->m);
		NSP = popValue(NSP);	/* discard value stored */
	}	break;
	case 'w':	{	/* previous(), nextn(), length()*/
		unsigned char subop = *iar++;
		long n;
		switch (subop) {
		case 'p':
			PTSTOMARK(&NSP->m, iar-2);
			if (nessmark_GetPos(&NSP->m) > 0) {
				nessmark_SetPos(&NSP->m, 
					nessmark_GetPos(&NSP->m)  - 1);
				nessmark_SetLength(&NSP->m, 1);
			}
			else nessmark_SetLength(&NSP->m, 0);
			break;
		case 'n':
			if (NSP->l.hdr != longHdr)
				sRunError(":tried to nextn with non-integer value", iar - 2);
			n = NSP->l.v;
			NSP = popValue(NSP);
			/* leave mark on stack and revise its value */
			PTSTOMARK(&NSP->m, iar-2);
			nessmark_NextN(&NSP->m, n);
			break;
		case 'l':
			PTSTOMARK(&NSP->m, iar-2);
			n = nessmark_Length(&NSP->m);
			NSP = popValue(NSP);
			/* push n */
			NSP = (union stackelement *)((unsigned long)NSP 
						- sizeof(struct longstkelt)); 
			NSP->l.hdr = longHdr;
			NSP->l.v = n;
			break;
		}
	}	break;
	case 'x':	{	/* string extent()  top arg is 2nd operand*/
		register struct nessmark *left, *right;
		right = &NSP->m;
		left = &(&(NSP->m))[1];
		PTSTOMARK(right, iar-1);
		PTSTOMARK(left, iar-1);
		if (nessmark_GetText(left) != nessmark_GetText(right))
			nessmark_Set(left, EmptyText, 0, 0);
		else {
			register start = nessmark_GetPos(left);
			register end = nessmark_GetPos(right) 
				+ nessmark_GetLength(right);
			if (end < start)
				start = end;
			nessmark_SetPos(left, start);
			nessmark_SetLength(left, end - start);
		}
		NSP = popValue(NSP);	/* discard right operand */
	}	break;
	case 'y':			/*  pop  */
		NSP = popValue(NSP);
		break;
	case 'z':	{		/* swap top two operand strings */
		register struct nessmark *second, *topmost;
		struct nessmark tempmark;
		topmost = &NSP->m;
		second = &(&(NSP->m))[1];
		PTSTOMARK(topmost, iar-1);
		PTSTOMARK(second, iar-1);
		if (nessmark_GetText(second) == nessmark_GetText(topmost)) {
			tempmark = *second;
			*second = *topmost;
			*topmost = tempmark;
			/* same base, unswap the links */
			nessmark_SetNext(topmost, (struct mark *)second);
			nessmark_SetNext(second, nessmark_GetNext(&tempmark));
		}
		else {
			struct simpletext *topt, *sect;
			topt = nessmark_GetText(topmost);
			sect = nessmark_GetText(second);
			/* different bases, redo links from texts */
			nessmark_DetachFromText(topmost);
			nessmark_DetachFromText(second);
			tempmark = *second;
			*second = *topmost;
			*topmost = tempmark;
			nessmark_AttachToText(topmost, sect);
			nessmark_AttachToText(second, topt);
		}
	}	break;	

	case 'A':	{	/* append top value on stack to second */
		register struct nessmark *source, *rcvr;
		long rcvrlen;
		struct simpletext *stext;

		source = &NSP->m;
		rcvr = &(&(NSP->m))[1];
		PTSTOMARK(rcvr, iar-1);
		PTSTOMARK(source, iar-1);
		stext = nessmark_GetText(rcvr);
		if (simpletext_GetReadOnly(stext)) {
			NSPstore = NSP;
			QueryReadOnly(ness, stext, ":cannot append to constant", iar-1);
		}

		/* ensure that rcvr is the finish of its base */
		rcvrlen = simpletext_GetLength(stext);
		nessmark_SetPos(rcvr, rcvrlen);
		nessmark_SetLength(rcvr, 0);

		/* put source into rcvr and compute base()*/
		nessmark_Replace(rcvr, source);
		nessmark_SetPos(rcvr, 0);
		nessmark_SetLength(rcvr, rcvrlen + nessmark_GetLength(source));

		NSP = popValue(NSP);	/* discard source */
	}	break;
	case 'B':	{	/* boolean operations */
		unsigned char *opiar = iar-1;
		unsigned char op = *iar++;
		struct boolstkelt *src, *dest;
		register unsigned long sysloc;
		switch (op) {
		case 'k':		/* load from sysmark */
			sysloc = (unsigned long)*iar++ << 8;
			sysloc += (unsigned long)*iar++;
			src = (struct boolstkelt *)&SysMarkLowEnd[sysloc];
			goto Bload;
		case 'l':		/* load from stack */
			src = (struct boolstkelt *)((unsigned long)FramePtr
					+ sizeof(struct frameelt) 
					+ (unsigned long)*iar++);
		Bload:
			if (src->hdr != boolHdr)
				sRunError(":tried to load non-Boolean value", opiar);
			NSP = (union stackelement *)((unsigned long)NSP 
						- sizeof(struct boolstkelt)); 
			NSP->b = *src;
			break;
		case 's':		/* store to stack */
			dest = (struct boolstkelt *)((unsigned long)FramePtr
					+ sizeof(struct frameelt) 
					+ (unsigned long)*iar++);
			goto Bstore;
		case 'v':		/* store to sysmark */
			sysloc = (unsigned long)*iar++ << 8;
			sysloc += (unsigned long)*iar++;
			dest = (struct boolstkelt *)&SysMarkLowEnd[sysloc];
		Bstore:
			if (NSP->b.hdr != boolHdr)
				sRunError(":tried to store non-Boolean value", opiar);
			*dest = NSP->b;
			NSP = (union stackelement *)((unsigned long)NSP 
						+ sizeof(struct boolstkelt));
			break;
		case 't':		/* compare */
			src = &(&(NSP->b))[1];
			if (NSP->b.hdr != boolHdr)
				sRunError(":right operand is not a boolean value", opiar);
			if (src->hdr != boolHdr)
				sRunError(":left operand is not a boolean value", opiar);
			CondCode = (NSP->b.v == src->v) ? 0 :
					(src->v < NSP->b.v) ? -1 : 1;
			NSP = popValue(NSP);	/* discard right operand */
			NSP = popValue(NSP);	/* discard left operand */
			break;
		}
	}	break;
	case 'C':	{	/* call an unknown function on the object atop stack */
			/* operand is four bytes giving address of a callnode  */
		register union {
			struct callnode *call;
			unsigned long symloc;
		} pcall;
		unsigned char c0, c1, c2, c3;
		c0 = *iar++;	/* hi end */
		c1 = *iar++;
		c2 = *iar++;
		c3 = *iar++;	/* lo end */
		pcall.symloc = (c0 << 24) | (c1 << 16) | (c2 << 8) | c3;

		NSPstore = NSP;
		PrevAddr = iar;
		if (pcall.call->variety == callSym)
			/* go find out what is being called and check arg types
				This will CHANGE pcall.call->variety */
			callCheck(pcall.call, NSP, iar-5, ness);

		/* now execute function as C code or Ness code */
		if (pcall.call->variety == callNess) {
			/* (XXX) (we could modify the object code to 
				be a direct call 'O' instead of indirect 'C') */
			nextiar = nessmark_GetPos(&SysMarkLowEnd
						[pcall.call->where.Nproc]);
			iar = (unsigned char *)simpletext_GetBuf(ObjectCode, nextiar, 1, &lengthgotten);
		}
		else {
			/* callC, callPE, callGet, callSet, 
				callMethod, callClProc */
			NSP = callCfunc(pcall.call, NSP, iar-5, ness);
		}
	}	break;
	case 'E':		/* load to stack top a pointer to the current textview object */
		if (ness_GetDefaultText(ness) == NULL)
			sRunError(":there is no default text", iar-1);
		NSP = (union stackelement *)((unsigned long)NSP 
						- sizeof(struct ptrstkelt));
		NSP->p.hdr = ptrHdr;
		NSP->p.v = (struct basicobject *)ness_GetDefaultText(ness); 
		break;
	case 'F': {		/* one of the search functions */
		unsigned char op = *iar++;
		NSP = (union stackelement *)
				SearchOp(op, NSPstore=NSP, iar-2);
	}	break;
	case 'G':	 {	/* make inset have input focus */
		struct view *v;
		if (NSP->p.hdr != ptrHdr || NSP->p.v == NULL 
				|| ! class_IsType(NSP->p.v, viewClass)) 
			 sRunError(":Arg was not a pointer to a view", iar-1);
		else v = (struct view *)NSP->p.v;
		NSP = popValue(NSP);	/* discard view pointer */
		view_WantInputFocus(v, v);	  /* try to give it the input focus */
	}	break;
	case 'H': {	/* unary operations on real values */
		unsigned char op = *iar++;
		NSP = (union stackelement *)realUnary(op, NSPstore=NSP, iar-2);
	}	break;
	case 'I':	{	/* integer operations */
		unsigned char *opiar = iar-1;
		unsigned char op = *iar++;
		register struct longstkelt *src, *dest;
		register unsigned long sysloc;
		register long t;
		switch (op) {
		case 'k':		/* load from sysmark */
			sysloc = (unsigned long)*iar++ << 8;
			sysloc += (unsigned long)*iar++;
			src = (struct longstkelt *)&SysMarkLowEnd[sysloc];
			goto Iload;
		case 'l':		/* load from stack */
			src = (struct longstkelt *)((unsigned long)FramePtr
					+ sizeof(struct frameelt) 
					+ (unsigned long)*iar++);
		Iload:
			if (src->hdr != longHdr)
				sRunError(":tried to load non-integer value", opiar);
			NSP = (union stackelement *)((unsigned long)NSP 
						- sizeof(struct longstkelt)); 
			NSP->l = *src;
			break;
		case 's':		/* store to stack */
			dest = (struct longstkelt *)((unsigned long)FramePtr
					+ sizeof(struct frameelt) 
					+ (unsigned long)*iar++);
			goto Istore;
		case 'v':		/* store to sysmark */
			sysloc = (unsigned long)*iar++ << 8;
			sysloc += (unsigned long)*iar++;
			dest = (struct longstkelt *)&SysMarkLowEnd[sysloc];
		Istore:
			if (NSP->l.hdr != longHdr)
				sRunError(":tried to store non-integer value", opiar);
			*dest = NSP->l;
			NSP = (union stackelement *)((unsigned long)NSP 
						+ sizeof(struct longstkelt));
			break;
		case 't':		/* compare */
			src = &(&(NSP->l))[1];
			if (NSP->l.hdr != longHdr)
				sRunError(":right operand is not an integer value", opiar);
			if (src->hdr != longHdr)
				sRunError(":left operand is not an integer value", opiar);
			t = src->v - NSP->l.v;
			CondCode = (t == 0) ? 0 : (t<0) ? -1 : 1;
			NSP = popValue(NSP);	/* discard right operand */
			NSP = popValue(NSP);	/* discard left operand */
			break;
		}
	}	break;
	case 'J':	{	/* call one of various object functions */
		unsigned char op = *iar++;
		NSP = callCheat(op, NSPstore=NSP, iar-2, ness);
	}	break;
	case 'K':	{	/* dokeys(obj, keys)  or DoMenu(obj, item) */
		struct view *OldIF;
		struct ptrstkelt *p;
		struct view *t;
		struct im *im;
		unsigned char *option;
		unsigned char op = *iar++;

		PTSTOMARK(&NSP->m, iar-2);	    /* check option (the 2nd arg) */
		p = (struct ptrstkelt *)((unsigned long)NSP + sizeof(struct nessmark));
		t = (struct view *)p->v;
		if (p->hdr != ptrHdr  || t == NULL 
				|| ! class_IsType(t, viewClass))
			 sRunError(":first argument was not a view", iar-2);
		/* XXX can't this use ProperPtr ? */
		if (t == ness->CurrentInset  &&  class_IsType(t, celviewClass)) {
			/* is celview:  use application or truechild */
			if (celview_GetApplication((struct celview *)t) != NULL)
				t = celview_GetApplication((struct celview *)t);
			else if (celview_GetTrueChild((struct celview *)t) != NULL)
				t = celview_GetTrueChild((struct celview *)t);
		}
		im = view_GetIM(t);
		if (im == NULL) goto pop1;
		OldIF = im_GetInputFocus(im);
		if (t != OldIF) {
			/* try to give the input focus to t */
			view_WantInputFocus(t, t);
			if (im_GetInputFocus(im) != t)
				/* t did not get IF.  Give up ? XXX */
				goto pop1;
		}
		option = nessmark_ToC(&NSP->m);
		if (op == 'k')
			im_DoKeySequence(im, option);
		else if (op == 'm')
			im_DoMenu(im, option);
		free(option);

		if (t != OldIF)
			/* restore input focus */
			view_WantInputFocus(OldIF, OldIF);
	pop1:
		NSP = popValue(NSP);	/* discard operand, leave one value to pop */
	}	break;
	case 'L':	{	/* convert value to string */
		char buf[50];
		if (NSP->p.hdr == ptrHdr)  {
			if (NSP->p.v == NULL)
				sprintf(buf, "NULL");
			else  sprintf(buf, "0x%lx", NSP->p.v);
		}
		else if (NSP->b.hdr == boolHdr) {
			if (NSP->b.v == TRUE) strcpy(buf, "TRUE");
			else if (NSP->b.v == FALSE) strcpy(buf, "FALSE");
			else sprintf(buf, "0x%lx", NSP->b.v);
		}
		else if (NSP->l.hdr == longHdr)
			sprintf(buf, "%d", NSP->l.v);
		else if (NSP->d.hdr == dblHdr) {
			char *bx;			
#if !defined(VAX_ENV) && !defined(PMAX_ENV)
#if (! SY_U5x && ! SY_AIXx)
			if (isnan(NSP->d.v) == 1)
				sprintf(buf, "*NaN*");
			else
#endif /* (!SYSV && !AIX) */
#endif /* !defined(VAX_ENV) && !defined(PMAX_ENV) */
			    sprintf(buf, "%0.5g", NSP->d.v);
			for (bx = buf; *bx != '\0' 
					&& *bx != '.'
					&& *bx != 'e'  
					&& *bx != 'N';    bx++)  {}
			if (*bx == '\0')
				/* value does not indicate it is real */
				strcat(buf, ".");   /* make it real */
		}
		else if ((TType)NSP->m.header.nessmark_methods == nessmarkHdr) {
			/* leave the arg as the value
				XXX should probably make a copy  */
			break;
		}
		else sRunError(":cannot convert value to string", iar-1);
		NSP = popValue(NSP);	/* discard value */
		NSP = (union stackelement *)((unsigned long)NSP - sizeof(struct nessmark));
		nessmark_Initialize(&NSP->m);
		nessmark_MakeConst(&NSP->m, buf);
	}	break;
	case 'M': {	/* operations on real numbers */
		unsigned char op = *iar++;
		NSP = (union stackelement *)realOther(op, NSPstore=NSP, iar-2);
	}	break;
	case 'N':		/* print a newline */
		putchar('\n');
		break;
	case 'O':	{	/* call a function */
			/* operand is two bytes giving index into SysMark */
		register unsigned long funcloc;
		long lengthgotten;
		if (gocount++ >1000) {
			if (im_CheckForInterrupt()) 
				sRunError(":Interrupted !", iar - 3);
			else gocount = 0;
		}
		funcloc = (unsigned long)*iar++ << 8;
		funcloc += (unsigned long)*iar++;
		PrevAddr = iar;
		nextiar = nessmark_GetPos(&SysMarkLowEnd[funcloc]);
		iar = (unsigned char *)simpletext_GetBuf(ObjectCode, nextiar, 1, &lengthgotten);
	}	break;
	case 'P':	{	/* enter a function */
			/* operand is number of bytes of locals */
		register unsigned long NlocBytes = (unsigned long)*iar++;
		unsigned long *t = (unsigned long *)NSP;

		FuncIAR = iar-2;	/* for SigHandler() */

		NSP = (union stackelement *)((unsigned long)NSP - NlocBytes);
		if (NSP - FUNCSTACKSLOP <= NSLowEnd)
			sRunError(":stack overflow on function entry", iar-2);
		/* zero out the locals */
			/* bzero(NSP, NlocBytes); */
		while (--t >= (unsigned long *)NSP) 
			*t = NULL; 
		NSP = (union stackelement *)((unsigned long)NSP - sizeof(struct frameelt));
					
		NSP->f.hdr = frameHdr;
		NSP->f.returnaddress = PrevAddr;  
		NSP->f.prevframe = FramePtr;
		FramePtr = &NSP->f;
	}	break;
	case 'Q':  {	/* return from function call */
		register long eltsize;
		boolean ismark = FALSE;
		struct simpletext *oldtext;
		register union stackelement *tsp = NSP,
			*targ = (union stackelement *)(((unsigned long)FramePtr) 
					+ sizeof(struct frameelt)
					+(unsigned long)*iar++);
		iar = FramePtr->returnaddress;
		switch (tsp->l.hdr) {
		      case (longHdr):
			eltsize = sizeof(struct longstkelt);  break;
		      case (boolHdr):
			eltsize = sizeof(struct boolstkelt);  break;
		      case (dblHdr):
			eltsize = sizeof(struct dblstkelt);  break;
		      case (ptrHdr):
			eltsize = sizeof(struct ptrstkelt);  break;
		      default:
			if (tsp->l.hdr == nessmarkHdr) {
				ismark = TRUE;
				eltsize = sizeof(struct nessmark);
				oldtext = nessmark_GetText(&tsp->m);
				nessmark_DetachFromText(&tsp->m);
		     	}
			else 
				/* ERROR - unknown value type; return 0 */
				eltsize = 0;
			break;
		}

		/* pop everything off stack, unlinking marks
			and resetting FramePtr  */
		/* but first skip over return value */
		NSP = (union stackelement *)((unsigned long)NSP + eltsize); 
		while (NSP < targ) 
			NSP = popValue(NSP);

		if (eltsize > 0) {
			/* copy return value, last word first */
			long nwords = eltsize/sizeof(unsigned long);
			register unsigned long *src, *dest;
			src = (unsigned long *)tsp + nwords;
			dest = (unsigned long *)NSP;
			NSP = (union stackelement *)((unsigned long) NSP - eltsize);
			/* it is slower to unroll the following loop
				because it fits in the instruction cache */
			while (src > (unsigned long *)tsp) 
				 *--dest = *--src;
			if (ismark) nessmark_AttachToText(&NSP->m, oldtext);
		}
		else {
			/* unknown type, push 0L */
			NSP = (union stackelement *)(((unsigned long)NSP) 
					- sizeof(struct longstkelt));
			NSP->l.hdr = longHdr;
			NSP->l.v = 0;
		}
		if (iar == 0) {
			NSPstore = NSP;
			longjmp(ExecutionExit, (char *)-1);
		}
	} 	break;	
	case 'R': {	/* operations on real numbers */
		unsigned char *opiar = iar-1;
		unsigned char op = *iar++;
		register struct dblstkelt *src, *dest;
		register unsigned long sysloc;
		double t;
		switch (op) {
		case 'k':		/* load from sysmark */
			sysloc = (unsigned long)*iar++ << 8;
			sysloc += (unsigned long)*iar++;
			src = (struct dblstkelt *)&SysMarkLowEnd[sysloc];
			goto Rload;
		case 'l':		/* load from stack */
			src = (struct dblstkelt *)((unsigned long)FramePtr
					+ sizeof(struct frameelt) 
					+ (unsigned long)*iar++);
			goto Rload;
		Rload:
			if (src->hdr != dblHdr)
				sRunError(":tried to load non-real value", opiar);
			NSP = (union stackelement *)((unsigned long)NSP 
						- sizeof(struct dblstkelt)); 
			NSP->d = *src;
			break;
		case 's':		/* store to stack */
			dest = (struct dblstkelt *)((unsigned long)FramePtr
					+ sizeof(struct frameelt) 
					+ (unsigned long)*iar++);
			goto Rstore;
		case 'v':		/* store to sysmark */
			sysloc = (unsigned long)*iar++ << 8;
			sysloc += (unsigned long)*iar++;
			dest = (struct dblstkelt *)&SysMarkLowEnd[sysloc];
			goto Rstore;
		Rstore:
			if (NSP->d.hdr != dblHdr)
				sRunError(":tried to store non-real value", opiar);
#if !defined(VAX_ENV) && !defined(PMAX_ENV)
#if (! SY_U5x && ! SY_AIXx)
			if (isnan(NSP->d.v) == 1)
				sRunError(":tried to store NaN value", opiar);
#endif /* (!SYSV && !AIX) */
#endif /* !defined(VAX_ENV) && !defined(PMAX_ENV) */
			*dest = NSP->d;
			NSP = (union stackelement *)((unsigned long)NSP 
						+ sizeof(struct dblstkelt));
			break;
		case 't':		/* compare */
			src = &(&(NSP->d))[1];
			if (NSP->d.hdr != dblHdr)
				sRunError(":right operand is not a real value", opiar);
			if (src->hdr != dblHdr)
				sRunError(":left operand is not a real value", opiar);
			t = src->v - NSP->d.v;
			CondCode = (t == 0) ? 0 : (t<0) ? -1 : 1;
			NSP = popValue(NSP);	/* discard right operand */
			NSP = popValue(NSP);	/* discard left operand */
			break;
		}
	}	break;
	case 'S':	{	/* get current selection as a marker */
		struct textview *v;
		unsigned char *opiar = iar-1;
		unsigned char op = *iar++;
		if (NSP->p.hdr != ptrHdr ||  
				(v=(struct textview *)ProperPtr((struct basicobject *)NSP->p.v,
						textviewClass)) == NULL) 
			 sRunError(":Arg was not a pointer to a textview", opiar);
		NSP = popValue(NSP);	/* discard view pointer */
		NSP = (union stackelement *)((unsigned long)NSP - sizeof(struct nessmark));
		nessmark_Initialize(&NSP->m);
		if (v == NULL)  {}
		else if (op == 's') {
			/* currentselection(textview) */
			nessmark_Set(&NSP->m, 
				(struct simpletext *)v->header.view.dataobject,
				textview_GetDotPosition(v),
				textview_GetDotLength(v));
		}
		else {
			/* op == 'm' :  currentmark(textview) */ 
			nessmark_Set(&NSP->m, 
				(struct simpletext *)v->header.view.dataobject,
				mark_GetPos(v->atMarker),
				mark_GetLength(v->atMarker));
		}
	}	break;	
	case 'T':		/* test TOS for TRUE (or FALSE) */
		if (NSP->b.hdr != boolHdr) 
			sRunError(":Not a pointer to a mark", iar);
		CondCode = (NSP->b.v == TRUE) ? 0 : -1;
		NSP = popValue(NSP);	/* discard value */
		break;
	case 'U':	{	/* call a user interface information function */
		unsigned char op = *iar++;
		NSP = neventInfo(op, NSPstore=NSP, iar-2, ness);
	}	break;
	case 'V':	{	/* pointer operations */
		unsigned char *opiar = iar-1;
		unsigned char op = *iar++;
		register struct ptrstkelt *src, *dest;
		register unsigned long sysloc;
		register long t;
		switch (op) {
		case 'k':		/* load from sysmark */
			sysloc = (unsigned long)*iar++ << 8;
			sysloc += (unsigned long)*iar++;
			src = (struct ptrstkelt *)&SysMarkLowEnd[sysloc];
			goto Vload;
		case 'l':		/* load from stack */
			src = (struct ptrstkelt *)((unsigned long)FramePtr
					+ sizeof(struct frameelt) 
					+ (unsigned long)*iar++);
		Vload:
			if (src->hdr != ptrHdr)
				sRunError(":tried to load non-pointer value", opiar);
			NSP = (union stackelement *)((unsigned long)NSP 
						- sizeof(struct ptrstkelt)); 
			NSP->p = *src;
			break;
		case 's':		/* store to stack */
			dest = (struct ptrstkelt *)((unsigned long)FramePtr
					+ sizeof(struct frameelt) 
					+ (unsigned long)*iar++);
			goto Vstore;
		case 'v':		/* store to sysmark */
			sysloc = (unsigned long)*iar++ << 8;
			sysloc += (unsigned long)*iar++;
			dest = (struct ptrstkelt *)&SysMarkLowEnd[sysloc];
		Vstore:
			if (NSP->p.hdr != ptrHdr)
				sRunError(":tried to store non-pointer value", opiar);
			*dest = NSP->p;
			NSP = (union stackelement *)((unsigned long)NSP 
						+ sizeof(struct ptrstkelt));
			break;
		case 't':		/* compare */
			src = &(&(NSP->p))[1];
			if (NSP->p.hdr != ptrHdr)
				sRunError(":right operand is not a pointer value", opiar);
			if (src->hdr != ptrHdr)
				sRunError(":left operand is not a pointer value", opiar);
			t = ((long)src->v) - ((long)NSP->p.v);
			CondCode = (t == 0) ? 0 : (t<0) ? -1 : 1;
			NSP = popValue(NSP);	/* discard right operand */
			NSP = popValue(NSP);	/* discard left operand */
			break;
		}
	}	break;
	case 'W':{		/* write second stack elt to file named by topmost */
				/* the writefile() function in ness has file name as 1st arg
					a 'swap' is compiled for that function
					prior to the call on this operator,
					so the contents can be returned as
					the value of the writefile */
		register struct nessmark *contents, *filename;
		struct text *t, *tempt;
		unsigned char *s;
		FILE *f;
		long pos, len;

		filename = &NSP->m;
		contents = &(&(NSP->m))[1];
		PTSTOMARK(contents, iar-1);
		PTSTOMARK(filename, iar-1);

		/* open the file */
		f = fopen(s=nessmark_ToC(filename), "w");
		free(s);
		if (f == NULL) 
			sRunError(":Could not open file for writing", iar-1);

		/* write contents to file */
		pos = nessmark_GetPos(contents);
		len = nessmark_GetLength(contents);
		t = (struct text *)nessmark_GetText(contents);
		if (environment_NumberOfChildren(t->rootEnvironment) > 0
				&& t->templateName == NULL) {
			/*  There's at least one style, and no template.  Read one.  */
			struct stylesheet *s =text_GetStyleSheet(t);
			char *sstpl;
			text_ReadTemplate(t, 
				(s != NULL
					&&  (sstpl=stylesheet_GetTemplateName(s)) 
						!= NULL
					&&  *sstpl != '\0')
				    ? sstpl : "default",      FALSE);
		}

		if (pos == 0  &&  len == text_GetLength(t)) 
			/* write the whole thing */
			text_Write(t, f, im_GetWriteID(), 0);
		else {
			/* copy to a temp text to do the substring */
			tempt = text_New();
			text_AlwaysCopyText(tempt, 0, t, pos , len);
			text_Write(tempt, f, im_GetWriteID(), 0);
			text_Destroy(tempt);
		}
		fclose(f);
		NSP = popValue(NSP);	/* discard filename */

	}	break;
	case 'X':	 {		/* system() */
		unsigned char *buf, *combuf;
		FILE *output;
		unsigned char frombuf[1025];
		struct simpletext *t;
		long i, end;
				
		PTSTOMARK(&NSP->m, iar-1);
		buf = nessmark_ToC(&NSP->m);
		i = strlen(buf);
		combuf = (unsigned char *)malloc(i+20);
		sprintf(combuf, "(%s)</dev/null", buf);		/* supply empty stdin */
		free(buf);
		NSP = popValue(NSP);	/* discard the argument */

		/* create a new mark for the file to read into 
			XXX most of this is from readfile() do we want a common subr? */
		NSP = (union stackelement *)((unsigned long)NSP - sizeof(struct nessmark));
		nessmark_Initialize(&NSP->m);
		t = simpletext_New();
		nessmark_Set(&NSP->m, t, 0, 0);

		output = popen(combuf, "r");
		free(combuf);

		end = 0;
		if (output != NULL) {
			/* now read the file into mark on top of stack */
			while ((i=fread(frombuf, sizeof(*frombuf), sizeof(frombuf), output)) > 0) {
				simpletext_InsertCharacters(t, end, frombuf, i);
				end += i;
			}
			pclose(output);
		}
		else {
			static char *msg = "ERROR: Could not execute command";
			simpletext_InsertCharacters(t, end, msg, strlen(msg));
			end += strlen(msg);
		}
		nessmark_SetLength(&NSP->m, end);
	}	break;

	case 'Y':	{	/*  setcurrentselection(textview, mark) */
		struct ptrstkelt *p;
		struct textview *t;
		PTSTOMARK(&NSP->m, iar-1);
		p = (struct ptrstkelt *)((unsigned long)NSP + sizeof(struct nessmark));
		if (p->hdr != ptrHdr  ||
				(t=(struct textview *)ProperPtr((struct basicobject *)p->v,
					textviewClass)) == NULL)
			 sRunError(":first argument was not a textview", iar-1);
		if ((struct simpletext *)t->header.view.dataobject
					== nessmark_GetText(&NSP->m))  {
			textview_SetDotPosition(t, nessmark_GetPos(&NSP->m));
			textview_SetDotLength(t, nessmark_GetLength(&NSP->m));
			textview_FrameDot(t, nessmark_GetPos(&NSP->m));
		}
		else
			 sRunError(":marker was not for text in the given object", iar-1);
		NSP = popValue(NSP);	/* discard selection marker */
		NSP = popValue(NSP);	/* discard object */
	}	break;
	case 'Z':		/* last() */
		PTSTOMARK(&NSP->m, iar-1);
		if (nessmark_GetLength(&NSP->m) > 0) {
			nessmark_SetPos(&NSP->m, 
				nessmark_GetPos(&NSP->m)
					+ nessmark_GetLength(&NSP->m)
					-1);
			nessmark_SetLength(&NSP->m, 1);
		}
		break;

}	/* end switch(*iar++) */
}	/* end while(TRUE) */
}	/* end else */

}	/* end interpretNess() */

