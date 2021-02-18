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
/*
  eval(crypt(PWD, "HA")) routine.
*/

#ifndef JS
#include <stdio.h>
#include "commonhd.h"
#include "jslib.h"
#endif

extern char *crypt();

#ifdef JS
static
#endif
new_pwd(src, encd)
char *src, *encd;
{
    int i, x,c;
    char xx[2];
    char *cr;

    if(encd == NULL)encd = src;
    if(strcmp(src, "") == 0){ bzero(encd, WNN_PASSWD_LEN);return;}
    x = time(NULL);
    xx[0] = x & 0x3f;
    xx[1] = (x & 0x3f00) >> 8;
    for (i = 0; i < 2; i++) {
	c = xx[i] + '.';
	if (c > '9')
	    c += 7;
	if (c > 'Z')
	    c += 6;
	xx[i] = c;
    }
    cr = crypt(src, xx);
    bzero(encd, WNN_PASSWD_LEN);
    strncpy(encd, cr, WNN_PASSWD_LEN);
}

#ifdef JS
static
#endif
check_pwd(src, encd)
char *src, *encd;
{
    if(strcmp(encd, "") == 0) return(1);  /* No passwd */
    return(!strncmp(encd, crypt(src, encd),WNN_PASSWD_LEN));
}
