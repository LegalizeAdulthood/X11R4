.EH ''''
.OH ''''
.EF 'X-Window$@F|K\8l(JVersion$@8&5f2q(J'3\-%'$@4A;zF~NO%7%9%F%`;EMM!JBh(J2$@HG!K(J'
.OF 'X-Window$@F|K\8l(JVersion$@8&5f2q(J'3\-%'$@4A;zF~NO%7%9%F%`;EMM!JBh(J2$@HG!K(J'
.nr H1 2
.nr H2 0
.nr H3 0
.nr PI 2n
.NH
.LG
XJP$@%i%$%V%i%j%$%s%?%U%'!<%9(J
.sp 1
.PP
$@%i%$%V%i%j%$%s%?%U%'!<%9$H$7$F$O!"(J2.$@%7%9%F%`35MW$G$b?($l$?$h$&$K0J(J
$@2<$N#2<oN`$,$"$k!#$J$*N><T$r9g$o$;$F(JXJP$@%i%$%V%i%j!J(JXJP_library$@!K$H(J
$@8F$V$3$H$K$9$k!#(J
.sp 1
.RS
.IP (1) 4
$@%3%_%e%K%1!<%7%g%s%-%C%H(J
.IP (2) 4
$@4J0W%i%$%V%i%j(J
.RE
.sp 1
.PP
(1)$@$O!"(JMMI$@$r$D$+$5$I$kF|K\8l%U%m%s%H%(%s%I$rJL%W%m%;%9!J%5!<%P(J
$@!K$H$7$F!"$3$l$H$NDL?.$r9T$&$?$a$N%i%$%V%i%j$G$"$k!#(J
$@%"%W%j%1!<%7%g%s%W%m%0%i%`$O$3$N%i%$%V%i%j$rMQ$$$F!"$+$J4A;zJQ49(J
$@=hM}MQ$NJQ49%&%#%s%I%&$N(J
$@I=<(@)8f!"$"$k$$$OJQ497k2L$NJ8;zNs!J3NDjJ8;zNs!K$N3MF@Ey$r9T$&!#(J
.PP
$@0lJ}(J(2)$@$O!"(JXlib$@$K$"$k(JXLookupString$@$HF1MM$K(JKeyEvent$@$N%P%C%/%(%s%I(J
$@$H$7$FF0:n$9$k4J0W4X?t$G$"$j!"JQ49%&%#%s%I%&$NI=<(@)8fEy$N:Y$+$$(J
$@A`:n$r9T$&$3$H$O$G$-$J$$!#$7$+$74{B8$N%"%W%j%1!<%7%g%s%W%m%0%i%`$NF|(J
$@K\8l2=$K$*$$$F$O!">/$J$/$H$b(JXLookupString$@$NBe$o$j$K(JXLookupKanjiString
$@$rMQ$$$k$3$H$K$h$j!"F|K\8l!J4A;z!KF~NO$O2DG=$K$J$k!#(J
.PP
$@$J$*!"@bL@$O0J2<$N7A<0$G9T$&!#(J
.sp 2
$@%i%$%V%i%jL>(J(3K)
.sp 1
.nr PI 8n
.SH
$@L>>N(J
.RS
intro \- XJP$@%i%$%V%i%j$N>R2p(J
.RE
.sp 1
.SH
$@5-=R7A<0(J
.RS
.nf
.ta 12
#include <X11/Xlib.h>	$@;HMQ$9$k%$%s%/%k!<%I%U%!%$%k(J
int kanji(a)	$@4X?t5Z$S7?L>(J
int a;	$@0z?t5Z$S7?L>(J
.RE
.sp 1
.SH
$@0z?t(J
.sp 1
.SH
$@@bL@(J
.RS
XJP$@%i%$%V%i%j$K$D$$$F@bL@$9$k!#(J
(3K)$@$3$l$i$N4X?t$O!"(JXJP$@%i%$%V%i%j(JlibXJp.a$@$r9=@.$9$k!#(J
$@%3%s%Q%$%i(Jcc(1)$@$O$3$l$r<+F0E*$K8F$S=P$5$J$$$,!"(J\-lXJp
$@%*%W%7%g%s$r;XDj$9$k$3$H$K$h$j%j%s%+$,$3$N%i%$%V%i%j$rC5:w$9$k!#(J
.RE
.sp 1
.SH
$@La$jCM(J
.sp 1
.nr PI 2n
.PP
$@0J2<!"(J3.1$@$G$O%3%_%e%K%1!<%7%g%s%-%C%H$K$D$$$F!"(J3.2$@$G$O4J0W%i%$(J
$@%V%i%j$K$D$$$F!"$=$N4X?t;EMM$r<($9!#(J
.RE
.bp
.NH 2
$@%3%_%e%K%1!<%7%g%s%-%C%H(J
.LP
.sp 1
.IP (1) 4
$@5!G=0lMw(J
.PP
$@%3%_%e%K%1!<%7%g%s%-%C%H$O5!G=JL$K!"<!$N$h$&$KJ,N`$G$-$k!#(J
.RS
.IP $@!&(J 2
XJP$@%U%m%s%H%(%s%IDL?.@)8f$K4X$o$k5!G=(J
.RS
.RS
XJp_open, XJp_begin, XJp_end, XJp_close
.RE
.RE
.IP $@!&(J 2
$@JQ49(J($@3NDj(J)$@J8;zNs$N3MF@$K4X$o$k5!G=(J
.RS
.RS
XJp_read, XJp_string
.RE
.RE
.IP $@!&(J 2
$@$+$J4A;zJQ49%&%#%s%I%&$NI=<(@)8f$K4X$o$k5!G=(J
.RS
.RS
XJp_visible, XJp_invisible, XJp_move, XJp_change
.RE
.RE
.IP $@!&(J 2
$@%(%i!<=hM}$K4X$o$k5!G=(J
.RS
.RS
XJp_error, XJp_reset
.RE
.RE
.RE
.sp 1
.IP (2) 4
$@0lHLE*$J;HMQNc(J
.PP
$@%3%_%e%K%1!<%7%g%s%-%C%H$rMQ$$$F!"F|K\8l(J($@4A;z(J)$@$rF~NO$9$k0lHLE*$J(J
$@;HMQJ}K!(J($@4X?t%3!<%k$N%7!<%1%s%9(J)$@$r0J2<$K<($9!#(J
.RS
.RS
.IP XJp_open() 20
XJP$@%U%m%s%H%(%s%I$H$N@\B3$H=i4|2=(J
.IP XJp_visible() 20
$@$+$J4A;zJQ49%&%#%s%I%&$NI=<((J
.IP XJp_begin() 20
XJP$@%U%m%s%H%(%s%I$X$NJQ493+;O;X<((J
.IP XJp_string() 20
$@JQ49J8;zNs$N3MF@(J
.IP XJp_end() 20
XJP$@%U%m%s%H%(%s%I$X$NJQ49=*N;;X<((J
.IP XJp_close() 20
XJP$@%U%m%s%H%(%s%I$H$N@\B3CG$H=*N;=hM}(J
.RE
.RE
.PP
$@$J$*!">e5-0J30$N4X?t$N;HMQ$K$D$$$F$O0J2<$NDL$j$G$"$k!#(J
.RS
.IP $@!&(J 2
XJp_string$@$NBe$o$j$K(JXJp_read$@$r;HMQ$G$-$k!#(J
.IP $@!&(J 2
$@I=<(@)8f$K4X$o$k(JXJp_visible, XJp_invisible, XJp_move, XJp_change$@$O(J
XJp_open$@$H(JXJp_close$@$N4V$G?o;~;HMQ$G$-$k!#(J
.IP $@!&(J 2
XJp_begin$@$H(JXJp_end$@$O(JXJp_open$@$H(JXJp_close$@$N4V$G?o;~;HMQ$G$-$k!#(J
.RE
.sp 1
.IP (3) 4
$@%(%i!<=hM}(J
.PP
XJP$@%i%$%V%i%j$K$O%(%i!<=hM}$r9T$&$?$a$N4X?t$,MQ0U$5$l$F$$$k!#(J
.RS
.RS
.IP XJp_error() 20
$@%(%i!<$N860x$r<($94JC1$J%a%C%;!<%8$rI8=`=PNO$X=PNO$9$k!#(J
.IP XJp_reset() 20
XJP$@%U%m%s%H%(%s%I!"F|K\8l%5!<%P$J$I$,0[>o=*N;$7$?;~$K!"(J
XJP$@%i%$%V%i%j$r%j%;%C%H$7$F$+$J4A;zJQ49MW5a%&%#%s%I%&$KBP$9$k(J
$@%-!<F~NO$rM-8z$K$9$k!#(J
.RE
.RE
.PP
$@%-!<F~NO$r3MF@$7$?$^$^(JXJP$@%U%m%s%H%(%s%I$,0[>o=*N;$7$?>l9g!"(J
XJP$@%i%$%V%i%j$O$=$N$3$H$rCN$k$3$H$,$G$-$J$$$N$G(J
$@$+$J4A;zJQ49MW5a%&%#%s%I%&$KBP$9$k%-!<F~NO$,$G$-$J$/$J$C$F$7$^$&!#(J
$@$3$l$rHr$1!"(JXJP$@%U%m%s%H%(%s%I$,0[>o=*N;$7$?$3$H$rCN$k$?$a$K!"(J
$@%/%i%$%"%s%H$O$+$J4A;zJQ49%&%#%s%I%&$,GK2u$5$l$?$3$H$rDLCN(J
$@$9$k(JDestroyNotify$@%$%Y%s%H$r<u$1<h$k$h$&$K$9$kI,MW$,$"$k!#(J
$@6qBNE*$K$O!"$+$J4A;zJQ49%&%#%s%I%&(JID$@$G$"$k(JXJp_open()$@$N%j%?!<%sCM$r(J
XSelectInput$@$NBhFs0z?t$H$7$F%3!<%k$9$k!#(J
.br
.nf
	xjpid = XJp_open(...);
	XSelectInput(display, xjpid, StructureNotifyMask);
.PP
$@$5$i$K%/%i%$%"%s%H$O$+$J4A;zJQ49%&%#%s%I%&$N(J
DestroyNotify$@%$%Y%s%H$r%A%'%C%/$7!"(J
$@$b$7$=$N%$%Y%s%H$,DLCN$5$l$?$J$i(JXJp_reset()$@$r%3!<%k$7$F(J
XJp_open()$@$r%3!<%k$9$k0JA0$N>uBV$KLa$9I,MW$,$"$k!#(J
.sp 1
.IP (4) 4
$@MxMQJ}K!(J
.PP
$@%3%_%e%K%1!<%7%g%s%-%C%H$rMxMQ$9$k>l9g$K$O!"0J2<$N$3$H$,(J
$@I,MW$G$"$k!#(J
.RS
.IP $@!&(J 2
/usr/include/X11/XJplib.h$@$r%$%s%/%k!<%I$9$k!#(J
.IP $@!&(J 2
/usr/lib/libXJp.a$@$r%j%s%/$9$k!#%j%s%/;~$K0J2<$N%*%W%7%g%s$r(J
$@;XDj$9$k!#(J
.br
               -lXJp
.RE
.sp 3
.LP
$@0J2<!"%$%s%/%k!<%I%U%!%$%k$*$h$S%i%$%V%i%j4X?t$N>\:Y$r<($9!#(J
.bp
.SH
$@!N%$%s%/%k!<%I%U%!%$%k!!(JXJplib.h$@!O(J
.LP
.nf
/* $@%3%_%e%K%1!<%7%g%s%-%C%H!&%$%s%/%k!<%I!&%U%!%$%k(J */ 
.sp 1
/* $@%3!<%I7O(J */ 
.ta 6 14
#define	XJP_JIS7	0
#define	XJP_JIS8	1
#define	XJP_EUC	2
#define	XJP_SJIS	3
.sp 1
/* $@J8;z%;%C%H(J */
.ta 6 16
#define	XJP_ascii	"(B"
#define	XJP_romanX0201	"(J"
#define	XJP_kanaX0201	"(I"
#define	XJP_kanjiX0208	"$B"
#define	XJP_gaiji1byte	"(0"
#define	XJP_gaiji2byte	"$0"
.sp 1
/* $@$+$J4A;zJQ49%&%#%s%I%&I=<(%b!<%I(J */ 
.ta 6 14
#define	XJP_UNDER	0
#define	XJP_XY	1
#define	XJP_ROOT	2
.sp 1
/* $@$+$J4A;zJQ49%&%#%s%I%&$NB0@-%^%9%/(J */
.ta 6 14 
#define	XJP_PWIDTH	0x00000001L 
#define	XJP_PHEIGHT	0x00000002L 
#define	XJP_X	0x00000004L 
#define	XJP_Y	0x00000008L 
#define	XJP_WIDTH	0x00000010L 
#define	XJP_HEIGHT	0x00000020L 
#define	XJP_FG	0x00000040L 
#define	XJP_BG	0x00000080L 
#define	XJP_BP	0x00000100L 
#define	XJP_FONTS	0x00000200L 
#define	XJP_STATUS	0x00000400L 
.sp 1
/* $@%U%)%s%H>pJs9=B$BN(J */ 
.ta 4 15
typedef struct {
	char	*id;
	char	*fontname;
} XJp_fontInfo;
.sp 1
/* $@$+$J4A;zJQ49%&%#%s%I%&$NB0@-9=B$BN(J */ 
.ta 4 15
typedef struct$@!P(J
	long	p_width;
	long	p_height;
	long	x;
	long	y;
	long	width;
	long	height;
	unsigned long	fg;
	unsigned long	bg;
	long	bp;
	long	status_x;
	long	status_y;
	long	status_width;
	long	status_height;
	XJp_fontInfo	*fonts;
	int	num_fonts;
$@!Q(JXJp_attributes;
.sp 1
typedef	unsigned long	XJp_id;
extern int	XJp_errno;
.EH 'XJp_open(3K)'XJP_library Manual'XJp_open(3K)'
.OH 'XJp_open(3K)'XJP_library Manual'XJp_open(3K)'
.bp
.nr PI 8n
.SH
$@L>>N(J
.RS
XJp_open \- $@$+$J4A;zJQ49%&%#%s%I%&$N=i4|2=(J
.RE
.SH
$@5-=R7A<0(J
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
XJp_id XJp_open(\fIdisplay, w, valuemask, values, dispmode, aux_data\fP)
.RS
.nf
.ft B
Display \fI*display\fP;
Window \fIw\fP; 
unsigned long \fIvaluemask\fP;
XJp_attributes \fI*values\fP;
int \fIdispmode\fP;
char \fI*aux_data\fP;
.ft R
.RE
.RE
.SH
$@0z?t(J
.RS
.IP \fIdisplay\fP 14
$@%G%#%9%W%l%$9=B$BN$X$N%]%$%s%?(J
.IP \fIw\fP 14
$@$+$J4A;zJQ49MW5a%&%#%s%I%&(JID
.IP \fIvaluemask\fP 14
values$@$GDj5A$9$kB0@-$r;XDj$9$k!#$3$N%^%9%/$OM-8z$J(J
$@B0@-$N%S%C%H%^%9%/$N%S%C%H(JOR$@$G$"$k!#(Jvaluemask$@$,%<(J
$@%m$N>l9g(J, $@<!$N(Jvalues$@0z?t$OL5;k$5$l$k!#$J$*!"%^%9%/$H$7$F$O(J
$@0J2<$N$b$N$,;XDj$G$-$k!#(J
.RS
.IP XJP_PWIDTH 16
$@$+$J4A;zJQ49MW5a%&%#%s%I%&$NI}(J
.IP XJP_PHEIGHT 16
$@$+$J4A;zJQ49MW5a%&%#%s%I%&$N9b$5(J
.IP XJP_X 16
$@JQ49%&%#%s%I%&$N0LCV(J
.IP XJP_Y 16
$@JQ49%&%#%s%I%&$N0LCV(J
.IP XJP_WIDTH 16
$@JQ49%&%#%s%I%&$NI}(J
.IP XJP_HEIGHT 16
$@JQ49%&%#%s%I%&$N9b$5(J
.IP XJP_FG 16
$@J8;z$N?'(J
.IP XJP_BG 16
$@GX7J?'(J
.IP XJP_BP 16
$@JQ49%&%#%s%I%&$N6-3&I}(J
.IP XJP_FONTS 16
$@%U%)%s%HL>(J
.IP XJP_STATUS 16
$@%9%F!<%?%9I=<(NN0h$N0LCV!J(JXJp_open$@$G$OL5;k$5(J
$@$l$k!#(JXJp_change$@$N$H$-$N$_M-8z$G$"$k!#!K(J
.RE
.IP \fIvalues\fP 14
$@$3$N9=B$BN$KB0@-$r@_Dj$9$k!#(Jvaluemask$@$K$O(Jvalues$@$K(J
$@@_Dj$7$?B0@-$K3:Ev$9$k%S%C%H%^%9%/$r;XDj$7$J$1$l$P(J
$@$J$i$J$$!#$J$*!"$3$N9=B$BN$NMWAG$O0J2<$NDL$j$G$"$k!#(J
.RS
.IP long\ p_width; 20
$@$+$J4A;zJQ49MW5a%&%#%s%I%&$NI}!J%T%/%;%kC10L!K(J
.IP long\ p_height; 20
$@$+$J4A;zJQ49MW5a%&%#%s%I%&$N9b$5!J%T%/%;%kC10L!K(J
.IP long\ x; 20
$@JQ49%&%#%s%I%&$N0LCV!J%T%/%;%kC10L!K(J
.br
$@$?$@$7!":BI8$O$+$J4A;zJQ49MW5a%&%#%s%I%&$KBP$9$kAjBP:BI8(J
.IP long\ y; 20
$@JQ49%&%#%s%I%&$N0LCV!J%T%/%;%kC10L!K(J
.IP long\ width; 20
$@JQ49%&%#%s%I%&$NI}!J%T%/%;%kC10L!K(J
.IP long\ height; 20
$@JQ49%&%#%s%I%&$N9b$5!J%T%/%;%kC10L!K(J
.IP unsigned\ long\ fg; 20
$@JQ49%&%#%s%I%&$K$*$1$kJ8;z$N?'(J
.IP unsigned\ long\ bg; 20
$@JQ49%&%#%s%I%&$K$*$1$kGX7J?'(J
.IP long\ bp; 20
$@JQ49%&%#%s%I%&$N6-3&I}!J%T%/%;%kC10L!K(J
.IP XJp_fontInfo\ *fonts; 20
$@%U%)%s%H>pJs9=B$BN$NG[Ns$X$N%]%$%s%?(J
.IP int\ num_fonts; 20
$@%U%)%s%H>pJs9=B$BN$NG[Ns?t(J
.IP long\ status_x; 20
$@%9%F!<%?%9I=<(NN0h$N0LCV!J%T%/%;%kC10L!K(J
.IP long\ status_y; 20
$@%9%F!<%?%9I=<(NN0h$N0LCV!J%T%/%;%kC10L!K(J
.IP long\ status_width; 20
$@%9%F!<%?%9I=<(NN0h$NI}!J%T%/%;%kC10L!KJVCM(J
.IP long\ status_height; 20
$@%9%F!<%?%9I=<(NN0h$N9b$5!J%T%/%;%kC10L!KJVCM(J
.IP char\ *aux_data; 20
$@%U%m%s%H%(%s%IMQ%W%i%$%Y!<%H%G!<%?$X$N%]%$%s%?!#$?$@$7!"%G!<%?%5%$%:(J
$@$O(J128$@%P%$%H0JFb!"FbMF$O%U%m%s%H%(%s%I$K0MB8$9$k!#(J
.RE
.IP \fIdispmode\fP 14
$@$+$J4A;zJQ49%&%#%s%I%&$NI=<(%b!<%I$r0J2<$NCM$G;XDj(J
$@$9$k!#(J
.RS
.IP XJP_UNDER 16
$@$+$J4A;zJQ49%&%#%s%I%&$r;XDj$7$?%&%#%s%I%&$N2<It$K3+$/!#(J
.IP XJP_XY 16
$@$+$J4A;zJQ49%&%#%s%I%&$r;XDj$7$?%&%#%s%I%&$N;XDj$7$?0LCV$K3+$/!#(J
.IP XJP_ROOT 16
$@$+$J4A;zJQ49%&%#%s%I%&$r%k!<%H%&%#%s%I%&$N;XDj$7$?0LCV$K3+$/!#(J
.RE
.RE
.SH
$@@bL@(J
.RS
XJp_open$@$O!"$+$J4A;zJQ49%&%#%s%I%&$K4X$9$k=i4|2==hM}$r9T$&!#2>(J
$@L>4A;zJQ49$N<B9T$K@hN)$A!"I,$:8F$S=P$9I,MW$,$"$k!#DL>o$O!"3F%/(J
$@%i%$%"%s%H$NK\BN%&%#%s%I%&@8@.8e$K8F$S=P$9!#(J
.br
\fIdispmode\fP$@$K$h$C$F!"(J\fIvalues\fP$@$G;XDj$G$-$k$b$N$,(J
$@0[$J$k!JI=#1$r;2>H!K!#(J
.br
$@$^$?!"(J\fIdispmode\fP$@$K1~$8$F!"0J2<$N0c$$$,$"$k!#(J
.IP XJP_UNDER 16
$@%U%m%s%H%(%s%I$,:n@.$9$kJQ49%&%#%s%I%&$N$?$a$KI,MW$JNN0h$r(J
$@%/%i%$%"%s%HB&$G3NJ]$7$F$*$/I,MW$,$"$k!#%9%F!<%?%9I=<(NN0h$N(J
$@:n@.$K4X$7$F$O!"%/%i%$%"%s%HB&$G;XDj$9$kI,MW$O$J$$!#(J
.IP XJP_XY 16
$@K\4X?t$r%3!<%k$9$k$H!"%9%F!<%?%9I=<(NN0h$NI}!"9b$5$,(J\fIvalues\fP$@$N(J
\fIstatus_width,status_height\fP$@$K%;%C%H$5$l$F%j%?!<%s$5$l$k!#(J
$@%9%F!<%?%9I=<(NN0h$N0LCV$r;XDj$9$k>l9g$O!"(JXJp_open$@$N8e$K(J
XJp_change$@$r%3!<%k$9$kI,MW$,$"$k!#(J
.IP XJP_ROOT 16
$@JQ49%&%#%s%I%&!"%9%F!<%?%9I=<(NN0h$K4X$7$F%/%i%$%"%s%HB&$G$O(J
$@2?$b;XDj$9$kI,MW$O$J$$!#(J
.LP
\fIfonts\fP$@$K$O%U%)%s%H>pJs9=B$BN$NG[Ns$r;XDj$9$k!#(J
$@%U%)%s%H>pJs9=B$BN$NG[Ns$OI,MW$H$9$k%U%)%s%H$N?t$@$1MQ0U$9$k!#(J
$@9=B$BN$NMWAG(Jid$@$K$O0J2<$K<($9$b$N$,;XDj$G$-$k!#(J
.IP XJP_ascii 20
ASCII$@J8;z(J
.IP XJP_romanX0201 20
JIS-X0201$@%m!<%^J8;z(J
.IP XJP_kanaX0201 20
JIS0201$@%+%?%+%JJ8;z(J
.IP XJP_kanjiX0208 20
JIS-X0208$@4A;z(J
.IP XJP_gaiji1byte 20
$@#1%P%$%H30;z(J
.IP XJP_gaiji2byte 20
$@#2%P%$%H30;z(J
.RE
.sp 1
.SH
$@La$jCM(J
.RS
$@@5>o=*N;$N>l9g!"$+$J4A;zJQ49MW5a(JID$@$r!"0[>o=*N;$N>l9g(J\-1$@$rJV$9!#(J
.RE
.sp 1
.SH
$@!N%$%s%W%j%a%s%H>e$NCm0U!O(J
.RS
.IP (1) 4
$@%/%i%$%"%s%H$,;`$s$@;~$K$+$J4A;zJQ49%&%#%s%I%&$N;q8;$r2rJ|$9$k$?(J
$@$a$K!"(JStructureNotifyMask$@$G(JDestroyWindow $@%$%Y%s%H$rBT$D$3$H!#(J
.IP (2) 4
XJp_open$@$N%j%?!<%sCM$O$+$J4A;zJQ49%&%#%s%I%&(JID$@$H$9$k!#(J
.RE
.KS
.sp 3
.DS C
$@I=#1!%(JXJp_open$@$G$N(J\fIdispmode\fP$@$H(J\fIvaluemask\fP
.DE
.TS
center box tab(;);
c||c s s
c||c|c|c.
XJp_attrubutes;dispmode
;_;_;_
$@%a%s%P!<L>(J;XJP_UNDER;XJP_XY;XJP_ROOT
=
XJP_PWIDTH;$@!{(J;$@!{(J;$@!](J
_
XJP_PHEIGHT;$@!{(J;$@!{(J;$@!](J
_
XJP_X;$@#0(J;$@!{(J;$@!](J
_
XJP_Y;p_height-height;$@!{(J;$@!](J
_
XJP_WIDTH;p_width;$@!{(J;$@!](J
_
XJP_HEIGHT;$@%U%)%s%H$N9b$5(J;$@%U%)%s%H$N9b$5(J;$@!](J
_
XJP_FG;$@%U%m%s%H%(%s%I$N(J;$@%U%m%s%H%(%s%I$N(J;$@!](J
;$@%U%)%"%0%i%s%I(J;$@%U%)%"%0%i%s%I(J
_
XJP_BG;$@%U%m%s%H%(%s%I$N(J;$@%U%m%s%H%(%s%I$N(J;$@!](J
;$@%P%C%/%0%i%s%I(J;$@%P%C%/%0%i%s%I(J
_
XJP_BP;$@#1(J;$@#1(J;$@!](J
_
XJP_FONTS;$@!{(J;$@!{(J;$@!](J
_
XJP_STATUS;$@!](J;$@!](J;$@!](J
_
.TE
.RS
.RS
.RS
.nf
$@!{(J : $@I,$:@_Dj$7$J$1$l$P$$$1$J$$$b$N(J
$@!](J : $@@_Dj$7$F$bL58z$K$J$k$b$N(J
$@$3$l$i0J30$N$b$N$O!"@_Dj$5$l$J$$>l9g$N%G%U%)%k%HCM(J
.fi
.RE
.RE
.RE
.KE
.EH 'XJp_begin(3K)'XJP_library Manual'XJp_begin(3K)'
.OH 'XJp_begin(3K)'XJP_library Manual'XJp_begin(3K)'
.bp
.SH
$@L>>N(J
.RS
XJp_begin \- $@%-!<F~NO$N3+;O(J
.RE
.SH
$@5-=R7A<0(J
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
int XJp_begin(\fIrequest_id, w_key\fP)
.RS
.nf
.ft B
XJp_id \fIrequest_id\fP; 
Window \fIw_key\fP;
.ft R
.RE
.RE
.SH
$@0z?t(J
.RS
.IP \fIrequest_id\fP 14
$@$+$J4A;zJQ49MW5a(JID
.IP \fIw_key\fP 14
$@%-!<F~NO$r<u$1IU$1$k%&%#%s%I%&(JID
.RE
.SH
$@@bL@(J
.RS
XJp_begin$@$O!";XDj$7$?%&%#%s%I%&(Jw_key$@$+$i$N%-!<F~NO$rM-8z$K$9(J
$@$k!#(J
.RE
.SH
$@La$jCM(J
.RS
$@@5>o=*N;$N>l9g(J0$@$r!"0[>o=*N;$N>l9g(J\-1$@$rJV$9!#(J
.RE
.EH 'XJp_visible(3K)'XJP_library Manual'XJp_visible(3K)'
.OH 'XJp_visible(3K)'XJP_library Manual'XJp_visible(3K)'
.bp
.SH
$@L>>N(J
.RS
XJp_visible \- $@$+$J4A;zJQ49%&%#%s%I%&$NI=<((J
.RE
.SH
$@5-=R7A<0(J
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
int XJp_visible(\fIrequest_id\fP) 
.RS
.nf
.ft B
XJp_id \fIrequest_id\fP; 
.ft R
.RE
.RE
.SH
$@0z?t(J
.RS
.IP \fIrequest_id\fP 14
$@$+$J4A;zJQ49MW5a(JID
.RE
.SH
$@@bL@(J
.RS
XJp_visible $@$O!";XDj$7$?$+$J4A;zJQ49MW5a(JID$@$KBP1~$7$?$+$J4A;zJQ49(J
$@%&%#%s%I%&$rI=<($9$k!#(J
.br
$@$?$@$7!"(J\fIdispmode\fP$@$,(JXJP_ROOT$@$N>l9g$K$O!"K\4X?t$OL58z$G$"$k!#(J
.RE
.SH
$@La$jCM(J
.RS
$@@5>o=*N;$N>l9g(J0$@$r!"0[>o=*N;$N>l9g(J\-1$@$rJV$9!#(J
.RE
.EH 'XJp_read(3K)'XJP_library Manual'XJp_read(3K)'
.OH 'XJp_read(3K)'XJP_library Manual'XJp_read(3K)'
.bp
.SH
$@L>>N(J
.RS
XJp_read \- $@$+$J4A;zJQ49$N7k2L$r3MF@!J(J1$@!K(J
.RE
.SH
$@5-=R7A<0(J
.RS
.nf
.ft B
#include <X11/Xlib.h>
#include <X11/XJplib.h>
int XJp_read(\fIxEv, buffer, buffer_size, code_length, character_set\fP)
.RS
.nf
.ft B
XClientMessageEvent \fI*xEv\fP; 
unsigned char \fI*buffer\fP; 
int \fIbuffer_size\fP; 
int \fI*code_length\fP; 
char \fI*character_set\fP;
.ft R
.RE
.RE
.SH
$@0z?t(J
.RS
.IP \fIxEv\fP 14
$@$+$J4A;zJQ49$N3NDj%G!<%?$rDLCN$9$k%/%i%$%"%s%H%a%C%;!<%8!&%$%Y%s%H(J
.IP \fIbuffer\fP 14
$@JQ497k2L$rJV$9%P%C%U%!(J
.IP \fIbuffer_size\fP 14
$@%P%C%U%!$NBg$-$5!J(J17$@%P%$%H0J>e$r3NJ]$7$F$*$/$3$H!K(J
.IP \fIcode_length\fP 14
$@#1J8;z$N%3!<%ID9!J%P%$%H!K(J
.IP \fIcharacter_set\fP 14
$@3NDj%G!<%?$NJ8;z%;%C%H!J(J4$@%P%$%H0J>e$r3NJ]$7$F$*$/$3$H!K(J
.RS
.IP XJP_ascii 20
ASCII$@J8;z(J
.IP XJP_romanX0201 20
JIS\-X0201$@%m!<%^J8;z(J
.IP XJP_kanaX0201 20
JIS\-X0201$@%+%?%+%JJ8;z(J
.IP XJP_kanjiX0208 20
JIS\-X0208$@4A;z(J
.IP XJP_gaiji1byte 20
$@#1%P%$%H30;z(J
.IP XJP_gaiji2byte 20
$@#2%P%$%H30;z(J
.RE
.RE
.SH
$@@bL@(J
.RS
XJp_read$@$O!";XDj$7$?(Jbuffer$@$KJ8;z%;%C%H$4$H$KJQ497k2L$rJV$9!J$?$@$7!"(J
$@:G8e$O(JNULL$@!K!#$=$N:]!"(Jcode_length, character_set $@$K$O(Jbuffer$@$K(J
$@3JG<$5$l$F$$$k#1J8;z$N%3!<%ID9!"J8;z%;%C%H$N<oN`$,F~$C$F$/$k!#$3$N(J
$@4X?t$OF|K\8l%U%m%s%H%(%s%I$h$j!"$+$J4A;zJQ493NDj!J%/%i%$%"%s%H!&%a(J
$@%C%;!<%8!K$,DLCN$5$l$?8e$K8F$VI,MW$,$"$k!#(J
.RE
.SH
$@La$jCM(J
.RS
$@@5>o=*N;$N>l9g$O%P%C%U%!Fb$K3JG<$5$l$?(J\fB$@%G!<%?$NJ8;z?t(J\fP$@$r!"0[>o=*(J
$@N;$N>l9g$O(J\-1$@$rJV$9!#(J
.RE
.EH 'XJp_string(3K)'XJP_library Manual'XJp_string(3K)'
.OH 'XJp_string(3K)'XJP_library Manual'XJp_string(3K)'
.bp
.SH
$@L>>N(J
.RS
XJp_string \- $@$+$J4A;zJQ49$N7k2L$r3MF@!J(J2$@!K(J
.RE
.SH
$@5-=R7A<0(J
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
int XJp_string(\fIxEv, buffer, buffer_size, code\fP) 
.RS
.nf
.ft B
XClientMessageEvent \fI*xEv\fP; 
unsigned char \fI*buffer\fP; 
int \fIbuffer_size\fP; 
int \fIcode\fP; 
.ft R
.RE
.RE
.SH
$@0z?t(J
.RS
.IP \fIxEv\fP 14
$@$+$J4A;zJQ49$N3NDj%G!<%?$rDLCN$9$k%/%i%$%"%s%H%a%C%;!<%8!&%$%Y%s%H(J
.IP \fIbuffer\fP 14
$@JQ497k2L$rJV$9%P%C%U%!(J
.IP \fIbuffer_size\fP 14
$@%P%C%U%!$NBg$-$5!J(J33$@%P%$%H0J>e3NJ]$9$k$3$H!K(J
.IP \fIcode\fP
$@%3!<%I7O(J
.RS
.nf
.IP XJP_JIS7 16
JIS7$@C10L%3!<%I7O(J
.IP XJP_JIS8 16
JIS8$@C10L%3!<%I7O(J
.IP XJP_EUC 16
EUC$@%3!<%I(J
.IP XJP_SJIS 16
$@%7%U%H(JJIS$@%3!<%I(J
.RE
.RE
.SH
$@@bL@(J
.RS
XJp_string$@$O!";XDj$7$?(Jbuffer$@$K(Jcode$@$G;XDj$5$l$?%3!<%I7O$G(J
$@JQ497k2L$rJV$9!J$?$@$7!":G8e$O(JNULL$@!K!#$3$N4X?t$OF|K\8l(J
$@%U%m%s%H%(%s%I$h$j!"$+$J4A;zJQ493NDj!J%/%i%$%"%s%H!&%a%C%;!<%8!K$,(J
$@DLCN$5$l$?8e$K8F$VI,MW$,$"$k!#(J
.RE
.SH
$@La$jCM(J
.RS
$@@5>o=*N;$N>l9g$O%P%C%U%!Fb$K3JG<$5$l$?(J\fB$@%G!<%?$N%P%$%H?t(J\fP$@$r!"0[>o(J
$@=*N;$N>l9g$O(J\-1$@$rJV$9!#(J
.RE
.EH 'XJp_invisible(3K)'XJP_library Manual'XJp_invisible(3K)'
.OH 'XJp_invisible(3K)'XJP_library Manual'XJp_invisible(3K)'
.bp
.SH
$@L>>N(J
.RS
XJp_invisible \- $@$+$J4A;zJQ49%&%#%s%I%&$NHsI=<((J
.RE
.SH
$@5-=R7A<0(J
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
int XJp_invisible(\fIrequest_id\fP)
.RS
.nf
.ft B
XJp_id \fIrequest_id\fP; 
.ft R
.RE
.RE
.SH
$@0z?t(J
.RS
.IP \fIrequest_id\fP 14
$@$+$J4A;zJQ49MW5a(JID
.RE
.SH
$@@bL@(J
.RS
XJp_invisible$@$O!"$+$J4A;zJQ49%&%#%s%I%&$rHsI=<($K$9$k!#(J
.br
$@$?$@$7!"K\4X?t$O(J\fIdispmode\fP$@$,(JXJP_ROOT$@$N>l9g$K$OL58z$G$"$k!#(J
.RE
.SH
$@La$jCM(J
.RS
$@@5>o=*N;;~$K$O(J0$@$r!"0[>o=*N;;~$K$O(J\-1$@$rJV$9!#(J
.RE
.EH 'XJp_end(3K)'XJP_library Manual'XJp_end(3K)'
.OH 'XJp_end(3K)'XJP_library Manual'XJp_end(3K)'
.bp
.SH
$@L>>N(J
.RS
XJp_end \- $@%-!<F~NO$N=*N;(J
.RE
.SH
$@5-=R7A<0(J
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
int XJp_end(\fIrequest_id, w_key\fP)
.RS
.nf
.ft B
XJp_id \fIrequest_id\fP; 
Window \fIw_key\fP;
.ft R
.RE
.RE
.SH
$@0z?t(J
.RS
.IP \fIrequest_id\fP 14
$@$+$J4A;zJQ49MW5a(JID
.IP \fIw_key\fP 14
$@%-!<F~NO$r<u$1IU$1$k%&%#%s%I%&(JID
.RE
.SH
$@@bL@(J
.RS
XJp_end$@$O!";XDj$7$?%&%#%s%I%&(Jw_key$@$+$i$N%-!<F~NO$rL58z$K$9$k!#(J
.RE
.SH
$@La$jCM(J
.RS
$@@5>o=*N;;~$K$O(J0$@$r!"0[>o=*N;;~$K$O(J\-1$@$rJV$9!#(J
.RE
.EH 'XJp_close(3K)'XJP_library Manual'XJp_close(3K)'
.OH 'XJp_close(3K)'XJP_library Manual'XJp_close(3K)'
.bp
.SH
$@L>>N(J
.RS
XJp_close \- $@$+$J4A;zJQ49MW5a$N=*N;(J
.RE
.SH
$@5-=R7A<0(J
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
int XJp_close(\fIrequest_id\fP)
.RS
.nf
.ft B
XJp_id \fIrequest_id\fP; 
.ft R
.RE
.RE
.SH
$@0z?t(J
.RS
.IP \fIrequest_id\fP 14
$@$+$J4A;zJQ49MW5a(JID
.RE
.SH
$@@bL@(J
.RS
XJp_close $@$O!"$+$J4A;zJQ49MW5a(Jrequest_ID$@$K$h$kJQ49$r=*N;$9$k!#(J
.RE
.SH
$@La$jCM(J
.RS
$@@5>o=*N;;~$K$O(J0$@$r!"0[>o=*N;;~$K$O(J\-1$@$rJV$9!#(J
.RE
.EH 'XJp_move(3K)'XJP_library Manual'XJp_move(3K)'
.OH 'XJp_move(3K)'XJP_library Manual'XJp_move(3K)'
.bp
.SH
$@L>>N(J
.RS
XJp_move \- $@JQ49%&%#%s%I%&$N0\F0(J
.RE
.SH
$@5-=R7A<0(J
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
int XJp_move(\fIrequest_id, x, y\fP)
.RS
.nf
.ft B
XJp_id \fIrequest_id\fP;
int \fIx, y\fP;
.ft R
.RE
.RE
.SH
$@0z?t(J
.RS
.IP \fIrequest_id\fP 14
$@$+$J4A;zJQ49MW5a(JID
.IP \fIx,\ y\fP 14
$@$+$J4A;zJQ49%&%#%s%I%&$rI=<($9$k0LCV$r!"$+$J4A;zJQ49MW5a%&(J
$@%#%s%I%&:8>e$+$i$N%T%/%;%kC10L$G;XDj$9$k!#(J
.RE
.SH
$@@bL@(J
.RS
XJp_move$@$O!"(JXJP$@%U%m%s%H%(%s%I$KBP$7!"4{$KAw=P$7$?(JXJp_open$@!"$^(J
$@$?$OD>A0$KH/9T$7$?(JXJp_move$@$G@_Dj$7$?$+$J4A;zJQ49%&%#%s%I%&$NB0(J
$@@-$rJQ99$9$k!#(J
.br
$@$3$N%i%$%V%i%j$O$+$J4A;zJQ49%&%#%s%I%&$N(Jdispmode$@$,(JXJP_XY$@$N;~$N(J
$@$_M-8z$G$"$k!#(J
.RE
.SH
$@La$jCM(J
.RS
$@@5>o=*N;;~$K$O(J0$@$r!"0[>o=*N;;~$K$O(J\-1$@$rJV$9!#(J
.RE
.EH 'XJp_change(3K)'XJP_library Manual'XJp_change(3K)'
.OH 'XJp_change(3K)'XJP_library Manual'XJp_change(3K)'
.bp
.SH
$@L>>N(J
.RS
XJp_change \- $@#X#J#P%U%m%s%H%(%s%I$KBP$9$k@)8f(J
.RE
.SH
$@5-=R7A<0(J
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
int XJp_change(\fIrequest_id, valuemask, values\fP)
.RS
.nf
.ft B
XJp_id \fIrequest_id\fP;
unsigned long \fIvaluemask\fP;
XJp_attributes \fI*values\fP;
.ft R
.RE
.RE
.SH
$@0z?t(J
.RS
.IP \fIrequest_id\fP 14
$@$+$J4A;zJQ49MW5a(JID
.IP \fIvaluemask\fP 14
values$@$GDj5A$9$kB0@-$r;XDj$9$k!#$3$N%^%9%/$OM-(J
$@8z$JB0@-$N%S%C%H%^%9%/$N%S%C%H(JOR$@$G$"$k!#(Jvalue
mask $@$,%<%m$N>l9g(J, $@<!$N(Jvalues$@0z?t$OL5;k$5$l$k!#(J
$@%S%C%H%^%9%/$N>\:Y$K$D$$$F$O!"(JXJp_open$@$r;2>H!#(J
.IP \fIvalues\fP 14
$@$3$N9=B$BN$KJQ99$7$?$$B0@-$r@_Dj$9$k!#(Jvaluemask
$@$K$O(Jvalues$@$K@_Dj$7$?B0@-$K3:Ev$9$k%S%C%H%^%9%/(J
$@$r;XDj$7$J$1$l$P$J$i$J$$!#$3$N9=B$BN$N>\:Y$K$D$$$F$O!"(J
XJp_open$@$r;2>H!#(J
.RE
.SH
$@@bL@(J
.RS
XJp_change$@$O!"#X#J#P%U%m%s%H%(%s%I$KBP$7!"4{$KAw=P$7$?(JXJp_open$@!"(J
$@$^$?$OD>A0$KH/9T$7$?(JXJp_change$@$G@_Dj$7$?$+$J4A;zJQ49%&%#%s%I%&(J
$@$NB0@-$rJQ99$9$k!#(J
.br
XJp_open$@$7$?;~$N(J\fIdispmode\fP$@$K$h$C$F(J
\fIvalues\fP$@$G;XDj$G$-$k$b$N$,0c$C$F$/$k!JI=#2$r;2>H!K!#(J
$@$^$?!"%9%F!<%?%9$NI=<($O!"(JXJp_open$@$N9`$G@bL@$7$?$h$&$K!"(J
XJp_open$@$G<hF@$7$?(J\fIstatus_width,status_height\fP$@$K4p$E$$$F(J
$@K\4X?t$G(J\fIstatus_x,status_y,status_width,status_height\fP$@$r(J
$@;XDj$9$k!#$?$@$7!"%9%F!<%?%9$NI=<(;XDj$O!"(J\fIdispmode\fP$@$,(J
XJP_XY$@$N>l9g$N$_M-8z$G$"$k!#(J
.RE
.SH
$@La$jCM(J
.RS
$@@5>o=*N;;~$K$O(J0$@$r!"0[>o=*N;;~$K$O(J\-1$@$rJV$9!#(J
.RE
.KS
.sp 3
.DS C
$@I=#2!%(JXJp_change$@$K$*$1$k(J\fIdispmode\fP$@$H(J\fIvaluemask\fP
.DE
.TS
center box tab(;);
c||c s s
c||c|c|c.
XJp_attrubutes;dispmode
;_;_;_
$@%a%s%P!<L>(J;XJP_UNDER;XJP_XY;XJP_ROOT
=
XJP_PWIDTH;$@!{(J;$@!{(J;$@!](J
_
XJP_PHEIGHT;$@!{(J;$@!{(J;$@!](J
_
XJP_X;$@!](J;$@!{(J;$@!](J
_
XJP_Y;$@!](J;$@!{(J;$@!](J
_
XJP_WIDTH;$@!](J;$@!{(J;$@!](J
_
XJP_HEIGHT;$@!{(J;$@!{(J;$@!](J
_
XJP_FG;$@!{(J;$@!{(J;$@!](J
_
XJP_BG;$@!{(J;$@!{(J;$@!](J
_
XJP_BP;$@!{(J;$@!{(J;$@!](J
_
XJP_FONTS;$@!_(J;$@!_(J;$@!](J
_
XJP_STATUS;$@!](J;$@!{(J($@Cm(J);$@!](J
_
.TE
.RS
.RS
.RS
.nf
$@!{(J : $@JQ992DG=$J$b$N(J
$@!_(J : $@JQ99$G$-$J$$$b$N(J
$@!](J : $@JQ99$7$F$bL5;k$5$l$k$b$N(J
.fi
.sp 1
.IP ($@Cm(J) 4
dispmode $@$,(J XJP_XY $@$G(J XJP_STATUS $@$r@_Dj$7$J$+$C$?>l9g!"%9%F!<%?%9(J
$@I=<(NN0h$N0LCV$O%U%m%s%H%(%s%I$K0MB8$7$?$b$N$H$J$k!#(J
.RE
.RE
.RE
.KE
.EH 'XJp_error(3K)'XJP_library Manual'XJp_error(3K)'
.OH 'XJp_error(3K)'XJP_library Manual'XJp_error(3K)'
.bp
.SH
$@L>>N(J
.RS
XJp_error \- $@%(%i!<%a%C%;!<%8$N<h$j=P$7(J
.RE
.SH
$@5-=R7A<0(J
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
char *XJp_error()
.RE
.SH
$@@bL@(J
.RS
XJp_error$@$O!":G?7$N%3%_%e%K%1!<%7%g%s%-%C%H$N%(%i!<%a%C%;!<%8(J
$@!J1QJ8!K$rJV$9!#(J
$@%(%i!<HV9f$O30It(Jint$@JQ?t(JXJp_errno$@$K@_Dj$5$l$F$$$kCM$r;H$&!#(J
XJp_errno$@$NCM$O%3%_%e%K%1!<%7%g%s%-%C%H$G%(%i!<$,H/@8$7$?;~$K(J
$@@_Dj$5$l!"%(%i!<$G$J$$;~$O99?7$5$l$J$$!#(J
.RE
.SH
$@La$jCM(J
.RS
$@@5>o=*N;;~$K$O%(%i!<%a%C%;!<%8J8;zNs$X$N%]%$%s%?$r!"0[>o=*N;(J
$@;~!J%(%i!<HV9f$KBP1~$9$k%(%i!<%a%C%;!<%8$,$J$$$J$I!K$K$O(JNULL$@$rJV$9!#(J
.RE
.sp 1
.EH 'XJp_reset(3K)'XJP_library Manual'XJp_reset(3K)'
.OH 'XJp_reset(3K)'XJP_library Manual'XJp_reset(3K)'
.bp
.SH
$@L>>N(J
.RS
XJp_reset \- XJP$@%i%$%V%i%j$N%j%;%C%H(J
.RE
.SH
$@5-=R7A<0(J
.RS
.nf
.ft B
#include <X11/Xlib.h> 
#include <X11/XJplib.h> 
XJp_reset(\fIevent\fP)
.RS
.nf
.ft B
XEvent *\fIevent\fP;
.ft R
.RE
.RE
.SH
$@0z?t(J
.RS
.IP \fIevent\fP 14
$@DLCN$5$l$?(JDestroyNotify$@%$%Y%s%H(J
.RE
.SH
$@@bL@(J
.RS
XJp_reset$@$O!"(JXJP$@%i%$%V%i%j$r%j%;%C%H$7$F(JXJp_open$@$,(J
$@%3!<%k$5$l$kA0$N>uBV!J(JSelectInputMask$@$NCM$J$I!K$K$9$k!#(J
.br
$@$J$*!"K\4X?t$O(JDestroyNotify$@%$%Y%s%H$KBP$7$F$N$_M-8z$G!"B>$N(J
$@%$%Y%s%H$KBP$7$F$O%(%i!<%j%?!<%s$9$k!#(J
.RE
.SH
$@La$jCM(J
.RS
$@@5>o=*N;;~$K$O(J0$@$r!"0[>o=*N;;~$K$O(J\$@$rJV$9!#(J
.RE
.sp 1
.ta 4,8,12,16,20,24
.nr PI 2n
/* $@%(%i!<=hM}$N%3!<%G%#%s%0Nc!J0z?t$J$I$O>JN,!K(J */
.sp 1
.RS
.nf
main()
{
	Window wid;
	XJp_id request_id;

	wid = XCreateWindow(....);	/* $@$+$J4A;zJQ49MW5a%&%#%s%I%&(J */
	XSelectInput(display, wid, KeyPressMask);
	request_id = XJp_open(....);	/* $@$+$J4A;zJQ49%&%#%s%I%&$N=i4|2=(J */
	XSelectInput(display, request_id, StructureNotifyMask);
	.....
	XJp_begin(....);		/* $@%-!<F~NO$N3+;O(J */
	XJp_visible(....);		/* $@$+$J4A;zJQ49%&%#%s%I%&$NI=<((J */
	.....
	while(1) {
		XEvent event;

		XNextEvent(&event);
		switch(event.type) {
		case DestroyNotify:
			if (event.XDestroyWindowEvent.window == request_id) {
				/* $@%&%#%s%I%&$N%A%'%C%/(J */
				XJp_reset(&event);	/* XJP$@$N%j%;%C%H(J */
				fprintf(stderr, "XJP$@%U%m%s%H%(%s%I$,0[>o=*N;$7$^$7$?!#(J\\n");
				....
			}
			break;
		.....
		}
	}
}
.RE
.EH ''''
.OH ''''
.bp
.nr PI 2n
.NH 2
$@4J0W%i%$%V%i%j(J
.PP
$@4J0W%i%$%V%i%j$r;HMQ$9$k>l9g$K$O!"0J2<$N$3$H$,I,MW$G(J
$@$"$k!#(J
.IP (1) 4
/usr/lib/libXJp.a
.PP
$@4J0W%i%$%V%i%j$rAH$_9~$s$@%W%m%0%i%`$r%j%s%/$9$k>l9g(J
$@$O0J2<$N%*%W%7%g%s$r;XDj$7$J$1$l$P$J$i$J$$!#(J
.br
.ta 6
	\-lXJp
.sp 3
.LP
$@0J2<$K%i%$%V%i%j4X?t$N>\:Y$r<($9!#(J
.EH 'XLookupKanjiString(3K)'XJP_library'XLookupKanjiString(3K)'
.OH 'XLookupKanjiString(3K)'XJP_library'XLookupKanjiString(3K)'
.bp
.nr PI 8n
.SH
$@L>>N(J
.RS
XLookupKanjiString \- $@%-!<%\!<%IF~NO$N4A;zJ8;zNs$X$NJQ49(J
.RE
.SH
$@5-=R7A<0(J
.RS
.nf
.ft B
#include <X11/XJplib.h>
int XLookupKanjiString(\fIevent_struct, buffer_return, bytes_buffer,\fP
             $@!!!!!!(J\fIkeysym_return, status_return\fP)
.RS
.nf
.ft B
XKeyEvent \fI*event_struct\fP;
char \fI*buffer_return\fP;
int \fIbytes_buffer\fP;
KeySym \fI*keysym_return\fP;
XComposeStatus \fI*status_return\fP;
.ft R
.RE
.RE
.SH
$@0z?t(J
.RS
.IP \fIevent_struct\fP 16
$@%-!<%$%Y%s%H$r;XDj$9$k!#(J
.IP \fIbuffer_return\fP 16
$@JQ49$7$?4A;zJ8;zNs$,JV$5$l$k!#(J
.IP \fIbytes_buffer\fP 16
$@%P%C%U%!$ND9$5$r;XDj$9$k!#(J
.IP \fIkeysym_return\fP 16
$@%-!<%7%s%\%k$rJV$9!#(JNULL$@$r;XDj$9$k$3$H$b2D!#(J
.IP \fIstatus_return\fP 16
XCompose$@9=B$BN$X$N%]%$%s%?$r;XDj$9$k!#!J8=:_$O;H$o$l$F$$$J$$!K(J
.RE
.SH
$@@bL@(J
.RS
\fIXLookupKanjiString\fP$@$O!"%-!<%$%Y%s%H$rF|K\8lJ8;zNs$K(J
$@BP1~$5$;$k4J0W%k!<%A%s$G!"(J
$@%-!<%$%Y%s%H$N%b%G%#%U%!%$%"%S%C%H$r;HMQ$7$F%7%U%H!"%m%C%/!"(J
$@%3%s%H%m!<%kEy$N=hM}$r9T$&!#(J
.br
\fIXLookupKanjiString\fP$@$K$h$C$F(J8bit$@%"%k%U%!%Y%C%HJ8;z5Z$S(J
16bit$@F|K\8lJ8;z$r<h$j07$&$3$H$,$G$-$k!#(J
.br
$@%"%k%U%!%Y%C%HJ8;z$N<h$j07$$$K$D$$$F$O!"(J
\fIXLookupKanjiString\fP$@$O(J\fIXLookupString\fP$@$HF1MM$NF0:n$K$J$k!#(J
.br
$@F|K\8lJ8;z$N<h$j07$$$K$D$$$F$O!"(J
$@$3$N4X?t$NFbIt$K$*$$$F%m!<%^;z$+$JJQ49!"(J
$@$+$J4A;zJQ49$,9T$o$l$k!#JQ49$,3NDj$7$?$H$-$N$_$K(J
$@%P%C%U%!$KJQ49$5$l$?(JEUC$@J8;zNs$,3JG<$5$l!"(J
$@$=$NJ8;zNs$ND9$5$,%P%$%HC10L$GJV$5$l$k!#$=$N$H$-(J
KeySym$@$H$7$F$O(JNoSymbol$@$,JV$5$l$k!#(J
.RE
.SH
$@La$jCM(J
.RS
$@%-!<%$%Y%s%H$K(J
$@$h$C$FJQ49$,3NDj$5$l$J$$>l9g$K$O%P%C%U%!$K$O(J
$@2?$b3JG<$5$l$:!"La$jCM$H$7$FD9$5#0$,JV$5$l$k!#(J
.br
$@%(%i!<$,8!=P$5$l$?>l9g$K$O!"La$jCM$H$7$F(J\-1$@$,JV$j!"(JXJp_errno$@$K(J
$@%(%i!<HV9f$,%;%C%H$5$l$k!#(J
.RE
.SH
$@;HMQNc(J
.RS
.nf
    XNextEvent ( dpy, &ev );   /* $@%$%Y%s%H$r$H$k!!(J*/
    switch ( ev.xany.type )
    {
    case Expose:
            proc_expose ( &ev );
            break;
    case KeyPress:             /* $@%-!<2!2<!!(J*/
            proc_keypress ( &ev );
            break;
    case ButtonPress:
            proc_buttonpress ( &ev );
            break;
    default:
            UnknownProc ( &ev );
    }

.......

proc_keypress ( ev )
    XKeyEvent *ev;
{
        char            buffer[BUFFERSIZE];
        int             len;
        KeySym          ks;
        XComposeStatus  xcs;

        len = \fIXLookupKanjiString\fP ( ev, buffer, BUFFERSIZE,
                                   &ks, &xcs);
        if ( len > 0 ) $@!!(J/*$@!!(Jlen$@$,@5$J$i$PJ8;zNs$,JV$C$F$-$F$$$k!!(J*/
                output_string ( buffer, len );
}
.RE
