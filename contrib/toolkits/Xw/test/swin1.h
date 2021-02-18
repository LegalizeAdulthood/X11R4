

/****************************************
*   Automatic test program generation   *
*       Mon Jul 11 14:21:55 1988        *
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

#define PROGNAME "swin1"

Boolean wait;
XWindowAttributes info;
Boolean trace = False;
int delay = 0;
Display *display;
Arg args[20];
int n;

Widget PopupShell = NULL;
Widget Shell1 = NULL;

Widget Form1 = NULL;
Widget StaticText1 = NULL;
Widget RowCol1 = NULL;
Widget PushButton1 = NULL;
Widget PushButton2 = NULL;
Widget PushButton3 = NULL;
Widget PushButton4 = NULL;
Widget PushButton5 = NULL;
Widget MenuSep1 = NULL;
Widget MenuSep2 = NULL;
Widget StaticText2 = NULL;
Widget TextEdit1 = NULL;
Widget StaticText3 = NULL;
Widget TextEdit2 = NULL;
Widget VPanedWindow1 = NULL;
Widget ScrolledWindow1 = NULL;
Widget TextEdit3 = NULL;
Widget Form2 = NULL;
Widget StaticText4 = NULL;
Widget TextEdit4 = NULL;
Widget ScrolledWindow2 = NULL;
Widget TextEdit5 = NULL;
