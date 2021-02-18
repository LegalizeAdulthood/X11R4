
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts4.c,v 1.3 89/10/26 14:07:38 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts4.c,v $

(c) Copyright 1989 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	atts4.c,v $
 * Revision 1.3  89/10/26  14:07:38  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.2  89/10/16  15:39:02  annm
 * fixing problems with macro arguements
 * 
 * Revision 1.1  89/09/21  06:26:12  mss
 * Initial revision
 * 
 * Revision 1.9  89/09/13  10:00:43  mss
 * Fixed declaration of byte pointer
 * 
 * Revision 1.8  89/09/12  09:12:45  mss
 * Removed #
 * 
 * Revision 1.7  89/08/31  18:02:33  mss
 * Changed local to internal error
 * 
 * Revision 1.6  89/08/26  13:38:25  mss
 * set switches, include files for proper nanme mapping
 * 
 * Revision 1.5  89/08/26  10:28:50  mss
 * Changed use of LOCAL in MM
 * 
 * Revision 1.4  89/08/25  11:07:05  mss
 * Fixed another bug in SetFontFamily (with strings instead of arrays)
 * 
 * Revision 1.3  89/08/25  09:21:11  mss
 * Fixed bug converting string into array of names
 * 
 * Revision 1.2  89/08/06  08:44:36  mss
 * Changed name of SetStringAttribute to SetStringAttr
 * 
 * Revision 1.1  89/08/04  17:53:46  mss
 * Initial revision
 * 
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts4.c,v $";
#endif



#define TK_IMPL_ATTS
#define TK_GVARS_ATTS4
#define TK_TKI_TKERR
#define TK_TKI_MM

#include <apirt.h>
#include <atts.h>
#include <acd.h>
#include <mm.h>
#include <parms.h>
#include <apityp.h>
#include <tkerr.h>
#include <tags.h>
#include <seq.h>
#include <str.h>

#undef TK_IMPL_ATTS
#undef TK_GVARS_ATTS4
#undef TK_TKI_TKERR

/* forward declarations */
static INT_type BreakUpName(/*SlashedName, NameArray, NumNames*/);

/* Set a family in a font descriptor */
/* The input to this routine is an array of pointers to null terminated C
strings that represent the font's name using whatever is registered */
INT_type SetFontFamily(FontReference, FamilyName)
IN OUT FONT_REFERENCE_type  * FontReference;
IN CHAR_type		    * FamilyName;
{
    INT_type	rval;	    /* return value from function */
    INT_type	dummy;	    /* For error message printing */
#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    /* Parameters OK? */
    if (FontReference == (FONT_REFERENCE_type *) 0) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SetFontFamily]: null pointer ");
	dummy = TKERR_Format("passed for font reference");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;	
    }

    if (ERROR_INT == String2StrctName(FamilyName, &FontReference->family)) {
	/* msg from called proc */
	rval = ERROR_INT;
	goto LEAVE;	
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return rval;
}

