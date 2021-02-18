/* Find closest TeX font we know about. */
extern char *ChangeFont( /* style, size */);

/* Write "\font\twelverm cmr10 scaled \magstep1" etc for each font used. */
extern void PutTexFontPostScript( /* fd */);

/* structure for mapping xpic fonts to TeX initialization commands */
struct fontinfo {
	char *style;
	int size;
	char *texname;
	char *texfontinit;
	int refcnt;
};

extern struct fontinfo *availfonts;
extern int navailfonts;
extern char *def_font;
