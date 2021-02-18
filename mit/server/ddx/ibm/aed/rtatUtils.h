/***********************************************************
		Copyright IBM Corporation 1988

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
#ifdef ATRIO
#define SHORT_128 0xd00a0000 /* Magic Memory location for MMU's pc memory */
#define VIKSTORE(lhs,rhs) *(volatile unsigned long int *)(&(lhs))=(rhs)
#define VIKSTOREB(lhs,rhs) *(((volatile unsigned long int *)&(lhs))+32768)=(rhs)
#define VIKLOAD(rhs) *((volatile unsigned long int *)(&(rhs)))

#undef MOVE
#define MOVE hcopy
void hcopy_to(),hcopy_from(),hcopy();

#else /* ATRIO */

#define VIKSTORE(lhs,rhs) (lhs)=(rhs)
#define VIKSTOREB(lhs,rhs) (lhs)=(rhs)
#define VIKLOAD(rhs) (rhs)

#endif /* ATRIO */
