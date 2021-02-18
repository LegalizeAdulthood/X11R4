/* $Header: handlers.c,v 1.5 89/07/02 15:58:18 moraes Exp $ */
#include "xpic.h"
#include "windows.h"
#include "newfonts.h"
#include "input.h"
#include "version.h"
#include "gels.h"
#include "patchlevel.h"
#include "draw.h"

extern void RedrawPicWin();
extern void CleanUpMode();

/*
 *  If a PushButton has been pressed in a mode other than START_MODE,
 *  there's probably rubber-banded stuff on teh screen that we'll have
 *  to clean up
 */
#define RESETMODE()	if(drawingMode != START_MODE) CleanUpMode(); else

/*ARGSUSED*/
void copy(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();

	editType = COPY;
	objectType = editMode;
}

/*ARGSUSED*/
void cut(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();

	editType = DELETE;
	objectType = editMode;
}


/*ARGSUSED*/
void paste(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();

	editType = PASTE;
	objectType = editMode;
}


/*ARGSUSED*/
void move(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();

	editType = MOVE;
	objectType = editMode;
}



/*ARGSUSED*/
void change(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();

	editType = CHANGE_ATTRIBUTE;
	objectType = editMode;
}



/*ARGSUSED*/
void adjust(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();

	editType = ADJUST;
	objectType = editMode;
}



/*ARGSUSED*/
void rotate(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();

	message("rotate: Not implemented yet. Sorry.");
}


/*ARGSUSED*/
void scale(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();

	message("scale: Not implemented yet. Sorry.");
}



/*ARGSUSED*/
void getcell(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();

	editType = GET;
	objectType = editMode;
}


/*ARGSUSED*/
void putcell(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();

	editType = PUT;
	objectType = editMode;
}


/*ARGSUSED*/
void element(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();

	/* Make sure everything is selectable - should be, but you never know! */
	ClearGelFlags(CurrentCell->gelList);
	
	if (STREQ(tag, "Line"))
		objectType = LINE;
	else if (STREQ(tag, "Box"))
		objectType = BOX;
	else if (STREQ(tag, "Ellipse"))
		objectType = ELLIPSE;
	else if (STREQ(tag, "Circle"))
		objectType = CIRCLE;
	else if (STREQ(tag, "Spline"))
		objectType = SPLINE;
	else if (STREQ(tag, "Text"))
		objectType = TEXT;
	else {
		(void) sprintf("Unknown element - %s", errstring);
		message(errstring);
	}
}


/*ARGSUSED*/
void editattrib(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();

	if (STREQ(tag, "Block")) {
		editMode = BLOCK;
		if (objectType == ELEMENT)
			objectType = BLOCK;
	} else if (STREQ(tag, "Element")) {
		editMode = ELEMENT;
		if (objectType == BLOCK)
			objectType = ELEMENT;
	} else {
		(void) sprintf(errstring,
		 "editattrib: Unknown editing mode - Tag = %s", tag);
		message(errstring);
	}
}


/*ARGSUSED*/
void linepattern(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();

	if (STREQ(tag, "Solid"))
		line_type = SOLID;
	else if (STREQ(tag, "Dotted"))
		line_type = DOTTED;
	else if (STREQ(tag, "Short-Dashed"))
		line_type = SDASH;
	else if (STREQ(tag, "Long-Dashed"))
		line_type = LDASH;
	else if (STREQ(tag, "Dot-Dashed"))
		line_type = DDASH;
	else {
		(void) sprintf(errstring,
		 "linepattern: Unknown line pattern - Tag = %s",tag);
		message(errstring);
	}
	SETDASHES(gcNormal, line_type)
	SETDASHES(gcInvert, line_type)
}


/*ARGSUSED*/
void linearrow(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();
	
	if (STREQ(tag, "None"))
		line_arrow = NO_ARROW;
	else if (STREQ(tag, "Start"))
		line_arrow = ST_ARROW;
	else if (STREQ(tag, "End"))
		line_arrow = EN_ARROW;
	else if (STREQ(tag, "Both"))
		line_arrow = ST_ARROW | EN_ARROW;
	else {
#ifdef DEBUG
		(void) sprintf(errstring,
		 "linearrow: Unknown line arrow - Tag = %s", tag);
		message(errstring);
#endif
	}
}


/*ARGSUSED*/
void linethickness(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();
	lineThickness = atoi(tag);
	if (lineThickness< 0) {
		message("Line Thickness must be positive");
		lineThickness = 0;
	}
	setwidth(gcNormal, lineThickness);
	setwidth(gcInvert, lineThickness);
}


/*ARGSUSED*/
void textvalign(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();
	if (STREQ(tag, "Top"))
		textVertAlign = TOPLINE;
	else if (STREQ(tag, "Middle"))
		textVertAlign = MIDLINE;
	else if (STREQ(tag, "Bottom"))
		textVertAlign = BOTLINE;
}


