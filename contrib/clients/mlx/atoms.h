
/* atoms.h      define  common  Atoms */
#include	<X11/X.h>

typedef	struct	{
	Window	flag;		/* Is a MLcode or Break signal */
	char	code[4];	/* 1 ML code */
}	ML_code;

char	hancode1[]="Hanzi Code";
char	hancode2[]="Hanzi Type";

