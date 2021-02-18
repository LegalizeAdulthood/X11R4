/*
 *	jclib -- $B$+$J4A;zJQ49MQ%i%$%V%i%j(J (Wnn Version4.0 $BBP1~HG(J)
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
 *		ishisone@sra.co.jp
 */

/*
 * Portability issue:
 *	+ define SYSV (or USG) if you don't have bcopy() or bzero().
 *
 *	+ jclib assumes that bcopy() (or memcpy()) can handle overlapping
 * 	  data blocks as xterm does.
 *	  If your bcopy can't, you should define OVERLAP_BCOPY,
 *	  which force to use my own bcopy() rather than the
 *	  one in libc.a. (but it's slow...)
 */

/*
 * $B35MW(J
 *
 * jclib $B$O(J Wnn $B$NF|K\8lF~NO%i%$%V%i%j(J jslib $B$N>e$K:n$i$l$?(J
 * ($BHf3SE*(J)$B9b%l%Y%k$N%i%$%V%i%j$G$"$k!#$+$J4A;zJQ49ItJ,$@$1$,(J
 * $BMQ0U$5$l$F$$$k$N$G!"$=$NB><-=q$K4X$9$kA`:n$J$I$O(J
 * $BD>@\(J jslib $B$r8F$S=P$9$3$H$K$J$k!#(J
 *
 * jclib $B$O$+$J%P%C%U%!$H%G%#%9%W%l%$%P%C%U%!$H$$$&#2$D$N%P%C%U%!$r;}$D!#(J
 * $B$+$J%P%C%U%!$K$OFI$_J8;zNs$,F~$j!"%G%#%9%W%l%$%P%C%U%!$K$OJQ497k2L(J
 * ($BI=<(J8;zNs(J)$B$,F~$k!#$+$J%P%C%U%!$H8@$&8F$SJ}$O$"$^$j@53N$G$O$J$$!#(J
 * Wnn Version 4 $B$G$O4A;z$+$JJQ49$b$G$-$k$+$i$G$"$k!#(J
 *
 * $B%I%C%H$H%+%l%s%HJ8@a$H$$$&35G0$r;}$A!"J8;z$NA^F~(J / $B:o=|$O%I%C%H$N0LCV$K(J
 * $BBP$7$F9T$J$o$l!"JQ49$=$NB>$NA`:n$O%+%l%s%HJ8@a$KBP$7$F9T$J$o$l$k!#(J
 * Wnn Version 4 $B$G$OBgJ8@a$H>.J8@a$H$$$&#2<oN`$NJ8@a$N35G0$,$"$k$,!"(J
 * $B$=$l$KBP1~$7$F(J jclib $B$b$3$N#2<oN`$r07$&$3$H$,$G$-$k!#(J
 *
 * $B$3$N%i%$%V%i%j$O<!$N$h$&$J5!G=$rDs6!$9$k!#(J
 *	$B!&$+$J%P%C%U%!$X$NJ8;z$NA^F~(J / $B:o=|(J
 *	$B!&$+$J4A;zJQ49(J / $B:FJQ49(J / $BL5JQ49(J
 *	$B!&$R$i$,$J"N%+%?%+%JJQ49(J
 *	$B!&3NDj(J
 *	$B!&J8@a$N3HBg(J / $B=L>.(J
 *	$B!&%+%l%s%HJ8@a(J / $B%I%C%H$N0\F0(J
 *	$B!&<!8uJd(J/$BA08uJdCV$-49$((J
 *	$B!&8uJd<h$j=P$7(J / $BA*Br(J
 *	$B!&%P%C%U%!$N%/%j%"(J
 *
 * $BJ8;z%3!<%I$H$7$F$O(J Wnn $B$HF1$8$/(J EUC $BFbIt%3!<%I(J (process code) $B$r;HMQ$9$k!#(J
 */

/*
 * Wnn Version 4 $BBP1~$K$"$?$C$F(J
 *
 * jclib $B$O$b$H$b$H(J Wnn Version 3 $B$N(J libjd $B$N>e$K:n$i$l$?%i%$%V%i%j$G$"$k!#(J
 * $B$3$l$r$r(J ver4 $BBP1~$K$9$k$K$"$?$C$F!"$$$/$D$+$NJ}?K$rN)$F$?!#(J
 *
 * 1. $B$+$J%P%C%U%!$H%G%#%9%W%l%$%P%C%U%!$NFs$D$NJ8;z%P%C%U%!$r;}$A!"(J
 * $B$+$J%P%C%U%!$K$OFI$_!"%G%#%9%W%l%$%P%C%U%!$K$OJQ497k2L$,F~$k$H$+(J
 * $BMM!9$JA`:n$O%+%l%s%HJ8@a$H8F$P$l$kJ8@a$KBP$7$F9T$J$o$l$k$H$+$$$C$?(J
 * $B4pK\E*$J%3%s%;%W%H$OJQ$($J$$!#(J
 *
 * 2. $B@N$N%i%$%V%i%j$r;H$C$?%"%W%j%1!<%7%g%s$,?7$7$$%i%$%V%i%j$K(J
 * $B0\9T$7$d$9$$$h$&$K!"%U%!%s%/%7%g%s%$%s%?!<%U%'%$%9$b$G$-$k$@$1(J
 * $B;w$?$b$N$K$9$k!#(J
 *
 * 3. 1,2 $B$NJ}?K$r$G$-$k$@$1<i$j$D$D!"(Jversion4.0 $B$GF3F~$5$l$?<!$N$h$&$J(J
 * $B5!G=$r%5%]!<%H$9$k!#(J
 *	$B!&4D6-(J
 *	$B!&%^%k%A%P%C%U%!(J ($BJQ49%P%C%U%!$,J#?t;}$F$k$h$&$K$9$k(J)
 *	$B!&BgJ8@a(J / $B>.J8@a(J ($BBgJ8@a$r4pK\$H$9$k$,!">.J8@a$b07$($k$h$&$K$9$k(J)
 *
 * 4. 1 $B$+$i(J 3 $B$^$G$NJ}?K$K=>$$$D$D!"$G$-$k$@$15^$$$G:n$k(J :-)
 */

/*
 * $B%a%b(J
 *
 * ver 0.0	89/07/21
 *	$B$H$j$"$($::n$j$O$8$a$k(J
 * ver 0.1	89/08/02
 *	$BH>J,$/$i$$$+$1$?(J
 *	$B<!8uJd4XO"$,$^$@$G$-$F$$$J$$(J
 * ver 0.2	89/08/04
 *	jcInsertChar() / jcDeleteChar() $B$r:n@.(J
 * ver 0.3	89/08/07
 *	$B0l1~$G$-$?(J
 *	$B$^$@$$$/$D$+5?LdE@$,$"$k$1$l$I(J
 * ver 0.4	89/08/08
 *	$B:#;H$C$?$h%S%C%H$N07$$$r;D$7$F!"$[$\$G$-$?$N$G$O$J$$$+$H(J
 *	$B;W$o$l$k(J
 *	$B:Y$+$$%P%0$r$+$J$j=$@5(J
 * ver 0.5	89/08/09
 *	$BN)LZ$5$s(J@KABA $B$K<ALd$7$?=j!":#;H$C$?$h%S%C%H$rMn$9$N$b(J
 *	$B%/%i%$%"%s%HB&$N@UG$$G$"$k$3$H$,$o$+$k(J
 *	$B$3$l$X$NBP1~(J
 *	$B$D$$$G$K%G!<%?9=B$$N@bL@$rDI2C(J
 *	$B%U%!%$%k$N%5%$%:$,(J 80KB $B$r1[$($F$7$^$C$?(J
 *	$B%3%a%s%H$r$H$l$P$+$J$j>.$5$/$J$k$s$@$1$I(J
 * ver 0.6	89/08/22
 *	jcDeleteChar() $B$rA4LLE*$K=q$-D>$9(J
 *	$B$3$l$G0l1~@5$7$/F0:n$9$k$h$&$K$J$C$?(J
 *	jcInsertChar() $B$G:G8e$N(J clauseInfo $B$N@_Dj$,4V0c$C$F$$$?$N$G(J
 *	$B$=$l$r=$@5(J
 *	jcPrintDetail() $B$K4JC1$J(J clauseInfo $B%G!<%?$N(J consistency check $B$r(J
 *	$BF~$l$k(J
 * ver 0.7	89/08/26
 *	jcExpand() $B$N%P%0=$@5(J
 *	$B>.J8@a$NC1J8@aJQ49$r>/$7=$@5(J
 * ver 0.8	89/08/30
 *	changecinfo() $B$G(J conv $B%U%i%0$r%;%C%H$9$k$N$rK:$l$F$$$?(J
 *	moveKBuf()/moveDBuf()/moveCInfo() $B$r>/$7=$@5(J
 *	SYSV $B$,(J define $B$5$l$F$$$l$P(J bcopy()/bzero() $B$NBe$o$j$K(J
 *	memcpy()/memset() $B$r;H$&$h$&$K=$@5(J
 * ver 0.9	89/09/22
 *	setLCandData() $B$G<!8uJd%P%C%U%!$N8uJd?t$K%+%l%s%HBgJ8@a$N(J
 *	$BJ,$r2C$($k$N$rK:$l$F$$$?(J
 * ver 0.10	89/10/16
 *	wnn-4.0.1 $B$G(J commonheader.h -> commonhd.h $B$K$J$C$?$N$G(J
 *	$B$=$l$N=$@5(J
 * ver 0.11	89/10/18
 *	USG $B$,(J define $B$5$l$F$$$F$b(J memcpy()/memset() $B$r;H$&$h$&$K=$@5(J
 * ver 0.12	89/10/19
 *	resizeBuffer() $B$G%I%C%H$N:F@_Dj$rK:$l$F$$$k$H$$$&=EBg$J%P%0$r=$@5(J
 * ver 4.0	89/10/27
 *	$B%P!<%8%g%sHV9f$r=$@5$7$F(J 4.0 $B$K$9$k!#(J
 */

/*
 * $B%U%!%s%/%7%g%s(J
 *
 * jcConvBuf *jcCreateBuffer(struct wnn_env *env, int nclause, int buffersize)
 *	$B;XDj$5$l$?4D6-$r;H$C$FJQ49$N%P%C%U%!$r:n@.$9$k!#%P%C%U%!$O(J
 *	$BJ#?t:n$k$3$H$,$G$-$k!#0l$D$N%P%C%U%!$G$OF1;~$KJ#?t$NJ8$r(J
 *	$BJQ49$9$k$3$H$O$G$-$J$$$N$G!"J#?t$NJ8$rJB9T$7$FJQ49$7$?$$>l9g$K$O(J
 *	$B4v$D$+$N%P%C%U%!$rMQ0U$7$J$/$F$O$J$i$J$$!#(J
 *	$B4D6-$N@_Dj$^$G$rM=$a$d$C$F$*$/I,MW$,$"$k!#$D$^$j%5!<%P$H$N@\B3!"(J
 *	$B4D6-$N@8@.!"<-=q$N@_Dj$J$I$O(J jclib $B$N<iHwHO0O$G$O$J$$!#(J
 *	$B0z?t$N(J nclause $B$H(J buffersize $B$G!"$=$l$>$l=i4|2=;~$K%"%m%1!<%H$9$k(J
 *	$BJ8@a>pJs$*$h$S$+$J!&%G%#%9%W%l%$%P%C%U%!$NBg$-$5$,;XDj$G$-$k!#(J
 *	$B$?$@$7$3$l$i$O!"%5%$%:$,B-$j$J$/$J$l$PI,MW$K1~$8$F<+F0E*$K(J
 *	$BA}$d$5$l$k$?$a!"$3$3$K;XDj$7$?0J>e$N?t$NJ8@a$d!"J8;zNs$,JQ49$G$-$J$$(J
 *	$B$o$1$G$O$J$$!#$=$l$>$l(J 0 $B$^$?$OIi$NCM$r;XDj$9$k$H!"%G%U%)%k%H$N(J
 *	$B%5%$%:$G%"%m%1!<%H$5$l$k!#=>$C$FDL>o$O(J nclause/buffersize $B$H$b(J
 *	0 $B$r;XDj$7$F$*$1$P$h$$!#(J
 *	$B%j%?!<%s%P%j%e!<$H$7$F%P%C%U%!$rJV$9!#%(%i!<$N;~$K$O(J NULL $B$,(J
 *	$BJV$5$l$k!#(J
 *
 * int jcDestroyBuffer(jcConvBuf *buf, int savedic)
 *	$B%P%C%U%!$N;HMQ$r=*N;$9$k!#4D6-$r>C$7$?$j!"%5!<%P$H$N@\B3$r@Z$C$?$j(J
 *	$B$9$k$3$H$O(J jclib $B$N<iHwHO0O30$G$"$k!#(J
 *	$B0z?t(J savedic $B$,(J 0 $B$G$J$1$l$P!"4D6-Cf$G;HMQ$5$l$F$$$kA4$F$N<-=q$r(J
 *	$B%;!<%V$9$k!#(J
 *
 * int jcClear(jcConvBuf *buf)
 *	$B%P%C%U%!$r%/%j%"$9$k!#?7$?$KJQ49$r;O$a$k:]$K$O:G=i$K$3$N(J
 *	$B%U%!%s%/%7%g%s$r8F$P$J$1$l$P$J$i$J$$!#(J
 *
 * int jcInsertChar(jcConvBuf *buf, int c)
 *	$B%I%C%H$K#1J8;zA^F~$9$k!#(J
 *	$B%+%l%s%HJ8@a$,4{$KJQ49$5$l$F$$$l$PL5JQ49$N>uBV$KLa$k!#(J
 *	$B%+%l%s%HJ8@a$OBgJ8@a$G$"$k!#(J
 *
 * int jcDeleteChar(jcConvBuf *buf, int prev)
 *	$B%I%C%H$NA0Kt$O8e$m$N#1J8;z$r:o=|$9$k!#(J
 *	$B%+%l%s%HJ8@a$,4{$KJQ49$5$l$F$$$l$PL5JQ49$N>uBV$KLa$k!#(J
 *	$B%+%l%s%HJ8@a$OBgJ8@a$G$"$k!#(J
 *
 * int jcConvert(jcConvBuf *buf, int small, int tan, int jump)
 *	$B%+%l%s%HJ8@a$+$i8e$m$rJQ49$9$k!#(J
 *	$B0z?t(J tan $B$,(J 0 $B$J$iO"J8@aJQ49!"$=$&$G$J$1$l$P%+%l%s%HJ8@a$r(J
 *	$BC1J8@aJQ49$7!"$=$N$"$H$rO"J8@aJQ49$9$k!#(J
 *	$B0z?t(J small $B$,(J 0 $B$G$J$1$l$P>.J8@a$,!"$=$&$G$J$1$l$PBgJ8@a$,(J
 *	$B%+%l%s%HJ8@a$H$7$F;H$o$l$k!#(J
 *	$B0z?t(J jump $B$G!"JQ498e$N%+%l%s%HJ8@a$N0LCV$,7h$^$k!#(Jjump $B$,(J
 *	0 $B$J$i%+%l%s%HJ8@a$N0LCV$OJQ49$7$F$bJQ$o$i$J$$(J ($B$?$@$7(J
 *	$B%+%l%s%HJ8@a$H$7$FBgJ8@a$r;XDj$7$?>l9g!"JQ498e$N%+%l%s%H(J
 *	$B>.J8@a$O%+%l%s%HBgJ8@a$N:G=i$N>.J8@a$K$J$k(J) $B$,!"(J0 $B$G$J$1$l$P(J
 *	$B:G8e$NJ8@a$N<!(J ($B6uJ8@a(J) $B$K0\F0$9$k!#C`<!JQ49$7$F$$$/$h$&$J(J
 *	$B%"%W%j%1!<%7%g%s$G$O$3$l$r(J 1 $B$K$9$k$H$h$$$@$m$&!#(J
 *
 * int jcUnconvert(jcConvBuf *buf)
 *	$B%+%l%s%HBgJ8@a$rL5JQ49$N>uBV$KLa$9!#(J
 *	$B%+%l%s%HBgJ8@a$,$$$/$D$+$N>.J8@a$+$i$G$-$F$$$?>l9g!"$3$l$i$N(J
 *	$B>.J8@a$O$^$H$a$i$l!"0l$D$NL5JQ49>uBV$NJ8@a$K$J$k!#(J
 *	$B%+%l%s%H>.J8@a$rL5JQ49$KLa$95!G=$OMQ0U$7$J$$!#$J$<$+$H$$$&$H!"(J
 *	$BBgJ8@a$NCf$N(J 1 $B>.J8@a$N$_$,L5JQ49$K$J$C$F$7$^$&$H!"$=$NJ8@a$K(J
 *	$B4X$7$F(J jcMove() $B$G0\F0$r9T$J$C$?;~!"$I$&0\F0$9$l$P$h$$$N$+(J
 *	$B$h$/$o$+$i$J$$!"$D$^$j0\F0$N%;%^%s%F%#%/%9$,ITL@3N$K$J$C$F$7$^$&(J
 *	$B$+$i$G$"$k!#(J
 *
 * int jcKana(jcConvBuf *buf, int small, int kind)
 *	$B%+%l%s%HJ8@a$r$+$J$K$9$k!#(J
 *	$B0z?t(J kind $B$,!"(JJC_HIRAGANA $B$J$i$R$i$,$J!"(JJC_KATAKANA $B$J$i%+%?%+%J$K(J
 *	$BJQ$o$k!#J8@a$NJQ49>uBV$OJQ2=$7$J$$!#$D$^$jJQ49$5$l$F$$$l$P(J
 *	$BJQ49>uBV$N$^$^!"L$JQ49$N>uBV$J$iL$JQ49$N$^$^$G$"$k!#(J
 *	$B0z?t(J small $B$,(J 0 $B$G$J$1$l$P%+%l%s%H>.J8@a$,!"$=$&$G$J$1$l$P(J
 *	$B%+%l%s%HBgJ8@a$,JQ$o$k!#(J
 *	$B%+%l%s%HBgJ8@a$r$+$J$K$9$k>l9g!"$=$NCf$N>.J8@a$O0l$D$K$^$H$a$i$l$k!#(J
 *
 * int jcFix(jcConvBuf *buf)
 *	$B8=:_!"%P%C%U%!$K$O$$$C$F$$$kJQ49J8;zNs$r3NDj$5$;$k!#(J
 *
 * int jcExpand(jcConvBuf *buf, int small, int convf)
 *	$B%+%l%s%HJ8@a$ND9$5$r#1J8;z?-$P$9!#0z?t(J convf $B$,(J 0 $B$G$J$1$l$P(J
 *	$B?-$P$7$?$"$H:FJQ49$9$k!#(J
 *	$B0z?t(J small $B$,(J 0 $B$G$J$1$l$P>.J8@a$,!"$=$&$G$J$1$l$PBgJ8@a$,(J
 *	$B%+%l%s%HJ8@a$H$7$F;H$o$l$k!#(J
 *
 * int jcShrink(jcConvBuf *buf, int small, int convf)
 *	$B%+%l%s%HJ8@a$ND9$5$r#1J8;z=L$a$k!#0z?t(J convf $B$,(J 0 $B$G$J$1$l$P(J
 *	$B=L$a$?$"$H:FJQ49$9$k!#(J
 *	$B0z?t(J small $B$,(J 0 $B$G$J$1$l$P>.J8@a$,!"$=$&$G$J$1$l$PBgJ8@a$,(J
 *	$B%+%l%s%HJ8@a$H$7$F;H$o$l$k!#(J
 *
 * int jcNext(jcConvBuf *buf, int small, int prev)
 *	$B%+%l%s%HJ8@a$r<!8uJdKt$OA08uJd$GCV$-49$($k!#(J
 *	$B0z?t(J small $B$,(J 0 $B$G$J$1$l$P>.J8@a$,!"$=$&$G$J$1$l$PBgJ8@a$,(J
 *	$B%+%l%s%HJ8@a$H$7$F;H$o$l$k!#(J
 *
 * int jcCandidateInfo(jcConvBuf *buf, int small, int *ncandp, int *curcandp)
 *	$B<!8uJd$N>pJs$rJV$9!#(J
 *	$B<!8uJd0lMw$r=P$9$?$a$K$O:G=i$K$3$N4X?t$r8F$V$H$h$$!#(J
 *
 * int jcGetCandidate(jcConvBuf *buf, int n, wchar_t *candstr)
 *	$B;XDj$5$l$?8uJdHV9f$NJ8;zNs$rJV$9!#%+%l%s%H8uJdHV9f$O$3$NHV9f$K(J
 *	$BJQ$o$k!#%G%#%9%W%l%$%P%C%U%!$OJQ2=$7$J$$!#(J
 *	$B@N$N(J jclib $B$O<!8uJd$,MQ0U$5$l$F$$$J$1$l$PMQ0U$7$?$,!"$3$N%P!<%8%g%s(J
 *	$B$G$O%(%i!<$K$J$k!#(JjcNext $B$d(J jcCandidateInfo $B$r@h$K8F$s$G$*$+$J$1$l$P(J
 *	$B$J$i$J$$!#(J
 *
 * int jcSelect(jcConvBuf *buf, int n)
 *	$B;XDj$5$l$?HV9f$N8uJd$G%G%#%9%W%l%$%P%C%U%!$rCV$-49$($k!#(J
 *	$B%+%l%s%H8uJdHV9f$O$3$NHV9f$KJQ$o$k!#(J
 *
 * int jcDotOffset(jcConvBuf *buf)
 *	$BBgJ8@a$N@hF,$+$i$N%I%C%H$N%*%U%;%C%H$rJV$9!#(J
 *	$BNc$($P(J 0 $B$J$i%I%C%H$,%+%l%s%HJ8@a$N@hF,$K$"$k$3$H$K$J$k!#(J
 *
 * int jcIsConverted(jcConvBuf *buf, int cl)
 *	$B;XDj$5$l$?J8@a$,JQ49$5$l$F$$$k$+$I$&$+$rJV$9(J
 *	0 $B$J$iL5JQ49>uBV(J
 *	1 $B$J$iJQ49>uBV(J
 *	-1 $B$J$i(J $B%(%i!<(J
 *
 * int jcMove(jcConvBuf *buf, int small, int dir)
 *	$B%I%C%H!&%+%l%s%HJ8@a$r0\F0$9$k!#(J
 *	$B%+%l%s%HJ8@a$,JQ49:Q$_$G$"$l$PJ8@a0\F0$7!"$=$&$G$J$1$l$P(J
 *	$B%I%C%H$N$_$,0\F0$9$k!#(J
 *	$BJ8@a0\F0;~$K!"0z?t(J small $B$,(J 0 $B$G$J$1$l$P>.J8@aC10L$G0\F0$7!"(J
 *	$B$=$&$G$J$1$l$PBgJ8@aC10L$K0\F0$9$k!#(J
 *
 * int jcTop(jcConvBuf *buf)
 *	$B%I%C%H!&%+%l%s%HJ8@a$rJ8$N@hF,$K0\F0$9$k!#%+%l%s%H>.J8@a!&(J
 *	$B%+%l%s%HBgJ8@a$H$b$K0\F0$9$k!#(J
 *
 * int jcBottom(jcConvBuf *buf)
 *	$B%I%C%H!&%+%l%s%HJ8@a$rJ8$N:G8e$K0\F0$9$k!#%+%l%s%H>.J8@a!&(J
 *	$B%+%l%s%HBgJ8@a$H$b$K0\F0$9$k!#(J
 *	$B$b$7!":G8e$NJ8@a$,L5JQ49>uBV$G$"$l$P%+%l%s%HJ8@a$O$=$NJ8@a$K$J$j!"(J
 *	$B%I%C%H$O$=$NJ8@a$N:G8e$KMh$k!#$=$&$G$J$1$l$P%+%l%s%HJ8@a$O(J
 *	$B:G8e$NJ8@a$N<!(J ($B$D$^$j6u$NJ8@a(J) $B$KMh$k!#(J
 *
 * int jcChangeClause(jcConvBuf *buf, wchar_t *str)
 *	$B%+%l%s%HBgJ8@a$r;XDj$5$l$?J8;zNs$GF~$l49$($k!#(J
 *	$B%G%#%9%W%l%$%P%C%U%!$@$1$G$O$J$/!"$+$J%P%C%U%!$NFbMF$b(J
 *	$BCV$-49$o$k!#J8@a$OL5JQ49>uBV$K$J$k!#(J
 *
 * int jcSaveDic(jcConvBuf *buf)
 *	$B;HMQCf$N4D6-$G;H$o$l$F$$$kA4$F$N<-=qJB$S$KIQEY%U%!%$%k$r(J
 *	$B%;!<%V$9$k!#(J
 *	$B$3$N%U%!%s%/%7%g%s$O>o$K(J 0 $B$rJV$9!#K\Ev$K%;!<%V$5$l$?$+$N(J
 *	$B%A%'%C%/$O$7$J$$!#(J
 *
 * $B$3$l$i$N%U%!%s%/%7%g%s$OFC$K=q$+$l$F$$$J$1$l$P@.8y$N>l9g$K$O(J 0,
 * $B%(%i!<$N>l9g$K$O(J -1 $B$rJV$9!#(J
 */

