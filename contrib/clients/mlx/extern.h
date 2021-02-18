
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

extern char *malloc();
extern saved,hcc;
extern char spaces[];
extern	char CODE[];
extern	Window	dialogwin,promptwin;
extern	GC	normalGC, asciiGC;
extern	char	*TblAddr, save_hanzi[],save_dialog[],save_count[];
extern	int	char_width,char_height;

extern Widget CreateWindow(), CreateButton(), LingBoxes();
extern char	*intTOstr();

extern Display *CurDpy;

extern	language(), DoHelp(), DoHide(), MapInput(), DoQuit(), DoNull(),
	DoCommon(), Input(), HandleKeyPress(), Choice(),
	RedrawDialog(), RedrawPrompt();

extern	short	hzflag;
extern	char	hzcode[],hzbuf[],char_f[];
extern	int	n,m,Hifreq_flg;
extern	int	L_flag;


