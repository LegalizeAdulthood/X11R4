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

/*
	BSD42
 */
#if defined(BSD42) && (! defined(BSD43))

#include <stdio.h>
#define SLOTSIZ 32

char *ttyname();
char *rindex();

int ttyfdslot(fd)
int fd;
{
    char eachslot[SLOTSIZ];
    register char *fullnamp;
    register char *ttynamp;
    register FILE *ttysfp;
    register char *p;
    register int slotnum;

    if ((fullnamp = ttyname(fd)) == NULL) return NULL;
    if ((ttynamp = rindex(fullnamp, '/')) == NULL) {
        ttynamp = fullnamp;
    } else {
        ttynamp++;
    }
    if ((ttysfp = fopen("/etc/ttys", "r")) == NULL) return NULL;
    for (slotnum = 0; fgets(eachslot, SLOTSIZ, ttysfp); ) {
        p = eachslot + strlen(eachslot) - 1;
        if (*p == '\n') *p = '\0';
        slotnum++;
        if (strcmp(ttynamp, &eachslot[2]) == 0) {
            fclose(ttysfp);
            return slotnum;
        }
    }
    fclose(ttysfp);
    return NULL;
}
#endif	/*	BSD42	*/


/*
	BSD43
 */
#ifdef BSD43

#include <ttyent.h>
#include <stdio.h>

char *ttyname();
char *rindex();

int ttyfdslot(fd)
int fd;
{
    register char *fullnamp;
    register char *ttynamp;
    register int slotnum;
    register struct ttyent	*te ;
    if ((fullnamp = ttyname(fd)) == NULL) return NULL;
    if ((ttynamp = rindex(fullnamp, '/')) == NULL) {
        ttynamp = fullnamp;
    } else {
        ttynamp++;
    }
    for (slotnum = 1; (te = getttyent()) != NULL; slotnum++) {
	if ( strcmp(te->ty_name, ttynamp) == 0 ) {
		endttyent() ;
		return	slotnum ;
	}
    }
    endttyent() ;
    return NULL;
}

#endif	/*	BSD43	*/
