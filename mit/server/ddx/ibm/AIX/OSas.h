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
/* "@(#)osas.h	3.1 88/09/22 09:29:54" */
#define	OSHEADER
#define	TEXT_ENTRY(n)	./**/n
#define	DATA_ENTRY(n)	_/**/n
#define	R0	0
#define	R1	1
#define	R2	2
#define	R3	3
#define	R4	4
#define	R5	5
#define	R6	6
#define	R7	7
#define	R8	8
#define	R9	9
#define	R10	10
#define	R11	11
#define	R12	12
#define	R13	13
#define	R14	14
#define	R15	15

#define	GET(r,c)	cal16 r,c&0xffff ; oiu r,r,c>16
#define	L	l
#define	LS	l

#define NILZ	nilz
#define	NILO	nilo

#define	INC	inc
#define	DEC	dec

#define	CIS	ci
#define	CI	ci

#define	AIS	ai
#define	SIS	sis

#define	A	a

#define	STS	st
#define	ST	st
#define	STHS	sth
#define	STH	sth

#define	LHS	lh
#define	LH	lh

#define	CAS	cas
#define	CAL	cal

#define	BX	bx
#define	BRX	brx
#define	BHX	bgtx
#define	BH	bgt
#define	BHR	bgtr
#define	BALIX	balix
#define	BALI	bali
#define	BLR	bltr

#define	JZ	beq
#define	JNZ	bne
#define	J	b
