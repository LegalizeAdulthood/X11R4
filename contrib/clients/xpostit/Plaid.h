/*
 * $Header$
 *
 * Plaid.h - public definitions for the plaid widget.
 *
 * Based on the Template widget from the X11R3 distribution.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94086
 * davy@riacs.edu
 *
 * $Log$
 */
#include <X11/copyright.h>

#ifndef _Plaid_h
#define _Plaid_h

/*
 * Resources:
 *
 * Name		     Class		RepType		Default Value
 * ----		     -----		-------		-------------
 * background	     Background		Pixel		XtDefaultBackground
 * border	     BorderColor	Pixel		XtDefaultForeground
 * borderWidth	     BorderWidth	Dimension	1
 * callback	     Callback		Callback	NULL
 * destroyCallback   Callback		Pointer		NULL
 * foreground	     Foreground		Pixel		XtDefaultForeground
 * height	     Height		Dimension	0
 * mappedWhenManaged MappedWhenManaged	Boolean		True
 * sensitive	     Sensitive		Boolean		True
 * width	     Width		Dimension	0
 * x		     Position		Position	0
 * y		     Position		Position	0
 */

/*
 * Declare specific PlaidWidget class and instance datatypes.
 */
typedef struct _PlaidClassRec*		PlaidWidgetClass;
typedef struct _PlaidRec*		PlaidWidget;

/*
 * Declare the class constant.
 */
extern WidgetClass			plaidWidgetClass;

#endif _Plaid_h
