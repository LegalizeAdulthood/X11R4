/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/ckcontent.c,v 1.2 89/09/08 17:25:57 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/ckcontent.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/ckcontent.c,v 1.2 89/09/08 17:25:57 mss Exp $";
#endif

/*
  $Log:	ckcontent.c,v $
 * Revision 1.2  89/09/08  17:25:57  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:14:18  mss
 * Initial revision
 * 
 * Revision 1.7  89/04/30  12:06:30  mss
 * Changed to use new ATK naming conventions
 * 
 * Revision 1.6  88/11/04  17:02:37  mss
 * Changed warning message so that it only appears when
 * debugging
 * 
 * Revision 1.5  88/11/02  16:55:53  mss
 * Added equation and table content types
 * 
 * Revision 1.4  88/10/11  22:00:09  mss
 * Changed name of GetContentType back;
 * rearrange include fiels to define void for vax
 * 
 * Revision 1.3  88/10/10  18:51:32  mss
 * renamed getcontenttype
 * 
 * Revision 1.2  88/09/22  10:59:10  mss
 * FIxed to use new LookUPObjectID
 * 
 * Revision 1.1  88/09/07  11:10:40  mss
 * Initial revision
 * 


*/

#include <odatk.h>
#include <class.h>
#include <ckcontent.h>
#include <odalib.ih>
#include <odadef.h>
#include <txtstvec.h>
#include <odaatk.h>
#define trace 1

void static CheckForSpecialContent(gD, specConstituent, contentType, contentMode)
p_odainfo_type gD;
CONSTITUENT specConstituent;
INT_type *contentType, *contentMode;{

    at_APPLICATION_COMMENTS_type   contentComment;
    INT_type			    status;

    if (gD->doDebugging) {
	printf("Entering CheckForSpecialContent\n");
    }

    /* See if we have an equation or table but sneaking a peek at the application-comments */
    status = odalib_GetAttr(specConstituent, at_APPLICATION_COMMENTS, (POINTER_type) &contentComment, /* Inherit */ BOOL_true, Ignore_Parms);

    *contentType = CharContent;
    *contentMode = ProcessableContent;

    if ((status != ERROR_INT) && (5 == contentComment->length) && (0==strncmp("Table",contentComment->sequence_value.bytes, 5))){
	*contentType = TableContent;
    }
    if ((status != ERROR_INT) && (8 == contentComment->length) && (0==strncmp("Equation",contentComment->sequence_value.bytes, 8))){
	*contentType = EquationContent;
    }

    if (gD->doDebugging) {
	printf("Leaving CheckForSpecialContent\n");
    }

}
void GetContentType(gD, specConstituent, contentType, contentMode)
p_odainfo_type gD;
CONSTITUENT specConstituent;
INT_type *contentType, *contentMode;{

    ASN1_OBJECT_ID_type	    curArchClass;
    VALUE_type		    curContentType;

    if (gD->doDebugging) {
	printf("Entering GetContentType\n");
    }

    { static int firstTime = 1;
      if (firstTime && gD->doDebugging) {
	  printf(">>>Ignoring content type and only looking at content architecture\n");
	  firstTime = 0;
      }
    }
    /* See if a content architecture class is present */
    

    if (ERROR_INT != odalib_GetAttr(specConstituent, at_CONTENT_ARCH_CLASS, (POINTER_type) &curArchClass, Use_Inherited, Ignore_Parms)) {
	/* Has a value, so use it  -- first check for special values */
	switch (odalib_LookUpObjectId(curArchClass)) {
	    case FORM_CC_id:
		*contentType = CharContent;
		*contentMode = FormattedContent;
		break;
	    case PROC_CC_id:
		CheckForSpecialContent(gD, specConstituent, contentType, contentMode);
		break;
	    case FP_CC_id:
		*contentType = CharContent;
		*contentMode = FormattedProcContent;
		break;
	    case FORM_RG_id:
		*contentType = RasterContent;
		*contentMode = FormattedContent;
		break;
	    case FP_RG_id:
		*contentType = RasterContent;
		*contentMode = FormattedProcContent;
		break;
	    case FORM_GG_id:
		*contentType = GeoContent;
		*contentMode = FormattedContent;
		break;
	    default:
		printf("Unknown content architecture class, using text\n");
		*contentType = CharContent;
		*contentMode = ProcessableContent;
	}
	odalib_DeleteSequence(curArchClass);
    }
    else if (ERROR_INT != MaybeGetAttr(specConstituent, at_CONTENT_TYPE, (POINTER_type) &curContentType, Use_Inherited, Ignore_Parms)) {
	printf("No arch class, but content type, assuming character\n");
	*contentType = CharContent;
	*contentMode = ProcessableContent;
	/* No arch class, but we have a content type */
    }
    else {
	/* nothing there -- make something up */
	printf("missing both content architecture class and content type, using text\n");
	*contentType = CharContent;
	*contentMode = ProcessableContent;
    }
}

