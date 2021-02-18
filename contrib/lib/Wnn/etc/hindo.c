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
		  				���� 9/1/89

	a�������, b�Ჾ���١ʶ������������ˤΤȤ�

	b��[a��x]��2(x��1) where x��[( sqrt(1��2a)��1 )��2]
	�ʷ׻���̤�126��ۤ�������126�Ȥ��롣��

	(min)a��(b��2y)(y��1)	      where y��[b��4]
	(mid)a��(b��2y)(y��1)��[y��2] where y��[b��4]
	��b��126�Ȥ��롣��

	������[ ]�ϥ���������
	b��Ϳ���Ƥ�a�ο�¬�ͤˤ��������ꡢ(min)a�ȤϤ��κǾ��͡�
	(mid)a�Ϥ��������
	b��4 �ʤ顢(min)a��(mid)a��b

	�����٤�b�ΤȤ������٤ι����γ�Ψ�� 1 / ([b��4]��1)
	â�� b��0�λ��ϡ����ٹ�����Ψ 0

    
	 a == -1 <==> b == 0x7f = 127; 
	���λ������Υ���ȥ꡼�ϡ��Ѵ��˷褷���Ѥ����ʤ�
    	(�����ȥ����Ȥ���Ƥ���)���Ȥ�ɽ����
		    	9/1/89 H.T.
*/

 /** ������a��������b */
asshuku(hin)
int	hin;
{
	register int	c;

	if(hin == -1) return(127);
	if(hin <= 4) return(hin); 
	 /* ��Ⱦ������0�����ꤷ�ƤΥ��ԡ��ɥ��åס�motoni1,2�Ǥ�Ʊ�� */

	c = (isqrt((hin <<= 1) + 1) + 1) & ~1;
	c += hin / c - 2;
	return( c < 126 ? c : 126);
}

 /** ������b��������(min)a */
/*
motoni1(hin)
int	hin;
{
	register int	c;

	if(hin == 127) return(-1);
	if(hin <= 4) return(hin); 
	c = hin >> 2;
	return( (hin - (c << 1)) * (c + 1) );
}
*/

 /** ������b��������(mid)a */
motoni2(hin)
int	hin;
{
	register int	c;

	if(hin == 127) return(-1);
	if(hin <= 4) return(hin); 
	c = hin >> 2;
	return( (hin - (c << 1)) * (c + 1) + (c >> 1) );
}

 /** ����������ʿ�����ؿ���â������<0�λ��Υ��顼�����å��Ϥʤ���0���֤��ˡ�*/
static	isqrt(i)
int	i;
{
	register int	a, b;

	if(i <= 0) return(0);
	for(a = i, b = 1; b <<= 1, a >>= 2; );
	while((a = i / b) < b) b = (b + a) >> 1;
	return(b);
}
