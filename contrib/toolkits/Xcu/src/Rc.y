%{

%}

%start	init

%union	{
	float	f_val ;
	int	i_val ;
	char	*s_val ;
	char	c_val ;
	}

%token <i_val>	NUMBER
%token 		ERROR

%%

init	:
	| init row
	;

row : NUMBER x NUMBER
	{
	int i, j ;
	for (i=0;  i < $1;  i++)
		{
		for (j=0;  j < $3;  j++)
			{
			*output_format++ = 'c' ;
			*output_format++ = ' ' ;
			}
		if (i < $1 - 1)
		    *output_format++ = '\n' ;
		else
		    *output_format++ = '.' ;
		}
	}

    ;

x   : 'x'
    | 'X'
    ;

%%

#include "Rc.lex.c"
