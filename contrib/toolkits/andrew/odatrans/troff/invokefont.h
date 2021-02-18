/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/invokefont.h,v 1.3 89/09/11 15:01:43 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/invokefont.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	invokefont.h,v $
 * Revision 1.3  89/09/11  15:01:43  mss
 * 
 * 
 * Removed #s
 * 
 * Revision 1.2  89/09/08  17:49:24  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:37:07  mss
 * Initial revision
 * 
 * Revision 1.2  88/11/17  15:05:06  annm
 * Removed the ' ' from the end of the one character long font names 
 * (i.e. R, I, B, H and C)
 * 
 * Revision 1.1  88/11/17  14:44:17  annm
 * Initial revision
 * 
*/

#define UNSPECIFIED_MOUNT_POSITION (-1)

/* structure return from InvokeFont */
struct FontStruct {
    char TroffName[3];
    int PointSize;
    int DoMount;
    int MountPosition;
};


/* INTERNALLY MAINTAINED INFORMATION 
 (mess with this and you are dog meat) 
*/

/* forward declarations for procedures */

#undef EXTERN

#ifdef INVOKEFONT
#define EXTERN 
#else
#define EXTERN extern
#endif

/* the non font */
#define NOFONT	( (INT_type) -1 )

EXTERN struct font_state_struct {
    int id;
    char TroffName[3];
    long seconds;
    long useconds;
} FontState[8] 

#ifdef INVOKEFONT

= {
{ NOFONT, "R", (long) 0, (long) 0},
{ NOFONT, "I", (long) 0, (long) 0},
{ NOFONT, "B", (long) 0, (long) 0},
{ NOFONT, "BI", (long) 0, (long) 0},
{ NOFONT, "H", (long) 0, (long) 0},
{ NOFONT, "HB", (long) 0, (long) 0},
{ NOFONT, "C", (long) 0, (long) 0},
{ NOFONT, "CB", (long) 0, (long) 0},
}

#endif
;




