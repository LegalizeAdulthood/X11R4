extern int errno;
char *malloc();

struct box {
	int w, h;
	int x, y;
};

struct character {
	char    *charId;
	int     encoding;
	int     swidth[2];
	int     dwidth[2];
	struct  box bbx;
	int     nrows;
	char    **rows;
};

struct font {
	int     rev, subrev;
	char    *comment;
	char    *fontname;
	int     sizes[3];
	struct box     boundingbox;
	int     nprop;
	int     descentValid, descent;
	int     ascentValid, ascent;
	int     copyrightValid;
	char    *copyright;
	int     nchars;
	struct box maxbbx;
	struct  character *characters;
	int	defaultCharValid, defaultChar;
};

struct font font;
