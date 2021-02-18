	.globl	.oVncs
	.set	.oVncs,0
	.globl	.oVhc2.1n
	.set	.oVhc2.1n,0
	.globl	.oVO
	.set	.oVO,0
	.data
	.align	2
L00DATA:
	.align	2
_rcsid:	.data	1
	.align	2
L00STRING:
	.align	2
L1:	.ascii	"$Header: /andrew/X11/r3src/release/server/ddx/ibm/vga/RC"
	.ascii	"S/vgaLine.c,v 30.0 89/01/23 19:13:43 paul Exp $\0"
	.data
	.long	L1
	.align	2
_vgaHorzLine:	.long	_.vgaHorzLine
	.text
	.align	2
L00TEXT:
	.globl	_.vgaHorzLine
	.globl	_vgaHorzLine
	.text
#--------------| vgaHorzLine |-----------------------#
_.vgaHorzLine:
	.using	_vgaHorzLine,r14
#/***********************************************************
#		Copyright IBM Corporation 1987,1988
#
#                      All Rights Reserved
#
#Permission to use, copy, modify, and distribute this software and its 
#documentation for any purpose and without fee is hereby granted, 
#provided that the above copyright notice appear in all copies and that
#both that copyright notice and this permission notice appear in 
#supporting documentation, and that the name of IBM not be
#used in advertising or publicity pertaining to distribution of the
#software without specific, written prior permission.  
#
#IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
#ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
#IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
#ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
#WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
#ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
#SOFTWARE.
#
#******************************************************************/
#
#/*
# *  Hardware interface routines for IBM VGA adapter for
# *  X.11 server(s) on IBM equipment.
# *
# *  Last Modified By
# *  P. Shupak -- Oct., 1987
# *
# */
#
#/* $Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/vga/RCS/vgaLine.032.s,v 1.1 89/05/07 15:34:14 jeff Exp $ */
#/* $Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/vga/RCS/vgaLine.032.s,v $ */
#
##ifndef lint
#static char *rcsid = "$Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/vga/RCS/vgaLine.032.s,v 1.1 89/05/07 15:34:14 jeff Exp $" ;
##endif
#
##include "X.h"
#
##include "OScompiler.h"
#
##include "vgaVideo.h"
#
##include "ibmIOArch.h"
#
##include "vgaReg.h"
#
#extern void vgaFillSolid( ) ;
#
	.data
	.align	2
_fast_x_line_right:	.long	_.fast_x_line_right
	.text
	.text
#--------------| fast_x_line_right |-----------------------#
_.fast_x_line_right:
	.using	_fast_x_line_right,r14
#
##if defined(USE_PORTABLE_FAST_LINES)
#
#/* X increasing */
#static void fast_x_line_right( et, e1, e2, len, y_increment, ptx, pty )
	stm	r11,-56(sp)
	cal	sp,-56(sp)
	mr	r12,r5
#register int et ;
#const int e1 ;
#const int e2 ;
#unsigned int len ;
#const int y_increment ;
#const int ptx ;
#const int pty ;
#{
#register unsigned volatile char *addr ;
##if defined(ATRIO)
#register unsigned long int tmp ;
##else
#register unsigned char tmp ;
##endif
#register unsigned char mask ;
#register unsigned single_bit ;
#
##define SINGLE_STEP \
#if ( et < 0 ) { \
#	mask |= single_bit ; \
#	et += e1 ; \
#} \
#else { \
#	tmp = *( (VgaMemoryPtr) addr ) ; \
#	et += e2 ; \
#	*( (VgaMemoryPtr) addr ) = mask ; \
#	mask = single_bit ; \
#	addr += y_increment ; \
#}
#
#/* Set-Up And Set Initial Mask Value */
#addr = VGABASE + BYTE_OFFSET( ptx, pty ) ;
	l	r0,64(sp)	# pty
	mr	r5,r0
	shl	r5,2
	a	r5,r0
	shl	r5,4
	ls	r15,60(sp)	# ptx
	mr	r0,r15
	shra	r0,3
	a	r5,r0
	cau	r0,-12276(r0)
	cas	r13,r0,r5
#mask = single_bit = SCRRIGHT8( LeftmostBit, ( tmp = BIT_OFFSET( ptx ) ) ) ;
	nilz	r11,r15,0x7
	cal	r0,128(r0)
	sra	r0,r11
	nilz	r15,r0,0xff
#tmp = ( tmp + 1 ) & 7 ;
	ais	r11,1
	nilz	r11,r11,0x7
#if ( !( single_bit = SCRRIGHT8( single_bit, 1 ) ) ) {
	mr	r0,r15
	shra	r0,1
	nilz	r0,r0,0xff
	jne	L00043
#	tmp = *( (VgaMemoryPtr) addr ) ;
	ls	r0,0(r13)
#	single_bit = LeftmostBit ;
	cal	r0,128(r0)
#	*( (VgaMemoryPtr) addr++ ) = mask ;
	sts	r15,0(r13)
	inc	r13,1
#	tmp = mask = 0 ;
	lis	r15,0
	lis	r11,0
L00043:
#}
#if ( ( 8 - tmp ) > --len ) {
	sfi	r5,r11,0x8
	sis	r12,1
	cl	r5,r12
	jnh	L00045
#	tmp = len ;
	mr	r11,r12
#	len = 8 ;
	bx	L000446
	lis	r12,8
L00045:
#}
#else if ( tmp ) {
	cis	r11,0
	beq	L000446
#	len += tmp ;	/* Round-Up */
	a	r12,r11
#	tmp = 8 - tmp ;
	bx	L000446
	mr	r11,r5
L00047:
#}
#
#branchPoint:
#switch ( tmp ) {
#	Loop_Top:
#	/* */
#	/* First -- Leftmost -- Most-Significant Bit In Byte */
#	case 0 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
	cis	r2,0
	jhe	L00049
	o	r15,r0
	nilz	r15,r15,0xff
	bx	L000410
	a	r2,r3
