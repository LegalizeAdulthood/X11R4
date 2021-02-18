#ifndef lint
static char	*RCSid = "$Header: /u5/davy/progs/xpostit/RCS/note.c,v 1.3 89/01/10 09:57:24 davy Exp $";
#endif

/*
 * note.c - routines for handling notes.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94086
 * davy@riacs.edu
 *
 * $Log:	note.c,v $
 * Revision 1.3  89/01/10  09:57:24  davy
 * Changed XGetNormalHints to XGetWindowAttributes for getting the size of
 * the text window when saving.  XGetNormalHints comes up with the wrong
 * size, for some reason.
 * 
 * Revision 1.2  89/01/10  09:13:13  davy
 * Added XtChain... arguments to buttons and text window to prevent the
 * buttons from getting screwed up on a resize.
 * 
 * Revision 1.1  89/01/10  09:00:21  davy
 * Initial revision
 * 
 */
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/AsciiText.h>
#include <X11/Command.h>
#include <X11/Shell.h>
#include <X11/Form.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <stdio.h>

#include "xpostit.h"

static PostItNote	*notes = NULL;

static PostItNote	*FindNote();
static PostItNote	*AllocNote();

static void		SaveNote();
static void		EraseNote();
static void		DestroyNote();
static void		CancelErase();
static void		ConfirmErase();
static void		CancelDestroy();
static void		ConfirmDestroy();
static void		MakeNoteWidget();
static void		SetTextOptions();

/*
 * CreateNewNote - create a new note of the specified size.
 */
void
CreateNewNote(size)
int size;
{
	static int hpi = 0;
	static int wpi = 0;
	register PostItNote *pn;
	register int hmm, wmm, hpixel, wpixel;

	/*
	 * Find out the number of pixels per inch on the screen.  We
	 * can get the number of pixels, and the size in millimeters.
	 * Then we convert to pixels/inch using the formula
	 *
	 *       2.54 cm     10 mm     pixels     pixels
	 *	--------- x ------- x -------- = --------
	 *	  inch        cm         mm        inch
	 *
	 * The only problem with this is that some servers (like Xsun)
	 * lie about what these numbers really are.
	 */
	if ((hpi == 0) || (wpi == 0)) {
		hpixel = DisplayHeight(display, DefaultScreen(display));
		wpixel = DisplayWidth(display, DefaultScreen(display));

		hmm = DisplayHeightMM(display, DefaultScreen(display));
		wmm = DisplayWidthMM(display, DefaultScreen(display));

		hpi = (int) ((25.4 * hpixel) / (float) hmm + 0.5);
		wpi = (int) ((25.4 * wpixel) / (float) wmm + 0.5);
	}

	/*
	 * Calculate sizes for the note.
	 */
	switch (size) {
	case PostItNote_1p5x2:
		hpixel = 1.5 * hpi;
		wpixel = 2 * wpi;
		break;
	case PostItNote_3x3:
		hpixel = 3 * hpi;
		wpixel = 3 * wpi;
		break;
	case PostItNote_3x5:
		hpixel = 3 * hpi;
		wpixel = 5 * wpi;
		break;
	}

	/*
	 * Allocate a new note structure.
	 */
	pn = AllocNote(NewIndex);

	/*
	 * Set the text window size.
	 */
	pn->pn_textwidth = wpixel;
	pn->pn_textheight = hpixel;

	/*
	 * Make the widget for the note.
	 */
	MakeNoteWidget(pn);
}

/*
 * LoadSavedNotes - load in the notes the user has saved.
 */
