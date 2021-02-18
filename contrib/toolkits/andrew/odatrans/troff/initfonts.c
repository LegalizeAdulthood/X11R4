/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/


/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/initfonts.c,v 1.3 89/09/08 17:48:51 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/initfonts.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	initfonts.c,v $
 * Revision 1.3  89/09/08  17:48:51  mss
 * /tmp/msg
 * 
 * Revision 1.2  89/08/30  16:50:12  mss
 * Changed capitalization of *font.[ch]
 * 
 * Revision 1.1  89/08/30  16:35:48  mss
 * Initial revision
 * 
 * Revision 1.5  89/05/16  13:31:34  annm
 * plugged core leak - font monster !
 * 
 * Revision 1.4  89/01/12  17:53:20  jaap
 * Just saving this version. I don't know why it was out in the first place
 * 
 * Revision 1.3  88/11/29  16:54:43  annm
 * added capability to examine the family in InitializeFonts
 * 
 * Revision 1.2  88/11/17  15:03:22  annm
 * Removed the ' ' from the end of the one character long font names 
 * (i.e. R, I, B, H and C)
 * 
 * Revision 1.1  88/11/17  14:42:54  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/initfonts.c,v $:";
#endif

#include <odatk.h>

#include <stdio.h>

#define INITFONTS

#include <initfonts.h>

#undef INITFONTS

int InitializeFonts( document )
DOCUMENT_type document;
{

    /*
	This routine processes the dp_FONTS_LIST to produce
	an internal version containing troff commands for use
	by subsequent InvokeFont calls

	If the routine succeeds it returns SUCCESS
	otherwise FAILURE is returned
    */

    CONSTITUENT DocumentProfile;

    SEQUENCE_FONT_PAIR_type FontMonster;

    INT_type SlaveRval;

    int i;

    DocumentProfile = FindDocumentProfile( document );
    if( DocumentProfile == ERROR_CONSTITUENT ){
	fprintf( stderr, "[InitializeFonts]: There is no document profile\n" );
	return( FAILURE );
    }

    SlaveRval = GetAttr( DocumentProfile, dp_FONTS_LIST, &FontMonster,
		    /* inherit*/ BOOL_false, 
		    /* parm mask */ (PARM_MASK_type) 0 );
    if( SlaveRval == ERROR_INT ){
	fprintf( stderr, "Can not get the dp_FONTS_LIST\n" );
	return( FAILURE );
    }

    for( i = 0; i < FontMonster->length; i++ ){
	SlaveRval = ProcessFont( &FontMonster->sequence_value.font_pairs[i] );
	if( SlaveRval == FAILURE ){
	    return( FAILURE );
	}
    }

    SlaveRval = DeleteSequence( FontMonster );
    if( SlaveRval == ERROR_INT ){
	fprintf( stderr, "Can not free the font monster\n" );
    }

    return( SUCCESS );

}

int ProcessFont( fp )
FONT_PAIR_type *fp;
{

    int SlaveRval;

    /*
	This Routine examines one font monster and sets up the
	information needed to .fp and .ft/.ps the font
    */

    if( fp->font_id > NFONTS || fp->font_id < 0 ){
	fprintf( stderr, "[ProcessFont]: font id out of range\n" );
	return( FAILURE );
    }

    if( FontInfo[fp->font_id].TroffName[0] != (char) 0 ||
	FontInfo[fp->font_id].TroffName[1] != (char) 0 ||
	FontInfo[fp->font_id].TroffName[2] != (char) 0 ||
	FontInfo[fp->font_id].PointSize != 0 ){
	fprintf( stderr, "[ProcessFont]: Seeing repeat of font id %d", fp->font_id );
	return( FAILURE );
    }

    /* first determine the point size */
    SlaveRval = PointSize( fp );
    if( SlaveRval == FAILURE ){
	return( FAILURE );
    }

    SlaveRval = WhichFont( fp );
    if( SlaveRval == FAILURE ){
	return( FAILURE );
    }

    return( SUCCESS );
    
}

int PointSize( fp )
FONT_PAIR_type *fp;
{

    double DoublePointSize;

    /* make sure that the design size was specified */
    if( fp->font_reference.dsnsize.second == (INT_type) 0 ){
	FontInfo[fp->font_id].PointSize = UNSPECIFIED_PS;
	return( SUCCESS );
    }

    DoublePointSize = (double) fp->font_reference.dsnsize.first / 
			(double) fp->font_reference.dsnsize.second;
    DoublePointSize = ( DoublePointSize / (double) 25.4 );
		/* this give inches because the dsnsize is in mm's
		    and because there are 25.4 mm per inch */
    DoublePointSize = DoublePointSize * (double) 72.0;
		/* this gives points because there are 72 points
		    per inch */
    DoublePointSize += 0.5;	/* prepare to round to integral point size */

    FontInfo[fp->font_id].PointSize = (int) DoublePointSize;

    return( SUCCESS );

}

int WhichFont( fp )
FONT_PAIR_type *fp;
{

    int SlaveRval;
    
    /* start out undefining the font */
    FontInfo[fp->font_id].TroffName[0] = '\0';

    /* attempt dsclass */
    SlaveRval = TryDsclass( fp );
    if( SlaveRval == SUCCESS ){
	return( SUCCESS );
    }
    else if( SlaveRval == FAILURE ){
	return( FAILURE );
    }
    
    /* return from TryDsclass was NOT_THERE */

    /* now try family */
    SlaveRval = TryFamily( fp );
    if( SlaveRval == FAILURE ){
	return( FAILURE );
    }
    else if( SlaveRval == SUCCESS ){
	return( SUCCESS );
    }
    else {
	/* NOT THERE */
	return( SUCCESS );	/* font is unspecified */
    }

}

