
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
/*	ar5free.c                               */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  12/88    BT	Original coding
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
void ArAttrFree(atP)
  Attr_t *atP;
{
  Attr_t *nextP;

    for ( ; atP ; ) {
	nextP = atP->nextP;
	free((char *)atP->nameP);
	free((char *)atP->stringP);
	free((char *)atP);
	atP = nextP;
    }
}
/**--------------------------------------------------------------------------**/
bool 
ArObjectFree( thingP )
  register NullObj_t *thingP;
{
  register bool result = YES;
  static char id_err_msg[] =
      "ArObjectFree(%#x): unknown identifier type %#x.  Get help.\n";
#ifdef UNUSED
  static char table_err_msg[] =
      "ArObjectFree(): can't free a table yet. Get help.\n";
#endif /* UNUSED */

    switch(Identify(thingP)) {
    case ID_Error:
	/** only happens when thingP == NULL. **/
	result = YES;
	break;
    case ID_NullObj:
	/** conceivably, this will orphan some storage.  It's not our fault; **/
	/** this is all that we can do here.				     **/
	result = ArObjectFree(thingP->nextP);
	break;
    /** these three object have only direct storage, and possibly   **/
    /** some unknown-props on their unkP list.			    **/
    /** NOTE that freeing an Il_state_t *does not* recurse on them. **/
    /** they are freed by freeing the whole Il_state_t structure.   **/
    case ID_OpsDecl:
	ArUnknownFree(((Ops_info_t *)thingP)->unkP);
	FREE((char *)thingP);
	result = YES;
	break;
    case ID_Document:
	ArUnknownFree(((Document_info_t *)thingP)->unkP);
	FREE((char *)thingP);
	result = YES;
	break;
    case ID_Page:
	ArUnknownFree(((Page_info_t *)thingP)->unkP);
	FREE((char *)thingP);
	result = YES;
	break;
    case ID_Color:
    case ID_Pattern:
    case ID_Font:
	if ((result = ArObjectFree(thingP->nextP)) == YES) {
	    FREE((char *)thingP);
	}
	break;
    case ID_ClassDef:
    case ID_MasterCmpn:
    case ID_Cmpn:
	result = ArCmpnFree(thingP);
	break;
    case ID_MasterFrame:
    case ID_Frame:
	ArUnknownFree(((Frame_t *)thingP)->unkP);
	result = ArObjectFree(thingP->nextP) &&  
		 ArDiagramFree(&((Frame_t *)thingP)->diagram);
	if (result == YES ) {
	    FREE((char *)(((Frame_t *)thingP)->page_numP));
	    FREE((char *)(((Frame_t *)thingP)->autosizeP));
	    ArAttrFree(((Frame_t *)thingP)->attrvalsP);
	    FREE((char *)(((Frame_t *)thingP)->rulingsP));
	    ArAutonumFree(((Frame_t *)thingP)->anumP);
	    FREE((char *)thingP);
	}
	break;
    case ID_MasterTable:
    case ID_Table:
	ArTableFree((Table_t *)thingP);
	break;
    case ID_MasterRow:
    case ID_Row:
	ArRowFree((Row_t *)thingP);
	break;
    case ID_Cell:
	ArCellFree((Cell_t *)thingP);
	break;
    case ID_HeadFoot:
	ArUnknownFree(((HeaderFooter_t *)thingP)->unkP);
	result = ArObjectFree(thingP->nextP) &&  
		 ArDiagramFree(&((HeaderFooter_t *)thingP)->diagram);
	if (result == YES ) {
	    FREE((char *)thingP);
	}
	break;
    case ID_AutoStream:
	if ((result = ArObjectFree(thingP->nextP)) == YES) {
	    FREE( (char *)(((An_stream_info_t *)thingP)->an_name) );
	    FREE( (char *)thingP );
	}
	break;
    case ID_FileInfo:
	SsFree( &(((File_info_t *)thingP)->inqueue) );
	FREE((char *)thingP);
	result = YES;
	break;
    case ID_State:
	{ register Il_state_t *stateP = (Il_state_t *)thingP;
	    /** the three fixed-size fields are freed as part of stateP,     **/
	    /** below.  but their unkowns must also be freed explicitly.     **/
	    ArUnknownFree(((Ops_info_t *)(&stateP->ops_info))->unkP);
	    ArUnknownFree(((Document_info_t *)(&stateP->doc_info))->unkP);
	    ArUnknownFree(((Page_info_t *)(&stateP->page_info))->unkP);
	    result = ArObjectFree( stateP->fontP          ) &&
		     ArObjectFree( stateP->colorP         ) &&
		     ArObjectFree( stateP->patternP	  ) &&
		     ArObjectFree( stateP->an_streamP     ) &&
		     ArObjectFree( stateP->class_defaultsP) &&
		     ArObjectFree( stateP->cmpnP	  ) &&
		     ArObjectFree( stateP->frameP	  ) &&
		     ArObjectFree( stateP->tableP	  ) &&
		     ArObjectFree( stateP->hdftP	  ) &&
		     ArObjectFree( stateP->bodyP	  );
	     if (result == YES) {
		 FREE( stateP );
	     }
	}
	break;
    case ID_Comment:
	if ((result = ArObjectFree(thingP->nextP)) == YES) {
	    SsFree(&(((Comment_t *)thingP)->text));
	    FREE((char *)thingP);
	}
	break;
    default:
	ArFprintf(stderr, id_err_msg, thingP, *(Identifier_t *)thingP);
	result = NO;
	break;
    }
    return( result );
}

/**--------------------------------------------------------------------------**/
