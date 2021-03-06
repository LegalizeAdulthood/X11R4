/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/class/pp/RCS/classpp.h,v 2.3 89/02/21 08:23:59 pgc Exp $ */


/*
 * for more comments please see "class.c" in this directory
 */



/*
 * ???
 */
#define class_EOF 0
#define class_Name 1
#define class_Other 2
#define class_Semi 3
#define class_Class 4
#define class_Returns 5
#define class_Methods 6
#define class_Data 7
#define class_Overrides 8
#define class_ClassProcedures 9
#define class_Comma 10
#define class_Colon 11
#define class_LeftBrace 12
#define class_RightBrace 13
#define class_LeftParen 14
#define class_RightParen 15
#define class_Comment 16
#define class_WhiteSpace 17
#define class_String 18
#define class_MacroMethods 19
#define class_MacroOverrides 20
#define class_Macros 21
#define class_Package 22
#define class_OnError 23
#define class_Return 24
#define class_LeftSquareBracket 25
#define class_RightSquareBracket 26
#define class_LASTTYPE 26

/*
 * ???
 */
#define name_InitializeObject 0
#define name_Allocate 1
#define name_Deallocate 2
#define name_InitializeClass 3
#define name_FinalizeObject 4
/* everything < name_RegularName is a special class procedures */
#define name_RegularName 5
#define name_ThisObject 6
#define name_Self 7
#define name_Unsigned 8
#define name_Int 9
#define name_Float 10
#define name_Void 11
#define name_Pointer 12
#define name_NULL 13
#define name_MinusOne 14
#define name_Exit 15
#define name_Boolean 16
#define name_Long 17

/*
 * ???
 */
enum ParseState {class_Normal, class_InClass,
    class_InMethods, class_InClassProcedures,class_InOverrides,
    class_InMacroMethods,class_InMacroOverrides,class_InMacros,
    class_InData, class_InFreeLink};



/* 
 * some "tweak"able constants
 */
#define	MAXINCLUDEPATHS	    50			/* maximum number of search paths */
#define	MAXMESSAGESIZE	    (2 * MAXPATHLEN)	/* longest error message */
#define	MAXFILENAMELEN	    8			/* longest base file name without error */

/*
 * exit codes from class preprocessor
 */
#define	EXITCODE_OK	    0  /* successful competion. */

#define	EXITCODE_COMMAND	((int) 1)  /* -h switch or a command line error. */
#define	EXITCODE_SLOPPY		((int) 2)  /* warnings without the -s switch. */
#define	EXITCODE_ACCESS		((int) 3)  /* files could not be accessed. */
#define	EXITCODE_DEFINITION	((int) 4)  /* error in the definition file. */
#define	EXITCODE_NO_SUPERCLASS	((int) 5)  /* this class has no superclass. */
#define	EXITCODE_PACKAGE	((int) 6)  /* attempt to subclass a package. */
#define	EXITCODE_SIGNAL		((int) 7)  /* killed by a caught signal. */
#define EXITCODE_RESOURCES	((int) 8)  /* too few resources (failed malloc, etc.). */
#define	EXITCODE_BUG		((int) 9)  /* program error (BUG!!!) (table overflowed, etc.). */


/* 
 * warning levels used with EmitWarning
 */
#define	WARNING_SILENT		((int) 0)   /* if silent, don't emit */
#define	WARNING_QUIET		((int) 1)   /* if quiet or silent, don't emit */
/* note, WARNING_SLOPPY can cause exit with EXITCODE_SLOPPY */
#define	WARNING_SLOPPY		((int) 2)   /* if sloppy, don't emit */ 

/*
 * This limits the level that subclassing
 * can go.  To create deeper heirarchies
 * make this number larger.
 */
#define FILESTACKSIZE 50


/*
 * ???
 */
#define errval_NULL 0
#define errval_MinusOne 1
#define errval_Fail 2
#define errval_NUM 3


/*
 * ???
 */
struct methods  {
    char *name;
    char inherited;
    enum {ptype_method,ptype_classproc,ptype_macro} type;
    short errval;
    short retType;
    char defined;
    char *macrodef;
    char *methodtype;
    char *methodargs;
    int argcount;
    struct methods *next;
};

/*
 * ???
 */
#define INITIALARGSTRSIZE 30
#define INITIALRETURNSTRSIZE 30
#define NULL 0
#define TRUE 1
#define FALSE 0

