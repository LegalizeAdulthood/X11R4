	.file	"vgaLine.386.s"
	.data
	.align	4
rcsid:	.string	"$Header: /andrew/X11/r3src/release/server/ddx/ibm/vga/RCS/vgaLine.386.s,v 1.8 89/02/27 21:40:08 paul Exp $"
	.align	4
//***********************************************************
/		Copyright IBM Corporation 1987, 1988
/
/                      All Rights Reserved
/
/Permission to use, copy, modify, and distribute this software and its
/documentation for any purpose and without fee is hereby granted,
/provided that the above copyright notice appear in all copies and that
/both that copyright notice and this permission notice appear in
/supporting documentation, and that the name of IBM not be
/used in advertising or publicity pertaining to distribution of the
/software without specific, written prior permission.
/
/IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
/ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
/IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
/ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
/WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
/ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
/SOFTWARE.
/
/******************************************************************/
/
//*
/ *  Hardware interface routines for IBM VGA adapter for
/ *  X.11 server(s) on IBM equipment.
/ *
/ *  Last Modified By
/ *  P. Shupak -- Oct., 1987
/ *  Assembly Code For the '386
/ *  P. Shupak -- Jan., 1989
/ *
/ */
	.text
	.align	4
/================== fast_x_line_right =====================/
	.globl	fast_x_line_right
