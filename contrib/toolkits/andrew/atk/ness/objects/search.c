/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/search.c,v 1.16 89/09/20 23:50:43 wjh Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/search.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/search.c,v 1.16 89/09/20 23:50:43 wjh Exp $";
#endif

/* 
	search.c  -  implement searches and pattern matches

	Entry points:

	DoSearch(code) - does the search indicated by code
*/
/*
 * $Log:	search.c,v $
 * Revision 1.16  89/09/20  23:50:43  wjh
 * fix firstobject for happybday.d
 * 
 * Revision 1.15  89/09/17  08:56:29  wjh
 * fix span function so it doesn't fail at end of text
 * 
 * Revision 1.14  89/09/03  22:49:04  wjh
 * newness
 * 
 * Revision 1.13  89/06/23  17:23:47  wjh
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
 * Revision 1.12  89/06/01  16:02:02  wjh
 * campus release version
 * 
 * Revision 1.5  88/12/20  19:47:13  wjh
 * fixed various bugs
 * 
 * Revision 1.4  88/12/08  15:53:08  wjh
 * fix a bug in HasStyles()
 * 
 * Revision 1.3  88/12/07  22:44:51  wjh
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
 * Revision 1.2  88/11/02  14:43:07  wjh
 * fixed bugs with byte swapping
 * added some more corrected copyrights
 * 
 * Revision 1.1  88/10/21  11:02:53  wjh
 * Initial revision
 * 
 * Creation 0.0  88/06/01 10:02:00  wjh
 * Initial creation by WJHansen
 * 
*/

#include <andrewos.h>	/* for bzero() */
#include <dataobj.ih>
#include <text.ih>
#include <textv.ih>
#include <viewref.ih>
#include <dict.ih>
#include <envrment.ih>
#include <stylesht.ih>
#include <style.ih>
#include <cel.ih>

#include <lex.ih>
#include <nessmark.ih>
#include <xgetchar.h>
#include <interp.h>		/* for frameelt for envt.h */
#include <envt.h>
#include <nevent.h>	/* for ProperPtr */

/* XXX  KLUDGE.  The following definition should be exported by text.ch */
#define TEXT_VIEWREFCHAR  '\377'


/* conventions for search-type functions:
	the subject in which the search is made is the first argument
	if this is an empty string, the search extends to the end of the base
		otherwise the search extends only over the argument string
	For success the return value is the non-empty substring matching the 
		criteria.
	For failure, the return value is the empty string at the end of the 
		original subject string.  (If the original subject were empty
		the return value for failure is that original subject.)
*/


/* the string location of most recent ParseInt, ParseReal, or FirstObject */
static struct nessmark *WhereItWas = NULL;


static boolean CharTable[256];	/* assumed initially zero */
	/* the CharTable is used to test for characters in sets */

/* SetCharTable(pat)
	Set to TRUE each element of CharTable that is in pat.
*/
	static void
SetCharTable(pat)
	struct nessmark *pat;
{
	register struct simpletext *ptext;
	register long pos, end;
	ptext = nessmark_GetText(pat);
	pos = nessmark_GetPos(pat);
	end = pos + nessmark_GetLength(pat);
	for ( ; pos < end; pos++)
		CharTable[simpletext_CorrectGetChar(ptext, pos)] = TRUE;
}

/* ClearCharTable(pat)
	Set to FALSE each element of CharTable.
	As an optimization, the 'pat' arg must list all elts of table which are TRUE.
*/
	static void
ClearCharTable(pat)
	struct nessmark *pat;
{
	register struct simpletext *ptext;
	register long pos, end, len;
	len = nessmark_GetLength(pat);
	if (len > 20) {
		bzero(CharTable, sizeof(CharTable));
		return;
	}
	ptext = nessmark_GetText(pat);
	pos = nessmark_GetPos(pat);
	end = pos + len;
	for ( ; pos < end; pos++)
		CharTable[simpletext_CorrectGetChar(ptext, pos)] = FALSE;
}

	void
search(subject, pat)
	struct nessmark *subject, *pat;
{
	register unsigned char first;
	register struct simpletext *stext, *ptext;
	register long pos, end, patpos, patlen;
	long spos, slen;