L00049:
	ls	r5,0(r13)
	a	r2,r4
	sts	r15,0(r13)
	nilz	r15,r0,0xff
	ls	r5,56(sp)	# y_increment
	a	r13,r5
L000410:
	nilz	r0,r0,0xff
	shra	r0,1
	nilz	r0,r0,0xff
L000411:
#	case 7 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
	cis	r2,0
	jhe	L000413
	o	r15,r0
	nilz	r15,r15,0xff
	bx	L000414
	a	r2,r3
L000413:
	ls	r5,0(r13)
	a	r2,r4
	sts	r15,0(r13)
	nilz	r15,r0,0xff
	ls	r5,56(sp)	# y_increment
	a	r13,r5
L000414:
	nilz	r0,r0,0xff
	shra	r0,1
	nilz	r0,r0,0xff
L000415:
#	case 6 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
	cis	r2,0
	jhe	L000417
	o	r15,r0
	nilz	r15,r15,0xff
	bx	L000418
	a	r2,r3
L000417:
	ls	r5,0(r13)
	a	r2,r4
	sts	r15,0(r13)
	nilz	r15,r0,0xff
	ls	r5,56(sp)	# y_increment
	a	r13,r5
L000418:
	nilz	r0,r0,0xff
	shra	r0,1
	nilz	r0,r0,0xff
L000419:
#	case 5 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
	cis	r2,0
	jhe	L000421
	o	r15,r0
	nilz	r15,r15,0xff
	bx	L000422
	a	r2,r3
L000421:
	ls	r5,0(r13)
	a	r2,r4
	sts	r15,0(r13)
	nilz	r15,r0,0xff
	ls	r5,56(sp)	# y_increment
	a	r13,r5
L000422:
	nilz	r0,r0,0xff
	shra	r0,1
	nilz	r0,r0,0xff
L000423:
#	case 4 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
	cis	r2,0
	jhe	L000425
	o	r15,r0
	nilz	r15,r15,0xff
	bx	L000426
	a	r2,r3
L000425:
	ls	r5,0(r13)
	a	r2,r4
	sts	r15,0(r13)
	nilz	r15,r0,0xff
	ls	r5,56(sp)	# y_increment
	a	r13,r5
L000426:
	nilz	r0,r0,0xff
	shra	r0,1
	nilz	r0,r0,0xff
L000427:
#	case 3 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
	cis	r2,0
	jhe	L000429
	o	r15,r0
	nilz	r15,r15,0xff
	bx	L000430
	a	r2,r3
L000429:
	ls	r5,0(r13)
	a	r2,r4
	sts	r15,0(r13)
	nilz	r15,r0,0xff
	ls	r5,56(sp)	# y_increment
	a	r13,r5
L000430:
	nilz	r0,r0,0xff
	shra	r0,1
	nilz	r0,r0,0xff
L000431:
#	case 2 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
	cis	r2,0
	jhe	L000433
	o	r15,r0
	nilz	r15,r15,0xff
	bx	L000434
	a	r2,r3
L000433:
	ls	r5,0(r13)
	a	r2,r4
	sts	r15,0(r13)
	nilz	r15,r0,0xff
	ls	r5,56(sp)	# y_increment
	a	r13,r5
L000434:
	nilz	r0,r0,0xff
	shra	r0,1
	nilz	r0,r0,0xff
L000435:
#	/* Last -- Rightmost -- Least-Significant Bit In Byte */
#	case 1 : SINGLE_STEP ;
	cis	r2,0
	jhe	L000437
	o	r0,r15
	nilz	r15,r0,0xff
	bx	L000438
	a	r2,r3
L000437:
	ls	r5,0(r13)
	a	r2,r4
	sts	r15,0(r13)
	nilz	r15,r0,0xff
	ls	r0,56(sp)	# y_increment
	a	r13,r0
L000438:
#	/* Flush buffered mask */
#	tmp = *( (VgaMemoryPtr) addr ) ;
	ls	r0,0(r13)
#	len -= 8 ;
	sis	r12,8
#	*( (VgaMemoryPtr) addr ) = mask ;
	sts	r15,0(r13)
#	if ( len > 7 ) {
	cli	r12,7
	jnh	L000441
#		mask = 0 ;
	lis	r15,0
#		addr++ ;
	inc	r13,1
#		single_bit = LeftmostBit ;
	bx	L00047
	cal	r0,128(r0)
L000441:
#		goto Loop_Top ;
#	}
#	else if ( len ) {
	cis	r12,0
	jeq	L000448
#		tmp = len ;
	mr	r11,r12
#		mask = 0 ;
	lis	r15,0
#		addr++ ;
	inc	r13,1
#		len = 8 ;
	lis	r12,8
#		single_bit = LeftmostBit ;
	cal	r0,128(r0)
L000446:
	cli	r11,7
	jh	L000448
	a	r11,r11
	lda	r5,LL00449(r11)
	lhas	r11,0(r5)
	a	r11,r5
	br	r11
LL00449:
	.short	L00047-LL00449
	.short	L000435-LL00449-2
	.short	L000431-LL00449-4
	.short	L000427-LL00449-6
	.short	L000423-LL00449-8
	.short	L000419-LL00449-10
	.short	L000415-LL00449-12
	.short	L000411-LL00449-14
L000448:
#		goto branchPoint ;
#	}
#	else
#		return ;
#}
##undef SINGLE_STEP
#/*NOTREACHED*/
#}
	lm	r11,0(sp)
	brx	r15
	cal	sp,56(sp)
	.byte	0xdf,0x07,0xdf	#Trace table for fast_x_line_right
	.byte	0xb8	# First nonvolatile register saved is r11
	.byte	0x71	# npars=7 words
	.byte	0xe	# lcl_offset=56
	.align	1
	.data
	.align	2
_fast_x_line_left:	.long	_.fast_x_line_left
	.text
	.text
#--------------| fast_x_line_left |-----------------------#
_.fast_x_line_left:
	.using	_fast_x_line_left,r14