fast_x_line_right:
/
//* X increasing */
/static void fast_x_line_right( et, e1, e2, len, y_increment, addr, x_offset )
/register int et ;
/const int e1 ;
/const int e2 ;
/unsigned int len ;
/const int y_increment ;
/register unsigned volatile char *addr ;
/const int x_offset ;
/{
/register unsigned char tmp ;			/* %cl */
/register unsigned char mask ;			/* %bh */
/register unsigned single_bit ;			/* %bl */
/
/#define SINGLE_STEP \
/if ( et < 0 ) { \
/	mask |= single_bit ; \
/	et += e1 ; \
/} \
/else { \
/	tmp = *( (VgaMemoryPtr) addr ) ; \
/	et += e2 ; \
/	*( (VgaMemoryPtr) addr ) = mask ; \
/	mask = single_bit ; \
/	addr += y_increment ; \
/}

//* Set-Up And Set Initial Mask Value */
	xchgl	24(%esp), %esi
/mask = single_bit = SCRRIGHT8( LeftmostBit, tmp = x_offset ) ;
	xchgl	28(%esp), %ebx
	movzbl	%bl, %ecx	/* The upper 3 bytes of %ecx are now clear !! */
	movb	$0x80, %al
	shrb	%cl, %al
	movb	%al, %ah
/tmp = ( tmp + 1 ) & 7 ;
	incb	%cl
	andb	$0x7, %cl

/if ( !( single_bit = SCRRIGHT8( single_bit, 1 ) ) ) {
	shrb	$1, %ah
	jnz	.adjustIndexRight
/	tmp = *( (VgaMemoryPtr) addr ) ;
/	*( (VgaMemoryPtr) addr++ ) = mask ;
	xchgb	(%esi), %al
	incl	%esi
/	single_bit = LeftmostBit ;
/	tmp = mask = 0 ;
	movw	$0x8000, %ax
	subl	%ecx, %ecx

/}
/if ( ( 8 - tmp ) > --len ) {
.adjustIndexRight:
	movl	$0x8, %edx
	subl	%ecx, %edx
	movl	16(%esp), %ebx
	decl	%ebx
	cmpl	%ebx, %edx
	jbe	.multiLoopXRight
/	tmp = len ;
	movl	%ebx, %ecx
/	len = 8 ;
	movl	$0x8, 16(%esp)
	jmp	.pre1046Right
	.align	4
/}
/else if ( tmp ) {
.multiLoopXRight:
	andb	%cl, %cl
	jz	.prepre1046Right
/	len += tmp ;	/* Round-Up */
	addl	%ecx, %ebx
/	tmp = 8 - tmp ;
	subb	$0x8, %cl
	negb	%cl
.prepre1046Right:
	movl	%ebx, 16(%esp)	/* Store back Length value */
.pre1046Right:
/* From Here On %edx holds "y_increment" !! */
	movl	20(%esp), %edx
/* From Here On %edi holds "e2" !! */
	xchgl	12(%esp), %edi
/* From Here On %ebp holds "e1" !! */
	xchgl	8(%esp), %ebp
/* From Here On %ebx holds the bresenham error term, "et" !! */
	movl	4(%esp), %ebx
/* Now Test the error term then, jump into the Loop at the correct spot */
	andl	%ebx, %ebx
	jmp	*.xOddLotTableRight(, %ecx, 4)
	.align	4
/}

/branchPoint:
/switch ( tmp ) {
/	Loop_Top:
/	/* First -- Leftmost -- Most-Significant Bit In Byte */
/	case 0 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
.Case_X_00_Right:
	movw	$0x4080, %ax
	jns	.XRFlush00
.XRNoFlush00:
	addl	%ebp, %ebx
	js	.XRNoFlush07
	jmp	.XRFlush07
	.align	4
.XRFlush00: /* Never need to flush here ! We must have just written the byte */
	addl	%edx, %esi
	addl	%edi, %ebx
.Case_X_07_Right:
	jns	.XRFlush07
/	case 7 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
.XRNoFlush07:
#define NO_FLUSH_STEP \
	orb	%ah, %al ; \
	shrb	$1, %ah ; \
	addl	%ebp, %ebx ;
	NO_FLUSH_STEP ;
	js	.XRNoFlush06
	jmp	.XRFlush06
	.align	4
.XRFlush07:
#define FLUSH_STEP \
	xchgb	(%esi), %al ; \
	movb	%ah, %al ; \
	shrb	$1, %ah ; \
	addl	%edx, %esi ; \
	addl	%edi, %ebx
	FLUSH_STEP ;
.Case_X_06_Right:
	jns	.XRFlush06
/	case 6 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
.XRNoFlush06:
	NO_FLUSH_STEP ;
	js	.XRNoFlush05
	jmp	.XRFlush05
	.align	4
.XRFlush06:
	FLUSH_STEP ;
.Case_X_05_Right:
	jns	.XRFlush05
/	case 5 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
.XRNoFlush05:
	NO_FLUSH_STEP ;
	js	.XRNoFlush04
	jmp	.XRFlush04
	.align	4
.XRFlush05:
	FLUSH_STEP ;
.Case_X_04_Right:
	jns	.XRFlush04
/	case 4 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
.XRNoFlush04:
	NO_FLUSH_STEP ;
	js	.XRNoFlush03
	jmp	.XRFlush03
	.align	4
.XRFlush04:
	FLUSH_STEP ;
.Case_X_03_Right:
	jns	.XRFlush03
/	case 3 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
.XRNoFlush03:
	NO_FLUSH_STEP ;
	js	.XRNoFlush02
	jmp	.XRFlush02
	.align	4
.XRFlush03:
	FLUSH_STEP ;
.Case_X_02_Right:
	jns	.XRFlush02
/	case 2 : SINGLE_STEP ; single_bit = SCRRIGHT8( single_bit, 1 ) ;
.XRNoFlush02:
	NO_FLUSH_STEP ;
	js	.XRNoFlush01
	jmp	.XRFlush01
	.align	4
.XRFlush02:
	FLUSH_STEP ;
.Case_X_01_Right:
/	/* Last -- Rightmost -- Most-Significant Bit In Byte */
/	case 1 : SINGLE_STEP ;
	jns	.XRFlush01
.XRNoFlush01:
	orb	%ah, %al
	addl	%ebp, %ebx
	jmp	.XR_LoopEnd
	.align	4
.XRFlush01:
	xchgb	(%esi), %al
	movb	%ah, %al
	addl	%edx, %esi
	addl	%edi, %ebx
.XR_LoopEnd:
/	/* Flush buffered mask */
/	tmp = *( (VgaMemoryPtr) addr ) ;
/	*( (VgaMemoryPtr) addr ) = mask ;
	xchgb	(%esi), %al
/	len -= 8 ;
	movl	16(%esp), %ecx
	subl	$0x8, %ecx
	je	.XRdone
/		addr++ ;
	incl	%esi
/	if ( len > 7 ) {
	cmpl	$0x7, %ecx
	jbe	.XR_LastLoop
	movl	%ecx, 16(%esp)
/		goto Loop_Top ;
	movw	$0x4080, %ax
	andl	%ebx, %ebx
	jns	.XRFlush00
	addl	%ebp, %ebx
	js	.XRNoFlush07
	jmp	.XRFlush07
	.align	4
/	}
/	else if ( len ) {
.XR_LastLoop:
/		mask = 0 ;
/		single_bit = LeftmostBit ;
	movw	$0x8000, %ax
/		tmp = len ;
/		len = 8 ;
	movl	$0x8, 16(%esp)
/		goto branchPoint ;
	andl	%ebx, %ebx
	jmp	*.xOddLotTableRight(, %ecx, 4)
	.align	4
/	}
/	else
/		return ;
.XRdone:
	movl	28(%esp), %ebx
	movl	24(%esp), %esi
	movl	12(%esp), %edi
	movl	 8(%esp), %ebp
	ret
/}
/#undef SINGLE_STEP
/}
#undef NO_FLUSH_STEP
#undef FLUSH_STEP
	.align	4

