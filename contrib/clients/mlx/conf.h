
#define	NAME_LEN	13
#define	FN_LEN		15
#define	KBD_LEN		12
#define METHOD		15

typedef	struct	_config{
	char	code;		/* Status */
	char	name[NAME_LEN]; /*language name, keyword */
	char	class;		/* L,S,B */
	char	nor_font[FN_LEN];	/* Normal font */
	char	bld_font[FN_LEN];	/* bold   font */
	char	direction[3];	/* Writing direction */
	char	keybd[KBD_LEN];	/* keyboard layout */
}	Config;

#define REC_LEN	sizeof(Config)

