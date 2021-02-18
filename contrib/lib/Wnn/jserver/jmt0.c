/*
	Copyright
		Kyoto University Research Institute for Mathematical Sciences
		1987, 1988, 1989
	Copyright OMRON TATEISI ELECTRONICS CO. 1987, 1988, 1989
	Copyright ASTEC, Inc. 1987, 1988, 1989


    Permission to use, copy, modify, and distribute this software
    and its documentation for any purpose and without any fee is
    hereby granted, subject to the following restrictions:

    The above copyright notice and this permission notice must appear
    in all versions of this software;

    The name of "Wnn" may not be changed; 

    All documentations of software based on "Wnn" must contain the wording
    "This software is based on the original version of Wnn developed by
    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
    OMRON TATEISI ELECTRONICS CO. and
    ASTEC, Inc.", followed by the above copyright notice;

    The name of KURIMS, OMRON and ASTEC may not be used
    for any purpose related to the marketing or advertising
    of any product based on this software.

    KURIMS, OMRON and ASTEC make no representations about
    the suitability of this software for any purpose.
    It is provided "as is" without express or implied warranty.

    KURIMS, OMRON and ASTEC DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
    SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
    IN NO EVENT SHALL KURIMS, OMRON AND ASTEC BE LIABLE FOR ANY SPECIAL,
    INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
    LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
    OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.

    Author: Hideki Tsuiki	Kyoto University
				tsuiki%kaba.or.jp@uunet.uu.net

	    Hiroshi Kuribayashi	OMRON TATEISI ELECTRONICS CO.
				kuri@frf.omron.co.jp
				uunet!nff.ncl.omron.co.jp!kuri

	    Naouki Nide		Kyoto University
				nide%kaba.or.jp@uunet.uu.net

	    Shozo Takeoka	ASTEC, Inc.
				take%astec.co.jp@uunet.uu.net

	    Takashi Suzuki	Advanced Software Technology & Mechatronics
				Research Institute of KYOTO
				suzuki%astem.or.jp@uunet.uu.net

*/
/*	Version 4.0
 */
#include "commonhd.h"
#include "jslib.h"
#include "ddefine.h"
#include "jdata.h"
#include "kaiseki.h"

#include <stdio.h>


init_jmt(x)
register int	x ;
{
	register int	n;
	register struct jdata **keep_ptr ;
	
	for (n = x; n < initjmt &&
	    (jmtp[n] == (struct jdata **)0 ||
	     jmtp[n] == (struct jdata **) -1); n++)
		;

	if (n < initjmt) {
		keep_ptr = jmt_ptr ;

		if (n == x)
			jmt_ptr = jmtp[x] ;
		else
			jmt_ptr = jmtp[n] + (maxj[n] - n + 1) ;

		for(n = 0;(jmt_ptr + n) < keep_ptr && *(jmt_ptr + n) <= 0; n++)
			;
		if ((jmt_ptr + n) < keep_ptr)
			j_e_p = *(jmt_ptr + n) ;
	} else {
	  	jmt_ptr = jmt_ ;  
    		j_e_p = jmtw_ ;
	}
/* this part is added 8/18 by H.T */
	for(keep_ptr = jmt_ptr ; keep_ptr < jmt_end ; keep_ptr++){
		*keep_ptr = NULL;
	}
}


jmt_set (yomi)
register int	yomi;	/* 読み文字列の先頭へのポインタ(逆順) */
{
	register int	n ;
	if ((n = jishobiki(&(bun[yomi]), jmt_ptr)) > 0) {
		jmtp[yomi] = jmt_ptr;
		jmt_ptr += n ;
		maxj[yomi] = yomi + (n - 1) ;
	} else {
		if (n == 0) {
			jmtp[yomi] = 0 ;
			maxj[yomi] = 0 ;
		} else{
		    jmtp[yomi] = 0;
		    maxj[yomi] = 0;
/*	H.T.		return(-1) ; */
		}
	}
	return(1) ;
}
