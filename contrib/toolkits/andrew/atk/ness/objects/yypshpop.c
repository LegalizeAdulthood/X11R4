/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: yypshpop.c,v 1.1 89/12/04 20:22:46 xguest Exp $ */
/* $ACIS:$ */
/* $Source: /xsrc/contrib/toolkits/andrew/atk/ness/objects/RCS/yypshpop.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *yypushpop_rcsid = "$Header: yypshpop.c,v 1.1 89/12/04 20:22:46 xguest Exp $";
#endif


struct yaccsave {
	struct errornode *ErrorList;
	YYSTYPE yylval, yyval;
#ifdef YYDEBUG
	int yydebug;
#endif
	YYSTYPE yyv[YYMAXDEPTH];
	int yychar;
	int yynerrs;
	short yyerrflag;
	struct yaccsave *next;
#if SY_AIXx
	int yys[ YYMAXDEPTH ];
	YYSTYPE *yypv, *yypvt;
	int *yyps, yystate, yytmp;
#endif  /* SY_AIXx */
} *yyyystack = NULL;

	void
yypushcompile()
{
	struct yaccsave *T;
	T = (struct yaccsave *)malloc(sizeof(struct yaccsave));

	T->ErrorList = ErrorList;
	T->yylval = yylval;
	T->yyval = yyval;
#ifdef YYDEBUG
	T->yydebug = yydebug;
#endif
	bcopy(yyv, T->yyv, sizeof(yyv));	/* T->yyv = yyv; */
	T->yychar = yychar;
	T->yynerrs = yynerrs;
	T->yyerrflag = yyerrflag;
#if SY_AIXx
	bcopy(yys, T->yys, sizeof(yys));
	T->yypv = yypv;
	T->yypvt = yypvt;
	T->yyps = yyps;
	T->yystate = yystate;
	T->yytmp = yytmp;
#endif  /* SY_AIXx */

	genPush();

	T->next = yyyystack;
	yyyystack = T;
}

yypopcompile()
{
	struct yaccsave *T = yyyystack;
	if (T == NULL) return;

	ErrorList = T->ErrorList;
	yylval = T->yylval;
	yyval = T->yyval;
#ifdef YYDEBUG
	yydebug = T->yydebug;
#endif
	bcopy(T->yyv, yyv, sizeof(yyv));	/* yyv = T->yyv; */
	yychar = T->yychar;
	yynerrs = T->yynerrs;
	yyerrflag = T->yyerrflag;
#if SY_AIXx
	bcopy(T->yys, yys, sizeof(yys));
	yypv = T->yypv;
	yypvt = T->yypvt;
	yyps = T->yyps;
	yystate = T->yystate;
	yytmp = T->yytmp;
#endif  /* SY_AIXx */

	genPop();

	yyyystack = T->next;
	free(T);
}

	struct errornode *
yycompile(ness)
	struct ness *ness;
{
	struct errornode *errlist;

	yypushcompile();
	ness->AttrDest = &ness->globals;		/* where to put defined symbols */
	SetupErrorHandling();
	curNess = ness;
	curLex = ness_GetLex(ness);
	lex_SetCurrent(curLex);
	ness->CurrentObject = NULL;

	yyparse();

	errlist = ErrorList;
	yypopcompile();
	return errlist;
}
