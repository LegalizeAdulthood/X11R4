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
 *	小文節解析				*
 ************************************************/
#include <stdio.h>
#include <ctype.h>
#include "commonhd.h"
#include "ddefine.h"
#include "de_header.h"
#include "jdata.h"

#include "fzk.h"
#include "kaiseki.h"

sbn_kai(yomi_sno, yomi_eno, endvect, endvect1, tsbnptr, bnst_num, parent)
int		yomi_sno;
int		yomi_eno;
int		endvect;	/* 終端ベクタ */
int		endvect1;	/* 終端ベクタ */
struct SYO_BNSETSU **tsbnptr;	/* 小文節解析結果 */
int		bnst_num;	/* これまでに解析した小文節数 */
struct SYO_BNSETSU *parent;	/* 親の幹語ノード */
{
    int	fzkcnt, hyokaval, hyokaval0, hinsidt, sbncnt;
    unsigned short	*hinsi_buf;
	     int	hinsidt_fk;	/* 複合品詞のセーブ */
    register int	i,		/* 幹語の終わりのインデックス */
			j,		/* 文節始めのインデックス */
			t,
			ii;
    struct		jdata		*jentptr;
    register struct	SYO_BNSETSU	*sbnptr;
    struct		SYO_BNSETSU	*sbnptr_top = 0;
    struct		ICHBNP		*ichbnpbp;
    int			k, ll;
    struct		SYO_BNSETSU	*kanptr;
    struct		SYO_BNSETSU	*kanptr_tmp;
    struct		SYO_BNSETSU	*kanptr_before;
    struct		SYO_BNSETSU	*giji_sbn;
    int			get_giji_flg = -1; /* 擬似文節を作れたか */

    enum	FLG {
		set,
		noset
    } setflg;
    enum	FLG	kanflg;	/* 同じベクタの品詞があったか否か */
    enum	GFLG {
		get,
		noget
    } getflg;
    struct	SYO_BNSETSU	*getsbnsp();
    int		kangovect;
    int		connect_flg;

    if ((yomi_sno == yomi_eno) ||
        (fzk_ckvt(endvect) == NO && fzk_ckvt(endvect1) == NO))
	return (NOTHING);

    if ((fzkcnt = fzk_kai(
		&bun[yomi_sno], &bun[yomi_eno], endvect, endvect1, &ichbnpbp)) <= 0)
	return (fzkcnt);	/* ERROR */
    for (ii = 0; ii < fzkcnt; ii++) 
	getfzkoh(ichbnpbp, ii)->offset += yomi_sno;

    for (ii = 0; ii < fzkcnt; ii++) {
	i = getfzkoh(ichbnpbp, ii)->offset;
	if (jmtp[i] == (struct jdata **) UN_KNOWN) { /* もう引いた? */
	    if(jmt_set(i) == -1){	/* 辞書引き */
		return (-1);
	    }
	}
    }

    j = j_max(ichbnpbp, fzkcnt);
    j = (j >= yomi_eno) ? yomi_eno - 1 : j;

    setflg = noset;
    if ((giji_sbn = getsbnsp()) == NO)
	return (-1);	/* ERROR */
    giji_sbn->kbcnt = bnst_num;
    giji_sbn->bend_m = yomi_sno;
    giji_sbn->parent = parent;

    /* 疑似文節を先に取り出します(もしあれば) */
    if (getgiji(yomi_sno, yomi_eno, giji_sbn, ichbnpbp, fzkcnt, bnst_num) >= 0) {
	sbncnt = 1;
	getflg = get;
        get_giji_flg = giji_sbn->j_c;
	if (giji_sbn->j_c > j) {
	    sbnptr_top = giji_sbn;
	    get_giji_flg = -1;
	    kanptr_tmp = giji_sbn->lnk_br;
	}
    } else {
	sbnptr = giji_sbn;
        sbncnt = 0;
	getflg = noget;
    }
 
