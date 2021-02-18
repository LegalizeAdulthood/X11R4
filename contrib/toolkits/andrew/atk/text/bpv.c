/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/bpv.c,v 1.1 89/05/22 16:11:01 tpn Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/bpv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/bpv.c,v 1.1 89/05/22 16:11:01 tpn Exp $";
#endif /* lint */

#include <class.h>
#include <bpv.eh>
enum view_DSattributes bpv__DesiredSize(self, width, height, pass, desiredwidth, desiredheight)
struct bpv *self;
long width, height;
enum view_DSpass pass;
long *desiredwidth, *desiredheight;
{
    *desiredwidth = width;
    *desiredheight = 6;
    return(view_HeightFlexible | view_WidthFlexible);
}
void bpv__FullUpdate(self,type,left,top,width,height)
struct bpv *self;
enum view_UpdateType type;
long left,top,width,height;
{
    struct rectangle enclosingRect;
    enclosingRect.top = 0; enclosingRect.left = 0;
    enclosingRect.width  = bpv_GetLogicalWidth(self) -1 ;
    enclosingRect.height = bpv_GetLogicalHeight(self) -1 ;
    bpv_SetTransferMode(self,graphic_WHITE);
    bpv_EraseRect(self,&(enclosingRect));
    bpv_SetTransferMode( self, graphic_COPY );
    bpv_FillRect(self,&(enclosingRect) ,bpv_GrayPattern(self,4,16));
}
void bpv__Print(self, f, process, final, toplevel)
    struct textview *self;
    FILE *f;
    char *process;
    char *final;
    int toplevel;
{
	fputs(".OC\n.bp\n",f);
    }
boolean bpv__InitializeObject(classID,self)
struct classheader *classID;
struct bpv *self;
{
    return TRUE;
}