#
#/* X decreasing */
#static void fast_x_line_left( et, e1, e2, len, y_increment, ptx, pty )
	stm	r11,-56(sp)
	cal	sp,-56(sp)
	mr	r12,r5
#register int et ;
#const int e1 ;
#const int e2 ;
#unsigned int len ;
#const int y_increment ;
#const int ptx ;
#const int pty ;
#{
#register unsigned volatile char *addr ;
##if defined(ATRIO)
#register unsigned long int tmp ;
##else
#register unsigned char tmp ;
##endif
#register unsigned char mask ;
#register unsigned single_bit ;
#
##define SINGLE_STEP \
#if ( et < 0 ) { \
#	mask |= single_bit ; \
#	et += e1 ; \
#} \
#else { \
#	tmp = *( (VgaMemoryPtr) addr ) ; \
#	et += e2 ; \
#	*( (VgaMemoryPtr) addr ) = mask ; \
#	mask = single_bit ; \
#	addr += y_increment ; \
#}
#
#/* Set-Up And Set Initial Mask Value */
#addr = VGABASE + BYTE_OFFSET( ptx, pty ) ;
	l	r0,64(sp)	# pty
	mr	r5,r0
	shl	r5,2
	a	r5,r0
	shl	r5,4
	ls	r15,60(sp)	# ptx
	mr	r0,r15
	shra	r0,3
	a	r5,r0
	cau	r0,-12276(r0)
	cas	r13,r0,r5
#mask = single_bit = SCRLEFT8( RightmostBit, ( tmp = 7 - BIT_OFFSET( ptx ) ) ) ;
	nilz	r11,r15,0x7
	sfi	r11,r11,0x7
	lis	r0,1
	sl	r0,r11
	nilz	r15,r0,0xff
#tmp = ( tmp + 1 ) & 7 ;
	ais	r11,1
	nilz	r11,r11,0x7
#if ( ( single_bit = SCRLEFT8( single_bit, 1 ) ) == SCRLEFT8( RightmostBit, 8 ) ) {
	mr	r0,r15
	shl	r0,1
	nilz	r0,r0,0xff
	jne	L00053
#	tmp = *( (VgaMemoryPtr) addr ) ;
	ls	r0,0(r13)
#	single_bit = RightmostBit ;
	lis	r0,1
#	*( (VgaMemoryPtr) addr-- ) = mask ;
	sts	r15,0(r13)
	dec	r13,1
#	tmp = mask = 0 ;
	lis	r15,0
	lis	r11,0
L00053:
#}
#if ( ( 8 - tmp ) > --len ) {
	sfi	r5,r11,0x8
	sis	r12,1
	cl	r5,r12
	jnh	L00055
#	tmp = len ;
	mr	r11,r12
#	len = 8 ;
	bx	L000546
	lis	r12,8
L00055:
#}
#else if ( tmp ) {
	cis	r11,0
	beq	L000546
#	len += tmp ;	/* Round-Up */
	a	r12,r11
#	tmp = 8 - tmp ;
	bx	L000546
	mr	r11,r5
L00057:
#}
#
#branchPoint:
#switch ( tmp ) {
#	Loop_Top:
#	/* */
#	/* First -- Rightmost -- Least-Significant Bit In Byte */
#	case 0 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
	cis	r2,0
	jhe	L00059
	o	r15,r0
	nilz	r15,r15,0xff
	bx	L000510
	a	r2,r3
L00059:
	ls	r5,0(r13)
	a	r2,r4
	sts	r15,0(r13)
	nilz	r15,r0,0xff
	ls	r5,56(sp)	# y_increment
	a	r13,r5
L000510:
	nilz	r0,r0,0xff
	shl	r0,1
	nilz	r0,r0,0xff
L000511:
#	case 7 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
	cis	r2,0
	jhe	L000513
	o	r15,r0
	nilz	r15,r15,0xff
	bx	L000514
	a	r2,r3
L000513:
	ls	r5,0(r13)
	a	r2,r4
	sts	r15,0(r13)
	nilz	r15,r0,0xff
	ls	r5,56(sp)	# y_increment
	a	r13,r5
L000514:
	nilz	r0,r0,0xff
	shl	r0,1
	nilz	r0,r0,0xff
L000515:
#	case 6 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
	cis	r2,0
	jhe	L000517
	o	r15,r0
	nilz	r15,r15,0xff
	bx	L000518
	a	r2,r3
L000517:
	ls	r5,0(r13)
	a	r2,r4
	sts	r15,0(r13)
	nilz	r15,r0,0xff
	ls	r5,56(sp)	# y_increment
	a	r13,r5
L000518:
	nilz	r0,r0,0xff
	shl	r0,1
	nilz	r0,r0,0xff
L000519:
#	case 5 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
	cis	r2,0
	jhe	L000521
	o	r15,r0
	nilz	r15,r15,0xff
	bx	L000522
	a	r2,r3
L000521:
	ls	r5,0(r13)
	a	r2,r4
	sts	r15,0(r13)
	nilz	r15,r0,0xff
	ls	r5,56(sp)	# y_increment
	a	r13,r5
L000522:
	nilz	r0,r0,0xff
	shl	r0,1
	nilz	r0,r0,0xff
L000523:
#	case 4 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
	cis	r2,0
	jhe	L000525
	o	r15,r0
	nilz	r15,r15,0xff
	bx	L000526
	a	r2,r3
L000525:
	ls	r5,0(r13)
	a	r2,r4
	sts	r15,0(r13)
	nilz	r15,r0,0xff
	ls	r5,56(sp)	# y_increment
	a	r13,r5
L000526:
	nilz	r0,r0,0xff
	shl	r0,1
	nilz	r0,r0,0xff
L000527:
#	case 3 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
	cis	r2,0
	jhe	L000529
	o	r15,r0
	nilz	r15,r15,0xff
	bx	L000530
	a	r2,r3