    for (; j >= yomi_sno; j--) {	/* 文節の長さ */
	if ((setflg == noset) && (get_giji_flg == j)) {
	    giji_sbn->lnk_br = sbnptr_top;	
	    sbnptr_top = giji_sbn;
	    setflg = set;
	    kanptr_tmp = giji_sbn->lnk_br;
	}
	for (ii = 0; (ii < fzkcnt) &&
		     ((i = (getfzkoh(ichbnpbp, ii)->offset)) <= j);
		     ii++) {		/* 付属語 */
	    jentptr = (struct jdata *) C(i, j);
	    for (; jentptr; jentptr = jentptr->jptr) { /* 辞書 */
		for (t = 0; t < (jentptr->kosuu); t++) { /* 幹語 */
		    if((jentptr->hindo[t] == 0x7f) ||
		       (jentptr->hindo_in && jentptr->hindo_in[t] == 0x7f))
			continue; /*コメントアウトされた辞書エントリ */
		    hyokaval = hyoka1(jentptr->hindo[t],
			(jentptr->hindo_in == 0 ? 0 : jentptr->hindo_in[t]),
			j - i + 1, j - yomi_sno + 1,
			dic_table[jentptr->jishono].nice);
		    if (bnst_num != 1) {
			if (parent != 0 && parent->status_bkwd == NO)
			    /* 前の文節が endvect に接続できないものは、
			       評価値を下げる */
			    hyokaval0 = _DIVID(hyokaval, 2);
			else
			    hyokaval0 = hyokaval;
		    }
		    hinsidt_fk = hinsidt = jentptr->hinsi[t];
		    if ((ll = wnn_get_fukugou_component_body(hinsidt_fk, &hinsi_buf)) == 0) {
			error1("wnn_get_fukugou_component:erroe in bunsetukai.");
			return (-1);
		    }
		    for ( ; ll > 0; ll--) {
		    hinsidt = *hinsi_buf;
		    hinsi_buf++;
		    /* ここから後は単純品詞だけ考えればいい */
			connect_flg = kan_ck_vector(hinsidt,
					(getfzkoh(ichbnpbp, ii)->vector));
			if (
			    (connect_flg == WNN_CONNECT_BK ||
			     bnst_num == 1 &&
			     kan_ck_vector(hinsidt,
					 (getfzkoh1(ichbnpbp, ii)->vector)) ==
					    WNN_CONNECT_BK)) {
			/* 接続できる */
			    if (bnst_num == 1)
				/* endvect に接続できないものは評価値を下げる */
				hyokaval0 = (connect_flg != WNN_NOT_CONNECT_BK) ?
						hyokaval: _DIVID(hyokaval, 2);
			    kangovect = ft->kango_hinsi_area[hinsidt];
			    if (getflg == get) {
				getflg = noget;
				if(0 == (sbnptr = getsbnsp()))
				    return (-1);	/* ERROR */
				sbnptr->kbcnt = bnst_num;
				sbnptr->bend_m = yomi_sno;
				sbnptr->parent = parent;
			    }
			    sbnptr->j_c = j;

			    if (setflg == noset) { /* この長さの文節は初めて */
			        sbnptr->lnk_br = sbnptr_top;	
			        sbnptr_top = sbnptr;
				setflg = set;
				sbnptr->v_jc = hyokaval0;
				sbnptr->t_jc = t;
				sbnptr->jentptr = jentptr;
				sbnptr->hinsi_fk = hinsidt_fk;
				sbnptr->kangovect = kangovect;
				sbnptr->i_jc = i;
				/* yoku wakaran 9/8
				if (endvect1 == WNN_VECT_NO)
				    sbnptr->status_bkwd = WNN_NOT_CONNECT_BK;
				else
				*/
				    sbnptr->status_bkwd = connect_flg;
				kanptr_tmp = sbnptr->lnk_br;
				getflg = get;
				sbncnt++;
			    } else {
				kanflg = noset;
				for (kanptr_before = kanptr = sbnptr_top;
				     kanptr != kanptr_tmp;
				     kanptr_before = kanptr,
				     kanptr = kanptr->lnk_br) {
				    if (kanptr->kangovect == kangovect) {
				    /* 同じベクタの品詞があった  */
					if (hyokaval0 > kanptr->v_jc) {
					/* こっちの方が評価値が高い */
					    kanptr->v_jc = hyokaval0;
					    kanptr->t_jc = t;
					    kanptr->jentptr = jentptr;
					    kanptr->hinsi_fk = hinsidt_fk;
					    kanptr->i_jc = i;
					    /* yoku wakaran 9/8
					    if (endvect1 == WNN_VECT_NO)
						kanptr->status_bkwd = WNN_NOT_CONNECT_BK;
					    else
					    */
						kanptr->status_bkwd = connect_flg;
					}
					kanflg = set;
					break;
				    } else if (kanptr->kangovect > kangovect) {
				    /* 同じベクタの品詞がなかった */
					break;
				    }
				} /* SORT & INSERT SYO_BN */

				if (kanflg == noset) {
				/* 同じベクタの品詞がなかった */
				    if (kanptr == sbnptr_top) {
					sbnptr->lnk_br = kanptr;
					sbnptr_top = sbnptr;
				    } else {
					sbnptr->lnk_br = kanptr;
					kanptr_before->lnk_br = sbnptr;
				    }
				    sbnptr->v_jc = hyokaval0;
				    sbnptr->t_jc = t;
				    sbnptr->jentptr = jentptr;
				    sbnptr->hinsi_fk = hinsidt_fk;
				    sbnptr->kangovect = kangovect;
				    sbnptr->i_jc = i;
				    /* yoku wakaran 9/8
				    if (endvect1 == WNN_VECT_NO)
					sbnptr->status_bkwd = WNN_NOT_CONNECT_BK;
				    else
				    */
					sbnptr->status_bkwd = connect_flg;
				    sbncnt++;
				    getflg = get;
			        }
			    }
			} /* 接続できる */
		    } /* 単純品詞 */
		} /* 幹語 */
	    } /* 辞書 */
	} /* 付属語 */
	setflg = noset;	/* 次の長さの漢語はまだsetして無い */
    } /* 文節の長さ */
    if (sbncnt == NOTHING) { /* 幹語がなかった */
	for (k = fzkcnt - 1; k >= 0; k--) {
	    if (kan_ck_vector(katakanago_no, getfzkoh(ichbnpbp, k)->vector) ==
			WNN_CONNECT_BK) {
		k = getgiji(yomi_sno, yomi_eno, sbnptr, ichbnpbp, k + 1, bnst_num);
		break;
	    }
	}
	if (k < 0) {
	    if (bnst_num == 1) {
		/* 何が何でも疑似文節を作るんだい */
		getgiji_f(yomi_sno, sbnptr);
		sbncnt = 1;
		sbnptr_top = sbnptr;
#ifdef	nodef
		wnn_errorno = WNN_NO_KOUHO;
		error1("Cannot get bunsetsu kouho in bunsetu-kaiseki.\n");
#endif	nodef
	    } else {
		freesbn(sbnptr);
		sbncnt = 0;
		sbnptr_top = 0;
	    }
	} else {
	    sbncnt = 1;
	    sbnptr_top = sbnptr;
	}
    } else if (getflg == noget) {
	freesbn(sbnptr);
    }
    *tsbnptr = sbnptr_top;	/* 短いものから並べる */
    freeibsp(ichbnpbp);
    return (sbncnt);
}

