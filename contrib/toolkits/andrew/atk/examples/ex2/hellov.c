/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/examples/ex2/RCS/hellov.c,v 2.5 89/09/13 11:46:09 tpn Exp $ */

#include <class.h>

#include "graphic.ih"
#include "hellov.eh"

#define POSUNDEF -1

boolean helloworldview__InitializeObject(classID, hwv)
    struct classheader *classID;
    struct helloworldview *hwv;
{
    hwv->x = POSUNDEF;
    hwv->y = POSUNDEF;
    hwv->HaveDownTransition = FALSE;
    return TRUE;
}


void helloworldview__FullUpdate(hwv, type, left, top, width, height)
    struct helloworldview *hwv;
    enum view_UpdateType type;
    long left;
    long top;
    long width;
    long height;
{
    struct rectangle myVisualRect; 

    if (hwv->x == POSUNDEF)  {
	helloworldview_GetVisualBounds(hwv,&myVisualRect);
	hwv->x = rectangle_Left(&myVisualRect) + rectangle_Width(&myVisualRect)/2;
	hwv->y = rectangle_Top(&myVisualRect) + rectangle_Height(&myVisualRect)/2;
    }
    
    helloworldview_MoveTo(hwv,hwv->x,hwv->y);
    helloworldview_DrawString(hwv,"hello world",
	graphic_BETWEENTOPANDBASELINE | graphic_BETWEENLEFTANDRIGHT);    
}

    
void helloworldview__Update(hwv)
    struct helloworldview *hwv;
{
    if (hwv->newX != hwv->x ||  hwv->newY != hwv->y) {
        helloworldview_SetTransferMode(hwv,graphic_INVERT);

        helloworldview_MoveTo(hwv, hwv->x , hwv->y);
        helloworldview_DrawString(hwv, "hello world", graphic_BETWEENTOPANDBASELINE | graphic_BETWEENLEFTANDRIGHT);

        hwv->x = hwv->newX;
        hwv->y = hwv->newY;

        helloworldview_MoveTo(hwv, hwv->x , hwv->y);
        helloworldview_DrawString(hwv, "hello world", graphic_BETWEENTOPANDBASELINE | graphic_BETWEENLEFTANDRIGHT);
   }
}


struct view *helloworldview__Hit(hwv, action, x, y, numberOfClicks)
    struct helloworldview *hwv;
    enum view_MouseAction action;
    long x;
    long y;
    long numberOfClicks;
{
    if (action == view_LeftDown)  {  
        hwv->HaveDownTransition = TRUE;
    }
    else if (hwv ->HaveDownTransition) {
	if (action == view_LeftUp)  {
	    hwv->newX = x;
	    hwv->newY = y;
	    hwv->HaveDownTransition = FALSE;
	    helloworldview_WantUpdate(hwv, hwv);
	}
    }
    return (struct view *) hwv;
}
    
