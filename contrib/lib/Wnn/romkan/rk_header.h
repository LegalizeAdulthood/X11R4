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
/***********************************************************************
			rk_header.h
						88. 5.20  �� ��

	rk_main.c rk_read.c rk_modread.c rk_bltinfn.c�ζ��̥إå���
	��Ǽ�äƤ���������礭���ʤɤ������
***********************************************************************/

/*	Version 3.0
 */

/*	make����ɬ�פ�define

	BSD42		BSD�ˤ�strings.h����ѡ�string.h��Ȥ��������ס�
	SYSVR2		System V�ˤ����������¤��줿toupper��tolower�����
			�ʤʤ��Ƥ�ư����
	MVUX		ECLIPSE MV�Ǥα�ž����define  IKIS����ưdefine�����

	RKMODPATH="ʸ����"
			����ʸ�����⡼�����ɽ�Υ������ѥ��δĶ��ѿ���
			̾���ˤ���
	WNNDEFAULT	��@LIBDIR�פ�ɸ������ɽ�Τ���ǥ��쥯�ȥ��ɽ����
			�褦�ˤ���	
	IKIS		Ⱦ�Ѳ�̾�Σ��Х����ܤ�0xA8�ʥǥե���Ȥ�0x8E�ˤˤ���

	����¾ �ǥХå�����ɬ�פ˱����� KDSP��CHMDSP��define
*/

#ifdef MVUX
#  define IKIS
#endif

#include <stdio.h>
#ifdef BSD42
#  include <strings.h>
#  define strchr  index
#  define strrchr rindex
#else
#  include <string.h>
#endif
#include "rk_macros.h"

#define ESCCHR	'\033'
#define BASEMX	(26 + 10)

#define REALFN	200 /* ɽ�Υե�͡���κ���Ĺ */

#define LINALL	1600 /* �б�ɽ�����ιԿ���� */
#define SIZALL	10000 /* �б�ɽ������ɽ���κ��祵������
			ɽ��Ĥ��ѿ����Ѱ�Ĺ�ι�פȤ��Ƥ�ȤäƤ� */
#define LINSIZ	1000 /* �б�ɽ�ΰ�Ԥκ��祵���� */
#define TRMSIZ	500 /* �б�ɽ�ΰ���ܤκ��祵������
			�⡼��̾�κ�Ĺ�Ȥ��Ƥ�ȤäƤ� */
#define KBFSIZ	100 /* �ܽ����Хåե��Υ����� */
#define DSPLIN	256 /* �ǥХå��� */
#define OUTSIZ	200 /* ���ϥХåե��Υ����� */
#define RSLMAX	20 /* �ؿ����ͤȤ����֤�ʸ����κ�Ĺ */

#define VARMAX	50 /* ɽ��Ĥ��ѿ��Ŀ� */
#define VARTOT	150 /* ��ɽ���ѿ��Ŀ��� */
#define VARLEN	500 /* �ѿ�̾��Ĺ���η� */

#define FILNST	20

	/* rk_modread.c�ǻȤ�define */

#define HYOMAX	40 /* �Ѵ��б�ɽ�κ���Ŀ� */
#define HYOMEI	500 /* ɽ̾��ʸ������ */
#define PTHMAX	30 /* �������ѥ��κ���Ŀ� */
#define PTHMEI	800 /* �������ѥ�̾��ʸ������ */
#define MODMAX	50 /* �⡼�ɤμ���� */
#define MODMEI	300 /* �⡼�ɤ���ʸ���� */
#define DMDMAX	40 /* �⡼��ɽ���μ���� */
#define DMDCHR	250 /* �⡼��ɽ������ʸ���� */
#define MDHMAX	2500 /* �⡼��ɽ�κ��祵���� */
  /* �⡼��ɽ�κǽ��listscan�λ��ϡ����顼���θ���ơ��ꥹ��1�Ĥ�buffer��
	ɽ�Υ�����ʬ��äƤ�����*/
#define MDT1LN	200 /* �⡼������ꥹ��1�Ĥκ���Ĺ */
#define NAIBMX	400 /* �⡼�����ɽ������ɽ���κ��祵���� */
			/* Change KURI 200 --> 400 */

#define ARGMAX	2 /* ���Ƚ�Ǵؿ��ΰ����κ���Ŀ� */

 /* �ǥ��쥯�ȥ�̾�ζ��ڤ��define��UNIX�ѡˡ�UNIX�ʳ��δĶ��ǻȤ��ˤ�
    ����ȡ�fixednamep()���ѹ���ɬ�פ������readmode()��getenv�ط��������ˡ�*/
#define KUGIRI '/'

 /* ���顼������ */
#include <setjmp.h>
