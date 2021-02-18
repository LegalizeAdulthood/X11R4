/* -*-C-*-
********************************************************************************
*
* File:         w_funtab.h
* RCS:          $Header: w_funtab.h,v 1.2 89/11/25 06:25:36 mayer Exp $
* Description:  Indexes (pointers) info functiontable.... there should be a
*               better way, but I didn't want to mess with XLISP.
* Author:       Niels Mayer, HPLabs
* Created:      Fri Nov 24 00:00:00 1989
* Modified:     Sat Nov 25 06:25:27 1989 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       X11r4 contrib tape release
*
* WINTERP 1.0 Copyright 1989 Hewlett-Packard Company (by Niels Mayer).
* XLISP version 2.1, Copyright (c) 1989, by David Betz.
*
* Permission to use, copy, modify, distribute, and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of Hewlett-Packard and David Betz not be
* used in advertising or publicity pertaining to distribution of the software
* without specific, written prior permission.  Hewlett-Packard and David Betz
* make no representations about the suitability of this software for any
* purpose. It is provided "as is" without express or implied warranty.
*
* HEWLETT-PACKARD AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
* IN NO EVENT SHALL HEWLETT-PACKARD NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
* INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
* LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
* OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*
* See ./winterp/COPYRIGHT for information on contacting the authors.
* 
* Please send modifications, improvements and bugfixes to mayer@hplabs.hp.com
* Post XLISP-specific questions/information to the newsgroup comp.lang.lisp.x
*
********************************************************************************
*/


extern LVAL rmhash();
extern LVAL rmquote();
extern LVAL rmdquote();
extern LVAL rmbquote();
extern LVAL rmcomma();
extern LVAL rmlpar();
extern LVAL rmrpar();
extern LVAL rmsemi();
extern LVAL xnotimp();
extern LVAL clnew();
extern LVAL clisnew();
extern LVAL clanswer();
extern LVAL obisnew();
extern LVAL obclass();
extern LVAL obshow();

/*
  note:
  	LAST_FUNTAB_POINTER_USED_BY_libXlisp is set in xlisp.h... we assume
  that the list of "funtab pointers" used by libXlisp.a (see FT_OBSHOW) is 
  kept up to date with the elements of w_funtab.c:funtab[].
	FTAB_blank* are just placeholders for the xlnotimps() in xlftab[].
   */
#define FTAB_blank1 LAST_FUNTAB_POINTER_USED_BY_libXlisp + 1
#define FTAB_blank2 FTAB_blank1 + 1
#define FTAB_blank3 FTAB_blank2 + 1
#define FTAB_blank4 FTAB_blank3 + 1

extern LVAL xeval();
#define FTAB_xeval FTAB_blank4 + 1

extern LVAL xapply();
#define FTAB_xapply FTAB_xeval + 1

extern LVAL xfuncall();
#define FTAB_xfuncall FTAB_xapply + 1

extern LVAL xquote();
#define FTAB_xquote FTAB_xfuncall + 1

extern LVAL xfunction();
#define FTAB_xfunction FTAB_xquote + 1

extern LVAL xbquote();
#define FTAB_xbquote FTAB_xfunction + 1

extern LVAL xlambda();
#define FTAB_xlambda FTAB_xbquote + 1

extern LVAL xset();
#define FTAB_xset FTAB_xlambda + 1

extern LVAL xsetq();
#define FTAB_xsetq FTAB_xset + 1

extern LVAL xsetf();
#define FTAB_xsetf FTAB_xsetq + 1

extern LVAL xdefun();
#define FTAB_xdefun FTAB_xsetf + 1

extern LVAL xdefmacro();
#define FTAB_xdefmacro FTAB_xdefun + 1

extern LVAL xgensym();
#define FTAB_xgensym FTAB_xdefmacro + 1

extern LVAL xmakesymbol();
#define FTAB_xmakesymbol FTAB_xgensym + 1

extern LVAL xintern();
#define FTAB_xintern FTAB_xmakesymbol + 1

extern LVAL xsymname();
#define FTAB_xsymname FTAB_xintern + 1

extern LVAL xsymvalue();
#define FTAB_xsymvalue FTAB_xsymname + 1

extern LVAL xsymplist();
#define FTAB_xsymplist FTAB_xsymvalue + 1

extern LVAL xget();
#define FTAB_xget FTAB_xsymplist + 1

extern LVAL xputprop();
#define FTAB_xputprop FTAB_xget + 1

extern LVAL xremprop();
#define FTAB_xremprop FTAB_xputprop + 1

extern LVAL xhash();
#define FTAB_xhash FTAB_xremprop + 1

extern LVAL xmkarray();
#define FTAB_xmkarray FTAB_xhash + 1

extern LVAL xaref();
#define FTAB_xaref FTAB_xmkarray + 1

extern LVAL xcar();
#define FTAB_xcar FTAB_xaref + 1

extern LVAL xcdr();
#define FTAB_xcdr FTAB_xcar + 1

extern LVAL xcaar();
#define FTAB_xcaar FTAB_xcdr + 1

extern LVAL xcadr();
#define FTAB_xcadr FTAB_xcaar + 1

extern LVAL xcdar();
#define FTAB_xcdar FTAB_xcadr + 1

extern LVAL xcddr();
#define FTAB_xcddr FTAB_xcdar + 1

extern LVAL xcaaar();
#define FTAB_xcaaar FTAB_xcddr + 1

extern LVAL xcaadr();
#define FTAB_xcaadr FTAB_xcaaar + 1

extern LVAL xcadar();
#define FTAB_xcadar FTAB_xcaadr + 1

extern LVAL xcaddr();
#define FTAB_xcaddr FTAB_xcadar + 1

extern LVAL xcdaar();
#define FTAB_xcdaar FTAB_xcaddr + 1

extern LVAL xcdadr();
#define FTAB_xcdadr FTAB_xcdaar + 1

extern LVAL xcddar();
#define FTAB_xcddar FTAB_xcdadr + 1

extern LVAL xcdddr();
#define FTAB_xcdddr FTAB_xcddar + 1

extern LVAL xcaaaar();
#define FTAB_xcaaaar FTAB_xcdddr + 1

extern LVAL xcaaadr();
#define FTAB_xcaaadr FTAB_xcaaaar + 1

extern LVAL xcaadar();
#define FTAB_xcaadar FTAB_xcaaadr + 1

extern LVAL xcaaddr();
#define FTAB_xcaaddr FTAB_xcaadar + 1

extern LVAL xcadaar();
#define FTAB_xcadaar FTAB_xcaaddr + 1

extern LVAL xcadadr();
#define FTAB_xcadadr FTAB_xcadaar + 1

extern LVAL xcaddar();
#define FTAB_xcaddar FTAB_xcadadr + 1

extern LVAL xcadddr();
#define FTAB_xcadddr FTAB_xcaddar + 1

extern LVAL xcdaaar();
#define FTAB_xcdaaar FTAB_xcadddr + 1

extern LVAL xcdaadr();
#define FTAB_xcdaadr FTAB_xcdaaar + 1

extern LVAL xcdadar();
#define FTAB_xcdadar FTAB_xcdaadr + 1

extern LVAL xcdaddr();
#define FTAB_xcdaddr FTAB_xcdadar + 1

extern LVAL xcddaar();
#define FTAB_xcddaar FTAB_xcdaddr + 1

extern LVAL xcddadr();
#define FTAB_xcddadr FTAB_xcddaar + 1

extern LVAL xcdddar();
#define FTAB_xcdddar FTAB_xcddadr + 1

extern LVAL xcddddr();
#define FTAB_xcddddr FTAB_xcdddar + 1

extern LVAL xcons();
#define FTAB_xcons FTAB_xcddddr + 1

extern LVAL xlist();
#define FTAB_xlist FTAB_xcons + 1

extern LVAL xappend();
#define FTAB_xappend FTAB_xlist + 1

extern LVAL xreverse();
#define FTAB_xreverse FTAB_xappend + 1

extern LVAL xlast();
#define FTAB_xlast FTAB_xreverse + 1

extern LVAL xnth();
#define FTAB_xnth FTAB_xlast + 1

extern LVAL xnthcdr();
#define FTAB_xnthcdr FTAB_xnth + 1

