/***********************************************************
Copyright International Business Machines Corporation 1989

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/



/*
 *	This is an internal header file for imkeybind.c
 */

/*
 *	Definition of keymap name and default language for keymap.
 */

#define	IMDEFAULTLANG	"ibmsbcs"
#define	IMKEYMAPNAME	"imkeymap"


 /* -------------------------------------------------------------- */
 /*   XLSTRUCT contains the data persistent 			   */
 /*   Mapping and used by it for its internal processing           */
 /* -------------------------------------------------------------- */

typedef	struct _XLMSTRUCT	{
	int	accum_state;		/* how many accumulated ?	*/
	char	accumulation;		/* accumulated value		*/
	int	ss_effect;		/* SS has been received or not  */
	int	dead_state;		/* TRUE if in dead state	*/
	OldKeyMapElt	deadkey;	/* save area for dead key	*/
	union	{			/* This structure holds a start	*/
		short	searchword;	/* stop point (index to COMP	*/	
		struct	{		/* table) between which to	*/
			char	start;	/* search for the second	*/
			char	stop;	/* keystroke when trying to	*/
		}	search;		/* combine two keys in dead key	*/
	}	ns;			/* state.  See xlmcomp.h	*/
}	XLMSTRUCT;


#define     SS_SET_MASK      0x80
#define     STATUSMASK       0x00FF

#define     AIXValidStates (ShiftMask|LockMask|ControlMask|Mod2Mask|Mod1Mask)

			   /* DEC     RT   key postions */
#define     ALT_NUM_0        XK_KP_0 /* 99   */
#define     ALT_NUM_1        XK_KP_1 /* 93   */
#define     ALT_NUM_2        XK_KP_2 /* 98   */
#define     ALT_NUM_3        XK_KP_3 /* 103  */
#define     ALT_NUM_4        XK_KP_4 /* 92   */
#define     ALT_NUM_5        XK_KP_5 /* 97   */
#define     ALT_NUM_6        XK_KP_6 /* 102  */
#define     ALT_NUM_7        XK_KP_7 /* 91   */
#define     ALT_NUM_8        XK_KP_8 /* 96   */
#define     ALT_NUM_9        XK_KP_9 /* 101  */

#define ic_acutestart     0x00
#define ic_gravestart     0x1A
#define ic_crflxstart     0x25
#define ic_umlautstart    0x3E
#define ic_tildestart     0x4B
#define ic_caronstart     0x56
#define ic_brevestart     0x69
#define ic_dblacustart    0x70
#define ic_ovrcrcstart    0x75
#define ic_ovrdotstart    0x7A
#define ic_macronstart    0x84
#define ic_cedilstart     0x8F
#define ic_ogonekstart    0x9F
#define ic_acutestop      0x19
#define ic_gravestop      0x24
#define ic_crflxstop      0x3D
#define ic_umlautstop     0x4A
#define ic_tildestop      0x55
#define ic_caronstop      0x68
#define ic_brevestop      0x6F
#define ic_dblacustop     0x74
#define ic_ovrcrcstop     0x79
#define ic_ovrdotstop     0x83
#define ic_macronstop     0x8E
#define ic_cedilstop      0x9E
#define ic_ogonekstop     0xA7

		   /* ASCII control code values */
#define   IC_ESC          0x1b

		   /* Key generated control sequences identifying
		      sequence with specific parameter(s) */
#define   KF_CUU   0x0101
#define   KF_CUD   0x0102
#define   KF_CUF   0x0103
#define   KF_CUB   0x0104
#define   KF_CBT   0x0105
#define   KF_CHT   0x0106
#define   KF_CVT   0x0107
#define   KF_HOM   0x0108
#define   KF_CPL   0x010B
#define   KF_CNL   0x010C
#define   KF_INS   0x0150
#define   KF_DCH   0x0151
#define   KF_IL    0x0152
#define   KF_DL    0x0153
#define   KF_EEOL  0x0154
#define   KF_EEOF  0x0155
#define   KF_CLR   0x0156
#define   KF_INIT  0x0157
#define   KF_RI    0x0162
#define   KF_IND   0x0163

			/* need to be changed to PFK equivalents */
#define   KF_IGNORE 0x01ff
			/* escape sequence final character values */
#define   FE_IND   0x44         /*needed for expnd*/
#define   FE_RI    0x4C
#define   FE_CSI   0x5B
#define   FS_RIS   0x63


	       /* CONTROL SEQUENCE FINAL CODES */     /*needed for expnd*/
#define   CSEQ_F_CBT    0x5A
#define   CSEQ_F_CHT    0x49
#define   CSEQ_F_CNL    0x45
#define   CSEQ_F_CUB    0x44
#define   CSEQ_F_CUD    0x42
#define   CSEQ_F_CUF    0x43
#define   CSEQ_F_CUP    0x48
#define   CSEQ_F_CUU    0x41
#define   CSEQ_F_DCH    0x50
#define   CSEQ_F_DL     0x4D
#define   CSEQ_F_EF     0x4E
#define   CSEQ_F_EL     0x4B
#define   CSEQ_F_ED     0x4A
#define   CSEQ_F_IL     0x4C
#define   CSEQ_F_PFK    0x71


#define  CHARSET_P0     0x3c
#define  CHARSET_P1     0x3d
#define  CHARSET_P2     0x3e


#define  charset_p0     0x3c
#define  charset_p1     0x3d
#define  charset_p2     0x3e


#define  SS_IN_EFFECT   1
#define  SS_JUST_COMPLETED  2

		   /* RT-ascii internal code values */
