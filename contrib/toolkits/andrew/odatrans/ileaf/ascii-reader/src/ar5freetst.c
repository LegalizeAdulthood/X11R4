
/*
 * Copyright 1989 by Interleaf, Inc, 10 Canal Park,  Cambridge, MA. 02141
 *
 *		All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Interleaf not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Interleaf, Inc. makes no representations 
 * about the suitability of this software for any purpose. 
 * It is provided "AS IS" without express or implied warranty.
 *
 * INTERLEAF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL INTERLEAF BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * 
 */

/*************************************************/
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/
#ifdef INTERLEAF_INCLUDE_FILES
#include "cdefs.h"
#include "sysstdio.h"
#else
#ifndef AR_DEFINES
#define AR_DEFINES
typedef short bool;
typedef short rsu;
typedef long Rsu;
typedef short shar;
typedef unsigned short ushar;
#endif
#endif
#include "ar5defs.h"

/**--------------------------------------------------------------------------**/

main(argc, argv, env)
  int argc;
  char **argv, **env;
{
  FILE *ifile        = NULL;
  File_info_t *fiP   = NULL;
  Il_state_t *stateP = NULL;
  NullObj_t *resultP = NULL;
  bool read_doc	     = YES;
  bool free_it       = NO;
  bool result        = NO;
  bool free_result   = NO;
  int  read_error    = 0;
  int  free_error    = 0;
  long new_sbrk	     = 0;
  long old_sbrk	     = 0;
  long diff    	     = 0;
  long total_count   = 0;
  long exit_count    = 1024;

    while (*++argv != NULL) {
	if (strcmp(*argv, "-count") == 0) {
	    exit_count = atoi(*++argv);
	}
	else if (strcmp(*argv, "-free") == 0) {
	    free_it = YES;
	}
	else if (strcmp(*argv, "-doc") == 0) {
	    read_doc = YES;
	}
	else if (strcmp(*argv, "-object") == 0) {
	    read_doc = NO;
	}
	else if (**argv != '-') {
	    ifile = fopen(*argv, "r");
	    break;
	}
	else {
	    ArFprintf(stderr, 
		"Usage: %s [-free] [-count N] [-doc|-object] Infile.\n", 
		argv[0]);
	    exit(-1);
	    break;
	}
    }
    if (read_doc == NO) {
	/** initialize it here, re-use it in the loop. **/
	rewind(ifile);
	fiP = ArFileInit(ifile);
    }
    for ( ; ; ++total_count) {
	new_sbrk = sbrk(0);
	diff = new_sbrk - old_sbrk;
	old_sbrk = new_sbrk;
	if (diff != 0 || total_count % 256 == 255) {
	    ArFprintf(stdout, "read_error %d, free_error %d, ",
		read_error, free_error);
	    ArFprintf(stdout, "sbrk(0) %#6x, diff %#6x, total_count %d\n",
		new_sbrk, diff, total_count );
	}
	if (total_count >= exit_count) {
	    break;
	}
	if (read_doc == YES) {
	    rewind(ifile);	/** start with a new file every time. **/
	    fiP = ArFileInit(ifile);
	    stateP = NEW(Il_state_t);
	    ArStateInit(stateP);
	    resultP = ArDocRead(fiP, stateP);
	    if (resultP == NULL) {
		++read_error;
		break;
	    }
	    if (free_it == YES) {
		free_result = ArObjectFree(stateP);
		if (free_result == NO) {
		    ++free_error;
		    break;
		}
	    }
	    ArObjectFree(fiP);	/** always do this. **/
	}
	else {
	    /** read an object at a time. **/
	retry:
	    resultP = ArNextObject(fiP, NULL);
	    if (resultP == NULL) {
		if (feof(ifile)) {
		    /** ArFprintf(stderr, "EOF during object read. retrying.\n"); BT **/
		    rewind(ifile);
		    ArObjectFree(fiP);	/** always do this. **/
		    fiP = ArFileInit(ifile);
		    goto retry;
		}
		++read_error;
		break;
	    }
	    if (free_it == YES) {
		free_result = ArObjectFree(resultP);
		if (free_result == NO) {
		    ++free_error;
		    break;
		}
	    }
	}
    }
    /** if (diff != 0) { BT **/
    {
	ArFprintf(stdout, "Final:\n");
	ArFprintf(stdout, "read_error %d, free_error %d, ",
	    read_error, free_error);
	ArFprintf(stdout, "sbrk(0) %#x, diff %#x, total_count %d\n",
	    new_sbrk, diff, total_count );
    }
    ArFprintf(stdout, "Ended with exit_count %d\n", exit_count);

    /** That's all, folks! **/
    exit( 0 );
}
/**--------------------------------------------------------------------------**/
