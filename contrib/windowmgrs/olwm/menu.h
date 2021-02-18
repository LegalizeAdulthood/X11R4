/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

/* @(#) menu.h 5.1 89/12/14 Crucible */

/* a handy typedef for pointers to functions returning int */
typedef int (*FuncPtr)();

/* Button stacks (menus) are implemented as lists of buttons.
 * Each button in a stack may in turn be stacked, this is indicated by
 * the stacked flag of the button. If this flag is True then the buttonAction
 * is a pointer to a new stack. Otherwise it is the function to be called
 * after the menu has been dispatched.
 */
typedef struct {
	FuncPtr		callback;	/* if not stacked; call this */
	struct _menu	*submenu;	/* actually a Menu pointer, but */
					/* Menu hasn't been defined yet */
} ButtonAction;

/* possible button states */
typedef enum {Disabled, Enabled} ButtonState;

typedef struct _button {
	char		*label;		/* displayed text */
	Bool		stacked;	/* True if this is a button stack */
	ButtonState	state;		/* Enabled/Disabled */
	ButtonAction	action;

	/* following are filled in when menu is created */
	int		labelX, labelY, menuMarkX;
	int		activeX, activeY, activeW, activeH;
} Button;

typedef struct _menu {
	char		*title;
	Button		*buttons;
	int		buttonCount;
	int		buttonDefault;
	Bool		hasPushPin;
	FuncPtr		pinAction;

	/* filled in when menu is created */
	Window		window;
	Window		shadow;
	int		titleHeight, titleWidth, titleX, titleY;
	int		pushPinX, pushPinY;
	int		x, y;
	int		width, height;

	Bool		currentlyDisplayed;  /* If this is true we gray out
					      * the pushpin when we display
					      * the menu. */
	struct _menu *originalMenu;	/* if this menu is pinned, this
					 * points to the original menu
					 * template */
} Menu;
