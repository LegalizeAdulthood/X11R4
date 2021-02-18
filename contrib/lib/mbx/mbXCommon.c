
/*"@(#)mbXCommon.c	1.3*/

/***********************************************************
Copyright International Business Machines Corporation 1989

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IBM not be
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

/*
 * Name: mbXCommon - Parsing alogorithm shared by all mbX functions
 *
 * Description:
 *
 * This file contains the common code used to parse a multibyte string into
 * multiple calls of X.  It is intended that all the mbX functions 
 * will include this module to perform their parsing.  
 * 
 * This code depends on the following variables being defined:
 * 
 * caddr_t _mbXbuf	- static pointer to work buffer
 * int _mbXbuflen	- static length (in bytes) of _mbXbuf
 * caddr_t str		- pointer to sjis string
 * int len		- length of str
 * 
 * In addition, the following macros need to be defined to perform
 * the specific X operation (see mbXDraw.c for example).
 * 
 * mbXflush(cs)		- Do X operation on single byte data
 * mbXflush16(cs)	- Do X operation on XChar2b data
 *
 *			  When these macros are invoked the following
 *			  variables are defined:
 *			  _mbXbuf   - contains string.
 *			  p-_mbXbuf - defines the number of bytes in _mbXbuf
 *			  cs 	    - defines the character set (0-3)
 * 			  (cs is an identifier of the font to be used.
 *			   It is defined to be 0 to 3 currently. )	
 * 
 */

{

caddr_t p, mbXbuffer;
int cs, new_cs;
int n; 
XChar2b *index16;

p = mbXbuffer = (caddr_t)XtMalloc( len * sizeof(XChar2b) );

n = mblen(str,len);

for ( cs = CSid(str, n, p ) ; len ; n = mblen(str,len) ) {

     if ( n < 1 ) {
	  /* In the middle of a code point, stop drawing */
	  break;
     }

     if ( (new_cs = CSid(str, n, &index16)) != cs ) {
          mbXflush16(cs) ;
	  cs = new_cs ; 
     }
     p[0] = index16->byte1; p[1] = index16->byte2;
     str += n ; len -= n ; p += sizeof(XChar2b);
}     
mbXflush16(cs);

XtFree( mbXbuffer );

}