/*******************************************/
/*	max value in maxj[] search routine */
/*******************************************/
int
j_max(fzkbnp, fzkcnt)
register struct	ICHBNP	*fzkbnp;
int		fzkcnt;		/* max number of fzkkouho table */
{
    register int	i, mxj = -1, k, ofst;

    for (i = 0; i < fzkcnt; i++) {
	if ((ofst = getfzkoh(fzkbnp, i)->offset) < maxchg)
	    if ((k = maxj[ofst]) > mxj)
		mxj = k;
    }
    return (mxj);
}

/**********************************************/
/*	疑似幹語の獲得			      */
/**********************************************/
int
getgiji(yomi_sno, yomi_eno, sbnptr, fzkbnp, fzkcnt, bnst_num)
int 			yomi_sno;
int 			yomi_eno;
struct	SYO_BNSETSU	*sbnptr;
struct	ICHBNP		*fzkbnp;
int			fzkcnt;
int			bnst_num;
{
	register int	j_end;
	int	j_end_sv;
	int	*vector;
	int	*vector1;
	int	giji_hinsi;
	int	giji_hindo;
	int	len = 0;
	wchar_t	c;

	j_end = getfzkoh(fzkbnp, fzkcnt - 1)->offset;
	vector = getfzkoh(fzkbnp, fzkcnt - 1)->vector;
	vector1 = getfzkoh1(fzkbnp, fzkcnt - 1)->vector;

	if (j_end >= yomi_eno) {	/* 付属語だけの場合 */
	    giji_hinsi = fuzokugo_no;
	    giji_hindo = c_env->fuzokogo_val;
	    len = 0;
	    goto	_Only_Fuzokugo;
	}
/*#ifdef	not_use*/
	if (isjhira(bun[j_end]))	/* ひらがなならバイバイ */
		return (-1);
/*#endif*/
	for (j_end_sv = j_end; j_end < yomi_eno; j_end++) {
	    /* スペースは、その前の疑似文節にくっつける */
	    if (!(isspace(c = bun[j_end])) && !(isjspace(c))) {
		if (TOKUSYU(c) || check_bar_katakana(j_end, yomi_eno) ||
		    isjhira(c) || isjkanji(c)) { /* 片仮名 ひらがな 漢字 */
		    j_end = j_end_sv;
		}
		break;
	    }
	}

	if (j_end >= yomi_eno) {	/* スペースだけの場合 */
	    giji_hinsi = fuzokugo_no;
	    giji_hindo = c_env->fuzokogo_val;
	    len = 0;
	    goto	_Only_Fuzokugo;
	}

/* 分類 */
	if (isascii(c = bun[j_end]) && isdigit(c) || isjdigit(c)) { /* 数字 */
	    giji_hinsi = suuji_no;
	    giji_hindo = c_env->suuji_val;
	} else if (isascii(c) && isparen_e(c) || isjparen_e(c)) { /* 閉括弧 */
	    giji_hinsi = toji_kakko_no;
	    giji_hindo = c_env->toji_kakko_val;
	} else if (TOKUSYU(c) || check_bar_katakana(j_end, yomi_eno)) { /* 片仮名 */
	    giji_hinsi = katakanago_no;
	    giji_hindo = c_env->kana_val;
	} else if (is_g_eisuu(c)) { /* アスキー(アルファベット, 数字) */
	    giji_hinsi = eisuu_no;
	    giji_hindo = c_env->eisuu_val;
	} else if (isascii(c) && isparen_s(c) || isjparen_s(c)) { /* 開括弧 */
	    giji_hinsi = kai_kakko_no;
	    giji_hindo = c_env->kaikakko_val;
#ifdef	nodef
	} else if (isjhira(c)) { /* ひらがな */
	    giji_hinsi = giji_no;
	    /*
	    giji_hindo = c_env->kana_val;
	    */
	    giji_hindo = 0;
#endif
	} else { /* 記号、漢字 */
	    giji_hinsi = kigou_no;
	    giji_hindo = c_env->kigou_val;
	}

/* 接続可能か */
	if (sbnptr->status_bkwd = kan_ck_vector(giji_hinsi, vector) == WNN_CONNECT_BK)
	    goto _Can_connect;
#ifdef	nodef
	/* 数字に接続できないが,アスキには接続できる */
	if (giji_hinsi == suuji_no && kan_ck_vector(eisuu_no, vector) == WNN_CONNECT_BK) {
	    giji_hinsi = eisuu_no;
	    giji_hindo = c_env->eisuu_val;
	    goto _Can_connect;
	}
#endif	nodef
	if (bnst_num != 1)
		return (-1);
	if (kan_ck_vector(giji_hinsi, vector1) == WNN_NOT_CONNECT_BK) {
	    if (giji_hinsi != suuji_no || kan_ck_vector(eisuu_no, vector1) == WNN_NOT_CONNECT_BK)
		return (-1);
	    else {
		giji_hinsi = eisuu_no;
		giji_hindo = c_env->eisuu_val;
	    }
	}

_Can_connect:

/* 長さを調べる */
	if (giji_hinsi == suuji_no) {
	    for ( ; (j_end + len < yomi_eno) &&
		    (isascii(bun[j_end + len]) && isdigit(bun[j_end + len]) ||
		     isjdigit(bun[j_end + len])); len++)
		;
	    if ((j_end + len < yomi_eno) && is_g_eisuu(bun[j_end + len])) {
	    /* 英数 */
		for (len++;
		     (j_end + len < yomi_eno) && is_g_eisuu(bun[j_end + len]);
		     len++)
			;
		if (kan_ck_vector(eisuu_no, vector) == WNN_NOT_CONNECT_BK &&
		    (bnst_num != 1 || kan_ck_vector(eisuu_no, vector1)))
			return (-1);
		giji_hinsi = eisuu_no;
	    /* 半角数字 */ /*
	    } else {
		if (kan_ck_vector(suuji_no, vector) == WNN_NOT_CONNECT_BK)
		    return (-1);
	    */
	    }
	} else if (giji_hinsi == eisuu_no) {
	    for ( ; ((j_end + len) < yomi_eno) && is_g_eisuu(bun[j_end + len]);
		 len++)
		    ;
	} else if (giji_hinsi == toji_kakko_no) { /* 閉括弧 */
	    len = 1;
	} else if (giji_hinsi == kigou_no) { /* 記号 */
	    len = 1;
	} else if (giji_hinsi == kai_kakko_no) { /* 開括弧 */
	    len = 1;
	} else if (giji_hinsi == giji_no) {
	    len = 1;
	} else if (giji_hinsi == katakanago_no) {
	    /* カタカナ 半角カタカナ 濁点 半濁点*/
	    for (len = 1; (j_end + len < yomi_eno) &&
		(TOKUSYU(bun[j_end + len]) || 
		 check_bar_katakana(j_end + len, yomi_eno)); len++)
		    ;
	} else {
	    if (j_end >= yomi_eno)
		    len = 0;
	    else
		    len = 1;
	}

	/* スペースは、そのあとの疑似文節にくっつける */
	if ((giji_hinsi == eisuu_no) || (giji_hinsi == suuji_no)) {
	    for ( ; ((j_end + len) < yomi_eno) &&
		    (isspace(bun[j_end + len]) || (isjspace(c))); len++)
			    ;
	}

_Only_Fuzokugo:
/* 評価値の決定 */
	sbnptr->v_jc = HYOKAVAL(giji_hindo, len, len + j_end - yomi_sno);
	sbnptr->i_jc = j_end;
	if (giji_hinsi == katakanago_no) {
	    sbnptr->t_jc = WNN_KATAKANA;
	} else if (isascii(c)) {
	    if (giji_hinsi == suuji_no)
		sbnptr->t_jc = c_env->giji.number;
	    else if (giji_hinsi == eisuu_no) {
		sbnptr->t_jc = c_env->giji.eisuu;
	    } else if (giji_hinsi == kigou_no ||
		       giji_hinsi == toji_kakko_no ||
		       giji_hinsi == kai_kakko_no) {
		sbnptr->t_jc = c_env->giji.kigou;
	    } else {
		sbnptr->t_jc = -1;
	    }
	    sbnptr->v_jc += 2; /* 次候補よりも評価値を上げるため */
	} else {
	    sbnptr->t_jc = -1;
	}
	sbnptr->hinsi_fk = giji_hinsi;
	sbnptr->jentptr = 0;
	sbnptr->kangovect = ft->kango_hinsi_area[giji_hinsi];
	sbnptr->j_c = j_end + len - 1;
	return (len);
}

