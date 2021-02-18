/*
 *	File:		gr_def.h
 *	Contents:	Header file containing all the typedefs and defs used
 *				by the graphics module.
 */

#define STRNG40				40
#define STRNG80				80
#define STRNG160			160

#define MAX_CUBLEV_WINDS	1
#define MAX_DSPLEV_WINDS	1

#define MAX_TOPLEV_WIDTH	400
#define MAX_TOPLEV_HEIGHT	500
#define MAX_CUBLEV_WIDTH	400
#define MAX_CUBLEV_HEIGHT	500

#define DEF_BORDER_WIDTH	3

#define DEF_LABEL_FONT		"8x13bold"

typedef enum {
	CNTRLWIND, CUBEWIND, DSPLWIND
} A_Wind_t;

typedef enum {
	NOSCROLL, VERTONLY, HORIZONLY, VERTHORIZ
} A_Scroll_t;

typedef enum {
	SELECTABLE, NONSELECTABLE
} A_TextSelect_t;

typedef enum {
	FILESOURCE, STRINGSOURCE
} A_TextSource_t;

/*
typedef struct {
	Widget			id;
	char			*label;
	XtCallbackProc	callBackProc;
	int				x,y;
	int				width,height;
} A_Button_t;
*/