/*ARGSUSED*/
void texthalign(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();
	if (STREQ(tag, "Centred"))
		textHorizAlign = CENTRE;
	else if (STREQ(tag, "Left Just."))
		textHorizAlign = LJUST;
	else if (STREQ(tag, "Right Just."))
		textHorizAlign = RJUST;
}


/*ARGSUSED*/
void setsnap(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();
	mouseResolution = atoi(tag);
	if (mouseResolution < 0) 
		message("Mouse resolution must be positive");
}


/*
 *  Status is printed on the message line - it prints certain global
 *  variables, like the program name, version number, buffername, the
 *  filename associated with the buffer, the * to show if the buffer was
 *  saved or not, and the present drawing mode
 */
void DisplayStatus()
{

	char *modified = " ";
	
	if (CurrentCell->saved & MODIFIED)
		modified = "[Modified]";
	(void) sprintf(errstring, "XPIC %d.%d%s   Buffer: %s    File: %s  %s", 
	 progVersion, PATCHLEVEL, progStatus, CurrentCell->name, 
	 CurrentCell->filename, modified);
	message(errstring);
}


/* Callback entry point */
/*ARGSUSED*/
void status(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();

	DisplayStatus();
}



/*ARGSUSED*/
void readcell(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	Cell *cell;
	
	RESETMODE();

	/* This will not be necessary when multiple buffers become available */
	if (CurrentCell->saved & MODIFIED) {
		message("You must first save the current cell");
		return;
	}
	
	if ((cell = ReadCell("Read file name ? ", (char *) NULL)) == NULL)
		return;

	cell->next = MainCell;
	MainCell = cell;
	LastCell = CurrentCell;
	CurrentCell = MainCell;
	RedrawPicWin();
	DisplayStatus();
}


/*ARGSUSED*/
void lprintcell(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();
	LPrintCell(CurrentCell);
}


/*ARGSUSED*/
void savecell(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	char *fname;
	
	RESETMODE();

	if(STREQ(tag, "Save As")) {
		fname = get_input("Save file name ? ", CurrentCell->filename, TRUE);
		if (fname == NULL)
			return;
		if (CurrentCell->filename && !(STREQ(CurrentCell->filename, nullfile)))
			free(CurrentCell->filename);
		CurrentCell->filename = fname;
		/* Force a save */
		CurrentCell->saved = MODIFIED | NEWFILE;
	}
	(void) WriteCell(CurrentCell, backupOnWrite);
}


/*ARGSUSED*/
void redisplay(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();
	RedrawPicWin();
	DisplayStatus();
}


/*ARGSUSED*/
void setgrid(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	Arg args[1];
	
	RESETMODE();
	gridOn = !gridOn;
	if (gridOn) {
		XtSetArg(args[0], XtNbackgroundPixmap, gridTile);
		XtSetValues(picWidget, args, 1);
	} else {
		XtSetArg(args[0], XtNbackgroundPixmap, blankTile);
		XtSetValues(picWidget, args, 1);
	}
}


/*ARGSUSED*/
void undo(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	Gel *g;
 	register Gel *tmp;
	int i;
	
	RESETMODE();

	g = PopGel(&(CurrentCell->gelList), CurrentCell->undo);
	for(tmp = g; tmp != NULL; tmp = tmp->next)
		GelDraw(tmp, ERASE);
	for(tmp = CurrentCell->undoList; tmp != NULL; tmp = tmp->next)
		GelDraw(tmp, DRAW);
	ClearGelFlags(CurrentCell->undoList);
	i = PushGel(&(CurrentCell->gelList), CurrentCell->undoList);
	CurrentCell->undo = i;
	CurrentCell->undoList = g;
	CurrentCell->saved |= MODIFIED;
	
}

	

/*ARGSUSED*/
void quit(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	Cell *cell;
	
	RESETMODE();
	/* Check all the buffers to make sure they're saved */
	for (cell = MainCell; cell != NULL; cell = cell->next) {
		if (cell->saved & MODIFIED) {
			(void) sprintf(errstring, "Buffer \"%s\" not saved. Save (y/n) ?",
			 cell->name);
			switch ( confirm(errstring, "y")) {
				case ABORT:
					return;
				case YES:
					if (WriteCell(cell, backupOnWrite))
						break;
					else
						return;
				case NO:
					break;
			}
		}
	}
	exit(0);
}

/*ARGSUSED*/
void change_buffer(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();
	message("change_buffer: Not implemented yet. Sorry.");
}


/*ARGSUSED*/
void kill_buffer(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();
	message("kill_buffer: Not implemented yet. Sorry.");
}


#ifdef DEBUG
/*ARGSUSED*/
void printcell(w, tag, calldata)
Widget w;
caddr_t tag;
caddr_t calldata;
{
	RESETMODE();
	PrintCell(CurrentCell);
}
#endif
