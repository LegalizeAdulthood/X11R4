
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/io1.h,v 1.1 89/09/21 06:31:35 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/io1.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	io1.h,v $
 * Revision 1.1  89/09/21  06:31:35  mss
 * Initial revision
 * 
 * Revision 1.10  89/08/29  16:05:39  annm
 * added real names for names that are too long
 * 
 * Revision 1.9  88/09/06  13:20:49  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.8  88/09/06  12:05:03  annm
 * 
 * no change
 * 
 * Revision 1.7  88/07/26  13:28:39  mz0k
 * Added the declarations for IO_WriteByte, IO_ReadByte(s), IO_ReadChar(s)
 * 
 * Revision 1.6  88/06/08  16:57:02  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.5  88/06/08  14:36:10  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_IO1
#define TK_INCLUDED_IO1

#undef EXTERN

#ifdef TK_GVARS_IO1
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_IO

/*
   None of the these routines are exported to the AP yet, but I
   bet they will be soon.
*/

#ifndef USE_LONG_NAMES

#define IO_WriteBytes	iowbts
#define IO_WriteByte	iowbyt
#define IO_WriteChars	iowcrs
#define IO_WriteChar	iowchr
#define IO_ReadBytes	iorbts
#define	IO_ReadByte	iorbyt
#define IO_ReadChars	iorcrs
#define	IO_ReadChar	iorchr
#define IO_PrintInt	iopint
#define IO_PrintString	iopstr
#define IO_PrintByte	iopbyt
#define IO_PrintChar	iopchr

#define PrintInt	giopin
#define StrLength	giosln
#define digits		giodig


#endif

#define IO_Print(od, format)	IO_PrintString(od, format, (CHAR_type *) 0)

EXTERN INT_type		IO_WriteBytes();
EXTERN INT_type		IO_WriteByte();
EXTERN INT_type		IO_WriteChars();
EXTERN INT_type		IO_WriteChar();
EXTERN INT_type		IO_ReadBytes();
EXTERN INT_type		IO_ReadByte();
EXTERN INT_type		IO_ReadChars();
EXTERN INT_type		IO_ReadChar();
EXTERN INT_type		IO_PrintInt();
EXTERN INT_type		IO_PrintString();
EXTERN INT_type		IO_PrintByte();
EXTERN INT_type		IO_PrintChar();

#define IO_Init    gioint	
#define IO_Final   giofnl	

EXTERN INT_type IO_Final();
EXTERN INT_type IO_Init();

#endif

/*
    Declarations seen within the IO module go here.
*/

#ifdef TK_IMPL_IO

#endif

#endif