extern LVAL xmember();
#define FTAB_xmember FTAB_xnthcdr + 1

extern LVAL xassoc();
#define FTAB_xassoc FTAB_xmember + 1

extern LVAL xsubst();
#define FTAB_xsubst FTAB_xassoc + 1

extern LVAL xsublis();
#define FTAB_xsublis FTAB_xsubst + 1

extern LVAL xremove();
#define FTAB_xremove FTAB_xsublis + 1

extern LVAL xlength();
#define FTAB_xlength FTAB_xremove + 1

extern LVAL xmapc();
#define FTAB_xmapc FTAB_xlength + 1

extern LVAL xmapcar();
#define FTAB_xmapcar FTAB_xmapc + 1

extern LVAL xmapl();
#define FTAB_xmapl FTAB_xmapcar + 1

extern LVAL xmaplist();
#define FTAB_xmaplist FTAB_xmapl + 1

extern LVAL xrplca();
#define FTAB_xrplca FTAB_xmaplist + 1

extern LVAL xrplcd();
#define FTAB_xrplcd FTAB_xrplca + 1

extern LVAL xnconc();
#define FTAB_xnconc FTAB_xrplcd + 1

extern LVAL xdelete();
#define FTAB_xdelete FTAB_xnconc + 1

extern LVAL xatom();
#define FTAB_xatom FTAB_xdelete + 1

extern LVAL xsymbolp();
#define FTAB_xsymbolp FTAB_xatom + 1

extern LVAL xnumberp();
#define FTAB_xnumberp FTAB_xsymbolp + 1

extern LVAL xboundp();
#define FTAB_xboundp FTAB_xnumberp + 1

extern LVAL xnull();
#define FTAB_xnull FTAB_xboundp + 1

extern LVAL xlistp();
#define FTAB_xlistp FTAB_xnull + 1

extern LVAL xconsp();
#define FTAB_xconsp FTAB_xlistp + 1

extern LVAL xminusp();
#define FTAB_xminusp FTAB_xconsp + 1

extern LVAL xzerop();
#define FTAB_xzerop FTAB_xminusp + 1

extern LVAL xplusp();
#define FTAB_xplusp FTAB_xzerop + 1

extern LVAL xevenp();
#define FTAB_xevenp FTAB_xplusp + 1

extern LVAL xoddp();
#define FTAB_xoddp FTAB_xevenp + 1

extern LVAL xeq();
#define FTAB_xeq FTAB_xoddp + 1

extern LVAL xeql();
#define FTAB_xeql FTAB_xeq + 1

extern LVAL xequal();
#define FTAB_xequal FTAB_xeql + 1

extern LVAL xcond();
#define FTAB_xcond FTAB_xequal + 1

extern LVAL xcase();
#define FTAB_xcase FTAB_xcond + 1

extern LVAL xand();
#define FTAB_xand FTAB_xcase + 1

extern LVAL xor();
#define FTAB_xor FTAB_xand + 1

extern LVAL xlet();
#define FTAB_xlet FTAB_xor + 1

extern LVAL xletstar();
#define FTAB_xletstar FTAB_xlet + 1

extern LVAL xif();
#define FTAB_xif FTAB_xletstar + 1

extern LVAL xprog();
#define FTAB_xprog FTAB_xif + 1

extern LVAL xprogstar();
#define FTAB_xprogstar FTAB_xprog + 1

extern LVAL xprog1();
#define FTAB_xprog1 FTAB_xprogstar + 1

extern LVAL xprog2();
#define FTAB_xprog2 FTAB_xprog1 + 1

extern LVAL xprogn();
#define FTAB_xprogn FTAB_xprog2 + 1

extern LVAL xgo();
#define FTAB_xgo FTAB_xprogn + 1

extern LVAL xreturn();
#define FTAB_xreturn FTAB_xgo + 1

extern LVAL xdo();
#define FTAB_xdo FTAB_xreturn + 1

extern LVAL xdostar();
#define FTAB_xdostar FTAB_xdo + 1

extern LVAL xdolist();
#define FTAB_xdolist FTAB_xdostar + 1

extern LVAL xdotimes();
#define FTAB_xdotimes FTAB_xdolist + 1

extern LVAL xcatch();
#define FTAB_xcatch FTAB_xdotimes + 1

extern LVAL xthrow();
#define FTAB_xthrow FTAB_xcatch + 1

extern LVAL xerror();
#define FTAB_xerror FTAB_xthrow + 1

extern LVAL xcerror();
#define FTAB_xcerror FTAB_xerror + 1

extern LVAL xbreak();
#define FTAB_xbreak FTAB_xcerror + 1

extern LVAL xcleanup();
#define FTAB_xcleanup FTAB_xbreak + 1

extern LVAL xtoplevel();
#define FTAB_xtoplevel FTAB_xcleanup + 1

extern LVAL xcontinue();
#define FTAB_xcontinue FTAB_xtoplevel + 1

extern LVAL xerrset();
#define FTAB_xerrset FTAB_xcontinue + 1

extern LVAL xbaktrace();
#define FTAB_xbaktrace FTAB_xerrset + 1

extern LVAL xevalhook();
#define FTAB_xevalhook FTAB_xbaktrace + 1

extern LVAL xfix();
#define FTAB_xfix FTAB_xevalhook + 1

extern LVAL xfloat();
#define FTAB_xfloat FTAB_xfix + 1

extern LVAL xadd();
#define FTAB_xadd FTAB_xfloat + 1

extern LVAL xsub();
#define FTAB_xsub FTAB_xadd + 1

extern LVAL xmul();
#define FTAB_xmul FTAB_xsub + 1

extern LVAL xdiv();
#define FTAB_xdiv FTAB_xmul + 1

extern LVAL xadd1();
#define FTAB_xadd1 FTAB_xdiv + 1

extern LVAL xsub1();
#define FTAB_xsub1 FTAB_xadd1 + 1

extern LVAL xrem();
#define FTAB_xrem FTAB_xsub1 + 1

extern LVAL xmin();
#define FTAB_xmin FTAB_xrem + 1

extern LVAL xmax();
#define FTAB_xmax FTAB_xmin + 1

extern LVAL xabs();
#define FTAB_xabs FTAB_xmax + 1

extern LVAL xsin();
#define FTAB_xsin FTAB_xabs + 1

extern LVAL xcos();
#define FTAB_xcos FTAB_xsin + 1

extern LVAL xtan();
#define FTAB_xtan FTAB_xcos + 1

extern LVAL xexpt();
#define FTAB_xexpt FTAB_xtan + 1

extern LVAL xexp();
#define FTAB_xexp FTAB_xexpt + 1

extern LVAL xsqrt();
#define FTAB_xsqrt FTAB_xexp + 1

extern LVAL xrand();
#define FTAB_xrand FTAB_xsqrt + 1

extern LVAL xlogand();
#define FTAB_xlogand FTAB_xrand + 1

extern LVAL xlogior();
#define FTAB_xlogior FTAB_xlogand + 1

extern LVAL xlogxor();
#define FTAB_xlogxor FTAB_xlogior + 1

extern LVAL xlognot();
#define FTAB_xlognot FTAB_xlogxor + 1

extern LVAL xlss();
#define FTAB_xlss FTAB_xlognot + 1

extern LVAL xleq();
#define FTAB_xleq FTAB_xlss + 1

extern LVAL xequ();
#define FTAB_xequ FTAB_xleq + 1

extern LVAL xneq();
#define FTAB_xneq FTAB_xequ + 1

extern LVAL xgeq();
#define FTAB_xgeq FTAB_xneq + 1

extern LVAL xgtr();
#define FTAB_xgtr FTAB_xgeq + 1

extern LVAL xstrcat();
#define FTAB_xstrcat FTAB_xgtr + 1

extern LVAL xsubseq();
#define FTAB_xsubseq FTAB_xstrcat + 1

extern LVAL xstring();
#define FTAB_xstring FTAB_xsubseq + 1

extern LVAL xchar();
#define FTAB_xchar FTAB_xstring + 1

extern LVAL xread();
#define FTAB_xread FTAB_xchar + 1

extern LVAL xprint();
#define FTAB_xprint FTAB_xread + 1

extern LVAL xprin1();
#define FTAB_xprin1 FTAB_xprint + 1