	ptext = nessmark_GetText(pat);
	patpos = nessmark_GetPos(pat);
	patlen = nessmark_GetLength(pat);
	stext = nessmark_GetText(subject);
	spos = nessmark_GetPos(subject);
	slen = nessmark_GetLength(subject);
	end = ((slen == 0) ? simpletext_GetLength(stext) : spos + slen)
			- patlen;

	first = simpletext_CorrectGetChar(ptext, patpos);

	for (pos = spos; pos <= end; pos++)  
		if (first == simpletext_CorrectGetChar(stext, pos)) {
			/* found first char. try to match rest of pat */
			register long t;
			for (t = 1;  t < patlen && 
					    simpletext_CorrectGetChar(stext, pos+t)
					    == simpletext_CorrectGetChar(ptext, patpos+t); 
					t++)
				{}
			if (t == patlen) {
				/* bingo */
				nessmark_SetPos(subject, pos);
				nessmark_SetLength(subject, patlen);
				return;
			}
		}
	/* fail: return empty mark at end of subject */
	nessmark_SetPos(subject, spos + slen);
	nessmark_SetLength(subject, 0);
}

/* match(subject, pat)
	checks subject to see if it begins with pat.  
	If so, returns that substring;  otherwise returns finish(subject)
	The match will fail unless the subject is of length zero or some 
		length longer than the pattern.
*/
	void
match(subject, pat)
	struct nessmark *subject, *pat;
{
	register struct simpletext *stext, *ptext;
	register long pos, end, patpos, patend;
	long spos, slen;

	stext = nessmark_GetText(subject);
	spos = nessmark_GetPos(subject);
	slen = nessmark_GetLength(subject);
	end = (slen == 0) ? simpletext_GetLength(stext) : spos+slen;
	ptext = nessmark_GetText(pat);
	patpos = nessmark_GetPos(pat);
	patend = patpos + nessmark_GetLength(pat);

	pos = spos;
	if (end-pos >= patend-patpos)
		while (patpos < patend 
				&& simpletext_CorrectGetChar(stext, pos)
				== simpletext_CorrectGetChar(ptext, patpos))
			pos++, patpos++;
	if (patpos == patend) {
		/* succeed */
		nessmark_SetLength(subject, nessmark_GetLength(pat));
	}
	else {
		/* fail */
		nessmark_SetPos(subject, spos + slen);
		nessmark_SetLength(subject, 0);
	}
}

	void
anyof(subject, pat)
	struct nessmark *subject, *pat;
{
	register struct simpletext *stext;
	register long pos, end;
	long slen, spos;

	stext = nessmark_GetText(subject);
	spos = nessmark_GetPos(subject);
	slen = nessmark_GetLength(subject);
	end = (slen == 0) ? simpletext_GetLength(stext) : spos + slen;

	SetCharTable(pat);

	for (pos = spos; pos < end; pos++)
		if (CharTable[simpletext_CorrectGetChar(stext, pos)]) 
			break;
	if (pos < end) {
		/* bingo */
		nessmark_SetPos(subject, pos);
		nessmark_SetLength(subject, 1);
	}
	else {
		/* nope */
		nessmark_SetPos(subject, spos + slen);
		nessmark_SetLength(subject, 0);
	}

	ClearCharTable(pat);
}

	void
span(subject, pat)
	struct nessmark *subject, *pat;
{
	register struct simpletext *stext;
	register long pos, end;
	long spos, slen;

	stext = nessmark_GetText(subject);
	spos = nessmark_GetPos(subject);
	slen = nessmark_GetLength(subject);
	end = (slen == 0) ? simpletext_GetLength(stext) : spos + slen;

	SetCharTable(pat);

	for (pos = spos; pos < end; pos++)
		if ( ! CharTable[simpletext_CorrectGetChar(stext, pos)]) 
			break;

	if (pos > spos)
		nessmark_SetLength(subject, pos - spos);
	else {
		nessmark_SetPos(subject, spos + slen);
		nessmark_SetLength(subject, 0);
	}
	
	ClearCharTable(pat);
}

	void
token(subject, pat)
	register struct nessmark *subject, *pat;
{
	register struct simpletext *stext;
	long slen, spos;
	stext = nessmark_GetText(subject);
	spos = nessmark_GetPos(subject);
	slen = nessmark_GetLength(subject);

	anyof(subject, pat);
	if (nessmark_GetLength(subject) == 0)
		return;

	/* revise subject to extend to the end of the base 
			or the end of the original subject */
	nessmark_SetLength(subject, 
		((slen == 0) ? simpletext_GetLength(stext) : spos + slen)
			- nessmark_GetPos(subject));
	span(subject, pat);
}

