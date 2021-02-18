

/****************************************
*   Automatic test program generation   *
*       Tue Jul 26 14:42:49 1988        *
****************************************/

#include <stdio.h>
#include <signal.h>
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/Shell.h>
#include <Xw/Xw.h>
#include <Xw/XwP.h>
#include <Xw/Arrow.h>
#include <Xw/BBoard.h>
#include <Xw/Form.h>
#include <Xw/ImageEdit.h>
#include <Xw/List.h>
#include <Xw/MenuSep.h>
#include <Xw/PopupMgr.h>
#include <Xw/PButton.h>
#include <Xw/RCManager.h>
#include <Xw/Sash.h>
#include <Xw/Valuator.h>
#include <Xw/ScrollBar.h>
#include <Xw/SWindow.h>
#include <Xw/SRaster.h>
#include <Xw/SText.h>
#include <Xw/TextEdit.h>
#include <Xw/TitleBar.h>
#include <Xw/Toggle.h>
#include <Xw/VPW.h>
#include <Xw/WorkSpace.h>
#include <Xw/MenuBtn.h>
#include <Xw/Cascade.h>
#include <Xw/Panel.h>

#define PROGNAME "list9"

Boolean wait;
XWindowAttributes info;
Boolean trace = False;
int delay = 0;
Display *display;
Arg args[20];
int n;

Widget PopupShell = NULL;
Widget Shell1 = NULL;
XEvent DummyEvent;

Widget RowCol1 = NULL;
Widget List1 = NULL;
Widget Arrow1 = NULL;
Widget Arrow2 = NULL;
Widget Arrow3 = NULL;
Widget Arrow4 = NULL;
Widget Arrow5 = NULL;
Widget Arrow6 = NULL;
Widget Arrow7 = NULL;
Widget Arrow8 = NULL;
Widget Arrow9 = NULL;
Widget Arrow10 = NULL;
Widget Arrow11 = NULL;
Widget Arrow12 = NULL;
Widget Arrow13 = NULL;
Widget Arrow14 = NULL;
Widget List2 = NULL;
Widget PushButton1 = NULL;
Widget PushButton2 = NULL;
Widget PushButton3 = NULL;
Widget PushButton4 = NULL;
Widget PushButton5 = NULL;
Widget PushButton6 = NULL;
Widget PushButton7 = NULL;
Widget PushButton8 = NULL;
Widget PushButton9 = NULL;
Widget PushButton10 = NULL;
Widget PushButton11 = NULL;
Widget PushButton12 = NULL;
Widget PushButton13 = NULL;
Widget List3 = NULL;
Widget Toggle1 = NULL;
Widget Toggle2 = NULL;
Widget Toggle3 = NULL;
Widget Toggle4 = NULL;
Widget Toggle5 = NULL;
Widget Toggle6 = NULL;
Widget Toggle7 = NULL;
Widget Toggle8 = NULL;
Widget Toggle9 = NULL;
Widget Toggle10 = NULL;
Widget List4 = NULL;
Widget StaticText1 = NULL;
Widget StaticText2 = NULL;
Widget StaticText3 = NULL;
Widget StaticText4 = NULL;
Widget StaticText5 = NULL;
Widget StaticText6 = NULL;
Widget StaticText7 = NULL;
Widget StaticText8 = NULL;
Widget StaticText9 = NULL;
Widget StaticText10 = NULL;
Widget StaticText11 = NULL;
Widget StaticText12 = NULL;
Widget StaticText13 = NULL;
Widget StaticText14 = NULL;
Widget List5 = NULL;
Widget Toggle11 = NULL;
Widget Toggle12 = NULL;
Widget Toggle13 = NULL;
Widget Toggle14 = NULL;
Widget Toggle15 = NULL;
Widget Toggle16 = NULL;
Widget Toggle17 = NULL;
Widget Toggle18 = NULL;
Widget Toggle19 = NULL;
Widget Toggle20 = NULL;
Widget List6 = NULL;
Widget List7 = NULL;
Widget Arrow15 = NULL;
Widget Arrow16 = NULL;
Widget Arrow17 = NULL;
Widget Arrow18 = NULL;
Widget Arrow19 = NULL;
Widget Arrow20 = NULL;
Widget Arrow21 = NULL;
Widget Arrow22 = NULL;
Widget Arrow23 = NULL;
Widget Arrow24 = NULL;
Widget PushButton14 = NULL;
Widget PushButton15 = NULL;
Widget PushButton16 = NULL;
Widget PushButton17 = NULL;
Widget PushButton18 = NULL;
Widget PushButton19 = NULL;
Widget PushButton20 = NULL;
Widget PushButton21 = NULL;
Widget PushButton22 = NULL;
Widget PushButton23 = NULL;
Widget PushButton24 = NULL;
Widget PushButton25 = NULL;
Widget PushButton26 = NULL;
Widget List8 = NULL;
Widget StaticText15 = NULL;
Widget StaticText16 = NULL;
Widget StaticText17 = NULL;
Widget StaticText18 = NULL;
Widget StaticText19 = NULL;
Widget StaticText20 = NULL;
Widget StaticText21 = NULL;
Widget StaticText22 = NULL;
Widget StaticText23 = NULL;
Widget StaticText24 = NULL;
Widget StaticText25 = NULL;
Widget StaticText26 = NULL;
Widget StaticText27 = NULL;
Widget StaticText28 = NULL;
Widget List9 = NULL;
Widget Arrow25 = NULL;
Widget Arrow26 = NULL;
Widget Arrow27 = NULL;
Widget Arrow28 = NULL;
Widget Arrow29 = NULL;
Widget Arrow30 = NULL;
Widget Arrow31 = NULL;
Widget Arrow32 = NULL;
Widget Arrow33 = NULL;
Widget Arrow34 = NULL;
Widget Arrow35 = NULL;
Widget Arrow36 = NULL;
Widget Arrow37 = NULL;
Widget Arrow38 = NULL;
Widget List10 = NULL;
Widget StaticText29 = NULL;
Widget StaticText30 = NULL;
Widget StaticText31 = NULL;
Widget StaticText32 = NULL;
Widget StaticText33 = NULL;
Widget StaticText34 = NULL;
Widget StaticText35 = NULL;
Widget StaticText36 = NULL;
Widget StaticText37 = NULL;
Widget StaticText38 = NULL;
Widget StaticText39 = NULL;
Widget StaticText40 = NULL;
Widget StaticText41 = NULL;
Widget StaticText42 = NULL;
Widget List11 = NULL;
Widget Arrow39 = NULL;
Widget Arrow40 = NULL;
Widget Arrow41 = NULL;
Widget Arrow42 = NULL;
Widget Arrow43 = NULL;
Widget Arrow44 = NULL;
Widget List12 = NULL;
Widget Toggle21 = NULL;
Widget Toggle22 = NULL;
Widget Toggle23 = NULL;
Widget Toggle24 = NULL;
Widget Toggle25 = NULL;
Widget Toggle26 = NULL;
Widget Toggle27 = NULL;
Widget Toggle28 = NULL;