L000529:
	ls	r5,0(r13)
	a	r2,r4
	sts	r15,0(r13)
	nilz	r15,r0,0xff
	ls	r5,56(sp)	# y_increment
	a	r13,r5
L000530:
	nilz	r0,r0,0xff
	shl	r0,1
	nilz	r0,r0,0xff
L000531:
#	case 2 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
	cis	r2,0
	jhe	L000533
	o	r15,r0
	nilz	r15,r15,0xff
	bx	L000534
	a	r2,r3
L000533:
	ls	r5,0(r13)
	a	r2,r4
	sts	r15,0(r13)
	nilz	r15,r0,0xff
	ls	r5,56(sp)	# y_increment
	a	r13,r5
L000534:
	nilz	r0,r0,0xff
	shl	r0,1
	nilz	r0,r0,0xff
L000535:
#	/* Last -- Leftmost -- Most-Significant Bit In Byte */
#	case 1 : SINGLE_STEP ;
	cis	r2,0
	jhe	L000537
	o	r0,r15
	nilz	r15,r0,0xff
	bx	L000538
	a	r2,r3
L000537:
	ls	r5,0(r13)
	a	r2,r4
	sts	r15,0(r13)
	nilz	r15,r0,0xff
	ls	r0,56(sp)	# y_increment
	a	r13,r0
L000538:
#	/* Flush buffered mask */
#	tmp = *( (VgaMemoryPtr) addr ) ;
	ls	r0,0(r13)
#	len -= 8 ;
	sis	r12,8
#	*( (VgaMemoryPtr) addr ) = mask ;
	sts	r15,0(r13)
#	if ( len > 7 ) {
	cli	r12,7
	jnh	L000541
#		mask = 0 ;
	lis	r15,0
#		addr-- ;
	dec	r13,1
#		single_bit = RightmostBit ;
	bx	L00057
	lis	r0,1
L000541:
#		goto Loop_Top ;
#	}
#	else if ( len ) {
	cis	r12,0
	jeq	L000548
#		tmp = len ;
	mr	r11,r12
#		mask = 0 ;
	lis	r15,0
#		addr-- ;
	dec	r13,1
#		len = 8 ;
	lis	r12,8
#		single_bit = RightmostBit ;
	lis	r0,1
L000546:
	cli	r11,7
	jh	L000548
	a	r11,r11
	lda	r5,LL00549(r11)
	lhas	r11,0(r5)
	a	r11,r5
	br	r11
LL00549:
	.short	L00057-LL00549
	.short	L000535-LL00549-2
	.short	L000531-LL00549-4
	.short	L000527-LL00549-6
	.short	L000523-LL00549-8
	.short	L000519-LL00549-10
	.short	L000515-LL00549-12
	.short	L000511-LL00549-14
L000548:
#		goto branchPoint ;
#	}
#	else
#		return ;
#}
##undef SINGLE_STEP
#/*NOTREACHED*/
#}
	lm	r11,0(sp)
	brx	r15
	cal	sp,56(sp)
	.byte	0xdf,0x07,0xdf	#Trace table for fast_x_line_left
	.byte	0xb8	# First nonvolatile register saved is r11
	.byte	0x71	# npars=7 words
	.byte	0xe	# lcl_offset=56
	.align	1
	.data
	.align	2
_fast_y_line_right:	.long	_.fast_y_line_right
	.text
	.text
#--------------| fast_y_line_right |-----------------------#
_.fast_y_line_right:
	.using	_fast_y_line_right,r14
#
#/* X increasing */
#static void fast_y_line_right( et, e1, e2, len, y_increment, ptx, pty )
	stm	r11,-56(sp)
	cal	sp,-56(sp)
	mr	r12,r5
#register int et ;
#const int e1 ;
#const int e2 ;
#unsigned int len ;
#const int y_increment ;
#const int ptx ;
#const int pty ;
#{
#register unsigned single_bit ;
#register unsigned volatile char *addr ;
##if defined(ATRIO)
#register unsigned long int tmp ;
##else
#register unsigned char tmp ;
##endif
#
##define SINGLE_STEP \
#addr += y_increment ; \
#if ( et < 0 ) { \
#	tmp = *( (VgaMemoryPtr) addr ) ; \
#	et += e1 ; \
#} \
#else { \
#	et += e2 ; \
#	if ( single_bit = SCRRIGHT8( single_bit, 1 ) ) { \
#		tmp = *( (VgaMemoryPtr) addr ) ; \
#	} \
#	else { \
#		tmp = *( (VgaMemoryPtr) ( ++addr ) ) ; \
#		single_bit = LeftmostBit ; \
#	} \
#} \
#*( (VgaMemoryPtr) addr ) = single_bit
#/* END MACRO */
#
#/* Draw First Dot */
#tmp = *( (VgaMemoryPtr) ( addr = VGABASE + BYTE_OFFSET( ptx, pty ) ) ) ;
	l	r0,64(sp)	# pty
	mr	r5,r0
	shl	r5,2
	a	r5,r0
	shl	r5,4
	ls	r13,60(sp)	# ptx
	mr	r0,r13
	shra	r0,3
	a	r5,r0
	cau	r0,-12276(r0)
	cas	r15,r0,r5
	ls	r0,0(r15)
#*( (VgaMemoryPtr) addr ) =
	nilz	r5,r13,0x7
	cal	r0,128(r0)
	sra	r0,r5
	nilz	r13,r0,0xff
	sts	r13,0(r15)
#	( single_bit = SCRRIGHT8( LeftmostBit, BIT_OFFSET( ptx ) ) ) ;
#
#if ( tmp = --len & 0xF ) /* Already Drew First Dot */
	sis	r12,1
	nilz	r11,r12,0xf
	jeq	L00063
#	len += 0xF ;
	ais	r12,15
L00063:
#len >>= 0x4 ;
	shr	r12,4
#
#switch ( tmp ) {
	cli	r11,15
	bh	L0006106
	ls	r0,56(sp)	# y_increment
	a	r11,r11
	lda	r5,LL006107(r11)
	lhas	r11,0(r5)
	a	r11,r5
	br	r11
