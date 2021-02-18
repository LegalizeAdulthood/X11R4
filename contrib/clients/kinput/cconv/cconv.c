/*
 *	cconv.c -- $BF~NOJ8;zJQ49%i%$%V%i%j(J (for X11)
 *		ver 8.0
 */

/*
 * Copyright (C) 1988  Software Research Associates, Inc.
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
 * $B35MW(J
 *
 * $BF~NOJ8;zJQ49$O!"$+$J4A;zJQ49$NA0CJ3,$G$"$j!"$=$NL\E*$O%-!<%\!<%I$+$i(J
 * $BF~NO$5$l$?%-!<%3!<%I$r2r<a$9$k$3$H$G$"$k!#(J
 * $B=>$C$F!"Bg$-$/J,$1$k$H<!$N(J2$B$D$N5!G=$r;}$D$3$H$K$J$k!#(J
 *
 * 1. $B%m!<%^;z$+$JJQ49$J$IF~NO$5$l$?J8;z$rJL$NJ8;z$KJQ$($k5!G=(J
 * 2. $B%-!<$NF~NO$K$h$j!"$+$J4A;zJQ49$G;H$o$l$k!VJQ49!W!"!V3NDj!W$J$I$N(J
 *    $B3F<o5!G=$r8F$S=P$95!G=(J
 *
 * $B$3$N(J cconv $B%i%$%V%i%j$O(J X Window version 11 $B$N$b$H$G;H$o$l$k$3$H$r(J
 * $BA[Dj$7$F$$$k!#(J
 */

/*
 * $B%a%b(J
 *
 * version 6.0	88/06/05
 *	$B$@$$$?$$$G$-$?(J
 * version 6.1	88/06/06
 *	$B%3%a%s%HF~$l(J
 * version 6.2	88/06/07
 *	$B$3$l$@$1F~$l$l$P%^%K%e%"%k=q$+$J$/$F$bBg>fIW$@$m$&(J
 * version 6.4	88/09/05
 *	$B2CF#;a(J@$BEl9)Bg$N0U8+$K$h$j(J redo $B5!G=$rF~$l$k(J
 *	$B$D$$$G$K(J static function $B$N@k8@$r$D$1$k(J
 *	$B$$$/$D$+$N4X?t$r(J external $B$+$i(J static $B$KJQ$($k(J
 * version 6.5	88/09/07
 *	function $B$,J#?t=q$1$k$h$&$K$7$?(J
 *	$B$=$N$?$a%G!<%?9=B$$rJQ99$9$k(J
 * version 6.6	88/10/07
 *	$B$J$s$H(J readRuleFile() $B$G%*!<%W%s$7$?%U%!%$%k$r(J
 *	$BJD$8$k$N$rK:$l$F$$$?(J -- $B$"!<$O$:$+$7$$(J
 *	$B=>$C$F(J wterm $B$G2?2s$b(J KanjiConvert $B$N(J on/off $B$r7+$jJV$9$H(J
 *	$B=i4|2=$K<:GT$9$k(J
 * version 6.7	88/12/19
 *	wstrlen() $B$O$3$N%i%$%V%i%j$GDj5A$9$k$Y$-$b$N$G$O$J$$$N$G30$9(J
 * version 6.8	89/07/21
 *	$B1F;3;a(J@$B>>2<EE4o$+$i%-!<%3!<%II=5-$,$&$^$/F0$+$J$$$H$NO"Mm$r<u$1$k(J
 *	getKey() $B$G(J XKEY $BI=5-$N>l9g!"(J#0x$B#1#6?J?t(J $B$NFI$_$H$j$K%P%0$,(J
 *	$B$"$k$3$H$,H=L@(J
 * version 7.0	89/08/16
 *	jclib $B$N(J Wnn version4 $BBP1~$K$h$j!"JQ49%P%C%U%!$,J#?t;}$F$k$h$&$K(J
 *	$B$J$C$?(J
 *	$B$=$l$K9g$o$;$F!"(Jcconv $B$b%P%C%U%!$,J#?t;}$F$k$h$&$K$7$J$1$l$P(J
 *	$B$J$i$J$$(J
 *	$B7k6IH>J,0J>e=q$-49$($F$7$^$C$?$h$&$@(J
 * version 7.1	89/08/17
 *	$B%P%0%U%#%C%/%9(J
 *	$B%b!<%IJQ99DLCNMQ$N%3!<%k%P%C%/$rDI2C$9$k(J
 * version 7.2	89/08/23
 *	$B%U%!%s%/%7%g%s%F!<%V%k$O!"$d$O$j(J ccParseRule() $B$G$O$J$/!"(J
 *	ccCreateBuf() $B$G;XDj$9$k$Y$-$b$N$J$N$G!"$=$N$h$&$KJQ99$9$k(J
 * version 7.3 89/08/25
 *	lint $B$G7Y9p$,=P$J$$$h$&$K=$@5(J
 *	dumpAllRules() $B$G$A$c$s$HJQ49%k!<%k$,%@%s%W$5$l$k$h$&$K=$@5(J
 * version 7.4 89/08/26
 *	ccParseRule() $B$G!"JQ49Dj5A%U%!%$%k$,A0$HF1$8$G$"$l$P(J
 *	$B%k!<%k$r6&MQ$9$k$h$&$KJQ99(J
 *	ccParseRule/ccFreeRule $B$G$N(J memory leak $B$N=$@5(J
 * version 7.5 89/09/26
 *	$BJQ49Dj5A%U%!%$%k$G(J include $B$,;H$($k$h$&$K=$@5(J
 * version 7.6 89/09/27
 *	include $B$N@bL@$r=q$/$N$rK:$l$F$$$?$N$G$=$l$rDI2C(J
 * version 7.7 89/10/26
 *	getKey() $B$G(J #$B?t;z(J $B7?$NI=5-$r$7$?;~$K$^$@%P%0$,$"$C$?$N$G=$@5(J
 *	$B$=$b$=$b$3$NJ}<0$NI=5-$G$O%^%9%/$,;XDj$G$-$J$$$H$$$&:,K\E*$J(J
 *	$BLdBj$,$"$k$,!"$^$"$3$l$O$$$$$3$H$K$7$F!#(J
 * version 8.0 89/10/27
 *	R4 contrib $B8~$1$N(J Release $B$K$"$?$C$F%P!<%8%g%sHV9f$r=$@5(J
 */

/*
 * $B;HMQK!(J
 *
 * $B;HMQK!$O3d9g4JC1$G$"$k!#:G=i$KJQ49%k!<%kDj5A%U%!%$%k$rFI$_9~$`$?$a$K(J
 * ccParseRule() $B$r%3!<%k$9$k!#(J
 * $B<!$KJQ49%P%C%U%!$r:n$k$?$a$K(J ccCreateBuf() $B$r8F$S!"%P%C%U%!$r:n$k!#(J
 * $B$3$N;~$K!";HMQ$9$k%k!<%k$d<o!9$N%3!<%k%P%C%/4X?t$r;XDj$9$k!#(J
 * $BJQ49%k!<%k$HJQ49%P%C%U%!$OJ#?t;}$D$3$H$,$G$-$k!#(J
 * $B$^$?!"A0$N%P!<%8%g%s$H$N8_49@-$N$?$a$K!"JQ49%k!<%k$NFI$_9~$_$H(J
 * $BJQ49%P%C%U%!$N:n@.$rF1;~$K9T$J$&!"(JccInit() $B$H$$$&4X?t$bMQ0U$5$l$F$$$k!#(J
 *
 * $B$"$H$O%-!<%W%l%9$N%$%Y%s%H$,Mh$?$i(J ccConvchar() $B$r8F$V$@$1$G$"$k!#(J
 * $B$3$N%U%!%s%/%7%g%s$,JQ49$r9T$J$$!"E,Ev$J%3!<%k%P%C%/%k!<%A%s$r8F$V!#(J
 *
 * $B8e$OI,MW$K1~$8$FMQ0U$5$l$F$$$k$=$NB>$N%U%!%s%/%7%g%s$r8F$Y$P$h$$!#(J
 */

/*
 * $BJQ49$N;EAH$_(J
 *
 * $B$3$NF~NOJ8;zJQ49$N;EAH$_$rM}2r$9$k$?$a$N%-!<%o!<%I$O<!$N(J3$B$D$G$"$k!#(J
 *	$B!&%b!<%I(J
 *	$B!&JQ49%k!<%k(J
 *	$B!&%3%s%F%-%9%H(J
 *
 * $B!&%b!<%I(J
 *	$BF~NOJ8;zJQ49$K$O!V%b!<%I!W$,$"$k!#$3$l$ONc$($P!"(J
 *	$B!V%m!<%^;z2>L>JQ49$r$7$F$+$J$rF~NO$9$k%b!<%I!W$H$+(J
 *	$B!V%"%k%U%!%Y%C%H$rF~NO$9$k%b!<%I!W$H$$$C$?$b$N$G$"$k!#(J
 *	$B%b!<%I$4$H$K0[$J$kJQ49%k!<%k$,@_Dj$G$-$k!#EvA3%b!<%I$N@ZBX$($b(J
 *	$BJQ49%k!<%k$H$7$F5-=R$G$-$k!#(J
 * 
 * $B!&JQ49%k!<%k(J
 *	$BJQ49%k!<%k$O(J4$B$D$N9`L\$+$i9=@.$5$l$k!#(J
 *		$BF~NO%-!<(J
 *		$B%3%s%F%-%9%HJ8;zNs(J
 *		$BJQ497k2LJ8;zNs(J
 *		$B%U%!%s%/%7%g%s(J
 *	$B$3$N$&$A!"F~NO%-!<$H%3%s%F%-%9%HJ8;zNs$,%k!<%k$N%^%C%A%s%0$K;H$o$l$k!#(J
 * 
 *	$BF~NO%-!<$O%-!<%3!<%I$r;XDj$9$k$b$N$G!"(JX $B$N%-!<%$%Y%s%H%3!<%I$G(J
 *	$B;XDj$9$kJ}K!$H!"$=$l$r(J XLookupString $B$7$?7k2L$N%-%c%i%/%?%3!<%I$G(J
 *	$B;XDj$9$kJ}K!$H$N(J2$BDL$j$,$"$k!#(J
 *
 *	$B%3%s%F%-%9%HJ8;zNs$O!"$=$l$^$G$K$I$N$h$&$JJ8;z$,F~NO$5$l$?$+$r(J
 *	$B;XDj$9$k$b$N$G$"$k!#(J
 *
 *	$BJQ497k2LJ8;zNs$O$=$NL>$NDL$j!"JQ497k2L$N;XDj$G!"%^%C%A$7$?(J
 *	$B%3%s%F%-%9%H$,$3$NJQ497k2L$KCV$-$+$o$k!#(J
 *
 *	$B%U%!%s%/%7%g%s$O%k!<%k$,%^%C%A$7$?$H$-$K<B9T$5$l$k%U%!%s%/%7%g%s(J 
 *	("$BJQ49(J" $B$H$+(J "$B3NDj(J" $B$H$+(J) $B$N;XDj$G$"$k!#%U%!%s%/%7%g%s$OJ#?t(J
 *	$B;XDj$9$k$3$H$b$G$-$k!#(J
 * 
 *	$BNc$r$"$2$k$H!"(J
 *	$B!V(J"ky" $B$HF~$C$F$$$k$H$3$m$G(J 'a' $B$H$$$&%-!<$,2!$5$l$?$i(J
 *	"$B$-$c$C(J!" $B$HJQ49$7!"%Y%k$rLD$i$9!W(J
 *	$B$H$$$&%k!<%k(J ($B$"$s$^$j0UL#$J$$$1$I(J) $B$G!"(J
 *		'a' $B$,F~NO%-!<!"(J
 *		"ky" $B$,%3%s%F%-%9%H!"(J
 *		"$B$-$c$C(J!" $B$,JQ497k2L!"(J
 *		<$B%Y%k$rLD$i$9(J> $B$,%U%!%s%/%7%g%s(J
 *	$B$H$$$&$3$H$K$J$k!#(J
 *
 * $B!&%3%s%F%-%9%H(J
 *	$B0J>e$N$h$&$K!"%3%s%F%-%9%H$H$$$&$N$O$=$l$^$G$K$I$N$h$&$JJ8;z$,(J
 *	$BF~NO$5$l$F$$$J$1$l$P$J$i$J$$$+$r;XDj$9$k$b$N$G$"$k!#(J
 *	$B$=$l$HF1;~$K$=$NJ8;z$rJQ497k2L$HCV$-JQ$($k$3$H$r;XDj$9$k$b$N(J
 *	$B$G$b$"$k!#(J
 *
 *	$B%3%s%F%-%9%H$O6uJ8;zNs$G$b$h$$!#$=$N$H$-$K$O$=$l$^$G$K$I$N$h$&$J(J
 *	$BJ8;z$,F~NO$5$l$?$+$K$+$+$o$i$:!"%^%C%A%s%0$,5/$3$k!#(J
 *
 *	$B%3%s%F%-%9%H$OH>3Q$N>l9gBgJ8;z>.J8;z$r6hJL$7$J$$!#(J
 *	$B$I$A$i$K$b%^%C%A$9$k!#(J
 *
 * $B!&JQ49$N<B:](J
 *	$B$5$F!"$I$N$h$&$K$7$FJQ49$,9T$J$o$l$k$N$+$r4JC1$K=q$$$F$_$k!#(J
 *
 *	$B%-!<%W%l%9%$%Y%s%H$r0z?t$K$7$F(J ccConvchar() $B$,8F$P$l$k!#(J
 *	$B$9$k$H!"$^$:$O%$%Y%s%H%3!<%I$G%+%l%s%H%b!<%I$GDj5A$5$l$F$$$k(J
 *	$BJQ49%k!<%k$NCf$+$i%^%C%A$9$k$b$N$rC5$9!#C5:w$O%k!<%k$N5-=R=g$K(J
 *	$B9T$J$o$l!":G=i$K%^%C%A$7$?$b$N$,A*$P$l$k!#(J
 *	$B$b$7%^%C%A$9$k%k!<%k$,$J$1$l$P!":#EY$O%$%Y%s%H%3!<%I$r(J
 *	XLookupString() $B$GJQ49$7$?%-%c%i%/%?%3!<%I$G!"%^%C%A$9$k$b$N$rC5$9!#(J
 *
 *	$B$=$l$G$b%^%C%A$9$k%k!<%k$,8+$D$+$i$J$+$C$?$H$-$K$O!"%G%U%)%k%H(J
 *	$B%"%/%7%g%s$N%3!<%k%P%C%/%k!<%A%s$,8F$P$l$k!#(J
 *
 *	$B%^%C%A$9$k%k!<%k$,8+$D$+$l$P!"JQ497k2LJ8;zNs$X$NCV$-49$($r9T$J$&!#(J
 *	$B$D$^$j!"$^$:%k!<%k$K5-=R$5$l$F$$$k%3%s%F%-%9%HJ8;zNs$NJ8;z?tJ,$@$1(J
 *	1$BJ8;z:o=|MQ$N%3!<%k%P%C%/%k!<%A%s$,8F$P$l!"$=$N8eJQ497k2LJ8;zNs$,(J
 *	1$BJ8;zF~NOMQ$N%3!<%k%P%C%/%k!<%A%s$r8F$s$GF~NO$5$l$k!#(J
 *	$B$=$N%k!<%k$K%U%!%s%/%7%g%s$,;XDj$5$l$F$$$l$P%G%#%9%Q%C%AMQ$N(J
 *	$B%3!<%k%P%C%/%k!<%A%s$,8F$S=P$5$l$k!#(J
 *
 *	$B%U%!%s%/%7%g%s$,B>$N%b!<%I$X$N@ZBX$($@$C$?>l9g$K$O>/$7J#;($G$"$k!#(J
 *	$B$=$N$H$-$K$O!"$^$:(J EXITMODE $B$H$$$&5?;w%-!<$r0z?t$K$7$F(J ccConvchar()
 *	$B$,8F$P$l$k!#$3$l$O$"$k%b!<%I$+$iH4$1$k:]$K2?$+=hM}$r$7$?$$!"(J
 *	$B$H$$$&$H$-$N$?$a$K$"$k!#$=$N8e%+%l%s%H%b!<%I$r?7$7$$%b!<%I$K$7$F!"(J
 *	$B:#EY$O(J ENTERMODE $B$H$$$&5?;w%-!<$r0z?t$K$7$F(J ccConvchar $B$,8F$P$l$k!#(J
 *	$B$3$l$K$h$C$F$"$k%b!<%I$KF~$C$?$H$-$N=hM}$r$9$k$3$H$,$G$-$k!#(J
 *	$B$=$N8e!"%3%s%F%-%9%H$,%/%j%"$5$l!":G8e$K!"%b!<%IJQ99DLCNMQ$N(J
 *	$B%3!<%k%P%C%/4X?t$,;XDj$5$l$F$$$l$P$=$l$,8F$P$l$k!#(J
 */

