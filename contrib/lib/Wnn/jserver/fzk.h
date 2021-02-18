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
 * fzk.h
 */

#define	YOMI_L	8		/* ��°����ɤߤ�Ĺ���κ����� */

#define STRK_L	128		/* ��°����ϤΤ���κ���ΰ���礭�� */
				/* ���줬����°��ʸ�����Ĺ���κ����� */

/* ��°��٥��ȥ�ι�¤ */
struct	fzkvect	{
	int	no;			/* ��°��٥��ȥ���ֹ� */
	int	vector[VECT_L];		/* ��°��٥��ȥ� */
};

/* ��°�쥨��ȥ꡼�ι�¤ */
struct	fzkentry {
	wchar_t	yomi[YOMI_L];		/* ��°����ɤߤ�ʸ���� */
	int	yomi_su;		/* ��°����ɤߤ�ʸ�����Ĺ�� */
	int	kosu;			/* ��°��٥��ȥ�θĿ� */
	struct	fzkvect	*pter;		/* ��°��٥��ȥ�ؤΥݥ��� */
	struct	fzkentry *link;		/* ������°�쥨��ȥ꡼�ؤΥݥ��� */
};

/* ��°��θ�����̤ξ���ι�¤ */
struct	fzkken {
	struct	fzkentry *ent_ptr;	/* ������°�쥨��ȥ꡼�ؤΥݥ��� */
	int	vector[VECT_L];		/* ��°��٥��ȥ� */
};

/* ��°����ϤΤ���κ���ΰ�ι�¤ */
struct	fzkwork {
	int	vector[VECT_L];		/* ��°��٥��ȥ� */
};


/*
	structure of FZK TABLE
 */
struct FT {
	struct	fzkvect		*vect_area;
	struct	kangovect	*kango_vect_area;
	struct	fzkentry	*tablefuzokugo;
	int			*kango_hinsi_area;
	int	fzkvect_l;			/* ��³�٥�����Ĺ�� * 32 bit */
	int	kango_vect_l;			/* ����٥�����Ĺ�� * 32 bit */
	int	fzklength;			/* ��°��� */
};
