/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/ileaf/ileafoda/RCS/attsup.c,v 1.2 89/09/08 17:19:52 mss Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/ileaf/ileafoda/RCS/attsup.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/ileaf/ileafoda/RCS/attsup.c,v 1.2 89/09/08 17:19:52 mss Exp $";
#endif

/*
 * $Log:	attsup.c,v $
 * Revision 1.2  89/09/08  17:19:52  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/09/05  21:58:23  mss
 * Initial revision
 * 
 * Revision 1.4  89/07/12  12:39:51  jaap
 * margins, and justification in place (sort of)
 * 
 * Revision 1.3  89/07/10  12:20:43  jaap
 * First shot at doing styles
 * 
 * Revision 1.2  89/07/03  13:44:58  jaap
 * Some structure & contents
 * 
 * Revision 1.1  89/06/28  16:05:17  jaap
 * Initial revision
 * 
*/
 
/*
 * shamelessly stolen from mss
 */

/* Attribute support routines to make life easier */
#include <odatk.h>
#include <attsup.h>

INT_type SafeSetAttr(constituent, attribute, value, mask)
    CONSTITUENT constituent;
    INT_type attribute;
    POINTER_type value;
    PARM_MASK_type mask; {

    INT_type retvalue, tmp;

    if (ERROR_INT == (retvalue = SetAttr(constituent, attribute, value, mask))) {
	tmp = TKError;
	warn("Error (%s) setting attribute %s (%d) for constituent %x\n",
		ErrorMsg(tmp), GetAttrName(attribute), attribute, constituent);
    }
    return retvalue;

}

INT_type
SafeSetIntAttr(constituent, attribute, value)
	CONSTITUENT constituent;
	INT_type attribute;
	INT_type value;
{

	INT_type retvalue, tmp;

	if (ERROR_INT == (retvalue = SetIntAttr(constituent, attribute, value))) {
		tmp = TKError;
		warn("Error (%s) setting attribute %s (%d) for constituent %x\n",
			ErrorMsg(tmp), GetAttrName(attribute), attribute, constituent);
	}
	return retvalue;
}

INT_type SafeSetValueAttr(constituent, attribute, value)
    CONSTITUENT constituent;
    INT_type attribute;
    VALUE_type value;
{

    INT_type retvalue, tmp;

    if (ERROR_INT == (retvalue = SetValueAttr(constituent, attribute, value))) {
	tmp = TKError;
	warn("Error (%s) setting attribute %s (%d) for constituent %x\n",
		ErrorMsg(tmp), GetAttrName(attribute), attribute, constituent);
    }
    return retvalue;

}

INT_type AddStringAttribute(CurCons, theAttr, textData, AsString)
    CONSTITUENT CurCons;
    INT_type theAttr;
    char * textData;
    BOOL_type AsString;
{
	SEQUENCE_BYTE_type tempStrSeq;
	INT_type i;

	/* Attach string to Constituent. Note we omit the NULL at the end */
	if (AsString) {
	    if (ERROR_STRING ==
		(tempStrSeq = Chars2String(textData))) {
		warn("Could not create byte sequence to hold string (%s)\n", ErrorMsg(TKError));
		return ERROR_INT;
	    }
	}
	else {
	    if (ERROR_SEQUENCE ==
		(tempStrSeq = MakeSequence(SEQUENCE_BYTE_tag, strlen(textData)))) {
		warn("Could not create byte sequence to hold string (%s)\n", ErrorMsg(TKError));
		return ERROR_INT;
	    }
	    for (i=0;i<strlen(textData);i++) {
		tempStrSeq->sequence_value.bytes[i] = textData[i];
	    }
	}

	if (ERROR_INT == SetAttr(CurCons, theAttr, (POINTER_type) tempStrSeq, ALL_PARMS)) {
	    warn("Could not set attribute %s (%d) for constituent %X because %s\n", GetAttrName(theAttr), theAttr, CurCons, ErrorMsg(TKError));
	    /* return ERROR_INT; */
	}

	/* And get rid of copy of sequence */
	if (ERROR_INT == DeleteSequence(tempStrSeq)) {
	    warn("Could not deallocate text sequence (%s)\n", ErrorMsg(TKError));
	    /* return ERROR_INT; */
	}

	return SUCCESS_INT;
}

