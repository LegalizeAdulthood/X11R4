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
#include "ddefine.h"
#include "de_header.h"
#include "kaiseki.h"
#include "wnn_malloc.h"

static struct SYO_BNSETSU	*free_sbn_top = NULL;
static struct free_list		*free_list_sbn = NULL;
/************************************************/
/* initialize link struct SYO_BNSETSU           */
/************************************************/
init_sbn()
{
	free_area(free_list_sbn);
	if (get_area(FIRST_SBN_KOSUU, sizeof(struct SYO_BNSETSU),
		&free_list_sbn) < 0)
	    return (-1);
	lnk_sbn(free_list_sbn);
	return (0);
}

get_area(kosuu, size, list)
register int	kosuu;
register int	size;
struct  free_list **list;
{
	register struct free_list *area;

	if ((area = (struct free_list *)
	    malloc(size * kosuu  + sizeof(struct free_list))
	    ) == NULL) {
		wnn_errorno = WNN_MALLOC_INITIALIZE;
		error1("Cannot Malloc in get_area.\n");
		return (-1);
	}
	area->lnk = *list;
	area->num = kosuu;
	*list = area;
	return (0);
}

free_area(list)
register struct  free_list *list;
{
	if (list == 0)
		return;
	free_area(list->lnk);
	free(list);
}

/* free_sbn が 0 でない時に呼んだらあかんよ */
static
lnk_sbn(list)
struct	free_list *list;
{
	register int	n;
	register struct SYO_BNSETSU *wk_ptr;

	free_sbn_top = wk_ptr =
	    (struct SYO_BNSETSU *)((char *)list + sizeof(struct free_list));

	for (n = list->num - 1; n > 0; wk_ptr++, n--) 
		wk_ptr->lnk_br = wk_ptr + 1;
	wk_ptr->lnk_br = 0;
}


/*******************************************************/
/* struct SYO_BNSETSU & ICHBNP & KANGO free エリア作成 */
/*******************************************************/
freesbn(sbn)			/* struct SYO_BNSETSU を free_area へリンク */
register struct SYO_BNSETSU *sbn;	/* クリアするノードのポインタ */
{
	if (sbn == 0)
		return;
	sbn->reference--;
	if (sbn->reference <= 0) {
		sbn->lnk_br = free_sbn_top;
		free_sbn_top = sbn;
	}
}

clr_sbn_node(sbn)
register struct	SYO_BNSETSU	*sbn;
{
	if (sbn == 0)
		return;
	if ((sbn->reference) <= 1) {
		freesbn(sbn);
		clr_sbn_node(sbn->parent);
	} else {
		sbn->reference--;
	}
}
/******************************************/
/* SYO_BNSETSU area の獲得		  */
/******************************************/
struct SYO_BNSETSU *
getsbnsp()
{
	register struct	SYO_BNSETSU	*rtnptr;

	if (free_sbn_top == 0) {
	    if (get_area(SBN_KOSUU, sizeof(struct SYO_BNSETSU), &free_list_sbn) < 0)
		    return ((struct SYO_BNSETSU *)-1);
	    lnk_sbn(free_list_sbn);
	}

	rtnptr = free_sbn_top;
	free_sbn_top = free_sbn_top->lnk_br;
	rtnptr->lnk_br = 0;
	rtnptr->parent = 0;
	rtnptr->son_v = 0;
	rtnptr->reference = 0;
	rtnptr->jentptr = 0;
	rtnptr->status = 0;
	return(rtnptr);
}