#define  ic_sp      0x20     /* space */
#define  ic_exc     0x21     /* exclamation mark */
#define  ic_2quot   0x22     /* double quote */
#define  ic_pound   0x23     /* # */
#define  ic_dollar  0x24     /* $ */
#define  ic_percent 0x25     /* percent */
#define  ic_amp     0x26     /* ampersand */
#define  ic_1quot   0x27     /* left single quote */
#define  ic_lpar    0x28     /* left parenthesis */
#define  ic_rpar    0x29     /* right parenthesis */
#define  ic_ast     0x2a     /* asterisk */
#define  ic_plus    0x2b     /* plus sign */
#define  ic_com     0x2c     /* comma */
#define  ic_dash    0x2d     /* dash */
#define  ic_period  0x2e     /* period */
#define  ic_slash   0x2f     /* slash */
#define  ic_0       0x30
#define  ic_1       0x31
#define  ic_2       0x32
#define  ic_3       0x33
#define  ic_4       0x34
#define  ic_5       0x35
#define  ic_6       0x36
#define  ic_7       0x37
#define  ic_8       0x38
#define  ic_9       0x39
#define  ic_colon   0x3a     /* colon */
#define  ic_semi    0x3b     /* semicolon */
#define  ic_lt      0x3c     /* less than sign */
#define  ic_eq      0x3d     /* equals sign */
#define  ic_gt      0x3e     /* greater than sign */
#define  ic_ques    0x3f     /* question mark */
#define  ic_at      0x40     /* @ */
#define  ic_uca     0x41     /* upper case a */
#define  ic_ucb     0x42
#define  ic_ucc     0x43
#define  ic_ucd     0x44
#define  ic_uce     0x45
#define  ic_ucf     0x46
#define  ic_ucg     0x47
#define  ic_uch     0x48
#define  ic_uci     0x49
#define  ic_ucj     0x4a
#define  ic_uck     0x4b
#define  ic_ucl     0x4c
#define  ic_ucm     0x4d
#define  ic_ucn     0x4e
#define  ic_uco     0x4f
#define  ic_ucp     0x50
#define  ic_ucq     0x51
#define  ic_ucr     0x52
#define  ic_ucs     0x53
#define  ic_uct     0x54
#define  ic_ucu     0x55
#define  ic_ucv     0x56
#define  ic_ucw     0x57
#define  ic_ucx     0x58
#define  ic_ucy     0x59
#define  ic_ucz     0x5a
#define  ic_lsb     0x5b     /* left square bracket */
#define  ic_bslash  0x5c     /* back slash */
#define  ic_rsb     0x5d     /* right square bracket */
#define  ic_and     0x5e     /* inverted v */
#define  ic_undsc   0x5f     /* underscore */
#define  ic_lquot   0x60
#define  ic_lca     0x61
#define  ic_lcb     0x62
#define  ic_lcc     0x63
#define  ic_lcd     0x64
#define  ic_lce     0x65
#define  ic_lcf     0x66
#define  ic_lcg     0x67
#define  ic_lch     0x68
#define  ic_lci     0x69
#define  ic_lcj     0x6a
#define  ic_lck     0x6b
#define  ic_lcl     0x6c
#define  ic_lcm     0x6d
#define  ic_lcn     0x6e
#define  ic_lco     0x6f
#define  ic_lcp     0x70
#define  ic_lcq     0x71
#define  ic_lcr     0x72
#define  ic_lcs     0x73
#define  ic_lct     0x74
#define  ic_lcu     0x75
#define  ic_lcv     0x76
#define  ic_lcw     0x77
#define  ic_lcx     0x78
#define  ic_lcy     0x79
#define  ic_lcz     0x7a
#define  ic_lbrace  0x7b     /* left curly brace */
#define  ic_or      0x7c     /* or symbol */
#define  ic_rbrace  0x7d     /* right curly brace */
#define  ic_approx  0x7e     /* approximately */
#define  ic_del     0x7f     /* delta */
#define  ic_uccced  0x80     /* upper case c cedilla */
#define  ic_lcuuml  0x81     /* lower case u umlaut */
#define  ic_lceacc  0x82     /* lower case e accent (acute) */
#define  ic_lcacrf  0x83     /* lower case a circumflex */
#define  ic_lcauml  0x84     /* lower case a umlaut */
#define  ic_lcagrv  0x85     /* lower case a grave */
#define  ic_lcaovc  0x86     /* lower case a overcircle */
#define  ic_lccced  0x87     /* lower case c cedilla */
#define  ic_lcecrf  0x88     /* lower case e circumflex */
#define  ic_lceuml  0x89     /* lower case e umlaut */
#define  ic_lcegrv  0x8a     /* lower case e grave */
#define  ic_lciuml  0x8b     /* lower case i umlaut */
#define  ic_lcicrf  0x8c     /* lower case i circumflex */
#define  ic_lcigrv  0x8d     /* lower case i grave */
#define  ic_ucauml  0x8e     /* upper case a umlaut */
#define  ic_ucaovc  0x8f     /* upper case o overcircle */
#define  ic_uceacc  0x90     /* upper case e acute */
#define  ic_lcae    0x91     /* lower case ae */
#define  ic_ucae    0x92     /* upper case ae */
#define  ic_lcocrf  0x93     /* lower case o circumflex */
#define  ic_lcouml  0x94     /* lower case e umlaut */
#define  ic_lcogrv  0x95     /* lower case o grave */
#define  ic_lcucrf  0x96     /* lower case u circumflex */
#define  ic_lcugrv  0x97     /* lower case u grave */
#define  ic_lcyuml  0x98     /* lower case y umlaut */
#define  ic_ucouml  0x99     /* upper case o umlaut */
#define  ic_ucuuml  0x9a     /* upper case u umlaut */
#define  ic_lcosls  0x9b     /* lower case o slash */
#define  ic_strlng  0x9c     /* pounds sterling */
#define  ic_ucosls  0x9d     /* upper case o slash */
#define  ic_multpl  0x9e     /* peseta sign */
#define  ic_florin  0x9f     /* florin sign */
#define  ic_lcaacc  0xa0     /* lower case a acute */
#define  ic_lciacc  0xa1     /* lower case i acute */
#define  ic_lcoacc  0xa2     /* lower case o acute */
#define  ic_lcuacc  0xa3     /* lower case u acute */
#define  ic_lcntil  0xa4     /* lower case n tilde */
#define  ic_ucntil  0xa5     /* upper case n tilde */
#define  ic_lcaund  0xa6     /* lower case a underscore */
#define  ic_lcound  0xa7     /* lower case o underscore */
#define  ic_invques 0xa8     /* inverted question mark */
#define  ic_regtrd  0xa9     /* registered trade mark */
#define  ic_notsign 0xaa     /* not sign */
#define  ic_half    0xab     /* one half */
#define  ic_quarter 0xac     /* one quarter */
#define  ic_invexcl 0xad     /* inverted exclamation point */
#define  ic_langquo 0xae     /* left angle quotes */
#define  ic_rangquo 0xaf     /* right angle quotes */
#define  ic_quahash 0xb0     /* quarter hashed */
#define  ic_hafhash 0xb1     /* half hashed */
#define  ic_fulhash 0xb2     /* full hashed */
#define  ic_vertbar 0xb3     /* vertical bar */
#define  ic_larm    0xb4     /* vertical bar w/left arm */
#define  ic_ucaacc  0xb5     /* upper case a accute */
#define  ic_ucacrf  0xb6     /* upper case a circumflex */
#define  ic_ucagrv  0xb7     /* upper case a grave */
#define  ic_copyrt  0xb8     /* copyright symbol */
#define  ic_cent    0xbd     /* cent symbol */
#define  ic_yen     0xbe     /* yen symbol */
#define  ic_urcorn  0xbf     /* upper right corner */
#define  ic_llcorn  0xc0     /* lower left corner */
#define  ic_invtee  0xc1     /* inverted tee */
#define  ic_tee     0xc2     /* upright tee */
#define  ic_rarm    0xc3     /* vertical bar w/right arm */
#define  ic_horzbar 0xc4     /* horizontal bar */
#define  ic_cross   0xc5     /* crossed bars */
#define  ic_lcatil  0xc6     /* lower case a tilde */
#define  ic_ucatil  0xc7     /* upper case a tilde */
#define  ic_intlcu  0xcf     /* international currency */
#define  ic_lceice  0xd0     /* lower case eth icelandic */
#define  ic_uceice  0xd1     /* upper case eth icelandic */
#define  ic_ucecrf  0xd2     /* upper case e circumflex */
#define  ic_uceuml  0xd3     /* upper case e umlaut */
#define  ic_ucegrv  0xd4     /* upper case e grave */
#define  ic_lcidtl  0xd5     /* lower case i dotless */
#define  ic_uciacc  0xd6     /* upper case i accute */
#define  ic_ucicrf  0xd7     /* upper case i circumflex */
#define  ic_uciuml  0xd8     /* upper case i umlaut */
#define  ic_lrcorn  0xd9     /* lower right corner */
#define  ic_ulcorn  0xda     /* upper left corner */
#define  ic_brite   0xdb     /* bright character cell */
#define  ic_botbrit 0xdc     /* bottom bright character cell */
#define  ic_vlbrok  0xdd     /* vertical line broken */
#define  ic_ucigrv  0xde     /* upper case i grave */
#define  ic_topbrit 0xdf     /* top bright character cell */
#define  ic_ucoacc  0xe0     /* upper case o accute */
#define  ic_dbless  0xe1     /* german double s; beta */
#define  ic_ucocrf  0xe2     /* upper case o circumflex */
#define  ic_ucogrv  0xe3     /* upper case o grave */
#define  ic_lcotil  0xe4     /* lower case o tilde */
#define  ic_ucotil  0xe5     /* upper case o tilde */
#define  ic_lcmu    0xe6     /* lower case mu */
#define  ic_lctice  0xe7     /* lower case thorn icelandic */
#define  ic_uctice  0xe8     /* upper case thorn icelandic */
#define  ic_ucuacc  0xe9     /* upper case u accute */
#define  ic_ucucrf  0xea     /* upper case u circumflex */
#define  ic_ucugrv  0xeb     /* upper case u grave */
#define  ic_lcyacc  0xec     /* lower case y accute */
#define  ic_ucyacc  0xed     /* upper case y accute */
#define  ic_ovrbar  0xee     /* overbar */
#define  ic_acute   0xef     /* accute */
#define  ic_sylhyp  0xf0     /* syllable hyphen */
#define  ic_plsmns  0xf1     /* lower case sigma */
#define  ic_dblund  0xf2     /* double underscore */
#define  ic_3quart  0xf3     /* three quarters */
#define  ic_para    0xf4     /* paragraph symbol */
#define  ic_sect    0xf5     /* section symbol */
#define  ic_divide  0xf6     /* division symbol */
#define  ic_cedilla 0xf7     /* cedilla */
#define  ic_degree  0xf8     /* degree symbol */
#define  ic_umlaut  0xf9     /* umlaut */
#define  ic_mddotf  0xfa     /* filled middle dot */
#define  ic_ssone   0xfb     /* superscript 1 */
#define  ic_ssthre  0xfc     /* superscript 3 */
#define  ic_sstwo   0xfd     /* superscript 2 */
#define  ic_vtrecf  0xfe     /* filled vertical rectangle */
#define  ic_foxsp   0xff     /* space */