LL006107:
	.short	L00064-LL006107
	.short	L000694-LL006107-2
	.short	L000688-LL006107-4
	.short	L000682-LL006107-6
	.short	L000676-LL006107-8
	.short	L000670-LL006107-10
	.short	L000664-LL006107-12
	.short	L000658-LL006107-14
	.short	L000652-LL006107-16
	.short	L000646-LL006107-18
	.short	L000640-LL006107-20
	.short	L000634-LL006107-22
	.short	L000628-LL006107-24
	.short	L000622-LL006107-26
	.short	L000616-LL006107-28
	.short	L000610-LL006107-30
L00064:
#	Loop_Top:
#	/* */
#	case 0x0 : SINGLE_STEP ;
	a	r15,r0
	cis	r2,0
	jhe	L00066
	ls	r5,0(r15)
	bx	L00069
	a	r2,r3
L00066:
	a	r2,r4
	nilz	r5,r13,0xff
	shra	r5,1
	nilz	r13,r5,0xff
	jeq	L00068
	bx	L00069
	ls	r5,0(r15)
L00068:
	inc	r15,1
	ls	r5,0(r15)
	cal	r13,128(r0)
L00069:
	sts	r13,0(r15)
L000610:
#	case 0xF : SINGLE_STEP ;
	a	r15,r0
	cis	r2,0
	jhe	L000612
	ls	r5,0(r15)
	bx	L000615
	a	r2,r3
L000612:
	a	r2,r4
	nilz	r5,r13,0xff
	shra	r5,1
	nilz	r13,r5,0xff
	jeq	L000614
	bx	L000615
	ls	r5,0(r15)
L000614:
	inc	r15,1
	ls	r5,0(r15)
	cal	r13,128(r0)
L000615:
	sts	r13,0(r15)
L000616:
#	case 0xE : SINGLE_STEP ;
	a	r15,r0
	cis	r2,0
	jhe	L000618
	ls	r5,0(r15)
	bx	L000621
	a	r2,r3
L000618:
	a	r2,r4
	nilz	r5,r13,0xff
	shra	r5,1
	nilz	r13,r5,0xff
	jeq	L000620
	bx	L000621
	ls	r5,0(r15)
L000620:
	inc	r15,1
	ls	r5,0(r15)
	cal	r13,128(r0)
L000621:
	sts	r13,0(r15)
L000622:
#	case 0xD : SINGLE_STEP ;
	a	r15,r0
	cis	r2,0
	jhe	L000624
	ls	r5,0(r15)
	bx	L000627
	a	r2,r3
L000624:
	a	r2,r4
	nilz	r5,r13,0xff
	shra	r5,1
	nilz	r13,r5,0xff
	jeq	L000626
	bx	L000627
	ls	r5,0(r15)
L000626:
	inc	r15,1
	ls	r5,0(r15)
	cal	r13,128(r0)
L000627:
	sts	r13,0(r15)
L000628:
#	case 0xC : SINGLE_STEP ;
	a	r15,r0
	cis	r2,0
	jhe	L000630
	ls	r5,0(r15)
	bx	L000633
	a	r2,r3
L000630:
	a	r2,r4
	nilz	r5,r13,0xff
	shra	r5,1
	nilz	r13,r5,0xff
	jeq	L000632
	bx	L000633
	ls	r5,0(r15)
L000632:
	inc	r15,1
	ls	r5,0(r15)
	cal	r13,128(r0)
L000633:
	sts	r13,0(r15)
L000634:
#	case 0xB : SINGLE_STEP ;
	a	r15,r0
	cis	r2,0
	jhe	L000636
	ls	r5,0(r15)
	bx	L000639
	a	r2,r3
L000636:
	a	r2,r4
	nilz	r5,r13,0xff
	shra	r5,1
	nilz	r13,r5,0xff
	jeq	L000638
	bx	L000639
	ls	r5,0(r15)
L000638:
	inc	r15,1
	ls	r5,0(r15)
	cal	r13,128(r0)
L000639:
	sts	r13,0(r15)
L000640:
#	case 0xA : SINGLE_STEP ;
	a	r15,r0
	cis	r2,0
	jhe	L000642
	ls	r5,0(r15)
	bx	L000645
	a	r2,r3
L000642:
	a	r2,r4
	nilz	r5,r13,0xff
	shra	r5,1
	nilz	r13,r5,0xff
	jeq	L000644
	bx	L000645
	ls	r5,0(r15)
L000644:
	inc	r15,1
	ls	r5,0(r15)
	cal	r13,128(r0)
L000645:
	sts	r13,0(r15)
L000646:
#	case 0x9 : SINGLE_STEP ;
	a	r15,r0
	cis	r2,0
	jhe	L000648
	ls	r5,0(r15)
	bx	L000651
	a	r2,r3
L000648:
	a	r2,r4
	nilz	r5,r13,0xff
	shra	r5,1
	nilz	r13,r5,0xff
	jeq	L000650
	bx	L000651
	ls	r5,0(r15)
L000650:
	inc	r15,1
	ls	r5,0(r15)
	cal	r13,128(r0)
L000651:
	sts	r13,0(r15)
L000652:
#	case 0x8 : SINGLE_STEP ;
	a	r15,r0
	cis	r2,0
	jhe	L000654
	ls	r5,0(r15)
	bx	L000657
	a	r2,r3
L000654:
	a	r2,r4
	nilz	r5,r13,0xff
	shra	r5,1
	nilz	r13,r5,0xff
	jeq	L000656
	bx	L000657
	ls	r5,0(r15)
L000656:
	inc	r15,1
	ls	r5,0(r15)
	cal	r13,128(r0)
L000657:
	sts	r13,0(r15)
L000658:
#	case 0x7 : SINGLE_STEP ;
	a	r15,r0
	cis	r2,0
	jhe	L000660
	ls	r5,0(r15)
	bx	L000663
	a	r2,r3