extern LVAL xprinc();
#define FTAB_xprinc FTAB_xprin1 + 1

extern LVAL xterpri();
#define FTAB_xterpri FTAB_xprinc + 1

extern LVAL xflatsize();
#define FTAB_xflatsize FTAB_xterpri + 1

extern LVAL xflatc();
#define FTAB_xflatc FTAB_xflatsize + 1

extern LVAL xopen();
#define FTAB_xopen FTAB_xflatc + 1

extern LVAL xformat();
#define FTAB_xformat FTAB_xopen + 1

extern LVAL xclose();
#define FTAB_xclose FTAB_xformat + 1

extern LVAL xrdchar();
#define FTAB_xrdchar FTAB_xclose + 1

extern LVAL xpkchar();
#define FTAB_xpkchar FTAB_xrdchar + 1

extern LVAL xwrchar();
#define FTAB_xwrchar FTAB_xpkchar + 1

extern LVAL xreadline();
#define FTAB_xreadline FTAB_xwrchar + 1

extern LVAL xload();
#define FTAB_xload FTAB_xreadline + 1

extern LVAL xtranscript();
#define FTAB_xtranscript FTAB_xload + 1

extern LVAL xgc();
#define FTAB_xgc FTAB_xtranscript + 1

extern LVAL xexpand();
#define FTAB_xexpand FTAB_xgc + 1

extern LVAL xalloc();
#define FTAB_xalloc FTAB_xexpand + 1

extern LVAL xmem();
#define FTAB_xmem FTAB_xalloc + 1

#ifdef SAVERESTORE
extern LVAL xsave();
extern LVAL xrestore();
#endif

#define FTAB_xsave FTAB_xmem + 1
#define FTAB_xrestore FTAB_xsave + 1

extern LVAL xtype();
#define FTAB_xtype FTAB_xrestore + 1

extern LVAL xexit();
#define FTAB_xexit FTAB_xtype + 1

extern LVAL xpeek();
#define FTAB_xpeek FTAB_xexit + 1

extern LVAL xpoke();
#define FTAB_xpoke FTAB_xpeek + 1

extern LVAL xaddrs();
#define FTAB_xaddrs FTAB_xpoke + 1

extern LVAL xvector();
#define FTAB_xvector FTAB_xaddrs + 1

extern LVAL xblock();
#define FTAB_xblock FTAB_xvector + 1

extern LVAL xrtnfrom();
#define FTAB_xrtnfrom FTAB_xblock + 1

extern LVAL xtagbody();
#define FTAB_xtagbody FTAB_xrtnfrom + 1

extern LVAL xpsetq();
#define FTAB_xpsetq FTAB_xtagbody + 1

extern LVAL xflet();
#define FTAB_xflet FTAB_xpsetq + 1

extern LVAL xlabels();
#define FTAB_xlabels FTAB_xflet + 1

extern LVAL xmacrolet();
#define FTAB_xmacrolet FTAB_xlabels + 1

extern LVAL xunwindprotect();
#define FTAB_xunwindprotect FTAB_xmacrolet + 1

extern LVAL xpp();
#define FTAB_xpp FTAB_xunwindprotect + 1

extern LVAL xstrlss();
#define FTAB_xstrlss FTAB_xpp + 1

extern LVAL xstrleq();
#define FTAB_xstrleq FTAB_xstrlss + 1

extern LVAL xstreql();
#define FTAB_xstreql FTAB_xstrleq + 1

extern LVAL xstrneq();
#define FTAB_xstrneq FTAB_xstreql + 1

extern LVAL xstrgeq();
#define FTAB_xstrgeq FTAB_xstrneq + 1

extern LVAL xstrgtr();
#define FTAB_xstrgtr FTAB_xstrgeq + 1

extern LVAL xstrilss();
#define FTAB_xstrilss FTAB_xstrgtr + 1

extern LVAL xstrileq();
#define FTAB_xstrileq FTAB_xstrilss + 1

extern LVAL xstrieql();
#define FTAB_xstrieql FTAB_xstrileq + 1

extern LVAL xstrineq();
#define FTAB_xstrineq FTAB_xstrieql + 1

extern LVAL xstrigeq();
#define FTAB_xstrigeq FTAB_xstrineq + 1

extern LVAL xstrigtr();
#define FTAB_xstrigtr FTAB_xstrigeq + 1

extern LVAL xintegerp();
#define FTAB_xintegerp FTAB_xstrigtr + 1

extern LVAL xfloatp();
#define FTAB_xfloatp FTAB_xintegerp + 1

extern LVAL xstringp();
#define FTAB_xstringp FTAB_xfloatp + 1

extern LVAL xarrayp();
#define FTAB_xarrayp FTAB_xstringp + 1

extern LVAL xstreamp();
#define FTAB_xstreamp FTAB_xarrayp + 1

extern LVAL xobjectp();
#define FTAB_xobjectp FTAB_xstreamp + 1

extern LVAL xupcase();
#define FTAB_xupcase FTAB_xobjectp + 1

extern LVAL xdowncase();
#define FTAB_xdowncase FTAB_xupcase + 1

extern LVAL xnupcase();
#define FTAB_xnupcase FTAB_xdowncase + 1

extern LVAL xndowncase();
#define FTAB_xndowncase FTAB_xnupcase + 1

extern LVAL xtrim();
#define FTAB_xtrim FTAB_xndowncase + 1

extern LVAL xlefttrim();
#define FTAB_xlefttrim FTAB_xtrim + 1

extern LVAL xrighttrim();
#define FTAB_xrighttrim FTAB_xlefttrim + 1

extern LVAL xwhen();
#define FTAB_xwhen FTAB_xrighttrim + 1

extern LVAL xunless();
#define FTAB_xunless FTAB_xwhen + 1

extern LVAL xloop();
#define FTAB_xloop FTAB_xunless + 1

extern LVAL xsymfunction();
#define FTAB_xsymfunction FTAB_xloop + 1

extern LVAL xfboundp();
#define FTAB_xfboundp FTAB_xsymfunction + 1

extern LVAL xsend();
#define FTAB_xsend FTAB_xfboundp + 1

extern LVAL xsendsuper();
#define FTAB_xsendsuper FTAB_xsend + 1

extern LVAL xprogv();
#define FTAB_xprogv FTAB_xsendsuper + 1

extern LVAL xcharp();
#define FTAB_xcharp FTAB_xprogv + 1

extern LVAL xcharint();
#define FTAB_xcharint FTAB_xcharp + 1

extern LVAL xintchar();
#define FTAB_xintchar FTAB_xcharint + 1

extern LVAL xrdbyte();
#define FTAB_xrdbyte FTAB_xintchar + 1

extern LVAL xwrbyte();
#define FTAB_xwrbyte FTAB_xrdbyte + 1

extern LVAL xmkstrinput();
#define FTAB_xmkstrinput FTAB_xwrbyte + 1

extern LVAL xmkstroutput();
#define FTAB_xmkstroutput FTAB_xmkstrinput + 1

extern LVAL xgetstroutput();
#define FTAB_xgetstroutput FTAB_xmkstroutput + 1

extern LVAL xgetlstoutput();
#define FTAB_xgetlstoutput FTAB_xgetstroutput + 1

extern LVAL xgcd();
#define FTAB_xgcd FTAB_xgetlstoutput + 1

extern LVAL xgetlambda();
#define FTAB_xgetlambda FTAB_xgcd + 1

extern LVAL xmacroexpand();
#define FTAB_xmacroexpand FTAB_xgetlambda + 1

extern LVAL x1macroexpand();
#define FTAB_x1macroexpand FTAB_xmacroexpand + 1

extern LVAL xchrlss();
#define FTAB_xchrlss FTAB_x1macroexpand + 1

extern LVAL xchrleq();
#define FTAB_xchrleq FTAB_xchrlss + 1

extern LVAL xchreql();
#define FTAB_xchreql FTAB_xchrleq + 1

extern LVAL xchrneq();
#define FTAB_xchrneq FTAB_xchreql + 1

extern LVAL xchrgeq();
#define FTAB_xchrgeq FTAB_xchrneq + 1

extern LVAL xchrgtr();
#define FTAB_xchrgtr FTAB_xchrgeq + 1