/*****************below are p1 offsets**************************/

#define  ic_para_old    0x34     /* paragraph symbol */
#define  ic_sect_old    0x35     /* section symbol */
#define  ic_lcatil_old  0x40     /* lower case a tilde */
#define  ic_lcsshp_old  0x41     /* lower case s sharp */
#define  ic_ucacrf_old  0x42     /* upper case a circumflex */
#define  ic_ucagrv_old  0x43     /* upper case a grave */
#define  ic_ucaacc_old  0x44     /* upper case a acute */
#define  ic_ucatil_old  0x45     /* upper case a tilde */
#define  ic_lcosls_old  0x46     /* lower case o slash */
#define  ic_ucecrf_old  0x47     /* upper case e circumflex */
#define  ic_uceuml_old  0x48     /* upper case e umlaut */
#define  ic_ucegrv_old  0x49     /* upper case e grave */
#define  ic_uciacc_old  0x4a     /* upper case i acute */
#define  ic_ucicrf_old  0x4b     /* upper case i circumflex */
#define  ic_uciuml_old  0x4c     /* upper case i umlaut */
#define  ic_ucigrv_old  0x4d     /* upper case i grave */
#define  ic_ucosls_old  0x4e     /* upper case o slash */
#define  ic_lceice_old  0x4f     /* lower case eth icelandic */
#define  ic_lcyacc_old  0x50     /* lower case u acute */
#define  ic_lctice_old  0x51     /* lower case thorn icelandic */
#define  ic_cedilla_old 0x52     /* cedilla diacritic */
#define  ic_intlcu_old  0x53     /* international currency */
#define  ic_uceice_old  0x54     /* upper case eth icelandic */
#define  ic_ucyacc_old  0x55     /* upper case y acute */
#define  ic_uctice_old  0x56     /* upper case thorn icelandic */
#define  ic_regtrd_old  0x57     /* registered trademark */
#define  ic_3quart_old  0x58     /* three quarters */
#define  ic_ovrbar_old  0x59     /* overbar */
#define  ic_umlaut_old  0x5a     /* umlaut diacritic */
#define  ic_acute_old   0x5b     /* acute diacritic */
#define  ic_dblund_old  0x5c     /* double underscore */
#define  ic_lcotil_old  0x5d     /* lower case o tilde */
#define  ic_lcidtl_old  0x5e     /* lower case i dotless */
#define  ic_ucocrf_old  0x5f     /* upper case o circumflex */
#define  ic_ucogrv_old  0x60     /* upper case o grave */
#define  ic_ucoacc_old  0x61     /* upper case o acute */
#define  ic_ucotil_old  0x62     /* upper case o tilde */
#define  ic_ssthre_old  0x63     /* superscript 3 */
#define  ic_ucucrf_old  0x64     /* upper case u circumflex */
#define  ic_ucugrv_old  0x65     /* upper case u grave */
#define  ic_ucuacc_old  0x66     /* upper case u acute */
#define  ic_lcaogo  0x67     /* lower case o ogonek */
#define  ic_lcecar  0x68     /* lower case e caron */
#define  ic_lcccar  0x69     /* lower case c caron */
#define  ic_lccacc  0x6a     /* lower case c acute */
#define  ic_lceogo  0x6b     /* lower case e ogonek */
#define  ic_lcuovc  0x6c     /* lower case u overcircle */
#define  ic_lcdcar  0x6d     /* lower case d caron */
#define  ic_lclacc  0x6e     /* lower case l acute */
#define  ic_ucaogo  0x6f     /* upper case a ogonek */
#define  ic_ucecar  0x70     /* upper case e caron */
#define  ic_ucccar  0x71     /* upper case c caron */
#define  ic_uccacc  0x72     /* upper case c acute */
#define  ic_caron   0x73     /* caron diacritic */
#define  ic_uceogo  0x74     /* upper case e ogonek */
#define  ic_ucuovc  0x75     /* upper case u overcircle */
#define  ic_ucdcar  0x76     /* upper case d caron */
#define  ic_uclacc  0x77     /* upper case l acute */
#define  ic_lclcar  0x78     /* lower case l caron */
#define  ic_lcncar  0x79     /* lower case n caron */
#define  ic_lcdstk  0x7a     /* lower case d stroke */
#define  ic_lcrcar  0x7b     /* lower case r caron */
#define  ic_lcsacc  0x7c     /* lower case s acute */
#define  ic_ovrcrc  0x7d     /* overcircle diacritic */
#define  ic_lclstk  0x7e     /* lower case l stroke */
#define  ic_lcnacc  0x7f     /* lower case n acute */
#define  ic_lcscar  0x80     /* lower case s caron */
#define  ic_uclcar  0x81     /* upper case l caron */
#define  ic_ucncar  0x82     /* upper case n caron */
#define  ic_ucrcar  0x83     /* upper case r caron */
#define  ic_ucsacc  0x84     /* upper case s acute */
#define  ic_ovrdot  0x85     /* overdot diacritic */
#define  ic_lczovd  0x86     /* lower case z overdot */
#define  ic_ogonek  0x87     /* ogonek */
#define  ic_uczovd  0x88     /* upper case z overdot */
#define  ic_lczcar  0x89     /* lower case z caron */
#define  ic_lczacc  0x8a     /* lower case z acute */
#define  ic_uczcar  0x8b     /* upper case z caron */
#define  ic_uczacc  0x8c     /* upper case z acute */
#define  ic_uclstk  0x8d     /* upper case l stroke */
#define  ic_ucnacc  0x8e     /* upper case n acute */
#define  ic_ucscar  0x8f     /* upper case s caron */
#define  ic_lctcar  0x90     /* lower case t caron */
#define  ic_lcracc  0x91     /* lower case r acute */
#define  ic_lcodac  0x92     /* lower case o double acute */
#define  ic_lcudac  0x93     /* lower case u double acute */
#define  ic_uctcar  0x94     /* upper case t caron */
#define  ic_ucracc  0x95     /* upper case r acute */
#define  ic_ucodac  0x96     /* upper case o double acute */
#define  ic_ucudac  0x97     /* upper case u double acute */
#define  ic_lcabrv  0x98     /* lower case a breve */
#define  ic_lcgbrv  0x99     /* lower case g breve */
#define  ic_uciovd  0x9a     /* upper case i overdot */
#define  ic_ucabrv  0x9b     /* upper case a breve */
#define  ic_ucgbrv  0x9c     /* upper case g breve */
#define  ic_breve   0x9d     /* breve diacritic */
#define  ic_dblacu  0x9e     /* double acute diacritic */
#define  ic_lcsced  0x9f     /* lower case s cedilla */
#define  ic_liter   0xa0     /* liter symbol */
#define  ic_ucsced  0xa2     /* upper case s cedilla */
#define  ic_macron  0xa3     /* macron diacritic */
#define  ic_lctced  0xa4     /* lower case t cedilla */
#define  ic_uctced  0xa5     /* upper case t cedilla */
#define  ic_lcamac  0xa6     /* lower case a macron */
#define  ic_ucamac  0xa7     /* upper case a macron */
#define  ic_lcccrf  0xa8     /* lower case c circumflex */
#define  ic_ucccrf  0xa9     /* upper case c circumflex */
#define  ic_lccovd  0xab     /* lower case c overdot */
#define  ic_uccovd  0xac     /* upper case c overdot */
#define  ic_lceovd  0xad     /* lower case e overdot */
#define  ic_uceovd  0xae     /* upper case e overdot */
#define  ic_lcemac  0xaf     /* lower case e macron */
#define  ic_ucemac  0xb0     /* upper case e macron */
#define  ic_lcgacc  0xb1     /* lower case g acute */
#define  ic_lcgcrf  0xb2     /* lower case g circumflex */
#define  ic_ucgcrf  0xb3     /* upper case g circumflex */
#define  ic_lcgovd  0xb4     /* lower case g overdot */
#define  ic_ucgovd  0xb5     /* upper case g overdot */
#define  ic_ucgced  0xb6     /* upper case g cedilla */
#define  ic_lchcrf  0xb7     /* lower case h circumflex */
#define  ic_uchcrf  0xb8     /* upper case h circumflex */
#define  ic_lchstk  0xb9     /* lower case h stroke */
#define  ic_uchstk  0xba     /* upper case h stroke */
#define  ic_lcitil  0xbb     /* lower case i tilde */
#define  ic_ucitil  0xbc     /* upper case i tilde */
#define  ic_lcimac  0xbd     /* lower case i macron */
#define  ic_ucimac  0xbe     /* upper case i macron */
#define  ic_lciogo  0xbf     /* lower case i ogonek */
#define  ic_uciogo  0xc0     /* upper case i ogonek */
#define  ic_lcij    0xc1     /* lower case ij ligature */
#define  ic_ucij    0xc2     /* upper case ij ligature */
#define  ic_lcjcrf  0xc3     /* lower case j circumflex */
#define  ic_ucjcrf  0xc4     /* upper case j circumflex */
#define  ic_lckced  0xc5     /* lower case k cedilla */
#define  ic_uckced  0xc6     /* upper case k cedilla */
#define  ic_lckgre  0xc7     /* lower case k greenlandic */
#define  ic_lclced  0xc8     /* lower case l cedilla */
#define  ic_uclced  0xc9     /* upper case l cedilla */
#define  ic_lclmdt  0xca     /* lower case l middle dot */
#define  ic_uclmdt  0xcb     /* upper case l middle dot */
#define  ic_lcnced  0xcc     /* lower case n cedilla */
#define  ic_ucnced  0xcd     /* upper case n cedilla */
#define  ic_lcnlap  0xce     /* lower case n eng lapp */
#define  ic_ucnlap  0xcf     /* upper case n eng lapp */
#define  ic_lcomac  0xd0     /* lower case o macron */
#define  ic_ucomac  0xd1     /* upper case o macron */
#define  ic_lcoe    0xd2     /* lower case oe ligature */
#define  ic_ucoe    0xd3     /* upper case oe ligature */
#define  ic_lcrced  0xd4     /* lower case r cedilla */
#define  ic_ucrced  0xd5     /* upper case r cedilla */
#define  ic_lcscrf  0xd6     /* lower case s circumflex */
#define  ic_ucscrf  0xd7     /* upper case s circumflex */
#define  ic_lctstk  0xd8     /* lower case t stroke */
#define  ic_uctstk  0xd9     /* upper case t stroke */
#define  ic_lcutil  0xda     /* lower case u tilde */
#define  ic_ucutil  0xdb     /* upper case u tilde */
#define  ic_lcubrv  0xdc     /* lower case u breve */
#define  ic_ucubrv  0xdd     /* upper case u breve */
#define  ic_lcumac  0xde     /* lower case u macron */
#define  ic_ucumac  0xdf     /* upper case u macron */
#define  ic_lcuogo  0xe0     /* lower case u ogonek */
#define  ic_ucuogo  0xe1     /* upper case u ogonek */
#define  ic_lcwcrf  0xe2     /* lower case w circumflex */
#define  ic_ucwcrf  0xe3     /* upper case w circumflex */
#define  ic_lcycrf  0xe4     /* lower case y circumflex */
#define  ic_ucycrf  0xe5     /* upper case y circumflex */
#define  ic_ucyuml  0xe6     /* upper case y umlaut */
#define  ic_multpl_old  0xee     /* multiply symbol */
  /**********************below are p2 offsets************************/