/*
 * $B%0%m!<%P%kJQ?t(J
 *
 * jclib $B$G;H$o$l$k%0%m!<%P%kJQ?t$O(J jcErrno $B$?$@0l$D$G$"$k!#(J
 *
 * extern int jcErrno
 *	$B%(%i!<$N:]$K!"%(%i!<%3!<%I$,BeF~$5$l$k!#%(%i!<%3!<%I$O(J jclib.h $B$G(J
 *	$BDj5A$5$l$F$$$k!#(J
 */

/*
 * $B%G!<%?9=B$(J
 *
 * jclib $B$N;}$D%G!<%?$G!"%"%W%j%1!<%7%g%s$+$iD>@\%"%/%;%9$7$F$h$$$N$O(J
 * $BJQ49%P%C%U%!(J jcConvBuf $B7?$N(J public member $B$H=q$+$l$?ItJ,$N$_$G$"$k!#(J
 * $BD>@\%"%/%;%9$7$F$h$$$H$$$C$F$b!"CM$r;2>H$9$k$@$1$G!"CM$rJQ99$9$k$3$H$O(J
 * $B5v$5$l$J$$!#%"%W%j%1!<%7%g%s$,>!<j$KCM$rJQ99$7$?>l9g$N(J jclib $B$NF0:n$O(J
 * $BJ]>Z$5$l$J$$!#(J
 *
 * <$BJQ49%P%C%U%!(J>
 *
 * jcConvBuf $B7?$O(J jclib.h $B$G<!$N$h$&$KDj5A$5$l$F$$$k!#(J
 *
 * typedef struct {
 *    /-* public member *-/
 *	int		nClause;	$BJ8@a?t(J
 *	int		curClause;	$B%+%l%s%HJ8@aHV9f(J
 *	int		curLCStart;	$B%+%l%s%HBgJ8@a3+;OJ8@aHV9f(J
 *	int		curLCEnd;	$B%+%l%s%HBgJ8@a=*N;J8@aHV9f(J
 *	wchar_t		*kanaBuf;	$B$+$J%P%C%U%!(J
 *	wchar_t		*kanaEnd;
 *	wchar_t		*displayBuf;	$B%G%#%9%W%l%$%P%C%U%!(J
 *	wchar_t		*displayEnd;
 *	jcClause	*clauseInfo;	$BJ8@a>pJs(J
 *	struct wnn_env	*env;
 *    /-* private member *-/
 *	[ $B>JN,(J ]
 * } jcConvBuf;
 *
 * nClause $B$O8=:_$NJ8@a?t$rI=$9!#$3$l$O>.J8@a$N?t$G$"$k!#(J
 * curClause $B$O%+%l%s%H>.J8@a$NHV9f$G$"$k!#(J
 * curLCStart $B$H(J curLCEnd $B$O%+%l%s%HBgJ8@a$NHO0O$r<($9!#(JcurLCStart $B$+$i(J
 * curLCEnd-1 $B$NHO0O$NJ8@a$,%+%l%s%HBgJ8@a$G$"$k!#$D$^$j!"(JcurLCEnd $B$O(J
 * $B<!$NBgJ8@a$N@hF,$NHV9f$G$"$k!#(J
 *
 * kanaBuf $B$H(J displayBuf $B$,$+$J%P%C%U%!$H%G%#%9%W%l%$%P%C%U%!$G$"$k!#(J
 * jcInsertChar() $BEy$r;H$C$FF~NO$5$l$?FI$_$O$+$J%P%C%U%!$H%G%#%9%W%l%$(J
 * $B%P%C%U%!$KF~$k!#$3$l$rJQ49$9$k$H!"%G%#%9%W%l%$%P%C%U%!$NJ}$@$1$,(J
 * $B4A;z$NJ8;zNs$K$J$k!#(J
 * kanaEnd $B$*$h$S(J displayEnd $B$O$=$l$>$l$N%P%C%U%!$KF~$l$i$l$?J8;zNs$N:G8e(J
 * $B$NJ8;z$N<!$r;X$7$F$$$k!#$+$J%P%C%U%!!&%G%#%9%W%l%$%P%C%U%!$O$I$A$i$b(J
 * NULL $B%?!<%_%M!<%H$5$l$J$$!#(J
 *
 * clauseInfo $B$OJ8@a>pJs$NF~$C$?G[Ns$G$"$k!#$3$l$O$"$H$G@bL@$9$k!#(J
 *
 * env $B$O$3$NJQ49%P%C%U%!$N;HMQ$9$k4D6-$G$"$k!#(J
 *
 * <$BJ8@a>pJs(J>
 *
 * $B3FJ8@a$N>pJs$O(J clauseInfo $B$H$$$&L>A0$N(J jcClause $B7?$NG[Ns$KF~$C$F$$$k!#(J
 * jcClause $B7?$O(J jclib.h $B$G<!$N$h$&$KDj5A$5$l$F$$$k!#(J
 *
 * typedef struct {
 *	wchar_t	*kanap;		$BFI$_J8;zNs(J ($B$+$J%P%C%U%!$NCf$r;X$9(J)
 *	wchar_t	*fzkp;		$BIUB08l$NFI$_J8;zNs(J ($B$+$J%P%C%U%!$NCf$r;X$9(J)
 *	wchar_t	*dispp;		$BI=<(J8;zNs(J ($B%G%#%9%W%l%$%P%C%U%!$NCf$r;X$9(J)
 *	int	dicno;		$B<-=qHV9f(J
 *	int	entry;		$B%(%s%H%j(J
 *	int	hinshi;		$BIJ;lHV9f(J
 *	int	kangovect;	$BA0C<$N@\B3%Y%/%H%k(J
 *	char	conv;		$BJQ49:Q$_$+(J
 *				0: $BL$JQ49(J 1: $BJQ49:Q(J -1: jclib$B$G5?;wJQ49(J
 *	char	ltop;		$BBgJ8@a$N@hF,$+(J?
 *	char	imabit;		$B:#;H$C$?$h%S%C%H(J
 * } jcClause;
 *
 * kanap, fzkp $B$O$+$J%P%C%U%!>e$N!"$=$NJ8@a$NFI$_$N;O$^$j$H!"IUB08l$N(J
 * $B;O$^$j$N0LCV$r<($9%]%$%s%?$G$"$k!#$^$?!"(Jdispp $B$O!"%G%#%9%W%l%$%P%C%U%!(J
 * $B>e$G!"$=$NJ8@a$N;O$^$j$N0LCV$r<($9!#=>$C$F!"(Jn $BHV$NJ8@a$O!"(J
 *	$B$h$_(J:	clauseInfo[n].kanap $B$+$i(J clauseInfo[n+1].kanap $B$NA0$^$G(J
 *	$BIUB08l(J:	clauseInfo[n].fzkp $B$+$i(J clauseInfo[n+1].kanap $B$NA0$^$G(J
 *	    ($BIUB08l$,$J$1$l$P(J clauseInfo[n].fzkp == clauseInfo[n+1].kanap $B$K(J
 *	     $B$J$k(J)
 *	$B4A;z(J:	clauseInfo[n].dispp $B$+$i(J clauseInfo[n+1].dispp $B$NA0$^$G(J
 * $B$H$J$k!#$3$N$h$&$K(J n $BHVL\$NJ8@a$NHO0O$r<($9$N$K(J n+1 $BHVL\$N(J clauseInfo $B$,(J
 * $BI,MW$J$?$a!"(JclauseInfo $B$NG[Ns$NMWAG$O>o$K@hF,$+$iJ8@a?t(J+1$B8D$,M-8z$G$"$k!#(J
 *
 * dicno, entry, hinshi, kangovect $B$K$D$$$F$O(J Wnn $B$N%I%-%e%a%s%H$r;2>H$N$3$H!#(J
 *
 * conv $B$O$=$NJ8@a$NJQ49>uBV$rI=$9!#(J0 $B$J$iL$JQ49>uBV!"(J1 $B$J$iJQ49>uBV!"(J
 * -1 $B$J$i(J jcKana() $B$K$h$C$F5?;wJQ49$5$l$?$3$H$r<($9!#(J
 *
 * ltop $B$,(J 0 $B$G$J$1$l$P$=$NJ8@a$,BgJ8@a$N@hF,$G$"$k$3$H$r<($9!#(Jimabit $B$O(J
 * $B$=$NJ8@a$N448l$N:#;H$C$?$h%S%C%H$,F~$C$F$$$k!#(J
 *
 * kanap, dispp $BEy$G!"(Jn $BHVL\$NJ8@a$NHO0O$r<($9$N$K(J n+1 $BHVL\$NJ8@a>pJs$,(J
 * $BI,MW$J$?$a!"(JclauseInfo $B$NG[Ns$NMWAG$O>o$K@hF,$+$iJ8@a?t(J+1$B8D$,M-8z$G$"$k!#(J
 * $BJ8@a?t(J+1 $B8DL\$NJ8@a>pJs(J (clauseInfo[nClause]) $B$O(J
 *	kanap, dispp: $B$=$l$>$l(J kanaEnd, displayEnd $B$KEy$7$$(J
 *	conv: 0 ($BL$JQ49>uBV(J)
 *	ltop: 1
 *	$B;D$j$N%a%s%P$OITDj(J
 * $B$G$"$k!#(J
 *
 * $BJ8@a>pJs$N(J kanap, fzkp, dispp $B$rNc$r;H$C$F<($7$F$*$/!#(J
 *
 * $BNcJ8(J: $B$3$l$O%G!<%?9=B$$r<($9$?$a$NNcJ8$G$9(J ($BJ8@a?t(J 6)
 *
 * kanap:   $B#0(J    $B#1(J    $B#2(J        $B#3(J    $B#4(J    $B#5(J          $B#6(J(=kanaEnd)
 *	    $B"-(J    $B"-(J    $B"-(J        $B"-(J    $B"-(J    $B"-(J          $B"-(J
 * kanaBuf: $B$3$l$O$G!<$?$3$&$>$&$r$7$a$9$?$a$N$l$$$V$s$G$9(J
 *		$B",(J      $B",(J      $B",(J    $B",(J    $B",(J        $B",(J
 * fzkp:	$B#0(J      $B#1(J      $B#2(J    $B#3(J    $B#4(J        $B#5(J
 *
 * dispp:      $B#0(J    $B#1(J    $B#2(J    $B#3(J  $B#4(J    $B#5(J      $B#6(J(=displayEnd)
 *	       $B"-(J    $B"-(J    $B"-(J    $B"-(J  $B"-(J    $B"-(J      $B"-(J
 * displayBuf: $B$3$l$O%G!<%?9=B$$r<($9$?$a$NNcJ8$G$9(J
 */

#ifndef lint
static char	*rcsid = "$Header: jclib.c,v 1.1 89/11/02 08:29:33 jim Exp $";
#endif

#ifdef DEBUG
#include	<stdio.h>
#endif
#include	"commonhd.h"
#include	"jslib.h"
#include	"jclib.h"

#ifdef CHECK_PROTOTYPE
#include	"js.c.p"
#endif

#ifndef NULL
#define NULL	0
#endif

#define CHECKFIXED(buf)	\
	{ if ((buf)->fixed) { jcErrno = JE_ALREADYFIXED; return -1; } }
#define Free(p)		{if (p) free((char *)(p));}
#define DotSet(buf)	(buf)->dot = (buf)->clauseInfo[(buf)->curLCStart].kanap

#define KANABEG	0xa4a1	/* '$B$!(J' */
#define KANAEND	0xa4f3	/* '$B$s(J' */
#define KATAOFFSET	0x100	/* $B%+%?%+%J$H$R$i$,$J$N%3!<%I!&%*%U%;%C%H(J */

/* $B%G%U%)%k%H$N%P%C%U%!%5%$%:(J */
#define DEF_BUFFERSIZE	100	/* 100 $BJ8;z(J */
#define DEF_CLAUSESIZE	20	/* 20 $BJ8@a(J */
#define DEF_CANDSIZE	1024	/* 1K $B%P%$%H(J */
#define DEF_RESETSIZE	10	/* 10 $BC18l(J */

/* buf->candKind $B$NCM(J */
#define CAND_SMALL	0	/* $B>.J8@a8uJd(J */
#define CAND_LARGE	1	/* $BBgJ8@a8uJd(J */

#define MAXFZK	LENGTHBUNSETSU

extern char	*malloc();
extern char	*realloc();
extern void	free();
#if defined(SYSV) || defined(USG)
extern char	*memcpy();
extern char	*memset();
#define bcopy(p, q, l)	memcpy(q, p, l)
#define bzero(p, l)	memset(p, 0, l)
#else
extern int	bcopy();
extern int	bzero();
#endif

/*
 * $B<!8uJd%P%C%U%!$N%G!<%?%?%$%W(J ($B%i%$%V%i%j$N30$+$i$O8+$($J$$$h$&$K$9$k(J)
 * buf->candBuf $B$N;X$9@h$O!">.J8@a8uJd$N;~$O(J jcSCand $B$NG[Ns$G!"(J $BBgJ8@a8uJd$N(J
 * $B;~$K$O(J jcLCand $B$NG[Ns$G$"$k(J
 */
/* $B>.J8@a8uJd(J */
typedef struct {
	wchar_t	*kanji;		/* $BI=<(J8;zNs(J */
	int	kanalen;	/* $B$+$J$ND9$5(J */
	int	fzkoffset;
	int	dicno;
	int	entry;
	int	hinshi;
	int	kangovect;
	int	status;
#define	IMA_BIT		1	/* $B:#;H$C$?$h%S%C%H(J */
#define CONNECT_PREV	2	/* $BA0J8@a$K@\B3$G$-$k$+(J? */
#define CONNECT_NEXT	4	/* $B8e$m$NJ8@a$K@\B3$G$-$k$+(J? */
} jcSCand;

/* $BBgJ8@a8uJd(J */
typedef struct {
	int	nscand;		/* $B4^$^$l$k>.J8@a$N?t(J */
	jcSCand	*scand;		/* $B>.J8@a$NG[Ns(J */
} jcLCand;

/*
 * $B%j%;%C%H%P%C%U%!(J ($B:#;H$C$?$h%S%C%H$rMn$9BP>]$N%(%s%H%j$r$N%G!<%?%?%$%W(J
 * ($B%i%$%V%i%j$N30$+$i$O8+$($J$$$h$&$K$9$k(J)
 * buf->resetBuf $B$N;X$9@h$O!"(JjcEntry $B$NG[Ns$G$"$k(J
 */
typedef struct {
	int	dicno;
	int	entry;
} jcEntry;

