/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/attsup.c,v 1.2 89/09/08 17:25:10 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/attsup.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/atk/RCS/attsup.c,v 1.2 89/09/08 17:25:10 mss Exp $";
#endif

/*
  $Log:	attsup.c,v $
 * Revision 1.2  89/09/08  17:25:10  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:14:35  mss
 * Initial revision
 * 
 * Revision 1.8  89/08/25  09:31:12  mss
 * Revmoed routines moved into tool kit
 * 
 * Revision 1.7  89/05/20  20:37:52  mss
 * Remove routines for setting pres/lay styles
 * 
 * Revision 1.6  89/03/23  07:02:50  mss
 * Use new component and DVL specifications
 * 
 * Revision 1.5  88/12/14  10:50:08  mss
 * Changed the use of NULL_CONSTITUENT constant to
 * odalib_NULL_CONSTITUENT() class procedure
 * 
 * Revision 1.4  88/11/29  14:35:22  mss
 * Changed CheckAndGetAttr so that an attribute will be tagged as
 * "not found" on a NULL_CONSTITUENT
 * 
 * Revision 1.3  88/10/10  18:51:04  mss
 * got rid of tracing
 * 
 * Revision 1.2  88/10/01  17:09:49  mss
 * Added comments to automatically generated l/p styles
 * 
 * Revision 1.1  88/09/07  11:07:59  mss
 * Initial revision
 * 
 

*/

/* Attribute support routines to make life easier */
#include <odatk.h>
#include <attsup.h>
#include <class.h>
#include <odalib.ih>
#include <errormsg.h>
#include <odadef.h>

#define tracing 0

INT_type SafeSetAttr(constituent, attribute, value, mask)
    CONSTITUENT constituent;
    INT_type attribute;
    POINTER_type value;
    PARM_MASK_type mask; {

    INT_type retvalue;

    if (ERROR_INT == (retvalue = odalib_SetAttr(constituent, attribute, value, mask))) {
	printf("Error (%s) setting attribute %s (%d) for constituent %x\n", ErrorMsg(odalib_TKError()), odalib_GetAttrName(attribute), attribute, constituent);
    }
    return retvalue;

}

INT_type SafeSetValueAttr(constituent, attribute, value)
    CONSTITUENT constituent;
    INT_type attribute;
    VALUE_type value;
{

    INT_type retvalue;

    if (ERROR_INT == (retvalue = odalib_SetValueAttr(constituent, attribute, value))) {
	printf("Error (%s) setting attribute %s (%d) for constituent %x\n", ErrorMsg(odalib_TKError()), odalib_GetAttrName(attribute), attribute, constituent);
    }
    return retvalue;

}


INT_type AppendCharContent(CurBLO, CurCP, textData)
    CONSTITUENT CurBLO;
    CONSTITUENT CurCP;
    char * textData;
    {
	SEQUENCE_CONSTITUENT_type tempSeq;

	/* Attach CP to BLO -- we always use 1 CP per BLO */
	if (odalib_ERROR_SEQUENCE() == 
	    (tempSeq = odalib_MakeSequence(SEQUENCE_CONSTITUENT_tag, 1))) {
	    printf("Could not create a sequence for adding CPs to BLOs (%s)\n", ErrorMsg(odalib_TKError()));
	    return ERROR_INT;
	}
	tempSeq->sequence_value.constituents[0] = CurCP;
	if (ERROR_INT == odalib_SetAttr(CurBLO, at_CONTENT_PORTIONS, (POINTER_type) tempSeq, ALL_PARMS)) {
	    printf("Could not attach seq of CPs to BLO (%s)\n", ErrorMsg(odalib_TKError()));
	}

	/* And get rid of copy of sequence */
	if (ERROR_INT == odalib_DeleteSequence(tempSeq)) {
	    printf("Could not deallocate content portion sequence (%s)\n", ErrorMsg(odalib_TKError()));
	    /* return ERROR_INT; */
	}

	/* Attach string to CP.  */
	if (ERROR_INT == odalib_SetStringAttr(CurCP, at_CONTENT_INFORMATION, textData, DATA_BYTE_TYPE)) {
	    printf("Could not add content information to content portion (%s)\n", ErrorMsg(odalib_TKError()));
	    return ERROR_INT;
	}

	/* Mark CP as having a text portion */
	if (ERROR_INT == odalib_SetAttr(CurBLO,  at_CONTENT_ARCH_CLASS, (POINTER_type)  odalib_at_CONTENT_ARCH_CLASS_proc_cc(),  ALL_PARMS)) {
	    printf("Could not set content arch class (proc/cc) for BLO (%s)\n", ErrorMsg(odalib_TKError()));
	    /* return ERROR_INT; */
	}

}


