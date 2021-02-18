

/****************************************
*   Automatic test program generation   *
*       Thu Jul 14 11:35:36 1988        *
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

#define PROGNAME "gen2"

Boolean wait;
XWindowAttributes info;
Boolean trace = False;
int delay = 0;
Display *display;
Arg args[20];
int n;

Widget PopupShell = NULL;
Widget Shell1 = NULL;

Widget RowCol1 = NULL;
Widget Arrow1 = NULL;
Widget PushButton1 = NULL;
Widget ScrollBar1 = NULL;
Widget StaticText1 = NULL;
Widget TextEdit1 = NULL;
Widget Toggle1 = NULL;
Widget Valuator1 = NULL;
