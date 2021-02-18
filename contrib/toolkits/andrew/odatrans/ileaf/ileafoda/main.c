/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/*
 * ileafoda
 *
 * interleaf (accii) to oda translator,
 *
 */
#include <ext.h>

/*
 * declarations for the ascii reader
 */

Il_state_t my_il_state;
Cmpn_t	my_cmpn;
Ar_init_t my_init = { 2, 1200, YES };

/*
 * ODA Toolkit declarations
 */
INIT_PARMS_type parms;
DOCUMENT_type Doc;

/*
 * translator declarations
 */
INT_type debug = 0;
char *cmdname = NULL;

main(argc, argv)
	int argc; char *argv[];
{
	char *outfile = NULL;

	cmdname = argv[0];

	while (--argc > 0 && (++argv)[0][0] == '-') {
		switch (argv[0][1]) {

		case 'o':	/* output file */
			if( argv[0][2] != '\0') {
				outfile = &argv[0][2];
			} else {
				argv++; argc--;
				outfile = argv[0];
				if(*outfile == NULL || *outfile == '-' )
					fatal("Missing output file for -o option");
			}
			continue;
		case 'd':	/* debugging */
			if( argv[0][2] != '\0') {
				if( freopen(&argv[0][2], "w", stderr) == NULL) {
					/*
					 * apparently stderr is now completely broken so
					 * is kind of difficult to get this messages out
					 */
					freopen("/dev/tty", "w", stderr);
					fatal("Cannot open %s for trace", &argv[0][2]);
				}
			}
			debug++;
			continue;
		default:	/* FALL THROUGH */
		case 'x':
			usage();
		}
	}
	if(outfile != NULL)
		if( freopen(outfile, "w", stdout) == NULL) {
			fatal("Cannot open %s", outfile);
		}
	if( argc == 1 )
		if( freopen(argv[0], "r", stdin) == NULL) {
			fatal("Cannot open %s", argv[0]);
		}
	init();
	doit();
	cleanup();
}

usage()
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "%s:\t-d (for debug)\n", cmdname);
	fprintf(stderr, "\t[-o output] [input]\n");
	exit(0);
}
/*
 * initialize the ascii reader
 * read the document
 *
 * initialize the Toolkit
 */

init()
{
	File_info_t *fiP;
	INT_type status;
	CONSTITUENT DocProf;
	

	ArInit(&my_init );
	ArStateInit(&my_il_state);
	ArCmpnInit(&my_cmpn);

	fiP = ArFileInit(stdin);

	ArDocRead(fiP, &my_il_state);

	status = LOCAL_Init();
	if(status == LCL_ERROR_INT)
		fatal("Couldn't init local : %s", ErrorMsg(LCLError));

	parms.automatic_label = BOOL_true;
	parms.error_od = fileno(stderr);
	parms.format_errors = BOOL_true;
	parms.output_filter = NULL_FILTER_ROUTINE;

	status = LOCAL_Init();
	if(status == LCL_ERROR_INT)
		fatal("Couldn't init toolkit: %s", ErrorMsg(LCLError));
	status = InitToolKit(&parms);
	if(status == ERROR_INT)
		fatal("Couldn't init toolkit: %s", ErrorMsg(TKError));
	if((Doc = MakeDocument()) == ERROR_DOCUMENT)
		fatal("Couldn't create ODA document %s", ErrorMsg(TKError));
	if((DocProf = MakeDocumentProfile(Doc)) == ERROR_CONSTITUENT)
		fatal("Couldn't create DAP %s", ErrorMsg(TKError));

}

cleanup()
{
	INT_type status = SUCCESS_INT;

	status = WriteODIF(fileno(stdout), Doc, dp_INTERCHANGE_FMT_CLASS_a);
	if(status == ERROR_INT)
		fatal("Couldn't write odif stream: %s", ErrorMsg(TKError));
	status = LOCAL_WriteBytes(fileno(stdout), (POINTER_type) NULL, (INT_type) -1);
	if(status == ERROR_INT)
		fatal("Couldn't close odif stream: %s", ErrorMsg(TKError));
	if(debug) {
		status = WriteDocumentState(fileno(stderr), Doc);
		if(status == ERROR_INT)
			fatal("Couldn't provide debugging", ErrorMsg(TKError));
	}
	status = FinalToolKit();
	if(status == ERROR_INT)
		fatal("Couldn't finalize toolkit: %s", ErrorMsg(TKError));
	status = LOCAL_Final();
	if(status == LCL_ERROR_INT)
		fatal("Couldn't finalize local: %s", ErrorMsg(LCLError));
}