void
LoadSavedNotes()
{
	DIR *dp;
	FILE *fp;
	char *realloc();
	register PostItNote *pn;
	register struct direct *d;
	char buf[BUFSIZ], fname[MAXPATHLEN];
	int n, len, nlen, shellx, shelly, texth, textw;

	/*
	 * Try to open the directory.
	 */
	if ((dp = opendir(app_res.note_dir)) == NULL)
		return;

	nlen = strlen(PostItNoteFname);

	/*
	 * For each entry...
	 */
	while ((d = readdir(dp)) != NULL) {
		/*
		 * Skip over anything which doesn't match our
		 * file naming scheme.
		 */
		if (strncmp(d->d_name, PostItNoteFname, nlen) != 0)
			continue;

		/*
		 * Make the full path name.
		 */
		sprintf(fname, "%s/%s", app_res.note_dir, d->d_name);

		/*
		 * Open the file.
		 */
		if ((fp = fopen(fname, "r")) == NULL)
			continue;

		/*
		 * Look for the magic cookie identifying this as
		 * a Post-It note.
		 */
		if ((fscanf(fp, "%s", buf) == EOF) ||
		    (strcmp(buf, PostItNoteMagic) != 0)) {
			fclose(fp);
			continue;
		}

		/*
		 * Get the note position and size information.
		 */
		fgets(buf, sizeof(buf), fp);

		n = sscanf(buf, "%d %d %d %d %d", &shellx, &shelly, &texth,
				&textw, &len);

		/*
		 * Bad format; skip it.
		 */
		if (n != 5) {
			fclose(fp);
			continue;
		}

		/*
		 * Get the index number of this note.
		 */
		n = atoi(&(d->d_name[nlen]));

		/*
		 * Get a note structure.
		 */
		pn = AllocNote(n);

		/*
		 * Set the information.
		 */
		pn->pn_shellx = shellx;
		pn->pn_shelly = shelly;
		pn->pn_textwidth = textw;
		pn->pn_textheight = texth;
		pn->pn_positionit = True;

		/*
		 * Save the file name.
		 */
		pn->pn_file = SafeAlloc(strlen(fname) + 1);
		strcpy(pn->pn_file, fname);

		/*
		 * If we need a bigger buffer than the default,
		 * get one.
		 */
		if (len >= pn->pn_textsize) {
			n = (len + app_res.buf_size - 1) / app_res.buf_size;
			n = n * app_res.buf_size;

			if ((pn->pn_text = realloc(pn->pn_text, n)) == NULL) {
				fprintf(stderr, "xpostit: out of memory.\n");
				ByeBye();
			}

			pn->pn_textsize = n;
		}

		/*
		 * Read in the text.
		 */
		fread(pn->pn_text, sizeof(char), len, fp);
		fclose(fp);

		/*
		 * Make a widget for this note.
		 */
		MakeNoteWidget(pn);
	}

	closedir(dp);
}

/*
 * RaiseAllNotes - raise all the notes by raising their shell windows.
 */
void
RaiseAllNotes()
{
	register PostItNote *pn;

	for (pn = notes; pn != NULL; pn = pn->pn_next)
		XRaiseWindow(display, XtWindow(pn->pn_shellwidget));
}

/*
 * LowerAllNotes - lower all the notes by lowering their shell windows.
 */
void
LowerAllNotes()
{
	register PostItNote *pn;

	for (pn = notes; pn != NULL; pn = pn->pn_next)
		XLowerWindow(display, XtWindow(pn->pn_shellwidget));
}

/*
 * SaveAllNotes - save all the notes.
 */
void
SaveAllNotes()
{
	register PostItNote *pn;

	for (pn = notes; pn != NULL; pn = pn->pn_next)
		SaveNote(pn->pn_shellwidget, (caddr_t) pn->pn_index, 0);
}

/*
 * MakeNoteWidget - make a widget for a Post-It note.
 */