INT_type SafeGetAttr(curConstituent, attribute, attributevalue, inherited, parmmask)
CONSTITUENT curConstituent;
INT_type attribute;
POINTER_type attributevalue;
BOOL_type inherited;
PARM_MASK_type *parmmask; {

    switch(odalib_HasAttr(curConstituent, attribute)) {
	    case HAS_ATTR:
		if (ERROR_INT == odalib_GetAttr(curConstituent, attribute, attributevalue, inherited, parmmask)) {
		    printf("Cannot retrieve attribute from constituent (%s)\n", ErrorMsg(odalib_TKError()));
		    return odalib_TKError();
		}
		break;
	    case NO_ATTR:
		printf("Missing expected attribute %s\n", odalib_GetAttrName(attribute));
		return ERROR_INT;
		/* break; */
	    case ERROR_INT:
	    default:
		printf("Error %s getting attribute %s (%d)\n", ErrorMsg(odalib_TKError()), odalib_GetAttrName(attribute), attribute);
		return ERROR_INT;
		/* break; */
    }

    return SUCCESS_INT;

}

INT_type CheckAndGetAttr(curConstituent, attribute, value, parmmask)
CONSTITUENT curConstituent;
INT_type attribute;
POINTER_type value;
PARM_MASK_type * parmmask;
{
    if (curConstituent == odalib_NULL_CONSTITUENT()) {
	return NoSuchAttr;
    }
    else{
	switch(odalib_HasAttr(curConstituent, attribute)) {
	    case HAS_ATTR:
		if (ERROR_INT == odalib_GetAttr(curConstituent, attribute, value, Use_Local_Value, parmmask)) {
		    printf("Cannot retrieve attribute from constituent (%s)\n", ErrorMsg(odalib_TKError()));
		    return odalib_TKError();
		}
		break;
	    case NO_ATTR:
		return NoSuchAttr;
		/* break; */
	    case ERROR_INT:
		/* see if not possible, or if there was a real error */
		if (odalib_TKError() == BADATTR_err) {
		    return NoSuchAttr;	/* just can't find it here */
		}
	    default:
		printf("Error %s getting attribute %s (%d)\n", ErrorMsg(odalib_TKError()), odalib_GetAttrName(attribute), attribute);
		return ERROR_INT;
		/* break; */
	}
    }
    return SUCCESS_INT;

}

INT_type GetOnObjOrStyle(curConstituent, attribute, value, parmmask)
CONSTITUENT curConstituent;
INT_type attribute;
POINTER_type value;
PARM_MASK_type * parmmask;
{
    INT_type rval, status;
    CONSTITUENT PStyle, LStyle;

    rval = SUCCESS_INT;

    rval = CheckAndGetAttr(curConstituent, attribute, value, parmmask);
    if (SUCCESS_INT == rval) {
	printf("Found attr on object\n");
	goto LEAVE;
    }

    /* Not there yet, keep looking */
    if (rval == NoSuchAttr) {
	/* Not there, go look on presentation style */
	if (tracing) {
	    printf("Not on object, checking p style\n");
	}
	status = CheckAndGetAttr(curConstituent, at_PRESENTATION_STYLE, (POINTER_type) &PStyle, Ignore_Parms);
	if (status == SUCCESS_INT) {
	    if (tracing) {
		printf("Found ps tyle, getting attr\n");
	    }
	    /* see if attribute on presentation style */
	    rval = CheckAndGetAttr(PStyle, attribute, value, parmmask);
	    if (SUCCESS_INT == rval) {
		/* Got it on presentation, all done */
		if (tracing) {
		    printf("Foud on p style\n");
		}
		goto LEAVE;
	    }
	}
    }

    /* Still not in hand, and no errors, check out layout style */
    if (rval == NoSuchAttr) {
	/* Not there, go look on layout style */
	if (tracing) {
	    printf("not on pstyle, checking l style\n");
	}
	status = CheckAndGetAttr(curConstituent, at_LAYOUT_STYLE, (POINTER_type) &LStyle, Ignore_Parms);
	if (status == SUCCESS_INT) {
	    /* see if attribute on layout style */
	    if (tracing) {
		printf("found lystyle, chekcing for att\n");
	    }
	    rval = CheckAndGetAttr(LStyle, attribute, value, parmmask);
	    if (SUCCESS_INT == rval) {
		/* Got it on layout, all done */
		goto LEAVE;
	    }
	}
    }

    /* Well, we either have it or run out of local places to check */

LEAVE:
    if (tracing) {
	printf("leaving multiple look with rval=%d\n", rval);
    }
    return rval;

}

INT_type GetBLOFromDVL(node, retBLO)
CONSTITUENT node;
CONSTITUENT * retBLO;
{
    INT_type rval, status;
    at_DEFAULT_VALUE_LISTS_type *tempDVL;
    INT_type i;

    rval = NoSuchAttr;
    *retBLO = odalib_NULL_CONSTITUENT();

    /* Does this CLO have a DVL? */
    status = CheckAndGetAttr(node, at_DEFAULT_VALUE_LISTS, (POINTER_type) &tempDVL, Ignore_Parms);
    if (SUCCESS_INT == status) {
	/* Well, we have the DVL, is there a BLO on it? */
	if (tempDVL->basic_logical_attributes != odalib_NULL_CONSTITUENT()) {
		rval = SUCCESS_INT;
		*retBLO = tempDVL->basic_logical_attributes;
	}
	if (tempDVL) {
	    free(tempDVL); /* get rid of copy of structure */
	}
    }
    return rval;
}

