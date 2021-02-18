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
#include "commonhd.h"
#include "jservercf.h"
#include "ddefine.h"
#include "de_header.h"
#include "fzk.h"
#include "kaiseki.h"
#include "wnn_malloc.h"

#define vgetc(pt) ((pt)? getc((pt)):xgetc_cur())
#define vungetc(k, pt) ((pt)? ungetc((k),(pt)):xungetc_cur(k))


static int	fzkvect_kosuu;			/* ��°��٥����� */
static int	kango_vect_kosuu;		/* ����٥����ο� */
static int	kango_length;			/* ����� */

/*
 *	��°��ե�����Υե����ޥå�

	��°��θĿ�		(ft->fzklength)
	��³�٥�����Ĺ��	(fzkvect_l)
	��°��٥����θĿ�	(fzkvect_kosuu)
	����٥�����Ĺ��	(kango_vect_l)		V4.0
	������ʻ��		(kango_length)		V4.0
	����٥����ο�		(kango_vect_kosuu)	V4.0

	��°���ɤ�ʸ����	��°��٥�����
		No.	��³�٥���
		��	��	
	��°���ɤ�ʸ����	��°��٥�����
		No.	��³�٥���
		��	��
		��	��	
	��
		��	��	
	��

	��ü�٥���
	��
	��
	��

	������³�٥���No.	������³�٥���		V4.0
		������³�٥��� kango_vect_l	��°����³�٥��� fzk_vect_l
		[31,30,...,1][63,62,....32]...
	��	��
	��	��
	��

	����No.	������³�٥���No.			V4.0
	��	��
	��	��
	��

	�����ʻ�No. ������³�٥���No. �����ʻ�٥���	(JKTVECT_KOSUU)	V4.0
 */


struct FT *
fzk_read(fp)
register	FILE	*fp;
{
	struct	FT	*fzk_tbl;
	extern	struct	FT	*fzk_ld();

	fzk_tbl = fzk_ld(fp);
	if (fzk_tbl != NULL) {
		if (fp != NULL)
			fclose(fp);
		link_job(fzk_tbl->fzklength, fzk_tbl);
	}
	return (fzk_tbl);
}

