/* 
 * array of global entry points for dynamic loader
 *
 * Defines a table of entry points and their names
 *
 */

/* macros which actually define entry points */

/*   globalref - direct reference to global symbol x */

#define globalref(x)	.globl x \
                        .data \
                        $$x: \
                         .asciiz	"x" \
                        .text \
                	.word	x, $$x
.text

/*   routine - reference to routine x - _x and (for ibm032) _.x */

#define routine(x)	globalref(x)

	.globl	globals
globals: /* beginning of entry point table */

/* from libclass */
	globalref(class_RoutineStruct)
	globalref(class_Error)

	routine(class_NewObject)
	routine(class_Load)
	routine(class_IsLoaded)
	routine(class_Lookup)
	routine(class_IsType)
	routine(class_IsTypeByName)
	routine(class_EnterInfo)
	routine(class_SetClassPath)
	routine(class_PrependClassPath)
	routine(class_GetEText)
	routine(class_GetTextBase)
	routine(class_GetTextLength)


/* common symbols referred to but not defined directly in libc.a */

	globalref(environ)		/* common symbol, defined nowhere */
	globalref(errno)		/* cerror */

/* do not delete the following line - make depends on it */

#include	<globalrefs._h>

	.text

	.globl	globalend
globalend:    .space 0

/* special cases for missing entry points under various machine types */

/* end of globals.s */
