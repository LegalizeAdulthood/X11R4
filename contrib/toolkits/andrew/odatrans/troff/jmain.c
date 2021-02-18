/**********************************************
(c) Copyright 1988 Carnegie Mellon University
See andrew/oda/doc/COPYRITE
***********************************************/
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/jmain.c,v 1.3 89/09/08 18:47:59 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/jmain.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
CMU-ITC-071) for full text & permitted uses.

$Log:	jmain.c,v $
 * Revision 1.3  89/09/08  18:47:59  mss
 * added finalization
 * 
 * Revision 1.2  89/09/08  17:49:34  mss
 * /tmp/msg
 * 
 * Revision 1.1  89/08/30  16:34:39  mss
 * Initial revision
 * 
 * Revision 1.2  89/05/17  15:10:54  jaap
 * Initialize font to NULL_SEQUENCE after toolkit to prevent call off
 * DeleteSequence with rubbish
 * plugging memory leaks
 * 
 * Revision 1.1  89/01/05  18:46:14  jaap
 * Initial revision
 * 
 */

#ifndef lint
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/odatrans/troff/RCS/jmain.c,v $";
#endif


#include <ext.h>

/*
 * A quick main program to allow a start up
 *
 * We will process the arguments or else stdin
 */

char *progname;
char msgsbuf[BUFSIZ];	/* buffer for errors etc. */

/*
 * Toolkit declarations
 */
INIT_PARMS_type parms;
DOCUMENT_type NewDoc;
dp_UNIT_SCALING_type smu;


main(argc, argv)
int argc; char *argv[];
{
	progname = *argv;
	argc--;
	argv++;

	if ( argc > 0) {    /* input file as argument, we process just one file */
		if (freopen(*argv, "r", stdin) == NULL) {
			sprintf(msgsbuf, "Cannot open %s", *argv);
			errexit(msgsbuf);
		}
	}

	initoda();	/* will be replaced by ann's stuff */
	trinit();	/* initialize troff */
	walk();		/* walk around, produce troff etc.*/
	trflush();
	finaloda();
	trcomment("End of the fun");
	exit(0);
}

initoda()
{
	INT_type status;
	CONSTITUENT_type profile;
	extern cc_CHARACTER_FONTS_type font;

	parms.automatic_label = BOOL_true;
	parms.error_od = fileno(stderr);
	parms.format_errors = BOOL_true;
	parms.output_filter = NULL_FILTER_ROUTINE;

	status = LOCAL_Init();
	if (status == LCL_ERROR_INT) {
		sprintf(msgsbuf, "Couldn't init local: %s", ErrorMsg(LCLError));
		errexit(msgsbuf);
	}
	status = InitToolKit(&parms);
	if (status == ERROR_INT) {
		sprintf(msgsbuf, "Couldn't init toolkit: %s", ErrorMsg(TKError));
		errexit(msgsbuf);
	}

	/*
	 * used in ProcessCP, needs to be initialized as well
	 */
	font = NULL_SEQUENCE;

	NewDoc = ReadODIF(fileno(stdin));
	if (NewDoc == ERROR_DOCUMENT) {
		sprintf(msgsbuf, "Error reading ODIF %s", ErrorMsg(TKError));
		errexit(msgsbuf);
	}
	/* Would this be the place to look for fonts etc, or will we do that later?. */
	if (InitializeFonts(NewDoc) == FAILURE) {
		sprintf(msgsbuf, "Cannot init fonts");
		errexit(msgsbuf);
	}
#if 0  /* shut up toolkit, since we won't find it */
	/*
	 * Look for scaling
	 *
	 * it is in the document profile
	 *
	 */
	if ((profile = FindDocumentProfile(NewDoc)) == ERROR_CONSTITUENT) {
		sprintf(msgsbuf, "Couldn't find document profile: %s", ErrorMsg(TKError));
		errexit(msgsbuf);
	}
	if(GetAttr(profile, dp_UNIT_SCALING, (POINTER_type) &smu, NOINHERIT, IGNORE) ==ERROR_INT) {
		switch(TKError){
			case NOATTR_err:
				sprintf(msgsbuf, "No SMU scaling specified");
				trcomment(msgsbuf);
				smu.num = smu.denom = 1;
				break;
			default:
				sprintf(msgsbuf, "Couldn't get scaling %s", ErrorMsg(TKError));
				errexit(msgsbuf);
		}
	}
#endif
	smu.num	= smu.denom = 1;    /* just set in until implemented */
}

walk(){

	if(GetText(NewDoc)== ERROR_INT) {
		sprintf(msgsbuf,"Couldn't get text %s", ErrorMsg(TKError));
		errexit(msgsbuf);
	}
}

