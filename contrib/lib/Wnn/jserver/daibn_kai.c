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
/************************************************
 *	��ʸ�����				*
 ************************************************/
#include <stdio.h>
#include "commonhd.h"
#include "ddefine.h"
#include "de_header.h"
#include "kaiseki.h"
#include "fzk.h"
#include "wnn_malloc.h"

extern	struct	BZD *getbzdsp();

static	int	yomi_sno_tmp;
int		_status = 0;
static	int	edagari_hyouka;

dbn_kai(yomi_sno, yomi_eno, beginvect, fzkchar, endvect, endvect1, nmax, rbzd)
int		yomi_sno;	/* �����ʸ�����ϰ��� */
register int	yomi_eno;	/* �����ʸ����λ���֡ʤμ���*/
int		beginvect;	/* ��ü�٥���(-1:ʸ����Ƭ��-2:�ʤ�Ǥ�)�ʻ�No.*/
wchar_t		*fzkchar;	/* ��ü����°��ʸ���� */
int		endvect;	/* ��ü�٥��ȥ� */
int		endvect1;	/* ��ü�٥��ȥ� */
register int	nmax;
struct	BZD	**rbzd;
 /* struct BZD *p; p = NULL; �Ȥ��� &p ��Ȥ�����������ʸ������
    �ꥹ�Ȥ����� */
{
     /* ��ʸ�����ꥹ�ȤΥ����ȥݥ��� */
    int			tmp;
    struct		SYO_BNSETSU	*db_set;
    register struct	SYO_BNSETSU	**sb_que_head;
    struct		BZD		*bzd = 0;
    int			dbncnt = 0;
    int			setflg;
    register struct	SYO_BNSETSU	*sb_one;
    struct		SYO_BNSETSU	*sb_set;
    register struct	SYO_BNSETSU	**sb_que_newcomer;
    extern struct	SYO_BNSETSU	*que_reorder();
    int			divid;
    int			edagari_hyouka_sbn;

    edagari_hyouka = _MININT;
    edagari_hyouka_sbn = _MININT;
    yomi_sno_tmp = yomi_sno;

    sb_que_head = &db_set;
    *sb_que_head = NULL;
    if((tmp = sbn_kai(yomi_sno, yomi_eno, endvect, endvect1, sb_que_head, 1,
			(struct SYO_BNSETSU *)0)) <= 0)
	return(tmp);
    for (sb_one = *sb_que_head; sb_one != 0; sb_one = sb_one->lnk_br)
	sb_one->son_v = sum_hyouka(sb_one);

    while (*sb_que_head != NULL) {
	sb_one = *sb_que_head;
	*sb_que_head = sb_one->lnk_br;
	sb_one->son_v = sum_hyouka(sb_one);
	setflg = 0;
	if (yomi_eno <= sb_one->j_c+1) {
	    divid = get_status(sb_one->kangovect, beginvect, fzkchar, &sb_one->status);
	    if (beginvect != WNN_ALL_HINSI && bzd != 0 &&
		bzd->j_c == sb_one->j_c &&
		(sb_one->status == WNN_SENTOU && bzd->sbn->status != WNN_SENTOU ||
		 sb_one->status == WNN_CONNECT && bzd->sbn->status != WNN_CONNECT))
		bzd->v_jc = 0;
	    if ((tmp = set_daibnsetu(rbzd, &bzd, sb_one, &setflg, divid)) < 0)
		return (tmp);
	    dbncnt += tmp;
	    if (setflg == 0)
		clr_sbn_node(sb_one);
	} else {
		if (kan_ckvt(sentou_no, sb_one->kangovect) == WNN_CONNECT_BK) {
		    divid = 0;
		    sb_one->status = WNN_SENTOU;
		} else {
/*		    divid = 2; */
		    divid = -1;	/* KURI */
		    sb_one->status = WNN_GIJI;
		}
		if ((tmp = set_daibnsetu(rbzd, &bzd, sb_one, &setflg, divid)) < 0)
		    return (tmp);
		dbncnt += tmp;

	    if (nmax > sb_one->kbcnt) {
/****************************/
	    if (_DIVID(edagari_hyouka_sbn, 3) < sb_one->son_v) {
		if(edagari_hyouka_sbn < sb_one->son_v) {
		    edagari_hyouka_sbn = sb_one->son_v;
		}
/****************************/
		sb_que_newcomer = &sb_set;
		*sb_que_newcomer = NULL;
		if ((tmp = sbn_kai(sb_one->j_c+1, yomi_eno, sb_one->kangovect,
				WNN_VECT_NO, sb_que_newcomer, sb_one->kbcnt +1,
				sb_one)) < 0)
		    return (tmp);	/* ERROR */
/****************************/
	    } else {
		tmp = 0;
	    }
/****************************/
		if (tmp > 0) {
		    sb_one->reference += tmp;
		    for (sb_one = *sb_que_newcomer; sb_one != 0;
			     sb_one = sb_one->lnk_br)
			sb_one->son_v = sum_hyouka(sb_one);
		    if (*sb_que_head != 0)
			*sb_que_head = que_reorder(*sb_que_head, *sb_que_newcomer);
		    else
			*sb_que_head = *sb_que_newcomer;
		} else {
		    if (setflg == 0)
			clr_sbn_node(sb_one);
		}
	    } else {
		if (setflg == 0)
		    clr_sbn_node(sb_one);
	    }
	}
    }
/**********/
{
    struct BZD *bzd_sv;
    bzd = *rbzd;
    while (dbncnt > 0 && bzd->v_jc < _DIVID(edagari_hyouka, 2)) {
	*rbzd = bzd->lnk_br;
	bzd->lnk_br = 0;
	clr_node(bzd);
	bzd = *rbzd;
        dbncnt--;
    }
    for (; bzd->lnk_br != 0; bzd = bzd->lnk_br) {
	if (bzd->lnk_br->v_jc < _DIVID(edagari_hyouka, 2)) {
	     bzd_sv = bzd->lnk_br->lnk_br;
	     bzd->lnk_br->lnk_br = 0;
	     clr_node(bzd->lnk_br);
	     bzd->lnk_br = bzd_sv;
	     dbncnt--;
	}
    }
}
/**********/
    return (dbncnt);
}

