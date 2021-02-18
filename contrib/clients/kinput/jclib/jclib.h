/* $Header: /private/usr/mmb/ishisone/src/jclib/RCS/jclib.h,v 4.0 89/10/27 20:15:25 ishisone Rel $ */

/*
 *	jclib.h -- jclib $BMQ%X%C%@%U%!%$%k(J (Wnn Version4 $BBP1~HG(J)
 *		version 4.0
 *		ishisone@sra.co.jp
 */

/*
 * Copyright (c) 1989  Software Research Associates, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Software Research
 * Associates makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * Author:  Makoto Ishisone, Software Research Associates, Inc., Japan
 */

#ifndef _jclib_h
#define _jclib_h

#include	"jslib.h"

/* $BDj?t(J */
#define JC_FORWARD	1
#define JC_BACKWARD	0
#define JC_NEXT		0
#define JC_PREV		1
#define JC_HIRAGANA	0
#define JC_KATAKANA	1

/* $B%(%i!<HV9f(J */
#define JE_NOERROR		0
#define JE_WNNERROR		1	/* jlib $B$N%(%i!<(J */
#define JE_NOCORE		2	/* $B%a%b%j$,3NJ]$G$-$J$$(J */
#define JE_NOTCONVERTED		3	/* $BBP>]J8@a$,$^$@JQ49$5$l$F$$$J$$(J */
#define JE_CANTDELETE		4	/* $B%P%C%U%!$N@hF,$NA0!"$"$k$$$O(J
					 * $B:G8e$N<!$NJ8;z$r:o=|$7$h$&$H$7$?(J */
#define JE_NOSUCHCLAUSE		5	/* $B;XDj$5$l$?HV9f$NJ8@a$,B8:_$7$J$$(J */
#define JE_CANTSHRINK		6	/* 1 $BJ8;z$NJ8@a$r=L$a$h$&$H$7$?(J */
#define JE_CANTEXPAND		7	/* $B:G8e$NJ8@a$r?-$P$=$&$H$7$?(J */
#define JE_NOCANDIDATE		8	/* $B<!8uJd$,$J$$(J */
#define JE_NOSUCHCANDIDATE	9	/* $B;XDj$5$l$?HV9f$N8uJd$,B8:_$7$J$$(J */
#define JE_CANTMOVE		10	/* $B%P%C%U%!$N@hF,$NA0!"$"$k$$$O(J
					 * $B:G8e$N<!$K0\F0$7$h$&$H$7$?(J */
#define JE_CLAUSEEMPTY		11	/* $B6u$NJ8$rJQ49$7$h$&$H$7$?(J */
#define JE_ALREADYFIXED		12	/* $B$9$G$K3NDj$5$l$F$$$kJ8$KBP$7$F(J
					 * $BA`:n$r9T$J$C$?(J */

/* $B%(%i!<HV9f(J */
extern int	jcErrno;	/* $B%(%i!<HV9f(J */

/* $B%G!<%?%?%$%W(J */
typedef struct {
	wchar_t	*kanap;		/* $BFI$_J8;zNs(J */
	wchar_t	*fzkp;		/* $BIUB08l$NFI$_J8;zNs(J */
	wchar_t	*dispp;		/* $BI=<(J8;zNs(J */
	int	dicno;
	int	entry;
	int	hinshi;
	int	kangovect;
	char	conv;		/* $BJQ49:Q$_$+(J */
				/* 0: $BL$JQ49(J 1: $BJQ49:Q(J -1: jclib$B$G5?;wJQ49(J */
	char	ltop;		/* $BBgJ8@a$N@hF,$+(J? */
	char	imabit;		/* $B:#;H$C$?$h%S%C%H(J */
} jcClause;

typedef struct {
    /* public member */
	int		nClause;	/* $BJ8@a?t(J */
	int		curClause;	/* $B%+%l%s%HJ8@aHV9f(J */
	int		curLCStart;	/* $B%+%l%s%HBgJ8@a3+;OJ8@aHV9f(J */
	int		curLCEnd;	/* $B%+%l%s%HBgJ8@a=*N;J8@aHV9f(J */
	wchar_t		*kanaBuf;	/* $B$+$J%P%C%U%!(J */
	wchar_t		*kanaEnd;
	wchar_t		*displayBuf;	/* $B%G%#%9%W%l%$%P%C%U%!(J */
	wchar_t		*displayEnd;
	jcClause	*clauseInfo;	/* $BJ8@a>pJs(J */
	struct wnn_env	*env;
    /* private member */
	int		fixed;		/* $B3NDj$5$l$?$+$I$&$+(J */
	wchar_t		*dot;		/* $B%I%C%H$N0LCV(J */
	int		candKind;	/* $BBgJ8@a$N8uJd$+>.J8@a$N8uJd$+$rI=$9%U%i%0(J */
	int		candClause;	/* $B<!8uJd$NJ8@aHV9f(J */
	int		candClauseEnd;	/* $BBgJ8@a$N<!8uJd$N;~!"=*N;J8@aHV9f(J */
	int		nCand;		/* $B<!8uJd$N?t(J */
	int		curCand;	/* $B%+%l%s%H$N<!8uJdHV9f(J */
	char		*candBuf;	/* opaque */
	int		nReset;		/* $B:#;H$C$?$h%S%C%H$rMn$9$Y$-(J
					 * $B%(%s%H%j$N?t(J */
	char		*resetBuf;	/* opaque */
	int		bufferSize;	/* kanaBuf/displayBuf $B$NBg$-$5(J */
	int		clauseSize;	/* clauseInfo $B$NBg$-$5(J */
	int		candSize;	/* candBuf $B$NBg$-$5(J */
	int		resetSize;	/* resetBuf $B$NBg$-$5(J */
} jcConvBuf;

#ifdef __STDC__
extern jcConvBuf *jcCreateBuffer(struct wnn_env *env,
				 int nclause, int buffersize);
extern int jcDestroyBuffer(jcConvBuf *buf, int savedic);
extern int jcClear(jcConvBuf *buf);
extern int jcInsertChar(jcConvBuf *buf, int c);
extern int jcDeleteChar(jcConvBuf *buf, int prev);
extern int jcConvert(jcConvBuf *buf, int small, int tan, int jump);
extern int jcUnconvert(jcConvBuf *buf);
extern int jcExpand(jcConvBuf *buf, int small, int convf);
extern int jcShrink(jcConvBuf *buf, int small, int convf);
extern int jcKana(jcConvBuf *buf, int small, int kind);
extern int jcFix(jcConvBuf *buf);
extern int jcNext(jcConvBuf *buf, int small, int prev);
extern int jcCandidateInfo(jcConvBuf *buf, int small,
			   int *ncandp, int *curcandp);
extern int jcGetCandidate(jcConvBuf *buf, int n, wchar_t *candstr);
extern int jcSelect(jcConvBuf *buf, int n);
extern int jcDotOffset(jcConvBuf *buf);
extern int jcIsConverted(jcConvBuf *buf, int cl);
extern int jcMove(jcConvBuf *buf, int small, int dir);
extern int jcTop(jcConvBuf *buf);
extern int jcBottom(jcConvBuf *buf);
extern int jcChangeClause(jcConvBuf *buf, wchar_t *str);
extern int jcSaveDic(jcConvBuf *buf);
#else
extern jcConvBuf *jcCreateBuffer();
extern int jcDestroyBuffer();
extern int jcClear();
extern int jcInsertChar();
extern int jcDeleteChar();
extern int jcConvert();
extern int jcUnconvert();
extern int jcExpand();
extern int jcShrink();
extern int jcKana();
extern int jcFix();
extern int jcNext();
extern int jcCandidateInfo();
extern int jcGetCandidate();
extern int jcSelect();
extern int jcDotOffset();
extern int jcIsConverted();
extern int jcMove();
extern int jcTop();
extern int jcBottom();
extern int jcChangeClause();
extern int jcSaveDic();
#endif

#endif /* _jclib_h */
