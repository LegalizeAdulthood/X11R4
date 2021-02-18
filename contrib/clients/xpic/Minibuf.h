/* $Header: Minibuf.h,v 1.1 88/08/18 23:54:16 moraes Exp $ */
#ifndef _XtMinibuf_h
#define _XtMinibuf_h

#include <X11/StringDefs.h>

/* New resources */
#define XtNfinishedCallback	"finishedCallback"
#define XtNcursor			"cursor"
#define XtNtypeoutColumns	"typeoutColumns"
#define XtCTypeoutColumns	"TypeoutColumns"
#define XtNtypeout		"typeout"
#define XtCtypeout		"Typeout"

/* Class record pointer */
extern WidgetClass minibufWidgetClass;

/* C Widget type definition */
typedef struct _MinibufRec *MinibufWidget;

/* Public routines - those that you're supposed to call anyway! */
extern void MinibufGetInput();
extern void MinibufDisplayMessage();


#endif _XtMinibuf_h
/* DON'T ADD STUFF AFTER THIS #endif */