extern LVAL xchrilss();
#define FTAB_xchrilss FTAB_xchrgtr + 1

extern LVAL xchrileq();
#define FTAB_xchrileq FTAB_xchrilss + 1

extern LVAL xchrieql();
#define FTAB_xchrieql FTAB_xchrileq + 1

extern LVAL xchrineq();
#define FTAB_xchrineq FTAB_xchrieql + 1

extern LVAL xchrigeq();
#define FTAB_xchrigeq FTAB_xchrineq + 1

extern LVAL xchrigtr();
#define FTAB_xchrigtr FTAB_xchrigeq + 1

extern LVAL xuppercasep();
#define FTAB_xuppercasep FTAB_xchrigtr + 1

extern LVAL xlowercasep();
#define FTAB_xlowercasep FTAB_xuppercasep + 1

extern LVAL xbothcasep();
#define FTAB_xbothcasep FTAB_xlowercasep + 1

extern LVAL xdigitp();
#define FTAB_xdigitp FTAB_xbothcasep + 1

extern LVAL xalphanumericp();
#define FTAB_xalphanumericp FTAB_xdigitp + 1

extern LVAL xchupcase();
#define FTAB_xchupcase FTAB_xalphanumericp + 1

extern LVAL xchdowncase();
#define FTAB_xchdowncase FTAB_xchupcase + 1

extern LVAL xdigitchar();
#define FTAB_xdigitchar FTAB_xchdowncase + 1

extern LVAL xcharcode();
#define FTAB_xcharcode FTAB_xdigitchar + 1

extern LVAL xcodechar();
#define FTAB_xcodechar FTAB_xcharcode + 1

extern LVAL xendp();
#define FTAB_xendp FTAB_xcodechar + 1

extern LVAL xremif();
#define FTAB_xremif FTAB_xendp + 1

extern LVAL xremifnot();
#define FTAB_xremifnot FTAB_xremif + 1

extern LVAL xdelif();
#define FTAB_xdelif FTAB_xremifnot + 1

extern LVAL xdelifnot();
#define FTAB_xdelifnot FTAB_xdelif + 1

extern LVAL xtrace();
#define FTAB_xtrace FTAB_xdelifnot + 1

extern LVAL xuntrace();
#define FTAB_xuntrace FTAB_xtrace + 1

extern LVAL xsort();
#define FTAB_xsort FTAB_xuntrace + 1

extern LVAL Prim_SYSTEM();
#define FTAB_Prim_SYSTEM FTAB_xsort + 1

extern LVAL Prim_POPEN();
#define FTAB_Prim_POPEN FTAB_Prim_SYSTEM + 1

extern LVAL Prim_PCLOSE();
#define FTAB_Prim_PCLOSE FTAB_Prim_POPEN + 1

extern LVAL Widget_Class_Method_ISNEW();
#define FTAB_Widget_Class_Method_ISNEW FTAB_Prim_PCLOSE + 1

extern LVAL Shell_Widget_Class_Method_ISNEW();
#define FTAB_Shell_Widget_Class_Method_ISNEW FTAB_Widget_Class_Method_ISNEW + 1

extern LVAL Popup_Shell_Widget_Class_Method_ISNEW();
#define FTAB_Popup_Shell_Widget_Class_Method_ISNEW FTAB_Shell_Widget_Class_Method_ISNEW + 1

extern LVAL Shell_Widget_Class_Method_REALIZE();
#define FTAB_Shell_Widget_Class_Method_REALIZE FTAB_Popup_Shell_Widget_Class_Method_ISNEW + 1

extern LVAL Shell_Widget_Class_Method_UNREALIZE();
#define FTAB_Shell_Widget_Class_Method_UNREALIZE FTAB_Shell_Widget_Class_Method_REALIZE + 1

extern LVAL Popup_Shell_Widget_Class_Method_POPUP();
#define FTAB_Popup_Shell_Widget_Class_Method_POPUP FTAB_Shell_Widget_Class_Method_UNREALIZE + 1

extern LVAL Popup_Shell_Widget_Class_Method_POPDOWN();
#define FTAB_Popup_Shell_Widget_Class_Method_POPDOWN FTAB_Popup_Shell_Widget_Class_Method_POPUP + 1

extern LVAL Widget_Class_Method_MANAGE();
#define FTAB_Widget_Class_Method_MANAGE FTAB_Popup_Shell_Widget_Class_Method_POPDOWN + 1

extern LVAL Widget_Class_Method_UNMANAGE();
#define FTAB_Widget_Class_Method_UNMANAGE FTAB_Widget_Class_Method_MANAGE + 1

extern LVAL Xm_File_Selection_Box_Widget_Class_Method_ISNEW();
#define FTAB_Xm_File_Selection_Box_Widget_Class_Method_ISNEW FTAB_Widget_Class_Method_UNMANAGE + 1

extern LVAL Xm_Form_Widget_Class_Method_ISNEW();
#define FTAB_Xm_Form_Widget_Class_Method_ISNEW FTAB_Xm_File_Selection_Box_Widget_Class_Method_ISNEW + 1

extern LVAL Xm_List_Widget_Class_Method_ISNEW();
#define FTAB_Xm_List_Widget_Class_Method_ISNEW FTAB_Xm_Form_Widget_Class_Method_ISNEW + 1

extern LVAL Xm_Text_Widget_Class_Method_ISNEW();
#define FTAB_Xm_Text_Widget_Class_Method_ISNEW FTAB_Xm_List_Widget_Class_Method_ISNEW + 1

extern LVAL Xm_Message_Box_Widget_Class_Method_ISNEW();
#define FTAB_Xm_Message_Box_Widget_Class_Method_ISNEW FTAB_Xm_Text_Widget_Class_Method_ISNEW + 1

extern LVAL Xm_Row_Column_Widget_Class_Method_ISNEW();
#define FTAB_Xm_Row_Column_Widget_Class_Method_ISNEW FTAB_Xm_Message_Box_Widget_Class_Method_ISNEW + 1

extern LVAL Xm_Selection_Box_Widget_Class_Method_ISNEW();
#define FTAB_Xm_Selection_Box_Widget_Class_Method_ISNEW FTAB_Xm_Row_Column_Widget_Class_Method_ISNEW + 1

extern LVAL Widget_Class_Method_SET_VALUES();
#define FTAB_Widget_Class_Method_SET_VALUES FTAB_Xm_Selection_Box_Widget_Class_Method_ISNEW + 1

extern LVAL Widget_Class_Method_ADD_CALLBACK();
#define FTAB_Widget_Class_Method_ADD_CALLBACK FTAB_Widget_Class_Method_SET_VALUES + 1

extern LVAL Xm_Text_Widget_Class_Method_GET_STRING();
#define FTAB_Xm_Text_Widget_Class_Method_GET_STRING FTAB_Widget_Class_Method_ADD_CALLBACK + 1

extern LVAL Wto_Prim_XtAddTimeOut();
#define FTAB_Wto_Prim_XtAddTimeOut FTAB_Xm_Text_Widget_Class_Method_GET_STRING + 1

extern LVAL Wto_Prim_XtRemoveTimeout();
#define FTAB_Wto_Prim_XtRemoveTimeout FTAB_Wto_Prim_XtAddTimeOut + 1

extern LVAL Wcb_Prim_XtRemoveCallback();
#define FTAB_Wcb_Prim_XtRemoveCallback FTAB_Wto_Prim_XtRemoveTimeout + 1

extern LVAL Widget_Class_Method_SET_CALLBACK();
#define FTAB_Widget_Class_Method_SET_CALLBACK FTAB_Wcb_Prim_XtRemoveCallback + 1

extern LVAL Widget_Class_Method_REMOVE_ALL_CALLBACKS();
#define FTAB_Widget_Class_Method_REMOVE_ALL_CALLBACKS FTAB_Widget_Class_Method_SET_CALLBACK + 1

extern LVAL Wut_Prim_XAllocColor();
#define FTAB_Wut_Prim_XAllocColor FTAB_Widget_Class_Method_REMOVE_ALL_CALLBACKS + 1

extern LVAL Wpm_Prim_XmGetPixmap();
#define FTAB_Wpm_Prim_XmGetPixmap FTAB_Wut_Prim_XAllocColor + 1

