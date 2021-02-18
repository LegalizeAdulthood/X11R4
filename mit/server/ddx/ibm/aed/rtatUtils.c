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

#ifdef ATRIO

#include "OScompiler.h"
#include "rtatUtils.h"

/* Some unresolved compiler dependancies here !! */
#ifdef __HIGHC__
#define REAL_MOVE _move
#else
#ifdef SYSV
#define REAL_MOVE memcpy
#else
#define REAL_MOVE bcopy
#endif
#endif

void
hcopy_to(src,dest,length)
register unsigned short int *src, *dest ;
register unsigned int length ;
{
  for ( length = (++length)/2 ; length-- ; )
    *(volatile unsigned long int *) (dest++) = (unsigned long int) (*src++) ;
  return ;
}

void
hcopy_from(src,dest,length)
register unsigned short int *src, *dest ;
register unsigned int length ;
{
  for ( length = length + 1 / 2 ; length-- ; )
    *dest++ = (* (volatile unsigned long int *) (src++) ) ;
  return ;
}

/*
**	This function uses hcopy_from, hcopy_to, or bcopy/_move,
**	depending on whether the source or destination pointers
**	point into the 128K window.  It assumes that hcopy will
**	NEVER be called with BOTH source and destination pointers
**	pointing into the 128K window.
*/

void
hcopy(src,dest,length)
register unsigned short int *src, *dest ;
register unsigned int length ;
{
  if (!(((int)src^SHORT_128)&(0xffff0000)))
  {
    hcopy_from(src,dest,length);
  }
  else if (!(((int)dest^SHORT_128)&(0xffff0000)))
  {
    hcopy_to(src,dest,length);
  }
  else
  {
    REAL_MOVE( src, dest, length ) ;
  }
  return ;
}

#endif /* RTAT */