/================== fast_x_line_left =====================/
	.globl	fast_x_line_left
fast_x_line_left:
/
//* X decreasing */
/static void fast_x_line_left( et, e1, e2, len, y_increment, addr, x_offset )
/register int et ;
/const int e1 ;
/const int e2 ;
/unsigned int len ;
/const int y_increment ;
/register unsigned volatile char *addr ;
/const int x_offset ;
/{
/register unsigned char tmp ;
/register unsigned char mask ;
/register unsigned single_bit ;
/
/#define SINGLE_STEP \
/if ( et < 0 ) { \
/	mask |= single_bit ; \
/	et += e1 ; \
/} \
/else { \
/	tmp = *( (VgaMemoryPtr) addr ) ; \
/	et += e2 ; \
/	*( (VgaMemoryPtr) addr ) = mask ; \
/	mask = single_bit ; \
/	addr += y_increment ; \
/}
/
/* Set-Up And Set Initial Mask Value */
	xchgl	24(%esp), %esi
/mask = single_bit = SCRLEFT8( RightmostBit, tmp = 7 - xoffset ) ;
	xchgl	28(%esp), %ebx
	movl	$0x7, %ecx	/* The upper 3 bytes of %ecx are now clear !! */
	subb	%bl, %cl
	movb	$1, %al
	shlb	%cl, %al
/tmp = ( tmp + 1 ) & 7 ;
	incb	%cl
	andb	$0x7, %cl
/if ( ( single_bit = SCRLEFT8( single_bit, 1 ) )
/			== SCRLEFT8( RightmostBit, 8 ) ) {
	movb	%al, %ah
	addb	%al, %ah
	jnc	.XL_checkLength
/	tmp = *( (VgaMemoryPtr) addr ) ;
/	*( (VgaMemoryPtr) addr-- ) = mask ;
	xchgb	(%esi), %al
	decl	%esi
/	single_bit = RightmostBit ;
/	tmp = mask = 0 ;
	movw	$0x100, %ax
	subl	%ecx, %ecx
/}
/if ( ( 8 - tmp ) > --len ) {
.XL_checkLength:
	movl	$0x8, %edx
	subl	%ecx, %edx
	movl	16(%esp), %ebx
	decl	%ebx
	cmpl	%ebx, %edx
	jbe	.multiLoopFastXLeft
/	tmp = len ;
	movl	%ebx, %ecx
/	len = 8 ;
	movl	$0x8, 16(%esp)
	jmp	.pre1046Left
	.align	4
/}
/else if ( tmp ) {
.multiLoopFastXLeft:
	andb	%cl, %cl
	je	.prepre1046Left
/	len += tmp ;	/* Round-Up */
	addl	%ecx, %ebx
/	tmp = 8 - tmp ;
	subb	$0x8, %cl
	negb	%cl
.prepre1046Left:
	movl	%ebx, 16(%esp)
.pre1046Left:
/* From Here On %edx holds "y_increment" !! */
	movl	20(%esp), %edx
/* From Here On %edi holds "e2" !! */
	xchgl	12(%esp), %edi
/* From Here On %ebp holds "e1" !! */
	xchgl	8(%esp), %ebp
/* From Here On %ebx holds the bresenham error term, "et" !! */
	movl	4(%esp), %ebx
/* Now Test the error term then, jump into the Loop at the correct spot */
	andl	%ebx, %ebx
	jmp	*.xOddLotTableLeft(, %ecx, 4)
	.align	4
/}
/branchPoint:
/switch ( tmp ) {
/	Loop_Top:
/	/* First -- Rightmost -- Least-Significant Bit In Byte */
/	case 0 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
.Case_X_00_Left:
	movw	$0x0201, %ax
	jns	.XLFlush00
.XLNoFlush00:
	addl	%ebp, %ebx
	js	.XLNoFlush07
	jmp	.XLFlush07
	.align	4
.XLFlush00: /* Never need to flush here ! We must have just written the byte */
	addl	%edx, %esi
	addl	%edi, %ebx
.Case_X_07_Left:
	jns	.XLFlush07
/	case 7 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
.XLNoFlush07:
#define NO_FLUSH_STEP \
	orb	%ah, %al ; \
	addb	%ah, %ah ;	/* This is really a shift by one */ \
	addl	%ebp, %ebx
	NO_FLUSH_STEP ;
	js	.XLNoFlush06
	jmp	.XLFlush06
	.align	4
.XLFlush07:
#define FLUSH_STEP \
	xchgb	(%esi), %al ; \
	movb	%ah, %al ; \
	addb	%al, %ah ;	/* This is really a shift by one */ \
	addl	%edx, %esi ; \
	addl	%edi, %ebx
	FLUSH_STEP ;
.Case_X_06_Left:
	jns	.XLFlush06
/	case 6 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
.XLNoFlush06:
	NO_FLUSH_STEP ;
	js	.XLNoFlush05
	jmp	.XLFlush05
	.align	4
.XLFlush06:
	FLUSH_STEP ;
.Case_X_05_Left:
	jns	.XLFlush05
/	case 5 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
.XLNoFlush05:
	NO_FLUSH_STEP ;
	js	.XLNoFlush04
	jmp	.XLFlush04
	.align	4
.XLFlush05:
	FLUSH_STEP ;
.Case_X_04_Left:
	jns	.XLFlush04
/	case 4 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
.XLNoFlush04:
	NO_FLUSH_STEP ;
	js	.XLNoFlush03
	jmp	.XLFlush03
	.align	4
.XLFlush04:
	FLUSH_STEP ;
.Case_X_03_Left:
	jns	.XLFlush03
/	case 3 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
.XLNoFlush03:
	NO_FLUSH_STEP ;
	js	.XLNoFlush02
	jmp	.XLFlush02
	.align	4
.XLFlush03:
	FLUSH_STEP ;
.Case_X_02_Left:
	jns	.XLFlush02
/	case 2 : SINGLE_STEP ; single_bit = SCRLEFT8( single_bit, 1 ) ;
.XLNoFlush02:
	NO_FLUSH_STEP ;
	js	.XLNoFlush01
	jmp	.XLFlush01
	.align	4
.XLFlush02:
	FLUSH_STEP ;
.Case_X_01_Left:
/	/* Last -- Leftmost -- Most-Significant Bit In Byte */
/	case 1 : SINGLE_STEP ;
	jns	.XLFlush01
.XLNoFlush01:
	orb	%ah, %al
	addl	%ebp, %ebx
	jmp	.XL_LoopEnd
	.align	4
.XLFlush01:
	FLUSH_STEP ;
.XL_LoopEnd:
/	/* Flush buffered mask */
/	tmp = *( (VgaMemoryPtr) addr ) ;
/	len -= 8 ;
/	*( (VgaMemoryPtr) addr ) = mask ;
	xchgb	(%esi), %al
	movl	16(%esp), %ecx
	subl	$0x8, %ecx
	je	.XLdone
/		addr-- ;
	decl	%esi
/	if ( len > 7 ) {
	cmpl	$0x7, %ecx
	jbe	.XL_LastLoop
	movl	%ecx, 16(%esp)
	movw	$0x0201, %ax
	andl	%ebx, %ebx
/		goto Loop_Top ;
/	}
	jns	.XLFlush00
	addl	%ebp, %ebx
	js	.XLNoFlush07
	jmp	.XLFlush07
	.align	4
/	else if ( len ) {
.XL_LastLoop:
/		mask = 0 ;
/		single_bit = RightmostBit ;
	movw	$0x100, %ax
/		tmp = len ;
/		len = 8 ;
	movl	$0x8, 16(%esp)
/		goto branchPoint ;
	andl	%ebx, %ebx
	jmp	*.xOddLotTableLeft(, %ecx, 4)
	.align	4
/	}
/	else
/		return ;
.XLdone:
	movl	28(%esp), %ebx
	movl	24(%esp), %esi
	movl	12(%esp), %edi
	movl	 8(%esp), %ebp
	ret
/}
#undef NO_FLUSH_STEP
#undef FLUSH_STEP
/}
	.align	4
