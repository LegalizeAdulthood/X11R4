/******************************************************************
Copyright 1987 by Apollo Computer Inc., Chelmsford, Massachusetts.
Copyright 1989 by Hewlett-Packard Company.

                        All Rights Reserved

Permission to use, duplicate, change, and distribute this software and
its documentation for any purpose and without fee is granted, provided
that the above copyright notice appear in such copy and that this
copyright notice appear in all supporting documentation, and that the
names of Apollo Computer Inc., the Hewlett-Packard Company, or MIT not
be used in advertising or publicity pertaining to distribution of the
software without written prior permission.

HEWLETT-PACKARD MAKES NO WARRANTY OF ANY KIND WITH REGARD
TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  Hewlett-Packard shall not be liable for errors
contained herein or direct, indirect, special, incidental or
consequential damages in connection with the furnishing,
performance, or use of this material.

This software is not subject to any license of the American
Telephone and Telegraph Company or of the Regents of the
University of California.
******************************************************************/

#include "apc.h"

/*
 * apcQueryBestSize -- DDX interface (screen)
 *      Answer the usual QueryBest questions.
 */
void
apcQueryBestSize(class, pwidth, pheight)
    int     class;
    short  *pwidth;
    short  *pheight;
{
    switch (class)
    {
      case CursorShape:
#ifdef GPR_SPRITES
      /* add this code if we add GPR sprite-rendering */
	  *pwidth  = 16;
	  *pheight = 16;
#endif
	  break;
      case TileShape:
      case StippleShape:
          *pwidth  = TILE_SIZE;
          *pheight = TILE_SIZE;
	  break;
    }
}
