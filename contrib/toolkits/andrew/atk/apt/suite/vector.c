/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#ifndef lint
static char *rcsidvector_c = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/apt/suite/RCS/vector.c,v 1.3 89/07/28 18:49:20 gk5g Exp $";
#endif

/*
    $Log:	vector.c,v $
 * Revision 1.3  89/07/28  18:49:20  gk5g
 * Reset removed data items to NULL in vector_DestroyItem()
 * 
 * Revision 1.2  89/07/19  12:43:59  gk5g
 * In vector_RemoveItem() now index the vector explicitly instead of relying on a terminating NULL data field. It is perfectly OK to have NULL data anywhere in the vector. NOTE: There may be more instances of this NULL-terminator reliance.
 * 
 * Revision 1.1  89/04/28  20:26:48  tom
 * Initial revision
 * 
*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Vector-object

MODULE	vector.c

VERSION	0.0

AUTHOR	GW Keim
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Vector-object.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  04/10/89	Created (GW Keim)
  07/19/89	In vector_RemoveItem() now index the vector explicitly instead
                of relying on a terminating NULL data field. It is perfectly OK
                to have NULL data anywhere in the vector; (GW Keim)
		NOTE: There may be more instances of this NULL-terminator reliance.
   07/28/89	Reset removed data items to NULL in vector_DestroyItem(); (GW Keim);

END-SPECIFICATION  ************************************************************/

#include <stdio.h>
#include <class.h>
#include "vector.eh"

#define Data			(self->data)    
#define InitialDataSize		(self->initial_vector_count)
#define DataSize		(self->current_vector_count)
#define DataUsed		(self->current_used_count)
#define ReallocFactor		(self->reallocation_factor)
#define Sorter			(self->sorter)
#define Destroyer		(self->destroyer)
#define ApplyProc		(self->apply)
#define Debug			(self->debug)
#define DataSpaceAvailable	(DataSize > DataUsed)

boolean
vector__InitializeObject( ClassID, self )
    register struct classheader	*ClassID;
    register struct vector	*self;
{
    Data = NULL;
    Debug = DataSize = DataUsed = InitialDataSize = ReallocFactor = 0;
    ApplyProc = Sorter = Destroyer = NULL;
    return(TRUE);
}

struct vector *
vector__Create( ClassID, init_data_size, reallocfactor )
    register struct classheader	    *ClassID;
    register long		     init_data_size, 
				     reallocfactor;
{
    register struct vector	    *self = NULL;

    if( !(self = vector_New()) ) {
	printf("vector: couldn't allocate new object.\n");
	exit(-1);
    }
    DataSize = InitialDataSize = init_data_size;
    ReallocFactor = reallocfactor;
    if( !(Data = (long*) calloc( InitialDataSize + 1, sizeof(long)) )) {
	printf("vector:couldn't allocate enough memory.\n");
	exit(-1);
    }
    return(self);
}

void
vector__FinalizeObject( ClassID, self )
    register struct classheader	*ClassID;
    register struct vector	*self;
{
    register long		 i = 0;

    if(Data)
	if(Destroyer)
	    while((i < DataUsed) && Data[i]) 
		Destroyer(Data[i++]);
    if(Data) {
	free(Data);
	Data = NULL;
    }
}


static void
ReallocData( self )
    register struct vector   *self;
{
    register long	      i = 0;

    DataSize *= ReallocFactor;
    if( !(Data = (long*) realloc( Data, sizeof(long) * (DataSize + 1))) ) {
	printf("vector: couldn't allocate enough memory.\n");
	exit(-1);
    }
    i = DataUsed;
    while(i < (DataSize+1)) Data[i++] = 0;
}

long
vector__AddItem( self, item )
    register struct vector	*self;
    long			 item;
{
    register long		 i = 0, 
				 insertOffset = 0, 
				 end = 0;

    if(!DataSpaceAvailable) 
	ReallocData(self);
    if(Sorter) {
	while(Data[i] && (Sorter(&Data[i],&item) < 0)) i++;
	insertOffset = i;
	end = DataUsed - 1;
	while(end >= insertOffset) {
	    Data[end + 1] = Data[end];
	    end--;
	}
    }
    else insertOffset = DataUsed;
    Data[insertOffset] = item;
    DataUsed++;
    return(insertOffset);
}

boolean
vector__ItemExists( self, item )
    register struct vector	*self;
    register long		 item;
{
    if(vector_Subscript(self,item) != -1) 
	return(TRUE);
    else 
	return(FALSE);
}

long
vector__RemoveItem( self, item )
    register struct vector    *self;
    register long	       item;
{
    register long	       i = 0, removeOffset = 0;

    while(Data[i]) {
	if(Data[i] == item) {
	    removeOffset = i;
	    while((i+1) < DataUsed) {
		Data[i] = Data[i+1];
		i++;
	    }
	    DataUsed--;
	    Data[DataUsed] = NULL;
	    return(removeOffset);
	}
	else i++;
    }
    return(-1);
}

long
vector__Sort( self )
    register struct vector   *self;
{
    register long	      status = 0;

    if(!Sorter) 
	status = vector_status_no_sort_routine;
    else 
	qsort(Data,DataSize,sizeof(long),Sorter);
    return(status);
}

long
vector__Subscript( self, item )
    register struct vector	*self;
    register long		 item;
{
    register long		 i = 0;

    if(Data)
	while((i < DataUsed) && Data[i]) 
	    if(Data[i] == item) 
		return(i);
	    else 
		i++;
    return(-1);
}

void
vector__Apply( self, proc )
    register struct vector  *self;
    long		   (*proc)();
{
    register int	 i = 0, status = 0;

    if(Data && Data[0])
	for( i = 0 ; (i < DataUsed) && !status ; i++ )
	    status = proc(Data[i]);
}