#ifdef __STDC__
/* $B%U%!%s%/%7%g%s%W%m%H%?%$%W@k8@(J */
static int wstrlen(wchar_t *);
static void setcinfo(jcClause *, struct wnn_sho_bunsetsu *,
		     wchar_t *, wchar_t *);
static void changecinfo(jcClause *, jcSCand *, wchar_t *, wchar_t *);
static int wstrlen(wchar_t *);
static void getFVec(jcClause *, int *, wchar_t *, int *);
static void getBVec(jcClause *, int *, int *, int *);
static void moveKBuf(jcConvBuf *, int, int);
static void moveDBuf(jcConvBuf *, int, int);
static void moveCInfo(jcConvBuf *, int, int);
static int resizeBuffer(jcConvBuf *, int);
static int resizeCInfo(jcConvBuf *, int);
static int resizeCandBuf(jcConvBuf *, int);
static void setCurClause(jcConvBuf *, int);
static int renConvert(jcConvBuf *, int);
static int tanConvert(jcConvBuf *, int);
static int doKanrenConvert(jcConvBuf *, int);
static int doKantanDConvert(jcConvBuf *, int, int);
static int doKantanSConvert(jcConvBuf *, int);
static int unconvert(jcConvBuf *, int, int);
static int expandOrShrink(jcConvBuf *, int, int, int);
static int getKanjiLenDbun(struct wnn_dai_bunsetsu *, int);
static int getKanjiLenSbun(struct wnn_sho_bunsetsu *, int);
static int getSCandidates(jcConvBuf *);
static int findSCand(jcSCand *, jcSCand *, struct wnn_sho_bunsetsu *);
static int setSCandData(jcConvBuf *, int, struct wnn_sho_bunsetsu *, int);
static int getSCandDataLen(jcClause *, int, struct wnn_sho_bunsetsu *);
static int getLCandidates(jcConvBuf *);
static int findLCand(jcLCand *, jcLCand *, struct wnn_dai_bunsetsu *);
static int setLCandData(jcConvBuf *, int, struct wnn_dai_bunsetsu *);
static int getLCandDataLen(jcClause *, jcClause *, int,
			   struct wnn_dai_bunsetsu *);
static int changeCand(jcConvBuf *, int);
static int setupCandBuf(jcConvBuf *, int);
static void checkAndResetCandidates(jcConvBuf *, int, int);
static void addResetClause(jcConvBuf *, int, int);
static void addResetCandidate(jcConvBuf *, int);
static void addResetEntry(jcConvBuf *, int, int);
static void saveDicAll(jcConvBuf *);
#else
static int wstrlen();
static void setcinfo();
static void changecinfo();
static int wstrlen();
static void getFVec();
static void getBVec();
static void moveKBuf();
static void moveDBuf();
static void moveCInfo();
static int resizeBuffer();
static int resizeCInfo();
static int resizeCandBuf();
static void setCurClause();
static int renConvert();
static int tanConvert();
static int doKanrenConvert();
static int doKantanDConvert();
static int doKantanSConvert();
static int unconvert();
static int expandOrShrink();
static int getKanjiLenDbun();
static int getKanjiLenSbun();
static int getSCandidates();
static int findSCand();
static int setSCandData();
static int getSCandDataLen();
static int getLCandidates();
static int findLCand();
static int setLCandData();
static int getLCandDataLen();
static int changeCand();
static int setupCandBuf();
static void checkAndResetCandidates();
static void addResetClause();
static void addResetCandidate();
static void addResetEntry();
static void saveDicAll();
#endif

/* $B%(%i!<HV9f(J */
int	jcErrno;

static struct wnn_ret_buf	jsbuf = { 0, NULL };

/*
 *	portability $B$N$?$a$N%U%!%s%/%7%g%s(J
 */

#ifdef OVERLAP_BCOPY
#undef bcopy
static
bcopy(from, to, n)
register char *from;
register char *to;
register int n;
{
	if (n <= 0 || from == to) return;

	if (from < to) {
		from += n;
		to += n;
		while (n-- > 0)
			*--to = *--from;
	} else {
		while (n-- > 0)
			*to++ = *from++;
	}
}
#endif

/*
 *	jclib $BFbIt$G;H$o$l$k%U%!%s%/%7%g%s(J
 */

/* wstrlen -- wchar_t $B7?J8;zNs$N(J strlen */
static int
wstrlen(s)
wchar_t *s;
{
	int	n = 0;

	while (*s++)
		n++;
	return n;
}

/* setcinfo -- clauesInfo $B$K>.J8@a$N>pJs$r%;%C%H$9$k(J */
static void
setcinfo(clp, sbun, kanap, dispp)
jcClause *clp;
struct wnn_sho_bunsetsu *sbun;
wchar_t *kanap;
wchar_t *dispp;
{
	/* ltop $B%U%i%0$O$H$j$"$($:(J 0 $B$K$7$F$*$/(J */
	clp->conv = 1;
	clp->dicno = sbun->dic_no;
	clp->entry = sbun->entry;
	clp->hinshi = sbun->hinsi;
	clp->kangovect = sbun->kangovect;
	clp->ltop = 0;
	clp->kanap = kanap + sbun->start;
	clp->fzkp = kanap + sbun->jiriend + 1;
	clp->dispp = dispp;
	clp->imabit = sbun->ima;
}

/* changecinfo -- clauseInfo $B$N>pJs$r;XDj$5$l$?8uJd$N$b$N$GCV$-49$($k(J */
static void
changecinfo(clp, candp, kanap, dispp)
jcClause *clp;
jcSCand *candp;
wchar_t *kanap;
wchar_t *dispp;
{
	clp->kanap = kanap;
	clp->fzkp = clp->kanap + candp->fzkoffset;
	clp->dispp = dispp;
	clp->dicno = candp->dicno;
	clp->entry = candp->entry;
	clp->hinshi = candp->hinshi;
	clp->kangovect = candp->kangovect;
	clp->conv = 1;
	clp->imabit = (candp->status & IMA_BIT) != 0;
	clp->ltop = (candp->status & CONNECT_PREV) == 0;
}

/* getFVec -- $B>.J8@a$N!"A0$H$N@\B3$rD4$Y$F%Q%i%a!<%?$r7h$a$k(J */
static void
getFVec(clp, hinship, fzk, connp)
jcClause *clp;
int *hinship;
wchar_t *fzk;
int *connp;
{
	wchar_t	*p, *q, *r;

	if (clp->ltop) {
		/* $BBgJ8@a@hF,(J */
		*hinship = WNN_BUN_SENTOU;
		fzk[0] = 0;
	} else if ((clp - 1)->conv != 1) {
		/* $BA0$NJ8@a$OJQ49$5$l$F$$$J$$!"$"$k$$$O$+$JJQ49$5$l$F$$$k(J */
		*hinship = WNN_ALL_HINSI;
		fzk[0] = 0;
	} else {
		*hinship = (clp - 1)->hinshi;
		if (connp) *connp |= CONNECT_PREV;
		/* $BA0$NJ8@a$NIUB08lJ8;zNs$r%3%T!<$9$k(J */
		p = fzk;
		q = (clp - 1)->fzkp;
		r = clp->kanap;
		while (q < r) {
			*p++ = *q++;
		}
		/* NULL $B%?!<%_%M!<%H$5$;$J$1$l$P$J$i$J$$(J */
		*p = 0;
	}
}

/* getBVec -- $B>.J8@a$N!"8e$m$H$N@\B3$rD4$Y$F%Q%i%a!<%?$r7h$a$k(J */
static void
getBVec(clp, vecp, vec1p, connp)
jcClause *clp;
int *vecp;
int *vec1p;
int *connp;
{
#if WNN_VECT_KANTAN != WNN_VECT_KANZEN
	/* $BF1$8(J getBVec() $B$rC1J8@aJQ49$HA48uJd<h$j=P$7$K;H$&$3$H$,$G$-$J$$(J */
	!! ERROR !!
#else
	if ((++clp)->ltop || clp->conv != 1) {
		*vecp = WNN_VECT_KANZEN;
		*vec1p = WNN_VECT_NO;
	} else {
		*vecp = clp->kangovect;
		*vec1p = WNN_VECT_KANZEN;
		if (connp) *connp |= CONNECT_NEXT;
	}
#endif
}

static int
getKanjiLenDbun(dbun, ndbun)
struct wnn_dai_bunsetsu *dbun;
int ndbun;
{
	struct wnn_sho_bunsetsu	*sbun;
	int	nsbun;
	int	nc = 0;

	while (ndbun-- > 0) {
		sbun = dbun->sbn;
		nsbun = dbun->sbncnt;
		while (nsbun-- > 0) {
			nc += wstrlen(sbun->kanji) + wstrlen(sbun->fuzoku);
			sbun++;
		}
		dbun++;
	}
	return nc;
}

static int
getKanjiLenSbun(sbun, nsbun)
struct wnn_sho_bunsetsu *sbun;
int nsbun;
{
	int	nc = 0;

	while (nsbun-- > 0) {
		nc += wstrlen(sbun->kanji) + wstrlen(sbun->fuzoku);
		sbun++;
	}

	return nc;
}

/* moveKBuf -- $B$+$J%P%C%U%!$N;XDj$5$l$?J8@a$N@hF,$+$i$"$H$rF0$+$9(J */
static void
moveKBuf(buf, cl, move)
jcConvBuf *buf;
int cl;
int move;
{
	jcClause	*clp = buf->clauseInfo + cl;
	jcClause	*clpend;
	int		movelen;

	if (move == 0) return;

	if ((movelen = buf->kanaEnd - clp->kanap) > 0) {
		/* $B$+$J%P%C%U%!$NFbMF$rF0$+$9(J */
		(void)bcopy((char *)clp->kanap, (char *)(clp->kanap + move),
			    movelen * sizeof(wchar_t));
	}

	/* $B$+$J%P%C%U%!$NJQ99$K9g$o$;$F(J clauseInfo $B$r%"%C%W%G!<%H$9$k(J */
	clpend = buf->clauseInfo + buf->nClause;
	while (clp <= clpend) {
		clp->kanap += move;
		clp++->fzkp += move;
	}

	/* kanaEnd $B$N%"%C%W%G!<%H(J */
	buf->kanaEnd += move;
}

/* moveDBuf -- $B%G%#%9%W%l%$%P%C%U%!$N;XDj$5$l$?J8@a$N@hF,$+$i$"$H$rF0$+$9(J */
static void
moveDBuf(buf, cl, move)
jcConvBuf *buf;
int cl;
int move;
{
	jcClause	*clp = buf->clauseInfo + cl;
	jcClause	*clpend;
	int		movelen;

	if (move == 0) return;

	if ((movelen = buf->displayEnd - clp->dispp) > 0) {
		/* $B%G%#%9%W%l%$%P%C%U%!$NFbMF$rF0$+$9(J */
		(void)bcopy((char *)clp->dispp, (char *)(clp->dispp + move),
			    movelen * sizeof(wchar_t));
	}

	/* $B%G%#%9%W%l%$%P%C%U%!$NJQ99$K9g$o$;$F(J clauseInfo $B$r(J
	 * $B%"%C%W%G!<%H$9$k(J
	 */
	clpend = buf->clauseInfo + buf->nClause;
	while (clp <= clpend) {
		clp++->dispp += move;
	}

	/* displayEnd $B$N%"%C%W%G!<%H(J */
	buf->displayEnd += move;
}

/* moveCInfo -- ClauseInfo $B$N;XDj$5$l$?J8@a$N@hF,$+$i$"$H$rF0$+$9(J */
static void
moveCInfo(buf, cl, move)
jcConvBuf *buf;
int cl;
int move;
{
	jcClause	*clp = buf->clauseInfo + cl;
	int		len;

	/* move $B$K@5$N?t$r;XDj$9$l$PJ8@a$NA^F~!"Ii$J$iJ8@a$N:o=|$K$J$k(J */

	if (move == 0) return;

	if ((len = buf->nClause + 1 - cl) > 0) {
		(void)bcopy((char *)clp, (char *)(clp + move),
			    len * sizeof(jcClause));
	}
	buf->nClause += move;
	if (buf->candClause >= 0) {
		buf->candClause += move;
		buf->candClauseEnd += move;
	}
}

/* resizeBuffer -- $B$+$J(J/$B%G%#%9%W%l%$%P%C%U%!$NBg$-$5$rJQ$($k(J */
static int
resizeBuffer(buf, len)
jcConvBuf *buf;
int len;
{
	wchar_t	*kbufold, *dbufold;
	wchar_t	*kbufnew, *dbufnew;
	int	allocsize;
	jcClause	*clp, *clpend;

	kbufold = buf->kanaBuf;
	dbufold = buf->displayBuf;

	/* realloc $B$9$k(J */
	allocsize = (len + 1) * sizeof(wchar_t);
	kbufnew = (wchar_t *)realloc((char *)kbufold, allocsize);
	dbufnew = (wchar_t *)realloc((char *)dbufold, allocsize);

	if (kbufnew == NULL || dbufnew == NULL) {
		Free(kbufnew);
		Free(dbufnew);
		jcErrno = JE_NOCORE;
		return -1;
	}

	buf->bufferSize = len;

	if (kbufnew == kbufold && dbufnew == dbufold) {
		/* $B%]%$%s%?$OA0$HJQ$o$C$F$$$J$$(J */
		return 0;
	}

	/* $B3F<o%]%$%s%?$r$D$1JQ$($k(J */

	buf->kanaBuf = kbufnew;
	buf->kanaEnd = kbufnew + (buf->kanaEnd - kbufold);
	buf->displayBuf = dbufnew;
	buf->displayEnd = dbufnew + (buf->displayEnd - dbufold);

	buf->dot = kbufnew + (buf->dot - kbufold);

	clp = buf->clauseInfo;
	clpend = clp + buf->nClause;
	while (clp <= clpend) {
		clp->kanap = kbufnew + (clp->kanap - kbufold);
		clp->fzkp = kbufnew + (clp->fzkp - kbufold);
		clp->dispp = dbufnew + (clp->dispp - dbufold);
		clp++;
	}

	return 0;
}

/* resizeCInfo -- clauseInfo $B%P%C%U%!$NBg$-$5$rJQ$($k(J */
static int
resizeCInfo(buf, size)
jcConvBuf *buf;
int size;
{
	jcClause	*cinfonew;

	/* realloc $B$9$k(J */
	cinfonew = (jcClause *)realloc((char *)buf->clauseInfo,
				       (size + 1) * sizeof(jcClause));
	if (cinfonew == NULL) {
		jcErrno = JE_NOCORE;
		return -1;
	}

	buf->clauseSize = size;
	buf->clauseInfo = cinfonew;
	return 0;
}

/* resizeCandBuf -- $B<!8uJd%P%C%U%!$NBg$-$5$rJQ$($k(J */
static int
resizeCandBuf(buf, nbytes)
jcConvBuf *buf;
int nbytes;
{
	char	*p;

	if ((p = realloc((char *)buf->candBuf, nbytes)) == NULL) {
		jcErrno = JE_NOCORE;
		return -1;
	}

	buf->candSize = nbytes;
	buf->candBuf = p;
	return 0;
}

/* setCurClause -- $B%+%l%s%HJ8@a$r@_Dj$9$k(J */
static void
setCurClause(buf, cl)
jcConvBuf *buf;
int cl;		/* $B%+%l%s%H>.J8@aHV9f(J */
{
	jcClause	*clp = buf->clauseInfo;
	int		i;

	/* $B%+%l%s%H>.J8@a(J */
	buf->curClause = cl;

	/* $B%+%l%s%HBgJ8@a3+;OJ8@a(J */
	for (i = cl; i > 0 && !clp[i].ltop; i--)
		;
	buf->curLCStart = i;

	/* $B%+%l%s%HBgJ8@a=*N;J8@a(J ($B$N<!(J) */
	for (i = cl + 1; i <= buf->nClause && !clp[i].ltop; i++)
		;
	buf->curLCEnd = i;
}

/* renConvert -- $B%+%l%s%HJ8@a$+$i8e$m$rO"J8@aJQ49$9$k(J */
static int
renConvert(buf, small)
jcConvBuf *buf;
int small;
{
	/* $BO"J8@aJQ49$9$k(J */
	if (doKanrenConvert(buf,
			    small ? buf->curClause : buf->curLCStart) < 0) {
		return -1;
	}

	/*
	 * $B%+%l%s%HJ8@a$N@_Dj(J
	 * small $B$,(J 0 $B$J$i!"(J
	 *	$B%+%l%s%HBgJ8@a$N@hF,$O(J buf->curLCStart $B$GJQ$o$i$:(J
	 *	$B%+%l%s%HBgJ8@a=*$j$O(J ltop $B%U%i%0$r%5!<%A$7$FC5$9(J
	 *	$B%+%l%s%H>.J8@a$O%+%l%s%HBgJ8@a@hF,$K0\F0(J
	 * small $B$,(J 0 $B$G$J$$$J$i!"(J
	 *	$B%+%l%s%H>.J8@a$O(J buf->curClause $B$GJQ$o$i$:(J
	 *	$B%+%l%s%HBgJ8@a$N@hF,$*$h$S=*$j$O!"%+%l%s%H>.J8@a$N(J
	 *	$BA08e$r(J ltop $B%U%i%0$r%5!<%A$7$FC5$9(J
	 */
	setCurClause(buf, small ? buf->curClause : buf->curLCStart);

	/* $B%I%C%H$N@_Dj(J */
	DotSet(buf);

	return 0;
}

/* tanConvert -- $B%+%l%s%HJ8@a$rC1J8@aJQ49$9$k(J */
static int
tanConvert(buf, small)
jcConvBuf *buf;
int small;
{
	/*
	 * $BC1J8@aJQ49$N>l9g!"4pK\E*$K(J 2 $BCJ3,$N=hM}$r9T$J$&$3$H$K$J$k(J
	 * $B$^$:!"%+%l%s%HJ8@a$rC1J8@aJQ49(J
	 * $B<!$K!"$=$N$"$H$rO"J8@aJQ49(J
	 */

	if (small) {
		/* $B$^$:C1J8@aJQ49$9$k(J */
		if (doKantanSConvert(buf, buf->curClause) < 0)
			return -1;

		/* $B%+%l%s%HJ8@a$N@_Dj(J
		 *	$B%+%l%s%H>.J8@a$O(J buf->curClause $B$GJQ$o$i$:(J
		 *	$B%+%l%s%HBgJ8@a$N@hF,$H:G8e$O%+%l%s%H>.J8@a$N(J
		 *	$BA08e$K(J ltop $B%U%i%0$r%5!<%A$7$FC5$9(J
		 */
		setCurClause(buf, buf->curClause);
		/* $B%I%C%H$N@_Dj(J */
		DotSet(buf);

		/* $BO"J8@aJQ49(J */
		if (buf->curClause + 1 < buf->nClause &&
		    buf->clauseInfo[buf->curClause + 1].conv == 0) {
			/* $B>.J8@a$NC1J8@aJQ49%b!<%I$G!"<!$NJ8@a$,(J
			 * $BL5JQ49$@$C$?>l9g!"(Jltop $B%U%i%0$r(J 0 $B$K$7$F(J
			 * $BA0$H@\B3$G$-$k$h$&$K$9$k(J
			 */
			buf->clauseInfo[buf->curClause + 1].ltop = 0;
		}
		if (doKanrenConvert(buf, buf->curClause + 1) < 0)
			return -1;

		/* $B$b$&0lEY%+%l%s%HJ8@a$N@_Dj(J
		 * $BO"J8@aJQ49$N7k2L$K$h$C$F$O%+%l%s%HBgJ8@a$N:G8e$,(J
		 * $B0\F0$9$k$3$H$,$"$k(J
		 */
		setCurClause(buf, buf->curClause);

		/* $B%I%C%H$O0\F0$7$J$$$N$G:F@_Dj$7$J$/$F$h$$(J */
	} else {
		/* $B$^$:C1J8@aJQ49$9$k(J */
		if (doKantanDConvert(buf, buf->curLCStart, buf->curLCEnd) < 0)
			return -1;

		/* $B%+%l%s%HJ8@a$N@_Dj(J
		 *	$B%+%l%s%HBgJ8@a$N@hF,$O(J buf->curLCStart $B$GJQ$o$i$:(J
		 *	$B%+%l%s%HBgJ8@a=*$j$O(J ltop $B%U%i%0$r%5!<%A$7$FC5$9(J
		 *	$B%+%l%s%H>.J8@a$O%+%l%s%HBgJ8@a@hF,$K0\F0(J
		 */
		setCurClause(buf, buf->curLCStart);
		DotSet(buf);

		/* $BO"J8@aJQ49(J */
		if (doKanrenConvert(buf, buf->curLCEnd) < 0)
			return -1;
		/* $B$3$A$i$O(J small $B$N;~$H0c$C$FO"J8@aJQ49$N7k2L%+%l%s%HJ8@a$,(J
		 * $B0\F0$9$k$3$H$O$J$$(J
		 */
	}

	return 0;
}

