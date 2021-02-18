/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/type/RCS/type.h,v 1.1 89/08/22 15:30:30 wjh Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/type/RCS/type.h,v $ */

#ifndef _type_h_
#define _type_h_
#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
	static char *type_h_rcsid = "$Header:";
#endif

/***************************************************************************\
	type.h

	declarations used between type and parse 

 \***************************************************************************/

/*
 * $Log:	type.h,v $
 * Revision 1.1  89/08/22  15:30:30  wjh
 * Initial revision
 * 
 */

extern struct type *proto;

extern struct text *textBuf, *indexText;

extern struct type_ctypes ctypes;
extern struct type * basicobject_methods;

extern struct lex *lexer;

extern sym_ScopeType garbageScope;

extern sym_ScopeType structs, unions, enums, typedefs;
extern sym_ScopeType grammarScope;

extern char * parse_error;

#endif /* _type_h_ */
