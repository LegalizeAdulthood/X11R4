/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/examples/ex1/RCS/helloa.c,v 2.5 89/09/13 11:40:39 tpn Exp $ */

#include <class.h>

#include "helloa.eh"

#include "im.ih"
#include "hellov.ih"

boolean helloworldapp__Start(hwapp)
struct helloworldapp *hwapp;
{
    struct helloworldview *hwv;
    struct im *im;

    if(!super_Start(hwapp))
	return FALSE;

    hwv=helloworldview_New();
    if(hwv==NULL)
	return FALSE;

    im=im_Create(NULL);
    if(im==NULL){
	helloworldview_Destroy(hwv);
	return FALSE;
    }

    im_SetView(im,hwv);

    return TRUE;
}