extern LVAL Wpm_Prim_XmInstallImage();
#define FTAB_Wpm_Prim_XmInstallImage FTAB_Wpm_Prim_XmGetPixmap + 1

extern LVAL Wpm_Prim_XmUninstallImage();
#define FTAB_Wpm_Prim_XmUninstallImage FTAB_Wpm_Prim_XmInstallImage + 1

extern LVAL Wpm_Prim_XmGetImageFromFile();
#define FTAB_Wpm_Prim_XmGetImageFromFile FTAB_Wpm_Prim_XmUninstallImage + 1

extern LVAL Widget_Class_Method_UPDATE_DISPLAY();
#define FTAB_Widget_Class_Method_UPDATE_DISPLAY FTAB_Wpm_Prim_XmGetImageFromFile + 1

extern LVAL Wut_UserClick_To_WidgetObj();
#define FTAB_Wut_UserClick_To_WidgetObj FTAB_Widget_Class_Method_UPDATE_DISPLAY + 1

extern LVAL Widget_Class_Method_DESTROY();
#define FTAB_Widget_Class_Method_DESTROY FTAB_Wut_UserClick_To_WidgetObj + 1

extern LVAL Widget_Class_Method_PARENT();
#define FTAB_Widget_Class_Method_PARENT FTAB_Widget_Class_Method_DESTROY + 1

extern LVAL Xm_Text_Widget_Class_Method_GET_LAST_POSITION();
#define FTAB_Xm_Text_Widget_Class_Method_GET_LAST_POSITION FTAB_Widget_Class_Method_PARENT + 1

extern LVAL Xm_Text_Widget_Class_Method_SET_STRING();
#define FTAB_Xm_Text_Widget_Class_Method_SET_STRING FTAB_Xm_Text_Widget_Class_Method_GET_LAST_POSITION + 1

extern LVAL Xm_Text_Widget_Class_Method_REPLACE();
#define FTAB_Xm_Text_Widget_Class_Method_REPLACE FTAB_Xm_Text_Widget_Class_Method_SET_STRING + 1

extern LVAL Xm_Text_Widget_Class_Method_GET_EDITABLE();
#define FTAB_Xm_Text_Widget_Class_Method_GET_EDITABLE FTAB_Xm_Text_Widget_Class_Method_REPLACE + 1

extern LVAL Xm_Text_Widget_Class_Method_SET_EDITABLE();
#define FTAB_Xm_Text_Widget_Class_Method_SET_EDITABLE FTAB_Xm_Text_Widget_Class_Method_GET_EDITABLE + 1

extern LVAL Xm_Text_Widget_Class_Method_GET_MAX_LENGTH();
#define FTAB_Xm_Text_Widget_Class_Method_GET_MAX_LENGTH FTAB_Xm_Text_Widget_Class_Method_SET_EDITABLE + 1

extern LVAL Xm_Text_Widget_Class_Method_SET_MAX_LENGTH();
#define FTAB_Xm_Text_Widget_Class_Method_SET_MAX_LENGTH FTAB_Xm_Text_Widget_Class_Method_GET_MAX_LENGTH + 1

extern LVAL Xm_Text_Widget_Class_Method_GET_SELECTION();
#define FTAB_Xm_Text_Widget_Class_Method_GET_SELECTION FTAB_Xm_Text_Widget_Class_Method_SET_MAX_LENGTH + 1

extern LVAL Xm_Text_Widget_Class_Method_SET_SELECTION();
#define FTAB_Xm_Text_Widget_Class_Method_SET_SELECTION FTAB_Xm_Text_Widget_Class_Method_GET_SELECTION + 1

extern LVAL Xm_Text_Widget_Class_Method_CLEAR_SELECTION();
#define FTAB_Xm_Text_Widget_Class_Method_CLEAR_SELECTION FTAB_Xm_Text_Widget_Class_Method_SET_SELECTION + 1

extern LVAL Xm_Text_Widget_Class_Method_GET_TOP_POSITION();
#define FTAB_Xm_Text_Widget_Class_Method_GET_TOP_POSITION FTAB_Xm_Text_Widget_Class_Method_CLEAR_SELECTION + 1

extern LVAL Xm_Text_Widget_Class_Method_SET_TOP_POSITION();
#define FTAB_Xm_Text_Widget_Class_Method_SET_TOP_POSITION FTAB_Xm_Text_Widget_Class_Method_GET_TOP_POSITION + 1

extern LVAL Xm_Text_Widget_Class_Method_GET_INSERTION_POSITION();
#define FTAB_Xm_Text_Widget_Class_Method_GET_INSERTION_POSITION FTAB_Xm_Text_Widget_Class_Method_SET_TOP_POSITION + 1

extern LVAL Xm_Text_Widget_Class_Method_SET_INSERTION_POSITION();
#define FTAB_Xm_Text_Widget_Class_Method_SET_INSERTION_POSITION FTAB_Xm_Text_Widget_Class_Method_GET_INSERTION_POSITION + 1

extern LVAL Xm_Text_Widget_Class_Method_GET_SELECTION_POSITION();
#define FTAB_Xm_Text_Widget_Class_Method_GET_SELECTION_POSITION FTAB_Xm_Text_Widget_Class_Method_SET_INSERTION_POSITION + 1

extern LVAL Xm_Text_Widget_Class_Method_XY_TO_POS();
#define FTAB_Xm_Text_Widget_Class_Method_XY_TO_POS FTAB_Xm_Text_Widget_Class_Method_GET_SELECTION_POSITION + 1

extern LVAL Xm_Text_Widget_Class_Method_POS_TO_XY();
#define FTAB_Xm_Text_Widget_Class_Method_POS_TO_XY FTAB_Xm_Text_Widget_Class_Method_XY_TO_POS + 1

extern LVAL Xm_Text_Widget_Class_Method_SHOW_POSITION();
#define FTAB_Xm_Text_Widget_Class_Method_SHOW_POSITION FTAB_Xm_Text_Widget_Class_Method_POS_TO_XY + 1

extern LVAL Xm_Text_Widget_Class_Method_SCROLL();
#define FTAB_Xm_Text_Widget_Class_Method_SCROLL FTAB_Xm_Text_Widget_Class_Method_SHOW_POSITION + 1

extern LVAL Xm_Text_Widget_Class_Method_DISABLE_REDISPLAY();
#define FTAB_Xm_Text_Widget_Class_Method_DISABLE_REDISPLAY FTAB_Xm_Text_Widget_Class_Method_SCROLL + 1

extern LVAL Xm_Text_Widget_Class_Method_ENABLE_REDISPLAY();
#define FTAB_Xm_Text_Widget_Class_Method_ENABLE_REDISPLAY FTAB_Xm_Text_Widget_Class_Method_DISABLE_REDISPLAY + 1

extern LVAL Xm_List_Widget_Class_Method_ADD_ITEM();
#define FTAB_Xm_List_Widget_Class_Method_ADD_ITEM FTAB_Xm_Text_Widget_Class_Method_ENABLE_REDISPLAY + 1

extern LVAL Xm_List_Widget_Class_Method_ADD_ITEM_UNSELECTED();
#define FTAB_Xm_List_Widget_Class_Method_ADD_ITEM_UNSELECTED FTAB_Xm_List_Widget_Class_Method_ADD_ITEM + 1

extern LVAL Xm_List_Widget_Class_Method_DELETE_ITEM();
#define FTAB_Xm_List_Widget_Class_Method_DELETE_ITEM FTAB_Xm_List_Widget_Class_Method_ADD_ITEM_UNSELECTED + 1

extern LVAL Xm_List_Widget_Class_Method_DELETE_POS();
#define FTAB_Xm_List_Widget_Class_Method_DELETE_POS FTAB_Xm_List_Widget_Class_Method_DELETE_ITEM + 1

extern LVAL Xm_List_Widget_Class_Method_SELECT_ITEM();
#define FTAB_Xm_List_Widget_Class_Method_SELECT_ITEM FTAB_Xm_List_Widget_Class_Method_DELETE_POS + 1

extern LVAL Xm_List_Widget_Class_Method_SELECT_POS();
#define FTAB_Xm_List_Widget_Class_Method_SELECT_POS FTAB_Xm_List_Widget_Class_Method_SELECT_ITEM + 1