/================== fast_y_line_left =====================/
	.globl		fast_y_line_left
fast_y_line_left:
/
//* X increasing */
/void fast_y_line_left( et, e1, e2, len, y_increment, addr, x_offset )
/register int et ;
/const int e1 ;					$0x08(%esp)
/						%ebx == e1 - e2
/const int e2 ;					%esi
/unsigned int len ;				upper half of %ecx
/const int y_increment ;			$0x14(%esp)
/register unsigned volatile char *addr ;	%edi
/const int x_offset ;
/{
/register unsigned single_bit ;			%ch
/register unsigned char tmp ;			%cl
/
/#define SINGLE_STEP \
/addr += y_increment ; \
/if ( et < 0 ) { \
/	tmp = *( (VgaMemoryPtr) addr ) ; \
/	et += e1 ; \
/} \
/else { \
/	et += e2 ; \
/	if ( single_bit = SCRLEFT8( single_bit, 1 ) ) { \
/		tmp = *( (VgaMemoryPtr) addr ) ; \
/	} \
/	else { \
/		tmp = *( (VgaMemoryPtr) ( --addr ) ) ; \
/		single_bit = RightmostBit ; \
/	} \
/} \
/* ( (VgaMemoryPtr) addr ) = single_bit */
/* END MACRO */
/
/* Draw First Dot */
/tmp = *( (VgaMemoryPtr) addr ) ;
	xchgl	0x18(%esp), %edi
	movb	(%edi), %al	/* %al is temporary scratch */
/* ( (VgaMemoryPtr) addr ) =
/	( single_bit = SCRRIGHT8( LeftmostBit, x_offset ) ) ; */
	movl	0x1C(%esp), %ecx
	movb	$0x80, %ch
	rorb	%cl, %ch
	movb	%ch, (%edi)

/tmp = --len & 0xF ; /* Already Drew First Dot */
	xchgl	0x10(%esp), %ebx
	decl	%ebx
	movzbl	%bl, %edx	/* Copy to %edx for initial jump in */
	andb	$0xF, %dl	/* Partial loop index */
/*	Put the length (mod 16) into the upper 16 bits of %ecx	*/
/len += 0xF ;
	addl	$0x0F, %ebx	/* Round-Up by adding 15 */
/len >>= 0x4 ;
	shrl	$0x04, %ebx	/* Divide by 16 */
/* ****** pre-decrement count by 16 - simpler test later */
	decl	%ebx
	shll	$0x10, %ebx	/* Up-Shift by 16 bits */
	orl	%ebx, %ecx	/* Move into top of %ecx */

/*	Set up the bressenham error and factors		*/
	movl	0x04(%esp), %eax
	movl	0x08(%esp), %ebx
	xchgl	0x0C(%esp), %esi
	subl	%esi, %ebx

/*	Get "y_increment" into a register	*/
	xchgl	0x14(%esp), %ebp
	negl	%ebp		/* We're going to subtract not add */

/switch ( tmp ) {
	jmp	*.yOddLotTableLeft(, %edx, 4)
	.align	4

/* Loop_Top: */
/	case 0x0 : SINGLE_STEP ;
.Case_Y_00_Left:
	cltd			/* Sign extend %eax into %edx */
	movb	%dl, %cl	/* Now %cl is either -1 or 0 */
	incb	%cl		/* Now %cl is either 0 or +1 */
	andl	%ebx, %edx	/* NOTE: clears carry flag !! */
/* NOTE: The rotate causes the wrapped bit to be "Xor"'ed with the carry flag */
	rolb	%cl, %ch	/* Conditional Shift */
/* If the carry flag is set we move one byte to the left !! */
	sbbl	%ebp, %edi	/* Next screen row - Conditional Shift */
	movb	(%edi), %cl	/* Read from video memory -- Prime the pump */
/* Calculate The Bressenham Error Term Sum */
	addl	%edx, %eax	/* The conditional term ( e1 - e2 ) */
	addl	%esi, %eax	/* The unconditional term ( e2 ) */
/* Now Change The Screen */
	movb	%ch, (%edi)	/* Write the single_bit value to video memory */
.Case_Y_15_Left:
#define SINGLE_STEP \
	cltd			; \
	movb	%dl, %cl	; \
	incb	%cl		; \
	andl	%ebx, %edx	; \
	rolb	%cl, %ch	; \
	sbbl	%ebp, %edi	; \
	movb	(%edi), %cl	; \
	addl	%edx, %eax	; \
	addl	%esi, %eax	; \
	movb	%ch, (%edi)
	SINGLE_STEP ;
.Case_Y_14_Left:
	SINGLE_STEP ;
.Case_Y_13_Left:
	SINGLE_STEP ;
.Case_Y_12_Left:
	SINGLE_STEP ;
.Case_Y_11_Left:
	SINGLE_STEP ;
.Case_Y_10_Left:
	SINGLE_STEP ;
.Case_Y_09_Left:
	SINGLE_STEP ;
.Case_Y_08_Left:
	SINGLE_STEP ;
.Case_Y_07_Left:
	SINGLE_STEP ;
.Case_Y_06_Left:
	SINGLE_STEP ;
.Case_Y_05_Left:
	SINGLE_STEP ;
.Case_Y_04_Left:
	SINGLE_STEP ;
.Case_Y_03_Left:
	SINGLE_STEP ;
.Case_Y_02_Left:
	SINGLE_STEP ;
.Case_Y_01_Left:
	SINGLE_STEP ;
/	if ( --len )
/		goto Loop_Top ;
	subl	$0x10000, %ecx
	jnc	.Case_Y_00_Left
/	else
/		return ;
	movl	0x0C(%esp), %esi
	movl	0x10(%esp), %ebx
	movl	0x14(%esp), %ebp
	movl	0x18(%esp), %edi
	ret
	.align	4
/}
#undef SINGLE_STEP
/}

