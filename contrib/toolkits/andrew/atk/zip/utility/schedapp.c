
#include <im.ih>
#include <frame.ih>
#include <message.ih>
#include <view.ih>
#include <environ.ih>
#include <lpair.ih>
#include <text.ih>
#include <textv.ih>
#include <sched.ih>
#include <schedv.ih>
#include <schedapp.eh>
#include <andrewos.h>
#include <zip.ih>


boolean 
schedapp__InitializeObject( classID, self )
  register struct classheader		 *classID;
  register struct schedapp		 *self;
  {
  IN(schedapp_InitializeObject);
  *(self->stream_name) = 0;
  OUT(schedapp_InitializeObject);
  return TRUE;
  }

boolean
schedapp__ParseArgs( self, argc, argv )
  register struct schedapp		 *self;
  register int				  argc;
  register char				**argv;
  {
  IN(schedapp_ParseArgs);
  while ( *++argv )
    {
    DEBUGst(ARGV,*argv);
    if ( **argv == '-' )
      {
      if ( strcmp( *argv, "-d" ) == 0 )
        debug = 1;
      else  printf( "Sched: Unrecognized switch '%s'\n", *argv );
      }
      else
      {
      if ( *(self->stream_name) == 0 )
        strcpy( self->stream_name, *argv );
      }
    }
  OUT(schedapp_ParseArgs);
  return TRUE;
  }

boolean 
schedapp__Start( self )
  register struct schedapp			 *self;
  {
  IN(schedapp_Start);
  if( !super_Start(self) )
    return FALSE;
  self->im = im_Create( NULL );
  if ( !self->im )
    {
    printf("Sched: Failed to create new window, exiting.\n");
    exit(1);
    }
if (*self->stream_name == 0 ) strcpy( self->stream_name,"itcCR.scd");
  self->sched = sched_Create( self->stream_name );
  sched_Set_Debug( self->sched, debug );
  self->schedview = schedv_New();
  schedv_Set_Debug( self->schedview, debug );
  schedv_SetDataObject( self->schedview, self->sched );
  self->frame = frame_New();
  frame_SetView( self->frame, self->schedview );
  im_SetView( self->im, self->frame );
  OUT(schedapp_Start);
  return TRUE;
  }

int
schedapp__Run(self)
  register struct schedapp			  *self;
  {
  IN(schedapp_Run);
  schedapp_Fork( self );
  schedv_WantInputFocus( self->schedview, self->schedview );
  DEBUG(...Interacting...);
  while ( im_Interact( 0 ) ) ;
  im_KeyboardProcessor();
  OUT(schedapp_Run);
  return(0);
  }