is_g_eisuu(c)
register wchar_t c;
{
    register int i;

    if (isascii(c) && isalnum(c) || isjalnum(c))
	return(YES);
    else {
	for (i = 0; i < MAX_GIJI_CHAR; i++) {
	    if (giji_eisuu[i] == 0xffff)
		return (NO);
	    else if (giji_eisuu[i] == c)
		return(YES);
	}
    }
    return(NO);
}

init_giji_eisuu_char()
{
    FILE *fp, *fopen();
    char data[256];
    register int num ;
    char code[256];
    char s[MAX_GIJI_CHAR][16];
    register int i;

    if((fp = fopen(jserverrcfile , "r")) == NULL){
	perror("");
	printf("Error can't open %s\n" , jserverrcfile);
	return(-1);
    }

    while(fgets(data, 256, fp) != NULL){
	num = sscanf(data,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",code, 
	      s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[8],s[9],
	      s[10],s[11],s[12],s[13],s[14],s[15],s[16],s[17],s[18],s[19]);

	if (strcmp(code, "set_giji_eisuu") == 0 && num >= 2){
	    for (i = 0; i < num - 1; i++) {
		giji_eisuu[i] = expand_argument(s[i]);
	    }
	    for ( ; i <= MAX_GIJI_CHAR; i++) {
		giji_eisuu[i] = 0xffff;
	    }
	}
    }
    fclose(fp);

    return(0);
}

