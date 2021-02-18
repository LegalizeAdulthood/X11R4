
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
Il_state_t my_il_state;
/**--------------------------------------------------------------------------**/
Cmpn_t     my_cmpn;
/**--------------------------------------------------------------------------**/
Ar_init_t  my_init = { 2, 1200, YES };
/**--------------------------------------------------------------------------**/

main(argc, argv, env)
  int argc;
  char **argv, **env;
{
  File_info_t *fiP;
  bool result;
  NullObj_t *resultP;
  Cmpn_t *bodyP;
  extern bool debug_on;

    /**/debug_on = argc - 1;

    ArInit(&my_init );
    ArStateInit(&my_il_state);
    ArCmpnInit(&my_cmpn);

    fiP = ArFileInit(stdin);

    resultP = ArDocRead(fiP, &my_il_state);

    for (bodyP=(Cmpn_t *)my_il_state.bodyP; 
	 bodyP!=NULL; 
	 bodyP=bodyP->nextP) {
	    ArFprintf(stdout, "\n<!Comment, another top-level object>\n");
	    /** ArObjectWrite(bodyP, &my_il_state, stdout); BT **/
	    if (Identify(bodyP) == ID_Cmpn) {
		ArCmpnBufDump(bodyP, stdout);
	    }
	    else if (Identify(bodyP) == ID_Comment) {
		ArFprintf(stdout, "<!Comment, found a top-level comment: %Q>\n",
				((Comment_t *)bodyP)->text.outP );
	    }
	    else if (Identify(bodyP) == ID_Table) {
		ArFprintf(stdout, "<!Comment, found a top-level table>\n");
	    }
	    else {
		ArFprintf(stdout,"<!Comment, unexpected %d at top-level??>\n",
				Identify(bodyP) );
	    }
    }
    ArFprintf(stdout, "\n");

    /** That's all, folks! **/
    exit( 0 );
}
/**--------------------------------------------------------------------------**/
