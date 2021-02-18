/*
 * Copyright IBM Corporation 1987,1988,1989
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
/***********************************************************
		Copyright IBM Corporation 1988

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $Header: /andrew/X11/R3src/Xbeta/server/ddx/ibm/BSDrt/RCS/OSas.h,v 9.0 88/10/16 22:57:21 erik Exp $ */
/* $Source: /andrew/X11/R3src/Xbeta/server/ddx/ibm/BSDrt/RCS/OSas.h,v $ */

#if !defined(lint) && !defined(LOCORE)  && defined(RCS_HDRS)
static char *rcsidOSas = "$Header: /andrew/X11/R3src/Xbeta/server/ddx/ibm/BSDrt/RCS/OSas.h,v 9.0 88/10/16 22:57:21 erik Exp $";
#endif

#define	OSHEADER	.globl	.oVncs;	.set	.oVncs,0
#define	TEXT_ENTRY(n)	_./**/n
#define	DATA_ENTRY(n)	_/**/n
#define	R0	r0
#define	R1	r1
#define	R2	r2
#define	R3	r3
#define	R4	r4
#define	R5	r5
#define	R6	r6
#define	R7	r7
#define	R8	r8
#define	R9	r9
#define	R10	r10
#define	R11	r11
#define	R12	r12
#define	R13	r13
#define	R14	r14
#define	R15	r15

#define	GET(r,c)	get r,$c
#define	L	l
#define	LS	ls

#define NILZ	nilz
#define	NILO	nilo

#define	INC	inc
#define	DEC	dec

#define	CIS	cis
#define	CI	ci

#define	AIS	ais
#define	SIS	sis

#define	A	a

#define	STS	sts
#define	ST	st
#define	STHS	sths
#define	STH	sth

#define	LHS	lhs
#define	LH	lh

#define	CAS	cas
#define	CAL	cal

#define	BX	bx
#define	BRX	brx
#define	BHX	bhx
#define	BH	bh
#define	BHR	bhr
#define	BALIX	balix
#define	BALI	bali
#define	BLR	blr

#define	JZ	jz
#define	JNZ	jnz
#define	J	j
