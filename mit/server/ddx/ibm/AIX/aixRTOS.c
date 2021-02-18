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
#ifdef	AIXrt

/***============================= NOTE ============================*
 ***
 ***	The following functions are temporarily fixed until
 ***	AIX 2.2.1 available.
 ***
 ***===============================================================*/

#include  <errno.h>
#include  <sys/types.h>
#include  <sys/uio.h>

writev (sd, iov, cnt)
    int     sd;
    struct iovec *iov ;
    int     cnt;
{
    int     rc;
    int     i;


#ifdef OldStuff /* that doesn't work */
    for ( rc = 0; cnt--; ++iov ) {
	    i = write( sd, iov->iov_base, iov->iov_len );
	    if ( i < 0 ) {
		    return( i );
	    }
	    rc += i;
    }
    return ( rc );

#else  NewStuff /* that does works */

    int     total ;

    /* similar to readv error checking fix               */
    /* probably helps to validate user's array of iovecs */
    if( (cnt <= 0) || (cnt > 16) ) {
	    errno = EINVAL;
	    return( -1 );
    }
    /* can'g easily check the pointers, but can check the lengths */
    for( i = 0; i < cnt; i++ ) {
	    if( iov[i].iov_len < 0 ) {
		    errno = EINVAL;
		    return( -1 );
	    }
    }
    /* more checks to come if you want emulation to be perfect ??? */

    /* write up till all user's iovec's are used... */
    for (total = 0; --cnt >= 0; iov++, total += i ) {
	/* for this iovec, read until it is full */
	for( i=0; i < iov->iov_len; i += rc ) {
	    rc = write(sd, iov->iov_base + i, iov->iov_len - i);

	    /* try to remember what has been writen */

	    if ( rc <= 0 ) {
		if (total==0)
		    return ( i == 0 ? rc : i );
		else
		    return (total + i);
	    }
	}
    }
    return (total);

#endif
}

readv (fd, iov, iovcnt )
    int     fd;
    struct iovec *iov ;
    int     iovcnt;
{
    int     rc;
    int     n;
    int     total;

    /* probably helps to validate user's array of iovecs */
    if( (iovcnt <= 0) || (iovcnt > 16) ) {
	    errno = EINVAL;
	    return( -1 );
    }
    /* can'g easily check the pointers, but can check the lengths */
    for( n = 0; n < iovcnt; n++ ) {
	    if( iov[n].iov_len < 0 ) {
		    errno = EINVAL;
		    return( -1 );
	    }
    }

    /* read up till all user's iovec's are used... */
    for (total = 0; --iovcnt >= 0; iov++, total += n ) {
	/* for this iovec, read until it is full */
	for( n=0; n < iov->iov_len; n += rc ) {
	    rc = read(fd, iov->iov_base+n, iov->iov_len-n);
	    /* if no more to read or error ... return */
	    /* note that if readv has read anything, subsequent */
	    /* errors must be ignored, or data gets lost */
	    if ( rc <= 0 ) {

#ifdef OldStuff /* that doesn't work */

		return ( total == 0 ? rc : total );

#else NewStuff /* that does work */

		if (total==0)
		   return ( n == 0 ? rc : n );
		else
		   return (total + n) ;
#endif
	    }
	}
    }
    return (total);
}

#endif /* AIXrt */