static void
MakeNoteWidget(pn)
PostItNote *pn;
{
	int options;
	Arg args[16];
	char geometry[64];
	Widget form, widgets[4];
	XtCallbackRec callbacks[2];
	register int nargs, nwidgets;
	char	title[128];
	char	icon_name[128];

	bzero(callbacks, sizeof(callbacks));
	nargs = 0;

	/*
	 * If the shell window coordinates are valid, use them.
	 */
	if (pn->pn_positionit) {
		sprintf(geometry, "+%d+%d", pn->pn_shellx, pn->pn_shelly);
		SetArg(XtNgeometry, geometry);
		SetArg(XtNx, pn->pn_shellx);
		SetArg(XtNy, pn->pn_shelly);
	}

	sprintf (title, "PostIt Note %d", pn->pn_index);
	sprintf (icon_name, "PostIt %d", pn->pn_index);
	SetArg (XtNtitle, title);
	SetArg (XtNiconName, icon_name);
	/*
	 * Make the shell widget for this note.
	 */
	pn->pn_shellwidget = XtAppCreateShell("postItNote", "XPostit",
			topLevelShellWidgetClass, XtDisplay (toplevel), args, nargs);

	/*
	 * Make a form widget.
	 */
	form = XtCreateWidget("form", formWidgetClass, pn->pn_shellwidget,
			NULL, 0);

	nwidgets = -1;

	/*
	 * Put each button into the shell widget.  The second and third
	 * buttons will position themselves relative to the first one;
	 * the first one will position itself relative to the edge of
	 * the form.
	 */
	SetCallback(SaveNote, pn->pn_index);

	nargs = 0;
	SetArg(XtNtop, XtChainTop);
	SetArg(XtNleft, XtChainLeft);
	SetArg(XtNright, XtChainLeft);
	SetArg(XtNbottom, XtChainTop);
	SetArg(XtNcallback, callbacks);
	widgets[++nwidgets] = XtCreateWidget("save", commandWidgetClass,
			form, args, nargs);

	SetCallback(EraseNote, pn->pn_index);

	nargs = 0;
	SetArg(XtNtop, XtChainTop);
	SetArg(XtNleft, XtChainLeft);
	SetArg(XtNright, XtChainLeft);
	SetArg(XtNbottom, XtChainTop);
	SetArg(XtNcallback, callbacks);
	SetArg(XtNfromHoriz, widgets[nwidgets]);
	widgets[++nwidgets] = XtCreateWidget("erase", commandWidgetClass,
			form, args, nargs);

	SetCallback(DestroyNote, pn->pn_index);

	nargs = 0;
	SetArg(XtNtop, XtChainTop);
	SetArg(XtNleft, XtChainLeft);
	SetArg(XtNright, XtChainLeft);
	SetArg(XtNbottom, XtChainTop);
	SetArg(XtNcallback, callbacks);
	SetArg(XtNfromHoriz, widgets[nwidgets]);
	widgets[++nwidgets] = XtCreateWidget("destroy", commandWidgetClass,
			form, args, nargs);

	/*
	 * Set text window options.
	 */
	SetTextOptions(&options);

	/*
	 * Create the text window.
	 */
	nargs = 0;
	SetArg(XtNtop, XtChainTop);
	SetArg(XtNleft, XtChainLeft);
	SetArg(XtNright, XtChainRight);
	SetArg(XtNstring, pn->pn_text);
	SetArg(XtNtextOptions, options);
	SetArg(XtNeditType, XttextEdit);
	SetArg(XtNbottom, XtChainBottom);
	SetArg(XtNlength, pn->pn_textsize);
	SetArg(XtNwidth, pn->pn_textwidth);
	SetArg(XtNheight, pn->pn_textheight);
	SetArg(XtNfromVert, widgets[nwidgets]);
	widgets[++nwidgets] = XtCreateWidget("text", asciiStringWidgetClass,
			form, args, nargs);

	/*
	 * Save the text widget.
	 */
	pn->pn_textwidget = widgets[nwidgets];

	/*
	 * Let the top level shell know all these guys are here.
	 */
	XtManageChildren(widgets, XtNumber(widgets));
	XtManageChild(form);

	/*
	 * Realize the note.
	 */
	XtRealizeWidget(pn->pn_shellwidget);
}

/*
 * SaveNote - save a note to a file.
 */