/* doKanrenConvert -- $B;XDj$5$l$?J8@a$+$i8e$m$rO"J8@aJQ49$9$k(J */
static int
doKanrenConvert(buf, cl)
jcConvBuf *buf;
int cl;
{
	jcClause	*clp;
	int	hinshi;
	wchar_t	*kanap, *dispp;
	wchar_t	*wp;
	wchar_t	fzk[MAXFZK];
	struct wnn_dai_bunsetsu	*dbun;
	struct wnn_sho_bunsetsu	*sbun;
	int	ndbun, nsbun;
	int	len;
	int	i, j;

	/*
	 * $B;XDj$5$l$?J8@a$+$i8e$m$rO"J8@aJQ49$9$k(J
	 * $B%+%l%s%HJ8@a$N:F@_Dj$J$I$O$7$J$$(J
	 */

	if (cl >= buf->nClause) {
		/* $B;XDj$5$l$?J8@a$O$J$$(J
		 * $B%(%i!<$K$O$7$J$$(J
		 * $B6u$NJ8@a$rJQ49$7$h$&$H$7$?;~$K!"$=$l$r;vA0$K%A%'%C%/$7$F(J
		 * $B%(%i!<$K$9$k$N$O>e0L$N4X?t$N@UG$$G$"$k(J
		 */
		return 0;
	}

	clp = buf->clauseInfo + cl;

	/* $BA0$NJ8@a$H$N@\B3$rD4$Y$k(J */
	getFVec(clp, &hinshi, fzk, NULL);

	*(buf->kanaEnd) = 0;	/* NULL $B%?!<%_%M!<%H$5$;$F$*$/(J */
	/* $BO"J8@aJQ49$9$k(J */
	ndbun = js_kanren(buf->env, clp->kanap, hinshi, fzk,
			  WNN_VECT_KANREN, WNN_VECT_NO, WNN_VECT_BUNSETSU,
			  &jsbuf);

	if (ndbun < 0) {
		jcErrno = JE_WNNERROR;
		return -1;
	}

	dbun = (struct wnn_dai_bunsetsu *)jsbuf.buf;

	/* $B$H$j$"$($:>.J8@a$N?t$r?t$($k(J */
	nsbun = 0;
	for (i = 0; i < ndbun; i++) {
		nsbun += dbun[i].sbncnt;
	}

	/* clauseInfo $B$N%5%$%:$N%A%'%C%/(J */
	if (cl + nsbun > buf->clauseSize) {
		if (resizeCInfo(buf, cl + nsbun) < 0)
			return -1;
	}

	/* $B<!$KJQ49J8;zNs$ND9$5$N%A%'%C%/(J */
	clp = buf->clauseInfo + cl;
	len = (clp->dispp - buf->displayBuf) + getKanjiLenDbun(dbun, ndbun);
	if (len > buf->bufferSize) {
		if (resizeBuffer(buf, len) < 0)
			return -1;
	}

	buf->nClause = cl + nsbun;

	/* $B$G$O(J clauseInfo $B$KJQ497k2L$rF~$l$F$$$/(J */
	clp = buf->clauseInfo + cl;
	kanap = clp->kanap;
	dispp = clp->dispp;
	for (i = 0; i < ndbun; i++) {
		int	connstate = dbun->sbn->status;

		sbun = dbun->sbn;
		nsbun = dbun->sbncnt;

		for (j = 0; j < nsbun; j++) {
			setcinfo(clp, sbun, kanap, dispp);

			/* ltop $B$N@_Dj(J */
			clp->ltop = (j == 0 && connstate != WNN_CONNECT);

			/* $B%G%#%9%W%l%$%P%C%U%!$X$NJQ49J8;zNs$N%3%T!<(J */
			/* $B<+N)8lItJ,(J */
			wp = sbun->kanji;
			while (*dispp++ = *wp++)
				;
			dispp--;
			/* $BIUB08lItJ,(J */
			wp = sbun->fuzoku;
			while (*dispp++ = *wp++)
				;
			dispp--;

			sbun++;
			clp++;
		}
		dbun++;
	}

	/* $B:G8e$N(J clauseInfo $B$N@_Dj(J */
	clp->kanap = buf->kanaEnd;
	clp->dispp = buf->displayEnd = dispp;
	clp->conv = 0;
	clp->ltop = 1;

	return 0;
}

/* doKantanDConvert -- $B;XDj$5$l$?HO0O$NJ8@a$rBgJ8@a$H$7$FC1J8@aJQ49$9$k(J */
static int
doKantanDConvert(buf, cls, cle)
jcConvBuf *buf;
int cls;
int cle;
{
	jcClause	*clps, *clpe;
	int	len, diff, newlen;
	int	cldiff, nclausenew;
	int	hinshi;
	wchar_t	*kanap, *dispp;
	wchar_t	*wp;
	wchar_t	fzk[MAXFZK];
	wchar_t	savechar;
	wchar_t	*savep;
	struct wnn_dai_bunsetsu	*dbun;
	struct wnn_sho_bunsetsu	*sbun;
	int	ndbun, nsbun;
	int	i;

	/*
	 * $B;XDj$5$l$?HO0O$NJ8@a$rBgJ8@a$H$7$FC1J8@aJQ49$9$k(J
	 * $B%+%l%s%HJ8@a$N:F@_Dj$J$I$O$7$J$$(J
	 */

	clps = buf->clauseInfo + cls;
	clpe = buf->clauseInfo + cle;
	/* $BA0$NJ8@a$H$N@\B3$rD4$Y$k(J */
	getFVec(clps, &hinshi, fzk, NULL);

	/* $BFI$_$r(J NULL $B%?!<%_%M!<%H$9$k(J
	 * $BC1$K(J 0 $B$rF~$l$k$H<!$NJ8@a$,2u$l$k$N$G!"$=$NA0$K%;!<%V$7$F$*$/(J
	 */
	savep = clpe->kanap;
	savechar = *savep;
	*savep = 0;

	/* $BC1J8@aJQ49$9$k(J */
	ndbun = js_kantan_dai(buf->env, clps->kanap, hinshi, fzk,
			      WNN_VECT_KANTAN, WNN_VECT_NO, &jsbuf);
	/* $B$9$+$5$:%;!<%V$7$F$"$C$?J8;z$r$b$H$KLa$9(J */
	*savep = savechar;
	if (ndbun < 0) {
		jcErrno = JE_WNNERROR;
		return -1;
	}

	dbun = (struct wnn_dai_bunsetsu *)jsbuf.buf;
	sbun = dbun->sbn;
	nsbun = dbun->sbncnt;

	cldiff = nsbun - (cle - cls);
	nclausenew = buf->nClause + cldiff;
	/* clauseInfo $B$N%5%$%:$N%A%'%C%/(J */
	if (nclausenew > buf->clauseSize) {
		if (resizeCInfo(buf, nclausenew) < 0)
			return -1;
	}

	/* $BJQ49J8;zNs$ND9$5$N%A%'%C%/(J */
	len = getKanjiLenDbun(dbun, 1);
	diff = len - (clpe->dispp - clps->dispp);
	newlen = (buf->displayEnd - buf->displayBuf) + diff;
	if (newlen > buf->bufferSize) {
		if (resizeBuffer(buf, newlen) < 0)
			return -1;
	}

	/* $BJ8@a$rA^F~$9$k$N$G!"%G%#%9%W%l%$%P%C%U%!$NFbMF$r0\F0$5$;$k(J */
	/* $B$I$&$;$"$H$+$iO"J8@aJQ49$9$k$+$i$$$$$G$O$J$$$+$H$$$&9M$(J}$b$"$k$,!"(J
	 * $B$I$3$G%(%i!<$,5/$3$C$F$b0l1~$N(J consistency $B$,J]$?$l$k$h$&$K(J
	 * $B$9$k$H$$$&$N$,L\I8$G$"$k(J
	 */
	moveDBuf(buf, cle, diff);

	/* clauseInfo $B$rF0$+$9(J ($BF1;~$K(J nClause $B$b%"%C%W%G!<%H$5$l$k(J) */
	moveCInfo(buf, cle, cldiff);

	/* $B$G$O(J clauseInfo $B$KJQ497k2L$rF~$l$k(J */
	clps = buf->clauseInfo + cls;
	kanap = clps->kanap;
	dispp = clps->dispp;
	for (i = 0; i < nsbun; i++) {
		setcinfo(clps, sbun, kanap, dispp);

		clps->ltop = i == 0 && sbun->status != WNN_CONNECT;

		/* $B%G%#%9%W%l%$%P%C%U%!$X$NJQ49J8;zNs$N%3%T!<(J */
		/* $B<+N)8lItJ,(J */
		wp = sbun->kanji;
		while (*dispp++ = *wp++)
			;
		dispp--;
		/* $BIUB08lItJ,(J */
		wp = sbun->fuzoku;
		while (*dispp++ = *wp++)
			;
		dispp--;

		clps++;
		sbun++;
	}

	/* $B<!$N(J clauseInfo $B$N@_Dj(J */
	clps->ltop = 1;

	return 0;
}

/* doKantanSConvert -- $B;XDj$5$l$?J8@a$r>.J8@a$H$7$FC1J8@aJQ49$9$k(J */
static int
doKantanSConvert(buf, cl)
jcConvBuf *buf;
int cl;
{
	jcClause	*clp;
	int	len, newlen, diff;
	int	hinshi;
	wchar_t	*dispp;
	wchar_t	fzk[MAXFZK];
	wchar_t	*wp;
	wchar_t	savechar;
	wchar_t	*savep;
	int	vec, vec1;
	struct wnn_sho_bunsetsu	*sbun;
	int	nsbun;

	/*
	 * $B;XDj$5$l$?J8@a$r>.J8@a$H$7$FC1J8@aJQ49$9$k(J
	 * $B%+%l%s%HJ8@a$N:F@_Dj$J$I$O$7$J$$(J
	 */

	clp = buf->clauseInfo + cl;

	/* $BA0$NJ8@a$H$N@\B3$rD4$Y$k(J */
	getFVec(clp, &hinshi, fzk, NULL);
	/* $B8e$m$N>.J8@a$H$N@\B3$rD4$Y$k(J */
	getBVec(clp, &vec, &vec1, NULL);

	/* $BFI$_$r(J NULL $B%?!<%_%M!<%H$9$k(J
	 * $BC1$K(J 0 $B$rF~$l$k$H<!$NJ8@a$,2u$l$k$N$G!"$=$NA0$K%;!<%V$7$F$*$/(J
	 */
	savep = (clp + 1)->kanap;
	savechar = *savep;
	*savep = 0;

	/* $BC1J8@aJQ49$9$k(J */
	nsbun = js_kantan_sho(buf->env, clp->kanap, hinshi, fzk,
			      vec, vec1, &jsbuf);
	/* $B$9$+$5$:%;!<%V$7$F$"$C$?J8;z$r$b$H$KLa$9(J */
	*savep = savechar;
	if (nsbun < 0) {
		jcErrno = JE_WNNERROR;
		return -1;
	}

	sbun = (struct wnn_sho_bunsetsu *)jsbuf.buf;

	/* $BJQ49J8;zNs$ND9$5$N%A%'%C%/(J */
	clp = buf->clauseInfo + cl;
	len = getKanjiLenSbun(sbun, 1);
	diff = len - ((clp + 1)->dispp - clp->dispp);
	newlen = (buf->displayEnd - buf->displayBuf) + diff;
	if (newlen > buf->bufferSize) {
		if (resizeBuffer(buf, newlen) < 0)
			return -1;
	}

	/* $BJ8@a$rA^F~$9$k$N$G!"%G%#%9%W%l%$%P%C%U%!$NFbMF$r0\F0$5$;$k(J */
	/* $B$I$&$;$"$H$+$iO"J8@aJQ49$9$k$+$i$$$$$G$O$J$$$+$H$$$&9M$(J}$b$"$k$,!"(J
	 * $B$I$3$G%(%i!<$,5/$3$C$F$b0l1~$N(J consistency $B$,J]$?$l$k$h$&$K(J
	 * $B$9$k$H$$$&$N$,L\I8$G$"$k(J
	 */
	moveDBuf(buf, cl + 1, diff);

	/* $B$G$O(J clauseInfo $B$KJQ497k2L$rF~$l$k(J */
	clp = buf->clauseInfo + cl;
	dispp = clp->dispp;

	setcinfo(clp, sbun, clp->kanap, dispp);
	clp->ltop = sbun->status != WNN_CONNECT;

	/* $B%G%#%9%W%l%$%P%C%U%!$X$NJQ49J8;zNs$N%3%T!<(J */
	/* $B<+N)8lItJ,(J */
	wp = sbun->kanji;
	while (*dispp++ = *wp++)
		;
	dispp--;
	/* $BIUB08lItJ,(J */
	wp = sbun->fuzoku;
	while (*dispp++ = *wp++)
		;

	/* $B<!$N(J clauseInfo $B$N@_Dj(J */
	(++clp)->ltop = sbun->status_bkwd == WNN_NOT_CONNECT_BK ||
	    vec == WNN_VECT_KANTAN;

	return 0;
}

/* unconvert -- $B;XDj$5$l$?HO0O$NJ8@a$r0l$D$NL5JQ49$NJ8@a$K$9$k(J */
static int
unconvert(buf, start, end)
jcConvBuf *buf;
int start;
int end;
{
	jcClause	*clps, *clpe;
	int	diff, len;

	if (end <= start)
		return 0;

	if (start >= buf->nClause)
		return 0;

	clps = buf->clauseInfo + start;
	clpe = buf->clauseInfo + end;

	/*
	 * $B%G%#%9%W%l%$%P%C%U%!$NFbMF$r%+%J%P%C%U%!$NFbMF$GCV$-49$($k(J
	 * $B!D$H$$$C$F$b<B:]$NF0:n$O$=$l$[$I4JC1$G$O$J$$(J
	 *
	 * $B!&$^$:!"CV$-49$($?7k2L!"%G%#%9%W%l%$%P%C%U%!$,$"$U$l$J$$$+D4$Y!"(J
	 *   $B$"$U$l$k$h$&$J$i%P%C%U%!$N%5%$%:$rBg$-$/$9$k(J
	 * $B!&%G%#%9%W%l%$%P%C%U%!$K!"$+$J%P%C%U%!$+$i%G!<%?$r0\$9(J
	 * $B!&(JclauseInfo $B$r=q$-49$($F!"(Jstart $B$+$i(J end-1 $B$^$G$NJ8@a$r(J
	 *   $B0l$D$NL5JQ49$NJ8@a$K$^$H$a$k(J
	 * $B!&$b$A$m$s(J nClause $B$bJQ$($k(J
	 * $B!&(Jstart+1 $B$+$i:G8e$^$G$NJ8@a$N(J clauseInfo $B$N(J dispp $B$r(J
	 *   $B%G%#%9%W%l%$%P%C%U%!$N$:$l$K1~$8$FD4@0$9$k(J
	 *
	 * $B$=$NB>$K<!$N$3$H$b9T$J$&I,MW$,$"$k$,!"$3$N4X?t$G$O$d$i$J$$(J
	 * $B>e0L$N4X?t$G@_Dj$9$k$3$H(J
	 * $B!&BgJ8@a%U%i%0(J (ltop) $B$N@_Dj(J
	 * $B!&%+%l%s%HJ8@a!"$*$h$S<!8uJdJ8@a$N0\F0(J
	 *   $B<!8uJdJ8@a$,L5JQ49$NJ8@a$K$J$C$F$7$^$C$?;~$N=hM}(J
	 * $B!&%I%C%H$N0\F0(J
	 */

	/* $BFI$_$ND9$5$H4A;z$ND9$5$N:9$rD4$Y$k(J */
	diff = (clpe->kanap - clps->kanap) - (clpe->dispp - clps->dispp);
	/* $BCV$-49$($?>l9g$N%G%#%9%W%l%$%P%C%U%!$ND9$5(J */
	len = (buf->displayEnd - buf->displayBuf) + diff;
	/* $B%P%C%U%!$N%5%$%:$,B-$j$J$1$l$P%5%$%:$rBg$-$/$9$k(J */
	if (len > buf->bufferSize) {
		if (resizeBuffer(buf, len) < 0) {
			/* $B%5%$%:$,JQ$($i$l$J$+$C$?(J */
			return -1;
		}
	}

	/* $BCV$-49$((J */
	/* $B$^$:8e$m$NItJ,$rF0$+$7$F$+$i(J */
	moveDBuf(buf, end, diff);
	/* $BFI$_$rF~$l$k(J */
	(void)bcopy((char *)clps->kanap, (char *)clps->dispp,
		    (clpe->kanap - clps->kanap) * sizeof(wchar_t));

	/*
	 * start $B$+$i(J end $B$^$G$NJ8@a$r0l$D$K$^$H$a$k(J
	 */

	/* $BL5JQ49>uBV$K$J$C$?J8@a$N(J clauseInfo $B$N@_Dj(J */
	clps->conv = 0;

	/* end $B$+$i$"$H$N(J clauseInfo $B$r(J'$B$D$a$k(J' */
	moveCInfo(buf, end, start + 1 - end);

	return 0;
}

