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
#ifndef MPEL_HDWR_SEEN
#define	MPEL_HDWR_SEEN 1
/***********************************************************
		Copyright IBM Corporation 1987,1988

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

/* $Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelHdwr.h,v 6.2 88/11/03 19:00:27 paul Exp $ */
/* $Source: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelHdwr.h,v $ */

#ifndef lint
static char *rcsidmpelhdwr = "$Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelHdwr.h,v 6.2 88/11/03 19:00:27 paul Exp $";
#endif


    /*
     * System addresses of bounds of adapter data and program areas
     */

#define	MPEL_LOWDATA		((volatile unsigned short int *)0xf4c00000)
#define	MPEL_HIGHDATA		((volatile unsigned short int *)0xf4c3ffff)

#define MPEL_MMAP		0xf4000000

#define BANK2START		0xf4c20000
#define BANK3START		0xf4c30000
#define BANK2MPEL		0x02008000
#define BANK3MPEL		0x03008000

#define	MPEL_LOWPROGRAM		((volatile unsigned short int *)0xf4d60000)
#define	MPEL_HIGHPROGRAM	((volatile unsigned short int *)0xf4d7ffff)

    /*
     * system addresses of:
     *    PCR= Primary Control Register
     *    PSR= Primary Status Register
     *	  CA_PTR=  Communications Area Pointer
     *    COMM_REQ= Communications Area Request Field
     *    COMM_REASON= Communications Area Reason Field
     *    COMM_PARM_ADDR= Communications Area Parameter Field Address
     *    FIFO= Command Fifo
     *    IPR= Interrupt Pending Register
     */

#define	MPEL_PCR		(*((volatile unsigned short int *)0xf0000948))
#define	MPEL_PSR		(*((volatile unsigned short int *)0xf000094a))
#define	MPEL_CA_PTR		(*((volatile unsigned long int *)0xf4c008e0))
#define MPEL_COMM_REQ		(*((volatile unsigned short int *)0xf4c00902))
#define MPEL_COMM_REASON	(*((volatile unsigned short int *)0xf4c00904))
#define MPEL_COMM_PARM_ADDR	((volatile unsigned short int *)0xf4c00954)
#define	MPEL_ATF_LOC_PTR	(*((volatile unsigned long int *)0xf4c00920))
#define MPEL_STATE_LIST_PTR	(*((volatile unsigned long int *)0xf4c00908))
#define MPEL_FIFO		(*((volatile unsigned short int *)0xf000094e))
#define MPEL_IPR		(*((volatile unsigned short int *)0xf0000940))

    /*
     * Masks for fields in PCR and PSR:
     */

#define	PCR_HOLD		0x0100
#define	PCR_RESET_TMS		0x0200
#define PCR_INTR_TMS		0x0400
#define	PCR_BYTE_ORDER		0x0800
#define	PCR_DMA_LVL0		0x1000
#define	PCR_DMA_LVL1		0x2000
#define	PCR_DMA_ENABLE		0x4000
#define PCR_ENABLE_TMS_INTR	0x0001
#define	PCR_ENABLE_PP_INTR	0x0002
#define	PCR_ENABLE_OVFLW_INTR	0x0004
#define	PCR_ENABLE_HE_INTR	0x0008
#define	PCR_ENABLE_HF_INTR	0x0010

#define	PSR_NOT_HOLD_ACK	0x0100
#define	PSR_INTR_PENDING	0x0200
#define	PSR_FIFO_NOT_FULL	0x2000
#define	PSR_FIFO_NOT_HALF_FULL	0x4000
#define	PSR_FIFO_NOT_EMPTY	0x8000
#define	PSR_FIFO_FLAGS		0xe000

    /*
     * Data/microcode files, and where (on the adapter) to load them
     */

#define	MPEL_UCODE_ADDR		((volatile unsigned short int *)0xf4d60000)
#define	MPEL_UCODE_FILE		"/usr/lib/mpel/mcr1v001.bin"

#define	MPEL_CIRCLE_ADDR	((volatile unsigned short int *)0xf4c01e00)
#define	MPEL_CIRCLE_FILE	"/usr/lib/mpel/cirtab.dat"

#define	MPEL_SINE_ADDR		((volatile unsigned short int *)0xf4c02e00)
#define	MPEL_SINE_FILE		"/usr/lib/mpel/sintab.dat"

#define	MPEL_MARKER_LOC_ADDR	((volatile unsigned short int *)0xf4c07360)
#define	MPEL_MARKER_LOC_FILE	"/usr/lib/mpel/markid.dat"

#define	MPEL_HATCH_ADDR		((volatile unsigned short int *)0xf4c07ca0)
#define	MPEL_HATCH_FILE		"/usr/lib/mpel/hchtab.dat"