/*
 * ccdef $B%U%!%$%k$N=q$-J}(J
 *
 * $BF~NOJ8;zJQ49Dj5A%U%!%$%k(J (ccdef $B%U%!%$%k$H>JN,$9$k(J) $B$N=q$-J}$r4JC1$K5-$9!#(J
 *
 * ccdef $B%U%!%$%k$O<!$N(J3$B$D$N%Q!<%H$+$i9=@.$5$l$k!#(J
 * $B$3$l$i$O$3$N=g$KJB$s$G$$$J$1$l$P$J$i$J$$!#(J
 *
 *	<$B%b!<%I@k8@(J>
 *	<$B=i4|%b!<%I@k8@(J>
 *	<$B3F%b!<%I$NJQ49%k!<%k5-=R(J>
 *
 * <$B%b!<%I@k8@(J> $B$O;HMQ$9$k%b!<%IL>$r@k8@$9$k$b$N$G!"%U%)!<%^%C%H$O<!$NDL$j!#(J
 *	defmode Mode1 Mode2 Mode3...
 * $B;HMQ$9$k%b!<%I$O$9$Y$F$3$3$G@k8@$7$F$*$+$J$/$F$O$J$i$J$$!#(J
 *
 * <$B=i4|%b!<%I@k8@(J> $B$O(J cconv $B$,=i4|2=$5$l$?$H$-$N%b!<%I$r;XDj$9$k$b$N$G$"$k!#(J
 * $B%U%)!<%^%C%H$O<!$NDL$j!#(J
 *	initialmode Mode3
 *
 *
 * <$B3F%b!<%I$NJQ49%k!<%k5-=R(J> $B$,<B:]$NJQ49%k!<%k$r5-=R$9$kItJ,$G$"$k!#(J
 * 1$B$D$N%b!<%I$KBP$9$k5-=R7A<0$O<!$NDL$j!#(J
 *
 *	mode <$B%b!<%IL>(J> "<$B%W%m%s%W%H(J>" [fallthrough <$B%b!<%IL>(J>]
 *		"<$B%3%s%F%-%9%H(J>" <$B%-!<(J> "<$BJQ497k2L(J>" [<$B%U%!%s%/%7%g%s(J>...]
 *			:
 *			:
 *	endmode
 *
 * <$B%b!<%IL>(J> $B$O(J <$B%b!<%I@k8@(J> $B$G@k8@$7$?%b!<%IL>$G$"$k!#(J
 *
 * <$B%W%m%s%W%H(J> $B$O%b!<%I$rI=$9J8;zNs$G$"$k!#$3$l$O%+%l%s%H%b!<%I$rI=<($9$k(J
 * $B:]$K;HMQ$5$l$k$b$N$G!"4A;z$G$b$h$$!#D9$5$O<+M3$G$"$k$,!"M>$jD9$$$H(J
 * $B$9$Y$FI=<($5$l$k$+$I$&$+5?Ld$G$"$k!#(J
 * $B$=$N8e$N!"(J[ ] $B$K$/$k$^$l$?ItJ,$O>JN,2DG=$J$3$H$r<($9!#(J
 * $B$b$7$"$l$P!"$3$N%b!<%I$G%^%C%A$9$k%k!<%k$,$J$+$C$?$H$-$K$O(J fallthrough $B$G(J
 * $B;XDj$5$l$k%b!<%I$N%k!<%k$,0z$-B3$$$FC5$5$l$k!#(J
 *
 * mode $B$H(J endmode $B$K$O$5$^$l$?ItJ,$,8D!9$NJQ49%k!<%k$G$"$k!#(J
 * <$B%3%s%F%-%9%H(J> $B$,%3%s%F%-%9%HJ8;zNs$G$"$k!#$3$3$G$O(J '^' $B$H(J '\' $B$,FC<l(J
 * $B%-%c%i%/%?$H$7$F;HMQ$G$-$k!#(J"^C" $B$H=q$1$P$=$l$O(J $B%3%s%H%m!<%k(J-C $B$rI=$9!#(J
 * $B%P%C%/%9%i%C%7%e(J '\' $B$O<!$NJ8;z$NFC<lJ8;z$H$7$F$N0UL#$r$J$/$9!#(J
 * '^' $B<+?H$r;H$$$?$1$l$P(J "\^" $B$H%(%9%1!<%W$9$k!#F1MM$K(J '\' $B<+?H$r;H$$$?$1$l$P(J
 * "\\" $B$H=E$M$l$P$h$$!#(J
 *
 * <$B%-!<(J> $B$OF~NO$5$l$?%-!<$G!"(JX $B$N%-!<%$%Y%s%H!"%-%c%i%/%?%3!<%I!"(J
 * $B%a%?%-%c%i%/%?!"5?;w%-!<$N$$$:$l$+$G;XDj$9$k!#(J
 *
 * X $B$N%-!<%$%Y%s%H$O!"4pK\E*$K!"(J
 *	$B%b%G%#%U%!%$%"(J-Keysym$BL>(J
 * $B$GI=$9!#Nc$($P!"(J
 *	Tab
 *	control-a
 *	shift-control-space
 * $B$J$I$G$"$k!#(J
 *
 * $B%-%c%i%/%?%3!<%I$r=q$/$K$O!"%7%s%0%k%/%)!<%H$r;HMQ$9$k!#Nc$($P(J
 *	'a'
 *	'^['
 *	'\''
 * $B$J$I$G$"$k!#$o$+$k$H;W$&$,!"(J2$BHVL\$O(J ESCAPE$B!"(J3$BHVL\$O%7%s%0%k%/%)!<%H(J
 * $B$=$N$b$N$r<($9!#(J
 *
 * $B%a%?%-%c%i%/%?$O<!$N(J10$B<oN`$,;HMQ$G$-$k!#(J
 *  $B%-!<%$%Y%s%H$G$b%-%c%i%/%?%3!<%I$G$b%^%C%A$9$k$b$N(J
 *	@any		- $B2?$K$G$b%^%C%A$9$k(J
 *  $B%-!<%$%Y%s%H$K%^%C%A$9$k$b$N(J
 *	@raw		- $B%-!<%$%Y%s%H$J$i2?$K$G$b%^%C%A(J
 *	@func		- $B%U%!%s%/%7%g%s%-!<$K%^%C%A(J
 *	@cursor		- $B%+!<%=%k%-!<$K%^%C%A(J
 *	@keypad		- $B%F%s%-!<%Q%C%I$N%-!<$K%^%C%A(J
 *	@modifier	- $B%b%G%#%U%!%$%"(J (shift, control $B$J$I(J) $B$K%^%C%A(J
 *	@non-ascii	- $B%-!<%$%Y%s%H$N$&$A!"(JXLookupString() $B$N7k2L$,(J
 *			  $BD9$5(J0$B!"$D$^$jBP1~$9$k%"%9%-!<J8;z$,$J$$$b$N$K%^%C%A(J
 *  $B%-%c%i%/%?%3!<%I$K%^%C%A$9$k$b$N(J
 *	@ascii		- $B%-%c%i%/%?%3!<%I$J$i2?$K$G$b%^%C%A(J
 *	@printable	- $B0u;z2DG=J8;z(J (0x20 $B!e(J c $B!e(J 0x7e) $B$K%^%C%A(J
 *	@control	- $B%3%s%H%m!<%kJ8;z(J (0x00 $B!e(J c $B!e(J 0x1f) $B$K%^%C%A(J
 *
 * $B5?;w%-!<$OK\Ev$N%-!<F~NO$G$O$J$$!#$3$l$K$O(J2$B<oN`$"$k!#(J
 *	ENTERMODE	- $B$"$k%b!<%I$KF~$C$?$H$-$K2>A[E*$KF~NO$5$l$k%-!<(J
 *	EXITMODE	- $B$"$k%b!<%I$+$iH4$1$k$H$-$K2>A[E*$KF~NO$5$l$k%-!<(J
 *
 * <$BJQ497k2L(J> $B$OJQ497k2L$r;XDj$9$k!#$3$3$G$O(J '&'$B!"(J'/'$B!"(J'^'$B!"(J'\' $B$N(J4$B<oN`$N(J
 * $BFC<lJ8;z$,;HMQ$G$-$k!#(J'^' $B$H(J '\' $B$K$D$$$F$O(J <$B%3%s%F%-%9%H(J> $B$HF1$8$G$"$k!#(J
 * '&' $B$O(J $B%^%C%A$7$?(J <$B%-!<(J> $B$HCV$-49$($i$l$k!#(J<$B%-!<(J> $B$,%$%Y%s%H$N>l9g$K$O(J
 * $B$=$N%$%Y%s%H$r(J XLookupString() $B$7$?7k2L$NJ8;zNs$GCV$-49$o$k!#(J
 * '/' $B$,=P$F$/$k$H!"$=$3$G%3%s%F%-%9%H$O%/%j%"$5$l$k!#(J
 *
 * <$B%U%!%s%/%7%g%s(J> $B$O$J$/$F$b$h$$!#$"$l$PJQ49$N8e!"%U%!%s%/%7%g%s$,(J
 * $B<B9T$5$l$k!#%U%!%s%/%7%g%s$rJ#?t;XDj$9$k$3$H$b$G$-!"$=$N>l9g$K$O(J
 * $B;XDj$5$l$?=g=x$G<B9T$5$l$k!#%U%!%s%/%7%g%s$N6h@Z$j$O6uGrJ8;z$G$"$k!#(J
 *	func1 func2 func3
 * $B$N$h$&$K;XDj$9$k!#(J
 * $B%U%!%s%/%7%g%s$H$7$F%+%l%s%H%b!<%I$NJQ99$r;XDj$9$k$3$H$b$G$-$k!#(J
 *	goto <$B%b!<%IL>(J>
 * $B$G%+%l%s%H%b!<%I$,JQ$o$k!#$3$N%b!<%IL>$H$7$F(J "PREV" $B$,;HMQ$G$-$k!#(J
 * $B$3$l$O0l$DA0$N%b!<%I$rI=$9!#(J
 * $B$^$?!":FJQ49$r;XDj$9$k$3$H$b$G$-$k!#(J<$B%U%!%s%/%7%g%s(J>$B$H$7$F(J
 *	redo
 * $B$H=q$/$H!"$b$&0lEY%^%C%A$9$k%k!<%k$r:G=i$+$iC5$7$K$$$/!#$?$@$7$=$NA0$K(J
 * <$B%3%s%F%-%9%H(J> $B$,(J <$BJQ497k2L(J> $B$KCV$-49$($i$l$F$$$k$3$H$KCm0U!#$3$N5!G=$r(J
 * $B;H$&;~$K$OL58B%k!<%W$K4Y$i$J$$$h$&$KCm0U$7$F%k!<%k$r=q$+$J$1$l$P$J$i$J$$!#(J
 * $B0l1~0BA4:v$H$7$F(J redo $B$,(J MAXREDO (=20) $B2s8F$P$l$k$H<:GT$9$k$h$&$K(J
 * $B$J$C$F$$$k!#(J
 * $BCm0U(J: goto $B$H(J redo $B$N$"$H$K;XDj$5$l$?%U%!%s%/%7%g%s$O<B9T$5$l$J$$!#(J
 * $B$D$^$j!"(J
 *	func1 goto XX func2
 * $B$@$H!":G=i$K(J func1 $B$,<B9T$5$l!"<!$K%+%l%s%H%b!<%I$,(J XX $B$KJQ99$5$l$k$,(J
 * func2 $B$O<B9T$5$l$J$$!#(J
 *
 * $B:G8e$K=EMW$JCm0U$r0l$D!#(Jccdef $B%U%!%$%k$O(J EUC $B%3!<%I$G=q$+$l$F$$$J$1$l$P(J
 * $B$J$i$J$$!#(J
 *
 * $B$J$*!"=q$-K:$l$?$,(J '#' $B$G;O$^$k9T$O%3%a%s%H9T$G$"$k!#(J
 *
 * $B$^$?!"B>$N%U%!%$%k$r(J include $BJ8$r;H$C$F%$%s%/%k!<%I$9$k$3$H$,$G$-$k!#(J
 * $B=q<0$O(J
 *	include	$B%U%!%$%kL>(J
 * $B$G$"$k!#%U%!%$%kL>Cf$K6uGrJ8;z$,4^$^$l$k;~$K$O(J ' $B$+(J " $B$G0O$a$P$h$$!#(J
 * $B%U%!%$%kL>$,AjBP%Q%9L>$N;~$K$O$^$:%+%l%s%H%G%#%l%/%H%j$,C5$5$l!"$J$1$l$P(J
 * CCDEFPATH (/usr/lib/X11/ccdef/) $B$N2<$,C5$5$l$k!#(J
 */