INT_type
GetText(doc)
DOCUMENT_type doc;
{
	CONSTITUENT SpecLogRoot;
	INT_type rval = SUCCESS_INT;

	/*
	  * find specific logical document root
	  */
	SpecLogRoot = FindDocumentRoot(doc, BOOL_true, BOOL_true);
	if (SpecLogRoot == ERROR_CONSTITUENT) {
		fprintf(stderr,"Couldn't get specific logical root %s\n",
			ErrorMsg(TKError));
		rval = ERROR_INT;
		goto LEAVE;
	}

	if(DoSingleLO(SpecLogRoot) == ERROR_INT) {
		fprintf(stderr,"Couldn't process logical root %s\n",
			ErrorMsg(TKError));
		rval = ERROR_INT;
		goto LEAVE;
	}
LEAVE:
	return rval;

}

INT_type
DoSingleLO(slr)
CONSTITUENT slr;
{
	INT_type rval;

	SEQUENCE_CONSTITUENT_type contents, subord;
	VALUE_type curcontype;
	INT_type contype, conmode, i;

	rval = SUCCESS_INT;
	if(GetValueAttr(slr, at_OBJECT_TYPE, (POINTER_type) &curcontype, LOCAL)
	   == ERROR_INT ) {
		fprintf(stderr, "Missing object type on constituent %s\n", ErrorMsg(TKError));
		rval = ERROR_INT;
		goto LEAVE;
	}

	switch(curcontype) {
		case at_OBJECT_TYPE_doc_logical_root:	/*FALL_TROUGH*/
		case at_OBJECT_TYPE_comp_logical_obj:
			/*
			 * Children?
			 */
			switch(HasAttr(slr, at_SUBORDINATES)) {
			       case HAS_ATTR:
				       break;	/* Go for it */
			       case NO_ATTR:	/* Nothing to do */
				       goto LEAVE;
			       case ERROR_INT:	/*FALL_THROUGH */
			       default:
				       fprintf(stderr,"Cannot locate subordinate attribute %s\n",ErrorMsg(TKError));
				       rval = ERROR_INT;
				       goto LEAVE;
			}
			if ((rval =
			     GetAttr(slr, at_SUBORDINATES,(POINTER_type)&subord,LOCAL, IGNORE)) == ERROR_INT) {
				fprintf(stderr,"Couldn't get subordinates attribute %s\n",
					ErrorMsg(TKError));
				goto LEAVE;
			}
			for( i = 0; i < subord->length; i++) {
				rval= DoSingleLO(subord->sequence_value.constituents[i]);
			}
			if (DeleteSequence(subord) == ERROR_INT) {
				fprintf(stderr, "Cannot delete subordinates %s\n", ErrorMsg(TKError));
			}
			break;

		case at_OBJECT_TYPE_bas_logical_obj:
			/*
			 * Look for content portions, pull them of, process
			 */
			if ((rval = GetAttr(slr, at_CONTENT_PORTIONS, (POINTER_type) &contents, LOCAL, IGNORE )) == ERROR_INT) {
				fprintf(stderr, "Couldn't get content from logical object %s\n", ErrorMsg(TKError));
				goto LEAVE;
			}
			GetContentType(slr, &contype, &conmode);
			/*
			 * find architecture & process
			 */
			switch(contype) {
				case CharContent:
				     rval = ProcessCharCP(slr, contents, contype, conmode);
					if(rval == ERROR_INT)
						goto LEAVE;
					break;
				case RasterContent:
					procraster(slr, contents, contype, conmode);
					break;
				case GeoContent:
					printf(stderr,"geometric not yet\n");
					break;
				default:
					fprintf(stderr, "Unknown content type %d\n", contype);
					break;
			}
			if(DeleteSequence(contents) == ERROR_INT) {
				fprintf(stderr, "Cannot delete sequence for content portion %s\n", ErrorMsg(TKError));
			}
			break;
		default:
			fprintf(stderr, "Unknown object type %d\n", curcontype);
			break;

	}
LEAVE:
	return rval;
}

finaloda()
{
	INT_type status;

	status = FinalToolKit();
	if (status == ERROR_INT) {
		sprintf(msgsbuf, "Couldn't final toolkit: %s", ErrorMsg(TKError));
		errexit(msgsbuf);
	}

	status = LOCAL_Final();
	if (status == LCL_ERROR_INT) {
		sprintf(msgsbuf, "Couldn't final local: %s", ErrorMsg(LCLError));
		errexit(msgsbuf);
	}



}
