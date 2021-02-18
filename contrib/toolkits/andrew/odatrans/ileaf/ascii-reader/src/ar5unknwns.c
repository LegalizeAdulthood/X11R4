
/*
 * Copyright 1989 by Interleaf, Inc, 10 Canal Park,  Cambridge, MA. 02141
 *
 *		All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Interleaf not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Interleaf, Inc. makes no representations 
 * about the suitability of this software for any purpose. 
 * It is provided "AS IS" without express or implied warranty.
 *
 * INTERLEAF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL INTERLEAF BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * 
 */

/*************************************************/
/*	ar5unknowns.c                               */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  11/88    BT	Original version
 */

/**--------------------------------------------------------------------------**/
/**	These routines recognize 'unkown' declarations and properties.	     **/

/**--------------------------------------------------------------------------**/
#ifdef INTERLEAF_INCLUDE_FILES
#include "cdefs.h"
#include "sysstdio.h"
#else
#ifndef AR_DEFINES
#define AR_DEFINES
typedef short bool;
typedef short rsu;
typedef long Rsu;
typedef short shar;
typedef unsigned short ushar;
#endif
#endif
#include "ar5defs.h"
#include "ar5private.h"
/**--------------------------------------------------------------------------**/
/**--------------------------------------------------------------------------**/
NullObj_t *ArBuildUnknownDecl(fiP)
  File_info_t *fiP;
{
  Unknown_t *unkP = NEW(Unknown_t);
  register shar c;

    SET_NullObjFields(unkP, ID_UnknownDecl);
    unkP->text = SsInitEmpty();
    while ((c = ArGetch(fiP)) != AR_CMD_TERM && c != EOF) {
	SsSaveS(c, &unkP->text);
    }
    SsSaveS('\0', &unkP->text);
    if (ARdebug > 0) {
	ArFprintf(stderr, "ArBuildUnknownDecl() ==> %Q\n", 
		  SsBufStart(&unkP->text));
    }
    return( (NullObj_t *)unkP );
}

/**--------------------------------------------------------------------------**/
/** Used to build an unknown property from the input.  The keyword and the   **/
/** equals-sign have been read by the caller; the keyword is pointed to by   **/
/** sP.  The list of previously read unknowns is in unkP.  This one	     **/
/** is built and added to the FRONT of the list (for simplicity).	     **/
/** the order of the list should not matter; if it does, this whole method   **/
/** will not reliably work.						     **/
/** NOTE there is no trailing comma in the text.  The print routine supplies **/
/** it, since it is a delimiter.					     **/
/** NOTE also, that if the prop ends with a ',', it is consumed, but if it   **/
/** ends with a '>', it is pushed back and the caller will consume it.	     **/

Unknown_t *ArBuildUnknownPropEq(fiP, sP, unkP)
  File_info_t *fiP;
  shar *sP;
  Unknown_t *unkP;
{
  Unknown_t *my_unkP = NEW(Unknown_t);
  register shar c;

    SET_NullObjFields(my_unkP, ID_UnknownProp);
    my_unkP->nextP = (NullObj_t *)unkP;	/** list is built in reverse. **/
    my_unkP->text = SsInitEmpty();
    SsSaveShr(sP, &my_unkP->text);
    SsSaveS(' ', &my_unkP->text);
    SsSaveS('=', &my_unkP->text);
    SsSaveS(' ', &my_unkP->text);
    while ((c = ArGetch(fiP)) != AR_PROP_TERM && c != AR_CMD_TERM && c != EOF) {
	SsSaveS(c, &my_unkP->text);
    }
    SsSaveS('\0', &my_unkP->text);
    if (c == AR_CMD_TERM) {
	ArUnGetch(c, fiP);
    }
    if (ARdebug > 0) {
	ArFprintf(stderr, "ArBuildUnknownPropEq() ==> %Q\n", 
		  SsBufStart(&my_unkP->text));
    }
    return( my_unkP );
}
/**--------------------------------------------------------------------------**/
/** Like ArBuildUnknownPropEq(), except a prfix string is added to the prop  **/
/** name.								     **/