/* ��(beginvect,fzkchar)����³�Ǥ��뤫Ĵ�١�
	��³�Ǥ���Ȥ�		1
	��ʸ�����Ƭ�λ�	0
	��³�Ǥ��ʤ��Ȥ�	-1
   ���֤�
	get_jkt_status ����
*/
int
get_status(kangovect, beginvect, fzkchar, status)
register int	kangovect;
int	beginvect;
wchar_t	*fzkchar;
register short *status;
{
    if (zentan_able(kangovect, beginvect, fzkchar) == YES) {
	_status = 2;
	if (beginvect == WNN_ALL_HINSI) {
	    if (kan_ckvt(sentou_no, kangovect) == WNN_CONNECT_BK) {
		*status = WNN_SENTOU;
/*
	    } else if (jentptr == 0) {
		*status = WNN_GIJI;
		return (-1);
*/
	    } else {
		*status = WNN_NOT_CONNECT;
		return (-1);
	    }
	} else if (beginvect == WNN_BUN_SENTOU) {
		    *status = WNN_SENTOU;
	} else {
		if ((fzkchar == NULL || *fzkchar == NULL) &&
		    beginvect == sentou_no) {
			*status = WNN_SENTOU;
		} else {
			*status = WNN_CONNECT;
			return (1);
		}
	}
    } else if (_status < 2 && kan_ckvt(sentou_no, kangovect) == WNN_CONNECT_BK) {
	_status = 1;
	*status = WNN_SENTOU;
    } else {
/*
	if (jentptr == 0)
	    *status = WNN_GIJI;
	else
*/
	    *status = WNN_NOT_CONNECT;
	return (-1);
    }
    return (0);
}

