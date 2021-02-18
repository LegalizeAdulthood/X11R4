/* ********************************************************************** *\
 *         Copyright IBM Corporation 1987,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/deskey.c,v 1.2 89/11/03 17:39:47 wjh Exp $ */
/* $ACIS: $ */
/* $Source:*/
#ifndef lint
	char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/deskey.c,v 1.2 89/11/03 17:39:47 wjh Exp $ ";
#endif /* lint */

/* deskey.c -- EZ package to describe key sequences.
 * David Nichols	 * September, 1987 
 * WJHansen	 * July, 1989,    list-proctable: November, 1989

The proctable entry 'deskey-describe-key' can be bound to a key in an .xxxinit file or can be called from Ness as deskey_describe_key().  It prompts for a key sequence and responds with the name of the proctable entry that will be called.  The sequence can be interrupted with control-G.

deskey_DescribeKey(keys) is a non-interactive interface to this package.  It returns the proctable entry for the given key sequence.

WARNING!  This package assumes that it's ok to call keystate_ApplyKey on the im's keystate variable.  This turns out to be true and is a bitch to program around otherwise. 
 */

#include <stdio.h>

#include <im.ih>
#include <keystate.ih>
#include <proctbl.ih>
#include <message.ih>

#include <deskey.eh>

/* Append a space and an ascii representation of the char c to the buffer. */
	static void
AppendKey(buf, ch)
	char *buf;
	char ch;
{
	char *p;

	p = buf + strlen(buf);
	*p++ = ' ';
	if (ch == 033)
		strcpy(p, "ESC");
	else if (ch == 0177)
		strcpy(p, "^?");
	else if (ch == 040)
		strcpy(p, "sp");
	else if (ch < 040) {
		*p++ = '^';
		*p++ = ch + '@';
		*p++ = 0;
	}
	else {
		*p++ = ch;
		*p++ = 0;
	}
}


/* Prompt for a key sequence and show the name and documentation of the function it's bound to. */
	static void
DescribeKey(im)
	struct im *im;
{
	int done = FALSE;
	char buf[1000];
	int c;
	struct proctable_Entry *pte;
	long rock;
	struct basicobject *object;

	strcpy(buf, "Describe key: ");
 	message_DisplayString(im, 0, buf);
	while (!done) {
		c = im_GetCharacter(im);
		if (c == EOF  ||  c == '\007') {
			keystate_Reset(im->keystate);
			message_DisplayString(im, 0, "Describe key aborted.");
			return;
		}
		c &= 0177;
		AppendKey(buf, c);
		switch (keystate_ApplyKey(im->keystate, c, &pte, &rock, &object)) {
			case keystate_ProcFound:
				done = TRUE;
				strcat(buf, " -> ");
				strcat(buf, proctable_GetName(pte));
				strcat(buf, ": ");
				/* Sigh.  Should call proctable_GetDocumentation(pte) 
				    but include file is broken. */
				if (!proctable_Defined(pte))
					strcat(buf, "<Function not yet loaded.>");
				else if (pte->doc == NULL)
					strcat(buf, "<No documentation.>");
				else
					strcat(buf, pte->doc);
				break;
			case keystate_NoBinding:
				done = TRUE;
				strcat(buf, " -> <Key not bound.>");
				break;
			case keystate_Pending:
				break;
		}
		message_DisplayString(im, 0, buf);
	}
}


static long NProcs;

/* ListAProc(pe, f)
	Format the entry 'pe' and print it to 'f'
*/
	static void
ListAProc(pe, f)
	struct proctable_Entry *pe;
	FILE *f;
{
	NProcs++;

	if (pe->name != NULL)
		fprintf(f, "%s", pe->name);
	else fprintf(f, "[unnamed]");

	if(pe->type != NULL)
		fprintf(f, " ( %s )", class_GetTypeName(pe->type));
	else fprintf(f, " ( ??? )");

	switch (pe->returntype) {
		case proctable_Void:  break;
		case proctable_Boolean:   fprintf(f, "=> Boolean");  break;
		case proctable_Char:  fprintf(f, "=> Char");  break;
		case proctable_Short:  fprintf(f, "=> Short");  break;
		case proctable_Long:  fprintf(f, "=> Long");  break;
		case proctable_Double:  fprintf(f, "=> Double");  break;
		case proctable_Object:  fprintf(f, "=> Object");  break;
		case proctable_NessMarker:  fprintf(f, "=> NessMarker");  break;
		case proctable_StaticString:  fprintf(f, "=> StaticString");  break;
		case proctable_DisposeString:  fprintf(f, "=> DisposableString");  break;
	}

	if (pe->doc != NULL)
		fprintf(f, "   %s\n", pe->doc);
	else fprintf(f, "\n");
}

/* ListProctable(im)
	write to /tmp/ProcList a list of all current proctable entries
	Return number of entries written.  
*/
	static long
ListProctable(im)
	struct im *im;
{
	FILE *f;
	NProcs = 0;
	f = popen("sort > /tmp/ProcList", "w");
	if (f == NULL) {
		message_DisplayString(im, 0, "Could not open /tmp/ProcList");
		return NProcs;
	}
	message_DisplayString(im, 0, "Writing /tmp/ProcList");
	im_ForceUpdate();	/* cause the message to appear */
	proctable_Enumerate(ListAProc, f);
	pclose(f);
	message_DisplayString(im, 0, "Wrote /tmp/ProcList");
	return NProcs;
}

	boolean
deskey__InitializeClass(classID)
	struct classheader *classID;
{
	struct classinfo *imc;
	imc = class_Load("im");
	proctable_DefineProc("deskey-describe-key", DescribeKey, imc, NULL, 
			"Show binding of a typed in key sequence.");
	proctable_DefineTypedProc("deskey-list-proctable", ListProctable, imc, NULL, 
			"List the entries in the proctable to /tmp/ProcList;   Return count.",
			proctable_Long);
	return TRUE;
}


/* deskey_DescribeKey(classID, keys, pRock)
	search key bindings for 'keys'
	return the proctable entry that would be called (or NULL if none)
	set *pRock to the rock value that will be passed
*/
	struct proctable_Entry *
deskey__DescribeKey(classID, keys, pRock)
	struct classheader *classID;
	char *keys;
	long *pRock;
{
	char *k = keys;	
	int c;

	struct proctable_Entry *pte;
	long rock;
	struct basicobject *object;

	struct im *im;

	*pRock = 0;
	im = im_GetLastUsed();
	if (im == NULL) return NULL;

	while (TRUE) {
		if (*k == '\0')
			return NULL;
		c = *k++;
		c &= 0177;
		switch (keystate_ApplyKey(im->keystate, c, &pte, &rock, &object)) {
			case keystate_ProcFound:
				*pRock = rock;
				return pte;
			case keystate_NoBinding:
				return NULL;
			case keystate_Pending:
				break;	/* out of switch.   continue in while loop */
		}
	}
}