static int
expandOrShrink(buf, small, expand, convf)
jcConvBuf *buf;
int small;
int expand;
int convf;
{
	jcClause	*clp, *clpe;
	int	start, end;

	start = small ? buf->curClause : buf->curLCStart;
	end = small ? start + 1 : buf->curLCEnd;

	clp = buf->clauseInfo + start;
	clpe = buf->clauseInfo + end;

	/*
	 * $B?-$S=L$_$G$-$k$+$N%A%'%C%/(J
	 */
	if (expand) {
		/*
		 * $B%+%l%s%HJ8@a$,:G8e$NJ8@a$N;~$K$O(J
		 * $B$b$&9-$2$i$l$J$$(J
		 */
		if (end >= buf->nClause) {
			jcErrno = JE_CANTEXPAND;
			return -1;
		}
	} else {
		if (buf->curClause == buf->nClause ||
		    clpe->kanap - clp->kanap <= 1) {
			/* $B%+%l%s%HJ8@a$,6u$+!"$"$k$$$OD9$5$,#10J2<(J */
			jcErrno = JE_CANTSHRINK;
			return -1;
		}
	}

	/* $B%+%l%s%HJ8@a$*$h$S$=$N<!$NC18l$r%j%;%C%H%j%9%H$KF~$l$k(J */
	addResetClause(buf, start, (end >= buf->nClause) ? end : end + 1);

	/* $B8uJdJ8@a$,%+%l%s%HJ8@a$N8e$m$K$"$l$PL58z$K$9$k(J */
	checkAndResetCandidates(buf, start, buf->nClause);

	/* $B%+%l%s%HJ8@a$,JQ49$5$l$F$$$l$P!"$H$j$"$($:L5JQ49$K$9$k(J */
	if (clp->conv) {
		if (unconvert(buf, start, end) < 0)
			return -1;
	}

	/* $B%+%l%s%HJ8@a$N:F@_Dj(J */
	if (small) {
		buf->curClause = start;
		buf->curLCEnd = start + 1;
	} else {
		buf->curClause = buf->curLCStart = start;
		buf->curLCEnd = start + 1;
	}
	DotSet(buf);
	buf->clauseInfo[start + 1].ltop = 1;

	/* $B$5$i$K%+%l%s%HJ8@a$N8e$m$rL5JQ49$K$9$k(J
	 * $B$3$N;~$K$O$9$G$K(J start $B$+$i(J end $B$^$G$NJ8@a$,0l$D$N(J
	 * $BL5JQ49$NJ8@a$K$^$H$a$i$l$F$$$k$3$H$KCm0U(J
	 */
	if (unconvert(buf, start + 1, buf->nClause) < 0)
		return -1;

	if (small) {
		if (start + 1 < buf->nClause)
			buf->clauseInfo[start + 1].ltop = 0;
		buf->curLCEnd = buf->nClause;
	}
	clp = buf->clauseInfo + start;
	/*
	 * $B%+%l%s%HJ8@a$ND9$5$r#1J8;z?-$S=L$_$5$;$k(J
	 */
	if (expand) {
		/* $B%+%l%s%HJ8@a$ND9$5$r?-$P$9(J */
		(++clp)->kanap++;
		clp->dispp++;
		/* $B9-$2$?7k2L!"8e$m$NJ8@a$,$J$/$J$k$3$H$,$"$k(J */
		if (clp->kanap == buf->kanaEnd) {
			buf->nClause--;
			clp->ltop = 1;
		}
	} else {
		/* $B%+%l%s%HJ8@a$ND9$5$r=L$a$k(J */
		(++clp)->kanap--;
		clp->dispp--;
		/* $B%+%l%s%HJ8@a$,:G8e$NJ8@a$@$C$?>l9g$K$O!"#1J8@aA}$($k(J */
		if (start == buf->nClause - 1) {
			if (buf->nClause >= buf->clauseSize) {
				if (resizeCInfo(buf, buf->nClause + 1) < 0) {
					/* $BJQ99$7$?=j$r85$KLa$9(J */
					clp = buf->clauseInfo + start + 1;
					clp->kanap++;
					clp->dispp--;
					return -1;
				}
			}

			/* $B>e$G(J clauseInfo $B$,(J realloc $B$5$l$?$+$b$7$l$J$$$N$G(J
			 * clp $B$r:F$S@_Dj(J
			 */
			clp = buf->clauseInfo + buf->nClause;

			/* $B?7$7$/$G$-$?J8@a$N(J clauseInfo $B$N@_Dj(J
			 * kanap, dispp $B$O$9$G$K>e$G%;%C%H$5$l$F$$$k(J
			 */
			clp->ltop = small ? 0 : 1;
			clp->conv = 0;

			/* $B:G8e$NJ8@a$N<!$N(J ($B=*C<$N(J) clauseInfo $B$N@_Dj(J */
			(++clp)->kanap = buf->kanaEnd;
			clp->dispp = buf->displayEnd;
			clp->conv = 0;

			buf->nClause++;
		}
	}

	if (convf) {
		return tanConvert(buf, small);
	}
	return 0;
}

/* getSCandidates -- $B%+%l%s%H>.J8@a$NA48uJd$r<h$j=P$9(J */
static int
getSCandidates(buf)
jcConvBuf *buf;
{
	wchar_t	savechar;
	wchar_t	*savep;
	int	hinshi;
	wchar_t	fzk[MAXFZK];
	int	vec, vec1;
	int	ncand;
	jcClause	*clp;
	int	conn = 0;

	clp = buf->clauseInfo + buf->curClause;

	/* $BA0$H$N@\B3$rD4$Y$k(J */
	getFVec(clp, &hinshi, fzk, &conn);
	/* $B8e$m$H$N@\B3$rD4$Y$k(J */
	getBVec(clp, &vec, &vec1, &conn);

	/* $BFI$_$r(J NULL $B%?!<%_%M!<%H$7$F$*$/(J */
	savep = (clp + 1)->kanap;
	savechar = *savep;
	*savep = 0;

	/* $BA48uJd$r<h$j=P$9(J */
	ncand = js_kanzen_sho(buf->env, clp->kanap, hinshi, fzk,
			  vec, vec1, &jsbuf);
	/* $B$9$+$5$:FI$_$r$b$H$KLa$9(J */
	*savep = savechar;
	if (ncand < 0) {
		jcErrno = JE_WNNERROR;
		return -1;
	}

	if (setSCandData(buf, ncand, (struct wnn_sho_bunsetsu *)jsbuf.buf,
			 conn) < 0)
		return -1;

	/* $B8uJdJ8@a$N>pJs$rF~$l$k(J */
	buf->curCand = 0;
	buf->candKind = CAND_SMALL;
	buf->candClause = buf->curClause;
	buf->candClauseEnd = buf->curClause + 1;	/* $BG0$N$?$a(J */

	return 0;
}

/* getLCandidates -- $B%+%l%s%HBgJ8@a$NA48uJd$r<h$j=P$9(J */
static int
getLCandidates(buf)
jcConvBuf *buf;
{
	wchar_t	savechar;
	wchar_t	*savep;
	int	ncand;
	jcClause	*clps, *clpe;

	clps = buf->clauseInfo + buf->curLCStart;
	clpe = buf->clauseInfo + buf->curLCEnd;

	/* $BFI$_$r(J NULL $B%?!<%_%M!<%H$7$F$*$/(J */
	savep = clpe->kanap;
	savechar = *savep;
	*savep = 0;

	/* $BA48uJd$r<h$j=P$9(J */
	ncand = js_kanzen_dai(buf->env, clps->kanap, WNN_BUN_SENTOU, NULL,
			      WNN_VECT_KANZEN, WNN_VECT_NO, &jsbuf);
	/* $B$9$+$5$:FI$_$r$b$H$KLa$9(J */
	*savep = savechar;
	if (ncand < 0) {
		jcErrno = JE_WNNERROR;
		return -1;
	}

	if (setLCandData(buf, ncand, (struct wnn_dai_bunsetsu *)jsbuf.buf) < 0)
		return -1;

	/* $B8uJdJ8@a$N>pJs$rF~$l$F$*$/(J */
	buf->curCand = 0;
	buf->candKind = CAND_LARGE;
	buf->candClause = buf->curLCStart;
	buf->candClauseEnd = buf->curLCEnd;

	return 0;
}

/* findSCand -- $BF1$88uJd$,$J$$$+$I$&$+D4$Y$k(J ($B>.J8@a8uJd(J) */
static int
findSCand(scps, scpe, sbun)
jcSCand *scps;
jcSCand *scpe;
struct wnn_sho_bunsetsu *sbun;
{
	int	kangovect = sbun->kangovect;
	int	hinshi = sbun->hinsi;
	wchar_t	c = sbun->kanji[0];
	wchar_t	*p, *q;

	while (scps < scpe) {
		/* $BIJ;l$H!"A0$X$N@\B3%Y%/%H%k$H!"@hF,$N#1J8;z$,F1$8$+$I$&$+$G(J
		 * $B$U$k$$$K$+$1$k(J
		 * jllib $B$N%=!<%9$r8+$k$H(J kangovect $B$NHf3S$O%3%a%s%H%"%&%H(J
		 * $B$5$l$F$$$k$,!"$J$<$@$m$&(J?
		 */
		if (scps->hinshi == hinshi &&
		    scps->kangovect == kangovect &&
		    scps->kanji[0] == c) {
			/* $B;zLL$,F1$8$+D4$Y$k(J */
			p = scps->kanji;
			q = sbun->kanji;
			while (*q) {
				if (*p++ != *q++) goto next;
			}
			q = sbun->fuzoku;
			while (*q) {
				if (*p++ != *q++) goto next;
			}
			if (*p) goto next;

			/* $BF1$8(J */
			return 1;
		}
	next:
		scps++;
	}
	return 0;
}

/* findSCand -- $BF1$88uJd$,$J$$$+$I$&$+D4$Y$k(J ($BBgJ8@a8uJd(J) */
static int
findLCand(lcps, lcpe, dbun)
jcLCand *lcps;
jcLCand *lcpe;
struct wnn_dai_bunsetsu *dbun;
{
	struct wnn_sho_bunsetsu	*sbun;
	jcSCand	*scps, *scpe;
	int	nsbun = dbun->sbncnt;
	wchar_t	*p, *q;

	while (lcps < lcpe) {
		/* $B$H$j$"$($:>.J8@a?t$G$U$k$$$K$+$1$k(J */
		if (lcps->nscand != nsbun) goto next;

		scps = lcps->scand;
		scpe = scps + nsbun;
		sbun = dbun->sbn;
		while (scps < scpe) {
			/* $BIJ;l$H!"A0$X$N@\B3%Y%/%H%k$H!"@hF,$N#1J8;z$,(J
			 * $BF1$8$+$I$&$+$G$U$k$$$K$+$1$k(J
			 * jllib $B$N%=!<%9$r8+$k$H(J kangovect $B$NHf3S$O(J
			 * $B%3%a%s%H%"%&%H$5$l$F$$$k$,!"$J$<$@$m$&(J?
			 */
			if (scps->hinshi == sbun->hinsi &&
			    scps->kangovect == sbun->kangovect &&
			    scps->kanji[0] == sbun->kanji[0]) {
				/* $B;zLL$,F1$8$+D4$Y$k(J */
				p = scps->kanji;
				q = sbun->kanji;
				while (*q) {
					if (*p++ != *q++) goto next;
				}
				q = sbun->fuzoku;
				while (*q) {
					if (*p++ != *q++) goto next;
				}
				if (*p) goto next;
				
				scps++;
				sbun++;
			} else {
				goto next;	/* $B0c$&(J */
			}
		}
		/* $BF1$8$@$C$?(J */
		return 1;

	next:
		lcps++;
	}
	return 0;
}

/* setSCandData -- $B<h$j=P$7$?%+%l%s%H>.J8@a$NA48uJd$r3JG<$9$k(J */
static int
setSCandData(buf, ncand, sbun, conn)
jcConvBuf *buf;
int ncand;
struct wnn_sho_bunsetsu *sbun;
int conn;
{
	jcClause	*clp;
	jcSCand *scandp, *scp;
	wchar_t	*sp, *spend;
	wchar_t *wp;
	int	bytesneeded;
	int	i;

	clp = buf->clauseInfo + buf->curClause;

	/* buf->candBuf $B$KI,MW$JBg$-$5$rD4$Y$k(J */
	bytesneeded = getSCandDataLen(clp, ncand, sbun);
	if (bytesneeded > buf->candSize &&
	    resizeCandBuf(buf, bytesneeded) < 0) {
		return -1;
	}

	scandp = scp = (jcSCand *)buf->candBuf;
	wp = (wchar_t *)(scandp + (ncand + 1));

	/* $B$^$:!"<!8uJd$N:G=i$K!"8=:_$N>.J8@a$NFbMF$rF~$l$k(J */
	scp->kanji = wp;
	scp->kanalen = (clp + 1)->kanap - clp->kanap;
	scp->fzkoffset = clp->fzkp - clp->kanap;
	scp->dicno = clp->dicno;
	scp->entry = clp->entry;
	scp->hinshi = clp->hinshi;
	scp->kangovect = clp->kangovect;
	scp->status = clp->imabit ? IMA_BIT : 0;
	if (!clp->ltop) scp->status |= CONNECT_PREV;
	if (!(clp + 1)->ltop) scp->status |= CONNECT_NEXT;
	scp++;
	sp = clp->dispp;
	spend = (clp + 1)->dispp;
	while (sp < spend)
		*wp++ = *sp++;
	*wp++ = 0;

	/* $B<!$KA48uJd$N>pJs$rF~$l$k(J */
	for (i = 0; i < ncand; i++, sbun++) {
		if (findSCand(scandp, scp, sbun)) {
			continue;
		}
		scp->kanji = wp;
		scp->kanalen = sbun->end - sbun->start + 1;
		scp->fzkoffset = sbun->jiriend - sbun->start + 1;
		scp->dicno = sbun->dic_no;
		scp->entry = sbun->entry;
		scp->hinshi = sbun->hinsi;
		scp->kangovect = sbun->kangovect;
		scp->status = sbun->ima ? IMA_BIT : 0;
		/* $B>.J8@a$NA48uJd<h$j=P$7$N;~$K$O!"A08e$N@\B3$r8+$k(J
		 * $BI,MW$,$"$k(J
		 */
		if ((conn & CONNECT_PREV) && sbun->status == WNN_CONNECT) {
			scp->status |= CONNECT_PREV;
		}
		if ((conn & CONNECT_NEXT) &&
		    sbun->status_bkwd == WNN_CONNECT_BK) {
			scp->status |= CONNECT_NEXT;
		}
		sp = sbun->kanji;
		while (*wp++ = *sp++)
			;
		wp--;
		sp = sbun->fuzoku;
		while (*wp++ = *sp++)
			;
		scp++;
	}

	/* $B<B:]$K(J candBuf $B$KF~$l$i$l$?8uJd$N?t(J */
	buf->nCand = scp - scandp;

	return 0;
}

/* setLCandData -- $B<h$j=P$7$?%+%l%s%HBgJ8@a$NA48uJd$r3JG<$9$k(J */
static int
setLCandData(buf, ncand, dbun)
jcConvBuf *buf;
int ncand;
struct wnn_dai_bunsetsu *dbun;
{
	jcClause	*clp, *clps, *clpe;
	jcLCand *lcandp, *lcp;
	jcSCand *scandp, *scp;
	struct wnn_sho_bunsetsu *sbun;
	wchar_t	*sp, *spend;
	wchar_t *wp;
	int	nscand;
	int	bytesneeded;
	int	i, j;

	clps = buf->clauseInfo + buf->curLCStart;
	clpe = buf->clauseInfo + buf->curLCEnd;

	/* buf->candBuf $B$KI,MW$JBg$-$5$rD4$Y$k(J */
	bytesneeded = getLCandDataLen(clps, clpe, ncand, dbun);
	if (bytesneeded > buf->candSize &&
	    resizeCandBuf(buf, bytesneeded) < 0) {
		return -1;
	}

	/* $B>.J8@a$N?t$r?t$($k(J */
	nscand = 0;
	for (i = 0; i < ncand; i++) {
		nscand += dbun[i].sbncnt;
	}
	nscand += buf->curLCEnd - buf->curLCStart;
	lcandp = lcp = (jcLCand *)buf->candBuf;
	scandp = scp = (jcSCand *)(lcandp + (ncand + 1));
	wp = (wchar_t *)(scandp + nscand);

	/* $B$^$:!"<!8uJd$N:G=i$K!"8=:_$NBgJ8@a$NFbMF$rF~$l$k(J */
	lcp->nscand = buf->curLCEnd - buf->curLCStart;
	lcp++->scand = scp;
	clp = clps;
	while (clp < clpe) {
		scp->kanji = wp;
		scp->kanalen = (clp + 1)->kanap - clp->kanap;
		scp->fzkoffset = clp->fzkp - clp->kanap;
		scp->dicno = clp->dicno;
		scp->entry = clp->entry;
		scp->hinshi = clp->hinshi;
		scp->kangovect = clp->kangovect;
		scp->status = clp->imabit ? IMA_BIT : 0;
		if (clp != clps) scp->status |= CONNECT_PREV;
		if (clp != clpe - 1) scp->status |= CONNECT_NEXT;
		sp = clp->dispp;
		spend = (clp + 1)->dispp;
		while (sp < spend)
			*wp++ = *sp++;
		*wp++ = 0;
		scp++;
		clp++;
	}

	/* $B<!$KA48uJd$N>pJs$rF~$l$k(J */
	for (i = 0; i < ncand; i++, dbun++) {
		if (findLCand(lcandp, lcp, dbun)) {
			continue;
		}
		sbun = dbun->sbn;
		lcp->nscand = dbun->sbncnt;
		lcp->scand = scp;
		for (j = 0; j < dbun->sbncnt; j++) {
			scp->kanji = wp;
			scp->kanalen = sbun->end - sbun->start + 1;
			scp->fzkoffset = sbun->jiriend - sbun->start + 1;
			scp->dicno = sbun->dic_no;
			scp->entry = sbun->entry;
			scp->hinshi = sbun->hinsi;
			scp->kangovect = sbun->kangovect;
			scp->status = sbun->ima ? IMA_BIT : 0;
			/* $BA08e$NJ8@a$H$N@\B3>pJs$rF~$l$k(J
			 * $BBgJ8@a$NC1J8@aJQ49$J$N$G!"(J
			 * $B:G=i$N>.J8@a0J30$OA0$K$D$J$,$k$7!"(J
			 * $B:G8e$N>.J8@a0J30$O8e$m$K$D$J$,$k(J
			 */
			if (j != 0) {
				scp->status |= CONNECT_PREV;
			}
			if (j != dbun->sbncnt - 1) {
				scp->status |= CONNECT_NEXT;
			}
			sp = sbun->kanji;
			while (*wp++ = *sp++)
				;
			wp--;
			sp = sbun->fuzoku;
			while (*wp++ = *sp++)
				;
			scp++;
			sbun++;
		}
		lcp++;
	}

	/* $B<B:]$K(J candBuf $B$KF~$l$i$l$?8uJd$N?t(J */
	buf->nCand = lcp - lcandp;

	return 0;
}

/* getSCandDataLen -- $BA48uJd$N>pJs$rF~$l$k$N$KI,MW$J%a%b%j$NNL$rD4$Y$k(J ($B>.J8@a(J) */
static int
getSCandDataLen(clp, ncand, sbun)
jcClause *clp;
int ncand;
struct wnn_sho_bunsetsu *sbun;
{
	int	nbyte = 0;
	int	i;

	/* $B8=:_I=<($5$l$F$$$kJ8@a$bA48uJd$K4^$a$k$N$G!"(J
	 * ncand + 1 $B8D$V$sI,MW(J
	 */
	nbyte = sizeof(jcSCand) * (ncand + 1);

	nbyte += (((clp + 1)->dispp - clp->dispp) + 1) * sizeof(wchar_t);
	for (i = 0; i < ncand; i++, sbun++) {
		nbyte += (wstrlen(sbun->kanji) + wstrlen(sbun->fuzoku) + 1) *
		    sizeof(wchar_t);
	}

	return nbyte;
}

/* getLCandDataLen -- $BA48uJd$N>pJs$rF~$l$k$N$KI,MW$J%a%b%j$NNL$rD4$Y$k(J ($BBgJ8@a(J) */
static int
getLCandDataLen(clps, clpe, ncand, dbun)
jcClause *clps;
jcClause *clpe;
int ncand;
struct wnn_dai_bunsetsu *dbun;
{
	int	ndbun;
	int	nsbun;
	int	nchar;
	int	i;

	/* $BBgJ8@a$N?t(J */
	ndbun = ncand;

	/* $B>.J8@a$N?t(J */
	nsbun = 0;
	for (i = 0; i < ncand; i++) {
		nsbun += dbun[i].sbncnt;
	}

	/* $B4A;z$ND9$5(J */
	nchar = 0;
	for (i = 0; i < ncand; i++) {
		int	j = dbun[i].sbncnt;
		struct wnn_sho_bunsetsu *sbun = dbun[i].sbn;

		while (j-- > 0) {
			nchar += wstrlen(sbun->kanji) +
				wstrlen(sbun->fuzoku) + 1;
			sbun++;
		}
	}

	/* $B8=:_I=<($5$l$F$$$kJ8@a$bA48uJd$K4^$a$k(J */
	ndbun += 1;
	nsbun += clpe - clps;
	nchar += clpe->dispp - clps->dispp;

	return ndbun * sizeof(jcLCand) + nsbun * sizeof(jcSCand) +
	    nchar * sizeof(wchar_t);
}