static INT_type BreakUpName(SlashedName, NameArray, NumNames)
IN CHAR_type   * SlashedName;
OUT CHAR_type	***NameArray;
OUT INT_type	* NumNames;
{
    INT_type	rval;	    /* return value from function */
    CHAR_type	* CharPtr;	/* Pointer into SlashedName array */
    CHAR_type	* StartPtr;	/* Where to start taking string */
    CHAR_type	* DestPtr;	/* Place to put string */
    INT_type	NameLength;	/* Length of intermediate string */
    INT_type	SlotNumber;	/* Keeping track of which name is used */
    INT_type	dummy;		/* For error message printing */

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    if ( (SlashedName == (CHAR_type *) 0) ||
	 (NameArray == (CHAR_type ***) 0 ) ||
	 (NumNames == (INT_type *) 0)) {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[BreakUpName]: null pointer ");
	dummy = TKERR_Format("passed for parameter");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;	
    }

   /* Count the number of slots */
    for(CharPtr = SlashedName,(*NumNames)=1; *CharPtr; CharPtr++) {
	if (*CharPtr == (CHAR_type) '/') {
	    (*NumNames)++;
	}
    }

    /* Allocate space for name array */
   /* Fill in each slot */
    *NameArray = (CHAR_type **) MM_Malloc((INT_type)(*NumNames) * sizeof(CHAR_type *));
    if ((POINTER_type)*NameArray == ERROR_POINTER){
	rval = ERROR_INT;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[BreakUpName]: Could not allocate");
	dummy = TKERR_Format("space for name array");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;	
    }

    /* Put each name into a slot */
    for (CharPtr = SlashedName, SlotNumber = 0, StartPtr = SlashedName;
	  /* Alway loop, have explicit break */;
	  CharPtr++) {
	if ( ((*CharPtr) == (CHAR_type) 0) || (*CharPtr == (CHAR_type) '/')) {
	    /* We found a break point, so allocate and copy string */
	    NameLength = CharPtr - StartPtr + 1; /* 1 for the null byte */
	    (*NameArray)[SlotNumber] = 
	             (CHAR_type *) MM_Malloc((INT_type) NameLength * sizeof(CHAR_type));
	    if ((POINTER_type)(*NameArray)[SlotNumber] == ERROR_POINTER){
		rval = ERROR_INT;
		TKError = NOMEM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[BreakUpName]: Could not allocate");
		dummy = TKERR_Format("space for name string");
		dummy = TKERR_EndErrMsg();
		goto LEAVE;	
	    }
	    /* Copy over the string */
	    for (DestPtr = ((*NameArray)[SlotNumber]); StartPtr != CharPtr;
	         StartPtr++, DestPtr++) {
		*DestPtr = *StartPtr;
	    }
	    /* And place the null terminator */
	    *DestPtr = (CHAR_type) 0;
	    /* Advance to next slot */
	    SlotNumber++;

	    /* Make sure start pointer is at beginning of next string,
	       not end of current string */
	    StartPtr++;

	    /* See if in middle of name or end of everything */
	    if ((*CharPtr) == (CHAR_type) 0){
		break;
	    }
	}
    }


LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return rval;

}