extern LVAL Xm_List_Widget_Class_Method_DESELECT_ITEM();
#define FTAB_Xm_List_Widget_Class_Method_DESELECT_ITEM FTAB_Xm_List_Widget_Class_Method_SELECT_POS + 1

extern LVAL Xm_List_Widget_Class_Method_DESELECT_POS();
#define FTAB_Xm_List_Widget_Class_Method_DESELECT_POS FTAB_Xm_List_Widget_Class_Method_DESELECT_ITEM + 1

extern LVAL Xm_List_Widget_Class_Method_DESELECT_ALL_ITEMS();
#define FTAB_Xm_List_Widget_Class_Method_DESELECT_ALL_ITEMS FTAB_Xm_List_Widget_Class_Method_DESELECT_POS + 1

extern LVAL Xm_List_Widget_Class_Method_SET_POS();
#define FTAB_Xm_List_Widget_Class_Method_SET_POS FTAB_Xm_List_Widget_Class_Method_DESELECT_ALL_ITEMS + 1

extern LVAL Xm_List_Widget_Class_Method_SET_BOTTOM_POS();
#define FTAB_Xm_List_Widget_Class_Method_SET_BOTTOM_POS FTAB_Xm_List_Widget_Class_Method_SET_POS + 1

extern LVAL Xm_List_Widget_Class_Method_SET_ITEM();
#define FTAB_Xm_List_Widget_Class_Method_SET_ITEM FTAB_Xm_List_Widget_Class_Method_SET_BOTTOM_POS + 1

extern LVAL Xm_List_Widget_Class_Method_SET_BOTTOM_ITEM();
#define FTAB_Xm_List_Widget_Class_Method_SET_BOTTOM_ITEM FTAB_Xm_List_Widget_Class_Method_SET_ITEM + 1

extern LVAL Xm_List_Widget_Class_Method_ITEM_EXISTS();
#define FTAB_Xm_List_Widget_Class_Method_ITEM_EXISTS FTAB_Xm_List_Widget_Class_Method_SET_BOTTOM_ITEM + 1

extern LVAL Xm_List_Widget_Class_Method_SET_HORIZ_POS();
#define FTAB_Xm_List_Widget_Class_Method_SET_HORIZ_POS FTAB_Xm_List_Widget_Class_Method_ITEM_EXISTS + 1

extern LVAL Xm_List_Widget_Class_Method_ADD_CALLBACK();
#define FTAB_Xm_List_Widget_Class_Method_ADD_CALLBACK FTAB_Xm_List_Widget_Class_Method_SET_HORIZ_POS + 1

extern LVAL Xm_List_Widget_Class_Method_SET_CALLBACK();
#define FTAB_Xm_List_Widget_Class_Method_SET_CALLBACK FTAB_Xm_List_Widget_Class_Method_ADD_CALLBACK + 1

extern LVAL Xm_Drawing_Area_Widget_Class_Method_ADD_CALLBACK();
#define FTAB_Xm_Drawing_Area_Widget_Class_Method_ADD_CALLBACK FTAB_Xm_List_Widget_Class_Method_SET_CALLBACK + 1

extern LVAL Xm_Drawing_Area_Widget_Class_Method_SET_CALLBACK();
#define FTAB_Xm_Drawing_Area_Widget_Class_Method_SET_CALLBACK FTAB_Xm_Drawing_Area_Widget_Class_Method_ADD_CALLBACK + 1

extern LVAL Xm_Drawn_Button_Widget_Class_Method_ADD_CALLBACK();
#define FTAB_Xm_Drawn_Button_Widget_Class_Method_ADD_CALLBACK FTAB_Xm_Drawing_Area_Widget_Class_Method_SET_CALLBACK + 1

extern LVAL Xm_Drawn_Button_Widget_Class_Method_SET_CALLBACK();
#define FTAB_Xm_Drawn_Button_Widget_Class_Method_SET_CALLBACK FTAB_Xm_Drawn_Button_Widget_Class_Method_ADD_CALLBACK + 1

extern LVAL Xm_Row_Column_Widget_Class_Method_ADD_CALLBACK();
#define FTAB_Xm_Row_Column_Widget_Class_Method_ADD_CALLBACK FTAB_Xm_Drawn_Button_Widget_Class_Method_SET_CALLBACK + 1

extern LVAL Xm_Row_Column_Widget_Class_Method_SET_CALLBACK();
#define FTAB_Xm_Row_Column_Widget_Class_Method_SET_CALLBACK FTAB_Xm_Row_Column_Widget_Class_Method_ADD_CALLBACK + 1

extern LVAL Xm_Scroll_Bar_Widget_Class_Method_ADD_CALLBACK();
#define FTAB_Xm_Scroll_Bar_Widget_Class_Method_ADD_CALLBACK FTAB_Xm_Row_Column_Widget_Class_Method_SET_CALLBACK + 1

extern LVAL Xm_Scroll_Bar_Widget_Class_Method_SET_CALLBACK();
#define FTAB_Xm_Scroll_Bar_Widget_Class_Method_SET_CALLBACK FTAB_Xm_Scroll_Bar_Widget_Class_Method_ADD_CALLBACK + 1

extern LVAL Xm_Toggle_Button_Widget_Class_Method_ADD_CALLBACK();
#define FTAB_Xm_Toggle_Button_Widget_Class_Method_ADD_CALLBACK FTAB_Xm_Scroll_Bar_Widget_Class_Method_SET_CALLBACK + 1

extern LVAL Xm_Toggle_Button_Widget_Class_Method_SET_CALLBACK();
#define FTAB_Xm_Toggle_Button_Widget_Class_Method_SET_CALLBACK FTAB_Xm_Toggle_Button_Widget_Class_Method_ADD_CALLBACK + 1

extern LVAL Xm_Selection_Box_Widget_Class_Method_ADD_CALLBACK();
#define FTAB_Xm_Selection_Box_Widget_Class_Method_ADD_CALLBACK FTAB_Xm_Toggle_Button_Widget_Class_Method_SET_CALLBACK + 1

extern LVAL Xm_Selection_Box_Widget_Class_Method_SET_CALLBACK();
#define FTAB_Xm_Selection_Box_Widget_Class_Method_SET_CALLBACK FTAB_Xm_Selection_Box_Widget_Class_Method_ADD_CALLBACK + 1

extern LVAL Xm_Command_Widget_Class_Method_ADD_CALLBACK();
#define FTAB_Xm_Command_Widget_Class_Method_ADD_CALLBACK FTAB_Xm_Selection_Box_Widget_Class_Method_SET_CALLBACK + 1

extern LVAL Xm_Command_Widget_Class_Method_SET_CALLBACK();
#define FTAB_Xm_Command_Widget_Class_Method_SET_CALLBACK FTAB_Xm_Command_Widget_Class_Method_ADD_CALLBACK + 1

extern LVAL Xm_File_Selection_Box_Widget_Class_Method_ADD_CALLBACK();
#define FTAB_Xm_File_Selection_Box_Widget_Class_Method_ADD_CALLBACK FTAB_Xm_Command_Widget_Class_Method_SET_CALLBACK + 1

extern LVAL Xm_File_Selection_Box_Widget_Class_Method_SET_CALLBACK();
#define FTAB_Xm_File_Selection_Box_Widget_Class_Method_SET_CALLBACK FTAB_Xm_File_Selection_Box_Widget_Class_Method_ADD_CALLBACK + 1

extern LVAL Xm_Scale_Widget_Class_Method_ADD_CALLBACK();
#define FTAB_Xm_Scale_Widget_Class_Method_ADD_CALLBACK FTAB_Xm_File_Selection_Box_Widget_Class_Method_SET_CALLBACK + 1

extern LVAL Xm_Scale_Widget_Class_Method_SET_CALLBACK();
#define FTAB_Xm_Scale_Widget_Class_Method_SET_CALLBACK FTAB_Xm_Scale_Widget_Class_Method_ADD_CALLBACK + 1

extern LVAL Xm_Text_Widget_Class_Method_ADD_CALLBACK();
#define FTAB_Xm_Text_Widget_Class_Method_ADD_CALLBACK FTAB_Xm_Scale_Widget_Class_Method_SET_CALLBACK + 1

