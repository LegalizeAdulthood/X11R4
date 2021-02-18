
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr19.c,v 1.5 89/11/01 15:47:10 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr19.c,v $

(c) Copyright 1988 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	attr19.c,v $
 * Revision 1.5  89/11/01  15:47:10  mss
 * Changed some casting and declarations for passing lint
 * 
 * Revision 1.4  89/10/31  13:15:15  jr
 * Lots of casting problems.
 * 
 * Revision 1.3  89/10/26  14:05:52  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.2  89/10/02  09:16:00  mss
 * Fixed bug where empty content was tagged an error instead of
 * returning NULL SEQUENCE from GetContent
 * 
 * Revision 1.7  89/09/29  16:06:53  annm
 * fixed bug relating to resource document searching in GetContent
 * 
 * Revision 1.6  89/09/12  09:05:55  mss
 * Removed #
 * 
 * Revision 1.5  89/05/18  17:56:51  annm
 * 
 * 
 * Revision 1.4  89/04/11  15:37:49  annm
 * support for GetContent
 * 
 * Revision 1.3  89/03/23  06:53:01  mss
 * Added dummy procedure for GetContent
 * 
 * Revision 1.2  89/02/23  20:36:46  annm
 * changes for new DVL stuff
 * 
 * Revision 1.1  89/02/01  14:52:36  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr19.c,v $";
#endif



#define TK_IMPL_ATTR
#define TK_GVARS_ATTR19
#define TK_TKI_TKERR
#define TK_IMPL_CONST

#include <rnames.h>
#include <acd.h>
#include <tkerr.h>
#include <init.h>

#include <values.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR19
#undef TK_TKI_TKERR
#undef TK_IMPL_CONST

/*
    This file contains:

	1) the code for the application visible toolkit routine
	    GetContentKind

	2) the code fo the application visible toolkit routine
	    GetContent
*/

/* 
    termination condition needed for the GetContent complex
    of routines. N.B. this value MUST be different than
    ERROR_INT and SUCCESS_INT 
*/
#define KEEP_GOING  ( (INT_type) 99)




