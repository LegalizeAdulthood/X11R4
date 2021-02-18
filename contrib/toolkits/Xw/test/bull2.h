

/****************************************
*   Automatic test program generation   *
*       Thu Jul  7 12:46:43 1988        *
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

#define PROGNAME "bull2"

Boolean wait;
XWindowAttributes info;
Boolean trace = False;
int delay = 0;
Display *display;
Arg args[20];
int n;

Widget PopupShell = NULL;
Widget Shell1 = NULL;

Widget BulletinBoard1 = NULL;
Widget Toggle1 = NULL;
Widget Toggle2 = NULL;
Widget Toggle3 = NULL;
Widget Toggle4 = NULL;
Widget Toggle5 = NULL;
Widget Toggle6 = NULL;
Widget Toggle7 = NULL;
Widget Toggle8 = NULL;
Widget Toggle9 = NULL;