extern LVAL Xm_Text_Widget_Class_Method_SET_CALLBACK();
#define FTAB_Xm_Text_Widget_Class_Method_SET_CALLBACK FTAB_Xm_Text_Widget_Class_Method_ADD_CALLBACK + 1

extern LVAL Widget_Class_Method_GET_VALUES();
#define FTAB_Widget_Class_Method_GET_VALUES FTAB_Xm_Text_Widget_Class_Method_SET_CALLBACK + 1

extern LVAL Xm_Cascade_Button_Widget_Class_Method_HIGHLIGHT();
#define FTAB_Xm_Cascade_Button_Widget_Class_Method_HIGHLIGHT FTAB_Widget_Class_Method_GET_VALUES + 1

extern LVAL Xm_Command_Widget_Class_Method_GET_CHILD();
#define FTAB_Xm_Command_Widget_Class_Method_GET_CHILD FTAB_Xm_Cascade_Button_Widget_Class_Method_HIGHLIGHT + 1

extern LVAL Xm_Command_Widget_Class_Method_SET_VALUE();
#define FTAB_Xm_Command_Widget_Class_Method_SET_VALUE FTAB_Xm_Command_Widget_Class_Method_GET_CHILD + 1

extern LVAL Xm_Command_Widget_Class_Method_APPEND_VALUE();
#define FTAB_Xm_Command_Widget_Class_Method_APPEND_VALUE FTAB_Xm_Command_Widget_Class_Method_SET_VALUE + 1

extern LVAL Xm_Command_Widget_Class_Method_ERROR();
#define FTAB_Xm_Command_Widget_Class_Method_ERROR FTAB_Xm_Command_Widget_Class_Method_APPEND_VALUE + 1

extern LVAL Xm_File_Selection_Box_Widget_Class_Method_GET_CHILD();
#define FTAB_Xm_File_Selection_Box_Widget_Class_Method_GET_CHILD FTAB_Xm_Command_Widget_Class_Method_ERROR + 1

extern LVAL Xm_File_Selection_Box_Widget_Class_Method_DO_SEARCH();
#define FTAB_Xm_File_Selection_Box_Widget_Class_Method_DO_SEARCH FTAB_Xm_File_Selection_Box_Widget_Class_Method_GET_CHILD + 1

extern LVAL Xm_Main_Window_Widget_Class_SET_AREAS();
#define FTAB_Xm_Main_Window_Widget_Class_SET_AREAS FTAB_Xm_File_Selection_Box_Widget_Class_Method_DO_SEARCH + 1

extern LVAL Xm_Main_Window_Widget_Class_SEP1();
#define FTAB_Xm_Main_Window_Widget_Class_SEP1 FTAB_Xm_Main_Window_Widget_Class_SET_AREAS + 1

extern LVAL Xm_Main_Window_Widget_Class_SEP2();
#define FTAB_Xm_Main_Window_Widget_Class_SEP2 FTAB_Xm_Main_Window_Widget_Class_SEP1 + 1

extern LVAL Xm_Message_Box_Widget_Class_Method_GET_CHILD();
#define FTAB_Xm_Message_Box_Widget_Class_Method_GET_CHILD FTAB_Xm_Main_Window_Widget_Class_SEP2 + 1

extern LVAL Xm_Row_Column_Widget_Class_Method_MENU_POSITION();
#define FTAB_Xm_Row_Column_Widget_Class_Method_MENU_POSITION FTAB_Xm_Message_Box_Widget_Class_Method_GET_CHILD + 1

extern LVAL Xm_Row_Column_Widget_Class_Method_OPTION_LABEL_GADGET();
#define FTAB_Xm_Row_Column_Widget_Class_Method_OPTION_LABEL_GADGET FTAB_Xm_Row_Column_Widget_Class_Method_MENU_POSITION + 1

extern LVAL Xm_Row_Column_Widget_Class_Method_OPTION_BUTTON_GADGET();
#define FTAB_Xm_Row_Column_Widget_Class_Method_OPTION_BUTTON_GADGET FTAB_Xm_Row_Column_Widget_Class_Method_OPTION_LABEL_GADGET + 1

extern LVAL xdefstruct();
#define FTAB_xdefstruct FTAB_Xm_Row_Column_Widget_Class_Method_OPTION_BUTTON_GADGET + 1

extern LVAL xstrtypep();
#define FTAB_xstrtypep FTAB_xdefstruct + 1

extern LVAL xmkstruct();
#define FTAB_xmkstruct FTAB_xstrtypep + 1

extern LVAL xcpystruct();
#define FTAB_xcpystruct FTAB_xmkstruct + 1

extern LVAL xstrref();
#define FTAB_xstrref FTAB_xcpystruct + 1

extern LVAL xstrset();
#define FTAB_xstrset FTAB_xstrref + 1

extern LVAL xasin();
#define FTAB_xasin FTAB_xstrset + 1

extern LVAL xacos();
#define FTAB_xacos FTAB_xasin + 1

extern LVAL xatan();
#define FTAB_xatan FTAB_xacos + 1

extern LVAL Prim_FSCANF_FIXNUM();
#define FTAB_Prim_FSCANF_FIXNUM FTAB_xatan + 1

extern LVAL Prim_FSCANF_STRING();
#define FTAB_Prim_FSCANF_STRING FTAB_Prim_FSCANF_FIXNUM + 1

extern LVAL Prim_FSCANF_FLONUM();
#define FTAB_Prim_FSCANF_FLONUM FTAB_Prim_FSCANF_STRING + 1

extern LVAL Prim_COPY_ARRAY();
#define FTAB_Prim_COPY_ARRAY FTAB_Prim_FSCANF_FLONUM + 1

extern LVAL Prim_ARRAY_INSERT_POS();
#define FTAB_Prim_ARRAY_INSERT_POS FTAB_Prim_COPY_ARRAY + 1

extern LVAL Prim_ARRAY_DELETE_POS();
#define FTAB_Prim_ARRAY_DELETE_POS FTAB_Prim_ARRAY_INSERT_POS + 1

extern LVAL Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING();
#define FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING FTAB_Prim_ARRAY_DELETE_POS + 1

extern LVAL Xm_Scale_Widget_Class_Method_SET_VALUE();
#define FTAB_Xm_Scale_Widget_Class_Method_SET_VALUE FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING + 1

extern LVAL Xm_Scale_Widget_Class_Method_GET_VALUE();
#define FTAB_Xm_Scale_Widget_Class_Method_GET_VALUE FTAB_Xm_Scale_Widget_Class_Method_SET_VALUE + 1

extern LVAL Xm_Scroll_Bar_Widget_Class_Method_SET_VALUE();
#define FTAB_Xm_Scroll_Bar_Widget_Class_Method_SET_VALUE FTAB_Xm_Scale_Widget_Class_Method_GET_VALUE + 1

extern LVAL Xm_Scroll_Bar_Widget_Class_Method_GET_VALUE();
#define FTAB_Xm_Scroll_Bar_Widget_Class_Method_GET_VALUE FTAB_Xm_Scroll_Bar_Widget_Class_Method_SET_VALUE + 1

extern LVAL Xm_Scrolled_Window_Widget_Class_Method_SET_AREAS();
#define FTAB_Xm_Scrolled_Window_Widget_Class_Method_SET_AREAS FTAB_Xm_Scroll_Bar_Widget_Class_Method_GET_VALUE + 1

extern LVAL Xm_Selection_Box_Widget_Class_Method_GET_CHILD();
#define FTAB_Xm_Selection_Box_Widget_Class_Method_GET_CHILD FTAB_Xm_Scrolled_Window_Widget_Class_Method_SET_AREAS + 1

extern LVAL Xm_Toggle_Button_Widget_Class_Method_GET_STATE();
#define FTAB_Xm_Toggle_Button_Widget_Class_Method_GET_STATE FTAB_Xm_Selection_Box_Widget_Class_Method_GET_CHILD + 1

extern LVAL Xm_Toggle_Button_Widget_Class_Method_SET_STATE();
#define FTAB_Xm_Toggle_Button_Widget_Class_Method_SET_STATE FTAB_Xm_Toggle_Button_Widget_Class_Method_GET_STATE + 1