SEQUENCE_BYTE_type GetContent( constituent )
    IN CONSTITUENT constituent;
{

    /*
	Strategy:
	    1) check the constituent to ensure that it is not
		NULL_CONSTITUENT or ERROR_CONSTITUENT
	    2) check to ensure that the constituent is basic
	    3) try to find content on the constituent - if
		fails bail out
	    4) try to find content on an object class associated
		with the constituent - if fails bail out
	    5) try to find content in the resource document 
    */

    SEQUENCE_BYTE_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    SEQUENCE_type TheSequence;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* check the constituent */

    if( constituent == ERROR_CONSTITUENT ||
	constituent == NULL_CONSTITUENT ){
	rval = ERROR_SEQUENCE;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = 
	    TKERR_Format( "[GetContent]: constituent parameter ERROR or NULL." );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    if( ! CONST_IsBasic( TheConstituent ) ){
	rval = ERROR_SEQUENCE;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[GetContent]: the constituent must be basic" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /*
	The routines below return:
	    SUCCESS_INT - if all well the TheSequence has been loaded
		with a value to return to the user
	    ERROR_INT - if there was an error and an error should be 
		returned to the user
	    KEEP_GOING - we should continue the search for content

	The termination tests are performed by first checking for
	    ERROR_INT, then checking for != KEEP_GOING which MUST
	    imply that the value returned was SUCCESS_INT

    */

    SlaveRval = ATTR_LookOnConstituent( constituent, &TheSequence );
    if( SlaveRval == ERROR_INT  ){
	rval = ERROR_SEQUENCE;
	goto LEAVE;
    }
    if( SlaveRval != KEEP_GOING ){
	rval = TheSequence;
	goto LEAVE;
    }
    
    SlaveRval = ATTR_LookOnObjClass( constituent, &TheSequence );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_SEQUENCE;
	goto LEAVE;
    }
    if( SlaveRval != KEEP_GOING ){
	rval = TheSequence;
	goto LEAVE;
    }

    SlaveRval = ATTR_LookInResourceDoc( constituent, &TheSequence );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_SEQUENCE;
	goto LEAVE;
    }
    if( SlaveRval != KEEP_GOING ){
	rval = TheSequence;
	goto LEAVE;
    }

    /* Does not seem to be any content associated with this basic */
    rval = NULL_SEQUENCE;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type ATTR_LookOnConstituent( constituent, TheSequence )
    IN CONSTITUENT constituent;
    OUT SEQUENCE_type *TheSequence;
{

    /* 

	This routine is part of the support for GetContent.

	Return Value:
	    1) ERROR_INT on error
	    2) SUCCESS_INT on success and TheSequence has been loaded
		with the sequence to return to the user
	    3) KEEP_GOING if the caller should continue the search    

	Strategy:
	    1) see if the constituent has at_CONTENT_PORTIONS -
		    if no return KEEP_GOING (if error return
		    ERROR_INT)
	    2) attempt to find the content portion - if this fails
		    return ERROR_INT
	    3) get the content poritons value - a sequence that we are going
		to rummage through looking for content
	    4) accumulage content and return the result to the caller
		    
    */

    /* 

	Before reading any further, you should take a pain killer -
	you are about to get a head ache.

	What follows below is messy because the standard is messy (my
	opinion). First the IS says in Section 2.5.1.2.5, which tells
	how to find the content associated with a basic object (and
	also how to find the attributes associated with a content
	portion which doing inheritance.)

	" The content associated to a basic object in a specific structure 
	is determined by the first of the following to specify either any 
	content portions or a content generator:

	    - the basic object description;

	    - an object class description referenced from the basic
		object description;

	    - an object class description in the resource-document
		referenced from an object class description referenced from
		the basic object.
	"

	At this point, we are examining either the object of the object 
	class. Since
	the object or object class  can have both a content portion and 
	a content
	generator, the question arises: which one to look at first.
	(N.B. The specific basic object could only have a content
	portion because content generator can only go on a basic
	object class.)

	What follows is specified in the IS, Section 2.5.3.2.2. which
	    defines how content generator works:

	"For any component description, this attribute is ignored if there
	is more than one content portion or if the single one specifies
	the attribute "content information" ".

	The "this attribute" is at_CONTENT_GENERATOR. In this case, the "any 
	component description" is the object or object class which we 
	are currently considering.

	Thus, we now check the length of the content portions.
	If the length is > 1 then we accumulate content and return the result
	of accumulating content. If the length == 1 then we return the content 
	if we can find it - otherwise we look for a content generator which
	we evaluate if we find. If there is no content portions then we
	look for a content generator - if we find it we evaluate it.
    */

    
    INT_type rval;

    INT_type SlaveRval;

    ATTR_attribute_t TheAttribute;

    SEQUENCE_CONSTITUENT_type TheContentPortions;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    /* first see if the basic constituent has content portions */
    SlaveRval = HasAttr( constituent, at_CONTENT_PORTIONS );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_LookOnConstituent]: can not HasAttr to find at_CONTENT_PORTIONS" );
	dummy = TKERR_EndErrMsg();
 	goto LEAVE;
    }
    if( SlaveRval == NO_ATTR ){
	/* there are not content portions so go look for a content generator */
	rval = ATTR_LookForContentGenerator( constituent, TheSequence );
	goto LEAVE;
    }

    /* attempt to locate the content portions attribute */
    TheAttribute = ATTR_FindAttr( constituent, 
		    at_CONTENT_PORTIONS );
    if( TheAttribute == ERROR_ATTRIBUTE ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_LookOnConstituent]: the constituent has a content portion but I can't find it" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* get the sequence of constituents that is the content portions */
    TheContentPortions = GET_SEQUENCE_CONSTITUENT( TheAttribute );

    if( TheContentPortions->length > (INT_type) 1 ){
	rval = ATTR_AccumulateContent( TheContentPortions, TheSequence );
	if( rval == ERROR_INT ){
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_LookOnObjClass]: the above error occurred when attempting to accumulate content for an object class during a GetContent" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	else {
	    /* ASSERT: TheSequence was set  - might be NULL_SEQUENCE */
	    /* but for more than one content portion this is legal */
	    goto LEAVE;
	}
    }
    else if( TheContentPortions->length == (INT_type) 1 ){
	rval = ATTR_AccumulateContent( TheContentPortions, TheSequence );
	if( rval == ERROR_INT ){
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_LookOnObjClass]: the above error occurred while attempting to accumulate content for the object class during a GetContent" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	else /* ASSERT: return was SUCCESS_INT */
	    if( *TheSequence == NULL_SEQUENCE ) {
	    rval = ATTR_LookForContentGenerator( constituent, TheSequence );
	    goto LEAVE;
	}
	else {
	    /* ASSERT: something was found - TheSequence was set */
	    goto LEAVE;
	}
    }
    else {
	/* ASSERT: TheContentPortions has length < 1 */
	/* Look for a content generator */
	rval = ATTR_LookForContentGenerator( constituent, TheSequence );
	goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_AccumulateContent( TheContentPortions, TheSequence )
    IN SEQUENCE_CONSTITUENT_type TheContentPortions;
    OUT SEQUENCE_type *TheSequence;
{

    /*
	This routine return:
	    ERROR_INT - there has been an error
	    SUCCESS_INT - success and TheSequence has been loaded with
		the value to return to the user - will be NULL_CONSTITUENT
		if no content was found.

	Strategy:

	    1) first make a sequence for accumulating the content
	    2) rummage through the content portion - for each:
		2a) see if has content information - if error return 
			ERROR_INT - if not continue the look
		2b) attempt to find the content information - if
			this fails return ERROR_INT
		2c) get the value of the content information - a sequence
		2d) append sequence to add the content information on the
			end of the sequence made in 1) above - if this
			fails return ERROR_INT
	    3) after the look see if the sequence of 1) still has zero 
		length - if so delete the sequence and return SUCCESS_INT
		AND load TheSequence with NULL_SEQUENCE
    */

    INT_type rval;

    SEQUENCE_type Content;

    INT_type i;

    CONSTITUENT ThisContentPortion;

    INT_type SlaveRval;

    ATTR_attribute_t TheAttribute;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    /* start with an empty content sequence */
    Content = MakeSequence( SEQUENCE_BYTE_tag, (INT_type) 0 );
    if( Content == ERROR_SEQUENCE ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AccumulateContent]: can not make a sequence to hold the content" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    
    /* rummage through the content portions accumulating content information */
    for( i = (INT_type) 0; i < TheContentPortions->length; i++ ){

	ThisContentPortion = TheContentPortions->sequence_value.constituents[i];

	/* see if the content portion has content information */
	SlaveRval = HasAttr( ThisContentPortion, at_CONTENT_INFORMATION );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = DeleteSequence( Content );
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AccumulateContent]:  can not HasAttr content information on basic's content portion number " );
	    dummy = TKERR_FormatInt( "%d", i );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	if( SlaveRval == NO_ATTR ){
	    continue;
	}

	TheAttribute = ATTR_FindAttr( ThisContentPortion, 
			at_CONTENT_INFORMATION );
	if( TheAttribute == ERROR_ATTRIBUTE ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = DeleteSequence( Content );
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AccumulateContent]:  content portion number " );
	    dummy = TKERR_FormatInt( "%d", i );
	    dummy = TKERR_Format( " has content information but I can't find it" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	SlaveRval = AppendSequence( Content, 
		    GET_SEQUENCE_BYTE( TheAttribute ) );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = DeleteSequence( Content );
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AccumulateContent]:  can not append on more content" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    
    }

    if( Content->length == (INT_type) 0 ){
	rval = SUCCESS_INT;
	*TheSequence = NULL_SEQUENCE;
	dummy = DeleteSequence( Content );
	goto LEAVE;	
    }
    else {
	/* ASSERT: content length > 0 */
	rval = SUCCESS_INT;
	*TheSequence = Content;
	goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_LookOnObjClass( constituent, TheSequence )
    IN CONSTITUENT constituent;
    OUT SEQUENCE_type *TheSequence;
{


    /*

	This routine helps to implement GetContent.

	Return Value:
	    1) ERROR_INT when there has been an error
	    2) SUCCESS_INT when content has been found - TheSequence
		    has been loaded with the value to return to the user
	    3) KEEP_GOING when the search should be continued

	Strategy:
	    1) see if the constituent has an object class
	    2) if it does then call LookOnConstituent passing
		along the object class.

    */


    INT_type rval;

    INT_type SlaveRval;

    INT_type dummy;

    CONSTITUENT TheObjectClass;

    ATTR_attribute_t AttrStructPtr;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    /* First see if the constituent has the object class attribute */
    SlaveRval = HasAttr( constituent, at_OBJECT_CLASS );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TkError has been set */
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_LookOnObjClass]: can not HasAttr to find at_OBJECT_CLASS" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    if( SlaveRval == NO_ATTR ){
	rval = KEEP_GOING;
	goto LEAVE;
    }
    
    /* ASSERT: HasAttr must have returned HAS_ATTR */
    AttrStructPtr = ATTR_FindAttr( constituent, at_OBJECT_CLASS );
    if( AttrStructPtr == ERROR_ATTRIBUTE ){
	/* Some thing is VERY wrong */
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_LookOnObjClass]: constituent has attribute at_CONTENT_PORTIONS but I can't find it" ); 
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    
    /* now get the value */
    TheObjectClass = (CONSTITUENT) GET_CONSTITUENT( AttrStructPtr );

    rval = ATTR_LookOnConstituent( TheObjectClass, TheSequence );    

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_LookForContentGenerator( TheObjectClass, TheSequence )
    IN CONSTITUENT TheObjectClass;
    OUT SEQUENCE_type *TheSequence;
{
    
    INT_type rval;

    INT_type SlaveRval;

    ATTR_attribute_t TheAttribute;

    STRING_EXPR_type *TheStringExpr;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    SlaveRval = HasAttr( TheObjectClass, at_CONTENT_GENERATOR );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_LookForContentGenerator]: error on attempting to HasAttr content generator for an object class during a GetContent" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    if( SlaveRval == NO_ATTR ){
	rval = KEEP_GOING;
	goto LEAVE;
    }

    TheAttribute = ATTR_FindAttr( TheObjectClass, at_CONTENT_GENERATOR );
    if( TheAttribute == ERROR_ATTRIBUTE ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_LookForContentGenerator]: the object class has a content generator but I can't find it during a GetContent" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheStringExpr = GET_STRING_EXPR( TheAttribute );

    /* 
	At this point call the string expr eval'er - but it 
	currently return STRING_type which is all wrong -
	Jonathan is thinking about this one 
    */

    rval = ERROR_INT;
    TKError = UNIMPL_err;
    dummy = TKERR_StartErrMsg();
    dummy = TKERR_Format( "[ATTR_LookForContentGenerator]: there is a semantics problem with the str expr eval'er - talk to Jonathan Rosenberg" );
    dummy = TKERR_EndErrMsg();
  
LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_LookInResourceDoc( constituent, TheSequence )
    IN CONSTITUENT constituent;
    OUT SEQUENCE_type *TheSequence;
{

    /*
	This routine is really umimplemented.
    
	Temporarily, return NULL_SEQUENCE as this is the end of
	the line in searching for content.
    */

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = KEEP_GOING;
    

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );


}


VALUE_type GetContentKind( constituent )
    IN CONSTITUENT constituent;
{

    /*
	Strategy:
	    1) check the constituent to ensure that it is not
		NULL_CONSTITUENT or ERROR_CONSTITUENT
	    2) check for constituent being either an object
		or an object class
	    3) for the object class do a local getattr for the
		    content arch. class then a local
		    getattr for content type
	    4) for the object follow rules a) to f) of
		    section 2.5 of the is then do a getattr
		    for content type

    */

    VALUE_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* check the constituent */

    if( constituent == ERROR_CONSTITUENT ||
	constituent == NULL_CONSTITUENT ){
	rval = ERROR_kind;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = 
	    TKERR_Format( "[GetContentKind]: constituent parameter ERROR or NULL." );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    if( !( CONST_IsBlock( TheConstituent ) ||
	CONST_IsBasLog( TheConstituent ) ) ){
	rval = ERROR_kind;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = 
	    TKERR_Format( "[GetContentKind]: constituent must be a basic component." );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( CONST_IsObjCl( TheConstituent ) ){
	rval =  ATTR_GetObjClContentKind( constituent );
	goto LEAVE;
    }
    else {
	rval = ATTR_GetObjectContentKind( constituent );
	goto LEAVE;
    }


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



VALUE_type ATTR_GetObjClContentKind( constituent )
    IN CONSTITUENT constituent;
{

    VALUE_type rval;

    INT_type SlaveRval;

    INT_type dummy;

    SEQUENCE_type ContentArchClass;
    VALUE_type	ContentType;

    PARM_MASK_type destination_mask;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* first try to find at_CONTENT_ARCH_CLASS */

    destination_mask = (PARM_MASK_type) 0;

    SlaveRval = ATTR_FindLocal( constituent, at_CONTENT_ARCH_CLASS,
		    (POINTER_type) &ContentArchClass, &destination_mask );
    if( SlaveRval == ERROR_INT && TKError != NOATTR_err ){
	/* something really went wrong */
	rval = ERROR_kind;
	goto LEAVE;
    }
    if( SlaveRval == SUCCESS_INT ){
	/* we got back a sequence - look up the kind */
	rval = ATTR_FindContentArchClass( (SEQUENCE_type) ContentArchClass );
	if( rval == ERROR_VALUE ){
	    rval = ERROR_kind;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[GetContentKind]: can not find the content arch class kind" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	else {
	    /* successful return */
	    goto LEAVE;
	}
    }
    /* ASSERT: if get here then didn't find at_CONTENT_ARCH_CLASS */

    /* second, try to find at_CONTENT_TYPE */

    SlaveRval = ATTR_FindLocal( constituent, at_CONTENT_TYPE,
		    (POINTER_type) &ContentType, &destination_mask );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_kind;
	goto LEAVE;
    }
    if( (VALUE_type) ContentType != at_CONTENT_TYPE_form_rgca ){
	rval = ERROR_kind;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[GetContentKind]: bad content type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    else {
	rval = FORM_RG_kind;
	goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



VALUE_type ATTR_GetObjectContentKind( constituent )
    IN CONSTITUENT constituent;
{

    VALUE_type rval;
    INT_type SlaveRval;
    SEQUENCE_type ContentArchClass;
	/* fake user return */
    VALUE_type ContentType;
	/* fake user return */
    PARM_MASK_type destination_mask, dummy;
    PARM_MASK_type wanted_mask;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    destination_mask = (PARM_MASK_type) 0;

    SlaveRval = ATTR_SterilizeParmMask( at_CONTENT_ARCH_CLASS, &wanted_mask );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_kind;
	goto LEAVE;
    }

    /* 
	First look on the constituent and associated
	style if any. This does steps a) and b) of 
	inheritance.
    */

    ContentArchClass = (SEQUENCE_type) 0;

    SlaveRval = ATTR_InheritFromConstAndStyle( constituent,
		    at_CONTENT_ARCH_CLASS, 
		    (POINTER_type) &ContentArchClass, 
		    /* fake GetAttr parameter mask */ (PARM_MASK_type *) 0,
		    wanted_mask, &destination_mask, 
		   (POINTER_type *)  &ContentArchClass );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: someone has set TKError */
	rval = ERROR_kind;
	goto LEAVE;
    }
    if( SlaveRval == ALL_DONE ){
	rval = ATTR_FindContentArchClass( ContentArchClass );
	if( rval == ERROR_VALUE ){
	    rval = ERROR_kind;
	}
	goto LEAVE;
    }

    /*
	Second: look for an object class, and if found inherit.
	This does steps c) and d) of inheritance.
    */

    SlaveRval = ATTR_InheritFromObjClass( constituent,
		    at_CONTENT_ARCH_CLASS, 
		    (POINTER_type) &ContentArchClass, 
		    /* fake GetAttr parameter mask */ (PARM_MASK_type *) 0,
		    wanted_mask, &destination_mask, 
		    (POINTER_type *) &ContentArchClass );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TKError has been set */
	rval = ERROR_kind;
	goto LEAVE;
    }
    if( SlaveRval == ALL_DONE ){
	rval = ATTR_FindContentArchClass( ContentArchClass );
	if( rval == ERROR_VALUE ){
	    rval = ERROR_kind;
	}
	goto LEAVE;
    }

    /*
	PUT CODE FOR RESOURCE DOCUMENT HERE.
	This does steps e) and f) of inheritance.
    */

    /*
	Now try to find at_CONTENT_TYPE.
    */
    dummy = 0;
    SlaveRval = GetAttr(constituent, at_CONTENT_TYPE,
			(POINTER_type) &ContentType,
			/* inherited */ BOOL_true,
			&dummy);
    if( SlaveRval == SUCCESS_INT ){
	if( ContentType == at_CONTENT_TYPE_form_rgca ){
	    rval = FORM_RG_kind;
	    goto LEAVE;
	}
	else {
	    rval = ERROR_kind;
	    goto LEAVE;
	}
    }
    else {
	rval = ERROR_kind;
	goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



VALUE_type ATTR_FindContentArchClass(TestSequence)
    IN SEQUENCE_type TestSequence;
{

/* Cannot cast the constant because VMS does not allow casted
    constants to be used as array declarations */
#define NIDS	6

    VALUE_type	rval;	    /* The returned value */

    REGISTER INT_type	i;
    REGISTER INT_type	j;

    static struct {
	ASN1_OBJECT_ID_type *id;
	VALUE_type	    type;
    } types[NIDS] = {
	{ &at_CONTENT_ARCH_CLASS_form_cc, FORM_CC_kind },
	{ &at_CONTENT_ARCH_CLASS_proc_cc, PROC_CC_kind },
	{ &at_CONTENT_ARCH_CLASS_fp_cc, FP_CC_kind },
	{ &at_CONTENT_ARCH_CLASS_form_rg, FORM_RG_kind }, 
	{ &at_CONTENT_ARCH_CLASS_fp_rg, FP_RG_kind },
	{ &at_CONTENT_ARCH_CLASS_form_gg, FORM_GG_kind }
    };

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Assume no match */

    rval = ERROR_VALUE;

    /* See if the length is appropriate */

    if( TestSequence->length !=5 ){
	goto LEAVE;
    }

    /* test against each predefined sequence */
    for( i=0; i<NIDS; i++ ) {

	for( j=0; j<5; j++ ) {

	    if (TestSequence->sequence_value.ints[j] != 
		    (*(types[i].id))->sequence_value.ints[j]){
		break;
	    }

	}

	if( j == 5 ){
	    /* Found match */
	    rval = types[i].type;
	    goto LEAVE;
	}

    }


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}
