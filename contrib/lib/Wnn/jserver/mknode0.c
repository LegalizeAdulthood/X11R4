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

static struct BZD	*free_bzd_top = NULL;
static struct free_list	*free_list_bzd = NULL;
static struct ICHBNP	*free_ichbn_top = NULL;
static struct free_list	*free_list_ichbn = NULL;

/************************************************/
/*        initialize link struct BZD            */
/************************************************/
init_bzd()
{
	free_area(free_list_bzd);
	if (get_area(FIRST_BZD_KOSUU, sizeof(struct BZD), &free_list_bzd) < 0)
	    return (-1);
	lnk_bzd(free_list_bzd);
	return (0);
}

/* free_bzd が 0 でない時に呼んだらあかんよ */
static
lnk_bzd(list)
struct	free_list *list;
{
	register int	n;
	register struct BZD *wk_ptr;

	free_bzd_top = wk_ptr =
	    (struct BZD *)((char *)list + sizeof(struct free_list));

	for (n = list->num - 1; n > 0; wk_ptr++, n--) 
		wk_ptr->lnk_son = wk_ptr + 1;
	wk_ptr->lnk_son = 0;
}


/***********************************************/
/*       initialize link struct ICHBNP         */
/***********************************************/
init_ichbnp()
{
	free_area(free_list_ichbn);
	if (get_area(FIRST_ICHBN_KOSUU, sizeof(struct ICHBNP), &free_list_ichbn) < 0)
	    return (-1);
	lnk_ichbn(free_list_ichbn);
	return (0);
}

/* free_ichbn が 0 でない時に呼んだらあかんよ */
static
lnk_ichbn(list)
struct	free_list *list;
{
	register int	n;
	register struct ICHBNP *wk_ptr;

	free_ichbn_top = wk_ptr =
	    (struct ICHBNP *)((char *)list + sizeof(struct free_list));

	for (n = list->num - 1; n > 0; wk_ptr++, n--) 
		wk_ptr->next_p = wk_ptr + 1;
	wk_ptr->next_p = 0;
}

/*********************************************************/
/*   struct BZD & ICHBNP   free area sakusei routine     */
/*********************************************************/
clr_node(bzd)
register struct BZD *bzd;		/*clear node top pointer */
{
	struct BZD *clrnd_sub();
	register struct BZD *return_p;	/*clrnd_sub return pointer */

	return_p = clrnd_sub(bzd->lnk_son);
	if (return_p != 0) {
		freebzd(return_p);
	}
	freebzd(bzd);
}

struct BZD *clrnd_sub(bzd)
register struct BZD *bzd;			/* node pointer */
{
	struct BZD *return_p;		  /* clrnd_sub return pointer */

	if (bzd != 0) {
		return_p = clrnd_sub(bzd->lnk_son);  
		if (return_p != 0) {
			freebzd(return_p);
		}
		return_p = clrnd_sub(bzd->lnk_br); 
		if (return_p != 0)
			freebzd(return_p);
		return(bzd);
	} else
		return(0);
}

freebzd(bzd)		/* struct BZD o free_area e link */
register struct BZD *bzd;	/* クリアするノードのポインタ */
{
	bzd->lnk_son = free_bzd_top;
	free_bzd_top = bzd;
	clr_sbn_node(bzd->sbn);
}

freeibsp(ichbnp_p)		/* struct ICHBNP o free_area e link  */
struct ICHBNP *ichbnp_p;	/* BZD ni link shiteiru ICHBNP eno pointer */
{
	register struct ICHBNP *wk_ptr;

	if (ichbnp_p == 0)
		return;
	for(wk_ptr = ichbnp_p;
	    wk_ptr->next_p != 0;
	    wk_ptr = wk_ptr->next_p) 
		;
	wk_ptr->next_p = free_ichbn_top;
	free_ichbn_top = ichbnp_p;
}

/******************************************/
/* BZD area no kakutoku routine		  */
/******************************************/
struct BZD *
getbzdsp()
{
	register struct	BZD	*rtnptr;

	if (free_bzd_top == 0) {
	    if (get_area(BZD_KOSUU, sizeof(struct BZD), &free_list_bzd) < 0)
		    return ((struct BZD *)-1);
	    lnk_bzd(free_list_bzd);
	}

	rtnptr = free_bzd_top;
	free_bzd_top = free_bzd_top->lnk_son;
	rtnptr->lnk_br = 0;
	rtnptr->lnk_son = 0;
	rtnptr->son_v = 0;
	return(rtnptr);
}

/******************************************/
/* ICHBNP area no kakutoku routine	  */
/******************************************/
struct ICHBNP *getibsp()
{
    register struct ICHBNP *rtnptr;

    if (free_ichbn_top == 0) {
	if (get_area(ICHBN_KOSUU, sizeof(struct ICHBNP), &free_list_ichbn) < 0)
		return ((struct ICHBNP *)-1);
	lnk_ichbn(free_list_ichbn);
    }

    rtnptr = free_ichbn_top;
    free_ichbn_top = free_ichbn_top->next_p;
    rtnptr->next_p = 0;
    return(rtnptr);
}


