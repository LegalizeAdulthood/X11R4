#include	"OSas.h"
 /* waits for fifo to be "NOT_FULL" -- expects PSR in R4 and clobbers */
 /* R0.  needs a label which is passed in.			      */
#define	WAITFIFO1(n)	n : LS R0,0(R4) ; NILZ R0,R0,0x2000 ; JZ n

OSHEADER

 /*------------------------------------------------------------------------*/
 /* mfCmd0Args(cmd) -- writes byte count (always 4) and then 'cmd' to fifo as */
 /*		  soon as room is available.*/
 /*------------------------------------------------------------------------*/
	.text
	.globl TEXT_ENTRY(mfCmd0Args)
	.align 1
TEXT_ENTRY(mfCmd0Args) :
	GET(R4,0xf000094a)	/* get address of PSR into R4*/
				/* get address of FIFO into 4(R4)*/

	CAL	R3,4(R0)	/* load byte count*/
	WAITFIFO1( cmdbcnt )	/* wait for fifo to come ready*/
	STHS	R3,4(R4)	/* write byte count to FIFO*/

	WAITFIFO1( cmdcmd )	/* wait for fifo to come ready*/

	BRX	R15		/* go home while...*/
	STHS	R2,4(R4)	/* writing 'cmd' to FIFO*/
	.align 2; .byte 0xdf, 2, 0xdf, 0;	/* trace table*/
	.data
	.globl DATA_ENTRY(mfCmd0Args)
DATA_ENTRY(mfCmd0Args) :
	.long	TEXT_ENTRY(mfCmd0Args)
 /*------------------------------------------------------------------------*/
 /* mfCmd1Arg(cmd,arg) --writes [byte count (always 6),cmd,a] to fifo as */
 /*			room is available. */
 /*------------------------------------------------------------------------*/
	.text
	.globl TEXT_ENTRY(mfCmd1Arg)
	.align 1
TEXT_ENTRY(mfCmd1Arg) :
	GET(R4,0xf000094a)	/* get address of PSR into R4*/
				/* get address of FIFO into 4(R4)*/

	WAITFIFO1( cmd1bcnt )	/* wait for fifo to come ready*/
	CAL	R0,6(R0)	/* load byte count*/
	STHS	R0,4(R4)	/* write byte count to FIFO*/

	WAITFIFO1( cmd1cmd )	/* wait for fifo to come ready*/
	STHS	R2,4(R4)	/* write command to FIFO*/

	WAITFIFO1( cmd1arg )	/* wait for fifo to come ready*/

	BRX	R15		/* go home while...*/
	STHS	R3,4(R4)	/* ...writing arg to FIFO*/
	.align 2; .byte 0xdf, 2, 0xdf, 0;	/* trace table*/
	.data
	.globl DATA_ENTRY(mfCmd1Arg)
DATA_ENTRY(mfCmd1Arg) :
	.long	TEXT_ENTRY(mfCmd1Arg)
 /*------------------------------------------------------------------------*/
 /* mfCmd2Args(cmd,a,b) -- writes [byte count (always 8),cmd,arg1,arg2] to*/
 /*			fifo as room is available. */
 /*------------------------------------------------------------------------*/
	.text
	.globl TEXT_ENTRY(mfCmd2Args)
	.align 1
TEXT_ENTRY(mfCmd2Args) :
	GET(R5,0xf000094a)	/* get address of PSR into R5*/
				/* get address of FIFO into 4(R5)*/

	WAITFIFO1( cmd2bcnt )	/* wait for fifo to come ready*/
	CAL	R0,8(R0)	/* load byte count*/
	STHS	R0,4(R5)	/* write byte count to FIFO*/

	WAITFIFO1( cmd2cmd )	/* wait for fifo to come ready*/
	STHS	R2,4(R5)	/* write command to FIFO*/

	WAITFIFO1( cmd2arg1 )	/* wait for fifo to come ready*/
	STHS	R3,4(R5)	/* write first arg to FIFO*/

	WAITFIFO1( cmd2arg2 )	/* wait for fifo to come ready*/

	BRX	R15		/* go home while...*/
	STHS	R4,4(R5)	/* ...writing second arg to FIFO*/
	.align 2; .byte 0xdf, 2, 0xdf, 0;	/* trace table*/
	.data
	.globl DATA_ENTRY(mfCmd2Args)