/*
 * $B%$%s%?!<%U%'%$%9!&%U%!%s%/%7%g%s(J 
 *
 * - $BJQ49%k!<%kFI$_9~$_(J -
 *
 * ccRule ccParseRule(char *deffile, void (*errprint)())
 *	$BF~NOJ8;zJQ49%U%!%$%k$rFI$_9~$`!#(J
 *	deffile $B$G;XDj$5$l$k%U%!%$%k$+$iJQ49%k!<%k$rFI$_9~$`!#$b$7$b(J
 *	deffile $B$,(J NULL $B$N;~$O!"4D6-JQ?t(J CC_DEF $B$NCM$,;HMQ$5$l$k!#(J
 *	deffile $B$,AjBP%Q%9L>$N;~$K$O$^$:%+%l%s%H%G%#%l%/%H%j$N2<$,C5$5$l!"(J
 *	$B$J$1$l$P(J CCDEFPATH (/usr/lib/X11/ccdef) $B$N2<$,C5$5$l$k!#(J
 *	$BF1$8JQ49%U%!%$%k$KBP$7$FJ#?t2s(J ccParseRule() $B$r9T$J$C$F$b!"(J
 *	$B<B:]$KFI$_9~$^$l$k$N$O:G=i$N0l2s$@$1$G$"$k!#(J
 *	errprint $B$O%(%i!<I=<(MQ$N%U%!%s%/%7%g%s$G$"$k$,!"(J NULL $B$N>l9g$K$O(J
 *	stderr $B$X(J fprintf() $B$r;HMQ$7$F=PNO$5$l$k!#(J
 *	$B@.8y;~$K$OJQ49%k!<%k$r!"<:GT;~$K$O(J NULL $B$rJV$9!#(J
 *
 * void ccFreeRule(ccRule rule)
 *	$B;HMQ$7$J$/$J$C$?JQ49%k!<%k$G;H$o$l$F$$$?NN0h$r2rJ|$9$k!#(J
 *
 *
 * - $BJQ49%P%C%U%!:n@.(J -
 *
 * ccBuf ccCreateBuf(ccRule rule, int contextsize,
 *		     char *functable[], int functablesize,
 *		     void (*default_action)(), void (*insert_char)(),
 *		     void (*delete_char)(), void (*function_dispatch)(),
 *		     void (*mode_notify)(), caddr_t client_data);
 *	$BJQ49%P%C%U%!$r:n$j!"$=$l$rJV$9!#(J
 *	rule $B$G!"$I$NJQ49%k!<%k$r;HMQ$9$k$+;XDj$9$k!#(J
 *	contextsize $B$K$O%3%s%F%-%9%H$r2?J8;zJ];}$9$k$+$r;XDj$9$k!#(J
 *	$B$h$[$IFC<l$J%k!<%k$,$J$$8B$j!"IaDL$O?tJ8;z$GB-$j$k$O$:$G$"$k!#(J
 *	functable $B$O%U%!%s%/%7%g%sL>$N0lMwI=!"(Jfunctablesize $B$O$=$N%5%$%:(J
 *	$B$G$"$k!#(J
 *	default_action $B$O!"F~NO$5$l$?%-!<$,$I$N%k!<%k$K$b%^%C%A$7$J$+$C$?(J
 *	$B$H$-$K8F$P$l$k%U%!%s%/%7%g%s$G$"$k!#(JNULL $B$r;XDj$9$k$H!"%^%C%A(J
 *	$B$7$J$+$C$?$H$-$K$O2?$b5/$3$i$J$$!#(J
 *	insert_char, delete_char $B$O$=$l$>$l(J 1$BJ8;zA^F~(J / 1$BJ8;z:o=|MQ$N(J
 *	$B%U%!%s%/%7%g%s$G$"$k!#(J
 *	function_dispatch $B$K$O%U%!%s%/%7%g%s%3!<%k$N$?$a$N%G%#%9%Q%C%A(J
 *	$B%k!<%A%s$r;XDj$9$k!#(J
 *	mode_notify $B$O!"%b!<%I$,JQ$o$C$?;~$K8F$S=P$5$l$k%U%!%s%/%7%g%s$G$"$k!#(J
 *	$B%3!<%k%P%C%/$NI,MW$,$J$$$H$-$O(J NULL $B$r;XDj$7$F$*$1$P$h$$!#(J
 *	client_data $B$O!"%3!<%k%P%C%/;~$K0z?t$H$7$FEO$5$l$k%G!<%?$G$"$k!#(J
 *	$B@.8y;~$K$O%P%C%U%!$r!"%(%i!<$N;~$K$O(J NULL $B$,JV$5$l$k!#(J
 *
 * void ccDestroyBuf(ccBuf buf)
 *	$B;HMQ$7$J$/$J$C$?JQ49%P%C%U%!$G;H$o$l$F$$$?NN0h$r2rJ|$9$k!#(J
 *
 *
 * - $BJQ49(J -
 *
 * int ccConvchar(ccBuf buf, XKeyPressedEvent *event)
 *	X11 $B$N%-!<%\!<%I$N%W%l%9%$%Y%s%H$r<u$1<h$j!"JQ49$r9T$J$&!#(J
 *	$B9T$J$C$?7k2L!"J8;z$NA^F~!&:o=|$O(J ccCreateBuf() $B$G;XDj$5$l$?(J
 *	insert_char, delete_char $B$,8F$S=P$5$l!"%U%!%s%/%7%g%s$K$D$$$F$O(J
 *	$BF1$8$/(J ccCreateBuf() $B$G;XDj$5$l$k(J function_dispatch $B$,%G%#%9%Q%C%A$N(J
 *	$B$?$a$K8F$P$l$k!#(J
 *	$B$I$N%k!<%k$K$b%^%C%A$7$J$1$l$P!"(Jdefault_action $B$,8F$P$l$k!#(J
 *
 *
 * - $B%+%l%s%H%b!<%I(J -
 *
 * int ccGetMode(ccBuf buf)
 *	$B%+%l%s%H%b!<%IHV9f$,JV$5$l$k!#(J
 *
 * wchar_t *ccGetModePrompt(ccBuf buf)
 *	$B%+%l%s%H%b!<%IL>$,(J EUC$B%W%m%;%9%3!<%I$GJV$5$l$k!#(J
 *
 *
 * - $B%P%C%U%!$N;HMQ$7$F$$$k%k!<%k(J -
 *
 * ccRule ccGetRule(ccBuf buf)
 *	$B;XDj$5$l$?JQ49%P%C%U%!$,;HMQ$7$F$$$kJQ49%k!<%k$rJV$9!#(J
 *
 *
 * - $B%3%s%F%-%9%H(J -
 *
 * $B%3%s%F%-%9%H$X$NJ8;z$NDI2C$d:o=|$O!"%k!<%k$K%^%C%A$7$?$H$-$K$O(J
 * $B<+F0E*$K9T$J$o$l$k!#$^$?!"%+%l%s%H%b!<%I$NJQ99$K$H$b$J$C$F(J
 * $B%3%s%F%-%9%H$O<+F0E*$K%/%j%"$5$l$k!#(J
 * $B=>$C$FIaDL$O%f!<%6$,$3$l$i$N%U%!%s%/%7%g%s$r8F$VI,MW$O$J$$!#(J
 * $B$?$@$7!"%^%C%A$7$J$+$C$?J8;z$r(J default_action $B$,A^F~$9$k$H$+!"(J
 * $B%P%C%/%9%Z!<%9$N=hM}$r9T$J$&!"$J$I$N>l9g$K$O%f!<%6$,(J explicit $B$K(J
 * $B8F$VI,MW$,$"$k!#(J
 *
 * void ccContextAppend(ccBuf buf, int c)
 *	$B%3%s%F%-%9%H$N:G8e$KJ8;z(J c (EUC $B%W%m%;%9%3!<%I(J)$B$rIU$12C$($k!#(J
 *
 * void ccContextDelete(ccBuf buf)
 *	$B%3%s%F%-%9%H$N:G8e$N(J1$BJ8;z$r:o=|$9$k!#(J
 *
 * void ccContextClear(ccBuf buf)
 *	$B%3%s%F%-%9%H$r%/%j%"$9$k!#(J
 *
 * void ccContextSet(ccBuf buf, wchar_t *cstr)
 *	$B%3%s%F%-%9%HJ8;zNs$r%;%C%H$9$k!#J8;zNs$O(J null $B%?!<%_%M!<%H$5$l$?(J
 *	EUC $B%W%m%;%9%3!<%I$G$J$1$l$P$J$i$J$$!#(J
 *	$BJ8;zNs$ND9$5$,(J ccCreateBuf() $B$G;XDj$5$l$?(J contextsize $B$h$j(J
 *	$BD9$$$H$-$K$O!":G8e$N(J contextsize $BJ8;z$,%;%C%H$5$l$k!#(J
 *
 * void ccContextGet(ccBuf buf, wchar_t *cstr)
 *	$B8=:_$N%3%s%F%-%9%HJ8;zNs$rJV$9!#J8;zNs$O(J null $B%?!<%_%M!<%H$5$l$?(J
 *	EUC $B%W%m%;%9%3!<%I$G$"$k!#(J
 */