/* changeCand -- $B%+%l%s%HJ8@a$r;XDj$5$l$?HV9f$N8uJd$GCV$-49$($k(J */
static int
changeCand(buf, n)
jcConvBuf *buf;
int n;
{
	int	buflen, oldlen, newlen;
	int	oldclen, newclen;
	wchar_t	*p;
	wchar_t	*kanap, *dispp;

	if (buf->candKind == CAND_SMALL) {
		jcSCand	*candp = ((jcSCand *)buf->candBuf) + n;
		jcClause	*clp = buf->clauseInfo + buf->curClause;
		/* $B%+%l%s%H>.J8@a$rJQ$($k(J */
		/* $B%G%#%9%W%l%$%P%C%U%!$N%5%$%:$N%A%'%C%/(J */
		newlen = wstrlen(candp->kanji);
		oldlen = (clp + 1)->dispp - clp->dispp;
		buflen = (buf->displayEnd - buf->displayBuf) + newlen - oldlen;
		if (buflen > buf->bufferSize &&
		    resizeBuffer(buf, buflen) < 0) {
			return -1;
		}
		/* $B%G%#%9%W%l%$%P%C%U%!$rF0$+$9(J */
		moveDBuf(buf, buf->curClause + 1, newlen - oldlen);
		/* $B8uJdJ8;zNs$r%3%T!<$9$k(J */
		(void)bcopy((char *)candp->kanji, (char *)clp->dispp,
			    newlen * sizeof(wchar_t));
		/* clauseInfo $B$N%"%C%W%G!<%H(J */
		changecinfo(clp, candp, clp->kanap, clp->dispp);
		/* $B8e$m$NJ8@a$N(J ltop $B$N@_Dj(J */
		(clp + 1)->ltop = !(candp->status & CONNECT_NEXT);
		/* $B%+%l%s%HBgJ8@a$N:F@_Dj(J */
		setCurClause(buf, buf->curClause);
	} else {
		jcLCand	*candp = ((jcLCand *)buf->candBuf) + n;
		jcSCand	*scandp;
		jcClause	*clps = buf->clauseInfo + buf->curLCStart;
		jcClause	*clpe = buf->clauseInfo + buf->curLCEnd;
		int	i;

		/* $B%+%l%s%HBgJ8@a$rJQ$($k(J */

		/* $B%G%#%9%W%l%$%P%C%U%!$N%5%$%:$N%A%'%C%/(J */
		newlen = 0;
		scandp = candp->scand;
		for (i = 0; i < candp->nscand; i++) {
			newlen += wstrlen(scandp++->kanji);
		}
		oldlen = clpe->dispp - clps->dispp;
		buflen = (buf->displayEnd - buf->displayBuf) + newlen - oldlen;
		if (buflen > buf->bufferSize &&
		    resizeBuffer(buf, buflen) < 0) {
			return -1;
		}

		/* caluseInfo $B$N%5%$%:$N%A%'%C%/(J */
		oldclen = buf->curLCEnd - buf->curLCStart;
		newclen = candp->nscand;
		if (buf->nClause + newclen - oldclen > buf->clauseSize &&
		    resizeCInfo(buf, buf->nClause + newclen - oldclen) < 0) {
			return -1;
		}

		/* $B%G%#%9%W%l%$%P%C%U%!$rF0$+$9(J */
		moveDBuf(buf, buf->curLCEnd, newlen - oldlen);

		/* clauseInfo $B$rF0$+$9(J */
		moveCInfo(buf, buf->curLCEnd, newclen - oldclen);

		/* $B8uJdJ8;zNs$r%3%T!<$7!"F1;~$K(J clauseInfo $B$r(J
		 * $B%"%C%W%G!<%H$9$k(J
		 */
		clps = buf->clauseInfo + buf->curLCStart;
		scandp = candp->scand;
		kanap = clps->kanap;
		dispp = clps->dispp;
		for (i = 0; i < candp->nscand; i++) {
			changecinfo(clps, scandp, kanap, dispp);
			clps++->ltop = (i == 0);
			kanap += scandp->kanalen;
			p = scandp++->kanji;
			while (*p)
				*dispp++ = *p++;
		}

		/* $B%+%l%s%HJ8@a$N%;%C%H(J */
		setCurClause(buf, buf->curLCStart);

		/* $B8uJdJ8@a$N%;%C%H(J
		 * $B$3$NF~$l49$($K$h$C$F!"%+%l%s%HBgJ8@a$N=*$j$,(J
		 * $B0\F0$9$k$3$H$,$"$k$N$G!"$3$3$G8uJdJ8@a$r(J
		 * $B:F@_Dj$9$k(J
		 */
		buf->candClause = buf->curLCStart;
		buf->candClauseEnd = buf->curLCEnd;
	}
	return 0;
}

/* setupCandBuf -- $B<!8uJd%P%C%U%!$K<!8uJd$N>pJs$rMQ0U$9$k(J
 *		   $B4{$K%;%C%H$5$l$F$$$l$P2?$b$7$J$$(J
 */
static int
setupCandBuf(buf, small)
jcConvBuf *buf;
int small;
{
	if (small) {
		if (buf->candKind != CAND_SMALL ||
		    buf->curClause != buf->candClause) {
			/* $B%+%l%s%H>.J8@a$N8uJd$r<h$j=P$9(J */
			buf->candClause = -1;
			if (getSCandidates(buf) < 0)
				return -1;
		}
	} else {
		if (buf->candKind != CAND_LARGE ||
		    buf->curLCStart != buf->candClause ||
		    buf->curLCEnd != buf->candClauseEnd) {
			/* $B%+%l%s%HBgJ8@a$N8uJd$r<h$j=P$9(J */
			buf->candClause = -1;
			if (getLCandidates(buf) < 0)
				return -1;
		}
	}

	return 0;
}

/* checkAndResetCandidates -- $B<!8uJd%P%C%U%!$NFbMF$,M-8z$+$I$&$+%A%'%C%/$7$F!"(J
 *			      $BI,MW$J=hM}$r9T$J$&(J
 */
static void
checkAndResetCandidates(buf, cls, cle)
jcConvBuf *buf;
int cls;
int cle;
{
	/* $BJ8@aHV9f(J cls $B$+$i(J cle - 1 $B$^$G$NJ8@a$,JQ99$5$l$k(J
	 * $B<!8uJd%P%C%U%!$K$O$$$C$F$$$k8uJdJ8@a$,$3$NCf$K4^$^$l$F$$$l$P(J
	 * $B<!8uJd%P%C%U%!$NFbMF$rL58z$K$7$J$/$F$O$J$i$J$$(J
	 *
	 * $B$I$N$h$&$J>l9g$+$H$$$&$H!"(J
	 * 1. buf->candKind $B$,(J CAND_SMALL $B$G!"(J
	 *	cls <= buf->candClause < cle
	 * 2. buf->candKind $B$,(J CAND_LARGE $B$G!"(J
	 *	buf->candClause < cle $B$+$D(J cls < buf->candClauseEnd 
	 */
	if (buf->candKind == CAND_SMALL)
		buf->candClauseEnd = buf->candClause + 1;
	if (buf->candClause < cle && cls < buf->candClauseEnd) {
		/* $BL58z$K$9$k(J */
		buf->candClause = buf->candClauseEnd = -1;
	}
}

/* addResetClause -- $B;XDj$5$l$?J8@a$NC18l$r%j%;%C%H%(%s%H%j$N%j%9%H$KF~$l$k(J */
static void
addResetClause(buf, cls, cle)
jcConvBuf *buf;
int cls;
int cle;
{
	jcClause	*clp;

	for (clp = buf->clauseInfo + cls; cls < cle; cls++, clp++) {
		if (clp->conv == 1 && clp->imabit) {
			addResetEntry(buf, clp->dicno, clp->entry);
		}
	}
}

/* addResetCandidate -- $B;XDj$5$l$?HV9f0J30$N8uJd$r%j%;%C%H%j%9%H$KF~$l$k(J */
static void
addResetCandidate(buf, n)
jcConvBuf *buf;
int n;
{
	int	i, j;
	int	ncand = buf->nCand;

	if (buf->candClause < 0) return;

	if (buf->candKind == CAND_SMALL) {
		jcSCand	*scp = (jcSCand *)buf->candBuf;
		for (i = 0; i < ncand; i++, scp++) {
			if (i != n && scp->status & IMA_BIT)
				addResetEntry(buf, scp->dicno, scp->entry);
		}
	} else {	/* CAND_LARGE */
		jcLCand	*lcp = (jcLCand *)buf->candBuf;
		for (i = 0; i < ncand; i++, lcp++) {
			jcSCand	*scp;
			if (i == n) continue;
			for (j = 0, scp = lcp->scand; j < lcp->nscand;
			     j++, scp++) {
				if (scp->status & IMA_BIT)
					addResetEntry(buf, scp->dicno,
						      scp->entry);
			}
		}
	}
}

/* addResetEntry -- $B:#;H$C$?$h%S%C%H$rMn$9BP>]$N%(%s%H%j$r%j%9%H$KIU$12C$($k(J */
static void
addResetEntry(buf, dicno, entry)
jcConvBuf *buf;
int dicno;
int entry;
{
	jcEntry	*ep;
	int	i;

	/*
	 * $B$3$N%U%!%s%/%7%g%s$r8F$V$N$O!"(J
	 *
	 * 1. jcNext() $B$,8F$P$l$?;~$K!"%+%l%s%HJ8@a$KF~$C$F$$$?8uJd(J
	 * 2. jcSelect() $B$,8F$P$l$?;~$K!"A*$P$l$J$+$C$?8uJd(J
	 * 3. jcExpand()/jcShrink() $B$,8F$P$l$?;~$K!"(J
	 *    $B%+%l%s%HJ8@a$H$=$N<!$NJ8@a$KF~$C$F$$$?8uJd(J
	 *
	 * $B$N#3<oN`$K$9$k(J
	 * 3. $B$O$A$g$C$H$d$j2a$.(J (jllib $B$O$b$C$H8-$$$3$H$r$7$F$$$k(J)
	 * $B$@$,!"$^$"Mn$5$J$$$h$j$OMn$9J}$,$$$$$N$G$O$J$$$+$H$$$&$3$H$G(J
	 */

	/* $B$9$G$K%j%9%H$K$O$$$C$F$$$J$$$+%A%'%C%/$9$k(J */
	ep = (jcEntry *)buf->resetBuf;
	i = buf->nReset;
	while (i-- > 0) {
		if (ep->entry == entry && ep->dicno == dicno) {
			/* $BF~$C$F$$$?(J */
			return;
		}
		ep++;
	}

	/* $B%P%C%U%!$NBg$-$5$N%A%'%C%/(J */
	if (buf->nReset >= buf->resetSize) {
		char	*p;
		p = realloc(buf->resetBuf,
			    (buf->resetSize + 10) * sizeof(jcEntry));
		if (p == NULL) return;
		buf->resetSize += 10;
		buf->resetBuf = p;
	}
	ep = (jcEntry *)buf->resetBuf + buf->nReset++;
	ep->dicno = dicno;
	ep->entry = entry;
}

/* saveDicAll -- $B4D6-$G;HMQ$5$l$F$$$k<-=q$r%;!<%V$9$k(J */
static void
saveDicAll(buf)
jcConvBuf *buf;
{
	int	ndic, i;
	WNN_DIC_INFO	*diclist;
	struct wnn_env	*env = buf->env;

	if ((ndic = js_dic_list(env, &jsbuf)) < 0)
		return;

	diclist = (WNN_DIC_INFO *)jsbuf.buf;
	for (i = 0; i < ndic; i++) {
		/* $BEPO?2DG=7A<0$G$J$$<-=q$r%;!<%V$9$kI,MW$O$J$$$@$m$&(J */
		if (diclist->type == WNN_UD_DICT && diclist->body >= 0) {
			/* $B<-=qK\BN$N%;!<%V(J */
			if (diclist->localf) {
				(void)js_file_write(env, diclist->body,
						    diclist->fname);
			} else {
				(void)js_file_receive(env, diclist->body,
						      diclist->fname);
			}
		}
		if (diclist->hindo >= 0) {
			/* $BIQEY%U%!%$%k(J */
			if (diclist->hlocalf) {
				(void)js_file_write(env, diclist->hindo,
						    diclist->hfname);
			} else {
				(void)js_file_receive(env, diclist->hindo,
						      diclist->hfname);
			}
		}
		diclist++;
	}
}

/*
 *	$B$3$3$+$i(J Public $B$J%U%!%s%/%7%g%s(J
 */

/* jcCreateBuf -- $BJQ49%P%C%U%!$N:n@.(J */
jcConvBuf *
jcCreateBuffer(env, nclause, buffersize)
struct wnn_env *env;
int nclause;
int buffersize;
{
	jcConvBuf	*buf;

	/* $B$^$:(J jcConvBuf $B$N3NJ](J */
	if ((buf = (jcConvBuf *)malloc(sizeof(jcConvBuf))) == NULL) {
		jcErrno = JE_NOCORE;
		return NULL;
	}
	(void)bzero((char *)buf, sizeof(jcConvBuf));
	buf->env = env;

	/* $B<!$K3F<o%P%C%U%!$N3NJ](J */

	/* $B$^$:!"$+$J%P%C%U%!$H%G%#%9%W%l%$%P%C%U%!(J */
	buf->bufferSize = (buffersize <= 0) ? DEF_BUFFERSIZE : buffersize;
	/* $B%P%C%U%!$N:G8e$r(J NULL $B%?!<%_%M!<%H$9$k$3$H$,$"$k$N$G!"(J
	 * 1$BJ8;zJ8Bg$-$/$7$F$*$/(J
	 */
	buf->kanaBuf = (wchar_t *)malloc((buf->bufferSize + 1) *
					 sizeof(wchar_t));
	buf->displayBuf = (wchar_t *)malloc((buf->bufferSize + 1) *
					    sizeof(wchar_t));

	/* $B<!$K(J clauseInfo $B%P%C%U%!(J */
	buf->clauseSize = (nclause <= 0) ? DEF_CLAUSESIZE : nclause;
	/* clauseInfo $B%P%C%U%!$O(J nclause + 1 $B8D%"%m%1!<%H$9$k(J
	 * $B$J$<$+$H$$$&$H(J clauseinfo $B$O%G%j%_%?$H$7$FMWAG$r(J
	 * 1$B8D;H$&$N$G(J nclause $B8D$NJ8@a$r07$&$?$a$K$O(J nclause + 1 $B8D$N(J
	 * $BBg$-$5$r;}$?$J$1$l$P$J$i$J$$$+$i$G$"$k(J
	 */
	buf->clauseInfo = (jcClause *)malloc((buf->clauseSize + 1)
					     * sizeof(jcClause));

	/* $B<!8uJd%P%C%U%!(J */
	buf->candSize = DEF_CANDSIZE;
	buf->candBuf = (char *)malloc(buf->candSize);

	/* $B%j%;%C%H%P%C%U%!(J */
	buf->resetSize = DEF_RESETSIZE;
	buf->resetBuf = (char *)malloc(buf->resetSize * sizeof(jcEntry));

	if (buf->kanaBuf == NULL || buf->displayBuf == NULL ||
	    buf->clauseInfo == NULL || buf->candBuf == NULL ||
	    buf->resetBuf == NULL) {
		/* malloc() $B$G$-$J$+$C$?(J */
		Free(buf->kanaBuf);
		Free(buf->displayBuf);
		Free(buf->clauseInfo);
		Free(buf->candBuf);
		Free(buf->resetBuf);
		Free(buf);
		jcErrno = JE_NOCORE;
		return NULL;
	}

	(void)jcClear(buf);
	return buf;
}

/* jcDestroyBuffer -- $BJQ49%P%C%U%!$N>C5n(J */
int
jcDestroyBuffer(buf, savedic)
jcConvBuf *buf;
int savedic;
{
	/* $B%"%m%1!<%H$7$?%a%b%j$N2rJ|(J */
	Free(buf->kanaBuf);
	Free(buf->displayBuf);
	Free(buf->clauseInfo);
	Free(buf->candBuf);
	Free(buf->resetBuf);

	/* savedic $B$,(J 0 $B$G$J$1$l$P!"4D6-$K%m!<%I$5$l$F$$$kA4$F$N%U%!%$%k$r(J
	 * save $B$9$k(J
	 */
	if (savedic)
		saveDicAll(buf);

	Free(buf);

	return 0;
}

/* jcClear -- jclib $B$N%/%j%"(J $B?7$?$JJQ49$r;O$a$kKh$K8F$P$J$1$l$P$J$i$J$$(J */
int
jcClear(buf)
jcConvBuf *buf;
{
	/* $B=i4|CM$N@_Dj(J */
	buf->nClause = buf->curClause = buf->curLCStart = 0;
	buf->curLCEnd = 1;
	buf->kanaEnd = buf->kanaBuf;
	buf->displayEnd = buf->displayBuf;
	buf->clauseInfo[0].kanap = buf->kanaBuf;
	buf->clauseInfo[0].fzkp = buf->kanaBuf;
	buf->clauseInfo[0].dispp = buf->displayBuf;
	buf->clauseInfo[0].conv = 0;
	buf->clauseInfo[0].ltop = 1;
	buf->dot = buf->kanaBuf;
	buf->fixed = 0;
	buf->candClause = buf->candClauseEnd = -1;
	buf->nReset = 0;
	jcErrno = JE_NOERROR;

	return 0;
}

/* jcConvert -- $B%+%l%s%HJ8@a0J9_$r$+$J4A;zJQ49$9$k(J */
int
jcConvert(buf, small, tan, jump)
jcConvBuf *buf;
int small;
int tan;
int jump;
{
	int	ret;

	CHECKFIXED(buf);

	if (buf->curClause == buf->nClause) {
		/* $B%+%l%s%HJ8@a$,:G8e$NJ8@a$G$7$+$b6u(J */
		jcErrno = JE_CLAUSEEMPTY;
		return -1;
	}

	/* $B$b$7<!8uJd%P%C%U%!$NFbMF$,%+%l%s%HJ8@a0J9_$@$HL50UL#$K$J$k(J */
	checkAndResetCandidates(buf,
				small ? buf->curClause : buf->curLCStart,
				buf->nClause);

	if (tan) {
		ret = tanConvert(buf, small);
	} else {
		ret = renConvert(buf, small);
	}

	if (ret < 0)
		return ret;

	if (jump) {
		/* $B%I%C%H$H%+%l%s%HJ8@a$rJ8$N:G8e$K0\F0$5$;$k(J */
		buf->curClause = buf->curLCStart = buf->nClause;
		buf->curLCEnd = buf->nClause + 1;
		buf->dot = buf->kanaEnd;
	}
	return 0;
}

/* jcUnconvert -- $B%+%l%s%HBgJ8@a$rL5JQ49$N>uBV$KLa$9(J */
int
jcUnconvert(buf)
jcConvBuf *buf;
{
	jcClause	*clp = buf->clauseInfo + buf->curClause;

	CHECKFIXED(buf);

	if (buf->curClause == buf->nClause) {
		/* $B%+%l%s%HJ8@a$,:G8e$NJ8@a$G$7$+$b6u(J */
		jcErrno = JE_CLAUSEEMPTY;
		return -1;
	}

	if (!clp->conv) {
		/* $B%+%l%s%HJ8@a$OJQ49$5$l$F$$$J$$(J */
		/* $BL5JQ49$NJ8@a$O(J jclib $BFbIt$G$O>o$KBgJ8@a$H$7$F(J
		 * $B07$o$l$k$N$G!"%+%l%s%H>.J8@a$NJQ49>uBV$r8+$F!"(J
		 * $B$=$l$,JQ49>uBV$J$i%+%l%s%HBgJ8@aFb$N(J
		 * $BA4$F$N>.J8@a$OJQ49>uBV!"$=$&$G$J$1$l$PL5JQ49>uBV!"(J
		 * $B$K$J$k(J
		 */
		jcErrno = JE_NOTCONVERTED;
		return -1;
	}

	checkAndResetCandidates(buf, buf->curLCStart, buf->curLCEnd);

	if (unconvert(buf, buf->curLCStart, buf->curLCEnd) < 0)
		return -1;

	/* $BBgJ8@a$N@_Dj(J */
	clp = buf->clauseInfo + buf->curLCStart;
	clp->ltop = 1;
	(++clp)->ltop = 1;

	/* $B%+%l%s%HJ8@a$N:F@_Dj(J */
	buf->curClause = buf->curLCStart;
	buf->curLCEnd = buf->curLCStart + 1;

	/* $B%I%C%H$N@_Dj(J */
	DotSet(buf);

	return 0;
}

