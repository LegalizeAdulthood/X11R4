/*      @(#)pkg_public.h 20.13 89/11/21 SMI      */

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef xview_pkg_public_DEFINED
#define xview_pkg_public_DEFINED


/*
 ***********************************************************************
 *			Include Files
 ***********************************************************************
 */

#include <xview/pkg.h>
#include <xview/xv_error.h>


/*
 ***********************************************************************
 *		Typedefs, Enumerations, and Structures
 ***********************************************************************
 *
 * SunView pkg. definition	
 */

/*
 * PRIVATE structures for pkg implementors only  
 */

/*
 * Last field before "embedded" struct in an "embedding object". 
 */
typedef long unsigned	 Xv_embedding;

typedef struct _xview_pkg {
    char		*name;
    Attr_pkg		 attr_id;
    unsigned		 size_of_object;
    struct _xview_pkg	*parent_pkg;
    int			 (*init)();
    Xv_opaque	 	 (*set)();
    Xv_opaque	 	 (*get)();
    int			 (*destroy)();
    Xv_object	 	 (*find)();
} Xv_pkg;


/*
 * Base instance for all objects	
 */
typedef struct {
    long unsigned	 seal;	/* Has "special" value meaning "am object" */
    Xv_pkg		*pkg;   /* Always points to pkg chain for an object */
} Xv_base;

/*
 ***********************************************************************
 *				Globals
 ***********************************************************************
 */

/*
 * PUBLIC General interface functions	
 */

Xv_public Xv_object	 xv_create( /* parent, pkg, attrs */ );
Xv_public Xv_object	 xv_find(	/* parent, pkg, attrs */ );
Xv_public Xv_opaque	 xv_set(	/* object, attrs */ );
Xv_public Xv_opaque	 xv_get(	/* object, attr, data */ );
Xv_public int		 xv_destroy(	/* object */ );
Xv_public int		 xv_destroy_check(	/* object */ );
Xv_public int		 xv_destroy_immediate(	/* object */ );
Xv_public int		 xv_destroy_safe(	/* object */ );

/*
 * PRIVATE functions for pkg implementors only  
 */

Xv_private Xv_opaque	xv_object_to_standard( /* object, caller */ );

#define XV_OBJECT_SEAL          0xF0A58142
#define XV_OBJECT_TO_STANDARD(_passed_object, _caller, _object)\
{\
      if (!_passed_object) {\
        xv_error(XV_NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INVALID_OBJ,\
                "not a pointer",\
                XV_ERROR_STRING, _caller,\
                0);\
        _object = ((Xv_opaque)0);\
    }\
    else\
       _object = (((Xv_base *)_passed_object)->seal == XV_OBJECT_SEAL) ? _passed_object : xv_object_to_standard(_passed_object, _caller);\
}

#endif xview_pkg_public_DEFINED
