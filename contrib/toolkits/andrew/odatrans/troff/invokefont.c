/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/invokefont.c,v 1.5 89/09/11 14:10:36 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/invokefont.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	invokefont.c,v $
 * Revision 1.5  89/09/11  14:10:36  mss
 * got rid of line with only #
 * 
 * Revision 1.4  89/09/08  17:49:13  mss
 * /tmp/msg
 * 
 * Revision 1.3  89/09/07  20:47:28  mss
 * fixed capitalization of invoke cfont
 * 
 * Revision 1.2  89/08/30  16:49:53  mss
 * Changed capitalization of *font.[ch]
 * 
 * Revision 1.1  89/08/30  16:35:55  mss
 * Initial revision
 * 
 * Revision 1.2  88/11/17  15:04:50  annm
 * Removed the ' ' from the end of the one character long font names 
 * (i.e. R, I, B, H and C)
 * 
 * Revision 1.1  88/11/17  14:43:59  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/invokefont.c,v $:";
#endif

#include <odatk.h>

#include <stdio.h>
#include <sys/time.h>

#include <initfonts.h>

#define INVOKEFONT

#include <invokefont.h>

#undef INVOKEFONT

int InvokeFont( id, FontStruct )
INT_type id;
struct FontStruct *FontStruct;
{

    int SlaveRval;

    if( id > (INT_type) NFONTS || id < (INT_type) 0 ){
	fprintf( stderr, "[InvokeFont]: font id of %d is out of range\n",
	    id );
	return( FAILURE );
    }

    SlaveRval = DoesFontExist( id );
    if( SlaveRval == NO ){
	return( FAILURE );
    }

    SlaveRval = CopyPsAndFontKind( id, FontStruct );
    if( SlaveRval == FAILURE ){
	return( FAILURE );
    }

    SlaveRval = MountFont( id, FontStruct );
    if( SlaveRval == FAILURE ){
	return( FAILURE );
    }

    return( SUCCESS );

}

int DoesFontExist( id )
INT_type id;
{

    if( FontInfo[id].TroffName[0] == (char) 0 &&
	FontInfo[id].TroffName[1] == (char) 0 &&   
	FontInfo[id].TroffName[2] == (char) 0 &&
	FontInfo[id].PointSize == 0 ){
	fprintf( stderr, "[DoesFontExist]: font %d does not exist",
	    id );
	return( NO );
    }

    return( YES );

}

int CopyPsAndFontKind( id, FontStruct )
INT_type id;
struct FontStruct *FontStruct;
{

    FontStruct->PointSize = FontInfo[id].PointSize;

    strcpy( FontStruct->TroffName, FontInfo[id].TroffName );

    return( SUCCESS );

}

int MountFont( id, FontStruct )
INT_type id;
struct FontStruct *FontStruct;
{

    int SlaveRval;

    int slot;

    SlaveRval = IsFontMounted( id, &slot );

    if( SlaveRval == YES ){
	FontStruct->DoMount = NO;
	FontStruct->MountPosition = UNSPECIFIED_MOUNT_POSITION;
	SlaveRval = SetLRU( slot );
	if( SlaveRval == FAILURE ){
	    fprintf( stderr, "[MountFont]: Can not update LRU info for an already mounted font\n" );
	    return( FAILURE );
	}
	return( SUCCESS );
    }

    /* find a slot */
    SlaveRval = FindLRU( &slot );
    if( SlaveRval == FAILURE ){
	return( FAILURE );
    }

    /* otherwise grab slot */
    FontState[slot].id = id;
    strcpy( FontState[slot].TroffName, FontInfo[id].TroffName );
    
    SlaveRval = SetLRU( slot );
    if( SlaveRval == FAILURE ){
	return( FAILURE );
    }

    FontStruct->DoMount = YES;
    FontStruct->MountPosition = slot + 1;

    return( SUCCESS );
    
}

int IsFontMounted( id, slot )
INT_type id;
int *slot;
{

    int i;

    for( i = 0; i < 8; i ++ ){

	if( strcmp( FontState[i].TroffName, FontInfo[id].TroffName ) == 0 ){
	    *slot = i;
	    return( YES );
	}

    }

    return( NO );

}


int FindLRU( slot )
int *slot;
{

    /* 
	rummage through the font state table 
	looking for the least recently used 
    */

    int i;

    int oldest;

    long OldestSec;
    long OldestUsec;

    struct timeval tp;
    struct timezone trp;

    gettimeofday( &tp, &trp);

    OldestSec = tp.tv_sec;
    OldestUsec = tp.tv_usec;

    oldest = -1;

    for( i = 0; i < 8; i++ ){
	
	if( FontState[i].seconds < OldestSec ||
	    ( FontState[i].seconds == OldestSec &&
	      FontState[i].useconds < OldestUsec ) ){

	    OldestSec = FontState[i].seconds;
	    OldestUsec = FontState[i].useconds;
	    oldest = i;

	}

    }

    /* if didn't find an oldest - UGH */
    if( oldest == -1 ){
	return( FAILURE );
    }

    *slot = oldest;

    return( SUCCESS );

}

int SetLRU( slot )
int slot;
{

    struct timeval tp;
    struct timezone trp;

    gettimeofday( &tp, &trp );

    FontState[slot].seconds = tp.tv_sec;
    FontState[slot].useconds = tp.tv_usec;

    return( SUCCESS );

}

