	.file	"io2.s"
	.version	"01.01"
	.text
	.align	4
	.def	inb;	.val	inb;	.scl	2;	.type	054;	.endef
	.globl	inb
inb:
	pushl	%ebp
	movl	%esp,%ebp
	movl	8(%ebp),%edx
	subl	%eax,%eax
	inb	(%dx)
	leave
	ret

/ASMQ
/ASMEND0
	.def	inb;	.val	.;	.scl	-1;	.endef
	.align	4
	.def	outb;	.val	outb;	.scl	2;	.type	054;	.endef
	.globl	outb
outb:
	pushl	%ebp
	movl	%esp,%ebp
	movl	8(%ebp),%edx
	movl	12(%ebp),%eax
	outb	(%dx)
	leave
	ret

/ASMQ
/ASMEND1
	.def	outb;	.val	.;	.scl	-1;	.endef
	.align	4
	.def	inw;	.val	inw;	.scl	2;	.type	055;	.endef
	.globl	inw
inw:
	pushl	%ebp
	movl	%esp,%ebp
	movl	8(%ebp),%edx
	subl	%eax,%eax
	inw	(%dx)
	leave
	ret
/ASMQ
/ASMEND2
	.def	inw;	.val	.;	.scl	-1;	.endef
	.align	4
	.def	outw;	.val	outw;	.scl	2;	.type	055;	.endef
	.globl	outw
outw:
	pushl	%ebp
	movl	%esp,%ebp
	movl	8(%ebp),%edx
	movl	12(%ebp),%eax
	outw	(%dx)
	leave
	ret

/ASMQ
/ASMEND3
	.def	outw;	.val	.;	.scl	-1;	.endef
