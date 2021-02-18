/* The code that follows was written for a MIPS R2000 or R3000 series
 * processor.  The code was written by Zalman Stern from the description in
 * the "MIPS R2000 RISC Architecture" by Gerry Kane
 */

/* Might be better to let the assembler fill the delay slots below, but I did
 * it myself. In any event, the code should still work even if the next line
 * is removed.
 */
	.set noreorder
	.data
	.globl	class_ErrorReturnValue
	.text
	.globl	class_Lookup

#include <regdef.h>

#ifdef __STDC__
#define ClassEntry(n) \
	.ent	ClassEntry ## n\
	.globl	ClassEntry ## n\
_ClassEntry ## n:\
	b	ClassEntry\
	  li	t0, n\
	.end		ClassEntry ## n
#else
#define ClassEntry(n) \
	.ent	ClassEntry/**/n;\
	.globl	ClassEntry/**/n;\
ClassEntry/**/n:\
	b	MainClassEntry;\
	  li	t0, n;\
	.end	ClassEntry/**/n
#endif

#include <../common/entrydefs.h>

	.ent	MainClassEntry
MainClassEntry:
#define framesize 44
	subu	sp, framesize
	.frame	sp, framesize, $31
/* Not sure if I should include the argument registers in the save mask.
 * That is how it is done now though.
 */
	.mask 0x800100e0, 4
	sw	ra, framesize - 4(sp) /* register 31 */
/* We need one temporary which spans the call to class_Lookup to hold the
 * classprocedure index. Register s0 is allocated for this and must be
 * saved.
 */
	sw	s0, framesize - 8(sp) /* register 16 */
/* Save the argument and floating point argument registers.
 * It might not be necessary to save the fp arg registers.
 * No need to save a0 since it is passed in soley for the use of this routine.
 * We just pass it through to class_Lookup so it works out fine.
 * All other argument registers need to be saved though.
 */
	sw	a3, framesize - 12(sp) /* register 7 */
	sw	a2, framesize - 16(sp) /* register 6 */
	sw	a1, framesize - 20(sp) /* register 5 */
/* I hope storing a double works even if a single precision arg was passed.
 */
	.fmask	0x00005000, 28
	s.d	$f14, framesize - 28(sp) /* register f14 */
	s.d	$f12, framesize - 36(sp) /* register f12 */
/* 2 words of save area in here for arguments to class_Lookup.
 * Finally save the return address.
 * Multiply index by four to get offset into classprocedure table.
 */
	sll	s0, t0, 2
/* Call class_Lookup, setting up second argument in the delay slot.
 */
	jal	class_Lookup
	  addi	a1, s0, 0
/* Restore what state we can now.
 */
	lw	a1, framesize - 20(sp)
	lw	a2, framesize - 16(sp)
	lw	a3, framesize - 12(sp)
	l.d	$f12, framesize - 36(sp)
	l.d	$f14, framesize - 28(sp)
/* Test for an error return from class_Lookup.
 */
	beq	v0, $0, error
	  lw	ra, framesize - 4(sp) /* Restore return address. */
	add	t0, s0, v0
	lw	t0, (t0)
	lw	s0, framesize - 8(sp)
	j	t0
	  add	sp, sp, framesize
error:
/* This next instruction assumes the assmebler is inteligent and will convert
 * the lw into the following:
 *	lui	v0, upper_16_bits_sign_adjusted(_class_ErrorReturnValue)
 *	lw	v0, v0, lower_16_bits(_class_ErrorReturnValue)
 */
	lw	v0, class_ErrorReturnValue
	lw	s0, framesize - 8(sp)
	j	ra
	  add	sp, sp, framesize
	.end MainClassEntry