struct FT *
fzk_ld(fp)
register FILE	*fp;
{
	extern	void	fzk_discard();
	struct	FT	*fzk_tbl;
	register int	k, l, m;
	register int	vect_count = 0;
	struct fzkentry	*ptr;
	unsigned char	charyomi[(YOMI_L + 1) * 2];
	int	fzklength;
	int	fzkvect_l;
	int	kango_vect_l;

	fzklength = get_decimal(fp);		/* ��°��θĿ� */
	fzkvect_l = get_decimal(fp);		/* ��°��٥�����Ĺ�� */
	fzkvect_kosuu = get_decimal(fp);	/* ��°��٥����θĿ� */
	kango_vect_l = get_decimal(fp);		/* ����٥�����Ĺ�� */
	kango_vect_kosuu = get_decimal(fp);	/* ����٥����θĿ� */
	kango_length = get_decimal(fp);		/* ������ʻ�� */

	if (fzkvect_l > VECT_L) {
		wnn_errorno = WNN_FZK_TOO_DEF;
		error1( "Sorry, your fuzokugo bit data has too many id.\n");
		error1( "Please change define VECT_L %d and compile again\n",
			fzkvect_l);
		return(NULL);
	}

	if (kango_vect_l > KANGO_VECT_L) {
		wnn_errorno = WNN_FZK_TOO_DEF;
		error1( "Sorry, your kango bit data has too many id.\n");
		error1( "Please change define KANGO_VECT_L %d and compile again\n",
			kango_vect_l);
		return(NULL);
	}
	if (kango_vect_kosuu > KANGO_VECT_KOSUU) {
		wnn_errorno = WNN_FZK_TOO_DEF;
		error1( "Sorry, your kango bit vector has too many id.\n");
		error1( "Please change define KANGO_VECT_KOSUU %d and compile again\n",
			kango_vect_kosuu);
		return(NULL);
	}

	if (kango_length > KANGO_HINSI_MX) {
		wnn_errorno = WNN_FZK_TOO_DEF;
		error1( "Sorry, your kango bit data has too many id.\n");
		error1( "Please change define KANGO_MX %d and compile again\n",
			kango_length);
		return(NULL);
	}
	if (kango_vect_kosuu > kango_length) {
		wnn_errorno = WNN_FZK_TOO_DEF;
		error1( "KANGO vector kosuu is more than KANGO hinsi suu\n");
		return(NULL);
	}
	if ((fzk_tbl = (struct FT *)malloc(sizeof(struct FT))) == NULL) {
		wnn_errorno = WNN_MALLOC_ERR;
		error1( "malloc error in fzk\n");
		return(NULL);
	}
	fzk_tbl->vect_area = NULL;
	fzk_tbl->kango_hinsi_area = NULL;
	fzk_tbl->kango_vect_area = NULL;
	fzk_tbl->tablefuzokugo = NULL;
	fzk_tbl->fzklength = fzklength;
	fzk_tbl->fzkvect_l = fzkvect_l;
	fzk_tbl->kango_vect_l = kango_vect_l;

	if ((fzk_tbl->vect_area =
	    (struct fzkvect *) malloc(sizeof (struct fzkvect) * fzkvect_kosuu)) == NULL) {
		wnn_errorno = WNN_MALLOC_ERR;
		error1( "malloc error in fzk\n");
		fzk_discard(fzk_tbl);
		return(NULL);
	}

	if ((fzk_tbl->kango_hinsi_area =
	    (int *) malloc(sizeof (int) * kango_length)) == NULL) {
		wnn_errorno = WNN_MALLOC_ERR;
		error1( "malloc error in fzk\n");
		fzk_discard(fzk_tbl);
		return(NULL);
	}
	if ((fzk_tbl->kango_vect_area =
	    (struct kangovect *) malloc(sizeof (struct kangovect) * (kango_vect_kosuu + SV_KOSUU))) == NULL) {
		wnn_errorno = WNN_MALLOC_ERR;
		error1( "malloc error in fzk\n");
		fzk_discard(fzk_tbl);
		return(NULL);
	}

	if ((fzk_tbl->tablefuzokugo =
	    (struct fzkentry *) malloc(sizeof (struct fzkentry) * fzk_tbl->fzklength)) == NULL) {
		wnn_errorno = WNN_MALLOC_ERR;
		error1( "malloc error in fzk\n");
		fzk_discard(fzk_tbl);
		return(NULL);
	}
	ptr = fzk_tbl->tablefuzokugo;

	for (m = 0; m < fzk_tbl->fzklength; m++, ptr++) {
		get_string(fp, charyomi);
		(void) Sstrcpy(ptr->yomi, charyomi);
		ptr->yomi_su = Strlen(ptr->yomi);
		ptr->kosu = get_decimal(fp);
		ptr->pter = &fzk_tbl->vect_area[vect_count];
		ptr->link = NULL;
		for (l = 0; l < ptr->kosu; l++, vect_count++) {
			fzk_tbl->vect_area[vect_count].no = get_decimal(fp);
			for (k = 0; k < fzkvect_l; k++) {
				fzk_tbl->vect_area[vect_count].vector[k] = get_hexsa(fp);
			}
		}
	}
/* ��ü vector �� SV_KOSUU ���ɤ߼��ޤ� */
	for (l = 0; l < SV_KOSUU; l++) {
		for (k = 0; k < fzkvect_l; k++) {
			fzk_tbl->kango_vect_area[l].vector[k] = get_hexsa(fp);
		}
	}
/* ������³�٥������ɤ߼�� V4.0 */
	/* ������ü��³�٥���No. - ������ü��³�٥��� */
	for (m = 0; m < kango_vect_kosuu; m++) {
		vect_count = get_decimal(fp);
		for (k = 0; k < fzkvect_l; k++) {
			fzk_tbl->kango_vect_area[SV_KOSUU+vect_count].vector[k] = get_hexsa(fp);
		}
	}
	/* �����ʻ�No. - ������ü��³�٥���No. */
	for (m = 0; m < kango_length; m++) {
		vect_count = get_decimal(fp);
		if ((k = get_decimal(fp)) != -1) {
			fzk_tbl->kango_hinsi_area[vect_count] = SV_KOSUU + k;
		} else {
			fzk_tbl->kango_hinsi_area[vect_count] = -1;
		}
	}

	if (check_eof(fp) < 0){
	    wnn_errorno = WNN_BAD_FZK_FILE;
	    return(NULL);
	}

	/* �����ʻ��ֹ��Ĵ�٤� */

	if ((sentou_no = wnn_find_hinsi_by_name(WNN_SENTOU_MEI)) == -1) {
		giji_hinsi_err(WNN_SENTOU_MEI);
		return(NULL);
	}
	if ((suuji_no = wnn_find_hinsi_by_name(WNN_SUUJI_MEI)) == -1) {
		giji_hinsi_err("SENTOU");
		return(NULL);
	}
	if ((katakanago_no = wnn_find_hinsi_by_name(WNN_KANA_MEI)) == -1) {
		giji_hinsi_err("KANA");
		return(NULL);
	}
	if ((eisuu_no = wnn_find_hinsi_by_name(WNN_EISUU_MEI)) == -1) {
		giji_hinsi_err("EISUU");
		return(NULL);
	}
	if ((kigou_no = wnn_find_hinsi_by_name(WNN_KIGOU_MEI)) == -1) {
		giji_hinsi_err("KIGOU");
		return(NULL);
	}
	if ((toji_kakko_no = wnn_find_hinsi_by_name(WNN_TOJIKAKKO_MEI)) == -1) {
		giji_hinsi_err("TOJI_KKAKO");
		return(NULL);
	}
	if ((fuzokugo_no = wnn_find_hinsi_by_name(WNN_FUZOKUGO_MEI)) == -1) {
		giji_hinsi_err("FUZOKUGO");
		return(NULL);
	}
	if ((kai_kakko_no = wnn_find_hinsi_by_name(WNN_KAIKKAKO_MEI)) == -1) {
		giji_hinsi_err("KAIKAKKO");
		return(NULL);
	}
	if ((giji_no = wnn_find_hinsi_by_name(WNN_GIJI_MEI)) == -1) {
		giji_hinsi_err("GIJI");
		return(NULL);
	}
	return (fzk_tbl);
}

