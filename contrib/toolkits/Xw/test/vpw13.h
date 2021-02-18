

/****************************************
*   Automatic test program generation   *
*       Thu Jul 14 11:04:00 1988        *
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

#define PROGNAME "vpw13"

Boolean wait;
XWindowAttributes info;
Boolean trace = False;
int delay = 0;
Display *display;
Arg args[20];
int n;

Widget PopupShell = NULL;
Widget Shell1 = NULL;

Widget VPanedWindow1 = NULL;
Widget RowCol1 = NULL;
Widget RowCol2 = NULL;
Widget RowCol3 = NULL;
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
Widget PushButton14 = NULL;
Widget PushButton15 = NULL;
Widget PushButton16 = NULL;
Widget PushButton17 = NULL;
Widget PushButton18 = NULL;
Widget PushButton19 = NULL;
