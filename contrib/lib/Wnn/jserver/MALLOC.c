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

#include <stdio.h>
#include "wnn_malloc.h"
#undef	malloc
#undef	realloc
#undef	free

#ifdef	HONMONO
extern char *malloc();
extern void free();
extern char *realloc();
extern char *calloc();
#else
extern char *MALLOC();
extern void free();
extern char *REALLOC();
extern char *CALLOC();
#define	malloc(x) MALLOC(x)
#define	calloc(x,y) CALLOC(x,y)
#define	realloc(x,y) REALLOC(x,y)
#endif

char *
malloc0(size)
int size;
{
    test2();
    if(size == NULL){
	size = 1;
    }
    return(calloc(size,1));
}

void
free0(pter)
char *pter;
{
    if(pter == NULL) return;
    free(pter);
}

char *
realloc0(pter,size)
char *pter;
int size;
{
    if(size == NULL){
	size = 1;
    }
    if(pter == NULL){
	return(malloc(size));
    }else{
	return(realloc(pter,size));
    }
}
/********************************/
#undef	realloc
#undef	malloc
#undef	calloc

extern char *malloc();
extern char *calloc();
static char *malloc_p;

test2()
{
 test(malloc(5000));
}
test1()
{
 test(malloc_p);
}
test(p)
int *p;
{
	static int tst = 1;
	static int tst1 = 0;
	int *p0, *p1;
	malloc_p = (char *)p;
	if (tst) {
		p1 = p - 1;
		p0 = p1;
		while(1) {
		    if (tst1) {
			    printf("%x: %x\n", p1, *p1);
		    }
		    p1 = *p1 & 0xfffffffe;
		    if (p1 == p || p1 == p0){
			printf("OK!\n");
			break;
		    }
		}
	}
}


char *
MALLOC(cnt)
int cnt;
{
	char *p;
	printf("malloc size = %d\n", cnt);
	p = malloc(cnt);
	test(p);
	return(p);
}

char *
CALLOC(cnt,size)
int cnt;
int size;
{
static int tmp = 0;
	char *p;
	printf("%d : calloc size = %d\n",tmp++, cnt);
	p = calloc(cnt,size);
	test(p);
	return(p);
}


char *
REALLOC(p, cnt)
int *p;
int cnt;
{
	extern char * realloc();
	char *p;
	printf("realloc size = %d\n", cnt);
	p = realloc(p, cnt);
	test(p);
	return(p);
}