/*
 * $B%3!<%k%P%C%/%U%!%s%/%7%g%s$H$=$N0z?t(J
 *
 *	$B%G%#%9%Q%C%A%U%!%s%/%7%g%s(J:
 *	    function_dispatch(int func, unsigned char *str, int nbytes,
 *			      caddr_t client_data)
 *		$B%^%C%A$7$?%k!<%k$K%U%!%s%/%7%g%s$,=q$+$l$F$$$?$H$-$K(J
 *		$B8F$S=P$5$l$k!#(J
 *		$B0z?t(J func $B$O!"%U%!%s%/%7%g%sHV9f$G$"$k!#$3$l$O(J
 *		ccParseRule() $B$N0z?t$H$7$FEO$5$l$?!"(Jfunctable[] $B$N(J
 *		$B%$%s%G%C%/%9$G$"$k!#(Jstr, nbytes $B$O$3$N(J
 *		$B%U%!%s%/%7%g%s$r0z$-5/$3$7$?%-!<$NI=$9J8;zNs!#(J
 *		client_data $B$O(J ccCreateBuf() $B$N;~$K;XDj$5$l$?%G!<%?$G$"$k!#(J
 *
 *	$B%G%U%)%k%H%"%/%7%g%sMQ%U%!%s%/%7%g%s(J:
 *	    default_action(unsigned char *str, int nbytes, caddr_t client_data)
 *		$B2!$5$l$?%-!<$KBP$7$F%^%C%A$9$k%k!<%k$,$J$+$C$?$H$-$K(J
 *		$B8F$S=P$5$l$k!#(J
 *		str, nbytes $B$O%-!<$NI=$9J8;zNs!#(J
 *		client_data $B$O(J ccCreateBuf() $B$N;~$K;XDj$5$l$?%G!<%?!#(J
 *
 *	$BJ8;zA^F~%U%!%s%/%7%g%s(J:
 *	    insert_char(wchar_t c, caddr_t client_data)
 *		$BJQ497k2L$NJ8;z$r%"%W%j%1!<%7%g%s$KEO$9$N$K;HMQ$5$l$k!#(J
 *		c $B$O(J EUC $B%W%m%;%9%3!<%I$G$"$k!#$3$N%U%!%s%/%7%g%s$O(J
 *		1$BJ8;z$4$H$K8F$P$l$k!#(J
 *		client_data $B$O(J ccCreateBuf() $B$N;~$K;XDj$5$l$?%G!<%?!#(J
 *
 *	$BJ8;z:o=|%U%!%s%/%7%g%s(J:
 *	    delete_char(caddr_t client_data)
 *		$BJQ497k2L$K$7$?$,$C$F:o=|$9$kJ8;z$,$"$l$P$=$NJ8;z?tJ,(J
 *		$B$3$N%U%!%s%/%7%g%s$,8F$P$l$k!#(J
 *		client_data $B$O(J ccCreateBuf() $B$N;~$K;XDj$5$l$?%G!<%?!#(J
 *
 *	$B%b!<%IJQ99%U%!%s%/%7%g%s(J:
 *	    mode_notify(int newmode, int oldmode, caddr_t client_data)
 *		$B%+%l%s%H%b!<%I$,JQ99$5$l$?;~$K$3$N%U%!%s%/%7%g%s$,(J
 *		$B8F$P$l$k!#(Jnewmode $B$,?7$7$$%+%l%s%H%b!<%I!"(Joldmode $B$,(J
 *		$BA0$N%+%l%s%H%b!<%IHV9f$G$"$k!#(J
 *
 *	$B%(%i!<I=<(%U%!%s%/%7%g%s(J:
 *	    error_handler(char *errstr)
 *		ccParseRule() $BCf$G!"%(%i!<$,H/@8$7$?;~$K8F$S=P$5$l$k!#(J
 */

#ifndef lint
static char	*rcsid = "$Id: cconv.c,v 8.0 89/10/27 20:23:48 ishisone Rel $";
#endif

#include	<stdio.h>
#include	<X11/Xlib.h>
#include	<X11/keysym.h>
#include	<X11/Xutil.h>
#include	<X11/Xos.h>
#include	"cconv.h"

#define uchar	unsigned char
#define ushort	unsigned short
#define ulong	unsigned long

#define Malloc(size)		malloc((unsigned int)(size))
#define Realloc(p, size)	realloc((char *)(p), (unsigned int)(size))
#define Free(p)			{ if (p) (void)free((char *)(p)); }

#define Strcmp(s1, s2)		strcmp((char *)(s1), (char *)(s2))
#define Strncmp(s1, s2, n)	strncmp((char *)(s1), (char *)(s2), n)
#define Strcpy(s1, s2)		strcpy((char *)(s1), (char *)(s2))
#define Strcat(s1, s2)		strcat((char *)(s1), (char *)(s2))
#define Strlen(s)		strlen((char *)(s))

/* $B%-!<%3!<%I$N%(%s%3!<%IJ}K!(J */
#define RAWKEY	(1L<<31)
#define PSEUDO	(1L<<30)
#define METAC	(1L<<29)
#define META_ASCII	1	/* any ascii character (not useful) */
#define META_CONTROL	2	/* any control character */
#define META_RAW	3	/* any key event (not useful) */
#define META_ANY	4	/* anything (character or event) */
#define META_FUNC	5	/* any function key event */
#define META_CURSOR	6	/* any cursor key event */
#define META_KEYPAD	7	/* any keypad key event */
#define META_MODIFIER	8	/* any modifier key event */
#define META_PRINTABLE	9	/* any printable character */
#define META_NONASCII	10	/* key event that has no ascii interpretation */

/* pseudo key code */
#define ENTERMODE	(ulong)(PSEUDO|1)
#define EXITMODE	(ulong)(PSEUDO|2)

/* function code */
#define ENDFUNC		0xffff
#define MODECHANGE	0x8000
#define REDO		0x7fff
#define PREVMODE	(MODECHANGE|0x1000)

#define MAXREDO		20

/* key encoding */
#define ccEncodeKey(ev) ((ulong)XLookupKeysym(ev, 0) | (ulong)RAWKEY)
#define ccEncodeMask(ev) ((int)((ev)->state & 0xff))

#define ccEncodeChar(c)	((ulong)(c))

#define NOMODE		0xffff