/* ʸ����Ƭ�ˤʤ�뤫 */
/* ��ü�٥����Υ����å� */
zentan_able(v, hinsi, fzkchar)
int	 v;
register int	hinsi;	/* ��ü�٥���(-1:ʸ����Ƭ��-2:�ʤ�Ǥ�)�ʻ�No.*/
wchar_t		*fzkchar;
{
    register int	ll;
    unsigned short	*buf;
    struct	ICHBNP	*ichbnpbp;
    wchar_t		*fzk_buf;
    int		fzkcnt;
    int		fzklen;
    wchar_t		*rev_fzk();

    if (hinsi == WNN_ALL_HINSI) {
	return (YES);
    } else if (hinsi == WNN_BUN_SENTOU)  {
	return (kan_ckvt(sentou_no, v));
    } else {
	if (fzkchar == NULL || *fzkchar == NULL) {
	    if ((ll = wnn_get_fukugou_component_body(hinsi, &buf)) == 0) {
		    error1("wnn_get_fukugou_component:erroe in zentan_able.");
		return (-1);
	    }
	    for ( ; ll > 0; ll--) {
		if (kan_ckvt(*buf, v) == WNN_CONNECT_BK)
		    return (YES);
		buf++;
	    }
	} else {
	    fzklen = Strlen(fzkchar);
	    fzk_buf = rev_fzk(fzkchar, fzklen);
	    if (fzk_buf == 0)
		    return(NO);	/* ���������� */

	    fzkcnt = fzk_kai(fzk_buf, fzk_buf + fzklen, v, WNN_VECT_NO, &ichbnpbp);
	    if ((fzkcnt <= 0) || (getfzkoh(ichbnpbp, fzkcnt-1)->offset != fzklen))
		    return(NO);
	    for (ll = wnn_get_fukugou_component_body(hinsi, &buf); ll > 0; ll--) {
		if (kan_ck_vector(*buf, getfzkoh(ichbnpbp, fzkcnt - 1)->vector) == WNN_CONNECT_BK) {
		    freeibsp(ichbnpbp);
		    return (YES);
		}
		buf++;
	    }
	    freeibsp(ichbnpbp);
	}
    }
    return(NO);
}

wchar_t *
rev_fzk(fzkchar, len)
register wchar_t	*fzkchar;
int			len;
{
    static	wchar_t	*fzk = 0;
    static	int	fzk_len = 0;

    if (fzk_len < len) {
	if(fzk != 0)
	    free(fzk);
	if ((fzk = (wchar_t *)malloc((len+1)*sizeof(wchar_t))) == 0) {
	    wnn_errorno = WNN_MALLOC_ERR;
	    fzk_len = 0;
	    return (fzk);
	}
	fzk_len = len;
    }

    (void)Sreverse(fzk, fzkchar);
    return (fzk);
}

/* ��ʸ����¤��ؤ� Ĺ���ȥ٥����ǥ����Ȥ��� */
/* que ����Ƭ���֤� */
/* Ĺ����٥�����Ʊ���ʤ�ɾ���ͤι⤤�������ˤ��� */
struct	SYO_BNSETSU *
que_reorder(que, new)
register struct	SYO_BNSETSU	*que, *new;
{
    struct		SYO_BNSETSU	*que_sv;
    register struct	SYO_BNSETSU	*q;
    register struct	SYO_BNSETSU	*tmp;
    register struct	SYO_BNSETSU	*next;
    int	flg;

    if (new == 0)
	return(que);
    if ((flg = sbjunjo(que, new)) < 0) {
	if (flg == -2) {
	    if (cmp_hyouka(new, que) > 0) {
		tmp = que->lnk_br;
		next = new->lnk_br;
		clr_sbn_node(que);
		que = new;
		que->lnk_br = tmp;
	    } else {
		next = new->lnk_br;
		clr_sbn_node(new);
	    }
	    que_sv = que;
	    new = next;
	} else
	    que_sv = new;
    } else
	que_sv = que;

    while(new != NULL){
	next = new->lnk_br;
	if ((flg = sbjunjo(que, new)) < 0) {
	    if (flg == -2) {
		if (cmp_hyouka(new, que) > 0) {
		    for (q = que_sv; q->lnk_br != que; q = q->lnk_br);
		    tmp = que->lnk_br;
		    clr_sbn_node(que);
		    que = q->lnk_br = new;
		    que->lnk_br = tmp;
		} else {
		    clr_sbn_node(new);
		}
	    } else {
		tmp = que;
		que = new;
		que->lnk_br = tmp;
	    }
	    new = next;
	    continue;
	}
	while(((flg = sbjunjo(que, new)) > 0) && (que)->lnk_br != NULL)
	    que = (que->lnk_br);
	tmp = que->lnk_br;
	if (flg == -2) {
	    if (cmp_hyouka(new, que) > 0) {
		for (q = que_sv; q->lnk_br != que; q = q->lnk_br);
		clr_sbn_node(que);
		que = q->lnk_br = new;
		    que->lnk_br = tmp;
	    } else {
		clr_sbn_node(new);
	    }
	} else {
	    que->lnk_br = new;
	    new->lnk_br = tmp;
	}
	new = next;
    }
    return (que_sv);
}

