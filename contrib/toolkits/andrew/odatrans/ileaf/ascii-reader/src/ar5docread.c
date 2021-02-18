
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
/*	ar5read.c                               */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  07/88    BT	Original AR version, derived from alMain.c
 */

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
extern NullObj_t *ArBuildComment();
/**--------------------------------------------------------------------------**/
#if 0	/** BT **/
#define xxxxxxxxxxxxListAddatEnd(listP, thingP) \
{NullObj_t *ptrP; \
if (listP==NULL){(NullObj_t*)listP=(NullObj_t*)thingP;}\
else{for(ptrP=(NullObj_t*)listP; \
	 ptrP->nextP!=NULL;\
	 ptrP=(NullObj_t*)ptrP->nextP){}\
     ptrP->nextP=(NullObj_t*)thingP;}}
#endif	/** BT **/
/**--------------------------------------------------------------------------**/
void
ListAddatEnd(listPP, thingP)
  NullObj_t **listPP;
  NullObj_t *thingP;
{
  NullObj_t *ptrP;

    thingP->nextP = NULL;
    if (*listPP == NULL ) {
	*listPP = thingP;
    }
    else {
	for (ptrP=(*listPP); ptrP->nextP != NULL; ptrP=(NullObj_t*)ptrP->nextP){
	    /** advance to the end. **/
	}
	ptrP->nextP = thingP;
    }
}
/**--------------------------------------------------------------------------**/
NullObj_t *
ArDocRead(fiP, ilP)
  File_info_t *fiP;
  Il_state_t *ilP;
{
  NullObj_t *resultP;
  static char *err_msg =
	"ArDocRead() internal error: ID_Frame returned from ArNextObject()\n";

    if (fiP == NULL || ilP == NULL) {
	return( NULL );
    }
    for ( ; (resultP = ArNextObject(fiP, ilP)) != NULL; ) {
	if (ld_error != NO) {
	    ArFprintf(stderr, "ArDocRead: ld_error==%d.\n", ld_error);
	    return( (NullObj_t *)ilP );
	}
	switch (Identify(resultP)) {
	case ID_NullObj :
	    break;
	case ID_OpsDecl :
	    ilP->ops_info = *(Ops_info_t *)resultP;
	    FREE(resultP);
	    break;
	case ID_Document :
	    ilP->doc_info = *(Document_info_t *)resultP;
	    FREE(resultP);
	    break;
	case ID_Font :
	    ilP->fontP = (Font_info_t *)resultP;
	    break;
	case ID_Color :
	    /* ListAddatEnd(&ilP->colorP, resultP); CK */
	    ilP->colorP = (Color_info_t *)resultP;
	    break;
	case ID_Pattern :
	    /* ListAddatEnd(&ilP->patternP, resultP); CK */
	    ilP->patternP = (Pattern_info_t *)resultP;
	    break;
	case ID_Page :
	    ilP->page_info = *(Page_info_t *)resultP;
	    FREE(resultP);
	    break;
	case ID_MasterFrame :
	    ListAddatEnd(&ilP->frameP, resultP);
	    /** ArFprintf(stderr, "Adding Master Frame %Q\n", 
				((Frame_t *)resultP)->name); BT **/
	    break;
	case ID_Frame :
	    /** won't happen -- a frame is not a top-level object. **/
	    ArFprintf(stderr, err_msg);
	    return( NULL );
	    break;
	case ID_ClassDef :
	    ilP->class_defaultsP = (Cmpn_t *)resultP;
	    break;
	case ID_MasterCmpn :
	    ListAddatEnd(&ilP->cmpnP, resultP);
	    /**/if (ARdebug) {
	      Attr_t *listP;
	      int length = 0;
		ArFprintf(stderr, "Adding %Q to component masters .\n",
				((Cmpn_t *)resultP)->cmpn_name);
		for (listP = ((Cmpn_t*)resultP)->attrvalsP;
		     listP&&listP->nextP;
		     listP=listP->nextP){
		    /** advance to end of list **/
		    ArFprintf(stderr, 
			"                  length %d, nameP %Q, stringP %Q\n",
			length, listP->nameP, listP->stringP);
		    length++;
		}
	    }
	    break;
	case ID_Cmpn :
	    ListAddatEnd(&ilP->bodyP, resultP);
	    /**/if (ARdebug) {
	      Attr_t *listP;
	      int length = 0;
		ArFprintf(stderr, "Adding component %Q to doc body.\n", 
				((Cmpn_t *)resultP)->cmpn_name);
		for (listP = ((Cmpn_t*)resultP)->attrvalsP;
		     listP&&listP->nextP;
		     listP=listP->nextP){
		    /** advance to end of list **/
		    ArFprintf(stderr, 
			"                  length %d, nameP %Q, stringP %Q\n",
			length, listP->nameP, listP->stringP);
		    length++;
		}
	    }
	    break;
	case ID_MasterTable :
	    ListAddatEnd(&ilP->tableP, resultP);
	    break;
	case ID_Table :
	    ListAddatEnd(&ilP->bodyP, resultP);
	    break;
	case ID_HeadFoot :
	    ListAddatEnd(&ilP->hdftP, resultP);
	    /** ArFprintf(stderr, "Adding HeadFoot to doc state.\n"); BT **/
	    break;
	case ID_AutoStream :
	    ListAddatEnd(&ilP->an_streamP, resultP);
	    break;
	case ID_FileInfo :
	    /** won't happen here. **/
	    break;
	case ID_State :
	    /** won't happen here. **/
	    break;
	case ID_Comment :
	    /** this may be out of order, but it is the only safe place to  **/
	    /** preserve comments.  the alternative is to make them vanish. **/
	    ListAddatEnd(&ilP->bodyP, resultP);
	    break;
	default:
	    ArFprintf(stderr, "ArDocRead() got weird Identifier %d\n",
		Identify(resultP));
	    break;
	}
	/*ArFprintf(stderr,"ArDocRead loop: type == %d\n", Identify(resultP));*/
    }
    return( (NullObj_t *)ilP );
}
/**--------------------------------------------------------------------------**/