expand_argument(st)
register unsigned char *st;
{
  int num;

  if(*st == '^'){
    return ((*(st + 1)) & 0x1f);
  } else if (*st == '\\') {
    return (get_bcksla(st + 1));
  } else if (*st == '0') {
    if (*(st + 1) == 'x' || *(st + 1) == 'X') {
      sscanf(st + 2,"%x",&num);
      return(num);
    } else {
      sscanf(st,"%o",&num);
      return(num);
    }
  } else if(*st == '\'') {
    if (*(st + 1) < 0x80)
	return (*(st + 1));
    else
	return (*(st + 1) * 0x100 + *(st + 2));
  } else if(!('0' <= *st && *st <= '9')){
    return(-1);
  } else {
    return (atoi(st));
  }
}

get_bcksla(st)
register char *st;
{
    int num;

    switch(*st) {
    case 'n':	return('\n');
    case 't':	return('\t');
    case 'b':	return('\b');
    case 'r':	return('\r');
    case 'f':	return('\f');
    case 'x':
    case 'X':
	sscanf(st +1, "%x",&num);
	return(num);
    case 'd':
    case 'D':
	return (atoi(*(st+1)));
    case 'o':
    case 'O':
	sscanf(st +1, "%o",&num);
	return(num);
    }
    return(-1);
}