/* jcExpand -- $B%+%l%s%HJ8@a$r#1J8;z9-$2$k(J */
int
jcExpand(buf, small, convf)
jcConvBuf *buf;
int small;
int convf;
{
	CHECKFIXED(buf);

	return expandOrShrink(buf, small, 1, convf);
}

/* jcShrink -- $B%+%l%s%HJ8@a$r#1J8;z=L$a$k(J */
int
jcShrink(buf, small, convf)
jcConvBuf *buf;
int small;
int convf;
{
	CHECKFIXED(buf);

	return expandOrShrink(buf, small, 0, convf);
}

/* jcKana -- $B%+%l%s%HJ8@a$r$+$J$K$9$k(J */
int
jcKana(buf, small, kind)
jcConvBuf *buf;
int small;
int kind;
{
	jcClause	*clp;
	wchar_t		*kanap, *kanaendp, *dispp;
	int		start, end;
	int		conv;
	int		c;

	CHECKFIXED(buf);

	/* $BJ8@aHV9f$N%A%'%C%/(J */
	if (buf->curClause >= buf->nClause) {
		/* $B%+%l%s%HJ8@a$,:G8e$NJ8@a$G$7$+$b6u$@$C$?>l9g(J
		 * $B$3$N>l9g%(%i!<$K$7$F$b$h$$$,(J...
		 */
		return 0;
	}

	/*
	 * $B%+%l%s%HJ8@a$,JQ49$5$l$F$$$l$P$$$C$?$sL5JQ49$K$9$k(J
	 */

	/* $B$"$H$GJQ49>uBV$r$b$H$KLa$9$?$a!"JQ49>uBV$r%;!<%V$7$F$*$/(J */
	conv = buf->clauseInfo[buf->curClause].conv;

	if (small) {
		start = buf->curClause;
		end = start + 1;
	} else {
		start = buf->curLCStart;
		end = buf->curLCEnd;
	}

	/* $B<!8uJd%P%C%U%!$NFbMF$N%A%'%C%/(J */
	checkAndResetCandidates(buf, start, end);

	if (unconvert(buf, start, end) < 0) {
		return -1;
	}

	/* small $B$,(J 0$B!"$D$^$j%+%l%s%HJ8@a$H$7$FBgJ8@a$rA*Br$7$?>l9g!"(J
	 * $B$=$NCf$N>.J8@a$O0l$D$K$^$H$a$i$l$k$N$G!"(JcurClause $B$H(J
	 * curLCEnd $B$rJQ$($kI,MW$,$"$k(J
	 */
	if (!small) {
		buf->curClause = buf->curLCStart;
		buf->curLCEnd = buf->curLCStart + 1;
	}

	/* $B$+$JJQ49$9$k(J */
	/* $B%G%#%9%W%l%$%P%C%U%!$@$1$G$O$J$/!"$+$J%P%C%U%!$bJQ49$9$k(J */
	/* $B$3$l$K$O$5$7$?$kM}M3$O$J$$$,!"$^$"!"(JVer3 $BHG$N(J jclib $B$,(J
	 * $B$=$&$@$C$?$N$G!D(J
	 */
	clp = buf->clauseInfo + buf->curClause;
	kanap = clp->kanap;
	kanaendp = (clp + 1)->kanap;
	dispp = clp->dispp;

	if (kind == JC_HIRAGANA) {	/* $B%+%?%+%J"*$R$i$,$J(J */
		/* $B%+%?%+%J$r$R$i$,$J$KJQ49$9$k:]$K$O$R$i$,$J$K$J$$;z(J
		 * "$B%t%u%v(J" $B$,$"$k$N$G$$$-$*$$$GJQ49$7$F$7$^$o$J$$$h$&$K(J
		 * $B5$$rIU$1$J$1$l$P$J$i$J$$(J
		 * ($B$^$"<B:]$O5$$r$D$1$k$H$$$&$[$I$N$b$N$G$O$J$$$,(J)
		 */
		while (kanap < kanaendp) {
			c = *kanap;
			if ((KANABEG + KATAOFFSET) <= c &&
					c <= (KANAEND + KATAOFFSET)) {
				*kanap = *dispp = c - KATAOFFSET;
			}
			kanap++, dispp++;
		}
	} else {	/* $B$R$i$,$J"*%+%?%+%J(J */
		while (kanap < kanaendp) {
			c = *kanap;
			if (KANABEG <= c && c <= KANAEND) {
				*kanap = *dispp = c + KATAOFFSET;
			}
			kanap++, dispp++;
		}
	}

	/*
	 * $BJQ49>uBV$r$b$H$KLa$7$F$*$/(J
	 */

	/* $B$H$O$$$C$F$b4{$KJQ49$5$l$?J8@a$N>l9g!"$3$l$NIQEY>pJs$r(J
	 * $B%5!<%P$KAw$k$H$^$:$$$N$G!"$"$H$G$+$JJQ49$7$?$3$H$,$o$+$k$h$&$K(J
	 * jcClause.conv $B$O(J -1 $B$K%;%C%H$9$k(J
	 */
	if (conv) {
		clp->conv = -1;
		clp->hinshi = WNN_ALL_HINSI;
		clp->fzkp = (clp + 1)->kanap;
	} else {
		clp->conv = 0;
	}
	return 0;
}

/* jcFix -- $B3NDj$9$k(J */
int
jcFix(buf)
jcConvBuf *buf;
{
	struct wnn_env	*env = buf->env;
	jcEntry		*ep;
	jcClause	*clp;
	int		i;

	if (buf->fixed) {
		/* $B4{$K3NDj$5$l$F$$$k(J
		 * $B%(%i!<$K$7$F$b$h$$$,!D(J
		 */
		return 0;
	}

	/* $BIQEY>pJs$r%;%C%H$9$k(J */

	/* $B$^$:$O:#;H$C$?$h%S%C%H$rMn$9(J */
	ep = (jcEntry *)buf->resetBuf;
	for (i = 0; i < buf->nReset; i++, ep++) {
		if (ep->dicno >= 0) {
			(void)js_hindo_set(env, ep->dicno, ep->entry,
					   WNN_IMA_OFF, WNN_HINDO_NOP);
		}
	}
	buf->nReset = 0;

	/* $B<!$K!"IQEY$r>e$2!":#;H$C$?$h%S%C%H$rN)$F$k(J */
	clp = buf->clauseInfo;
	for (i = 0; i < buf->nClause; i++) {
		/* $BJQ49%U%i%0(J clp->conv $B$O(J -1, 0, 1 $B$N#3<oN`$NCM$r$H$k(J
		 * $B$3$N$&$A!"(J-1 $B$O(J jclib $B$K$h$k5?;wJ8@a!"(J0 $B$OL$JQ49$N(J
		 * $BJ8@a$J$N$G!"$3$l$i$K4X$7$F$OL5;k$9$k(J
		 */
		if (clp->conv == 1) {
			(void)js_hindo_set(buf->env, clp->dicno, clp->entry,
					   WNN_IMA_ON, WNN_HINDO_INC);
		}
		clp++;
	}

	/* $B3NDj%U%i%0$rN)$F$k(J */
	buf->fixed = 1;

	return 0;
}

/* jcNext -- $B%+%l%s%HJ8@a$r<!8uJd(J/$BA08uJd$GCV$-49$($k(J */
int
jcNext(buf, small, prev)
jcConvBuf *buf;
int small;
int prev;
{
	int	curcand;

	CHECKFIXED(buf);

	if (!buf->clauseInfo[buf->curClause].conv) {
		/* $B$^$@JQ49$5$l$F$$$J$$(J */
		jcErrno = JE_NOTCONVERTED;
		return -1;
	}

	/* $B:#;H$o$l$F$$$kC18l$r%j%;%C%H%P%C%U%!$K2C$($k(J */
	if (small) {
		addResetClause(buf, buf->curClause, buf->curClause + 1);
	} else {
		addResetClause(buf, buf->curLCStart, buf->curLCEnd);
	}

	/* $B<!8uJd$N>pJs$r<!8uJd%P%C%U%!$KMQ0U$9$k(J */
	if (setupCandBuf(buf, small) < 0)
		return -1;

	if (buf->nCand == 1) {
		/* $B<!8uJd$,$J$$(J */
		jcErrno = JE_NOCANDIDATE;
		return -1;
	}

	if (prev) {
		curcand = buf->curCand - 1;
		if (curcand < 0)
			curcand = buf->nCand - 1;
	} else {
		curcand = buf->curCand + 1;
		if (curcand >= buf->nCand)
			curcand = 0;
	}

	/* $B%G%#%9%W%l%$%P%C%U%!$rJQ99(J */
	if (changeCand(buf, curcand) < 0)
		return -1;

	buf->curCand = curcand;

	return 0;
}

/* jcCandidateInfo -- $B<!8uJd$N?t$H8=:_$N8uJdHV9f$rD4$Y$k(J
 *		      $B$b$7<!8uJd$,$^$@%P%C%U%!$KF~$C$F$$$J$1$l$PMQ0U$9$k(J
 */
int
jcCandidateInfo(buf, small, ncandp, curcandp)
jcConvBuf *buf;
int small;
int *ncandp;
int *curcandp;
{
	CHECKFIXED(buf);

	if (!buf->clauseInfo[buf->curClause].conv) {
		/* $B$^$@JQ49$5$l$F$$$J$$(J */
		jcErrno = JE_NOTCONVERTED;
		return -1;
	}

	/* $B<!8uJd$N>pJs$r<!8uJd%P%C%U%!$KMQ0U$9$k(J */
	if (setupCandBuf(buf, small) < 0)
		return -1;

	*ncandp = buf->nCand;
	*curcandp = buf->curCand;

	return 0;
}

/* jcGetCandidate -- $B;XDj$5$l$?HV9f$N8uJd$r<h$j=P$9(J */
int
jcGetCandidate(buf, n, candstr)
jcConvBuf *buf;
int n;
wchar_t *candstr;
{
	int	ns;
	wchar_t	*p;
	jcLCand *lcandp;
	jcSCand *scandp;

	CHECKFIXED(buf);

	/* $BJ8@a$N%A%'%C%/(J */
	if (buf->candClause < 0) {
		jcErrno = JE_NOCANDIDATE;
		return -1;
	}

	/* $B8uJdHV9f$N%A%'%C%/(J */
	if (n < 0 || n >= buf->nCand) {
		jcErrno = JE_NOSUCHCANDIDATE;
		return -1;
	}

	/* $BJ8;zNs$r%3%T!<(J */
	if (buf->candKind == CAND_SMALL) {
		scandp = (jcSCand *)buf->candBuf;
		p = scandp[n].kanji;
		while (*candstr++ = *p++)
			;
	} else {
		lcandp = (jcLCand *)buf->candBuf;
		scandp = lcandp[n].scand;
		ns = lcandp[n].nscand;
		while (ns-- > 0) {
			p = scandp++->kanji;
			while (*candstr++ = *p++)
				;
			candstr--;
		}
	}

	return 0;
}

/* jcSelect -- $B%G%#%9%W%l%$%P%C%U%!$r;XDj$5$l$?8uJd$HCV$-49$($k(J */
int
jcSelect(buf, n)
jcConvBuf *buf;
int n;
{
	CHECKFIXED(buf);

	/* $BJ8@a$N%A%'%C%/(J */
	if (buf->candClause < 0) {
		jcErrno = JE_NOCANDIDATE;
		return -1;
	}

	/* $B8uJdHV9f$N%A%'%C%/(J */
	if (n < 0 || n >= buf->nCand) {
		jcErrno = JE_NOSUCHCANDIDATE;
		return -1;
	}

	/* $B;XDj$5$l$?8uJd0J30$N$b$N$r%j%;%C%H%j%9%H$KF~$l$k(J */
	addResetCandidate(buf, n);

	/* $B%G%#%9%W%l%$%P%C%U%!$rJQ99(J */
	if (changeCand(buf, n) < 0)
		return -1;

	/* CurCand $B$NJQ99(J */
	buf->curCand = n;

	return 0;
}

/* jcDotOffset -- $BBgJ8@a$N@hF,$+$i$N%I%C%H$N%*%U%;%C%H$rJV$9(J */
int
jcDotOffset(buf)
jcConvBuf *buf;
{
	return buf->dot - buf->clauseInfo[buf->curLCStart].kanap;
}

/* jcIsConverted -- $B;XDj$5$l$?J8@a$,JQ49$5$l$F$$$k$+$I$&$+$rJV$9(J */
int
jcIsConverted(buf, cl)
jcConvBuf *buf;
int cl;
{
	if (cl < 0 || cl > buf->nClause) {
		/* cl == jcNClause $B$N$H$-$r%(%i!<$K$7$F$b$$$$$N$@$1$l$I(J
		 * $B%+%l%s%HJ8@a$,(J jcNClause $B$N$H$-$,$"$k$N$G(J
		 * $B%(%i!<$H$O$7$J$$$3$H$K$7$?(J
		 */
		return -1;
	}
	return (buf->clauseInfo[cl].conv != 0);
}

/* jcMove -- $B%I%C%H!&%+%l%s%HJ8@a$r0\F0$9$k(J */
int
jcMove(buf, small, dir)
jcConvBuf *buf;
int small;
int dir;
{
	jcClause	*clp = buf->clauseInfo + buf->curClause;
	int		i;

	if (!clp->conv) {
		/* $B%+%l%s%HJ8@a$,JQ49$5$l$F$$$J$$$N$G!"%I%C%H$N0\F0$K$J$k(J */
		if (dir == JC_FORWARD) {
			if (buf->curClause == buf->nClause) {
				/* $B$9$G$K0lHV:G8e$K$$$k(J */
				jcErrno = JE_CANTMOVE;
				return -1;
			} else if (buf->dot == (clp + 1)->kanap) {
				/* $B%I%C%H$,%+%l%s%HJ8@a$N:G8e$K$"$k$N$G(J
				 * $BJ8@a0\F0$9$k(J
				 */
				goto clausemove;
			} else {
				buf->dot++;
			}
		} else {
			if (buf->dot == clp->kanap) {
				/* $B%I%C%H$,%+%l%s%HJ8@a$N@hF,$K$"$k$N$G(J
				 * $BJ8@a0\F0$9$k(J
				 */
				goto clausemove;
			} else
				buf->dot--;
		}
		return 0;
	}

clausemove:	/* $BJ8@a0\F0(J */
	clp = buf->clauseInfo;

	if (small) {
		/* $B>.J8@aC10L$N0\F0(J */
		if (dir == JC_FORWARD) {
			if (buf->curClause == buf->nClause) {
				jcErrno = JE_CANTMOVE;
				return -1;
			}
			buf->curClause++;
			if (buf->curClause >= buf->curLCEnd) {
				/* $BBgJ8@a$b0\F0$9$k(J */
				buf->curLCStart = buf->curLCEnd;
				for (i = buf->curLCStart + 1;
				     i <= buf->nClause && !clp[i].ltop; i++)
					;
				buf->curLCEnd = i;
			}
		} else {	/* JC_BACKWARD */
			if (buf->curClause == 0) {
				jcErrno = JE_CANTMOVE;
				return -1;
			}
			buf->curClause--;
			if (buf->curClause < buf->curLCStart) {
				/* $BBgJ8@a$b0\F0$9$k(J */
				buf->curLCEnd = buf->curLCStart;
				for (i = buf->curClause; !clp[i].ltop; i--)
					;
				buf->curLCStart = i;
			}
		}
	} else {
		/* $BBgJ8@aC10L$N0\F0(J */
		if (dir == JC_FORWARD) {
			if (buf->curLCStart == buf->nClause) {
				jcErrno = JE_CANTMOVE;
				return -1;
			}
			buf->curLCStart = buf->curClause = buf->curLCEnd;
			for (i = buf->curLCStart + 1;
			     i <= buf->nClause && !clp[i].ltop; i++)
				;
			buf->curLCEnd = i;
		} else {
			if (buf->curLCStart == 0) {
				jcErrno = JE_CANTMOVE;
				return -1;
			}
			buf->curLCEnd = buf->curLCStart;
			for (i = buf->curLCEnd - 1; !clp[i].ltop; i--)
				;
			buf->curLCStart = buf->curClause = i;
		}
	}

	/* $BJ8@a0\F0$7$?$i%I%C%H$O$=$NJ8@a$N@hF,$K0\F0$9$k(J */
	buf->dot = clp[buf->curClause].kanap;

	return 0;
}

/* jcTop -- $B%I%C%H!&%+%l%s%HJ8@a$rJ8$N@hF,$K0\F0$9$k(J */
int
jcTop(buf)
jcConvBuf *buf;
{
	/* $B%+%l%s%HJ8@a$r(J 0 $B$K$7$F%I%C%H$r@hF,$K;}$C$F$/$k(J */
	setCurClause(buf, 0);
	buf->dot = buf->kanaBuf;

	return 0;
}

/* jcBottom -- $B%I%C%H!&%+%l%s%HJ8@a$rJ8$N:G8e$K0\F0$9$k(J */
int
jcBottom(buf)
jcConvBuf *buf;
{
	/*
	 * Ver3 $BBP1~$N(J jclib $B$G$O!"%+%l%s%HJ8@a$r(J jcNClause $B$K$7$F(J
	 * $B%I%C%H$r:G8e$K;}$C$F$/$k$@$1$@$C$?(J
	 * $B$3$l$@$H!":G8e$NJ8@a$K$+$J$rF~$l$F$$$F!"%+!<%=%k$rF0$+$7$F(J
	 * jcBottom() $B$G85$KLa$C$F:F$S$+$J$rF~$l$k$H!"JL$NJ8@a$K(J
	 * $B$J$C$F$7$^$&(J
	 * $B$=$3$G!":G8e$NJ8@a$,L5JQ49>uBV$N;~$K$O!"%+%l%s%HJ8@a$O(J
	 * buf->nClause $B$G$O$J$/!"(Jbuf->nClause - 1 $B$K$9$k$3$H$K$9$k(J
	 */
	if (buf->nClause > 0 && !buf->clauseInfo[buf->nClause - 1].conv) {
		buf->curClause = buf->curLCStart = buf->nClause - 1;
		buf->curLCEnd = buf->nClause;
	} else {
		buf->curClause = buf->curLCStart = buf->nClause;
		buf->curLCEnd = buf->nClause + 1;
	}
	buf->dot = buf->kanaEnd;
	return 0;
}

