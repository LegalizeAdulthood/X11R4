/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/examples/ex10/RCS/helloa.c,v 2.5 89/09/13 11:41:04 tpn Exp $ */

#include <class.h>

#include "helloa.eh"

#include "im.ih"
#include "frame.ih"
#include "hello.ih"
#include "hellov.ih"

boolean helloworldapp__Start(hwapp)
struct helloworldapp *hwapp;
{
    struct helloworld *hw;
    struct helloworldview *hwv;
    struct view *applayer;
    struct frame *frame;
    struct im *im;

    if(!super_Start(hwapp))
	return FALSE;

    hw=helloworld_New();
    if(hw==NULL)
	return FALSE;

    hwv=helloworldview_New();
    if(hwv==NULL) {
	helloworld_Destroy(hw);
	return FALSE;
    }

    applayer=helloworldview_GetApplicationLayer(hwv);
    if(applayer==NULL) {
	helloworldview_Destroy(hwv);
	return FALSE;
    }

    frame=frame_New();
    if(frame==NULL) {
	helloworldview_DeleteApplicationLayer(hwv,applayer);
	return FALSE;
    }

    im=im_Create(NULL);
    if(im==NULL) {
	frame_Destroy(frame);
	return FALSE;
    }

    helloworldview_SetDataObject(hwv,hw);
    frame_SetView(frame,applayer);
    im_SetView(im,frame);

    helloworldview_WantInputFocus(hwv,hwv);

    return TRUE;

}