#define  ic_logor   0x24     /* logical or symbol */
#define  ic_peseta  0x75     /* Peseta */
#define  ic_revnot  0x76     /* reversed logical not */
#define  ic_lbrite  0x88     /* left half bright cell */
#define  ic_lctau   0x90     /* lower case tau */
#define  ic_ucphi   0x91     /* upper case phi */
#define  ic_elemnt  0x97     /* element */
#define  ic_identy  0x99     /* identity symbol */
#define  ic_gteq    0x9a     /* greater than/equal */
#define  ic_lteq    0x9b     /* less than/equal */
#define  ic_uintgl  0x9c     /* upper half integral sign */
#define  ic_lintgl  0x9d     /* lower half integral sign */
#define  ic_sqroot  0xa0     /* square root */
#define  ic_ssn     0xa2     /* superscript n */


/***********************************************************************/
/* Diacritic table for dead key composition.                           */
/* This table delimits the range of the various diacritic symbols.     */
/* The first 16 bits of each entry represents the diacritic, itself;   */
/* the next 16 are the start and stop pointers into the VTMCOMPosition */
/* table.  Once a diacritic is located in this table, the larger table */
/* may be searched for the correct alphabetic to be combined with the  */
/* given diacritic.                                                    */
/* NOTE: the acute may be validly represented by either of two symbols.*/
/*                                                                     */
/***********************************************************************/