giji_hinsi_err(str)
char	*str;
{
	wnn_errorno = WNN_GIJI_HINSI_ERR;
	error1("GIJI hinsi (%s) is not defined in hinsi data file.\n", str);
}

void
fzk_discard(fzk_tbl)
struct	FT	*fzk_tbl;
{
	if (fzk_tbl->vect_area != NULL)
		free(fzk_tbl->vect_area);
	if (fzk_tbl->kango_hinsi_area != NULL)
		free(fzk_tbl->kango_hinsi_area);
	if (fzk_tbl->kango_vect_area != NULL)
		free(fzk_tbl->kango_vect_area);
	if (fzk_tbl->tablefuzokugo != NULL)
		free(fzk_tbl->tablefuzokugo);
	free(fzk_tbl);
}

link_job(x, fzk_tbl)
register int	x;
struct	FT	*fzk_tbl;
{
	register int	n;
	register struct	fzkentry	*pter_a, *pter_b;

	for (pter_a = fzk_tbl->tablefuzokugo, x--, n = 0; n < x; n++, pter_a++) {
		for (pter_b = pter_a + 1; pter_b <= fzk_tbl->tablefuzokugo + x &&
		    Strncmp(pter_a->yomi, pter_b->yomi, pter_a->yomi_su) == 0;
		    pter_b++)
			pter_b->link = pter_a;
	}
}


