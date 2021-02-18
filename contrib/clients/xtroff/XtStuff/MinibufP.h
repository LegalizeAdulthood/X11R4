/* 
 * MinibufP.h - Private definitions for Minibuf widget
 * 
 */

#ifndef _XtMinibufP_h
#define _XtMinibufP_h

/***********************************************************************
 *
 * Minibuf Widget Private Data
 *
 ***********************************************************************/

#include "Minibuf.h"

/* New fields for the Minibuf widget class record */

typedef struct {int foo;} MinibufClassPart;

/* Full class record declaration */
typedef struct _MinibufClassRec {
	CoreClassPart		core_class;
	MinibufClassPart	minibuf_class;
} MinibufClassRec;

extern MinibufClassRec minibufClassRec;

/* New fields for the Minibuf widget record */
typedef struct {
	/* resources */
	Pixel		foreground;
	XFontStruct	*finfo;
	Cursor		cursor;
	Widget		typeout;

	/* Callback stuff */
	XtCallbackList finishedCallback;/* 
									 *  Invoked on "newline" event
									 *  (usually ^M) or "abort" event
									 *  (usually ^G)
									 */

	/* private state */
	GC			normal_gc;
	GC          invert_gc;
	int cursorX;			/* Physical position of cursor in window */

	/* Logical info */
	char *string;			/* Actual buffer */
	int startPos;			/* First character of string displayed in window */
	int cursorPos;			/* Position of the cursor in the buffer */
	int mark;
	char *killBuffer;
	int killBufferLen;
	int killBufferSize;
	int size;				/* Size of the string buffer */
	int len;				/* Actual length of the string */
	int cols;				/* Width of buffer in columns */
	int promptLen;			/* First promptLen chars of string are 
							   non-editable prompt */
	int arg;				/* Universal argument */
	int typeoutColumns;		/* Number of columns in the typeout window */
	int inputMode;			/* True, if we are in input mode */
	int cursorOn;			/* Records the state of the cursor */
	int completion;			/* Are we doing completion */
} MinibufPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _MinibufRec {
	CorePart	core;
	MinibufPart	minibuf;
} MinibufRec;

#endif _XtMinibufP_h
