#include <stdio.h>
#include <class.h>
#include <im.ih>
#include <frame.ih>
#include <message.ih>
#include <view.ih>
#include <environ.ih>
#include <lpair.ih>
#include <text.ih>
#include <textv.ih>
#include <lt.ih>
#include <ltv.ih>
#include <ltapp.eh>
#include <sys/types.h>
#include <andrewos.h>
#include <zip.ih>


boolean 
ltapp__InitializeObject( classID, self )
  register struct classheader		 *classID;
  register struct ltapp			 *self;
  {
  IN(ltapp_InitializeObject);
  *(self->trace_stream_name) = 0;
  *(self->trace_raster_name) = 0;
  ltapp_SetMajorVersion( self, 1 );
  ltapp_SetMinorVersion( self, 0 );
  OUT(ltapp_InitializeObject);
  return TRUE;
  }

boolean
ltapp__ParseArgs( self, argc, argv )
  register struct ltapp			 *self;
  register int				  argc;
  register char				**argv;
  {
  IN(ltapp_ParseArgs);
  while ( *++argv )
    {
    DEBUGst(ARGV,*argv);
    if ( **argv == '-' )
      {
      if ( strcmp( *argv, "-d" ) == 0 )
        debug = 1;
      else  printf( "LightTable: Unrecognized switch '%s'\n", *argv );
      }
      else
      {
      if ( *(self->trace_stream_name) == 0 )
        strcpy( self->trace_stream_name, *argv );
	else
        if ( *(self->trace_raster_name) == 0 )
          strcpy( self->trace_raster_name, *argv );
	  else  printf( "LightTable: Unrecognized argument '%s'\n", *argv );
      }
    }
  OUT(ltapp_ParseArgs);
  return TRUE;
  }

boolean 
ltapp__Start( self )
  register struct ltapp			 *self;
  {
  IN(ltapp_Start);
  if( !super_Start(self) )
    return FALSE;
  self->im = im_Create( NULL );
  if ( !self->im )
    {
    printf("LightTable: Failed to create new window, exiting.\n");
    exit(1);
    }
  self->lt = lt_New();
  lt_Set_Debug( self->lt, debug );
  if ( self->trace_stream_name[0]  && self->trace_raster_name[0]  )
    if ( lt_Read_Visuals( self->lt, self->trace_stream_name, self->trace_raster_name ) )
      exit (-1);
  self->ltview = ltv_New();
  ltv_Set_Debug( self->ltview, debug );
  ltv_SetDataObject( self->ltview, self->lt );
  self->frame = frame_New();
  frame_SetView( self->frame, self->ltview );
  im_SetView( self->im, self->frame );
  OUT(ltapp_Start);
  return TRUE;
  }

int
ltapp__Run(self)
  register struct ltapp			  *self;
  {
  IN(ltapp_Run);
  ltapp_Fork( self );
  ltv_WantInputFocus( self->ltview, self->ltview );
  DEBUG(...Interacting...);
  while ( im_Interact( 0 ) ) ;
  im_KeyboardProcessor();
  OUT(ltapp_Run);
  return(0);
  }