Unknown_t *ArBuildUnknownPropPrefixEq(fiP, prefix, sP, unkP)
  File_info_t *fiP;
  int prefix;
  shar *sP;
  Unknown_t *unkP;
{
  Unknown_t *my_unkP = NEW(Unknown_t);
  register shar c;
  static shar left  [] = { 'L','e','f','t',         0 };
  static shar right [] = { 'R','i','g','h','t',     0 };
  static shar top   [] = { 'T','o','p',             0 };
  static shar bottom[] = { 'B','o','t','t','o','m', 0 };

    SET_NullObjFields(my_unkP, ID_UnknownProp);
    my_unkP->nextP = (NullObj_t *)unkP;	/** list is built in reverse. **/
    my_unkP->text = SsInitEmpty();
    switch(prefix) {
    case LD_LEFT:
	SsSaveShr(left, &my_unkP->text);
	break;
    case LD_RIGHT:
	SsSaveShr(right, &my_unkP->text);
	break;
    case LD_TOP:
	SsSaveShr(top, &my_unkP->text);
	break;
    case LD_BOTTOM:
	SsSaveShr(bottom, &my_unkP->text);
	break;
    case NO_MATCH:
    default:
	break;
    }
    SsSaveShr(sP, &my_unkP->text);
    SsSaveS(' ', &my_unkP->text);
    SsSaveS('=', &my_unkP->text);
    SsSaveS(' ', &my_unkP->text);
    while ((c = ArGetch(fiP)) != AR_PROP_TERM && c != AR_CMD_TERM && c != EOF) {
	SsSaveS(c, &my_unkP->text);
    }
    SsSaveS('\0', &my_unkP->text);
    if (c == AR_CMD_TERM) {
	ArUnGetch(c, fiP);
    }
    if (ARdebug > 0) {
	ArFprintf(stderr, "ArBuildUnknownPropEq() ==> %Q\n", 
		  SsBufStart(&my_unkP->text));
    }
    return( my_unkP );
}
/**--------------------------------------------------------------------------**/
/**  ... this is used in ArBuildAnStreamInfo(), where keywords are rare....  **/
/** Used to build an unknown property from the input.  Whatever is there,    **/
/** it is not a keyword.  save everything up to a ',' or '>'.		     **/
/** The list of previously read unknowns is in unkP.  This one	     	     **/
/** is built and added to the FRONT of the list (for simplicity).	     **/
/** the order of the list should not matter; if it does, this whole method   **/
/** will not reliably work.						     **/
/** NOTE there is no trailing comma in the text.  The print routine supplies **/
/** it, since it is a delimiter.					     **/
/** NOTE also, that if the prop ends with a ',', it is consumed, but if it   **/
/** ends with a '>', it is pushed back and the caller will consume it.	     **/

Unknown_t *
ArBuildUnknownProp(fiP, unkP)
  File_info_t *fiP;
  Unknown_t *unkP;
{
  Unknown_t *my_unkP = NEW(Unknown_t);
  register shar c;

    SET_NullObjFields(my_unkP, ID_UnknownProp);
    my_unkP->nextP = (NullObj_t *)unkP;	/** list is built in reverse. **/
    my_unkP->text = SsInitEmpty();
    while ((c = ArGetch(fiP)) != AR_PROP_TERM && c != AR_CMD_TERM && c != EOF) {
	SsSaveS(c, &my_unkP->text);
    }
    SsSaveS('\0', &my_unkP->text);
    if (c == AR_CMD_TERM) {
	ArUnGetch(c, fiP);
    }
    if (ARdebug > 0) {
	ArFprintf(stderr, "ArBuildUnknownProp() ==> %Q\n", 
		  SsBufStart(&my_unkP->text));
    }
    return( my_unkP );
}

/**--------------------------------------------------------------------------**/
void
ArUnknownWrite(unkP, ofile)
  Unknown_t *unkP;
  FILE *ofile;
{
    for ( ; unkP != NULL ; unkP = (Unknown_t *)unkP->nextP) {
	if (SsAmount(&unkP->text) > 0) 
	{ 
	    switch (Identify(unkP)) {
	    case ID_UnknownDecl:
		ArFprintf(ofile, UNKNOWN_DECL_FMT, SsBufStart(&unkP->text));
		break;
	    case ID_UnknownProp:
		ArFprintf(ofile, UNKNOWN_PROP_FMT, SsBufStart(&unkP->text));
		break;
	    default:
		/**  should never get here. **/
		ArFprintf(stderr, 
		   "Unexpected object type %d in ArUnknownWrite(). Get help.\n",
		   Identify(unkP) );
		break;	
	    }
	}
    }
}
/**--------------------------------------------------------------------------**/
void
ArUnknownFree(unkP)
  Unknown_t *unkP;
{
  NullObj_t *nextP;

    for ( ; unkP != NULL; ) {
	nextP = unkP->nextP;
	SsFree(&unkP->text);
	FREE((char *)unkP);
	unkP = (Unknown_t *)nextP;
    }
}
/**--------------------------------------------------------------------------**/