int TryDsclass( fp )
FONT_PAIR_type *fp;
{

    int SlaveRval;

    VALUE_type WhichISOFont;

    if( fp->font_reference.dsclass == NULL_SEQUENCE ){
	return( NOT_THERE );
    }

    WhichISOFont = Ints2Dsclass( fp->font_reference.dsclass );
    if( WhichISOFont == DSCLASS_ERROR ){
	fprintf(stderr, "[WhichFont]: Ints2Dsclass returned error\n" );
	return( FAILURE );
    }

    if( WhichISOFont == DSCLASS_NOMATCH ){
	fprintf( stderr, "[WhichFont]: Can not match a dsclass\n" );
	return( FAILURE );
    }

    if( WhichISOFont != DSCLASS_SANS_gothic_neo &&
	WhichISOFont != DSCLASS_SERIFS_legi_rounded &&
	WhichISOFont != DSCLASS_SERIFS_square_type ){
	fprintf( stderr, "[WhichFont]: Dsclass we don't handle encountered\n" );
	return( FAILURE );
    }

    SlaveRval = SetUpFont( fp, WhichISOFont );
    if( SlaveRval == FAILURE ){
	return( FAILURE );
    }
    else {
	return( SUCCESS );
    }

}

int TryFamily( fp )
FONT_PAIR_type *fp;
{

    char name[100];

    int SlaveRval;

    int i;

    int length;

    /* if family is null then succeed */
    if( fp->font_reference.family.structured_name_tag == 
	    STRUCTURED_NAME_UNSPEC_tag ){
	return( SUCCESS );
    }

    /* make sure that the octet string is really there */
    if( fp->font_reference.family.structured_name_tag ==
	STRUCTURED_NAME_ID_tag ){
	fprintf( stderr, "[TryFamily]: family with id structured name\n" );
	return( FAILURE );
    }

    /* it must be a STRUCTURE_SEQ_NAME_tag */
    /* check length of sequence */
    if( fp->font_reference.family.structured_name_value.idseq->length !=
	(INT_type) 2 ){
	fprintf( stderr, "[TryFamily]: structured name with bad length\n" );
	return( FAILURE );
    }
    
    /* check to make sure its a byte sequence */
    if( fp->font_reference.family.structured_name_value.idseq->sequence_value.iso9541_ids[0].iso9541_id_tag == ID_INT_tag ||
	fp->font_reference.family.structured_name_value.idseq->sequence_value.iso9541_ids[1].iso9541_id_tag == ID_INT_tag ){
    
	fprintf(stderr, "[TryFamily]: uses and ID INT\n" );
	return( FAILURE );
    }
    
    /* build up organization/kind form of name */

    length = 	fp->font_reference.family.structured_name_value.idseq->sequence_value.iso9541_ids[1].iso9541_id_value.octet_string->length;

    strncpy( name, 
	&fp->font_reference.family.structured_name_value.idseq->sequence_value.iso9541_ids[1].iso9541_id_value.octet_string->sequence_value.bytes[0],
	length
	    /* length of the byte string */
    );	

    name[length] = '\0';

    /* now look through the blasted table */
    for( i = 0 ; i < NFAMILIES; i++ ){
    
	if( strcmp( family[i].name, name ) == 0 ){
	    break;
	}

    }

    if( i == NFAMILIES ){
	fprintf( stderr, "[TryFamily]: Can not find org/kind in family table\n" );
	fprintf( stderr, "Name is '%s'\n", name );
	return( FAILURE );
    }

    SlaveRval = SetUpFont( fp, family[i].dsclass );
    if( SlaveRval == FAILURE ){
	return( FAILURE );
    }
    else {
	return( SUCCESS );
    }

}

int SetUpFont( fp, dsclass )
FONT_PAIR_type *fp;
VALUE_type dsclass;
{

    int IsBold;
    int IsItalic;
    int BoldItalic;

    if( fp->font_reference.weight == 7 )
	IsBold = YES;
    else
	IsBold = NO;	

    if( fp->font_reference.posture == 4 )
	IsItalic = YES;
    else
	IsItalic = NO;

    if( IsBold == NO && IsItalic == NO )
	BoldItalic = NONE;
    else if( IsBold == YES && IsItalic == NO )
	BoldItalic = BOLD;
    else if( IsBold == NO && IsItalic == YES )
	BoldItalic = ITALIC;
    else BoldItalic = BOTH;


    /* handle blasted times specially */
    if( dsclass == DSCLASS_SERIFS_legi_rounded ){
	switch( BoldItalic ){
	case NONE:
	    strcpy( FontInfo[fp->font_id].TroffName, "R" ); 
	    break;
	case BOLD:
	    strcpy( FontInfo[fp->font_id].TroffName, "B" ); 
	    break;
	case ITALIC:
	    strcpy( FontInfo[fp->font_id].TroffName, "I" ); 
	    break;
	case BOTH:
	    strcpy( FontInfo[fp->font_id].TroffName, "BI" ); 
	    break;
	}
	return( SUCCESS );
    }

    /* must be helvetica or courier */
    if( dsclass == DSCLASS_SANS_gothic_neo )
	strcpy( FontInfo[fp->font_id].TroffName, "H" );
    else
	strcpy( FontInfo[fp->font_id].TroffName, "C" );

    switch( BoldItalic ){
    case NONE:
	break;
    case BOLD:
	strcat( FontInfo[fp->font_id].TroffName, "B" );
	break;
    case ITALIC:
	strcat( FontInfo[fp->font_id].TroffName, "O" );
	break;
    case BOTH:
	strcat( FontInfo[fp->font_id].TroffName, "D" );
	break;
    }

    return( SUCCESS );

}