/*
is_g_digit(c)
wchar_t c;
{
    register int i;

    if (isascii(c) && isdigit(c) || isjdigit(c)) {
	return(YES);
    else {
	for (i = 0; i < MAX_GIJI_CHAR; i++) {
	    if (giji_digit[i] == 0xffff)
		return (NO);
	    else if (giji_digit[i] == c)
		return(YES);
	}
    }
    return(NO);
}
*/

/* 何が何でも疑似文節を作るんだい */
int
getgiji_f(yomi_sno, sbnptr)
register int 			yomi_sno;
register struct	SYO_BNSETSU	*sbnptr;
{
	sbnptr->v_jc = HYOKAVAL(0, 1, 1);
	sbnptr->i_jc = yomi_sno;
	sbnptr->t_jc = -1;
	sbnptr->hinsi_fk = giji_no;
	sbnptr->jentptr = 0;
	sbnptr->kangovect = ft->kango_hinsi_area[kigou_no];
	sbnptr->j_c = yomi_sno;

	return (1);
}

/* カタカナか? 長音記号の前は、カタカナでないとダメ */
check_bar_katakana(i, yomi_eno)
register int i;
register int yomi_eno;
{
	register int j;
	for (j = i; j < yomi_eno; j++) {
		if (isjkata(bun[j]))
			return (YES);
		if (!(BAR_CODE(bun[j])))
			return (NO);
	}
	return (NO);
}

/*
 * kan_ckvt	hinsi の品詞がvectorに接続できるか
 */

kan_ckvt(hinsi, vector)
register unsigned short hinsi;	/* 品詞No. */
register int vector;	/* 付属語前端ベクタ */
{
	register int *v;
	v = (int *) ((struct kangovect *)ft->kango_vect_area + vector);
	return (kan_ck_vector(hinsi, v));
	/*
	return (kan_ck_vector(hinsi,
			((struct kangovect *) ft->kango_vect_area + vector)));
	*/
}

#ifdef	change_macro
kan_ck_vector(hinsi, vector)
unsigned short hinsi;	/* 品詞No. */
int vector[];	/* 付属語前端ベクタ */
{
	register int wvect;

	wvect = vector[hinsi / (sizeof (int) << 3)];/* << 3 == * 8  */
	wvect >>= (hinsi % (sizeof (int) << 3));
	if ((wvect & 0x00000001) == 1)
		return (WNN_CONNECT_BK);
	else
		return (WNN_NOT_CONNECT_BK);
}
#endif	change_macro