#undef environment_GetParent
#define environment_GetParent(env) \
		((struct environment *)(env)->header.nestedmark.parent)


	static void
AddStyles(text, pos, len, env)
	struct text *text;
	long pos, len;
	struct environment *env;
{
	struct style *style;
	struct environment *dad = environment_GetParent(env);
	if (dad != NULL)
		AddStyles(text, pos, len, dad);
	if (env->type == environment_Style) {
		style = stylesheet_Find(text->styleSheet, env->data.style->name);
		if (style == NULL)  {
			/* XXX Why doesn't text_AddStyle do this ??? */
			/* XXX How do we decide we should use default.template */
			style = style_New();
			style_Copy(env->data.style, style);	
			style->template = FALSE;
			stylesheet_Add(text->styleSheet, style); 
		}
		text_AddStyle(text, pos, len, env->data.style);
	}
}



#define PTSTOMARK(arg,iar)  ((TType)((struct nessmark *)arg)->header.nessmark_methods \
		== nessmarkHdr) ? TRUE :   \
		RunError(":not a pointer to a mark (uninitialized variable?)", iar);


/* SearchOp(op, NSP, opiar)
	First five ops perform searches, depending on op.
	Each search routine modifies its first argument marker to indicate the
		result of searching for the second.
	Other operations parse strings, handle objects, and process styles.
*/
	union stackelement *