static void
SaveNote(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	FILE *fp;
	char *MakeFname();
	XSizeHints sizehints;
	register PostItNote *pn;
	XWindowAttributes win_attributes;
	int len, shellx, shelly, texth, textw;
	int x, y;
	unsigned int width, height, borderWidth, depth;
	Window	root, child;

	/*
	 * Find the note we're saving.
	 */
	if ((pn = FindNote((int) client_data)) == NULL)
		return;

	/*
	 * If it doesn't have a file name, make one.
	 */
	if (pn->pn_file == NULL)
		pn->pn_file = MakeFname(pn->pn_index);

	/*
	 * Create the file.
	 */
	if ((fp = fopen(pn->pn_file, "w")) == NULL) {
		fprintf(stderr, "xpostit: ");
		perror(pn->pn_file);
		return;
	}

	/*
	 * Get the position of the shell window.
	 */

	XGetGeometry (display, XtWindow (pn->pn_shellwidget), &root, 
		&x, &y, &width, &height, &borderWidth, &depth);
	
	XTranslateCoordinates (display, XtWindow (pn->pn_shellwidget),
		root, 
		x - borderWidth, y - borderWidth, &shellx, &shelly, &child);
		
	/*
	 * Get the size of the text window.  Can't use XGetNormalHints
	 * here... it seems to get the wrong size for the window.
	 * Maybe because of the scroll bar or something.
	 */
	XGetWindowAttributes(display, XtWindow(pn->pn_textwidget),
		&win_attributes);
	texth = win_attributes.height;
	textw = win_attributes.width;

	/*
	 * Get the length of the text in the window.
	 */
	len = strlen(pn->pn_text);

	/*
	 * Print out the information needed to recreate the note.
	 */
	fprintf(fp, "%s %d %d %d %d %d\n", PostItNoteMagic, shellx, shelly,
			texth, textw, len);

	/*
	 * Write out the text of the note.
	 */
	if (len)
		fwrite(pn->pn_text, sizeof(char), len, fp);

	fclose(fp);
}

/*
 * EraseNote - erase all the text in a note.
 */
static void
EraseNote(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	PostItNote *pn;
	XtCallbackRec cancel[2], confirm[2];

	bzero(confirm, sizeof(confirm));
	bzero(cancel, sizeof(cancel));

	/*
	 * Find the note we're erasing.
	 */
	if ((pn = FindNote((int) client_data)) == NULL)
		return;

	/*
	 * If there's nothing in the window, then there's
	 * no need to erase it.
	 */
	if (strlen(pn->pn_text) == 0)
		return;

	confirm[0].callback = ConfirmErase;
	confirm[0].closure = (caddr_t) pn->pn_index;
	cancel[0].callback = CancelErase;
	cancel[0].closure = (caddr_t) pn->pn_index;

	/*
	 * Get confirmation of what they want to do.
	 */
	ConfirmIt(confirm, cancel);
}

/*
 * DestroyNote - destroy a note.
 */
static void
DestroyNote(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	PostItNote *pn;
	XtCallbackRec cancel[2], confirm[2];

	bzero(confirm, sizeof(confirm));
	bzero(cancel, sizeof(cancel));

	/*
	 * Find the note we're destroying.
	 */
	if ((pn = FindNote((int) client_data)) == NULL)
		return;

	confirm[0].callback = ConfirmDestroy;
	confirm[0].closure = (caddr_t) pn->pn_index;
	cancel[0].callback = CancelDestroy;
	cancel[0].closure = (caddr_t) pn->pn_index;

	/*
	 * Get confirmation of what they want to do.
	 */
	ConfirmIt(confirm, cancel);
}

/*
 * ConfirmErase - callback for erase confirmation.
 */