/*******************************************/
/* ICHBNP table からのオフセットの取りだし */
/*******************************************/
struct	fzkkouho *getfzkoh_body(ichbnp_p,no)
register  struct  ICHBNP 	*ichbnp_p;
int	no;	/* table index 0 org */
{
	register  int	cnt,no1;

	no1 = no / FZKIBNO;
	for(cnt = 0;cnt < no1;cnt++)
		ichbnp_p = ichbnp_p->next_p;
	return(&(ichbnp_p->fzkib[no % FZKIBNO]));
}

struct	fzkkouho *getfzkoh1_body(ichbnp_p,no)
register  struct  ICHBNP 	*ichbnp_p;
register  int	no;	/* table index 0 org */
{
	register  int	cnt,no1;

	no1 = no / FZKIBNO;
	for(cnt = 0;cnt < no1;cnt++)
		ichbnp_p = ichbnp_p->next_p;
	return(&(ichbnp_p->fzkib1[no % FZKIBNO]));
}


/*********************************************/
/*            make_node routine              */
/*********************************************/
mknode(ptr, yomi_eno, beginvect, fzkchar, endvect, mx_syo_bnst)
struct BZD	*ptr;	/* struct BZD node top pointer */
int	yomi_eno;	/* kaiseki bunsyou end index (end char no tugi) */
int	beginvect;      /* 前端ベクタ(-1:文節先頭、-2:なんでも)品詞No.*/
wchar_t	*fzkchar;
int	endvect;	/* shuutan vector */
int	mx_syo_bnst;	/* 解析を止める小文節の数 */
{
	register int     flg = 1;		/* call or nocall value */

	flg = mknd_sub(ptr, yomi_eno, beginvect, fzkchar, endvect, flg, mx_syo_bnst);
	switch (flg) {
	    case  1: ptr->kbcnt++;	 /* new_node ga tsukurareta toki */
		     return(1);
	    case  0: return(0);    /* new_node ga tsukurarenakatta toki */
	    default: return(-1);     /* free_area ga nakatta toki */
	}
}

mknd_sub(ptr, yomi_eno, beginvect, fzkchar, endvect, flg, mx_syo_bnst)
struct BZD	*ptr;	/* struct BZD node pointer */
int	yomi_eno;	/* kaiseki bunsyou end index (end char no tugi) */
int	beginvect;	/* 前端ベクタ(-1:文節先頭、-2:なんでも)品詞No.*/
wchar_t	*fzkchar;
int	endvect;	/* shuutan vector */
int	flg;		/* call or nocall value */
int	mx_syo_bnst;	/* 解析を止める小文節の数 */
{
    register int	keep_v;		/* max 評価値 */
    register struct BZD *wk_ptr;	/* work pointer */

    if (ptr->lnk_son != 0) {
	for (wk_ptr = ptr->lnk_son, keep_v = 0;
		    wk_ptr != 0; wk_ptr = wk_ptr->lnk_br) {
	    switch (mknd_sub(wk_ptr, yomi_eno, beginvect, fzkchar, endvect,
			     flg, mx_syo_bnst)) {
	    case 1:
		if (flg == 1) { /* 解析文節数 */
		    wk_ptr->kbcnt++;
		    /* brothers max hyoka value */
		    if (wk_ptr->son_v > keep_v) 
			keep_v = wk_ptr->son_v;
		    wk_ptr->son_v /= wk_ptr->kbcnt;
		}
		break;
	    case 0:
		if (flg == 1){
		    keep_v = wk_ptr->son_v;
		    flg = 0;
		} else {	/* brothers max hyoka value */
		    if (wk_ptr->son_v > keep_v) 
			keep_v = wk_ptr->son_v;
		}
		wk_ptr->son_v += wk_ptr->v_jc;
		break;
	    default :
		return(-1);
	    }
	}
	if (keep_v != 0)
	    ptr->son_v = hyoka2(keep_v, ptr->v_jc);
	return(flg);
    } else {
	if (ptr->j_c + 1 < yomi_eno) {
	    if (flg == 1) {	/* make new node */
		if (dbn_kai(ptr->j_c + 1, yomi_eno, beginvect, fzkchar,
		    endvect, WNN_VECT_NO, mx_syo_bnst, &(ptr->lnk_son)) < 0)
			return(-1); 		   /* ERROR */
		for (wk_ptr = ptr->lnk_son, keep_v = 0;
		     wk_ptr != 0; wk_ptr = wk_ptr->lnk_br) {
			if (wk_ptr->v_jc > keep_v)
				keep_v = wk_ptr->v_jc;    
				/* brothers max hyoka value */
		}
		ptr->son_v = hyoka2(keep_v, ptr->v_jc); 
	    }
	    return(1); 	/* make flg */
	} else 
	    return(0);	/* no make flg */
    }
}

/********************************************/
/*           bunsetsu hyoka kansuu          */
/********************************************/
hyoka2(max_v, par_v)
int  max_v;		/* brothers max hyoka value */
int  par_v;		/* parent hyoka value */
{
	return((max_v + par_v));
}
