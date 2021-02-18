/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *      November 1985.
 *
 *	%W%	%G%
*/

#ifndef	X11

#define				 INV_PAINT	(PIX_SRC ^ PIX_DST)
#define				 MERGE		PIX_SRC|PIX_DST
#define				 PAINT		PIX_SRC
#define				 ERASE		PIX_NOT(PIX_SRC)

#else

#define				 INV_PAINT	GXxor
#define				 MERGE		GXor
#define				 PAINT		GXcopy
#define				 ERASE		GXcopyInverted

#endif	X11

#define				 DRAW		1
#define				 UNDRAW		0
