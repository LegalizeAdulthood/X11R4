/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/examples/ex16/RCS/hello.c,v 2.5 89/09/13 11:44:01 tpn Exp $ */

#include <stdio.h>
#include <class.h>

#include "hello.eh"

boolean helloworld__InitializeObject(classID,hw)
struct classheader *classID;
struct helloworld *hw;   
{
    hw->x=POSUNDEF;
    hw->y=POSUNDEF;
    hw->blackOnWhite=TRUE;
    return TRUE;
}

long helloworld__Read(hw,file,id)
struct helloworld *hw;
FILE *file;
long id;
{
    char buf[100];

    helloworld_SetID(hw,helloworld_UniqueID(hw));

    if(fgets(buf,sizeof(buf),file)==NULL)
	return dataobject_PREMATUREEOF;
    /* the %hd tells scanf that blackOnWhite is a short, not an int */
    if(sscanf(buf,"%d %d %hd\n",&hw->x,&hw->y,&hw->blackOnWhite)<3)
	return dataobject_PREMATUREEOF;

    if(fgets(buf,sizeof(buf),file)==NULL) /* read in the \enddata{...} */
	return dataobject_MISSINGENDDATAMARKER;

    return dataobject_NOREADERROR;
}

long helloworld__Write(hw,file,writeId,level)
struct helloworld *hw;
FILE *file;
long writeId;
int level;
{
    if(writeId!=helloworld_GetWriteID(hw)){ /* only write a given version once */
	helloworld_SetWriteID(hw,writeId);
	fprintf(file,"\\begindata{%s,%d}\n",
		class_GetTypeName(hw), helloworld_UniqueID(hw));
	fprintf(file,"%d %d %d\n",hw->x,hw->y,hw->blackOnWhite);
	fprintf(file,"\\enddata{%s,%d}\n",
		class_GetTypeName(hw), helloworld_UniqueID(hw));
    }

    return helloworld_UniqueID(hw);
}