#define    NUMDIACRITICS  18

   struct  shdiac  {
	  short SYMB;
	  short INDEX;
   };

   struct diacrentry {
	 char PAGE;
	 char POINT;
	 char START;
	 char STOP;
   };

static struct diacrentry DIAC[18] = {
{charset_p0, ic_1quot,       ic_acutestart,  ic_acutestop }, /*acute    0 */
{charset_p1, ic_acute_old,   ic_acutestart,  ic_acutestop }, /*acute    1 */
{charset_p0, ic_acute,       ic_acutestart,  ic_acutestop }, /*acute    2 */
{charset_p0, ic_lquot,       ic_gravestart,  ic_gravestop }, /*grave    3 */
{charset_p0, ic_and,         ic_crflxstart,  ic_crflxstop }, /*crcflex  4 */
{charset_p1, ic_umlaut_old,  ic_umlautstart, ic_umlautstop}, /*umlaut   5 */
{charset_p0, ic_umlaut,      ic_umlautstart, ic_umlautstop}, /*umlaut   6 */
{charset_p0, ic_approx,      ic_tildestart,  ic_tildestop},  /*tilde    7 */
{charset_p1, ic_caron,       ic_caronstart,  ic_caronstop},  /*caron    8 */
{charset_p1, ic_breve,       ic_brevestart,  ic_brevestop},  /*breve    9 */
{charset_p1, ic_dblacu,      ic_dblacustart, ic_dblacustop}, /*dblacute 10*/
{charset_p1, ic_ovrcrc,      ic_ovrcrcstart, ic_ovrcrcstop}, /*ovrcrcl  11*/
{charset_p0, ic_degree,      ic_ovrcrcstart, ic_ovrcrcstop}, /*ovrcrcl  12*/
{charset_p1, ic_ovrdot,      ic_ovrdotstart, ic_ovrdotstop}, /*overdot  13*/
{charset_p1, ic_macron,      ic_macronstart, ic_macronstop}, /*macron   14*/
{charset_p1, ic_cedilla_old, ic_cedilstart,  ic_cedilstop},  /*cedilla  15*/
{charset_p0, ic_cedilla,     ic_cedilstart,  ic_cedilstop},  /*cedilla  16*/
{charset_p1, ic_ogonek,      ic_ogonekstart, ic_ogonekstop}  /*ogonek   17*/
};


