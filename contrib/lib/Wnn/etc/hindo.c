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
		  				修正 9/1/89

	a＝実頻度, b＝仮頻度（共に非負整数）のとき

	b＝[a÷x]＋2(x−1) where x＝[( sqrt(1＋2a)＋1 )÷2]
	（計算結果が126を越えた場合は126とする。）

	(min)a＝(b−2y)(y＋1)	      where y＝[b÷4]
	(mid)a＝(b−2y)(y＋1)＋[y÷2] where y＝[b÷4]
	（b≦126とする。）

	ここで[ ]はガウス記号
	bを与えてのaの推測値には幅があり、(min)aとはその最小値、
	(mid)aはその中間値
	b≦4 なら、(min)a＝(mid)a＝b

	仮頻度がbのとき、頻度の更新の確率は 1 / ([b÷4]＋1)
	但し b＝0の時は、頻度更新確率 0

    
	 a == -1 <==> b == 0x7f = 127; 
	この時、このエントリーは、変換に決して用いられない
    	(コメントアウトされている)ことを表す。
		    	9/1/89 H.T.
*/

 /** 実頻度a→仮頻度b */
asshuku(hin)
int	hin;
{
	register int	c;

	if(hin == -1) return(127);
	if(hin <= 4) return(hin); 
	 /* 大半は頻度0と想定してのスピードアップ。motoni1,2でも同じ */

	c = (isqrt((hin <<= 1) + 1) + 1) & ~1;
	c += hin / c - 2;
	return( c < 126 ? c : 126);
}

 /** 仮頻度b→実頻度(min)a */
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

 /** 仮頻度b→実頻度(mid)a */
motoni2(hin)
int	hin;
{
	register int	c;

	if(hin == 127) return(-1);
	if(hin <= 4) return(hin); 
	c = hin >> 2;
	return( (hin - (c << 1)) * (c + 1) + (c >> 1) );
}

 /** 整数引数の平方根関数。但し引数<0の時のエラーチェックはなし（0を返す）。*/
static	isqrt(i)
int	i;
{
	register int	a, b;

	if(i <= 0) return(0);
	for(a = i, b = 1; b <<= 1, a >>= 2; );
	while((a = i / b) < b) b = (b + a) >> 1;
	return(b);
}
