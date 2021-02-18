#include <X11/Intrinsic.h>

static Widget *wstack = NULL;
static int maxwidgets = 0;
#define INC_WIDGETS 32
static int curw = 0;

static Widget subtree;

/*
 * These are the global procedures that keep a stack of who has the Focus in a
 * subtree. This saves us having to propagate global information about the
 * present status of the Focus to anyone who might need/want it. It's
 * restricted to one subtree - could be generalized to multiple subtrees, I
 * guess
 */

SetFocusRoot(w)
Widget w;
{
	if (curw != 0)
		XtError("Changing Focus subtree when someone has the focus");
	subtree = w;
}

AcquireFocus(w)
Widget w;
{
	if (curw == maxwidgets) {
		if (curw == 0) {
			maxwidgets += INC_WIDGETS;
			wstack = (Widget *) XtCalloc((unsigned) maxwidgets,
			 sizeof(Widget));
		} else {
			maxwidgets += INC_WIDGETS;
			wstack = (Widget *) XtRealloc((char *) wstack,
			 (unsigned) (maxwidgets * sizeof(Widget)));
		}
	}
	wstack[curw++] = w;
	XtSetKeyboardFocus(subtree, w);
}

ReleaseFocus(w)
Widget w;
{
	curw--;
	if (curw == 0) {
		if (wstack[curw] != w)
			XtError("Mismatched ReleaseFocus");
		XtSetKeyboardFocus(subtree, (Widget) None);
		curw = 0;
	} else if (curw > 0) {
		if (wstack[curw] != w)
			XtError("Mismatched ReleaseFocus");
		XtSetKeyboardFocus(subtree, wstack[curw-1]);
	} else {
		XtWarning("ReleaseFocus with no corresponding Acquire");
		curw = 0;
	}
}

Widget
WhoHasFocus()
{
	return (curw == 0)? (Widget) None : wstack[curw - 1];
}