/************************************************************************/
/* Composition table for dead keys.                                     */
/* This table folds two input code points into one output (composite)   */
/* code point.  The first input, a diacritic, defines a range of the    */
/* table (start and stop points for searching); the second input is the */
/* alphabetic code page/point, "inpage", "inpoint".  The resulting com- */
/* posite code page/point is "outpage", "outpoint".  The table is set   */
/* up so that the first entry of each diacritic range is the space      */
/* "character", which will result in an output of the diacritic, itself.*/
/*                                                                      */
/************************************************************************/

struct shcomp  {
       short COMPIN;
       short COMPOUT;
};

struct compentry {
       char INPAGE;
       char INPOINT;
       char OUTPAGE;
       char OUTPOINT;
};

static struct compentry COMP[168] = {

/*   ACUTE TABLE      */
{charset_p0, ic_sp , charset_p0, ic_acute},   /* acute            (0 */
{charset_p0, ic_lce, charset_p0, ic_lceacc},  /* e acute small    (1 */
{charset_p0, ic_lca, charset_p0, ic_lcaacc},  /* a acute small    (2 */
{charset_p0, ic_lci, charset_p0, ic_lciacc},  /* i acute small    (3 */
{charset_p0, ic_lco, charset_p0, ic_lcoacc},  /* o acute small    (4 */
{charset_p0, ic_lcu, charset_p0, ic_lcuacc},  /* u acute small    (5 */
{charset_p0, ic_lcy, charset_p0, ic_lcyacc},  /* y acute small    (6 */
{charset_p0, ic_lcc, charset_p1, ic_lccacc},  /* c acute small    (7 */
{charset_p0, ic_lcl, charset_p1, ic_lclacc},  /* l acute small    (8 */
{charset_p0, ic_lcs, charset_p1, ic_lcsacc},  /* s acute small    (9 */
{charset_p0, ic_lcn, charset_p1, ic_lcnacc},  /* n acute small    (10 */
{charset_p0, ic_lcg, charset_p1, ic_lcgacc},  /* g acute small    (11 */
{charset_p0, ic_lcr, charset_p1, ic_lcracc},  /* r acute small    (12 */
{charset_p0, ic_lcz, charset_p1, ic_lczacc},  /* z acute small    (13 */
{charset_p0, ic_uce, charset_p0, ic_uceacc},  /* e acute cap      (14 */
{charset_p0, ic_uca, charset_p0, ic_ucaacc},  /* a acute cap      (15 */
{charset_p0, ic_uci, charset_p0, ic_uciacc},  /* i acute cap      (16 */
{charset_p0, ic_uco, charset_p0, ic_ucoacc},  /* o acute cap      (17 */
{charset_p0, ic_ucu, charset_p0, ic_ucuacc},  /* u acute cap      (18 */
{charset_p0, ic_ucy, charset_p0, ic_ucyacc},  /* y acute cap      (19 */
{charset_p0, ic_ucc, charset_p1, ic_uccacc},  /* c acute cap      (20 */
{charset_p0, ic_ucl, charset_p1, ic_uclacc},  /* l acute cap      (21 */
{charset_p0, ic_ucs, charset_p1, ic_ucsacc},  /* s acute cap      (22 */
{charset_p0, ic_ucn, charset_p1, ic_ucnacc},  /* n acute cap      (23 */
{charset_p0, ic_ucr, charset_p1, ic_ucracc},  /* r acute cap      (24 */
{charset_p0, ic_ucz, charset_p1, ic_uczacc},  /* z acute cap      (25 */
/*  GRAVE TABLE  */
{charset_p0, ic_sp , charset_p0, ic_lquot},   /* grave            (26 */
{charset_p0, ic_lce, charset_p0, ic_lcegrv},  /* e grave small    (27 */
{charset_p0, ic_lca, charset_p0, ic_lcagrv},  /* a grave small    (28 */
{charset_p0, ic_lci, charset_p0, ic_lcigrv},  /* i grave small    (29 */
{charset_p0, ic_lco, charset_p0, ic_lcogrv},  /* o grave small    (30 */
{charset_p0, ic_lcu, charset_p0, ic_lcugrv},  /* u grave small    (31 */
{charset_p0, ic_uce, charset_p0, ic_ucegrv},  /* e grave cap      (32 */
{charset_p0, ic_uca, charset_p0, ic_ucagrv},  /* a grave cap      (33 */
{charset_p0, ic_uci, charset_p0, ic_ucigrv},  /* i grave cap      (34 */
{charset_p0, ic_uco, charset_p0, ic_ucogrv},  /* o grave cap      (35 */
{charset_p0, ic_ucu, charset_p0, ic_ucugrv},  /* u grave cap      (36 */
/*    CIRCUMFLEX TABLE    */
{charset_p0, ic_sp , charset_p0, ic_and},     /* circumflex       (37 */
{charset_p0, ic_lce, charset_p0, ic_lcecrf},  /* e circmflx small (38 */
{charset_p0, ic_lca, charset_p0, ic_lcacrf},  /* a circmflx small (39 */
{charset_p0, ic_lci, charset_p0, ic_lcicrf},  /* i circmflx small (40 */
{charset_p0, ic_lco, charset_p0, ic_lcocrf},  /* o circmflx small (41 */
{charset_p0, ic_lcu, charset_p0, ic_lcucrf},  /* u circmflx small (42 */
{charset_p0, ic_lcy, charset_p1, ic_lcycrf},  /* y circmflx small (43 */
{charset_p0, ic_lcc, charset_p1, ic_lcccrf},  /* c circmflx small (44 */
{charset_p0, ic_lcj, charset_p1, ic_lcjcrf},  /* j circmflx small (45 */
{charset_p0, ic_lcs, charset_p1, ic_lcscrf},  /* s circmflx small (46 */
{charset_p0, ic_lcw, charset_p1, ic_lcwcrf},  /* w circmflx small (47 */
{charset_p0, ic_lcg, charset_p1, ic_lcgcrf},  /* g circmflx small (48 */
{charset_p0, ic_lch, charset_p1, ic_lchcrf},  /* h circmflx small (49 */
{charset_p0, ic_uce, charset_p0, ic_ucecrf},  /* e circmflx cap   (50 */
{charset_p0, ic_uca, charset_p0, ic_ucacrf},  /* a circmflx cap   (51 */
{charset_p0, ic_uci, charset_p0, ic_ucicrf},  /* i circmflx cap   (52 */
{charset_p0, ic_uco, charset_p0, ic_ucocrf},  /* o circmflx cap   (53 */
{charset_p0, ic_ucu, charset_p0, ic_ucucrf},  /* u circmflx cap   (54 */
{charset_p0, ic_ucy, charset_p1, ic_ucycrf},  /* y circmflx cap   (55 */
{charset_p0, ic_ucc, charset_p1, ic_ucccrf},  /* c circmflx cap   (56 */
{charset_p0, ic_ucj, charset_p1, ic_ucjcrf},  /* j circmflx cap   (57 */
{charset_p0, ic_ucs, charset_p1, ic_ucscrf},  /* s circmflx cap   (58 */
{charset_p0, ic_ucw, charset_p1, ic_ucwcrf},  /* w circmflx cap   (59 */
{charset_p0, ic_ucg, charset_p1, ic_ucgcrf},  /* g circmflx cap   (60 */
{charset_p0, ic_uch, charset_p1, ic_uchcrf},  /* h circmflx cap   (61 */
/*   UMLAUT TABLE   */
{charset_p0, ic_sp , charset_p0, ic_umlaut},  /* umlaut           (62 */
{charset_p0, ic_lcu, charset_p0, ic_lcuuml},  /* u umlaut small   (63 */
{charset_p0, ic_lca, charset_p0, ic_lcauml},  /* a umlaut small   (64 */
{charset_p0, ic_lci, charset_p0, ic_lciuml},  /* i umlaut small   (65 */
{charset_p0, ic_lco, charset_p0, ic_lcouml},  /* o umlaut small   (66 */
{charset_p0, ic_lce, charset_p0, ic_lceuml},  /* e umlaut small   (67 */
{charset_p0, ic_lcy, charset_p0, ic_lcyuml},  /* y umlaut small   (68 */
{charset_p0, ic_ucu, charset_p0, ic_ucuuml},  /* u umlaut cap     (69 */
{charset_p0, ic_uca, charset_p0, ic_ucauml},  /* a umlaut cap     (70 */
{charset_p0, ic_uci, charset_p0, ic_uciuml},  /* i umlaut cap     (71 */
{charset_p0, ic_uco, charset_p0, ic_ucouml},  /* o umlaut cap     (72 */
{charset_p0, ic_uce, charset_p0, ic_uceuml},  /* e umlaut cap     (73 */
{charset_p0, ic_ucy, charset_p1, ic_ucyuml},  /* y umlaut cap     (74 */
/*    TILDE TABLE    */
{charset_p0, ic_sp , charset_p0, ic_approx},  /* tilde            (75 */
{charset_p0, ic_lcn, charset_p0, ic_lcntil},  /* n tilde small    (76 */
{charset_p0, ic_lca, charset_p0, ic_lcatil},  /* a tilde small    (77 */
{charset_p0, ic_lci, charset_p1, ic_lcitil},  /* i tilde small    (78 */
{charset_p0, ic_lco, charset_p0, ic_lcotil},  /* o tilde small    (79 */
{charset_p0, ic_lcu, charset_p1, ic_lcutil},  /* u tilde small    (80 */
{charset_p0, ic_ucn, charset_p0, ic_ucntil},  /* n tilde cap      (81 */
{charset_p0, ic_uca, charset_p0, ic_ucatil},  /* a tilde cap      (82 */
{charset_p0, ic_uci, charset_p1, ic_ucitil},  /* i tilde cap      (83 */
{charset_p0, ic_uco, charset_p0, ic_ucotil},  /* o tilde cap      (84 */
{charset_p0, ic_ucu, charset_p1, ic_ucutil},  /* u tilde cap      (85 */
/*    CARON TABLE    */
{charset_p0, ic_sp , charset_p1, ic_caron},   /* caron            (86 */
{charset_p0, ic_lce, charset_p1, ic_lcecar},  /* e caron small    (87 */
{charset_p0, ic_lcc, charset_p1, ic_lcccar},  /* c caron small    (88 */
{charset_p0, ic_lcd, charset_p1, ic_lcdcar},  /* d caron small    (89 */
{charset_p0, ic_lcl, charset_p1, ic_lclcar},  /* l caron small    (90 */
{charset_p0, ic_lcs, charset_p1, ic_lcscar},  /* s caron small    (91 */
{charset_p0, ic_lcn, charset_p1, ic_lcncar},  /* n caron small    (92 */
{charset_p0, ic_lct, charset_p1, ic_lctcar},  /* t caron small    (93 */
{charset_p0, ic_lcr, charset_p1, ic_lcrcar},  /* r caron small    (94 */
{charset_p0, ic_lcz, charset_p1, ic_lczcar},  /* z caron small    (95 */
{charset_p0, ic_uce, charset_p1, ic_ucecar},  /* e caron cap      (96 */
{charset_p0, ic_ucc, charset_p1, ic_ucccar},  /* c caron cap      (97 */
{charset_p0, ic_ucd, charset_p1, ic_ucdcar},  /* d caron cap      (98 */
{charset_p0, ic_ucl, charset_p1, ic_uclcar},  /* l caron cap      (99 */
{charset_p0, ic_ucs, charset_p1, ic_ucscar},  /* s caron cap      (100 */
{charset_p0, ic_ucn, charset_p1, ic_ucncar},  /* n caron cap      (101 */
{charset_p0, ic_uct, charset_p1, ic_uctcar},  /* t caron cap      (102 */
{charset_p0, ic_ucr, charset_p1, ic_ucrcar},  /* r caron cap      (103 */
{charset_p0, ic_ucz, charset_p1, ic_uczcar},  /* z caron cap      (104 */
/*    BREVE TABLE   */
{charset_p0, ic_sp , charset_p1, ic_breve},   /* breve            (105 */
{charset_p0, ic_lca, charset_p1, ic_lcabrv},  /* a  breve small   (106 */
{charset_p0, ic_lcu, charset_p1, ic_lcubrv},  /* u  breve small   (107 */
{charset_p0, ic_lcg, charset_p1, ic_lcgbrv},  /* g  breve small   (108 */
{charset_p0, ic_uca, charset_p1, ic_ucabrv},  /* a  breve cap     (109 */
{charset_p0, ic_ucu, charset_p1, ic_ucubrv},  /* u  breve cap     (110 */
{charset_p0, ic_ucg, charset_p1, ic_ucgbrv},  /* g  breve cap     (111 */
/*    DOUBLE ACUTE TABLE   */
{charset_p0, ic_sp , charset_p1, ic_dblacu},  /* double acute     (112 */
{charset_p0, ic_lco, charset_p1, ic_lcodac},  /* o double acute small(113 */
{charset_p0, ic_lcu, charset_p1, ic_lcudac},  /* u double acute small(114 */
{charset_p0, ic_uco, charset_p1, ic_ucodac},  /* o double acute cap  (115 */
{charset_p0, ic_ucu, charset_p1, ic_ucudac},  /* u double acute cap  (116 */
/*    OVERCIRCLE TABLE   */
{charset_p0, ic_sp , charset_p0, ic_degree},  /* overcircle          (117 */
{charset_p0, ic_lca, charset_p0, ic_lcaovc},  /* a overcircle small  (118 */
{charset_p0, ic_lcu, charset_p1, ic_lcuovc},  /* u overcircle small  (119 */
{charset_p0, ic_uca, charset_p0, ic_ucaovc},  /* a overcircle cap    (120 */
{charset_p0, ic_ucu, charset_p1, ic_ucuovc},  /* u overcircle cap    (121 */
/*    OVERDOT TABLE   */
{charset_p0, ic_sp , charset_p1, ic_ovrdot},  /* overdot             (122 */
{charset_p0, ic_lcz, charset_p1, ic_lczovd},  /* z overdot small     (123 */
{charset_p0, ic_lcc, charset_p1, ic_lccovd},  /* c overdot small     (124 */
{charset_p0, ic_lce, charset_p1, ic_lceovd},  /* e overdot small     (125 */
{charset_p0, ic_lcg, charset_p1, ic_lcgovd},  /* g overdot small     (126 */
{charset_p0, ic_ucz, charset_p1, ic_uczovd},  /* z overdot cap       (127 */
{charset_p0, ic_ucc, charset_p1, ic_uccovd},  /* c overdot cap       (128 */
{charset_p0, ic_uce, charset_p1, ic_uceovd},  /* e overdot cap       (129 */
{charset_p0, ic_ucg, charset_p1, ic_ucgovd},  /* g overdot cap       (130 */
{charset_p0, ic_uci, charset_p1, ic_uciovd},  /* i overdot cap       (131 */
/*    MACRON TABLE   */
{charset_p0, ic_sp , charset_p1, ic_macron},  /* macron              (132 */
{charset_p0, ic_lca, charset_p1, ic_lcamac},  /* a macron small      (133 */
{charset_p0, ic_lce, charset_p1, ic_lcemac},  /* e macron small      (134 */
{charset_p0, ic_lci, charset_p1, ic_lcimac},  /* i macron small      (135 */
{charset_p0, ic_lco, charset_p1, ic_lcomac},  /* o macron small      (136 */
{charset_p0, ic_lcu, charset_p1, ic_lcumac},  /* u macron small      (137 */
{charset_p0, ic_uca, charset_p1, ic_ucamac},  /* a macron cap        (138 */
{charset_p0, ic_uce, charset_p1, ic_ucemac},  /* e macron cap        (139 */
{charset_p0, ic_uci, charset_p1, ic_ucimac},  /* i macron cap        (140 */
{charset_p0, ic_uco, charset_p1, ic_ucomac},  /* o macron cap        (141 */
{charset_p0, ic_ucu, charset_p1, ic_ucumac},  /* u macron cap        (142 */
/*   CEDILLA TABLE   */
{charset_p0, ic_sp , charset_p0, ic_cedilla}, /* cedilla             (143 */
{charset_p0, ic_lcc, charset_p0, ic_lccced},  /* u cedilla small     (144 */
{charset_p0, ic_lcs, charset_p1, ic_lcsced},  /* u cedilla small     (145 */
{charset_p0, ic_lct, charset_p1, ic_lctced},  /* u cedilla small     (146 */
{charset_p0, ic_lck, charset_p1, ic_lckced},  /* o cedilla small     (147 */
{charset_p0, ic_lcn, charset_p1, ic_lcnced},  /* a cedilla small     (148 */
{charset_p0, ic_lcl, charset_p1, ic_lclced},  /* e cedilla small     (149 */
{charset_p0, ic_lcr, charset_p1, ic_lcrced},  /* i cedilla small     (150 */
{charset_p0, ic_ucc, charset_p0, ic_uccced},  /* c cedilla cap       (151 */
{charset_p0, ic_ucs, charset_p1, ic_ucsced},  /* s cedilla cap       (152 */
{charset_p0, ic_uct, charset_p1, ic_uctced},  /* t cedilla cap       (153 */
{charset_p0, ic_uck, charset_p1, ic_uckced},  /* k cedilla cap       (154 */
{charset_p0, ic_ucg, charset_p1, ic_ucgced},  /* g cedilla cap       (155 */
{charset_p0, ic_ucn, charset_p1, ic_ucnced},  /* n cedilla cap       (156 */
{charset_p0, ic_ucl, charset_p1, ic_uclced},  /* l cedilla cap       (157 */
{charset_p0, ic_ucr, charset_p1, ic_ucrced},  /* r cedilla cap       (158 */
/*   OGONEK TABLE   */
{charset_p0, ic_sp , charset_p1, ic_ogonek},  /* ogonek              (159 */
{charset_p0, ic_lca, charset_p1, ic_lcaogo},  /* a ogonek small      (160 */
{charset_p0, ic_lce, charset_p1, ic_lceogo},  /* e ogonek small      (161 */
{charset_p0, ic_lci, charset_p1, ic_lciogo},  /* i ogonek small      (162 */
{charset_p0, ic_lcu, charset_p1, ic_lcuogo},  /* u ogonek small      (163 */
{charset_p0, ic_uca, charset_p1, ic_ucaogo},  /* a ogonek cap        (164 */
{charset_p0, ic_uce, charset_p1, ic_uceogo},  /* e ogonek cap        (165 */
{charset_p0, ic_uci, charset_p1, ic_uciogo},  /* i ogonek cap        (166 */
{charset_p0, ic_ucu, charset_p1, ic_ucuogo}   /* u ogonek cap        (167 */
/**********************************************************************/
};