INT_type String2StrctName(NameString, StructuredName)
IN CHAR_type			*NameString;
IN OUT STRUCTURED_NAME_type	*StructuredName;
{
    INT_type	rval;	    /* return value from function */
    INT_type	dummy;	    /* For error message printing */
    CHAR_type	**NameArray;   /* pointer to array of string names */
    INT_type	NumNames;   /* Number of names in array */
    INT_type	i;	    /* Loop counter */
#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    if (NameString == (CHAR_type *) 0) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[String2StrctName]: null pointer ");
	dummy = TKERR_Format("passed for string name");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;	
    }


    /* Break up the names into array slots and pass it along */
    if (ERROR_INT == BreakUpName(NameString, &NameArray, &NumNames)) {
	/* Error msg produced by caller */
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Assign to structured Name */

    if (ERROR_INT == Array2StrctName(NameArray, NumNames, StructuredName)) {
	/* msg generated by called proc */
	rval = ERROR_INT;
	goto LEAVE;
    }

    /* Throw away created storage */
    for (i=0; i<NumNames; i++) {
	if (ERROR_INT == MM_Free((POINTER_type)NameArray[i])) {
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[String2StrctName]: cannot deallocate string storage");
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;	
	}
    }
    if (ERROR_INT == MM_Free((POINTER_type)NameArray)) {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[String2StrctName]: cannot deallocate array storage");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;	
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return rval;
}

INT_type Array2StrctName(NameArray, NumNames, StructuredName)
IN CHAR_type		    * * NameArray;
IN INT_type			NumNames;
IN OUT STRUCTURED_NAME_type	*StructuredName;
{
    INT_type	rval;	    /* return value from function */
    INT_type	dummy;	    /* For error message printing */
    SEQUENCE_ISO9541_ID_type	tempSeq;
    INT_type	i;	    /* Loop index */
    CHAR_type	*StartPtr;   /* Where to read chars from */
    BYTE_type	*DestPtr;    /* Where to place chars */
    SEQUENCE_BYTE_type	nameSeq;    /* Temp for placing characters */

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    /* Parameters OK? */
    if (StructuredName == (STRUCTURED_NAME_type *) 0) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[Array2StrctName]: null pointer ");
	dummy = TKERR_Format("passed for structured name");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;	
    }

    if (NameArray == (CHAR_type **) 0) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[Array2StrctName]: null pointer ");
	dummy = TKERR_Format("passed for name array");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;	
    }

    if ( (NumNames < 2) || (NumNames > 72)) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[Array2StrctName]: number of ");
	dummy = TKERR_Format("identifiers in structured name must be between");
	dummy = TKERR_FormatInt(" 2 and 72. %d passed",(INT_type) NumNames);
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Everything looks OK so far, build the structured name for use in SetAttr */
    StructuredName->structured_name_tag = STRUCTURED_NAME_SEQ_tag;
    StructuredName->structured_name_value.idseq =
      (SEQUENCE_ISO9541_ID_type) MakeSequence(SEQUENCE_ISO9541_ID_tag,NumNames);
    if (StructuredName->structured_name_value.idseq == (SEQUENCE_ISO9541_ID_type) ERROR_SEQUENCE) {
	rval = ERROR_INT;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[Array2StrctName]: could not allocate ");
	dummy = TKERR_Format("sequence for structured name");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Check each name for legality */
    for (i=0; i< NumNames; i++) {
	/* Check the ith name */
	for(StartPtr = NameArray[i]; *StartPtr; StartPtr++) {
	    if ( ! ( ((*StartPtr >= (CHAR_type) 'a') && (*StartPtr <= (CHAR_type) 'z')) ||
		     ((*StartPtr >= (CHAR_type) 'A') && (*StartPtr <= (CHAR_type) 'Z')) ||
		     ((*StartPtr >= (CHAR_type) '0') && (*StartPtr <= (CHAR_type) '9')) ||
		     (*StartPtr == (CHAR_type) '-') )) {
		rval = ERROR_INT;
		TKError = BADPARM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt("[Array2StrctName]: illegal character %d ",
				     (INT_type) *StartPtr);
		dummy = TKERR_FormatInt("in name %d", (INT_type) i);
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	    }
	}
	/* See if right number of characters -- StartPtr should point at null*/
	if (((StartPtr-NameArray[i]) > 72) ||
	    ((StartPtr-NameArray[i]) < 1)) {
		rval = ERROR_INT;
		TKError = BADPARM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[Array2StrctName]: identifiers in ");
		dummy = TKERR_Format("structured names must be between 1 and 72");
		dummy = TKERR_FormatInt(" characters. Is %d", (INT_type) (StartPtr-(*NameArray)[i]));
		dummy = TKERR_FormatInt(" in %d", (INT_type) i);
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	}

	/* The characters look OK, copy them over */
	tempSeq = StructuredName->structured_name_value.idseq;
	tempSeq->sequence_value.iso9541_ids[i].iso9541_id_tag = ID_STR_tag;
	tempSeq->sequence_value.iso9541_ids[i].iso9541_id_value.octet_string =
	  (SEQUENCE_BYTE_type) MakeSequence(SEQUENCE_BYTE_tag,(INT_type)(StartPtr-NameArray[i]));
	nameSeq = tempSeq->sequence_value.iso9541_ids[i].iso9541_id_value.octet_string;
	if (nameSeq == (SEQUENCE_BYTE_type) ERROR_SEQUENCE) {
	    rval = ERROR_INT;
	    TKError = NOMEM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[Array2StrctName]: could not allocate ");
	    dummy = TKERR_Format("space for single structured name");
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	for(StartPtr = NameArray[i], DestPtr = nameSeq -> sequence_value.bytes;
	    *StartPtr;
	    StartPtr++, DestPtr++) {
	    *DestPtr = (BYTE_type) *StartPtr;
	}
    }

    /* Now have filled in structured name, ready for use */
    
LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return rval;

}

INT_type SetFontFamilyArray(FontReference, FamilyArray, NumNames)
IN OUT FONT_REFERENCE_type  * FontReference;
IN CHAR_type		    * *FamilyArray;
IN INT_type			NumNames;
{
    INT_type	rval;	    /* return value from function */
    INT_type	dummy;	    /* For error message printing */

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    /* Parameters OK? */
    if (FontReference == (FONT_REFERENCE_type *) 0) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SetFontFamilyArray]: null pointer ");
	dummy = TKERR_Format("passed for font reference");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;	
    }

    if (ERROR_INT == Array2StrctName(FamilyArray, NumNames, &FontReference->family)) {
	/* Error msg created be called proc */
	rval = ERROR_INT;
	goto LEAVE;	
    }

    
LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return rval;

}


