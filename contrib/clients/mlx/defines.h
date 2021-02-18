
#include	<ctype.h>

#define escape  0x1b
#define	delete	0x7f
#define bspace  0x8
#define mlspace	0xa9a1
#define isletter(x)	(isupper(x) || islower(x))
#define ischoicekey(c)	((c&0x80) || (c==' '))
#define issmallkbd(c)	(isdigit(c) || (c=='.') || (c=='-') || (c=='+'))

#define	fkeyno	18
#define WID	110+32*fkeyno
#define HIG	86
#define NUM	15	
#define	W1	86
#define	Y1	24
#define	Y2	48
#define	Y3	64

