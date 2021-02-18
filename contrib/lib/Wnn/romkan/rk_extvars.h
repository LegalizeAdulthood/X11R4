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
			rk_extvars.h
						88. 6.16  訂 補

	二つ以上のファイルにまたがって使われる変数のextern宣言。
	本体はrk_vars.cで定義。
***********************************************************************/

/*	Version 3.0
 */

extern	char	rk_errstat;

extern	int	flags;
extern	jmp_buf env0;

extern	FILE	*modefile;
extern	char	nulstr[];

extern	char	*hcurread, *mcurread, *curfnm, *curdir;
extern	letter	*ltrbufbgn;
extern	FILE	*nestfile[], **base;

extern	struct	modestat
{
	modetyp	moderng;
	modetyp	curmode;
} modesw[];

extern	char	hyoshu[];
extern	char	**modmeiptr, *modmeibgn[], *modmeimem, modmeimem_[];
extern	char	**dspnamptr, *dspnambgn[], *dspcod, dspcod_[], *dspmod[][2];

extern	char	**hyomeiorg, **hyomeiptr, *hyomeimem;
extern	char	**pathmeiorg, **pathmeiptr, *pathareaorg, *pathmeimem;

extern	int	usemaehyo[], usehyo[], useatohyo[], naibu_[], *naibu;

extern	letter	*lptr;
extern	letter	rk_input, disout[], rk_output[], keybuf[], urabuf[];
extern	int	lastoutlen, lastkbflen;
extern	letter	oneletter[], nil[];
extern	int	hyonum;

extern	struct	funstr
{
	char	*fnname, appear, argnum, fntype;
};

extern	struct	dat
{
	letter	*code[3];
};

extern	struct	hyo
{
	struct	dat	*data;
	letter		**hensudef;
};

extern	struct	funstr	func[];
extern	struct	hyo	hyo_n[];