/*
 * fzk_kai : fuzokugo kaiseki
 */

fzk_kai(start, end, syuutan_vect, syuutan_vect1, ichbnp_p)
wchar_t	*start;			/* string start pointer */
wchar_t	*end;			/* string end   pointer */
int	syuutan_vect;		/* ʸ�Ὢü vector */
int	syuutan_vect1;		/* ʸ�Ὢü vector 1 */
struct	ICHBNP	**ichbnp_p;	/* ich-bunpou area ��°����� set pointer
				pointer */
{
	static struct fzkwork	fzkwk[STRK_L + 1];/* ���� work area */
	static struct fzkwork	fzkwk1[STRK_L + 1];/* ���� work area */
	static int	maxpoint = STRK_L;

	struct fzkken	fzkinf[YOMI_L + 1];/* ���� work area */
	struct fzkken	fzkinf1[YOMI_L + 1];/* ���� work area */
	struct fzkken	*fzkinfp;

	struct ICHBNP	*getibsp();
	struct ICHBNP	*ichbnptr, *wkptr;

	register int	point;	/* index */
	register int	i, j;	/* work index */
	register int	cnt, n;	/* counter */
struct	kangovect	*endvect;	/* ʸ�Ὢü vector pointer */
struct	kangovect	*endvect1;	/* ʸ�Ὢü vector 1 */

	int	fzkvect_l = ft->fzkvect_l;
	int	kango_vect_l = ft->kango_vect_l;

	endvect = (struct kangovect *) ft->kango_vect_area + syuutan_vect;
	if (syuutan_vect1 != WNN_VECT_NO)
	    endvect1 = (struct kangovect *) ft->kango_vect_area + syuutan_vect1;
	else
	    endvect1 = 0;

 /*
  * initialize
  */
	for (i = maxpoint; i >= 0; i--) {
	    for (j = VECT_L - 1; j >= 0; j--) {
		fzkwk[i].vector[j] = 0;
		fzkwk1[i].vector[j] = 0;
	    }
	}
	maxpoint = 0;
	for (j = 0; j < fzkvect_l; j++) {
	    fzkwk[0].vector[j] = endvect->vector[j];
	    if (endvect1 != 0)
		fzkwk1[0].vector[j] = endvect1->vector[j];
	}
 /*
  * ��°�����
  */

	for (point = 0; point <= maxpoint; point++) {
	    if (point > STRK_L) {
		wnn_errorno = WNN_WKAREA_FULL;
		error1("Fuzokugo-kaiseki area is full");
		return (-1);
	    }
	    if (fzk_ck_vector(&fzkwk[point]) ) {
	    /* ��°�측�� vector */
		fzk_ken(start + point, end, &fzkwk[point], &fzkinf[0]);
		for (fzkinfp = &fzkinf[0]; fzkinfp->ent_ptr; fzkinfp++) {
		    n = (fzkinfp->ent_ptr)->yomi_su;
		    fzk_orvt(&fzkwk[point + n], &(fzkinfp->vector[0]));
		    maxpoint = (maxpoint < point + n) ? point + n : maxpoint;
		}
	    }
	    if (fzk_ck_vector(&fzkwk1[point])) {
	    /* ��°�측�� vector 1 */
		fzk_ken(start + point, end, &fzkwk1[point], &fzkinf1[0]);
		for (fzkinfp = &fzkinf1[0]; fzkinfp->ent_ptr; fzkinfp++) {
		    n = (fzkinfp->ent_ptr)->yomi_su;
		    fzk_orvt(&fzkwk1[point + n], &(fzkinfp->vector[0]));
		    maxpoint = (maxpoint < point + n) ? point + n : maxpoint;
		}
	    }
	}


 /* 
  * ��°����� set
  */
	for (point = cnt = 0, *ichbnp_p = NULL; point <= maxpoint; point++) {
	    if (kng_ckvt(&fzkwk[point], kango_vect_l) ||
	        kng_ckvt(&fzkwk1[point], kango_vect_l)) {
		if (!(n = cnt % FZKIBNO)) {
		    if ((wkptr = getibsp()) == NULL) {
			if (*ichbnp_p != NULL)
			    (void) freeibsp(*ichbnp_p);
			*ichbnp_p = NULL;
			error1("Error in fzk_kai.");
			return (-1);
		    } else {
			if (*ichbnp_p == NULL)
			    *ichbnp_p = wkptr;
			else
			    ichbnptr->next_p = wkptr;
			ichbnptr = wkptr;
		    }
		}
		ichbnptr->fzkib[n].offset = point;
		for (i = 0; i < kango_vect_l; i++) {
		    ichbnptr->fzkib[n].vector[i] = fzkwk[point].vector[i];
		    ichbnptr->fzkib1[n].vector[i] = fzkwk1[point].vector[i];
		}
		cnt++;
	    }
	}
	return (cnt);
}


