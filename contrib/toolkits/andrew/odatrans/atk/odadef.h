/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/* Constants to use until they get defined inside of api */

#ifndef NULL
#define	NULL	(0)
#endif

#define Use_Logical (BOOL_true)
#define Use_Layout (BOOL_false)
#define Use_Specific (BOOL_true)
#define Use_Generic (BOOL_false)
/* Use if does work */
#define Use_Inherited ( (BOOL_type) BOOL_true)
/* if inheritance does not work */
/* #define Use_Inherited (BOOL_false)*/
#define Use_Local_Value (BOOL_false)

#define Ignore_Parms ((PARM_MASK_type *) 0)
