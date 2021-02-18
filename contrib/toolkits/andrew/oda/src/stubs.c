
/*
		(c) Copyright 1988 Carnegie Mellon University

 See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
 CMU-ITC-071) for full text & permitted uses.
    WARNING: this file is generated from the API.
    WARNING: modifications to this file will disappear
    WARNING: when the file is next generated.



This file is to be generated ONCE.

It includes<stub procedures for the canonica>
procedures for performing operations on the
aunion.h types.

Each procedure returns ERROR_MUMBLE with
TKError = UNIMPL_err.

As the routines are written, these stubs
are to be deleted (manually).



*/

#include <lcsite.h>
#include <tkerr.h>
#include <rnames.h>
#include <errors.h>
#include <const.h>

#define TK_IMPL_ATTR

#include <attr.h>

#undef TK_IMPL_ATTR


/*
The stub procedures follow.
*/


INT_type at_BORDER_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type at_DIMENSIONS_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type at_MEDIUM_TYPE_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type at_POSITION_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type at_SAME_LAYOUT_OBJECT_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}


INT_type cc_ITEMISATION_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type dp_LOCAL_FILE_REFERENCE_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type dp_ODA_VERSION_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type dp_DOC_APP_PROF_DEFAULTS_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_COLOUR_REPS_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_COLOUR_REPS_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_COLOUR_REPS_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_COLOUR_REPS_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_COLOUR_REPS_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_COLOUR_REPS_Set(AttrStructPtr, TheStorage)
	ATTR_attribute_t AttrStructPtr;
	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_COLOUR_REPS_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_EDGE_RENDITION_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_EDGE_RENDITION_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_EDGE_RENDITION_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_EDGE_RENDITION_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_EDGE_RENDITION_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_EDGE_RENDITION_Set(AttrStructPtr, TheStorage)
	ATTR_attribute_t AttrStructPtr;
	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_EDGE_RENDITION_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_FILLED_AREA_REND_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_FILLED_AREA_REND_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_FILLED_AREA_REND_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_FILLED_AREA_REND_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_FILLED_AREA_REND_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_FILLED_AREA_REND_Set(AttrStructPtr, TheStorage)
	ATTR_attribute_t AttrStructPtr;
	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_FILLED_AREA_REND_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_ENCODING_ANNOUNCER_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_ENCODING_ANNOUNCER_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_ENCODING_ANNOUNCER_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_ENCODING_ANNOUNCER_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_ENCODING_ANNOUNCER_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_ENCODING_ANNOUNCER_Set(AttrStructPtr, TheStorage)
	ATTR_attribute_t AttrStructPtr;
	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_ENCODING_ANNOUNCER_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_LINE_RENDITION_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_LINE_RENDITION_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_LINE_RENDITION_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_LINE_RENDITION_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_LINE_RENDITION_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_LINE_RENDITION_Set(AttrStructPtr, TheStorage)
	ATTR_attribute_t AttrStructPtr;
	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_LINE_RENDITION_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_MARKER_RENDITION_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_MARKER_RENDITION_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_MARKER_RENDITION_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_MARKER_RENDITION_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_MARKER_RENDITION_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_MARKER_RENDITION_Set(AttrStructPtr, TheStorage)
	ATTR_attribute_t AttrStructPtr;
	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_MARKER_RENDITION_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_PICTURE_DIMENSIONS_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_PICTURE_DIMENSIONS_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_PICTURE_DIMENSIONS_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_PICTURE_DIMENSIONS_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_PICTURE_DIMENSIONS_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_PICTURE_DIMENSIONS_Set(AttrStructPtr, TheStorage)
	ATTR_attribute_t AttrStructPtr;
	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_PICTURE_DIMENSIONS_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_REGION_INTEREST_SPEC_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_REGION_INTEREST_SPEC_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_REGION_INTEREST_SPEC_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_REGION_INTEREST_SPEC_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_REGION_INTEREST_SPEC_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_REGION_INTEREST_SPEC_Set(AttrStructPtr, TheStorage)
	ATTR_attribute_t AttrStructPtr;
	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_REGION_INTEREST_SPEC_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_TEXT_RENDITION_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_TEXT_RENDITION_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_TEXT_RENDITION_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_TEXT_RENDITION_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_TEXT_RENDITION_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_TEXT_RENDITION_Set(AttrStructPtr, TheStorage)
	ATTR_attribute_t AttrStructPtr;
	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_TEXT_RENDITION_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_TRANSFORMATION_SPEC_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_TRANSFORMATION_SPEC_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_TRANSFORMATION_SPEC_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_TRANSFORMATION_SPEC_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_TRANSFORMATION_SPEC_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_TRANSFORMATION_SPEC_Set(AttrStructPtr, TheStorage)
	ATTR_attribute_t AttrStructPtr;
	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_TRANSFORMATION_SPEC_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_TRANSPARENCY_SPEC_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_TRANSPARENCY_SPEC_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_TRANSPARENCY_SPEC_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_TRANSPARENCY_SPEC_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_TRANSPARENCY_SPEC_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type gg_TRANSPARENCY_SPEC_Set(AttrStructPtr, TheStorage)
	ATTR_attribute_t AttrStructPtr;
	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

POINTER_type gg_TRANSPARENCY_SPEC_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_POINTER;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type SEQUENCE_ASN1_OBJECT_ID_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}


INT_type SEQUENCE_at_MEDIUM_TYPE_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type SEQUENCE_BINDING_PAIR_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type SEQUENCE_COPYRIGHT_INFO_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type SEQUENCE_NAME_ORG_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type SEQUENCE_RESOURCE_MAP_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type SEQUENCE_REVISION_HISTORY_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}


INT_type SEQUENCE_STRING_OR_ID_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type SEQUENCE_TAB_STOP_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type STRING_EXPR_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

INT_type STRING_OR_ID_WDbug()
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