L000660:
	a	r2,r4
	nilz	r5,r13,0xff
	shra	r5,1
	nilz	r13,r5,0xff
	jeq	L000662
	bx	L000663
	ls	r5,0(r15)
L000662:
	inc	r15,1
	ls	r5,0(r15)
	cal	r13,128(r0)
L000663:
	sts	r13,0(r15)
L000664:
#	case 0x6 : SINGLE_STEP ;
	a	r15,r0
	cis	r2,0
	jhe	L000666
	ls	r5,0(r15)
	bx	L000669
	a	r2,r3
L000666:
	a	r2,r4
	nilz	r5,r13,0xff
	shra	r5,1
	nilz	r13,r5,0xff
	jeq	L000668
	bx	L000669
	ls	r5,0(r15)
L000668:
	inc	r15,1
	ls	r5,0(r15)
	cal	r13,128(r0)
L000669:
	sts	r13,0(r15)
L000670:
#	case 0x5 : SINGLE_STEP ;
	a	r15,r0
	cis	r2,0
	jhe	L000672
	ls	r5,0(r15)
	bx	L000675
	a	r2,r3
L000672:
	a	r2,r4
	nilz	r5,r13,0xff
	shra	r5,1
	nilz	r13,r5,0xff
	jeq	L000674
	bx	L000675
	ls	r5,0(r15)
L000674:
	inc	r15,1
	ls	r5,0(r15)
	cal	r13,128(r0)
L000675:
	sts	r13,0(r15)
L000676:
#	case 0x4 : SINGLE_STEP ;
	a	r15,r0
	cis	r2,0
	jhe	L000678
	ls	r5,0(r15)
	bx	L000681
	a	r2,r3
L000678:
	a	r2,r4
	nilz	r5,r13,0xff
	shra	r5,1
	nilz	r13,r5,0xff
	jeq	L000680
	bx	L000681
	ls	r5,0(r15)
L000680:
	inc	r15,1
	ls	r5,0(r15)
	cal	r13,128(r0)
L000681:
	sts	r13,0(r15)
L000682:
#	case 0x3 : SINGLE_STEP ;
	a	r15,r0
	cis	r2,0
	jhe	L000684
	ls	r5,0(r15)
	bx	L000687
	a	r2,r3
L000684:
	a	r2,r4
	nilz	r5,r13,0xff
	shra	r5,1
	nilz	r13,r5,0xff
	jeq	L000686
	bx	L000687
	ls	r5,0(r15)
L000686:
	inc	r15,1
	ls	r5,0(r15)
	cal	r13,128(r0)
L000687:
	sts	r13,0(r15)
L000688:
#	case 0x2 : SINGLE_STEP ;
	a	r15,r0
	cis	r2,0
	jhe	L000690
	ls	r5,0(r15)
	bx	L000693
	a	r2,r3
L000690:
	a	r2,r4
	nilz	r5,r13,0xff
	shra	r5,1
	nilz	r13,r5,0xff
	jeq	L000692
	bx	L000693
	ls	r5,0(r15)
L000692:
	inc	r15,1
	ls	r5,0(r15)
	cal	r13,128(r0)
L000693:
	sts	r13,0(r15)
L000694:
#	case 0x1 : SINGLE_STEP ;
	a	r15,r0
	cis	r2,0
	jhe	L000696
	ls	r5,0(r15)
	bx	L000699
	a	r2,r3
L000696:
	a	r2,r4
	nilz	r5,r13,0xff
	shra	r5,1
	nilz	r13,r5,0xff
	jeq	L000698
	bx	L000699
	ls	r5,0(r15)
L000698:
	inc	r15,1
	ls	r5,0(r15)
	cal	r13,128(r0)
L000699:
	sts	r13,0(r15)
#	/* */
#	if ( --len )
	sis	r12,1
	bne	L00064
L0006106:
#		goto Loop_Top ;
#	else
#		return ;
#}
##undef SINGLE_STEP
#
#/*NOTREACHED*/
#}
	lm	r11,0(sp)
	brx	r15
	cal	sp,56(sp)
	.byte	0xdf,0x07,0xdf	#Trace table for fast_y_line_right
	.byte	0xb8	# First nonvolatile register saved is r11
	.byte	0x71	# npars=7 words
	.byte	0xe	# lcl_offset=56
	.align	1
	.data
	.align	2
_fast_y_line_left:	.long	_.fast_y_line_left
	.text
	.text
#--------------| fast_y_line_left |-----------------------#
_.fast_y_line_left:
	.using	_fast_y_line_left,r14
#
#/* X decreasing */
#static void fast_y_line_left( et, e1, e2, len, y_increment, ptx, pty )
	stm	r10,-60(sp)
	cal	sp,-60(sp)
	mr	r11,r5
#register int et ;
#const int e1 ;
#const int e2 ;
#unsigned int len ;
#const int y_increment ;
#const int ptx ;
#const int pty ;
#{
#register unsigned volatile char *addr ;
#register unsigned single_bit ;
##if ( !defined(OLDHC) && defined(BSDrt) ) && !defined(i386)
#register const unsigned stop_bit = SCRLEFT8( RightmostBit, 8 ) ;
	lis	r15,0
##else
##define stop_bit SCRLEFT8( RightmostBit, 8 )
##endif
##if defined(ATRIO)
#register unsigned long int tmp ;
##else
#register unsigned char tmp ;
##endif
#
##define SINGLE_STEP \
#addr += y_increment ; \
#if ( et < 0 ) { \
#	tmp = *( (VgaMemoryPtr) addr ) ; \
#	et += e1 ; \
#} \
#else { \
#	et += e2 ; \
#	if ( ( single_bit = SCRLEFT8( single_bit, 1 ) ) != stop_bit ) { \
#		tmp = *( (VgaMemoryPtr) addr ) ; \
#	} \
#	else { \
#		tmp = *( (VgaMemoryPtr) ( --addr ) ) ; \
#		single_bit = RightmostBit ; \
#	} \
#} \
#*( (VgaMemoryPtr) addr ) = single_bit
#/* END MACRO */
#
#/* Draw First Dot */
#tmp = *( (VgaMemoryPtr) ( addr = VGABASE + BYTE_OFFSET( ptx, pty ) ) ) ;
	l	r0,68(sp)	# pty
	mr	r5,r0
	shl	r5,2
	a	r5,r0
	shl	r5,4
	l	r13,64(sp)	# ptx
	mr	r0,r13
	shra	r0,3
	a	r5,r0
	cau	r0,-12276(r0)
	cas	r12,r0,r5
	ls	r0,0(r12)
