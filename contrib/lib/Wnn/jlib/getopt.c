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
/*LINTLIBRARY*/

/***********************************************************************
			getopt.c

	BSD用getopt関数。ライブラリファイルはlibgetopt.aです。
	システムに予めgetopt()が用意されていなかったら、これを
	リンクして下さい。そのためには、トップレベルの
	メークファイルのGETOPTLIBに、../jlib/libgetopt.aを
	加えて下さい。

***********************************************************************/

#define	NULL	0
#define	EOF	(-1)
#define	ERR(s, c)							\
	if(opterr){							\
		extern	int	strlen(), write();			\
		char	errbuf[2];					\
									\
		errbuf[0] = c;						\
		errbuf[1] = '\n';					\
		(void)write(2, argv[0], (unsigned)strlen(argv[0]));	\
		(void)write(2, s, (unsigned)strlen(s));			\
		(void)write(2, errbuf, 2);				\
	}

#ifdef	BSD42
#define	strchr	index
#endif

extern	int	strcmp();
extern	char	*strchr();

int	opterr = 1;
int	optind = 1;
int	optopt;
char	*optarg;

int	getopt(argc, argv, opts)
int	argc;
char	**argv, *opts;
{
	static	int	sp = 1;
	register int	c;
	register char	*cp;

	if(sp == 1){
		if(optind >= argc ||
		   argv[optind][0] != '-' || argv[optind][1] == '\0'){
			return(EOF);
		} else if(strcmp(argv[optind], "--") == NULL){
			optind++;
			return(EOF);
		}
	}
	optopt = c = argv[optind][sp];
	if(c == ':' || (cp=strchr(opts, c)) == NULL){
		ERR(": unknown option, -", c);
		if(argv[optind][++sp] == '\0'){
			optind++;
			sp = 1;
		}
		return('?');
	}
	if(*++cp == ':'){
		if(argv[optind][sp+1] != '\0'){
			optarg = &argv[optind++][sp+1];
		} else if(++optind >= argc){
			ERR(": argument missing for -", c);
			sp = 1;
			return('?');
		} else {
			optarg = argv[optind++];
		}
		sp = 1;
	} else {
		if(argv[optind][++sp] == '\0'){
			sp = 1;
			optind++;
		}
		optarg = NULL;
	}
	return(c);
}

/*
  Local Variables:
  kanji-flag: t
  End:
*/