/* que �� new �ν��
	1: que ����
	0: que �θ�� new
	-1:new ����
	-2:Ʊ���� */
sbjunjo(que, new)
register struct	SYO_BNSETSU	*que, *new;
{
    if (new == 0)
	return (1);
    if (que->j_c > new->j_c)
	return (-1);
    if (que->j_c < new->j_c) {
	if (que->lnk_br == 0)
	    return (0);
	if (que->lnk_br->j_c > new->j_c)
	    return (0);
	if (que->lnk_br->j_c < new->j_c)
	    return (1);
	if (que->lnk_br->kangovect > new->kangovect)
	    return (0);
	return (1);
    }
    if (que->kangovect == new->kangovect)
	return (-2);
    if (que->kangovect > new->kangovect)
	return (-1);
    if (que->lnk_br == 0)
	return (0);
    if (que->lnk_br->j_c > new->j_c)
	return (0);
    if (que->lnk_br->kangovect > new->kangovect)
	return (0);
    return (1);
}


/* ʸ�����Ƭ�ˤʤ��С���ʸ��θ���򥻥åȤ���
   bzd �˾�ʸ����ɲä����ʤ� 1 �ɲä��ʤ���� 0 ���֤��� */
set_daibnsetu(rbzd, bzd, sbn, setflg, divid)
struct		BZD		**rbzd;
register struct	BZD		**bzd;
register struct	SYO_BNSETSU	*sbn;
int				*setflg;
int				divid;
{
    register int	ret;
    int hyouka;

    hyouka = DIVID_HYOUKA(ave_hyouka(sbn), divid);
    if(hyouka > edagari_hyouka)
	edagari_hyouka = hyouka;
    else if(hyouka < _DIVID(edagari_hyouka, 2))
	return (0);

    if (*bzd != 0) {
	if ((*bzd)->j_c == sbn->j_c) {
	    /* Ʊ��Ĺ�� */
	    if ((*bzd)->v_jc >= hyouka)
		return (0);
	    clr_sbn_node((*bzd)->sbn);
	    ret = 0;
	} else {
	    if (((*bzd)->lnk_br = getbzdsp()) == 0)
		return(-1);
	    (*bzd) = (*bzd)->lnk_br;
		ret = 1;
	}
    } else {
	if ((*rbzd = *bzd = getbzdsp()) == 0)
	    return(-1);
	ret = 1;
    }
    (*bzd)->v_jc = hyouka;
    (*bzd)->j_c = sbn->j_c;
    (*bzd)->sbn_cnt = sbn->kbcnt;
    (*bzd)->lnk_br = 0;
    (*bzd)->lnk_son = 0;
    (*bzd)->son_v = 0;
    (*bzd)->sbn = sbn;
    (*bzd)->kbcnt = 1;
    sbn->reference++;
    (*bzd)->bend_m = yomi_sno_tmp;
    *setflg = 1;
    return (ret);
}

/* ��ʸ���ɾ���ؿ� �Ȥꤢ���� */

/* sbn �˴ޤޤ�뾮ʸ���ɾ���ͤι�� */
sum_hyouka(sbn)
register struct	SYO_BNSETSU	*sbn;
{
    return(sbn->v_jc + (sbn->parent ? sbn->parent->son_v : 0));
}

ave_hyouka(sbn)
register struct	SYO_BNSETSU	*sbn;
{
    register int	len;		/* ��ʸ��Ĺ */

    len = sbn->j_c - yomi_sno_tmp + 1;
    return (hyoka_dbn(sbn->son_v, sbn->kbcnt, len));
}

/* ��ʸ���ɾ����
	Ĺ����Ʊ���ǡ���ü�٥�����Ʊ��2�Ĥ���ʸ��Τɤ����
	���֤����뤿��˻Ȥ�
	��ʸ��Υꥹ�� sbn1 �������⤤�������ͤ��֤�
 */
cmp_hyouka(sbn1, sbn2)
register struct	SYO_BNSETSU	*sbn1;
register struct	SYO_BNSETSU	*sbn2;
{
    /* ��ꤢ������ʸ���ɾ���ͤϡ���ʸ���ɾ���ͤ�ʿ�Ѥȹͤ��Ƥ��� */
    return (ave_hyouka(sbn1) - ave_hyouka(sbn2));
}