INT_type AppendGenForSub(Parent,Child)
CONSTITUENT Parent, Child; {

    INT_type status;
    INT_type lastIndex;

    at_GEN_FOR_SUBORDINATES_type * OptRepPart;
    at_GEN_FOR_SUBORDINATES_type * ChoPart;

    status = HasAttr(Parent, at_GEN_FOR_SUBORDINATES);
    switch(status){
	case ERROR_INT: 
	    warn("Error return on hasattr (%s)\n", ErrorMsg(TKError));
	    return ERROR_INT;
	    /* break; */
	case HAS_ATTR:
	    if (ERROR_INT == GetAttr(Parent, at_GEN_FOR_SUBORDINATES, (POINTER_type) &OptRepPart, BOOL_false, ( (PARM_MASK_type *) 0))) {
		warn("Could not get generator for subordinates from parent (%s)\n",ErrorMsg(TKError));
		return ERROR_INT;
	    }
	    ChoPart = OptRepPart->construction_value.term.factor.factor_value.expr;
	    if (ERROR_INT == ExpandSequence(ChoPart->construction_value.construction, 1)) {
		warn("Could not expand generated subordinate sequence (%s) \n",ErrorMsg(TKError));
		return ERROR_INT;
	    }
	    break;
	case NO_ATTR:
	    OptRepPart = (at_GEN_FOR_SUBORDINATES_type *) LOCAL_Malloc(sizeof(at_GEN_FOR_SUBORDINATES_type));
	    if ((POINTER_type)OptRepPart == ERROR_POINTER) {
		warn("Could not allocate space for top level node in gen for sub ordinates\n");
		return ERROR_INT;
	    }
	    ChoPart = (at_GEN_FOR_SUBORDINATES_type *) LOCAL_Malloc(sizeof(at_GEN_FOR_SUBORDINATES_type));
	    if ((POINTER_type)ChoPart == ERROR_POINTER) {
		warn("Could not allocate space for second level node in gen for sub ordinates\n");
		return ERROR_INT;
	    }
	    OptRepPart->construction_tag = TERM_tag;
	    OptRepPart->construction_value.term.term_tag = OPT_REP_tag;
	    OptRepPart->construction_value.term.factor.factor_tag = EXPR_tag;
	    OptRepPart->construction_value.term.factor.factor_value.expr = ChoPart;
	    ChoPart->construction_tag = CHOICE_tag;
	    ChoPart->construction_value.construction = MakeSequence(SEQUENCE_CONSTRUCTION_TERM_tag,1);
	    if (ERROR_SEQUENCE == ChoPart->construction_value.construction) {
		warn("Could not make new sequence of construction terms (%s)\n",ErrorMsg(TKError));
		return ERROR_INT;
	    }
	    break;
	default:
	    warn("Unknown return values from HasAttr %d\n",status);
	    break;
    }


    /* Have the gen for subordinates, so now add new one */
    lastIndex = ChoPart->construction_value.construction->length-1;
    ChoPart->construction_value.construction->sequence_value.construction_terms[lastIndex].term_tag = REQ_tag;
    ChoPart->construction_value.construction->sequence_value.construction_terms[lastIndex].factor.factor_tag = OBJECT_CLASS_ID_tag;
    ChoPart->construction_value.construction->sequence_value.construction_terms[lastIndex].factor.factor_value.object_class_id = Child;

    /* Attach it to parent */
    if (ERROR_INT == SetAttr(Parent, at_GEN_FOR_SUBORDINATES, (POINTER_type) OptRepPart, ALL_PARMS)) {
	warn("Could not attach new generator for subordinates to parent constituent (%s)\n", ErrorMsg(TKError));
    }
    
    /* And get rid of copy of gen for sub */
    if (ERROR_INT == DeleteSequence(ChoPart->construction_value.construction)) {
	warn("Could not deallocate subordinate sequence\n (%s)", ErrorMsg(TKError));
	/* return ERROR_INT; */
    }
    if (ERROR_INT == LOCAL_Free(ChoPart)) {
	warn("Could not delete lower node of gen for sub tree (%s)\n", ErrorMsg(TKError));
	/* return ERROR_INT; */
    }
    if (ERROR_INT == LOCAL_Free(OptRepPart)) {
	warn("Could not delete top node of gen for sub tree (%s)\n", ErrorMsg(TKError));
	/* return ERROR_INT; */
    }

    return SUCCESS_INT;
}