#define	MPEL_MARKER_FNT_ADDR	((volatile unsigned short int *)0xf4c04f00)
#define	MPEL_MARKER_FNT_FILE	"/usr/lib/mpel/marker.dat"


/*
 * This macro converts an RT address of the form 0xf4c????? and 
 * converts it to a megapel address.  For constants all of this
 * should get done at compile time.
 * WARNING: this macro evaluates its argument several times, 
 *   beware args with side effects.
 */

#define mpelAddr(x) \
	( ( ( ( ( (unsigned long int) (x) ) & 0x00030000 ) << 8 ) \
	| ( ( ( (unsigned long int) (x) ) >> 1 ) & 0x00007fff ) ) \
	+ ( ( ( (unsigned long int) (x) ) & 0x00030000 ) ? 0x8000 : 0 ) )

#define rtAddr(x) \
	( (volatile unsigned short int *)( ( ( (unsigned long )(x) & 0x03000000 ) >> 8 ) \
	| ( ( (unsigned long )(x) & 0x00007fff ) << 1 ) \
	| ( 0xf4c00000 ) ) )


/*
 * Megapel offscreen memory usage for bank 1
 */

/* 256 bytes for each Pattern area */
#define MPEL_PAT1		((volatile unsigned short int *)0xf4c10000)
#define MPEL_PAT2		((volatile unsigned short int *)0xf4c10100)
#define MPEL_PAT3		((volatile unsigned short int *)0xf4c10200)
#define MPEL_PAT4		((volatile unsigned short int *)0xf4c10300)
/* 1024 bytes for each the cursor save area */
#define MPEL_CURSOR_SAVE	((volatile unsigned short int *)0xf4c10400)
/* 1028 bytes for each the color table area */
#define MPEL_COLOR_TABLE	((volatile unsigned short int *)0xf4c10800)
/* 36 bytes for each the pattern table area */
#define MPEL_PAT_TABLE		((volatile unsigned short int *)0xf4c10c04)
/* 61440 bytes for each the blit staging area */
#define MPEL_BLIT_STAGE		((volatile unsigned short int *)0xf4c11000)
#define MPEL_BLIT_STAGE_SIZE	0xf000

/*
 * Megapel offscreen memory usage for bank 2
 */

#define	MPEL_ATF_REGION		BANK2START

/*
 * Megapel command definitions
 */

#define MPELCMD_ENTER_FIFO      0x02
#define MPELCMD_SET_PICK        0x06
#define MPELCMD_STOP_FIFO       0x0a
#define MPELCMD_CONT_FIFO       0x0c
#define MPELCMD_ACT_CUR         0x10
#define MPELCMD_DEF_CUR         0x12
#define MPELCMD_REM_CUR         0x14
#define MPELCMD_CLEAR_FB        0x16
#define MPELCMD_LOAD_CMAP       0x20
#define MPELCMD_FLUSH_FIFO      0x22
#define MPELCMD_SET_PICK_WIN    0x26
#define MPELCMD_RESET_PICK      0x28
#define MPELCMD_SET_BLINK       0x2c
#define MPELCMD_RESET_BLINK     0x2e

/*
 * Megapel reason codes
 */

#define	MPELRSN_RESET		0x00
#define	MPELRSN_INIT_DONE	0x01
#define MPELRSN_ENTER_FIFO_DONE	0x03
#define	MPELRSN_SET_PICK_DONE	0x07
#define	MPELRSN_STOP_FIFO_DONE	0x0b
#define	MPELRSN_CONT_FIFO_DONE	0x0d
#define	MPELRSN_ACT_CUR_DONE	0x11
#define	MPELRSN_DEF_CUR_DONE	0x13
#define	MPELRSN_REM_CUR_DONE	0x15
#define	MPELRSN_CLEAR_FB_DONE	0x17
#define	MPELRSN_LOAD_CMAP_DONE	0x21
#define	MPELRSN_FLUSH_FIFO_DONE	0x23
#define	MPELRSN_SET_PICK_WIN_DONE	0x27
#define	MPELRSN_RESET_PICK_DONE	0x29
#define	MPELRSN_SET_BLINK_DONE	0x2d
#define	MPELRSN_RESET_BLINK_DONE	0x2f
#define	MPELRSN_PICK_OCCURRED	0x43
#define	MPELRSN_ILLEGAL_REQUEST	0x53
#define	MPELRSN_ILLEGAL_ELEMENT	0x55
#define	MPELRSN_SYNC_ELEMENT	0x57
#define	MPELRSN_FIFO_FLUSH_ERR	0x200
#define	MPELRSN_BAD_STR_LEN	0x201
#define	MPELRSN_NO_FONT		0x203

#define	DRAW_EDGE	1
#define	DONT_DRAW_EDGE	2

#endif /* ndef MPEL_HDWR_SEEN */