#*( (VgaMemoryPtr) addr ) =
	nilz	r5,r13,0x7
	sfi	r5,r5,0x7
	lis	r0,1
	sl	r0,r5
	nilz	r13,r0,0xff
	sts	r13,0(r12)
#	( single_bit = SCRLEFT8( RightmostBit, ( 7 - BIT_OFFSET( ptx ) ) ) ) ;
#
#if ( tmp = --len & 0xF ) /* Already Drew First Dot */
	sis	r11,1
	nilz	r10,r11,0xf
	jeq	L00073
#	len += 0xF ;
	ais	r11,15
L00073:
#len >>= 0x4 ;
	shr	r11,4
#
#switch ( tmp ) {
	cli	r10,15
	bh	L0007106
	ls	r0,60(sp)	# y_increment
	a	r10,r10
	lda	r5,LL007107(r10)
	lhas	r10,0(r5)
	a	r10,r5
	br	r10
LL007107:
	.short	L00074-LL007107
	.short	L000794-LL007107-2
	.short	L000788-LL007107-4
	.short	L000782-LL007107-6
	.short	L000776-LL007107-8
	.short	L000770-LL007107-10
	.short	L000764-LL007107-12
	.short	L000758-LL007107-14
	.short	L000752-LL007107-16
	.short	L000746-LL007107-18
	.short	L000740-LL007107-20
	.short	L000734-LL007107-22
	.short	L000728-LL007107-24
	.short	L000722-LL007107-26
	.short	L000716-LL007107-28
	.short	L000710-LL007107-30
L00074:
#	Loop_Top:
#	/* */
#	case 0x0 : SINGLE_STEP ;
	a	r12,r0
	cis	r2,0
	jhe	L00076
	ls	r5,0(r12)
	bx	L00079
	a	r2,r3
L00076:
	a	r2,r4
	nilz	r5,r13,0xff
	shl	r5,1
	nilz	r13,r5,0xff
	c	r13,r15
	jeq	L00078
	bx	L00079
	ls	r5,0(r12)
L00078:
	dec	r12,1
	ls	r5,0(r12)
	lis	r13,1
L00079:
	sts	r13,0(r12)
L000710:
#	case 0xF : SINGLE_STEP ;
	a	r12,r0
	cis	r2,0
	jhe	L000712
	ls	r5,0(r12)
	bx	L000715
	a	r2,r3
L000712:
	a	r2,r4
	nilz	r5,r13,0xff
	shl	r5,1
	nilz	r13,r5,0xff
	c	r13,r15
	jeq	L000714
	bx	L000715
	ls	r5,0(r12)
L000714:
	dec	r12,1
	ls	r5,0(r12)
	lis	r13,1
L000715:
	sts	r13,0(r12)
L000716:
#	case 0xE : SINGLE_STEP ;
	a	r12,r0
	cis	r2,0
	jhe	L000718
	ls	r5,0(r12)
	bx	L000721
	a	r2,r3
L000718:
	a	r2,r4
	nilz	r5,r13,0xff
	shl	r5,1
	nilz	r13,r5,0xff
	c	r13,r15
	jeq	L000720
	bx	L000721
	ls	r5,0(r12)
L000720:
	dec	r12,1
	ls	r5,0(r12)
	lis	r13,1
L000721:
	sts	r13,0(r12)
L000722:
#	case 0xD : SINGLE_STEP ;
	a	r12,r0
	cis	r2,0
	jhe	L000724
	ls	r5,0(r12)
	bx	L000727
	a	r2,r3
L000724:
	a	r2,r4
	nilz	r5,r13,0xff
	shl	r5,1
	nilz	r13,r5,0xff
	c	r13,r15
	jeq	L000726
	bx	L000727
	ls	r5,0(r12)
L000726:
	dec	r12,1
	ls	r5,0(r12)
	lis	r13,1
L000727:
	sts	r13,0(r12)
L000728:
#	case 0xC : SINGLE_STEP ;
	a	r12,r0
	cis	r2,0
	jhe	L000730
	ls	r5,0(r12)
	bx	L000733
	a	r2,r3
L000730:
	a	r2,r4
	nilz	r5,r13,0xff
	shl	r5,1
	nilz	r13,r5,0xff
	c	r13,r15
	jeq	L000732
	bx	L000733
	ls	r5,0(r12)
L000732:
	dec	r12,1
	ls	r5,0(r12)
	lis	r13,1
L000733:
	sts	r13,0(r12)
L000734:
#	case 0xB : SINGLE_STEP ;
	a	r12,r0
	cis	r2,0
	jhe	L000736
	ls	r5,0(r12)
	bx	L000739
	a	r2,r3
L000736:
	a	r2,r4
	nilz	r5,r13,0xff
	shl	r5,1
	nilz	r13,r5,0xff
	c	r13,r15
	jeq	L000738
	bx	L000739
	ls	r5,0(r12)
L000738:
	dec	r12,1
	ls	r5,0(r12)
	lis	r13,1
L000739:
	sts	r13,0(r12)
L000740:
#	case 0xA : SINGLE_STEP ;
	a	r12,r0
	cis	r2,0
	jhe	L000742
	ls	r5,0(r12)
	bx	L000745
	a	r2,r3
L000742:
	a	r2,r4
	nilz	r5,r13,0xff
	shl	r5,1
	nilz	r13,r5,0xff
	c	r13,r15
	jeq	L000744
	bx	L000745
	ls	r5,0(r12)
