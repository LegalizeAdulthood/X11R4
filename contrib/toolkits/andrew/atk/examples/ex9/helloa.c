/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/examples/ex9/RCS/helloa.c,v 2.5 89/09/13 11:48:13 tpn Exp $ */

#include <class.h>

#include "helloa.eh"

#include "im.ih"
#include "frame.ih"
#include "hellov.ih"

boolean helloworldapp__Start(hwapp)
struct helloworldapp *hwapp;
{
    struct helloworldview *hwv;
    struct view *applayer;
    struct frame *frame;
    struct im *im;

    if(!super_Start(hwapp))
	return FALSE;

    hwv=helloworldview_New();
    if(hwv==NULL)
	return FALSE;

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

    frame_SetView(frame,applayer);
    im_SetView(im,frame);

    helloworldview_WantInputFocus(hwv,hwv);

    return TRUE;

}

