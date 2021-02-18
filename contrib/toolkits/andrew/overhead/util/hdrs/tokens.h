/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/hdrs/RCS/tokens.h,v 2.5 89/02/16 12:58:25 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/hdrs/RCS/tokens.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_h = "$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/hdrs/RCS/tokens.h,v 2.5 89/02/16 12:58:25 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
		tokens.h -- Include file for fiddling with tokens.
*/

#include <andyenv.h>

/* Length of packed tokens */
#ifdef AFS_ENV
#define TOKENS_LEN  ((12 * sizeof(long int)) \
			+ sizeof(AuthMagic) \
			+ 2*sizeof(auth_EncryptionKey))
#else /* AFS_ENV */
#define TOKENS_LEN 4
#endif /* AFS_ENV */

#define TokIsPrimary	3
#define TokNotPrimary	2
#define TokLocalPrimary	5
#define TokLocalNotPrimary	4