/* jcInsertChar -- $B%I%C%H$N0LCV$K0lJ8;zA^F~$9$k(J */
int
jcInsertChar(buf, c)
jcConvBuf *buf;
int c;
{
	jcClause	*clp;
	wchar_t	*dot, *dispdot;
	int	ksizenew, dsizenew;

	CHECKFIXED(buf);

	/* $B8uJdJ8@a$,%+%l%s%HJ8@a$K$"$l$PL58z$K$9$k(J */
	checkAndResetCandidates(buf, buf->curLCStart, buf->curLCEnd);

	/*
	 * $B!&%+%l%s%HJ8@aHV9f$,(J buf->nClause $B$G$"$k>l9g(J
	 *	- $B$3$l$O%I%C%H$,:G8e$NJ8@a$N<!$K$"$k$H$$$&$3$H$J$N$G(J
	 *	  $B?7$7$$J8@a$r:n$k(J
	 * $B!&JQ49:Q$_$NJ8@a$N>l9g(J
	 *	- $BL5JQ49$N>uBV$KLa$7$F$+$iA^F~(J
	 * $B!&$=$NB>(J
	 *	- $BC1$KA^F~$9$l$P$h$$(J
	 */
	clp = buf->clauseInfo + buf->curLCStart;
	if (buf->curLCStart == buf->nClause) {
		/* $B?7$?$KJ8@a$r:n$k(J */
		/* clauseInfo $B$N%5%$%:$N%A%'%C%/(J */
		if (buf->nClause >= buf->clauseSize &&
		    resizeCInfo(buf, buf->nClause + 1) < 0) {
			return -1;
		}
		/* buf->nClause $B$N%"%C%W%G!<%H$H(J clauseInfo $B$N@_Dj(J */
		clp = buf->clauseInfo + ++(buf->nClause);
		clp->conv = 0;
		clp->ltop = 1;
		clp->kanap = buf->kanaEnd;
		clp->dispp = buf->displayEnd;
	} else if (clp->conv) {
		/* $BL5JQ49>uBV$K$9$k(J */
		if (unconvert(buf, buf->curLCStart, buf->curLCEnd) < 0)
			return -1;
		buf->curClause = buf->curLCStart;
		buf->curLCEnd = buf->curLCStart + 1;
		DotSet(buf);
	}

	clp = buf->clauseInfo + buf->curLCStart;

	/* $B%P%C%U%!$NBg$-$5$N%A%'%C%/(J */
	ksizenew = (buf->kanaEnd - buf->kanaBuf) + 1;
	dsizenew = (buf->displayEnd - buf->displayBuf) + 1;
	if ((ksizenew > buf->bufferSize || dsizenew > buf->bufferSize) &&
	    resizeBuffer(buf, ksizenew > dsizenew ? ksizenew : dsizenew) < 0) {
		    return -1;
	}

	/* $B$+$J%P%C%U%!$r%"%C%W%G!<%H(J */
	dot = buf->dot;
	/* $B%+%l%s%HJ8@a$N8e$m$r0lJ8;z$:$i$9(J */
	moveKBuf(buf, buf->curLCStart + 1, 1);
	/* $B%+%l%s%HJ8@aFb$N%I%C%H0J9_$r0lJ8;z$:$i$9(J */
	(void)bcopy((char *)dot, (char *)(dot + 1),
		    ((clp + 1)->kanap - dot) * sizeof(wchar_t));
	/* $BA^F~(J */
	*dot = c;

	/* $B%G%#%9%W%l%$%P%C%U%!$r%"%C%W%G!<%H(J */
	dispdot = clp->dispp + (dot - clp->kanap);
	/* $B%+%l%s%HJ8@a$N8e$m$r0lJ8;z$:$i$9(J */
	moveDBuf(buf, buf->curLCStart + 1, 1);
	/* $B%+%l%s%HJ8@aFb$N%I%C%H0J9_$r0lJ8;z$:$i$9(J */
	(void)bcopy((char *)dispdot, (char *)(dispdot + 1),
	      ((clp + 1)->dispp - dispdot) * sizeof(wchar_t));
	/* $BA^F~(J */
	*dispdot = c;

	/* $B%I%C%H$r99?7(J */
	buf->dot++;

	return 0;
}

/* jcDeleteChar -- $B%I%C%H$NA0$^$?$O8e$m$N0lJ8;z$r:o=|$9$k(J */
int
jcDeleteChar(buf, prev)
jcConvBuf *buf;
int prev;
{
	jcClause	*clp;
	wchar_t		*dot, *dispdot;

	CHECKFIXED(buf);

	clp = buf->clauseInfo;
	if (buf->nClause == 0) {
		/* $BJ8@a?t$,(J 0$B!"$D$^$j2?$bF~$C$F$$$J$$;~(J:
		 *	- $B%(%i!<(J
		 */
		jcErrno = JE_CANTDELETE;
		return -1;
	} else if (buf->curClause >= buf->nClause) {
		/* $B%+%l%s%HJ8@a$,:G8e$NJ8@a$N<!$K$"$k;~(J:
		 *	- prev $B$G$"$l$P!"A0$NJ8@a$N:G8e$NJ8;z$r:o=|(J
		 *	  $B%+%l%s%HJ8@a$OA0$NJ8@a$K0\F0$9$k(J
		 *	  $BI,MW$J$i$PA0$NJ8@a$rL5JQ49$KLa$7$F$+$i:o=|$9$k(J
		 *	  $BA0$NJ8@a$,$J$$$3$H$O$"$jF@$J$$(J
		 *	- !prev $B$J$i$P%(%i!<(J
		 */
		if (!prev) {
			jcErrno = JE_CANTDELETE;
			return -1;
		}
		(void)jcMove(buf, 0, JC_BACKWARD);
	} else if (clp[buf->curLCStart].conv) {
		/* $B%+%l%s%HJ8@a$,JQ49$5$l$F$$$k;~(J:
		 *	- prev $B$G$"$l$PA0$NJ8@a$N:G8e$NJ8;z$r:o=|(J
		 *	  $B%+%l%s%HJ8@a$OA0$NJ8@a$K0\F0$9$k(J
		 *	  $BI,MW$J$i$PA0$NJ8@a$rL5JQ49$KLa$7$F$+$i:o=|$9$k(J
		 *	  $B%+%l%s%HJ8@a$,@hF,$J$i$P%(%i!<(J
		 *	- !prev $B$J$i%+%l%s%HJ8@a$rL5JQ49$KLa$7$F!"J8@a$N(J
		 *	  $B:G=i$NJ8;z$r:o=|(J
		 */
		if (prev) {
			if (buf->curLCStart == 0) {
				jcErrno = JE_CANTDELETE;
				return -1;
			}
			(void)jcMove(buf, 0, JC_BACKWARD);
		}
	} else {
		/* $B%+%l%s%HJ8@a$,JQ49$5$l$F$$$J$$;~(J:
		 *	- prev $B$G$"$l$P%I%C%H$NA0$NJ8;z$r:o=|(J
		 *	  $B$?$@$7%I%C%H$,J8@a$N@hF,$K$"$l$PA0$NJ8@a$N(J
		 *	  $B:G8e$NJ8;z$r:o=|(J
		 *	  $B$=$N;~$K$O%+%l%s%HJ8@a$OA0$NJ8@a$K0\F0$9$k(J
		 *	  $BI,MW$J$i$PA0$NJ8@a$rL5JQ49$KLa$7$F$+$i:o=|$9$k(J
		 *	  $B%+%l%s%HJ8@a$,@hF,$J$i$P%(%i!<(J
		 *	- !prev $B$J$i%I%C%H$N<!$NJ8;z$r:o=|(J
		 *	  $B%I%C%H$,J8@a$N:G8e$NJ8;z$N<!$K$"$l$P%(%i!<(J
		 */
		if (prev) {
			if (buf->dot == clp[buf->curLCStart].kanap) {
				if (buf->curLCStart == 0) {
					jcErrno = JE_CANTDELETE;
					return -1;
				}
				(void)jcMove(buf, 0, JC_BACKWARD);
			}
		} else {
			if (buf->dot == clp[buf->curLCEnd].kanap) {
				jcErrno = JE_CANTDELETE;
				return -1;
			}
		}
	}

	/* $B8uJdJ8@a$,%+%l%s%HJ8@a$K$"$l$PL58z$K$9$k(J */
	checkAndResetCandidates(buf, buf->curLCStart, buf->curLCEnd);

	if (buf->clauseInfo[buf->curLCStart].conv) {
		/* $B%+%l%s%HJ8@a$,JQ49:Q$_$G$"$l$PL5JQ49$KLa$9(J */
		if (jcUnconvert(buf) < 0)
			return -1;
		/* prev $B$G$"$l$PJ8@a$N:G8e$NJ8;z!"$=$&$G$J$1$l$PJ8@a$N(J
		 * $B@hF,$NJ8;z$r:o=|$9$k(J
		 */
		if (prev) {
			buf->dot = buf->clauseInfo[buf->curLCEnd].kanap - 1;
		} else {
			buf->dot = buf->clauseInfo[buf->curLCStart].kanap;
		}
	} else {
		/* prev $B$J$i%I%C%H$r#1J8;zLa$7$F$*$/(J
		 * $B$3$&$9$l$P%I%C%H$N8e$m$NJ8;z$r:o=|$9$k$3$H$K$J$k(J
		 * $B:o=|$7=*$o$C$?$H$-$K%I%C%H$rF0$+$9I,MW$b$J$$(J
		 */
		if (prev)
			buf->dot--;
	}

	clp = buf->clauseInfo + buf->curLCStart;

	/* $B$+$J%P%C%U%!$r%"%C%W%G!<%H(J */
	dot = buf->dot;
	/* $B%+%l%s%HJ8@aFb$N%I%C%H0J9_$r0lJ8;z$:$i$9(J */
	(void)bcopy((char *)(dot + 1), (char *)dot,
		    ((clp + 1)->kanap - (dot + 1)) * sizeof(wchar_t));
	/* $B%+%l%s%HJ8@a$N8e$m$r0lJ8;z$:$i$9(J */
	moveKBuf(buf, buf->curLCEnd, -1);

	/* $B%G%#%9%W%l%$%P%C%U%!$r%"%C%W%G!<%H(J */
	dispdot = clp->dispp + (dot - clp->kanap);
	/* $B%+%l%s%HJ8@aFb$N%I%C%H0J9_$r0lJ8;z$:$i$9(J */
	(void)bcopy((char *)(dispdot + 1), (char *)dispdot,
		   ((clp + 1)->dispp - (dispdot + 1)) * sizeof(wchar_t));
	/* $B%+%l%s%HJ8@a$N8e$m$r0lJ8;z$:$i$9(J */
	moveDBuf(buf, buf->curLCEnd, -1);

	/* $B%+%l%s%HJ8@a$ND9$5$,#1$@$C$?>l9g$K$OJ8@a$,#18:$k$3$H$K$J$k(J */
	if (clp->kanap == (clp + 1)->kanap) {
		/* $BJ8@a$,$J$/$J$C$F$7$^$C$?(J */
		moveCInfo(buf, buf->curLCEnd, -1);
		setCurClause(buf, buf->curLCStart);
		DotSet(buf);
	}

	return 0;
}

/* jcChangeClause -- $B%+%l%s%HBgJ8@a$r;XDj$5$l$?J8;zNs$GCV$-49$($k(J */
int
jcChangeClause(buf, str)
jcConvBuf *buf;
wchar_t *str;
{
	jcClause	*clps, *clpe;
	wchar_t	*p;
	int	newlen;
	int	oklen, odlen;
	int	ksize, dsize;

	CHECKFIXED(buf);

	checkAndResetCandidates(buf, buf->curLCStart, buf->curLCEnd);

	clps = buf->clauseInfo + buf->curLCStart;
	clpe = buf->clauseInfo + buf->curLCEnd;

	newlen = 0;
	p = str;
	while (*p++)
		newlen++;

	/* $B$+$J%P%C%U%!$H%G%#%9%W%l%$%P%C%U%!$N%5%$%:$rD4$Y$F!"(J
	 * $BF~$i$J$+$C$?$iBg$-$/$9$k(J
	 */
	if (buf->curLCStart < buf->nClause) {
		oklen = clpe->kanap - clps->kanap;
		odlen = clpe->dispp - clps->dispp;
	} else {
		oklen = odlen = 0;
	}
	ksize = (buf->kanaEnd - buf->kanaBuf) + newlen - oklen;
	dsize = (buf->displayEnd - buf->displayBuf) + newlen - odlen;
	if (ksize > buf->bufferSize || dsize > buf->bufferSize) {
		if (resizeBuffer(buf, ksize > dsize ? ksize : dsize) < 0)
			return -1;
	}

	/* curLCStart $B$,(J nClause $B$KEy$7$$;~$@$1!"?7$?$KJ8@a$,:n$i$l$k(J */
	if (buf->curLCStart == buf->nClause) {
		/* clauseInfo $B$NBg$-$5$rD4$Y$k(J*/
		if (buf->nClause + 1 > buf->clauseSize) {
			if (resizeCInfo(buf, buf->nClause + 1) < 0)
				return -1;
		}
		/* $B?7$?$K$G$-$?(J clauseInfo $B$K$O!"(JnClause $BHVL\(J
		 * ($B$D$^$j:G8e$N(J clauseInfo) $B$NFbMF$r%3%T!<$7$F$*$/(J
		 */
		clpe = buf->clauseInfo + buf->nClause + 1;
		*clpe = *(clpe - 1);

		buf->nClause++;
	}

	clps = buf->clauseInfo + buf->curLCStart;
	clpe = buf->clauseInfo + buf->curLCEnd;

	/* $B$+$J%P%C%U%!$NJQ99(J */
	/* $B$^$:$O8e$m$r0\F0$5$;$k(J */
	moveKBuf(buf, buf->curLCEnd, newlen - oklen);
	/* str $B$r%3%T!<(J */
	(void)bcopy((char *)str, (char *)clps->kanap,
		    newlen * sizeof(wchar_t));
	/* $B%G%#%9%W%l%$%P%C%U%!$NJQ99(J */
	/* $B$^$:$O8e$m$r0\F0$5$;$k(J */
	moveDBuf(buf, buf->curLCEnd, newlen - odlen);
	/* str $B$r%3%T!<(J */
	(void)bcopy((char *)str, (char *)clps->dispp,
		    newlen * sizeof(wchar_t));

	/* clauseInfo $B$NJQ99(J */
	/* $B$^$:$O8e$m$r0\F0$5$;$k(J */
	if (clpe > clps + 1) {
		(void)bcopy((char *)clpe, (char *)(clps + 1),
			    (buf->nClause + 1 - buf->curLCEnd) *
			    sizeof(jcClause));
	}
	clps->conv = 0;
	clps->ltop = 1;
	(clps + 1)->ltop = 1;

	return 0;
}

/* jcSaveDic -- $B<-=q!&IQEY%U%!%$%k$r%;!<%V$9$k(J */
int
jcSaveDic(buf)
jcConvBuf *buf;
{
	saveDicAll(buf);
	return 0;
}

#ifdef DEBUG

#include <WStr.h>

#define UL	"\033[4m"	/* $B%"%s%@!<%i%$%s(J */
#define MD	"\033[1m"	/* xterm $B%\!<%k%I(J */
#define SO	"\033[7m"	/* $B%9%?%s%I%"%&%H(J */
#define ST	"\033[m"	/* $B%N!<%^%k(J */

static void
wputstr(ss, se)
register wchar_t	*ss, *se;
{
	wchar_t	wline[256];
	wchar_t	*p;
	unsigned char	line[256];

	p = wline;
	while (ss < se)
		*p++ = *ss++;
	*p = 0;
	(void)convJWStoSJIS(wline, line);
	fputs(line, stdout);
}

void
jcPrintKanaBuf(buf)
jcConvBuf *buf;
{
	int	i;

	for (i = 0; i < buf->nClause; i++) {
		wputstr(buf->clauseInfo[i].kanap,
			buf->clauseInfo[i + 1].kanap);
		if (i < buf->nClause - 1) {
			putchar('-');
		}
	}
	putchar('\n');
}

void
jcPrintDisplayBuf(buf)
jcConvBuf *buf;
{
	int	i;
	jcClause	*clp = buf->clauseInfo;

	for (i = 0; i < buf->nClause; i++) {
		if (buf->curLCStart <= i && i < buf->curLCEnd) {
			fputs(MD, stdout);
		}
		if (i == buf->curClause) {
			fputs(SO, stdout);
		}
		if (!clp->conv) {
			fputs(UL, stdout);
		}
		wputstr(clp->dispp, (clp + 1)->dispp);
		fputs(ST, stdout);
		if (i < buf->nClause - 1) {
			if ((clp + 1)->ltop) {
				putchar('|');
			} else {
				putchar('-');
			}
		}
		clp++;
	}
	putchar('\n');
}

void
jcPrintDetail(buf)
jcConvBuf *buf;
{
	int	i;
	jcClause	*clp = buf->clauseInfo;
	int	jlen;

	printf("NofClauses: %d, CurClause: %d, CurLClause: %d-%d\n",
	       buf->nClause, buf->curClause, buf->curLCStart, buf->curLCEnd);
	if (buf->candClause >= 0) {
		if (buf->candKind == CAND_SMALL) {
			printf("CandClause: %d, ", buf->candClause);
		} else {
			printf("CandClause: %d-%d, ",
			       buf->candClause, buf->candClauseEnd);
		}
		printf("NofCandidates: %d\n", buf->nCand);
	}
	for (i = 0; i < buf->nClause; i++) {
		printf("cl[%d]: %c ", i, clp->ltop ? '*' : ' ');
		if (clp->conv > 0) {
			printf("<converted> dic=%d,entry=%d,hinshi=%d ",
			       clp->dicno, clp->entry, clp->hinshi);
			if (clp->imabit) putchar('*');
			printf("kanji: ");
			jlen = ((clp + 1)->dispp - clp->dispp) -
			    ((clp + 1)->kanap - clp->fzkp);
			wputstr(clp->dispp, clp->dispp + jlen);
			if (clp->fzkp != (clp + 1)->kanap) {
				putchar('-');
				wputstr(clp->dispp + jlen, (clp + 1)->dispp);
			}
		} else if (clp->conv < 0) {
			printf("<pseudo> kanji: ");
			wputstr(clp->dispp, (clp + 1)->dispp);
		} else {
			printf("<unconverted> kanji: ");
			wputstr(clp->dispp, (clp + 1)->dispp);
		}
		putchar('\n');
		clp++;
	}
	printf("cl[%d]: %c (last)\n", i, clp->ltop ? '*' : ' ');
	/* consistency check */
	if (clp->conv || clp->kanap != buf->kanaEnd || !clp->ltop) {
		printf("inconsistency found in last cinfo! - ");
		if (clp->conv) printf("<conv flag> ");
		if (clp->kanap != buf->kanaEnd) printf("<kanap> ");
		if (!clp->ltop) printf("<ltop flag>");
		putchar('\n');
	}
}

void
jcPrintCandBuf(buf)
jcConvBuf *buf;
{
	int	i;

	if (buf->candClause < 0) {
		printf("not ready\n");
		return;
	}

	if (buf->candKind == CAND_SMALL) {
		jcSCand	*scp;

		printf("<SMALL> CandClause: %d NofCandidates: %d CurCand: %d\n",
		       buf->candClause, buf->nCand, buf->curCand);
		scp = (jcSCand *)buf->candBuf;
		for (i = 0; i < buf->nCand; i++) {
			printf("%ccand[%d]%c: ",
			       i == buf->curCand ? '+' : ' ', i,
			       scp->status & IMA_BIT ? '*' : ' ');
			printf("dic=%d,entry=%d,hinshi=%d kanji: ",
			       scp->dicno, scp->entry, scp->hinshi);
			if (scp->status & CONNECT_PREV) putchar('<');
			wputstr(scp->kanji, scp->kanji + wstrlen(scp->kanji));
			if (scp->status & CONNECT_NEXT) putchar('>');
			putchar('\n');
			scp++;
		}
	} else {
		jcLCand	*lcp;
		jcSCand	*scp;
		int	j;

		printf("<LARGE> CandClause: %d-%d NofCandidates: %d CurCand: %d\n",
		       buf->candClause, buf->candClauseEnd,
		       buf->nCand, buf->curCand);
		lcp = (jcLCand *)buf->candBuf;
		for (i = 0; i < buf->nCand; i++) {
			printf("%ccand[%d](%d): ",
			       i == buf->curCand ? '+' : ' ', i, lcp->nscand);
			scp = lcp->scand;
			for (j = lcp->nscand; j > 0; j--) {
				wputstr(scp->kanji,
					scp->kanji + wstrlen(scp->kanji));
				if (j > 1) putchar('-');
				scp++;
			}
			putchar('\n');
			lcp++;
		}
	}
}
#endif
