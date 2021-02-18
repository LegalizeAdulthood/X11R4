	.file	"cerror.o"
/ modified version of cerror
/
/ The idea is that every time an error occurs in a system call
/ I want a special function "syserr" called.  This function will
/ either print a message and exit or do nothing depending on
/ defaults and use of "onsyserr".
/
/
/ the non-underline cerror's are for compatability with the
/ pre-beta c library.  They can be removed when everyone is
/ updated.

/	Copyright (c) 1984 by Sun Microsystems, Inc.

.globl	cerror
.globl	_cerror
.globl	errno

.data
errno:	.long	0
.string  "@(#)cerror.s 1.2 89/03/02 SMI;"
.even
.text

cerror:
_cerror:
	movl	%eax,errno
	call	syserr		/ new code
	movl	$-1,%eax
	ret

.globl	_mycerror		/ clumsy way to get this loaded

_mycerror:
	.long	0
	ret

