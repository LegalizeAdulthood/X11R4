#
# modified version of cerror
#
# The idea is that every time an error occurs in a system call
# I want a special function "syserr" called.  This function will
# either print a message and exit or do nothing depending on
# defaults and use of "onsyserr".
#
|	Copyright (c) 1984 by Sun Microsystems, Inc.

.globl	cerror
.globl	_errno

.data
_errno:	.word	0
.asciz	"@(#)cerror.s 1.8 89/03/03 Copyr 1984 Sun Micro"
.even
.text

cerror:
	movl	d0,_errno
	jsr	_syserr		| new code
	moveq	#-1,d0
	rts

.globl	__mycerror		| clumsy way to get this loaded

__mycerror:
	.word	0
	rts