DATA_ENTRY(mfCmd2Args) :
	.long	TEXT_ENTRY(mfCmd2Args)

 /*------------------------------------------------------------------------*/
 /* mfNOut(pData,n,&PSR,&FIFO) -- Writes n half-words starting at pData */
 /*		to fifo as soon as room is available.  &PSR should be */
 /*		address of PSR, &FIFO address of FIFO. */
 /*------------------------------------------------------------------------*/
	.text
	.align 1
mfNOut:
	LHS	R0,0(R4)	/* get PSR into R0*/
	NILZ	R0,R0,0x6000	/* AND with FIFO_NOT_HALF_FULL|FIFO_NOT_FULL*/
	JZ	mfNOut		/* fifo is full -- spin.*/
	NILZ	R0,R0,0x4000	/* AND with FIFO_NOT_HALF_FULL*/
	JNZ	writemany	/* less than half full, write lots of words*/
writeone:			/* more than half full, write one word*/
	SIS	R3,2		/* decrement counter*/
	BLR	R15		/* go home if less than 0*/
	LHS	R0,0(R2)	/* load data to be written*/
	INC	R2,2		/* increment pointer*/
	BX	mfNOut		/* branch to check PSR while...*/
	STHS	R0,4(R4)	/* ...writing data to fifo*/
writemany:
	SIS	R3,2		/* decrement counter*/
	BLR	R15		/* go home if less than 0*/
	LHS	R0,0(R2)	/* load data to be written*/
	INC	R2,2		/* increment pointer*/
	STHS	R0,4(R4)	/* write data to fifo*/
	J	writemany	/* loop back to write some more*/

 /*------------------------------------------------------------------------*/
 /* mfCmdNArgs(cmd,nBytes,pData) --*/
 /*	writes data to fifo when fifo is ready*/
 /*	if nBytes is odd it is increased by one*/
 /*	nBytes *must* be less than 1024*/
 /*	writes [nBytes,cmd,nBytes-4 of data from pData] */
 /*------------------------------------------------------------------------*/
	.text
	.globl TEXT_ENTRY(mfCmdNArgs)
	.align 1
TEXT_ENTRY(mfCmdNArgs) :
	DEC	R1,4		/* bump stack pointer (need 4 bytes of temp)*/
	CAS	R5,R15,R0	/* save return address in R5*/
	STS	R4,0(R1)	/* save 'pData' on the stack*/

	INC	R3,1		/* round nBytes by adding one and...*/
	NILZ	R3,R3,0xfffe	/* ...lopping off low bit */
	GET(R4,0xf000094a)	/* get address of PSR into R4*/
				/* get address of FIFO into 4(R4)*/

	WAITFIFO1( cmdNbcnt )	/* wait for fifo to come ready*/
	STHS	R3,4(R4)	/* write nBytes to FIFO*/

	WAITFIFO1( cmdNcmd )	/* wait for fifo to come ready*/
	STHS	R2,4(R4)	/* write 'cmd' to FIFO*/

	DEC	R3,4		/* deduct bytes already written from nBytes*/

	BALIX	R15,mfNOut	/* call write many function while...*/
	LS	R2,0(R1)	/* ...getting pData off of the stack*/

	BRX	R5		/* go home while... Note return address is in R5*/
	INC	R1,4		/* ...restoring stack pointer*/

	.align 2; .byte 0xdf, 2, 0xdf, 0;	/* trace table*/
	.data
	.globl DATA_ENTRY(mfCmdNArgs)
DATA_ENTRY(mfCmdNArgs) :
	.long	TEXT_ENTRY(mfCmdNArgs)
 /*------------------------------------------------------------------------*/
 /* mfCmdVArgs(cmd,nBytes,pData,nExtra) --*/
 /*	writes data to fifo when fifo is ready*/
 /*	nBytes *must* be even*/
 /*	if nExtra is odd it is increased by one*/
 /*	writes [nBytes,cmd,nBytes-4 of data from pData] */
 /*------------------------------------------------------------------------*/
	.text
	.globl TEXT_ENTRY(mfCmdVArgs)
	.align 1