/*
 * kng_ckvt	����٥����Υ����å�	V4.0
 */

kng_ckvt(vector, kango_vect_l)
register int	*vector;
register int	kango_vect_l;
{
	register int	i, rts;

	for (rts = 0, i = kango_vect_l; i > 0; i--)
		rts |= *vector++;/* OR cheak */
	return (rts);
}


/*
 * fzk_ckvt
 */
fzk_ckvt(vector)
register int	vector;
{
	return (fzk_ck_vector(((struct kangovect *) ft->kango_vect_area + vector)));
}


fzk_ck_vector(vector)
register int	*vector;
{
	register int	i, rts;

	rts = *vector++ & ~0x01;	/* ����Ƭ�ġפΥӥåȤ���� */
	for (i = ft->fzkvect_l - 1; i > 0; i--)
		rts |= *vector++;/* OR cheak */
	return (rts);
}


/*
 * fzk_orvt
 */

fzk_orvt(vector1, vector2)
register int	*vector1;
register int	*vector2;
{
	register int	i;

	for (i = ft->fzkvect_l; i > 0; i--)
		*vector1++ |= *vector2++;/* OR set */
}


fzk_ken(start, end, vector, fzkptr)
wchar_t	*start;
wchar_t	*end;
int	vector[];
struct fzkken	*fzkptr;
{
	register int	yomicnt;
	register int	setno;
	struct fzkken	*ansptr;
	register struct	fzkentry	*search_ptr;
	wchar_t	key[YOMI_L + 1];
	struct fzkentry	*bsrch();
	int	fzkvect_l = ft->fzkvect_l;