INT_type SetStringAttr(constituent, attribute, data, chars)
    CONSTITUENT constituent;
    INT_type attribute;
    CHAR_type * data;
    BOOL_type chars;
{
	SEQUENCE_BYTE_type	tempStrSeq;
	INT_type    i;		/* For loops */
	INT_type    rval;	/* Error return values */
	INT_type    dummy;	/* For error messages */
	CHAR_type   * tempPtr;	/* For moveing through the characters */

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

	rval = SUCCESS_INT;

	/* Parameters validated by other lower calls and as needed */

	/* Attach string to Constituent. Note we omit the NULL at the end */
	if (chars == DATA_CHAR_TYPE) {
	    tempStrSeq = Chars2String(data);
	    if (ERROR_STRING == tempStrSeq) {
		/* Msg by called routine */
		rval = ERROR_INT;
		goto LEAVE;
	    }
	}
	else if (chars == DATA_BYTE_TYPE){
	    tempPtr = data;
	    while (*tempPtr) {
		tempPtr++;
	    }
	    tempStrSeq = MakeSequence(SEQUENCE_BYTE_tag, (INT_type)(tempPtr-data));
	    if (ERROR_SEQUENCE == tempStrSeq) {
		/* Msg by called routine */
		rval = ERROR_INT;
		goto LEAVE;
	    }
	    for (i=0;i<tempStrSeq->length;i++) {
		tempStrSeq->sequence_value.bytes[i] = (BYTE_type) data[i];
	    }
	}
	else {
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SetStringAttr]: unknown boolean chars ");
	    dummy = TKERR_Format("flag passed");
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;	
	}

	/* Try to set the attribute (note that setattr does all of the
           parameter checking) */
	if (ERROR_INT == SetAttr(constituent, attribute, (POINTER_type) tempStrSeq, ALL_PARMS)) {
	    /* Msg by called routine */
	    rval = ERROR_INT;
	    goto LEAVE;
	}

	/* And get rid of copy of sequence */
	if (ERROR_INT == DeleteSequence(tempStrSeq)) {
	    /* Msg by called routine */
	    rval = ERROR_INT;
	    goto LEAVE;
	}

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return rval;

}

INT_type AppendSubordinate(parent,child)
CONSTITUENT parent;
CONSTITUENT child; 
{
    SEQUENCE_CONSTITUENT_type tempSubordinates;
    INT_type status;
    INT_type	rval;		/* error return value */
    INT_type	dummy;		/* for error msg returns */

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

	rval = SUCCESS_INT;

	/* Validate child -- parent will be validated by subsidiary calls */
	if ( (child == NULL_CONSTITUENT) ||
	     (child == ERROR_CONSTITUENT) ||
	     (child == (CONSTITUENT) 0)){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[AppendConstituent]: child is error or null constituent");
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;	
	}

	status = HasAttr(parent, at_SUBORDINATES);
	switch(status){
	    case ERROR_INT: 
		/* Msg by called routine -- hasattr */
		rval = ERROR_INT;
		goto LEAVE;
		/* break; */
	    case HAS_ATTR:
		if (ERROR_INT == GetAttr(parent, at_SUBORDINATES, 
					 (POINTER_type) &tempSubordinates, BOOL_false, 
					 ( (PARM_MASK_type *) 0))) {
		    /* Msg by called routine -- hasattr */
		    rval = ERROR_INT;
		    goto LEAVE;
		}
		if (ERROR_INT == ExpandSequence(tempSubordinates, (INT_type) 1)) {
		    /* Msg by called routine -- hasattr */
		    rval = ERROR_INT;
		    goto LEAVE;
		}
		break;
	    case NO_ATTR:
		tempSubordinates = MakeSequence(SEQUENCE_CONSTITUENT_tag,(INT_type) 1);
		if (ERROR_SEQUENCE == tempSubordinates) {
		    /* Msg by called routine -- hasattr */
		    rval = ERROR_INT;
		    goto LEAVE;
		}
		break;
	    default:
		rval = ERROR_INT;
		TKError = INTERNAL_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt("[AppendConstituent]: unknown HasAttr value %d",
				     (INT_type) status);
		dummy = TKERR_EndErrMsg();
		goto LEAVE;	
		/* break; */
	}


    /* Have the subordinates, so now add new one */
    tempSubordinates->sequence_value.constituents[tempSubordinates->length - 1] = child;

    /* Attach it to parent */
    if (ERROR_INT == SetAttr(parent, at_SUBORDINATES, (POINTER_type) tempSubordinates, ALL_PARMS)) {
	/* Msg by called routine -- setattr */
	rval = ERROR_INT;
	goto LEAVE;
    }
    
    /* And get rid of copy of sequence */
    if (ERROR_INT == DeleteSequence(tempSubordinates)) {
	/* Msg by called routine -- deletesequence */
	rval = ERROR_INT;
	goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return rval;

}