extern LVAL Xm_Toggle_Button_Gadget_Class_Method_GET_STATE();
#define FTAB_Xm_Toggle_Button_Gadget_Class_Method_GET_STATE FTAB_Xm_Toggle_Button_Widget_Class_Method_SET_STATE + 1

extern LVAL Xm_Toggle_Button_Gadget_Class_Method_SET_STATE();
#define FTAB_Xm_Toggle_Button_Gadget_Class_Method_SET_STATE FTAB_Xm_Toggle_Button_Gadget_Class_Method_GET_STATE + 1

extern LVAL Wtx_Prim_XT_PARSE_TRANSLATION_TABLE();
#define FTAB_Wtx_Prim_XT_PARSE_TRANSLATION_TABLE FTAB_Xm_Toggle_Button_Gadget_Class_Method_SET_STATE + 1

extern LVAL Wtx_Prim_XT_PARSE_ACCELERATOR_TABLE();
#define FTAB_Wtx_Prim_XT_PARSE_ACCELERATOR_TABLE FTAB_Wtx_Prim_XT_PARSE_TRANSLATION_TABLE + 1

extern LVAL Widget_Class_Method_OVERRIDE_TRANSLATIONS();
#define FTAB_Widget_Class_Method_OVERRIDE_TRANSLATIONS FTAB_Wtx_Prim_XT_PARSE_ACCELERATOR_TABLE + 1

extern LVAL Widget_Class_Method_AUGMENT_TRANSLATIONS();
#define FTAB_Widget_Class_Method_AUGMENT_TRANSLATIONS FTAB_Widget_Class_Method_OVERRIDE_TRANSLATIONS + 1

extern LVAL Widget_Class_Method_UNINSTALL_TRANSLATIONS();
#define FTAB_Widget_Class_Method_UNINSTALL_TRANSLATIONS FTAB_Widget_Class_Method_AUGMENT_TRANSLATIONS + 1

extern LVAL Widget_Class_Method_INSTALL_ACCELERATORS();
#define FTAB_Widget_Class_Method_INSTALL_ACCELERATORS FTAB_Widget_Class_Method_UNINSTALL_TRANSLATIONS + 1

extern LVAL Widget_Class_Method_INSTALL_ALL_ACCELERATORS();
#define FTAB_Widget_Class_Method_INSTALL_ALL_ACCELERATORS FTAB_Widget_Class_Method_INSTALL_ACCELERATORS + 1

extern LVAL Widget_Class_Method_ADD_EVENT_HANDLER();
#define FTAB_Widget_Class_Method_ADD_EVENT_HANDLER FTAB_Widget_Class_Method_INSTALL_ALL_ACCELERATORS + 1

extern LVAL Widget_Class_Method_SET_EVENT_HANDLER();
#define FTAB_Widget_Class_Method_SET_EVENT_HANDLER FTAB_Widget_Class_Method_ADD_EVENT_HANDLER + 1

extern LVAL Widget_Class_Method_BUILD_EVENT_MASK();
#define FTAB_Widget_Class_Method_BUILD_EVENT_MASK FTAB_Widget_Class_Method_SET_EVENT_HANDLER + 1

extern LVAL Weh_Prim_REMOVE_EVENT_HANDLER();
#define FTAB_Weh_Prim_REMOVE_EVENT_HANDLER FTAB_Widget_Class_Method_BUILD_EVENT_MASK + 1

extern LVAL Wxt_Prim_XT_MANAGE_CHILDREN();
#define FTAB_Wxt_Prim_XT_MANAGE_CHILDREN FTAB_Weh_Prim_REMOVE_EVENT_HANDLER + 1

extern LVAL Wxt_Prim_XT_UNMANAGE_CHILDREN();
#define FTAB_Wxt_Prim_XT_UNMANAGE_CHILDREN FTAB_Wxt_Prim_XT_MANAGE_CHILDREN + 1

extern LVAL Widget_Class_Method_ADD_TAB_GROUP();
#define FTAB_Widget_Class_Method_ADD_TAB_GROUP FTAB_Wxt_Prim_XT_UNMANAGE_CHILDREN + 1

extern LVAL Widget_Class_Method_REMOVE_TAB_GROUP();
#define FTAB_Widget_Class_Method_REMOVE_TAB_GROUP FTAB_Widget_Class_Method_ADD_TAB_GROUP + 1

extern LVAL Widget_Class_Method_ADD_GRAB();
#define FTAB_Widget_Class_Method_ADD_GRAB FTAB_Widget_Class_Method_REMOVE_TAB_GROUP + 1

extern LVAL Widget_Class_Method_REMOVE_GRAB();
#define FTAB_Widget_Class_Method_REMOVE_GRAB FTAB_Widget_Class_Method_ADD_GRAB + 1

extern LVAL Widget_Class_Method_IS_COMPOSITE();
#define FTAB_Widget_Class_Method_IS_COMPOSITE  FTAB_Widget_Class_Method_REMOVE_GRAB + 1

extern LVAL Widget_Class_Method_IS_CONSTRAINT();
#define FTAB_Widget_Class_Method_IS_CONSTRAINT  FTAB_Widget_Class_Method_IS_COMPOSITE + 1

extern LVAL Widget_Class_Method_IS_SHELL();
#define FTAB_Widget_Class_Method_IS_SHELL  FTAB_Widget_Class_Method_IS_CONSTRAINT + 1

extern LVAL Widget_Class_Method_IS_PRIMITIVE();
#define FTAB_Widget_Class_Method_IS_PRIMITIVE  FTAB_Widget_Class_Method_IS_SHELL + 1

extern LVAL Widget_Class_Method_IS_GADGET();
#define FTAB_Widget_Class_Method_IS_GADGET  FTAB_Widget_Class_Method_IS_PRIMITIVE + 1

extern LVAL Widget_Class_Method_IS_MANAGER();
#define FTAB_Widget_Class_Method_IS_MANAGER  FTAB_Widget_Class_Method_IS_GADGET + 1

extern LVAL Widget_Class_Method_SET_SENSITIVE();
#define FTAB_Widget_Class_Method_SET_SENSITIVE  FTAB_Widget_Class_Method_IS_MANAGER + 1

extern LVAL Widget_Class_Method_SET_MAPPED_WHEN_MANAGED();
#define FTAB_Widget_Class_Method_SET_MAPPED_WHEN_MANAGED  FTAB_Widget_Class_Method_SET_SENSITIVE + 1

extern LVAL Widget_Class_Method_IS_MANAGED();
#define FTAB_Widget_Class_Method_IS_MANAGED  FTAB_Widget_Class_Method_SET_MAPPED_WHEN_MANAGED + 1

extern LVAL Widget_Class_Method_IS_REALIZED();
#define FTAB_Widget_Class_Method_IS_REALIZED  FTAB_Widget_Class_Method_IS_MANAGED + 1

extern LVAL Widget_Class_Method_IS_SENSITIVE();
#define FTAB_Widget_Class_Method_IS_SENSITIVE  FTAB_Widget_Class_Method_IS_REALIZED + 1

extern LVAL Widget_Class_Method_WINDOW();
#define FTAB_Widget_Class_Method_WINDOW  FTAB_Widget_Class_Method_IS_SENSITIVE + 1

extern LVAL Widget_Class_Method_MAP();
#define FTAB_Widget_Class_Method_MAP  FTAB_Widget_Class_Method_WINDOW + 1

extern LVAL Widget_Class_Method_UNMAP();
#define FTAB_Widget_Class_Method_UNMAP  FTAB_Widget_Class_Method_MAP + 1


#define LAST_FUNTAB_POINTER_USED_BY_libWinterp FTAB_Widget_Class_Method_UNMAP

/*
  since you have to update this file every time you append a new primitive
  or method to w_funtab.c, you might as well update
  INDEX_OF_LAST_FUNTAB_ENTRY_USED_BY_libWinterp. We do a quick
  sanity check in winterp.c upon startup to make sure the number
  of funtab[] entries in this file is correct. This won't check for
  the sequencing of funtab entries being out-of-sync, but then again
  this is all pretty lame anyways.
*/
#define INDEX_OF_LAST_FUNTAB_ENTRY_USED_BY_libWinterp 450

#ifdef 0			/* template */
extern LVAL Widget_Class_Method_();
#define FTAB_Widget_Class_Method_  FTAB_Widget_Class_Method_ + 1
#endif