	ansptr = fzkptr;
	for (yomicnt = end - start; yomicnt < YOMI_L; key[yomicnt++] = 0)
		;
	for (yomicnt = 0; (yomicnt < YOMI_L) && (start < end);
	    key[yomicnt++] = *start++)
		;
	key[yomicnt] = 0;
	for (; yomicnt > 0; key[--yomicnt] = NULL) {
		search_ptr = (struct fzkentry *) bsrch(key);
		if (search_ptr != NULL) {
			setno = setfzk(search_ptr, &ansptr, vector, fzkvect_l);
			ansptr->ent_ptr = NULL;
			return (setno);
		}
	}
	fzkptr->ent_ptr = NULL;
	return (NO);
}
/******************************************/
setfzk(entry_ptr, answer_ptr, vector, fzkvect_l)
struct fzkentry	*entry_ptr;
struct fzkken	**answer_ptr;
int	vector[];
int	fzkvect_l;
{
	register int	setno;
	register int	vectroop;
	register int	wcnt, setflg;
	register struct	fzkvect	*vect_ptr;

	if (entry_ptr != NULL) {
		setno = setfzk(entry_ptr->link, answer_ptr, vector, fzkvect_l);
		for (wcnt = 0; wcnt < fzkvect_l; (*answer_ptr)->vector[wcnt++] = 0);
		setflg = 0;
		for (vectroop = entry_ptr->kosu, vect_ptr = entry_ptr->pter;
		    vectroop > 0; vectroop--, vect_ptr++) {
			if (bittest(vector, vect_ptr->no) > 0) {
				setflg = 1;
				for (wcnt = 0; wcnt < fzkvect_l; wcnt++)
					(*answer_ptr)->vector[wcnt] |= vect_ptr->vector[wcnt];
			}
		}
		if (setflg != 0) {
			(*answer_ptr)->ent_ptr = entry_ptr;
			(*answer_ptr)++;
			return (setno + 1);
		} else
			return (setno);
	} else
		return (0);
}
/***************************************************/
bittest(vector, no)
int	vector[];
int	no;
{
	register int	wvect;

	wvect = vector[no / (sizeof (int) << 3)];/* << 3 == * 8  */
	wvect >>= (no % (sizeof (int) << 3));
	if ((wvect & 0x00000001) == 1)
		return (1);
	else
		return (-1);
}
/*
int Strncmp(s1,s2,n)
register w_char *s1;
register w_char *s2;
register int n;
{
  if(n == 0)return(0);
  for (;n > 0 && *s1++ == *s2++;n--);
  return (int)(*--s1 - *--s2);
}
*/
/*****************************************************/
struct fzkentry *bsrch(key_yomi)
wchar_t	*key_yomi;
{
	register int	low, high, j, flg;

	for (low = 0, high = ft->fzklength; low < high;) {
		j = (high + low) >> 1;
		/*
		flg = Strncmp(key_yomi, (ft->tablefuzokugo + j)->yomi, YOMI_L);
		*/
{
		register int n;
		register wchar_t *s1, *s2;
		s1 = key_yomi;
		s2 = (ft->tablefuzokugo + j)->yomi;
		for (n = YOMI_L;n > 0 && *s1++ == *s2++;n--);
		flg = (int)(*--s1 - *--s2);
}
		if (flg > 0)
			low = j + 1;
		else
			if (flg < 0)
				high = j;
			else
				return (ft->tablefuzokugo + j);
	}
	return (0);
}

error_fzk() {
	wnn_errorno = WNN_NOT_FZK_FILE;
	error1( "Bat format in fzk_file\n");
	return (-1);
}

error_eof() {
	wnn_errorno = WNN_NOT_FZK_FILE;
	error1( "Unecpected EOF in reading fzk_file\n");
	return (-1);
}

get_decimal(fp)
register FILE	*fp;
{
	unsigned char	buf[24];
	int	k;
	if (get_string(fp, buf) == EOF) {
		return (error_eof());
	}
	if (sscanf(buf, "%d", &k) != 1) {
		return (error_fzk());
	}
	return (k);
}

get_hexsa(fp)
register FILE	*fp;
{
	unsigned char	buf[24];
	int	k;
	if (get_string(fp, buf) == EOF) {
		return (error_eof());
	}
	if (sscanf(buf, "%x", &k) != 1) {
		return (error_fzk());
	}
	return (k);
}

get_string(fp, buf)
register FILE	*fp;
register unsigned char	*buf;
{
	register unsigned char	*c = buf;
	register int	k;
	for (; (k = vgetc(fp)) == ';' || k == '\n' || k == '\t' || k == ' ';) {
		if (k == ';') {
			for (; (k = vgetc(fp)) != '\n';) {
				if (k == EOF) {
					return (EOF);
				}
			}
		}
	}
	if (k == EOF) {
		return (EOF);
	}
	vungetc(k, fp);
	for (; (k = vgetc(fp)) != ';' && k != '\n' &&
			k != '\t' && k != ' ' && k != EOF;) {
		*c++ = k;
	}
	*c = '\0';
	return (0);		/* not EOF */
}

check_eof(fp)
register FILE	*fp;
{
	unsigned char	buf[24];
	if (get_string(fp, buf) != EOF) {
		wnn_errorno = WNN_NOT_FZK_FILE;
		error1( "Not at the end of fzk_file\n");
		return (-1);
	}
	return (0);
}