TEXT_ENTRY(mfCmdVArgs) :
	DEC	R1,8		/* bump stack pointer (need 12 bytes of temp)*/
	STHS	R3,0(R1)	/* save 'nBytes' on the stack*/
	STS	R4,4(R1)	/* save 'pData' on the stack*/

	A	R5,R3		/* add 'nBytes' and 'extra' */
	INC	R5,1		/* round R5 by adding one and...*/
	NILZ	R3,R5,0xfffe	/* ...lopping off low bit (results into R3)*/
	CAS	R5,R15,R0	/* save return address in R5*/
	GET(R4,0xf000094a)	/* get address of PSR into R4*/
				/* get address of FIFO into 4(R4)*/

	WAITFIFO1( cmdVbcnt )	/* wait for FIFO to come ready*/
	STHS	R3,4(R4)	/* write nBytes+extra to FIFO*/

	WAITFIFO1( cmdVcmd )	/* wait for FIFO to come ready*/
	STHS	R2,4(R4)	/* write command to FIFO*/

	LH	R3,0(R1)	/* get 'nBytes' back from the stack*/
	DEC	R3,4		/* deduct bytes already written*/
	CI	R3,1024		/* is request bigger than half of queue*/
	BH	bigcall		/* too big, branch to bigcall*/
	
smallcall:
	BALIX	R15,mfNOut	/* call write many function while...*/
	LS	R2,4(R1)	/* ...getting pData off of the stack*/

	BRX	R5		/* go home while... Note return address is in R5*/
	INC	R1,8		/* ...restoring stack pointer*/

bigcall:			/* *** Note *** return address is in R5*/
	CAL	R3,-1024(R3)	/* decrement 'nBytes'*/
	STHS	R3,0(R1)	/* save new 'nBytes' on the stack*/
	CAL	R3,1024(R0)	/* set counter to 1024*/

	BALIX	R15,mfNOut	/* call write many function while...*/
	LS	R2,4(R1)	/* ...getting pData off of the stack*/

	LH	R3,0(R1)	/* get (new) 'nBytes' from the stack*/
	CI	R3,1024		/* still too big?*/
	BHX	bigcall		/* yup, jump to big call again while...*/
	STS	R2,4(R1)	/* ...saving new value of 'pData'*/
	J	smallcall	/* nope, make last call and get out of here*/

	.align 2; .byte 0xdf, 2, 0xdf, 0;	/* trace table*/
	.data
	.globl DATA_ENTRY(mfCmdVArgs)
DATA_ENTRY(mfCmdVArgs) :
	.long	TEXT_ENTRY(mfCmdVArgs)
 /*------------------------------------------------------------------------*/
 /* mfData(pData,nBytes)*/
 /*	writes data to fifo when fifo is ready*/
 /*	writes [nBytes of data from pData] */
 /**/
	.text
	.globl TEXT_ENTRY(mfData)
	.align 1
TEXT_ENTRY(mfData) :
	DEC	R1,8		/* increase stack pointer (need 8 bytes)*/
	CAS	R5,R15,R0	/* save return address in R5*/

	GET(R4,0xf000094a)	/* get address of PSR into R4*/
				/* get address of FIFO into 4(R4)*/

	INC	R3,1		/* round R3 by adding one and...*/
	NILZ	R3,R3,0xfffe	/* ...lopping off low bit*/
	CI	R3,1024		/* how big is nBytes?*/
	BHX	bigcall		/* too big -- branch to bigcall...*/
	STS	R2,4(R1)	/* ...while storing pData on stack*/

	BALI	R15,mfNOut	/* call write many function */

	BRX	R5		/* go home while... Note return address is in R5*/
	INC	R1,8		/* ...restoring stack pointer*/

	.align 2; .byte 0xdf, 2, 0xdf, 0;	/* trace table*/
	.data
	.globl DATA_ENTRY(mfData)
DATA_ENTRY(mfData) :
	.long	TEXT_ENTRY(mfData)
