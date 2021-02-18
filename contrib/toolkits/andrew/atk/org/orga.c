/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

#ifndef lint
static char *rcsidorga_c = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/org/RCS/orga.c,v 1.8 89/09/07 19:43:26 tom Exp $";
#endif

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Org Application-Class

MODULE	orga.c

VERSION	1.0

AUTHOR	TC Peters
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Org Application-Class.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  01/19/89	Created (TCP)
  05/10/89	Move im_Create outside im_SetView (macro problem) (TCP)
  05/19/89	Added Ancestry testing in Org_Hit (TCP)
  05/24/89	Permit argument of non-existent org file (TCP)
  05/31/89	Add CLissID to FinalizeObject parameters (TCP)
  08/22/89	Use super_ParseArgs (TCP)
		Remove Testing via Org_Hit
		Remove parochial Specifications
  09/07/89	Set V1.0 msg (TCP)

END-SPECIFICATION  ************************************************************/

#include  <im.ih>
#include  <filetype.ih>
#include  <frame.ih>
#include  <apt.h>
#include  <tree.ih>
#include  <org.ih>
#include  <orgv.ih>
#include  <orga.eh>


#define  Org		    (self->org_data_object)
#define  OrgView	    (self->org_view_object)
#define  Frame		    (self->frame)
#define  Im		    (self->im)
#define  Source		    (self->source)
#define  Tree		    (Org->tree_data_object)

boolean 
orgapp__InitializeObject( classID, self )
  register struct classheader	 *classID;
  register struct orgapp	 *self;
  {
  IN(orgapp_InitializeObject);
  *Source = 0;
  orgapp_SetMajorVersion( self, 1 );
  orgapp_SetMinorVersion( self, 0 );
  OUT(orgapp_InitializeObject);
  return TRUE;
  }

void
orgapp__FinalizeObject( classID, self )
  register struct classheader	 *classID;
  register struct orgapp	 *self;
  {}

boolean
orgapp__ParseArgs( self, argc, argv )
  register struct orgapp	 *self;
  register int			  argc;
  register char			**argv;
  {
  IN(orgapp_ParseArgs);
  super_ParseArgs( self, argc, argv );
  while ( *++argv )
    { DEBUGst(ARGV,*argv);
    if ( **argv == '-' )
      {
      if ( strcmp( *argv, "-D" ) == 0 )
        debug = 1;
      else  printf( "Org: Unrecognized switch '%s' (Ignored)\n", *argv );
      }
      else
      {
      if ( *(Source) == 0 )
        strcpy( Source, *argv );
	else
	printf( "Org: Excessive Argument (Ignored) '%s'\n", *argv );
      }
    }
  OUT(orgapp_ParseArgs);
  return TRUE;
  }

boolean
orgapp__Start( self )
  register struct orgapp         *self;
  {
  register FILE			 *file;
  long				  id, status = true;

  super_Start( self );
  if ( Org = org_New() )
    org_SetDebug( Org, debug );
  if ( OrgView = orgv_New() )
    orgv_SetDebug( OrgView, debug );
  if ( Org  &&  OrgView )
    {
    Frame = frame_New();
    frame_SetView( Frame, OrgView );
    Im = im_Create(NULL);
    im_SetView( Im, Frame );
    if ( *Source )
      {
      org_SetFileName( Org, Source );
      frame_SetTitle( Frame, org_FileName( Org ) );
      if ( file = fopen( Source, "r" ) )
        {
        filetype_Lookup( file, Source, &id, 0);
        org_Read( Org, file, NULL );
        fclose( file );
        }
      }
    if ( status == true )
      {
      orgv_SetDataObject( OrgView, Org );
      orgv_WantInputFocus( OrgView, OrgView );
      }
    return  status;
    }
    else 
    {
    printf("Org: Failed to Create objects\n");
    return FALSE;
    }
  }