static void
ConfirmErase(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	XtTextPosition pos = 0;
	register PostItNote *pn;

	/*
	 * Get rid of the confirmation box.
	 */
	ClearConfirm();

	/*
	 * Find the note we're erasing.
	 */
	if ((pn = FindNote((int) client_data)) == NULL)
		return;

	/*
	 * This is a kludge.  They should have provided
	 * an XtTextErase function.
	 */
	XtTextDisableRedisplay(pn->pn_textwidget);

	bzero(pn->pn_text, pn->pn_textsize);

	XtTextSetInsertionPoint(pn->pn_textwidget, pos);
	XtTextSetLastPos(pn->pn_textwidget, pos);
	XtTextEnableRedisplay(pn->pn_textwidget);
}

/*
 * CancelErase - callback for erase cancellation.
 */
static void
CancelErase(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	/*
	 * Get rid of the confirmation box.
	 */
	ClearConfirm();
}

/*
 * ConfirmDestroy - callback for destroy confirmation.
 */
static void
ConfirmDestroy(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	register PostItNote *pn, *prev;

	/*
	 * Get rid of the confirmation box.
	 */
	ClearConfirm();

	/*
	 * Find the note we're destroying.
	 */
	if ((pn = FindNote((int) client_data)) == NULL)
		return;

	/*
	 * Get rid of the widgets for this note.
	 */
	XtPopdown(pn->pn_shellwidget);
	XtDestroyWidget(pn->pn_shellwidget);

	/*
	 * Get rid of the note structure.
	 */
	if (pn != notes) {
		for (prev = notes; prev->pn_next; prev = prev->pn_next) {
			if (prev->pn_next == pn)
				break;
		}

		prev->pn_next = pn->pn_next;
	}
	else {
		notes = pn->pn_next;
	}

	/*
	 * Get rid of the file.
	 */
	if (pn->pn_file) {
		unlink(pn->pn_file);
		free(pn->pn_file);
	}

	/*
	 * Free the memory we used.
	 */
	free(pn->pn_text);
	free(pn);
}

/*
 * CancelDestroy - callback for destroy cancellation.
 */
static void
CancelDestroy(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	/*
	 * Get rid of the confirmation box.
	 */
	ClearConfirm();
}

/*
 * AllocNote - allocate a new note structure and insert in into the
 *	       list of notes.
 */
static PostItNote *
AllocNote(index)
int index;
{
	register PostItNote *pn;

	/*
	 * Allocate a structure.
	 */
	if (notes == NULL) {
		notes = (PostItNote *) SafeAlloc(sizeof(PostItNote));
		pn = notes;
	}
	else {
		for (pn = notes; pn->pn_next != NULL; pn = pn->pn_next)
			;

		pn->pn_next = (PostItNote *) SafeAlloc(sizeof(PostItNote));
		pn = pn->pn_next;
	}

	/*
	 * Initialize the note.
	 */
	pn->pn_positionit = False;
	pn->pn_textsize = app_res.buf_size;
	pn->pn_text = SafeAlloc(pn->pn_textsize);

	/*
	 * If the index number was given, use it.  Otherwise,
	 * get a new index number.
	 */
	pn->pn_index = (index == NewIndex ? NoteIndex() : index);

	return(pn);
}

/*
 * FindNote - find the note structure with the given index number.
 */
static PostItNote *
FindNote(index)
register int index;
{
	register PostItNote *pn;

	for (pn = notes; pn != NULL; pn = pn->pn_next) {
		if (pn->pn_index == index)
			return(pn);
	}

	return(NULL);
}

/*
 * SetTextOptions - set text window options from application resources.
 */
static void
SetTextOptions(options)
register int *options;
{
	*options = editable | wordBreak;

	if (app_res.scroll_ovf)
		*options |= scrollOnOverflow;

	if (app_res.scroll_bar)
		*options |= scrollVertical;
}

/*
 * NoteIndex - find the lowest free index number.
 */
static int
NoteIndex()
{
	register int index;
	register PostItNote *pn;

	/*
	 * This is O(n**2), but the list should be small.
	 */
	for (index = 1; ; index++) {
		if ((pn = FindNote(index)) == NULL)
			return(index);
	}
}
