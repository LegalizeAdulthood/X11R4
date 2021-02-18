%{
#define ManageContext 1
#define DirectiveContext 2
static int oops_context ;

static	void convert_nl () ;
static	void convert_numeric () ;

static Boolean	first_time = True ; /* recognizes the first time for yylex()*/
static Boolean	nested_args = False ; 

extern int lex_bug ;

static char *ttt ;

#undef input
#undef unput

/***
#include "y.tab.h"
***/
%}

%START	string

%%

"/*"  /* look for start of comment */
    {
    char c ;
    for (;;)
	{
	while ((c = input()) != '*')
	    ;
	if ((c = input()) == '/')
	    break ;
	unput (c) ;
	}
    }

["]["]  /* look for start of string */
    {
    Cu_copy_ds (&ttt, "") ;
    yylval.s_val = ttt ;
    return STRING ;
    }

["]  /* look for start of string */
    {
    BEGIN string ;
    }

<string>[^"]*\"  /* anything but another quote */
    {
    yytext[yyleng-1] = '\0' ; /* trash the trailing quote */
    Cu_copy_ds (&ttt, yytext) ;
    convert_nl (ttt) ;	/* convert '\' 'n' to '\n' and remove '\' '\n' */
    convert_numeric (ttt) ;/* convert '\xxx' to number */
    yylval.s_val = ttt ;

    BEGIN 0 ;
    return STRING ;
    }

if		/* if keyword */
	{
	return IF_KEY ;
	}

XtSetValues	/* set_values keyword */
	{
	return SET_VALUES_KEY ;
	}

\#include|include		/* include keyword */
	{
	return INCL_KEY ;
	}

manage		/* manage keyword */
	{
	return MANAGE_KEY ;
	}

[^ \t\n\{\}\:\(\)\[\]\"\,\=\&\!\<\>\*]*	/* item (anything but punctuation) */
	{
	Cu_copy_ds (&ttt, yytext) ;
	yylval.s_val = ttt ;
	return WORD ;
	}

[\{\}\<\>\(\)\[\]\:\,\=\&\!\*]				/* punctuation */
	{
	return (yytext[0]) ;
	}

[ \n\t]		/* white space	*/
	{
	;
	}

.	{
	sprintf (error_text, "LEX returning ERROR on char (%c)\n", *yytext) ;
	XtWarning (error_text) ;
	return ERROR ;
	}
%%

static void
convert_nl (text)
    char *text ;
{
Cardinal i ;
for (i=0;  i < strlen (text);  i++)
	{
	if (text[i] == '\\' && text[i+1] == 'n')
		{
		Cardinal j ;
		text[i] = '\n' ;
		j = i+1 ;
		while (text[j] != 0)
			{
			text[j] = text[j+1] ;
			j++ ;
			}
		}
	else
	if (text[i] == '\\' && text[i+1] == '\n')
		{
		Cardinal j = i ;
		while (1)
			{
			text[j] = text[j+2] ;
			if (text[j] == '\0')
				break ;
			j++ ;
			}
		}
	}
return ;
}

static void
convert_numeric (text)
    char *text ;
{
Cardinal i ;
for (i=0;  i < strlen (text);  i++)
	{
	if (text[i] == '\\' &&  isdigit(text[i+1]) &&
				isdigit(text[i+2]) &&
				isdigit(text[i+3]))
		{
		Cardinal j = i+1 ;
		int ival ;
		char save = text[i+4] ;
		text[i+4] = '\0' ;
		sscanf (&text[j], "%o", &ival) ;
		text[i] = ival ;
		text[i+4] = save ;
		do
			{
			text[j] = text[j+3] ;
			}
			while (text[j++] != '\0') ;
		}
	}
return ;
}

