class schedapp : application [app]
  {
  classprocedures:
    InitializeObject( struct schedapp *)	returns boolean;

  overrides:
    ParseArgs( int argc, char **argv)	returns boolean;
    Start()				returns boolean;
    Run()				returns int;

  data:
    struct frame		    	*frame;
    struct im				*im;
    struct sched			*sched;
    struct schedv			*schedview;
    char				 stream_name[512];

  };