/================== fast_y_line_right =====================/
	.globl		fast_y_line_right
fast_y_line_right:
/
//* X decreasing */
/static void fast_y_line_right( et, e1, e2, len, y_increment, addr, x_offset )
/register int et ;
/const int e1 ;
/const int e2 ;
/unsigned int len ;
/const int y_increment ;
/register unsigned volatile char *addr ;
/const int x_offset ;
/{
/register unsigned single_bit ;
/#define stop_bit SCRLEFT8( RightmostBit, 8 )
/register unsigned char tmp ;
/
/#define SINGLE_STEP \
/addr += y_increment ; \
/if ( et < 0 ) { \
/	tmp = *( (VgaMemoryPtr) addr ) ; \
/	et += e1 ; \
/} \
/else { \
/	et += e2 ; \
/	if ( ( single_bit = SCRRIGHT8( single_bit, 1 ) ) != stop_bit ) { \
/		tmp = *( (VgaMemoryPtr) addr ) ; \
/	} \
/	else { \
/		tmp = *( (VgaMemoryPtr) ( ++addr ) ) ; \
/		single_bit = LeftmostBit ; \
/	} \
/} \
/* ( (VgaMemoryPtr) addr ) = single_bit */
//* END MACRO */
/
//* Draw First Dot */
/tmp = *( (VgaMemoryPtr) addr ) ;
	xchgl	0x18(%esp), %edi
	movb	(%edi), %al	/* %al is temporary scratch */
/*( (VgaMemoryPtr) addr ) = */
/*	( single_bit = SCRRIGHT8( LeftmostBit, x_offset ) ) ; */
	movl	0x1C(%esp), %ecx
	movb	$0x80, %ch
	rorb	%cl, %ch
	movb	%ch, (%edi)

/tmp = --len & 0xF ; /* Already Drew First Dot */
	xchgl	0x10(%esp), %ebx
	decl	%ebx
	movzbl	%bl, %edx	/* Copy to %edx for initial jump in */
	andb	$0xF, %dl	/* Partial loop index */
/*	Put the length (mod 16) into the upper 16 bits of %ecx	*/
/len += 0xF ;
	addl	$0xF, %ebx	/* Round-Up by adding 15 */
/len >>= 0x4 ;
	shrl	$0x04, %ebx	/* Divide by 16 */
/* ****** pre-decrement count by 16 - simpler test later */
	decl	%ebx
	shll	$0x10, %ebx	/* Up-Shift by 16 bits */
	orl	%ebx, %ecx	/* Move into top of %ecx */

/*	Set up the bressenham error and factors		*/
	movl	0x04(%esp), %eax
	movl	0x08(%esp), %ebx
	xchgl	0x0C(%esp), %esi
	subl	%esi, %ebx

/*	Get "y_increment" into a register	*/
	xchgl	20(%esp),%ebp

/switch ( tmp ) {
	jmp	*.yOddLotTableRight(, %edx, 4)
	.align 4

/* Loop_Top: */
/	case 0x0 : SINGLE_STEP ;
.Case_Y_00_Right:
	cltd			/* Sign extend %eax into %edx */
	movb	%dl, %cl	/* Now %cl is either -1 or 0 */
	incb	%cl		/* Now %cl is either 0 or +1 */
	andl	%ebx, %edx	/* NOTE: clears carry flag !! */
/* NOTE: The rotate causes the wrapped bit to be "Xor"'ed with the carry flag */
	rorb	%cl, %ch	/* Conditional Shift */
/* If the carry flag is set we move one byte to the right !! */
	adcl	%ebp, %edi	/* Next screen row - Conditional Shift */
	movb	(%edi), %cl	/* Read from video memory -- Prime the pump */
/* Calculate The Bressenham Error Term Sum */
	addl	%edx, %eax	/* The conditional term ( e1 - e2 ) */
	addl	%esi, %eax	/* The unconditional term ( e2 ) */
/* Now Change The Screen */
	movb	%ch, (%edi)	/* Write the single_bit value to video memory */
.Case_Y_15_Right:
#define SINGLE_STEP \
	cltd			; \
	movb	%dl, %cl	; \
	incb	%cl		; \
	andl	%ebx, %edx	; \
	rorb	%cl, %ch	; \
	adcl	%ebp, %edi	; \
	movb	(%edi), %cl	; \
	addl	%edx, %eax	; \
	addl	%esi, %eax	; \
	movb	%ch, (%edi)
	SINGLE_STEP ;
.Case_Y_14_Right:
	SINGLE_STEP ;
.Case_Y_13_Right:
	SINGLE_STEP ;
.Case_Y_12_Right:
	SINGLE_STEP ;
.Case_Y_11_Right:
	SINGLE_STEP ;
.Case_Y_10_Right:
	SINGLE_STEP ;
.Case_Y_09_Right:
	SINGLE_STEP ;
.Case_Y_08_Right:
	SINGLE_STEP ;
.Case_Y_07_Right:
	SINGLE_STEP ;
.Case_Y_06_Right:
	SINGLE_STEP ;
.Case_Y_05_Right:
	SINGLE_STEP ;
.Case_Y_04_Right:
	SINGLE_STEP ;
.Case_Y_03_Right:
	SINGLE_STEP ;
.Case_Y_02_Right:
	SINGLE_STEP ;
.Case_Y_01_Right:
	SINGLE_STEP ;
/	if ( --len )
/		goto Loop_Top ;
	subl	$0x10000, %ecx
	jnc	.Case_Y_00_Right
/	else
/		return ;
	movl	0x18(%esp), %edi
	movl	0x14(%esp), %ebp
	movl	0x10(%esp), %ebx
	movl	0x0C(%esp), %esi
	ret
/}
#undef SINGLE_STEP
/}
	.align	4