L000744:
	dec	r12,1
	ls	r5,0(r12)
	lis	r13,1
L000745:
	sts	r13,0(r12)
L000746:
#	case 0x9 : SINGLE_STEP ;
	a	r12,r0
	cis	r2,0
	jhe	L000748
	ls	r5,0(r12)
	bx	L000751
	a	r2,r3
L000748:
	a	r2,r4
	nilz	r5,r13,0xff
	shl	r5,1
	nilz	r13,r5,0xff
	c	r13,r15
	jeq	L000750
	bx	L000751
	ls	r5,0(r12)
L000750:
	dec	r12,1
	ls	r5,0(r12)
	lis	r13,1
L000751:
	sts	r13,0(r12)
L000752:
#	case 0x8 : SINGLE_STEP ;
	a	r12,r0
	cis	r2,0
	jhe	L000754
	ls	r5,0(r12)
	bx	L000757
	a	r2,r3
L000754:
	a	r2,r4
	nilz	r5,r13,0xff
	shl	r5,1
	nilz	r13,r5,0xff
	c	r13,r15
	jeq	L000756
	bx	L000757
	ls	r5,0(r12)
L000756:
	dec	r12,1
	ls	r5,0(r12)
	lis	r13,1
L000757:
	sts	r13,0(r12)
L000758:
#	case 0x7 : SINGLE_STEP ;
	a	r12,r0
	cis	r2,0
	jhe	L000760
	ls	r5,0(r12)
	bx	L000763
	a	r2,r3
L000760:
	a	r2,r4
	nilz	r5,r13,0xff
	shl	r5,1
	nilz	r13,r5,0xff
	c	r13,r15
	jeq	L000762
	bx	L000763
	ls	r5,0(r12)
L000762:
	dec	r12,1
	ls	r5,0(r12)
	lis	r13,1
L000763:
	sts	r13,0(r12)
L000764:
#	case 0x6 : SINGLE_STEP ;
	a	r12,r0
	cis	r2,0
	jhe	L000766
	ls	r5,0(r12)
	bx	L000769
	a	r2,r3
L000766:
	a	r2,r4
	nilz	r5,r13,0xff
	shl	r5,1
	nilz	r13,r5,0xff
	c	r13,r15
	jeq	L000768
	bx	L000769
	ls	r5,0(r12)
L000768:
	dec	r12,1
	ls	r5,0(r12)
	lis	r13,1
L000769:
	sts	r13,0(r12)
L000770:
#	case 0x5 : SINGLE_STEP ;
	a	r12,r0
	cis	r2,0
	jhe	L000772
	ls	r5,0(r12)
	bx	L000775
	a	r2,r3
L000772:
	a	r2,r4
	nilz	r5,r13,0xff
	shl	r5,1
	nilz	r13,r5,0xff
	c	r13,r15
	jeq	L000774
	bx	L000775
	ls	r5,0(r12)
L000774:
	dec	r12,1
	ls	r5,0(r12)
	lis	r13,1
L000775:
	sts	r13,0(r12)
L000776:
#	case 0x4 : SINGLE_STEP ;
	a	r12,r0
	cis	r2,0
	jhe	L000778
	ls	r5,0(r12)
	bx	L000781
	a	r2,r3
L000778:
	a	r2,r4
	nilz	r5,r13,0xff
	shl	r5,1
	nilz	r13,r5,0xff
	c	r13,r15
	jeq	L000780
	bx	L000781
	ls	r5,0(r12)
L000780:
	dec	r12,1
	ls	r5,0(r12)
	lis	r13,1
L000781:
	sts	r13,0(r12)
L000782:
#	case 0x3 : SINGLE_STEP ;
	a	r12,r0
	cis	r2,0
	jhe	L000784
	ls	r5,0(r12)
	bx	L000787
	a	r2,r3
L000784:
	a	r2,r4
	nilz	r5,r13,0xff
	shl	r5,1
	nilz	r13,r5,0xff
	c	r13,r15
	jeq	L000786
	bx	L000787
	ls	r5,0(r12)
L000786:
	dec	r12,1
	ls	r5,0(r12)
	lis	r13,1
L000787:
	sts	r13,0(r12)
L000788:
#	case 0x2 : SINGLE_STEP ;
	a	r12,r0
	cis	r2,0
	jhe	L000790
	ls	r5,0(r12)
	bx	L000793
	a	r2,r3
L000790:
	a	r2,r4
	nilz	r5,r13,0xff
	shl	r5,1
	nilz	r13,r5,0xff
	c	r13,r15
	jeq	L000792
	bx	L000793
	ls	r5,0(r12)
L000792:
	dec	r12,1
	ls	r5,0(r12)
	lis	r13,1
L000793:
	sts	r13,0(r12)
L000794:
#	case 0x1 : SINGLE_STEP ;
	a	r12,r0
	cis	r2,0
	jhe	L000796
	ls	r5,0(r12)
	bx	L000799
	a	r2,r3
L000796:
	a	r2,r4
	nilz	r5,r13,0xff
	shl	r5,1
	nilz	r13,r5,0xff
	c	r13,r15
	jeq	L000798
	bx	L000799
	ls	r5,0(r12)
L000798:
	dec	r12,1
	ls	r5,0(r12)
	lis	r13,1
L000799:
	sts	r13,0(r12)
#	/* */
#	if ( --len )
	sis	r11,1
	bne	L00074
L0007106:
#		goto Loop_Top ;
#	else
#		return ;
#}
##undef SINGLE_STEP
#
#/*NOTREACHED*/
#}
	lm	r10,0(sp)
	brx	r15
	cal	sp,60(sp)
	.byte	0xdf,0x07,0xdf	#Trace table for fast_y_line_left
	.byte	0xa8	# First nonvolatile register saved is r10
	.byte	0x71	# npars=7 words
	.byte	0xf	# lcl_offset=60