SearchOp(op, NSP, opiar)
	unsigned char op;
	union stackelement *NSP; 
	unsigned char *opiar;	/* iar of the opcode */
{

	/* these definitions are global because GDB can't get at local decls. */

	struct nessmark *pat, *subject;
	boolean boolval;
	long intval;
	double realval;
	unsigned char *cstring;
	struct basicobject *objval;
	long pos, len;
	struct text *text, *pattext;
	struct viewref *vr;
	static boolean Inited = FALSE;
	long success;
	long envpos;
	struct environment *env, *patenv, *tenv;

	if (! Inited) {
		Inited = TRUE;
		WhereItWas = nessmark_New();
	}

	/* check arguments */
	if (op == 'w')	{}		/* no args */
	else {
		/* at least one arg */
		if (op >= 'p') {
			/* this is both one and three arg ops */
			PTSTOMARK(&NSP->m, opiar);
			subject = &NSP->m;
		}
		else {
			/* two marker args */
			PTSTOMARK(&(&(NSP->m))[1], opiar);
			subject = &(&(NSP->m))[1];
			pat = &NSP->m; 
		}
	}
	switch (op) {

	case 'a':  search(subject, pat);  NSP = popValue(NSP);  break;
	case 'c':  anyof(subject, pat);  NSP = popValue(NSP);  break;
	case 'e':  token(subject, pat);  NSP = popValue(NSP);  break;
	case 'd':  span(subject, pat);  NSP = popValue(NSP);  break;
	case 'b':  match(subject, pat);  NSP = popValue(NSP);  break;

	/* {"addstyles", "Fj", {Tstr, Tstr, Tstr, Tvoid}, ness_codeYellow} */
	case 'j':
		/* revise subject to have style of pat */
		text = (struct text *)nessmark_GetText(subject);
		pattext = (struct text *)nessmark_GetText(pat);
		if (class_IsType(text, textClass) 
				&& class_IsType(pattext, textClass))
			AddStyles(text, nessmark_GetPos(subject),
				nessmark_GetLength(subject), 				environment_GetInnerMost(
						pattext->rootEnvironment,
						nessmark_GetPos(pat)));
		
		NSP = popValue(NSP);  /* discard style */
		break;
	/* {"hasstyles", "Fk", {Tbool,Tstr,Tstr,Tvoid}, ness_codeOrange} */
	case 'k':
		/* set boolval indicating whether subject has style of pat */
		boolval = TRUE;

		pattext = (struct text *)nessmark_GetText(pat);
		if ( ! class_IsType(pattext, textClass))
			goto gotval;	/*  no styles: trivially TRUE */
		pos = nessmark_GetPos(pat);
		patenv = environment_GetInnerMost(pattext->rootEnvironment, pos);
		while (patenv != NULL && patenv->type != environment_Style)
			patenv = environment_GetParent(patenv);
		if (patenv == NULL)
			goto gotval;	/*  pat has no styles: trivially TRUE */

		/* now we know pat has at least one style */
		text = (struct text *)nessmark_GetText(subject);
		if ( ! class_IsType(text, textClass)) {
			boolval = FALSE;
			goto gotval;	/*  text is simple: trivially FALSE */
		}
		pos = nessmark_GetPos(subject);
		env = environment_GetInnerMost(text->rootEnvironment, pos);
		while (env != NULL && env->type != environment_Style)
			env = environment_GetParent(env);
		if (env == NULL) {
			boolval = FALSE;
			goto gotval;	/*  text has no styles: trivially FALSE */
		}

		/* now we know text and pattext both have at least one style 
			return TRUE unless we find a style on pat that is
			not on text */
		for ( ; patenv != NULL;  patenv = environment_GetParent(patenv)) {
			if (patenv->type == environment_Style)  {
			    for (tenv = env; tenv != NULL; 
					tenv = environment_GetParent(tenv)) 
				if (tenv->type == environment_Style
					    && tenv->data.style == patenv->data.style)
					break;  /* tenv != NULL */
			    if (tenv == NULL) {
				boolval = FALSE;
				goto gotval;	/*  could not find a pat style:  FALSE */
			    }
			}
		}
		/* found all styles of pat,  return the TRUE set far above */

	gotval:
		NSP = popValue(NSP);  
		NSP = popValue(NSP);  

		/* push boolval */
		NSP = (union stackelement *)(((unsigned long)NSP) 
					- sizeof(struct boolstkelt)); 
		NSP->b.hdr = boolHdr;
		NSP->b.v = boolval;
		break;
	/* {"parseint", "Fp", {Tlong, Tstr, Tvoid}, ness_codeOrange} */
	case 'p':
		/* set intval by scanning subject, use -2**31 for error 
			save matched portion w/ RememberWhereItWas */
		text = (struct text *)nessmark_GetText(subject);
		pos = nessmark_GetPos(subject);

		success = lex_ParseNumber(text, pos, &len, &intval, &realval);
		if (success == 0)
			intval = (1<<31);
		nessmark_Set(WhereItWas, text, pos, len);
		NSP = popValue(NSP);

		/* push intval */
		NSP = (union stackelement *)(((unsigned long)NSP) 
					- sizeof(struct longstkelt)); 
		NSP->l.hdr = longHdr;
		NSP->l.v = intval;
		break;
	/* {"parsereal", "Fq", {Tdbl, Tstr, Tvoid}, ness_codeOrange} */
	case 'q':
		text = (struct text *)nessmark_GetText(subject);
		pos = nessmark_GetPos(subject);

		/* set realval by scanning subject, use NaN for error 
			save matched portion in WhereItWas */
		success = lex_ParseNumber(text, pos, &len, &intval, &realval);

		if (success == 0)
			realval = sqrt(-1.0);
		else if (success == 1)
			realval = intval;
		nessmark_Set(WhereItWas, text, pos, len);
		NSP = popValue(NSP);

		/* push realval */
		NSP = (union stackelement *)(((unsigned long)NSP) 
					- sizeof(struct dblstkelt)); 
		NSP->d.hdr = dblHdr;
		NSP->d.v = realval;
		break;
	/* {"firstobject", "Fr", {Tptr, Tstr, Tvoid}, ness_codeOrange} */
	case 'r':
		/* set objval by scanning subject, use NULL for error 
			save matched location w/ RememberWhereItWas */
		nessmark_MakeConst(WhereItWas, "");	/* EmptyText */
		text = (struct text *)nessmark_GetText(subject);
		objval = NULL;
		if (class_IsType(text, textClass)) {
			len = text_GetLength(text);
			objval = NULL;
			for (pos = nessmark_GetPos(subject); pos < len; pos++) {
				if (text_GetChar(text, pos) 
						!= TEXT_VIEWREFCHAR)
					continue;
				vr = text_FindViewreference(text, pos, 1);
				if (vr == NULL)  continue;

				/* try to get the view, if there is one */
				objval = (struct basicobject *)
						dictionary_LookUp(text, (char *)vr);
				if (objval == NULL || objval == (struct basicobject *)
						textview_UNKNOWNVIEW) {
					objval =  (struct basicobject *)vr->dataObject;
				}
				nessmark_Set(WhereItWas, text, pos, 1);
				break;
			}
		}
		NSP = popValue(NSP);
		/* push objval */
		NSP = (union stackelement *)(((unsigned long)NSP) 
					- sizeof(struct ptrstkelt)); 
		NSP->p.hdr = ptrHdr;
		NSP->p.v = objval;
		break;
	/* {"nextstylegroup", "Fs", {Tstr, Tstr, Tvoid}, ness_codeOrange} */
	case 's':
		/* revise subject, advancing to next larger style group 
				or return start of arg 
		if subject is at start of style and a larger group starts at same place
			return the smallest enclosing style;  
			otherwise return the next succeeding style group 
		A program can find all styles by successive application of
			NextStyleGroup().   */
		text = (struct text *)nessmark_GetText(subject);
		if ( ! class_IsType(text, textClass)) {
			nessmark_Start(subject);
			break;
		}
		pos = nessmark_GetPos(subject);
		len = nessmark_GetLength(subject);
		env = environment_GetInnerMost(text->rootEnvironment, pos);
		while (env != NULL  &&  env->type != environment_Style) 
			env = environment_GetParent(env);

		/* env is the current style environment.  look first for a superior at pos */
		tenv = env;
		while (tenv != NULL  
				&&  pos == (envpos=environment_Eval(tenv)) 
				&& (tenv->type != environment_Style
					|| len >= tenv->header.nestedmark.length)) {
			tenv = environment_GetParent(tenv);
		}
		while (tenv != NULL  &&  tenv->type != environment_Style) {
			tenv = environment_GetParent(tenv);
			if (tenv != NULL)
				envpos = environment_Eval(tenv);
		}
		/* see if there is a larger group at current position */
		if (tenv != NULL && pos == envpos 
				&& len < tenv->header.nestedmark.length) {
			/* this is it ! */
			nessmark_SetLength(subject, 
				tenv->header.nestedmark.length);
			break;
		}

		/* at this point there is no superior style at pos
			we must advance to the next style start 
			Loop till we have a next environment or NULL */
		envpos = pos;
		do {
			envpos += environment_GetNextChange(text->rootEnvironment,
							envpos);
			if (envpos < text_GetLength(text))
				tenv = environment_GetInnerMost(text->rootEnvironment,
						envpos);
			else tenv = NULL;
			while (tenv != NULL  &&  tenv->type != environment_Style) 
				tenv = environment_GetParent(tenv);
			/* The "next change" may be an end or start of an environment 
				It is an end if tenv is a parent of old env */
			while (env != NULL && env != tenv)
				env = environment_GetParent(env);
		} while (env != NULL  &&  tenv != NULL) ;
			/* if env == NULL and tenv does not, we have gone up tree from env 
			and not found tenv.  Therefore, tenv is the next style group */

		/* at this point, tenv is NULL 
			or points to the environment for the next group,
			which starts at envpos */
		if (tenv != NULL) {
			/* got it! */
			nessmark_SetPos(subject, envpos);
			nessmark_SetLength(subject, 
				tenv->header.nestedmark.length);
		}
		else
			nessmark_Start(subject);
		break;

	/* {"enclosingstylegroup","Ft", {Tstr, Tstr, Tvoid}, ness_codeOrange} */
	case 't':
		/* subject must be empty or correspond to a style group
			revise subject, advancing to next outer style group 
			or return start of arg (?)
			entire doc is not a style group unless explicitly set */
		text = (struct text *)nessmark_GetText(subject);
		if ( ! class_IsType(text, textClass)) {
			nessmark_Start(subject);
			break;
		}
		pos = nessmark_GetPos(subject);
		len = nessmark_GetLength(subject);

		/* get an enclosing style group.*/
		env = environment_GetInnerMost(text->rootEnvironment, pos);

		/* be sure the selected text is a style group */
		while (env != NULL  &&  len > env->header.nestedmark.length)
			env = environment_GetParent(env);
		if (env != NULL && len > 0 
				&& (pos != environment_Eval(env)  
				    ||  len !=  env->header.nestedmark.length))
			env = NULL;

		/* scan further up to find enclosing style */
		while (env != NULL && 
				(len == env->header.nestedmark.length  
				|| env->type != environment_Style))
			env = environment_GetParent(env);

		/* now set the mark to the enclosing range */
		if (env != NULL) {
			nessmark_SetPos(subject, environment_Eval(env));
			nessmark_SetLength(subject, env->header.nestedmark.length);
		}
		else /* no enclosing style */
			nessmark_Start(subject);
		break;
	/* {"clearstyles", "Fu", {Tstr, Tstr, Tvoid}, ness_codeOrange} */
	case 'u':

		/* remove all styles that include exactly the subject */
		text = (struct text *)nessmark_GetText(subject);
		if (class_IsType(text, textClass)) {

#if 0	
/* debugging code */	
for (pos = 0; pos <= text_GetLength(text); pos ++) {
	/* print values of getnextchange and getinnermost */
	envpos = environment_GetNextChange(text->rootEnvironment, pos);
	env = environment_GetInnerMost(text->rootEnvironment, pos);
	printf("%d:  + %d    %c   ", pos, envpos, 
		simpletext_CorrectGetChar((struct simpletext *)text, pos+envpos));
	while (env != NULL  &&  env->header.nestedmark.length < 999999999) {
		printf("    %d : %d", environment_Eval(env),
				env->header.nestedmark.length);
		if (env->type != environment_Style)
			printf("(not Style)");
		env = environment_GetParent(env);
	}
	printf("\n");
}
#else
			pos = nessmark_GetPos(subject);
			len = nessmark_GetLength(subject);
			if (len == 0) break;
			if (environment_Remove(text->rootEnvironment,
					pos, len, environment_Style, TRUE))
				text_SetModified(text);
			text_RegionModified(text, pos, len);
			text_NotifyObservers(text, 0);
#endif
		}
		break;

	/* {"whereitwas", "Fw", {Tstr, Tvoid}, ness_codeOrange} */
	case 'w':
		/* push marker WhereItWas onto stack */
		if (WhereItWas == NULL)
			WhereItWas = nessmark_New();
		NSP = (union stackelement *)(((unsigned long)NSP) 
					- sizeof(struct nessmark)); 
		nessmark_Initialize(&NSP->m);
		nessmark_SetFrom(&NSP->m, WhereItWas);
		break;
	/* {"replacewithobject", "Fx", {Tstr, Tstr, Tptr, Tstr, Tvoid},
						ness_codeYellow} */
	case 'x': {
		cstring = nessmark_ToC(subject);   /* get view name */
		NSP = popValue(NSP);	
		/* get object pointer */
		if (NSP->p.hdr != ptrHdr || NSP->p.v == NULL 
				|| (objval=ProperPtr((struct basicobject *)NSP->p.v,
					dataobjectClass)) == NULL)
			RunError(":Item to insert should be a dataobject", 
					opiar);
		NSP = popValue(NSP);
		/* ??? If the pointer is to a cel, and the type is not celview,
			we use the object in the cel.
			XXX should use view_CanView(), but it is not fully implemented */
		if (class_IsType(objval, celClass) && strcmp(cstring, "celview") != 0)
			objval = (struct basicobject *)cel_GetObject((struct cel *)objval);

		PTSTOMARK(&NSP->m, opiar);
		subject = &NSP->m;		/* get text to be replaced */
		/* replace contents of marker now atop stack with
			a viewref for the object pointed at by objval 
			and having view given by cstring */
		text = (struct text *)nessmark_GetText(subject);
		if ( ! class_IsType(text, textClass))
			RunError(":objects cannot go in simple text", opiar);
		pos = nessmark_GetPos(subject);
		len = nessmark_GetLength(subject);
		if (*cstring == '\0') {
			free(cstring);
			cstring = (unsigned char *)freeze(dataobject_ViewName(
					(struct dataobject *)objval  ));
		}

		text_AddView(text, pos+len, cstring, objval);  /* DO IT ! */

		if (len > 0)
			text_DeleteCharacters(text, pos, len);
		/* XXX note that nessmarks do not get the special
			treatment given by the normal ness replace() */
		free(cstring);
		nessmark_SetLength(subject, 1);    /* set returned mark */
	}	break;
	}
	return NSP;
}