INT_type AppendSubordinate(Parent,Child)
CONSTITUENT Parent, Child; {
    SEQUENCE_CONSTITUENT_type tempSubordinates;
    INT_type status;

    status = HasAttr(Parent, at_SUBORDINATES);
    switch(status){
	case ERROR_INT: 
	    warn("Error return on hasattr (%s)\n", ErrorMsg(TKError));
	    return ERROR_INT;
	    /* break; */
	case HAS_ATTR:
	    if (ERROR_INT == GetAttr(Parent, at_SUBORDINATES, (POINTER_type) &tempSubordinates, BOOL_false, ( (PARM_MASK_type *) 0))) {
		warn("Could not get subordinates from parent (%s)\n",ErrorMsg(TKError));
		return ERROR_INT;
	    }
	    if (ERROR_INT == ExpandSequence(tempSubordinates, 1)) {
		warn("Could not expand suborindate sequence (%s) \n",ErrorMsg(TKError));
		return ERROR_INT;
	    }
	    break;
	case NO_ATTR:
	    tempSubordinates = MakeSequence(SEQUENCE_CONSTITUENT_tag,1);
	    if (ERROR_SEQUENCE == tempSubordinates) {
		warn("Could not make new sequence of subordinates (%s)\n",ErrorMsg(TKError));
		return ERROR_INT;
	    }
	    break;
	default:
	    warn("Unknown return values from HasAttr %s\n",status);
	    break;
    }


    /* Have the subordinates, so now add new one */
    tempSubordinates->sequence_value.constituents[tempSubordinates->length - 1] = Child;

    /* Attach it to parent */
    if (ERROR_INT == SetAttr(Parent, at_SUBORDINATES, (POINTER_type) tempSubordinates, ALL_PARMS)) {
	warn("Could not attach new subordinates to parent constituent (%s)\n", ErrorMsg(TKError));
    }
    
    /* And get rid of copy of sequence */
    if (ERROR_INT == DeleteSequence(tempSubordinates)) {
	warn("Could not deallocate subordinate sequence\n (%s)", ErrorMsg(TKError));
	/* return ERROR_INT; */
    }

    return SUCCESS_INT;
}


INT_type AppendCharContent(CurBLO, CurCP, textData)
    CONSTITUENT CurBLO;
    CONSTITUENT CurCP;
    char * textData;
    {
	SEQUENCE_CONSTITUENT_type tempSeq;

	/* Attach CP to BLO -- we always use 1 CP per BLO */
	if (ERROR_SEQUENCE == 
	    (tempSeq = MakeSequence(SEQUENCE_CONSTITUENT_tag, 1))) {
	    warn("Could not create a sequence for adding CPs to BLOs (%s)\n", ErrorMsg(TKError));
	    return ERROR_INT;
	}
	tempSeq->sequence_value.constituents[0] = CurCP;
	if (ERROR_INT == SetAttr(CurBLO, at_CONTENT_PORTIONS, (POINTER_type) tempSeq, ALL_PARMS)) {
	    warn("Could not attach seq of CPs to BLO (%s)\n", ErrorMsg(TKError));
	}

	/* And get rid of copy of sequence */
	if (ERROR_INT == DeleteSequence(tempSeq)) {
	    warn("Could not deallocate content portion sequence (%s)\n", ErrorMsg(TKError));
	    /* return ERROR_INT; */
	}

	/* Attach string to CP.  */
	if (ERROR_INT == AddStringAttribute(CurCP, at_CONTENT_INFORMATION, textData, DoAsBytes)) {
	    warn("Could not add content information to content portion (%s)\n", ErrorMsg(TKError));
	    return ERROR_INT;
	}

	/* Add "Content Architecture Class" attribute */
	if (SetAttr(CurCP, at_TYPE_OF_CODING, (POINTER_type) at_TYPE_OF_CODING_cc,
		ALL_PARMS) == ERROR_INT)
	    warn("\"Type of Coding\" failed (%s)\n", ErrorMsg(TKError));


	/* Mark CP as having a text portion */
	if (ERROR_INT == SetAttr(CurBLO,  at_CONTENT_ARCH_CLASS, (POINTER_type)  at_CONTENT_ARCH_CLASS_proc_cc,  ALL_PARMS)) {
	    warn("Could not set content arch class (proc/cc) for BLO (%s)\n", ErrorMsg(TKError));
	    /* return ERROR_INT; */
	}

}

INT_type SafeGetAttr(curConstituent, attribute, attributevalue, inherited, parmmask)
CONSTITUENT curConstituent;
INT_type attribute;
POINTER_type attributevalue;
BOOL_type inherited;
PARM_MASK_type *parmmask; {

    switch(HasAttr(curConstituent, attribute)) {
	    case HAS_ATTR:
		if (ERROR_INT == GetAttr(curConstituent, attribute, attributevalue, inherited, parmmask)) {
		    warn("Cannot retrieve attribute from constituent (%s)\n", ErrorMsg(TKError));
		    return TKError;
		}
		break;
	    case NO_ATTR:
		warn("Missing expected attribute %s\n", GetAttrName(attribute));
		return ERROR_INT;
		/* break; */
	    case ERROR_INT:
	    default:
		warn("Error %s getting attribute %s (%d)\n", ErrorMsg(TKError), GetAttrName(attribute), attribute);
		return ERROR_INT;
		/* break; */
    }

    return SUCCESS_INT;

}

CONSTITUENT
SafeMakeStyle(adoc, akind)
	DOCUMENT_type adoc;
	VALUE_type akind;
{
	CONSTITUENT rval;

	if((rval = MakeStyle(adoc, akind)) == ERROR_CONSTITUENT) {
		fatal("Couldn't create style (%s)", ErrorMsg(TKError));
	}
	return rval;
}