#define MATCHED_CHAR	0x8080	/* $B$3$l$O(J EUC $B$H$7$F$O(J illegal $B$J%3!<%I$J$N$G(J
				 * $BFCJL$JL\E*$KMQ$$$k(J */
#define CCLEAR_CHAR	0x8081	/* $B>e$KF1$8(J */

#define CANONIC(c)	(((c) >= 'A' && (c) <= 'Z') ? c += ('a' - 'A') : c)

/* $B%G!<%?9=B$(J */
/* $BJQ49%k!<%k(J */
typedef struct convdesc {
	ulong	key;		/* $BF~NO%-!<(J */
	ushort	mask;		/* modifier mask (of X KeyEvent) */
	ushort	context;	/* context$BJ8;zNs(J ($B5U=g$K3JG<$5$l$k(J) */
	ushort	result;		/* $BJQ497k2LJ8;zNs(J */
	ushort	function;	/* function $B%Y%/%?$N%$%s%G%C%/%9(J */
} ConvDesc;

/* $B%b!<%I$4$H$N%F!<%V%k(J */
typedef struct {
	char		*name;		/* $B%b!<%IL>(J */
	int		nrule;		/* $B%k!<%k$N?t(J */
	ConvDesc	*cdbuf;		/* $B%k!<%k(J */
	wchar_t		*prompt;	/* $B%W%m%s%W%HJ8;zNs(J */
	ushort		fallthrough;
} ModeTable;

/* ccParseRule() $B$GFbItI=8=$KJQ49$5$l$?%k!<%k(J */
typedef struct _ccRule {
	char		*rulefile;	/* $BDj5A%U%!%$%kL>(J */
	ccRule		next;		/* $B%k!<%k%j%9%H$N<!$NMWAG(J */
	int		refcnt;		/* $B;2>H?t(J */
	int		nmode;		/* $B%b!<%I$N?t(J */
	int		initialmode;	/* $B=i4|%b!<%I(J */
	ModeTable	*modes;		/* $B%b!<%I%F!<%V%k(J */
	wchar_t		*strbuf;	/* $B%9%H%j%s%0%P%C%U%!(J */
	ushort		*funcbuf;	/* $B%U%!%s%/%7%g%s%P%C%U%!(J */
	int		nfunc;		/* $B%U%!%s%/%7%g%s$N?t(J */
	char		*funcnamebuf;	/* $B%U%!%s%/%7%g%sL>$N%P%C%U%!(J */
	void		(*errorfunc)();	/* $B%(%i!<%3!<%k%P%C%/(J */
} ccRuleRec;

static ccRule	ccrules;	/* $BJQ49%k!<%k$N%j%9%H(J */

/* ccCreateBuf() $B$G:n$i$l$kJQ49%P%C%U%!(J */
typedef struct _ccBuf {
	ccRule	rule;			/* $BJQ49%k!<%k(J */
	short	*functbl;
	void	(*defaultaction)();	/* callback functions */
	void	(*insertchar)();
	void	(*deletechar)();
	void	(*executefunction)();
	void	(*modenotify)();
	caddr_t	client_data;
	int	currentmode;		/* $B8=:_$N%b!<%I(J */
	int	previousmode;		/* $B0l$DA0$N%b!<%I(J */
	wchar_t	*context;
	wchar_t	*contextend;
	int	contextsize;
} ccBufRec;

#define STRSIZE		200
#define MORESTRSIZE	100
typedef struct {
	wchar_t	*strbuf;
	wchar_t	*strend;
	wchar_t	*strp;
} _strbufRec;

#define	CDSIZE		10
#define	MORECDSIZE	30
typedef struct {
	ConvDesc	*cdbuf;		/* ConvDesc $B%"%l%$(J */
	ConvDesc	*cdend;
	ConvDesc	*cdp;
} _cdbufRec;

#define FUNCSIZE	20
#define MOREFUNCSIZE	20
typedef struct {
	ushort	*funcbuf;	/* $B%U%!%s%/%7%g%s%Y%/%?(J */
	ushort	*funcend;
	ushort	*funcp;
} _funcbufRec;

#define MAXFUNC			1024	/* $B%U%!%s%/%7%g%s$N<oN`$N>e8B(J */
#define FUNCNAMESIZE		100
#define MOREFUNCNAMESIZE	50
typedef struct {
	int	nfunc;
	ushort	funcnames[MAXFUNC];
	char	*funcnamebuf;
	char	*funcnameend;
	char	*funcnamep;
	int	funcsize;
} _funcnameRec;

/* $BDj5A%U%!%$%k(J (for include directive) */
#define MAXINC		10
typedef struct {
	int	index;
	FILE	*fp[MAXINC];
} Files;

extern char	*malloc();
extern char	*realloc();

#ifdef __STDC__
static int wstrlen(wchar_t *);
static wchar_t *wrev(wchar_t *);
static void eproc();
static wchar_t *promptsave(wchar_t *);
static int parseLine(uchar *, uchar **, int);
static FILE *openfile(char *);
static int doinclude(uchar *, Files *, void (*)());
static uchar *getline(uchar *, int, Files *, void (*)());
static int readRuleFile(ccRule, char *);
static int registMode(ccRule, int, uchar **);
static int newMode(ccRule, Files *, _strbufRec *, _funcbufRec *,
		   _funcnameRec *, int, uchar **);
static int getDesc(ccRule, uchar *, _funcbufRec *, _funcnameRec *,
		   ulong *, int *, wchar_t *, wchar_t *, int *);
static int getMode(ccRule, uchar *);
static uchar *getQuote(uchar *, wchar_t *, int);
static int getKey(uchar *, ulong *, int *);
static int getmask(uchar *);
static char *strinc(char *, char *);
static ulong getcode(uchar *);
static int getFunc(ccRule, _funcbufRec *, _funcnameRec *, int, uchar **);
static ccRule findRule(char *);
static void addRule(ccRule);
static void deleteRule(ccRule);
static int wstrsave(_strbufRec *, wchar_t *);
static int wstralloc(_strbufRec *, int);
static void wstradjust(_strbufRec *);
static ConvDesc *cdalloc(_cdbufRec *);
static void cdadjust(_cdbufRec *);
static int funcalloc(_funcbufRec *, int);
static void funcadjust(_funcbufRec *);
static int funcsearch(_funcnameRec *, char *);
static void funcnameadjust(_funcnameRec *);
static int convchar(ccBuf, ulong, int, char *, int);
static int cconvert(ccBuf, int, ulong, int, int *, char *, int);
static int metamatch(ulong, ulong, int);
static int contextMatch(ccBuf, wchar_t *);
static void substStr(ccBuf, wchar_t *, wchar_t *, char *, int);
#else
static int wstrlen();
static wchar_t *wrev();
static void eproc();
static wchar_t *promptsave();
static int parseLine();
static int readRuleFile();
static int registMode();
static int newMode();
static int getDesc();
static int getMode();
static uchar *getQuote();
static int getKey();
static int getmask();
static char *strinc();
static ulong getcode();
static int getFunc();
static int wstrsave();
static int wstralloc();
static void wstradjust();
static ConvDesc *cdalloc();
static void cdadjust();
static int funcalloc();
static void funcadjust();
static int funcsearch();
static void funcnameadjust();
static int convchar();
static int cconvert();
static int metamatch();
static int contextMatch();
static void substStr();
#endif

/*
 *	Private Functions
 */

static int wstrlen(str)
wchar_t *str;
{
	int	len = 0;

	while (*str++)
		len++;
	return len;
}

static wchar_t *wrev(s)
wchar_t *s;
{
	wchar_t	*str = s;
	wchar_t	*end = str;
	int	c;

	while (*end++)
		;
	end -= 2;
	while (str < end) {
		c = *str;
		*str++ = *end;
		*end-- = c;
	}
	return s;
}

/* VARARGS2 */
static void eproc(efunc, format, a1, a2, a3, a4, a5)
void (*efunc)();
char *format;
{
	char	str[1024];

	(void)sprintf(str, format, a1, a2, a3, a4, a5);

	if (efunc == NULL) {
		/* stderr $B$K%W%j%s%H(J */
		fprintf(stderr, "%s\n", str);
	} else {
		/* $B%(%i!<%O%s%I%i$r8F$V(J */
		(*efunc)(str);
	}
}

static wchar_t *promptsave(str)
wchar_t *str;
{
	int	len = (wstrlen(str) + 1) * sizeof(wchar_t);
	wchar_t	*p = (wchar_t *)Malloc(len);

	if (p != NULL) {
		wchar_t	*q = p;
		while (*q++ = *str++)
			;
	}
	return p;
}

static int parseLine(line, argv, argvsize)
uchar *line;
uchar **argv;
int argvsize;
{
	int	c, qc;
	int	argc;
	int	state;
#define IN_WORD		1	/* $B%o!<%I$NCf(J */
#define IN_QUOTE	2	/* $B%o!<%I$NCf$G$5$i$K%/%)!<%H$NCf(J */

	qc = 0;		/* not necessary, but for lint */
	argc = 0;
	state = 0;	/* $B%o!<%I$N30(J */
	while (c = *line) {
		/* $B%/%)!<%H$5$l$F$$$J$$6uGrJ8;z$O!"%o!<%I$r=*$i$;$k(J */
		if (state != IN_QUOTE && (c == ' ' || c == '\t' || c == '\n')) {
			/* NULL $B%?!<%_%M!<%H$5$;$k(J */
			*line++ = '\0';
			state = 0;
			continue;
		}
		/* $B%o!<%I$N30$N6uGr0J30$NJ8;z$O!"%o!<%I$N;O$^$j$K$J$k(J */
		if (state == 0) {
			if (argc >= argvsize)
				return argc;
			argv[argc++] = line;
			state = IN_WORD;
		}
		/* $B%P%C%/%9%i%C%7%e$O!"<!$NJ8;z$r%9%-%C%W$5$;$k(J */
		if (c == '\\') {
			/* $B$H$O$$$C$F$b<!$,(J Nul $BJ8;z$J$iOC$OJL(J */
			if (*++line == '\0') {
				*--line = '\0';
				break;
			}
		} else if (state == IN_QUOTE) {
			/* $B%/%)!<%H$O;O$^$j$HF1$8J8;z$G=*$k(J */
			if (c == qc)
				state = IN_WORD;
		} else if (c == '\'' || c == '"') {
			/* $B%/%)!<%H$N30$K%/%)!<%HJ8;z$,$"$l$P%/%)!<%H$N;O$^$j(J */
			state = IN_QUOTE;
			qc = c;
		}
		line++;
	}

	/* $B:G8e$N%/%)!<%H$,JD$8$F$$$J$$$+$b$7$l$J$$(J */
	return state != IN_QUOTE ? argc : argc - 1;
}

/* openfile -- $BDj5A%U%!%$%k$r%*!<%W%s$9$k(J */
static FILE *openfile(file)
char *file;
{
	FILE	*fp;
	char	filename[1024];
	FILE	*fopen();

	if ((fp = fopen(file, "r")) == NULL && *file != '/') {
		(void)Strcpy(filename, CCDEFPATH);
		(void)Strcat(filename, file);
		fp = fopen(filename, "r");
	}
	return fp;
}

/* doinclude -- include $B9T$N=hM}$r$9$k(J */
static int doinclude(line, files, efunc)
uchar *line;
Files *files;
void (*efunc)();
{
	int	argc;
	uchar	*argv[2];
	char	*name;

	argc = parseLine(line, argv, 2);
	if (files->index > MAXINC - 2) {
		eproc(efunc, "include nesting too deep");
		return -1;
	}
	if (argc < 2) {
		eproc(efunc, "missing include filename");
		return -1;
	}
	name = (char *)argv[1];
	if (*name == '\'' || *name == '"') {
		name++;
		name[strlen(name) - 1] = '\0';
	}
	if ((files->fp[++files->index] = openfile(name)) == NULL) {
		eproc(efunc, "can't open %s", name);
		return -1;
	}
	return 0;
}

/* getline -- 1$B9TFI$_9~$`(J ($B$=$N:](J include $B$N=hM}$r9T$J$&(J) */
static uchar *getline(line, linesize, files, efunc)
uchar *line;
int linesize;
Files *files;
void (*efunc)();
{
 redo:
	if (fgets((char *)line, linesize, files->fp[files->index])) {
		register uchar	*p = line;
		while (*p == ' ' || *p == '\t')
			p++;
		if (!Strncmp(p, "include", 7)) {
			if (doinclude(p, files, efunc) < 0) {
				return NULL;
			} else {
				goto redo;
			}
		}
		return line;
	} else if (files->index > 0) {
		(void)fclose(files->fp[files->index]);
		files->index--;
		goto redo;
	}
	(void)fclose(files->fp[files->index]);
	return NULL;
}

/* readRuleFile -- $BJQ49%k!<%kDj5A%U%!%$%k$rFI$_9~$`(J */
static int readRuleFile(rule, file)
ccRule rule;
char *file;
{
	FILE		*fp;
	int		moderegistered;
	uchar	line[256], tmp[256];
	uchar	*argv[20];
	int		argc;
	_strbufRec	strrec;
	_funcbufRec	funcrec;
	_funcnameRec	fnrec;
	Files		files;
	void		(*efunc)() = rule->errorfunc;

	if ((fp = openfile(file)) == NULL) {
		eproc(efunc, "can't open file %s", file);
		return -1;
	}
	files.index = 0;
	files.fp[0] = fp;

	moderegistered = 0;

	strrec.strbuf = NULL;
	funcrec.funcbuf = NULL;
	fnrec.nfunc = 0;
	fnrec.funcnamebuf = NULL;

	while (getline(line, sizeof(line), &files, efunc)) {
		(void)Strcpy(tmp, line);
		if ((argc = parseLine(tmp, argv, 20)) == 0)
			continue;

		/* '#' $B$G;O$^$k9T$O%3%a%s%H(J */
		if (*line == '\0' || *line == '\n' || *line == '#') {
			continue;
		} else if (!moderegistered && argc > 1 &&
			   !Strcmp(argv[0], "defmode")) {
			/* $B%b!<%IDj5A9T(J */
			if (registMode(rule, argc, argv) < 0) {
				(void)fclose(fp);
				return -1;
			}
			moderegistered++;
		} else if (!Strcmp(argv[0], "mode") && argc > 2) {
			/* $B$"$k%b!<%I$KBP$9$kJQ49Dj5A(J */
			if (!moderegistered) {
				eproc(efunc, "'mode' before 'defmode'");
				(void)fclose(fp);
				return -1;
			}
			if (newMode(rule, &files, &strrec, &funcrec, &fnrec,
				    argc, argv) < 0)
				return -1;
		} else if (!Strcmp(argv[0], "initialmode") &&
			   argc > 1) {
			if (!moderegistered) {
				eproc(efunc, "'initialmode' before 'defmode'");
				(void)fclose(fp);
				return -1;
			}
			rule->initialmode = getMode(rule, argv[1]);
		} else {
			eproc(efunc, "syntax error - %s", line);
		}
	}
	(void)fclose(fp);

	wstradjust(&strrec);
	funcadjust(&funcrec);
	funcnameadjust(&fnrec);

	rule->strbuf = strrec.strbuf;
	rule->funcbuf = funcrec.funcbuf;
	rule->funcnamebuf = fnrec.funcnamebuf;
	rule->nfunc = fnrec.nfunc;
#ifdef DEBUG
	dumpAllRules(rule);
#endif

	return 0;
}

static int registMode(rule, ac, av)
ccRule rule;
int ac;
uchar **av;
{
	int		nmode;
	ModeTable	*modes;
	int		i;

	ac--, av++;

	nmode = ac;
	modes = (ModeTable *)Malloc(nmode * sizeof(ModeTable));
	if (modes == NULL) {
		eproc(rule->errorfunc, "can't alloc memory");
		return -1;
	}
	rule->modes = modes;

	for (i = 0; i < nmode; i++) {
		if ((modes[i].name = Malloc(Strlen(av[i]) + 1)) == NULL) {
			eproc(rule->errorfunc, "can't alloc memory");
			Free(modes);
			return -1;
		}
		(void)Strcpy(modes[i].name, av[i]);
		modes[i].nrule = 0;
		modes[i].cdbuf = NULL;
		modes[i].prompt = NULL;
		modes[i].fallthrough = NOMODE;
	}

	rule->nmode = nmode;
	return 0;
}

/* newMode -- $B$"$k%b!<%I$K$D$$$F$N%k!<%k$rFI$_9~$`(J */
static int newMode(rule, files, srec, frec, fnrec, ac, av)
ccRule rule;
Files *files;
_strbufRec *srec;
_funcbufRec *frec;
_funcnameRec *fnrec;
int ac;
uchar **av;
{
	uchar	line[256];
	int		mode;
	ulong		inkey;
	int		modmask;
	wchar_t		prompt[30], context[100], result[100];
	int		func;
	int		ndesc = 0;
	ModeTable	*mp;
	ConvDesc	*cdp;
	_cdbufRec	cdbuf;
	void		(*efunc)() = rule->errorfunc;

	/* $B%U%)!<%^%C%H$O(J
	 * "mode <$B%b!<%IL>(J> <"$B%W%m%s%W%HJ8;zNs(J"> [fallthrough <$B%b!<%IL>(J>]
	 */
	/* $B%b!<%I$N%A%'%C%/(J */
	if ((mode = getMode(rule, av[1])) < 0) {
		eproc(efunc, "illegal modename: %s", av[1]);
		return -1;	/* No Such Mode */
	}
	mp = &rule->modes[mode];

	if (getQuote(av[2], prompt, 0) == NULL) {
		eproc(efunc, "illegal prompt: %s", av[2]);
		return -1;
	}
	mp->prompt = promptsave(prompt);
	mp->nrule = 0;
	
	if (ac > 4 && !Strcmp(av[3], "fallthrough")) {
		mp->fallthrough = getMode(rule, av[4]);
	} else {
		mp->fallthrough = NOMODE;
	}

	cdbuf.cdbuf = NULL;

	/* $B%k!<%k$rFI$s$G%9%H%"$9$k(J */
	while (getline(line, sizeof(line), files, efunc)) {
		/* '#' $B$G;O$^$k9T$O%3%a%s%H(J */
		if (*line == '\0' || *line == '\n' || *line == '#')
			continue;
		if (!Strncmp(line, "endmode", 6))
			break;
		if (getDesc(rule, line, frec, fnrec, &inkey, &modmask,
			    context, result, &func)) {
			if ((cdp = cdalloc(&cdbuf)) == NULL) {
				return -1;
			}

			/* $B%k!<%k$N%9%H%"(J */
			cdp->key = inkey;
			cdp->mask = modmask;
			cdp->context = *context ? wstrsave(srec, wrev(context)) : 0;
			cdp->result = *result ? wstrsave(srec, result) : 0;
			cdp->function = func;
			ndesc++;
		} else
			eproc(efunc, "illegal description - %s", line);
	}

	/* $B%k!<%k$,#1$D$b$J$1$l$P%(%i!<$K$9$Y$-$@$m$&(J */
	/* $B$H;W$C$?$,Nc$($P(J ASCII $B%b!<%I$N$H$-$K$O%k!<%k$,#1$D$b$J$$$3$H$,(J
	 * $B$"$j$&$k(J
	 */

	cdadjust(&cdbuf);

	mp->nrule = ndesc;
	mp->cdbuf = cdbuf.cdbuf;

	return 0;
}

static int getDesc(rule, line, frec, fnrec, keyp, maskp, context, result, funcp)
ccRule rule;
uchar *line;
_funcbufRec *frec;
_funcnameRec *fnrec;
ulong *keyp;
int *maskp;
wchar_t *context;
wchar_t *result;
int *funcp;
{
	uchar	tmp[256];
	uchar	*av[20];
	int		ac;
	void		(*efunc)() = rule->errorfunc;

	/* valid description format is:
		"context"	key	"result"	[function...]
	*/

	(void)Strcpy(tmp, line);
	ac = parseLine(tmp, av, 20);
	if (ac < 3) {
		eproc(efunc, "syntax error - %s", line);
		return 0;
	}

	/* context $B$NFI$_9~$_(J */
	if (getQuote(av[0], context, 0) == NULL)
		return 0;

	/* $B%-!<%3!<%I$rFI$_9~$s$G(J */
	if (getKey(av[1], keyp, maskp) < 0) {
		eproc(efunc, "no such key (%s)", av[1]);
		return 0;
	}

	/* result $B$rFI$_9~$s$G(J */
	if (getQuote(av[2], result, 1) == NULL)
		return 0;

	/* $B%U%!%s%/%7%g%s$N5-=R$,$"$l$P$=$l$rFI$_9~$`(J */
	/* $B$b$7AjEv$9$k%U%!%s%/%7%g%s$,$J$/$F$b%(%i!<$K$7$J$$(J */
	if (ac > 3) {
		*funcp =  getFunc(rule, frec, fnrec, ac - 3, &av[3]);
	} else {
		*funcp = 0;
	}

	return 1;
}

static int getMode(rule, str)
ccRule rule;
uchar *str;
{
	ModeTable	*modes = rule->modes;
	int	i;

	for (i = 0; i < rule->nmode; i++) {
		if (!Strcmp(str, modes[i].name))
			return i;
	}
	eproc(rule->errorfunc, "undefined mode %s", str);
	return -1;
}

/* getQuote -- $B%/%)!<%F!<%7%g%s5-9f$G0O$^$l$?J8;zNs$rFI$s$G(J wchar_t $B$K$9$k(J */
static uchar *getQuote(line, str, metaf)
uchar	*line;
wchar_t		*str;
int		metaf;	/* '&' $B$H(J '/' $B$r%a%?!&%-%c%i%/%?$H$9$k$+$I$&$+(J */
{
	int	c;
	int	quote;		/* quote flag */
	int	qc = *line++;	/* quote character */
#define SS2	0x8e
#define SS3	0x8f

	if (qc != '\'' && qc != '"')
		return((uchar *)NULL);
	
	quote = 0;
	while ((c = *line++) && c != qc) {
		if (c == '\\' && !quote) {
			quote = 1;
			continue;
		}
			
		if (c == '^' && !quote) {
			if (c = *line++)
				*str++ = c - '@';
			else
				break;
		} else if (metaf && c == '&' && !quote)
			*str++ = MATCHED_CHAR;
		else if (metaf && c == '/' && !quote)
			*str++ = CCLEAR_CHAR;
		else if (c < 0x80)
			*str++ = c;
		else if (c == SS2)
			*str++ = *line++ | 0x80;
		else if (c == SS3) {
			c = *line++;
			*str++ = (c << 8) | (*line++ & 0x7f) | 0x8000;
		} else {
			*str++ = (c << 8) | *line++ | 0x8080;
		}
	}
	*str = 0;

	return((c == qc) ? line : (uchar *)NULL);
}

/* getKey -- $B%-!<%3!<%I$rFI$`(J */
static int getKey(line, keyp, maskp)
uchar *line;
ulong *keyp;
int *maskp;
{
	/*
	 * $B%-!<%3!<%I$N5-=RK!$O#2DL$j(J
	 * 1. ASCII $BI=5-(J
	 *	'a'
	 *	'^H'
	 *	'\033'
	 *	'\xff'
	 *
	 * 2. XKEY $BI=5-(J
	 *	#124
	 *	#0132
	 *	#0x58
	 *	shift-A
	 *	shift-control-meta-HENKAN
	 *
	 * pseudo code
	 *	ENTERMODE
	 *	EXITMODE
	 *
	 * wild character
	 *	@ascii
	 *	@control
	 *	@raw
	 *	@any
	 */

	int	key = 0;

	*maskp = 0;

	if (*line == '\'') {	/* $B%7%s%0%k%/%)!<%H$G;O$^$k$N$G(J ASCII $BI=5-(J */
		if (*++line == '\\') {
			/* '\'' $B$N>l9g(J($B%7%s%0%k%/%)!<%H<+?H(J)$B!"(J
			 * '\\' $B$N>l9g(J($B%P%C%/%9%i%C%7%e(J)
			 * '\033' $B$N$h$&$J#8?JI=5-$N>l9g$H(J
			 * '\x27' $B$N$h$&$J#1#6?JI=5-$N>l9g$,$"$k(J
			 */
			if (*++line == '\'')	/* '\'' */
				key = '\'';
			else if (*line == '\\')	/* '\\' */
				key = '\\';
			else if (*line == 'x')	/* hexadecimal */
				(void)sscanf((char *)++line, "%x", &key);
			else			/* octal */
				(void)sscanf((char *)line, "%o", &key);
			key &= 0xff;
		} else if (*line == '^') {
			/* '^' ($B%+%l%C%H<+?H(J) $B$^$?$O%3%s%H%m!<%k%3!<%I(J */
			if (*++line == '\'')
				key = '^';
			else
				key = *line - '@';
		} else {
			key = *line;
		}
		*keyp = key;
	} else if (*line == '#') {	/* event code */
		if (*++line == '0') {	/* octal or hexadecimal */
			if (*(line + 1) == 'x')	/* hexadecimal */
				(void)sscanf((char *)line + 2, "%x", &key);
			else			/* octal */
				(void)sscanf((char *)line, "%o", &key);
		} else {
			key = atoi((char *)line);
		}
		*keyp = (ulong)key | (ulong)RAWKEY;
	} else if (!Strcmp(line, "ENTERMODE")) {
		*keyp = ENTERMODE;
	} else if (!Strcmp(line, "EXITMODE")) {
		*keyp = EXITMODE;
	} else if (*line == '@') {
		/* $B%o%$%k%I!&%-%c%i%/%?(J */
		line++;
		if (!Strcmp(line, "ascii"))
			key = META_ASCII;
		else if (!Strcmp(line, "printable"))
			key = META_PRINTABLE;
		else if (!Strcmp(line, "control"))
			key = META_CONTROL;
		else if (!Strcmp(line, "raw"))
			key = META_RAW;
		else if (!Strcmp(line, "any"))
			key = META_ANY;
		else if (!Strcmp(line, "func"))
			key = META_FUNC;
		else if (!Strcmp(line, "cursor"))
			key = META_CURSOR;
		else if (!Strcmp(line, "keypad"))
			key = META_KEYPAD;
		else if (!Strcmp(line, "modifier"))
			key = META_MODIFIER;
		else if (!Strcmp(line, "non-ascii"))
			key = META_NONASCII;
		else
			return -1;
		*keyp = (ulong)key | (ulong)METAC;
	} else {
		if ((key = getcode(line)) == 0)
			return -1;
		*keyp = (ulong)key | (ulong)RAWKEY;
		*maskp = getmask(line);
	}
	return 0;
}

/* getmask -- $B%b%G%#%U%!%$%"!&%^%9%/$rJV$9(J */
static int getmask(s)
uchar *s;
{
	int	mask = 0;
	char	buf[256];
	uchar	*p;

	if ((p = (uchar *)rindex((char *)s, '-')) == NULL)
		return 0;
	(void)strncpy(buf, (char *)s, p - s);
	buf[p - s] = '\0';
	
	if (strinc(buf, "shift"))
		mask |= ShiftMask;
	if (strinc(buf, "control"))
		mask |= ControlMask;
	if (strinc(buf, "lock"))
		mask |= LockMask;
	if (strinc(buf, "mod1"))
		mask |= Mod1Mask;
	if (strinc(buf, "mod2"))
		mask |= Mod2Mask;
	if (strinc(buf, "mod3"))
		mask |= Mod3Mask;
	if (strinc(buf, "mod4"))
		mask |= Mod4Mask;
	if (strinc(buf, "mod5"))
		mask |= Mod5Mask;
	return mask;
}

static char *strinc(s, k)
char *s;
char *k;
{
	register int	len = Strlen(k);

	while (s = index(s, *k))
		if (!Strncmp(s, k, len))
			return s;
		else
			s++;
	return NULL;
}


/* getcode -- KeySym $B$rJV$9(J */
static ulong getcode(s)
uchar *s;
{
	register uchar	*t;
	KeySym			keysym;
	KeySym			XStringToKeysym();

	if ((t = (uchar *)rindex((char *)s, '-')) == NULL)
		t = s;
	else
		t++;
	keysym = XStringToKeysym(t);
	if (keysym == NoSymbol)
		return (ulong)0;
	else
		return (ulong)keysym;
}

static int getFunc(rule, frec, fnrec, n, args)
ccRule rule;
_funcbufRec *frec;
_funcnameRec *fnrec;
int	n;
uchar	**args;
{
	int	i, j;
	uchar	*func;
	uchar	*arg;
	ushort	*fp;
	int	findex;
	void	(*efunc)() = rule->errorfunc;

	findex = funcalloc(frec, n + 1);
	fp = frec->funcbuf + findex;

	j = 0;
	while (n > 0) {
		func = *args++;
		if (!Strcmp(func, "goto")) {
			/* $B%b!<%I$NJQ99(J */
			if (n < 2)
			    break;	/* $B%b!<%IL>$,=q$$$F$J$$(J */
			arg = *args++;
			--n;
			if (!Strcmp(arg, "PREV")) {
				fp[j++] = PREVMODE;
				break;
			}
			if ((i = getMode(rule, arg)) < 0) {
				break;
			}
			fp[j++] = MODECHANGE | i;
			break;
		} else if (!Strcmp(func, "redo")) {
			fp[j++] = REDO;
			break;
		} else {
			/* $B%U%!%s%/%7%g%sI=$N8!:w(J */
			int	fnum;
			if ((fnum = funcsearch(fnrec, (char *)func)) < 0) {
				eproc(efunc, "too many functions (> %d)", MAXFUNC);
			} else {
				fp[j++] = fnum;
			}
		}
		--n;
	}
	fp[j++] = ENDFUNC;
	frec->funcp = fp + j;	/* kludge */
	return findex;
}

static ccRule
findRule(rulefile)
char *rulefile;
{
	ccRule	rule = ccrules;

	while (rule) {
		if (rule->rulefile && !strcmp(rulefile, rule->rulefile)) {
			return rule;
		}
		rule = rule->next;
	}
	return NULL;
}

static void
addRule(rule)
ccRule rule;
{
	rule->refcnt = 1;
	rule->next = ccrules;
	ccrules = rule;
}

static void
deleteRule(rule)
ccRule	rule;
{
	ccRule	rp = ccrules;
	ccRule	rp0 = NULL;

	while (rp) {
		if (rule == rp) {
			if (rp0 == NULL) {
				ccrules = rp->next;
			} else {
				rp0->next = rp->next;
			}
			return;
		}
		rp0 = rp;
		rp = rp->next;
	}
}

static int wstrsave(srec, str)
_strbufRec *srec;
wchar_t *str;
{
	int	len = wstrlen(str);
	int	pos;
	wchar_t	*p;

	pos = wstralloc(srec, len + 1);
	if (pos > 0) {
		p = srec->strbuf + pos;
		while (len-- > 0) {
			*p++ = *str++;
		}
		*p = 0;
	}
	return pos;
}

/* $B%P%C%U%!%"%m%1!<%H%U%!%s%/%7%g%s(J */

static int wstralloc(srec, len)
_strbufRec *srec;
int len;
{
	int	ret;

	if (srec->strbuf == NULL) {
		/* allocate srec->strbuf */
		srec->strbuf = (wchar_t *)Malloc(STRSIZE * sizeof(wchar_t));
		if (srec->strbuf == NULL)
			return 0;
		srec->strend = srec->strbuf + STRSIZE;
		srec->strp = srec->strbuf;
		*srec->strp++ = 0;	/* dummy */
	}
	if (srec->strp + len > srec->strend) {
		/* allocate more memory */
		int	size = (srec->strp + len) - srec->strend;
		int	offset = srec->strp - srec->strbuf;
		wchar_t	*wp;

		if (size < MORESTRSIZE)
			size = MORESTRSIZE;
		size += srec->strend - srec->strbuf;
		wp = (wchar_t *)Realloc(srec->strbuf, size * sizeof(wchar_t));
		if (wp == NULL)
			return 0;
		srec->strp = wp + offset;
		srec->strbuf = wp;
		srec->strend = wp + size;
	}

	ret = srec->strp - srec->strbuf;
	srec->strp += len;

	return ret;
}

static void wstradjust(srec)
_strbufRec *srec;
{
	int	size = srec->strp - srec->strbuf;
	wchar_t	*wp;

	if (size == 0) return;
	wp = (wchar_t *)Realloc(srec->strbuf, size * sizeof(wchar_t));
	if (wp != NULL) {
		srec->strbuf = wp;
		srec->strp = srec->strend = wp + size;
	}
}

static ConvDesc *cdalloc(crec)
_cdbufRec *crec;
{
	ConvDesc	*ret;

	if (crec->cdbuf == NULL) {
		crec->cdbuf = (ConvDesc *)Malloc(CDSIZE * sizeof(ConvDesc));
		if (crec->cdbuf == NULL)
			return NULL;
		crec->cdend = crec->cdbuf + CDSIZE;
		crec->cdp = crec->cdbuf;
	}
	if (crec->cdp >= crec->cdend) {
		int	size = crec->cdend - crec->cdbuf + MORECDSIZE;
		int	offset = crec->cdp - crec->cdbuf;
		ConvDesc	*cdp;

		cdp = (ConvDesc *)Realloc(crec->cdbuf, size * sizeof(ConvDesc));
		if (cdp == NULL) {
			return NULL;
		}
		crec->cdp = cdp + offset;
		crec->cdbuf = cdp;
		crec->cdend = cdp + size;
	}

	ret = crec->cdp++;
	return ret;
}

static void cdadjust(crec)
_cdbufRec *crec;
{
	int		size = crec->cdp - crec->cdbuf;
	ConvDesc	*cdp;

	if (size == 0) return;
	cdp = (ConvDesc *)Realloc(crec->cdbuf, size * sizeof(ConvDesc));
	if (cdp != NULL) {
		crec->cdbuf = cdp;
		crec->cdp = crec->cdend = cdp + size;
	}
}

static int funcalloc(frec, n)
_funcbufRec *frec;
int n;
{
	int	ret;

	if (frec->funcbuf == NULL) {
		/* allocate funcbuf */
		frec->funcbuf = (ushort *)Malloc(FUNCSIZE * sizeof(ushort));
		if (frec->funcbuf == NULL)
			return 0;
		frec->funcend = frec->funcbuf + FUNCSIZE;
		frec->funcp = frec->funcbuf;
		*(frec->funcp)++ = ENDFUNC;	/* dummy */
	}
	if (frec->funcp + n > frec->funcend) {
		/* allocate more memory */
		int	size = (frec->funcp + n) - frec->funcend;
		int	offset = frec->funcp - frec->funcbuf;
		ushort	*up;

		if (size < MOREFUNCSIZE)
			size = MOREFUNCSIZE;
		size += frec->funcend - frec->funcbuf;
		up = (ushort *)Realloc(frec->funcbuf, size * sizeof(ushort));
		if (up == NULL)
			return 0;
		frec->funcp = up + offset;
		frec->funcbuf = up;
		frec->funcend = up + size;
	}

	ret = frec->funcp - frec->funcbuf;
	frec->funcp += n;

	return ret;
}

static void funcadjust(frec)
_funcbufRec *frec;
{
	int	size = frec->funcp - frec->funcbuf;
	ushort	*fp;

	if (size == 0) return;
	fp = (ushort *)Realloc(frec->funcbuf, size * sizeof(ushort));
	if (fp != NULL) {
		frec->funcbuf = fp;
		frec->funcp = frec->funcend = fp + size;
	}
}

static int funcsearch(fnrec, funcname)
_funcnameRec *fnrec;
char *funcname;
{
	int	nfunc = fnrec->nfunc;
	ushort	*fnames = fnrec->funcnames;
	char	*fnbuf = fnrec->funcnamebuf;
	int	i;
	int	len;

	for (i = 0; i < nfunc; i++) {
		if (!strcmp(funcname, fnbuf + *fnames++))
			return i;
	}

	if (nfunc >= MAXFUNC)
		return -1;

	len = strlen(funcname) + 1;

	/* add new function */
	if (fnrec->funcnamebuf == NULL) {
		/* allocate funcnamebuf */
		if ((fnrec->funcnamebuf = Malloc(FUNCNAMESIZE)) == NULL)
			return -1;
		fnrec->funcnameend = fnrec->funcnamebuf + FUNCNAMESIZE;
		fnrec->funcnamep = fnrec->funcnamebuf;
	}
	if (fnrec->funcnamep + len > fnrec->funcnameend) {
		/* allocate more memory */
		int	size = (fnrec->funcnamep + len) - fnrec->funcnameend;
		int	offset = fnrec->funcnamep - fnrec->funcnamebuf;
		char	*cp;

		if (size < MOREFUNCNAMESIZE)
			size = MOREFUNCNAMESIZE;
		size += fnrec->funcnameend - fnrec->funcnamebuf;
		if ((cp = Realloc(fnrec->funcnamebuf, size)) == NULL)
			return 0;
		fnrec->funcnamep = cp + offset;
		fnrec->funcnamebuf = cp;
		fnrec->funcnameend = cp + size;
	}

	(void)strcpy(fnrec->funcnamep, funcname);
	fnrec->funcnames[nfunc] = fnrec->funcnamep - fnrec->funcnamebuf;
	fnrec->funcnamep += len;

	return fnrec->nfunc++;
}

static void funcnameadjust(fnrec)
_funcnameRec *fnrec;
{
	int	size = fnrec->funcnamep - fnrec->funcnamebuf;
	char	*cp;

	if (size == 0) return;
	if (cp = Realloc(fnrec->funcnamebuf, size)) {
		fnrec->funcnamebuf = cp;
		fnrec->funcnamep = fnrec->funcnameend = cp + size;
	}
}

static int convchar(buf, key, mask, str, len)
ccBuf buf;
ulong key;	/* keysym (RAWKEY) or ascii code */
int mask;	/* modifier mask */
char *str;	/* ascii interpretation */
int len;	/* length of str */
{
	int	r;
	int	func;
	int	redocount = 0;
	ushort	*fp;
	ccRule	rule = buf->rule;

 redo:
	/* $B$^$:$O8=:_$N%b!<%I$N%k!<%k$GJQ49$7$F$_$k(J */
	r = cconvert(buf, buf->currentmode, key, mask, &func, str, len);
	if (r < 0) {
		/* $B%^%C%A$9$k%k!<%k$,8+$D$+$i$J$+$C$?$N$G(J
		 * fallthrough $B$G;XDj$5$l$k%b!<%I$N%k!<%k$rC5$9(J
		 */
		int	tmpmode = rule->modes[buf->currentmode].fallthrough;

		while (tmpmode != NOMODE) {
			r = cconvert(buf, tmpmode, key, mask, &func, str, len);
			if (r >= 0)	/* $B%^%C%A$7$?(J */
				break;
			tmpmode = rule->modes[tmpmode].fallthrough;
		}
	}

	if (r < 0)
		return -1;	/* $B$I$l$K$b%^%C%A$7$J$+$C$?(J */

	if (func == 0)
		return 0;	/* $B2?$b$7$J$$(J */

	fp = rule->funcbuf + func;
	while ((func = *fp++) != ENDFUNC) {
		if (func == REDO) {
			if (redocount++ > MAXREDO)
			    return -1;	/* $B$?$V$sL58B%k!<%W(J */
			else
			    goto redo;	/* redo -- $B$b$&0lEY(J */
		} else if (func & MODECHANGE) {	/* $B%+%l%s%H%b!<%I$NJQ99(J */
			int	tmpmode = buf->currentmode;
			
			/* pseudo-key $B$NF~NO(J */
			(void)convchar(buf, EXITMODE, 0, (char *)NULL, 0);
			
			if (func == PREVMODE) {
				buf->currentmode = buf->previousmode;
			} else {
				buf->currentmode = func & ~MODECHANGE;
			}
			buf->previousmode = tmpmode;
			
			/* pseudo-key $B$NF~NO(J */
			(void)convchar(buf, ENTERMODE, 0, (char *)NULL, 0);
			
			/* $B%b!<%I$,JQ$o$C$?;~$K$O%3%s%F%-%9%H$r%/%j%"$9$k(J */
			ccContextClear(buf);

			/* $B%b!<%IJQ99%3!<%k%P%C%/$,$"$l$P8F$S=P$9(J */
			if (buf->modenotify) {
				(*buf->modenotify)(buf->currentmode,
						  buf->previousmode,
						  buf->client_data);
			}
			break;
		} else {
			int	truefunc = buf->functbl[func];
			/* $B%G%#%9%Q%C%A!&%k!<%A%s$r8F$V(J */
			if (truefunc >= 0) {
				(*buf->executefunction)(truefunc, str, len,
							buf->client_data);
			}
		}
	}
	return 0;
}

static int cconvert(buf, mode, inkey, mask, func, str, len)
ccBuf buf;
int mode;	/* current mode */
ulong inkey;	/* input key (raw/mapped) */
int mask;
int *func;	/* function */
char *str;
int len;
{
	ccRule		rule = buf->rule;
	ConvDesc	*entry;
	ulong		key;
	int		n;
	ModeTable	*modep;

	if (mode < 0 || mode >= rule->nmode)
		return -1;	/* No Such Mode */

	modep = &rule->modes[mode];

	if ((n = modep->nrule) <= 0)
		return -1;	/* No Rules */

	for (entry = modep->cdbuf; --n >= 0; entry++) {
		key = entry->key;
		if (key & (ulong)METAC) {
			/* $B%o%$%k%I%+!<%IJ8;z$N%^%C%A%s%0(J */
			if (!metamatch(key, inkey, len == 0))
				continue;
		} else if (key & (ulong)RAWKEY && mask != entry->mask) {
			continue;
		} else if (key != inkey) {
			continue;
		}

		/* $B%-!<$,%^%C%A$7$?(J */
		if (contextMatch(buf, rule->strbuf + entry->context)) {
			substStr(buf, rule->strbuf + entry->context,
				 rule->strbuf + entry->result, str, len);
			*func = entry->function;
			return 0;
		}
	}

	return -1;	/* No Match */
}

static int metamatch(rkey, inkey, nonascii)
ulong rkey;
ulong inkey;
int nonascii;
{
	int	type = (int)(rkey & 0xff);

	switch (type) {
	case META_ASCII:
		return !(inkey & (ulong)(RAWKEY|PSEUDO));
	case META_PRINTABLE:
		return (0x20 <= inkey && inkey < 0x7f);
	case META_CONTROL:
		return inkey < 0x20;
	case META_RAW:
		return inkey & (ulong)RAWKEY;
	case META_ANY:
		return 1;
	case META_FUNC:
		return (inkey & (ulong)RAWKEY) && IsFunctionKey(inkey & 0xffff);
	case META_CURSOR:
		return (inkey & (ulong)RAWKEY) && IsCursorKey(inkey & 0xffff);
	case META_KEYPAD:
		return (inkey & (ulong)RAWKEY) && IsKeypadKey(inkey & 0xffff);
	case META_MODIFIER:
		return (inkey & (ulong)RAWKEY) && IsModifierKey(inkey & 0xffff);
	case META_NONASCII:
		return (inkey & (ulong)RAWKEY) && nonascii;
	default:
		return 0;
	}
	/* NOTREACHED */
}

static int contextMatch(buf, context)
ccBuf buf;
wchar_t *context;
{
	wchar_t	*c0 = buf->contextend;
	wchar_t	*c1 = buf->context;

	if (context == 0 || *context == 0)
		return 1;	/* $BL5>r7o%^%C%A(J */
	if (c0 - c1 < wstrlen(context))	/* $BD9$5$N%A%'%C%/(J */
		return 0;	/* matching fail */
	c0--;
	while (*context) {
		if (CANONIC(*c0) == CANONIC(*context))
			c0--, context++;
		else
			return 0;	/* fail */
	}
	return 1;
}

static void substStr(buf, context, result, str, len)
ccBuf buf;
wchar_t *context;
wchar_t *result;
char *str;
int len;
{
	register int	c;
	int		nbytes;
	uchar	*bufp;

	/* $B%3%s%F%-%9%H$NJ,$r>C$9(J */
	while (*context++) {
		ccContextDelete(buf);
		(*buf->deletechar)(buf->client_data);
	}

	while (c = *result++) {
		if (c == MATCHED_CHAR) {
			nbytes = len;
			bufp = (uchar *)str;
			while (nbytes-- > 0) {
				c = *bufp++;
				/* bufp $B$NCf?H$O(J ASCII $B$+(J $B%+%J$J$N$G(J
				 * EUC $B%W%m%;%9%3!<%I$X$NJQ49$r$o$6$o$6(J
				 * $B$d$kI,MW$O$J$$(J
				 */
				ccContextAppend(buf, c);
				(*buf->insertchar)(c, buf->client_data);
			}
			continue;
		} else if (c == CCLEAR_CHAR) {
			/* $B%3%s%F%-%9%H$r%/%j%"$9$k(J */
			ccContextClear(buf);
			continue;
		}
		ccContextAppend(buf, c);
		(*buf->insertchar)(c, buf->client_data);
	}
}

/*
 *	Public Functions
 */

/* ccParseRule -- $BJQ49Dj5A%U%!%$%k$rFI$_9~$`(J */
ccRule ccParseRule(deffile, errprint)
char *deffile;		/* $BF~NOJ8;zJQ49Dj5A%U%!%$%k(J */
void (*errprint)();	/* $B%(%i!<%a%C%;!<%8I=<(MQ%3!<%k%P%C%/(J */
{
	ccRule		rule;
	extern char	*getenv();

	/* $BJQ49Dj5A%U%!%$%k$NFI$_9~$_(J */
	if (deffile == NULL) {
		/* $B4D6-JQ?t(J CC_DEF $B$rD4$Y$k(J */
		if ((deffile = getenv("CC_DEF")) == NULL) {
			return (ccRule)NULL;
		}
	}

	if (rule = findRule(deffile)) {
		/* same rule found */
		rule->refcnt++;
		return rule;
	}

	/* ccRuleRec $B$N%"%m%1!<%H(J */
	if ((rule = (ccRule)Malloc(sizeof(ccRuleRec))) == NULL) {
		return (ccRule)NULL;
	}

	rule->errorfunc = errprint;
	rule->rulefile = Malloc(Strlen(deffile) + 1);
	if (rule->rulefile) {
		(void)Strcpy(rule->rulefile, deffile);
	}

	if (readRuleFile(rule, deffile) < 0) {
		Free(rule);
		return (ccRule)NULL;
	}

	addRule(rule);

	return rule;
}

/* ccCreateBuf -- $BJQ49%P%C%U%!$r:n$k(J */
ccBuf ccCreateBuf(rule, csize, functable, nfunc,
		  def_action, insert, delete, execute, modenotify, data)
ccRule rule;		/* $B;HMQ$9$kF~NOJ8;zJQ49%k!<%k(J */
int csize;		/* context size ($BJ8;z?t(J) */
char *functable[];	/* $B%U%!%s%/%7%g%s!&%F!<%V%k(J */
int nfunc;		/* functable $B$N%(%s%H%j?t(J */
void (*def_action)();	/* $B%G%U%)%k%H!&%"%/%7%g%s!&%3!<%k%P%C%/(J */
void (*insert)();	/* $BJ8;zF~NO%3!<%k%P%C%/(J */
void (*delete)();	/* $BJ8;z:o=|%3!<%k%P%C%/(J */
void (*execute)();	/* $B%U%!%s%/%7%g%s<B9T%3!<%k%P%C%/(J */
void (*modenotify)();	/* $B%b!<%IJQ99DLCN%3!<%k%P%C%/(J */
caddr_t data;		/* callback $B%G!<%?(J */
{
	ccBuf	buf;
	char	*funcnamep;
	short	*functblp;
	int	i, j;

	/* ccBuf $B$N%"%m%1!<%H(J */
	if ((buf = (ccBuf)Malloc(sizeof(ccBufRec))) == NULL) {
		return (ccBuf)NULL;
	}

	buf->rule = rule;

	/* context $B%P%C%U%!$N%"%m%1!<%H(J */
	if (csize <= 0) csize = 1;
	buf->context = (wchar_t *)Malloc(csize * sizeof(wchar_t));
	if (buf->context == NULL) {
		Free(buf);
		return (ccBuf)NULL;
	}
	buf->contextend = buf->context;
	buf->contextsize = csize;

	/* function $B%3%s%P!<%H%F!<%V%k$N%"%m%1!<%H(J */
	buf->functbl = (short *)Malloc(rule->nfunc * sizeof(short));
	if (buf->functbl == NULL) {
		Free(buf->context);
		Free(buf);
		return (ccBuf)NULL;
	}
	/* ccRule $B$KF~$C$F$$$k%U%!%s%/%7%g%sI=$H!"0z?t$GM?$($i$l$?(J
	 * $B%U%!%s%/%7%g%sI=$+$i!"(JccRule $BFbIt$N%U%!%s%/%7%g%sHV9f$H(J
	 * $B:#M?$($i$l$?%U%!%s%/%7%g%sHV9f$H$NBP1~I=$r:n$k(J
	 */
	funcnamep = rule->funcnamebuf;
	functblp = buf->functbl;
	for (i = rule->nfunc, functblp = buf->functbl; i > 0; i--, functblp++) {
		for (j = 0; j < nfunc; j++) {
			if (!strcmp(functable[j], funcnamep)) {
				*functblp = j;
				break;
			}
		}
		if (j >= nfunc) *functblp = -1;

		while (*funcnamep++)
			;
	}

	buf->defaultaction = def_action;
	buf->insertchar = insert;
	buf->deletechar = delete;
	buf->executefunction = execute;
	buf->modenotify = modenotify;
	buf->client_data = data;

	/* $B%+%l%s%H%b!<%I$N@_Dj(J */
	buf->previousmode = buf->currentmode = rule->initialmode;

	return buf;
}

/* ccFreeRule -- $B%k!<%k$r<N$F$k(J */
void ccFreeRule(rule)
ccRule rule;
{
	ModeTable	*modep;
	int		i;

	if (rule == NULL) return;
	if (--rule->refcnt > 0) return;

	deleteRule(rule);

	for (modep = rule->modes, i = 0; i < rule->nmode; modep++, i++) {
		Free(modep->name);
		Free(modep->cdbuf);
		Free(modep->prompt);
	}
	Free(rule->rulefile);
	Free(rule->modes);
	Free(rule->strbuf);
	Free(rule->funcbuf);
	Free(rule->funcnamebuf);
	Free(rule);
}

/* ccDestroyBuf -- $BJQ49%P%C%U%!$r<N$F$k(J */
void ccDestroyBuf(buf)
ccBuf buf;
{
	if (buf == NULL) return;
	Free(buf->context);
	Free(buf->functbl);
	Free(buf);
}

/* ccConvchar -- $BJQ49MQ%U%!%s%/%7%g%s(J */
int ccConvchar(buf, event)
ccBuf buf;
XKeyPressedEvent *event;	/* $B%-!<%$%Y%s%H(J */
{
	int	r;
	char	str[256];
	char	*p;
	int	nbytes;
	int	n;

	/* $B$H$j$"$($:(J LookupString $B$7$F$*$/(J */
	nbytes = XLookupString(event, str, sizeof(str), 0, 0);

	/* $B$^$:$O%$%Y%s%H%3!<%I$GJQ49$7$F$_$k(J */
	r = convchar(buf, ccEncodeKey(event), ccEncodeMask(event),
		       str, nbytes);
	if (r < 0) {
		/* ASCII $BI=5-$GJQ49$7$F$_$k(J */
		p = str;
		n = nbytes;
		while (n-- > 0) {
			r = convchar(buf, ccEncodeChar(*p), 0, p, 1);
			/* $B$3$l$b%^%C%A$7$J$1$l$P!"(Jdefault action $B$N(J
			 * $B%U%!%s%/%7%g%s$r$h$V(J
			 */
			if (r < 0 && buf->defaultaction != NULL) {
				(void)(*buf->defaultaction)(str, nbytes, buf->client_data);
			}
			p++;
		}
	}
	return 0;
}

/* ccGetMode -- $B8=:_$N%b!<%IHV9f$rJV$9(J */
int ccGetMode(buf)
ccBuf buf;
{
	return buf->currentmode;
}

/* ccGetModePrompt -- $B8=:_$N%b!<%I$N%W%m%s%W%HJ8;zNs$rJV$9(J */
wchar_t *ccGetModePrompt(buf)
ccBuf buf;
{
	return buf->rule->modes[buf->currentmode].prompt;
}

/* ccGetRule -- $BJQ49%P%C%U%!$G;H$o$l$F$$$kJQ49%k!<%k$rJV$9(J */
ccRule ccGetRule(buf)
ccBuf buf;
{
	return buf->rule;
}

/* ccContextAppend -- $B%3%s%F%-%9%H$K(J1$BJ8;z2C$($k(J */
void ccContextAppend(buf, c)
ccBuf buf;
int c;
{
	wchar_t	*p;

	/* $B%3%s%F%-%9%HJ8;zNs$KA^F~(J */
	if (buf->contextend - buf->context < buf->contextsize) {
		*buf->contextend++ = c;
	} else {
		/* $B%3%s%F%-%9%HJ8;zNs$,0lGU$J$N$G(J
		 * $B@hF,$N#1J8;z$r<N$F$F$D$a$k(J
		 */
		p = buf->context + 1;
		while (p < buf->contextend) {
			*(p - 1) = *p;
			p++;
		}
		/* $B$"$$$?=j$KA^F~(J */
		*--p = c;
	}
}

/* ccContextDelete -- $B%3%s%F%-%9%H$r(J1$BJ8;z:o=|$9$k(J */
void ccContextDelete(buf)
ccBuf buf;
{
	if (buf->contextend > buf->context)
		buf->contextend--;
}

/* ccContextClear -- $B%3%s%F%-%9%H$r%/%j%"$9$k(J */
void ccContextClear(buf)
ccBuf buf;
{
	/* $B%b!<%I$,JQ$o$C$?;~$K$O<+F0E*$K%/%j%"$5$l$k$,$=$l0J30$K(J
	 * $B8=:_$N%3%s%F%-%9%H$r6/@)E*$K%/%j%"$7$?$$>l9g$KMQ$$$k(J
	 */
	buf->contextend = buf->context;
}

/* ccContextSet -- $B%3%s%F%-%9%H$r%;%C%H$9$k(J */
void ccContextSet(buf, cstr)
ccBuf buf;
wchar_t *cstr;
{
	int	len = wstrlen(cstr);
	wchar_t	*p = buf->context;

	if (len > buf->contextsize) {
		cstr += len - buf->contextsize;
		len = buf->contextsize;
	}
	while (len-- > 0) {
		*p++ = *cstr++;
	}
}

/* ccContextGet -- $B8=:_$N%3%s%F%-%9%H$rJV$9(J */
void ccContextGet(buf, cstr)
ccBuf buf;
wchar_t	*cstr;
{
	register wchar_t	*wp = buf->context;

	while (wp < buf->contextend)
		*cstr++ = *wp++;
	*cstr = 0;
}


/*
 *	Obsolete Functions
 */

/* ccInit -- $BJQ49%k!<%k$rFI$_9~$s$G%P%C%U%!$r:n$k(J */
ccBuf ccInit(deffile, contextsize, defactfunc, insertfunc, deletefunc, dofunc,
	     errprint, functable, functablesize)
char *deffile;
int contextsize;
void (*defactfunc)();
void (*insertfunc)();
void (*deletefunc)();
void (*dofunc)();
void (*errprint)();
char *functable[];
int functablesize;
{
	ccRule	rule;

	if ((rule = ccParseRule(deffile, errprint)) == NULL) {
		return (ccBuf)NULL;
	}

	return ccCreateBuf(rule, contextsize, functable, functablesize,
			   defactfunc, insertfunc, deletefunc, dofunc,
			   (void (*)())NULL, (caddr_t)NULL);
}

/* ccTerminate -- $BF~NOJ8;zJQ49$r=*N;$9$k(J */
void ccTerminate(buf)
ccBuf buf;
{
	ccFreeRule(buf->rule);
	ccDestroyBuf(buf);
}


#ifdef DEBUG
/*
 *	Debug Functions
 */

static void putws(s)
wchar_t *s;
{
	unsigned char	line[256];

	(void)convJWStoSJIS(s, line);
	fputs(line, stdout);
}

static void puteuc(s)
uchar *s;
{
	wchar_t	tmp[256];

	(void)convEUCtoJWS(s, tmp);
	putws(tmp);
}

void dumpRules(rule, mode)
ccRule rule;
int mode;
{
	int	nkey;
	ModeTable	*modep;
	ConvDesc	*cdp;
	wchar_t		*strbuf = rule->strbuf;
	ushort		*funcbuf = rule->funcbuf;
	char		**funcnames;
	wchar_t		*p, *q;
	wchar_t		restmp[256];
	ushort		*funcp;
	int	i, j;

	funcnames = (char **)__builtin_alloca(rule->nfunc * sizeof(char *));
{	char	*cp, **fnp;
	cp = rule->funcnamebuf;
	fnp = funcnames;
	for (i = 0; i < rule->nfunc; i++) {
		*fnp++ = cp;
		while (*cp++)
			;
	}
}

	if (mode < 0 || mode >= rule->nmode) {
		printf("No such mode %d\n", mode);
		return;
	}
	modep = &rule->modes[mode];

	printf("mode: %s (%d) prompt: ", modep->name, mode);
	putws(modep->prompt);
	if (modep->fallthrough != NOMODE) {
		printf(" fallthrough: %d", modep->fallthrough);
	}
	putchar('\n');
	cdp = modep->cdbuf;
	for (i = 0; i < modep->nrule; i++) {
		printf("rule[%d]: \"", i);
		putws(strbuf + cdp->context);
		printf("\"\t");
		if (cdp->key & RAWKEY) {
			ulong	key = cdp->key & ~RAWKEY;
			int	mask = cdp->mask;
			char	*keysymname;
			if (mask & ShiftMask) printf("shift-");
			if (mask & ControlMask) printf("control-");
			if (mask & LockMask) printf("lock-");
			if (mask & Mod1Mask) printf("mod1-");
			if (mask & Mod2Mask) printf("mod2-");
			if (mask & Mod3Mask) printf("mod3-");
			if (mask & Mod4Mask) printf("mod4-");
			if (mask & Mod5Mask) printf("mod5-");
			keysymname = XKeysymToString((KeySym)key);
			printf(keysymname ? keysymname : "<illegal keysym>");
		} else if (cdp->key & METAC) {
			switch (cdp->key & ~METAC) {
			case META_ASCII:	printf("@ascii"); break;
			case META_CONTROL:	printf("@control"); break;
			case META_RAW:		printf("@raw"); break;
			case META_ANY:		printf("@any"); break;
			case META_FUNC:		printf("@func"); break;
			case META_CURSOR:	printf("@cursor"); break;
			case META_KEYPAD:	printf("@keypad"); break;
			case META_MODIFIER:	printf("@modifier"); break;
			case META_NONASCII:	printf("@non-ascii"); break;
			case META_PRINTABLE:	printf("@printable"); break;
			default:		printf("<illegal meta>");
			}
		} else if (cdp->key & PSEUDO) {
			switch (cdp->key) {
			case ENTERMODE:	printf("ENTERMODE"); break;
			case EXITMODE:	printf("EXITMODE"); break;
			default:	printf("<illegal pseudo>");
			}
		} else {
			putchar('\'');
			if (cdp->key >= 0x80) {
				printf("\\x%x", cdp->key);
			} else if (cdp->key < 0x20) {
				putchar('^');
				putchar(cdp->key + '@');
			} else if (cdp->key == 0x7f) {
				printf("^?");
			} else {
				putchar(cdp->key);
			}
			putchar('\'');
		}
		printf("\t\"");
		p = restmp;
		q = strbuf + cdp->result;
		while (*q) {
			if (*q == MATCHED_CHAR) {
				*p++ = '&';
			} else if (*q == CCLEAR_CHAR) {
				*p++ = '/';
			} else {
				*p++ = *q;
			}
			q++;
		}
		*p = 0;
		putws(restmp);
		printf("\"\t");
		funcp = funcbuf + cdp->function;
		while (*funcp != ENDFUNC) {
			if (*funcp == REDO) {
				printf("redo ");
			} else if (*funcp == PREVMODE) {
				printf("goto prev ");
			} else if (*funcp & MODECHANGE) {
				int	gotomode = *funcp & ~MODECHANGE;
				if (gotomode < 0 || gotomode >= rule->nmode) {
					printf("<illegal goto>");
				} else {
					printf("goto %s ",
					       rule->modes[gotomode].name);
				}
			} else {
				if (*funcp >= rule->nfunc) {
					printf("<illegal function> ");
				} else {
					printf("%s ", funcnames[*funcp]);
				}
			}
			funcp++;
		}
		putchar('\n');
		cdp++;
	}
	putchar('\n');
}

void dumpAllRules(rule)
ccRule rule;
{
	int	i;

	printf("** RULE DUMP **\n");
	printf("number of modes: %d  initialmode: %s (%d)\n\n",
	       rule->nmode,
	       rule->modes[rule->initialmode].name,
	       rule->initialmode);
	for (i = 0; i < rule->nmode; i++) {
		dumpRules(rule, i);
	}
	fflush(stdout);
}
#endif
