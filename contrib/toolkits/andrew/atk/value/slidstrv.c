/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/slidstrv.c,v 2.4 89/02/17 17:50:33 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/slidstrv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/value/RCS/slidstrv.c,v 2.4 89/02/17 17:50:33 ghoti Exp $ ";
#endif /* lint */

#include <class.h>
#include <value.ih>
#include <slidstrv.eh>

char *sliderstrV__GetValueString(self)
struct sliderstrV *self;
{
    struct value *w = sliderstrV_Value(self);
    long len,val;
    char **arr;
    if(((len = value_GetArraySize(w)) == 0) || 
	((arr = value_GetStringArray(w)) == NULL) || 
	  ((val = sliderstrV_GetTmpVal(self))< 0) ||
	  val >= len)
	return super_GetValueString(self);
    return(arr[val]);
}
