  kterm $B$O!"(BX11 $B%&%#%s%I%&%7%9%F%`>e$GF|K\8l$rMxMQ$9$k$?$a$N%?!<%_%J%k(B
$B%(%_%e%l!<%?$G$9!#(Bkterm $B$O(B xterm $B$r85$K:n$i$l$F$$$^$9$N$G!"(Bxterm $B$K4X$9(B
$B$k%P%0$O!"$[$H$s$I(B kterm $B$K$bEv$F$O$^$j$^$9!#$^$?(B kterm $BFH<+$N%P%0$b3'(B
$BL5$H$O8@$($^$;$s!#(Bkterm $B$KFH<+$N%P%0$K4X$7$F$O!";d$K$*CN$i$;2<$5$$!#(B

                                              1989$BG/(B5$B7n(B20$BF|(B   $BdFC+M5?M(B
                                              kagotani@cs.titech.ac.jp

$B%$%s%9%H!<%k(B

  $BK\%P!<%8%g%s$N(B kterm $B$O!"(BX11R3 $B$N(B xterm $B$r85$K:n$i$l$F$$$^$9!#(BX11R3
$B$K$O!"%*%U%#%7%c%k%Q%C%A$rEv$F$J$1$l$P$J$j$^$;$s$,!"(BNo.4 $B$+$i(B No.9 $B$^(B
$B$G$J$i!"$I$N%l%Y%k$G$b9=$$$^$;$s!#(BNo.10 $B0J9_$K(B xterm $B$N%Q%C%A$,4^$^$l(B
$B$k$+$I$&$+J,$+$i$J$$$N$G(B No.10 $B0J9_$G$&$^$/$$$/$H$$$&J]>Z$O$"$j$^$;$s!#(B

  $BE,Ev$J%G%#%l%/%H%j$G(B xterm $B$N%=!<%9$KK\%Q%C%A$rEv$F$^$9!#0J2<$N%3%s(B
$B%Q%$%k%*%W%7%g%s$rA*$s$G(B Imakefile $B$r=$@5$7$F%3%s%Q%$%k$7$F2<$5$$!#(B
-DKANJI		$BF|K\8l$NI=<(!"%+%C%H%*%Z%l!<%7%g%s$,$G$-$k$h$&$K$J$j$^(B
		$B$9!#(B
-DSTATUSLINE	$B%9%F!<%?%9%i%$%s$,;H$($k$h$&$K$J$j$^$9!#(B
-DKEEPALIVE	X$B%5!<%P$H$N@\B3$K(B KEEPALIVE $B%*%W%7%g%s$N;HMQ$,A*Br$G$-(B
		$B$k$h$&$K$J$j$^$9!#(B
$B$3$l$i$N%*%W%7%g%s$rA4$/IU$1$J$$$H!"%T%e%"$J(B xterm $B$HF1$8$b$N$,$G$-$^$9!#(B

kterm $B$r(B make install $B$K$h$C$F%$%s%9%H!<%k$7$F2<$5$$!#(B
termcap $B$K(B kterm $BMQ$N%(%s%H%j$r:n@.$7$F2<$5$$!#(B
X11R3 $B$KIUB0$7$F$$$k(B a14,r14,k14 $B$N3F%U%)%s%H$r%$%s%9%H!<%k$7$F2<$5$$!#(B
kterm $B>e$G(B README.j $B$r(B cat $B$9$k$3$H$G(B kterm $B$,@5$7$/F0$$$F$$$k$3$H$,(B
$B%F%9%H$G$-$^$9!#(B


$B%+%C%H%*%Z%l!<%7%g%s(B

  ICCCM $B$N7hDj$K$h$j!"%+%C%H%P%C%U%!$K$OHs(B ASCII $BJ8;z$rF~$l$i$l$J$/$J(B
$B$C$?$N$G!"(Bselection $B$r(B COMPOUND_TEXT $B$H$7$FMQ$$$^$9!#$=$N$?$aK\%P!<%8%g(B
$B%s0J9_$N(B kterm $B$G%+%C%H$7$?Hs(B ASCII $BJ8;zNs$O!"8E$$(B kterm $B$d(B NEmacs $B$G(B
$B$O%Z!<%9%H$G$-$^$;$s!#5UJ}8~$N%Z!<%9%H$O$G$-$^$9!#(B

  COMPOUND_TEXT $B$KMQ$$$i$l$k%3!<%I$O!V(BCompound Text Encoding Version 1$B!W(B
$B$K4p$E$$$F$*$j!"0J2<$N$h$&$K;H$o$l$^$9!#(B
	G0 $B$O(B GL $B$K!"(BG1 $B$O(B GR $B$K8F$S=P$5$l$F$$$F!"8GDj$5$l$k!#(B
	ASCII		$B>o$K(B G0 $B$K;X<($5$l$F$$$k!#(B
	Latin-1 $B1&B&(B	$BDL>o!"(BG1 $B$K;X<($5$l$F$$$k!#(B
	$BH>3Q$+$J(B	$BI,MW$K1~$8$F(B G1 $B$K;X<($5$l$F;HMQ$5$l$k!#(B
	$B4A;z(B		$BI,MW$K1~$8$F(B G1 $B$K;X<($5$l$F;HMQ$5$l$k!#(B
$B$?$@$7!"%Z!<%9%H$9$kB&$G$O!"$I$N%-%c%i%/%?%;%C%H$,(B G0,G1 $B$N$$$:$l$K;X<((B
$B$5$l$F$b@5$7$/2r<a$5$l$^$9!#(B
