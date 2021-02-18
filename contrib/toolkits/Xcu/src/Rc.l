%{

#undef input
#undef unput

%}

%%

[0-9]+ 			/* number string */
	{
	yylval.i_val = atoi (yytext) ;
	return NUMBER ;
	}

x			/* x */
	{
	return yytext[0] ;
	}

[ \t\n]*		/* white space */
	{
	    ;
	}

.   			/* anything else */
    {
    char *my_text = "Rc.lex error with (%s)\n" ;
    char *etext = (char *) XtMalloc (strlen(my_text) + strlen(yytext)) ;
    sprintf (etext, my_text, yytext) ;
    XtError (etext) ;
    }
%%

