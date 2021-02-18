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
#include <stdio.h>
#include "commonhd.h"
#include "config.h"
#include "de_header.h"
#include "wnnerror.h"
#include "jslib.h"
#include "hinsi_file.h"
#include "jdata.h"
#include "wnn_malloc.h"

/*	Version 4.0
 */

make_hinsi_list(jtl)
struct JT *jtl;
{
    int k, state, cnt, wc;
    w_char *j, *d, *start;

    if (jtl->maxhinsi_list == 0){
	jtl->node = (struct wnn_hinsi_node *)malloc(0);
	jtl->maxnode = 0;
	return;
    }
    jtl->maxnode = Strtime(jtl->hinsi_list, jtl->maxhinsi_list, NODE_CHAR);
    jtl->node = (struct wnn_hinsi_node *)
	malloc(jtl->maxnode * sizeof(struct wnn_hinsi_node) + 
	jtl->maxhinsi_list * sizeof(w_char));
    d = (w_char *)(jtl->node + jtl->maxnode);
    if(jtl->hinsi_list == NULL) return;
    j = jtl->hinsi_list;
    k = -1;
    for(;;){
	wc = get_a_word(j, &start, &state);
	if(state == 2 && wc == 0){ break;}
	Strncpy(d, start, wc);
	d[wc] = 0;
	if(state == 2){ cnt++; break;}
	if(state == 1){
	    if(k >= 0) jtl->node[k].kosuu = cnt;
	    cnt = 0;
	    k++;
	    jtl->node[k].name = d;
	    jtl->node[k].son = d + wc + 1;
	}else{
	    cnt++;
	}
	d += wc + 1;
	j = start + wc + 1;
    }
    if(k >= 0) jtl->node[k].kosuu = cnt;
}
    

Strtime(w, wc, c)
w_char *w;
int wc;
w_char c;
{
    int ret = 0;
    for(;wc;wc--, w++){
	if(*w == c) ret++;
    }
    return(ret);
}

get_a_word(d, startp, statep)
w_char *d, **startp;
int *statep;
{
    w_char *d0 = d;
    int c, wc;
    for(;;){
	if((c = *d++) == 0){
	    *statep = 2;
	    *startp = d0;
	    return(0);
	}
	if(c == IGNORE_CHAR1 || c == IGNORE_CHAR2 || c == IGNORE_CHAR1 ||
	   c == CONTINUE_CHAR || c == '\n' || c == HINSI_SEPARATE_CHAR){
	    continue;
	}
	if(c == COMMENT_CHAR){
	    for(;(*d++ != '\n'););
	    continue;
	}
	break;
    }
    *startp = d - 1;
    wc = 1;
    for(;;wc++){
	if((c = *d++) == 0){
	    *statep = 2;
	    return(wc);
	}
	if(c == IGNORE_CHAR1 || c == IGNORE_CHAR2 || c == IGNORE_CHAR1 ||
	   c == CONTINUE_CHAR || c == '\n' || c == HINSI_SEPARATE_CHAR){
	    *statep = 0;
	    return(wc);
	}else if(c == NODE_CHAR){
	    *statep = 1;
	    return(wc);
	}
    }
}

extern wchar *wnn_hinsi_name();


has_dict(n, m, w)
struct wnn_hinsi_node *n;
int m;
w_char *w;
{
    int j;
    for(;m;m--){
	if(Strcmp(n[m].name, w) == 0)
	for(j = n[m].kosuu; j ; j--){
	    if(Strcmp(n[m].son[j], w) == 0)
		return(1);
	}
    }
    return(0);
}

hinsi_table_set(jtl, table)
struct JT *jtl;
w_char *table;
{
    int len;

    free(jtl->node);
    free(jtl->hinsi_list);

    jtl->maxhinsi_list = Strlen(table);
    
    jtl->hinsi_list = (w_char *)malloc(jtl->maxhinsi_list * sizeof(w_char));
    Strcpy(jtl->hinsi_list, table);
    make_hinsi_list(jtl);
    jtl->dirty = 1;
    return(0);
}