.yOddLotTableLeft:
	.long	.Case_Y_00_Left
	.long	.Case_Y_01_Left
	.long	.Case_Y_02_Left
	.long	.Case_Y_03_Left
	.long	.Case_Y_04_Left
	.long	.Case_Y_05_Left
	.long	.Case_Y_06_Left
	.long	.Case_Y_07_Left
	.long	.Case_Y_08_Left
	.long	.Case_Y_09_Left
	.long	.Case_Y_10_Left
	.long	.Case_Y_11_Left
	.long	.Case_Y_12_Left
	.long	.Case_Y_13_Left
	.long	.Case_Y_14_Left
	.long	.Case_Y_15_Left

	.align	4
.yOddLotTableRight:
	.long	.Case_Y_00_Right
	.long	.Case_Y_01_Right
	.long	.Case_Y_02_Right
	.long	.Case_Y_03_Right
	.long	.Case_Y_04_Right
	.long	.Case_Y_05_Right
	.long	.Case_Y_06_Right
	.long	.Case_Y_07_Right
	.long	.Case_Y_08_Right
	.long	.Case_Y_09_Right
	.long	.Case_Y_10_Right
	.long	.Case_Y_11_Right
	.long	.Case_Y_12_Right
	.long	.Case_Y_13_Right
	.long	.Case_Y_14_Right
	.long	.Case_Y_15_Right

	.align	4
.xOddLotTableRight:
	.long	.Case_X_00_Right
	.long	.Case_X_01_Right
	.long	.Case_X_02_Right
	.long	.Case_X_03_Right
	.long	.Case_X_04_Right
	.long	.Case_X_05_Right
	.long	.Case_X_06_Right
	.long	.Case_X_07_Right

	.align	4
.xOddLotTableLeft:
	.long	.Case_X_00_Left
	.long	.Case_X_01_Left
	.long	.Case_X_02_Left
	.long	.Case_X_03_Left
	.long	.Case_X_04_Left
	.long	.Case_X_05_Left
	.long	.Case_X_06_Left
	.long	.Case_X_07_Left
