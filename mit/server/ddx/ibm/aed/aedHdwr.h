/***********************************************************
		Copyright IBM Corporation 1987

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

#include "rtatUtils.h"

#if defined(ATRIO)

#define VIKADDR 0xA0000
#define VIKROOT SHORT_128
#define VWRITE(BUF,N,OFF)	\
	{ hcopy_to(BUF,(char *)(VIKROOT + OFF),(int) N+N);  }
#define VREAD(BUF,N,OFF)	\
	{ hcopy_from((char *)(VIKROOT + OFF),BUF,(int) N+N);  }

#else /* RTAT */

#define VIKROOT 0xF40A0000
#define VWRITE(BUF,N,OFF)       \
        { bcopy(BUF,(char *)(VIKROOT + OFF),(int) N+N);  }
#define VREAD(BUF,N,OFF)        \
        { bcopy((char *)(VIKROOT + OFF),BUF,(int) N+N);  }

#endif /* RTAT */

#define AED_SCREEN_HEIGHT 800
#define AED_SCREEN_WIDTH 1024
#define AED_SCREEN_DEVICE "/dev/aed"
#define MOUSE_DEVICE "/dev/msaed"

#define MICROCODE "/usr/lib/aed/whim.aed"

#define VIKSEMA		0
#define VIKCMD 		1
#define ORMERGE 	2
#define ORFONTID 	3
#define ORWIDTH 	4
#define ORDASHPAT 	5
#define ORDASHLEN 	6
#define ORXPOSN 	7
#define ORYPOSN 	8
#define ORXORG 		9
#define ORYORG 		10
#define ORCLIPLX 	11
#define ORCLIPLY 	12
#define ORCLIPHX 	13
#define ORCLIPHY 	14
#define ORCOLOR 	15
#define ORLEN 		16
#define ORDATA 		17
#define FFID		2
#define CFFID		2
#define CFCID		3

extern volatile unsigned short *vikint;
extern volatile unsigned short *semaphore;
extern int vikoff;
extern int mergexlate[16];
