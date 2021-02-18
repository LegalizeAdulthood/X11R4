/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/initfonts.h,v 1.3 89/09/11 15:01:33 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/initfonts.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	initfonts.h,v $
 * Revision 1.3  89/09/11  15:01:33  mss
 * Removed #s
 * 
 * Revision 1.2  89/09/08  17:49:03  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:37:01  mss
 * Initial revision
 * 
 * Revision 1.3  88/11/29  16:54:33  annm
 * added capability to examine the family in InitializeFonts
 * 
 * Revision 1.2  88/11/17  15:04:39  annm
 * Removed the ' ' from the end of the one character long font names 
 * (i.e. R, I, B, H and C)
 * 
 * Revision 1.1  88/11/17  14:43:43  annm
 * Initial revision
 * 
*/

/* returns from InitializeFonts */
#define SUCCESS 1
#define FAILURE 2

/* return used internally */
#define	NOT_THERE	3

#define YES 1
#define NO  2

/* INTERNALLY MAINTAINED INFORMATION 
 (mess with this and you are dog meat) 
*/

#define NFONTS 100

#define UNSPECIFIED_PS (-1)

struct FontInfo_struct {
    char TroffName[3];
    int PointSize;
} FontInfo[NFONTS];

#define NONE 0
#define BOLD 1
#define ITALIC 2
#define BOTH 3

#define NFAMILIES 3

/* table for finding family */
struct family_struct {
    char name[40];
    VALUE_type dsclass;
} family[NFAMILIES]

#ifdef INITFONTS
= {

{ "Helvetica", DSCLASS_SANS_gothic_neo},		/* Helvetica */
{ "Times", DSCLASS_SERIFS_legi_rounded },	/* Roman */
{ "Courier", DSCLASS_SERIFS_square_type }	/* Courier */

}

#endif
;

/* forward declarations for procedures */

#undef EXTERN

#ifdef INITFONTS
#define EXTERN 
#else
#define EXTERN extern
#endif

EXTERN int InitializeFonts();
EXTERN int ProcessFont();
EXTERN int PointSize();
EXTERN int WhichFont();

EXTERN int TryDsclass();
EXTERN int TryFamily();

EXTERN int SetUpFont();



