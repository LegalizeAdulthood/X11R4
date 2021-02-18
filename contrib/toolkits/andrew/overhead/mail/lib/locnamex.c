/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
	locnamex.c -- Deallocator for low-level resolver of local addresses
*/

#include <stdio.h>
#include "mail.h"

void la_FreeMD(MD)
struct MailDom *MD;
{/* Somebody's about to erase the pointer MD. */

    if (MD != NULL && (--(MD->Refs)) == 0) {	/* Keep the reference count */
	if (MD->Next != NULL && MD->Prev != NULL) {
	    MD->Next->Prev = MD->Prev;
	    MD->Prev->Next = MD->Next;
	}
	if (MD->Orig != NULL) free(MD->Orig);
	if (MD->Final != NULL) free(MD->Final);
	if (MD->FwdPrefs != NULL) free(MD->FwdPrefs);
	if (MD->Fwds != NULL) {
	    if (MD->Fwds[0] != NULL) free(MD->Fwds[0]);
	    free(MD->Fwds);
	}
	free(MD);
    }
}
