/*\
 *
 *  (C) Copyright 1989 by Carnegie Mellon University
 *
 *  Permission to use, copy, modify, and distribute these programs
 *  and their documentation for any purpose and without fee is
 *  hereby granted, provided that this copyright and permission
 *  notice appear on all copies and supporting documentation, and
 *  that the name of Carnegie Mellon University not be used in
 *  advertising or publicity pertaining to distribution of the
 *  programs without specific prior permission and notice be given
 *  in supporting documentation that copying and distribution is
 *  by permission of Carnegie Mellon University.
 *
 *  Carnegie Mellon University makes no representations about the
 *  suitability of this software for any purpose.  It is provided
 *  as is, without express or implied warranty.
 *
 *  Software by Andrew Werth, Information Technology Center,
 *  Carnegie Mellon University, except where noted.
\*/

/*\
 *  This code has been taken from the Fuzzy Bitmap library, as
 *  noted below.  It has been modified in several ways:  Input/
 *  Output has been changed to match Raster Graphics Tool Kit
 *  specifications; debugging output has been removed; error
 *  handling has been modified for RGTK specs; procedures
 *  have been made static where appropriate; and minor changes
 *  to more closely resemble RGTK coding conventions.
 *
 *  Modifications by Andrew Werth.
 *
\*/

/*****************************************************************
 * flbyte.c: FBM Library 0.94 (Beta test) 20-May-89  Michael Mauldin
 *
 * Copyright (C) 1989 by Michael Mauldin.  Permission is granted to
 * use this file in whole or in part provided that you do not sell it
 * for profit and that this copyright notice is retained unchanged.
 *
 * flbyte.c: 
 *
 * CONTENTS
 *	get_long (rfile, order)
 *	get_short (rfile, order)
 *	put_long (long, wfile, order)
 *	put_short (word, wfile, order)
 *	machine_byte_order ()
 *
 *	order	BIG	msb first (Sun, IBM-RT, 68000)
 *		LITTLE	lsb first (Vax, 6502)
 *
 * EDITLOG
 *	LastEditDate = Sat May 20 19:11:04 1989 - Michael Mauldin
 *	LastFileName = /usr2/mlm/src/misc/fbm/flbyte.c
 *
 * HISTORY
 * 20-May-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *	Bug fix from Dave Cohrs <dave@cs.wisc.edu>
 *
 * 07-Mar-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *	Beta release (version 0.9) mlm@cs.cmu.edu
 *
 * 12-Nov-88  Michael Mauldin (mlm) at Carnegie-Mellon University
 *	Created.
 *****************************************************************/

#define TK_TKI_fbm
#define TK_IMPL_fbm
#define TK_GVARS_flbyte

#include <fbm.h>
#include <rgsite.h>
#include <rgreal.h>

#undef TK_GVARS_flbyte
#undef TK_IMPL_fbm
#undef TK_TKI_fbm

int (*fbm_GetByte)();
int (*fbm_PutByte)();
char *fbm_Parameter;

int fbmread(buffer,len,num,from)
    unsigned char *buffer;
    int	len;
    int num;
    FILE *from;
{
    int rval;


    unsigned char *ch;
    int i;
    int byte;

    ch = buffer;
    
    for (i = 0; i < len * num; i++) {
	byte = (*fbm_GetByte)(fbm_Parameter);
	if (byte == RG_EOF) {
	    rval = 0;
	    goto LEAVE;
	}
	*ch = byte & 0xff;
	ch++;
    }

    rval = i;

LEAVE:

    return(rval);
}


/*\
 *  FBMWRITE:  Simulate the fwrite() call.
\*/

int fbmwrite(string, size, len, fp)
    char *string;
    int size;
    int len;
    FILE *fp;
{
    int i;
    int bytes;

    bytes = size * len;
    for (i = 0; i < bytes; i++) {
	(*fbm_PutByte)(fbm_Parameter, string[i]);
    }
    
LEAVE:

    return(i);
}


/*\
 *  FBMPRINTF:  Simulate fprintf("string").
\*/

int fbmprintf(string)
    char *string;
{
    char *ch;

    ch = string;
    while (*ch != '\0') {
	(*fbm_PutByte)(fbm_Parameter, *ch);
	ch++;
    }
    return(0);
}



/****************************************************************
 * get_long: get a long integer from a file
 ****************************************************************/

long get_long (file, order)
FILE *file;
int order;
{ register long word;

  if (order == BIG)
  { word = FBMGETC(file) & 0x0ff;
    word = word << 8 | (FBMGETC(file) & 0x0ff);
    word = word << 8 | (FBMGETC(file) & 0x0ff);
    word = word << 8 | (FBMGETC(file) & 0x0ff);
  }
  else
  { word = FBMGETC(file) & 0x0ff;
    word = word | ((FBMGETC(file) & 0x0ff) << 8);
    word = word | ((FBMGETC(file) & 0x0ff) << 16);
    word = word | ((FBMGETC(file) & 0x0ff) << 24);
  }

  return (word);
}

/****************************************************************
 * put_long: Write a long integer to a file
 ****************************************************************/

put_long (word, file, order)
register long word;
FILE *file;
int order;
{
  if (order == BIG)
  { FBMPUTC ((word >> 24) & 0x0ff, file); /* Put out biggest byte */
    FBMPUTC ((word >> 16) & 0x0ff, file); /* Put out 2nd byte */
    FBMPUTC ((word >> 8) & 0x0ff, file);  /* Put out 3rd byte */
    FBMPUTC (word & 0x0ff, file);	  /* Put out little byte */
  }
  else
  { FBMPUTC (word & 0x0ff, file);	 /* Put out littlest byte */
    FBMPUTC ((word >> 8) & 0x0ff, file); /* Put out 3rd byte */
    FBMPUTC ((word >> 16) & 0x0ff, file);/* Put out 2nd byte */
    FBMPUTC ((word >> 24) & 0x0ff, file);/* Put out biggest byte */
  }
}

/****************************************************************
 * get_short: get a short integer from a file
 ****************************************************************/

get_short (file, order)
FILE *file;
int order;
{ register int word;

  if (order == BIG)
  { word = FBMGETC(file) & 0x0ff;
    word = word << 8 | (FBMGETC(file) & 0x0ff);
  }
  else
  { word = FBMGETC(file) & 0x0ff;
    word = word | ((FBMGETC(file) & 0x0ff) << 8);
  }

  return (word);
}

/****************************************************************
 * put_short: Write a short integer to a file
 ****************************************************************/

put_short (word, file, order)
register int word;
FILE *file;
int order;
{
  if (order == BIG)
  { FBMPUTC ((word >> 8) & 0x0ff, file);/* Put out 3rd byte */
    FBMPUTC (word & 0x0ff, file);	/* Put out littlest byte */
  }
  else
  { FBMPUTC (word & 0x0ff, file);	/* Put out littlest byte */
    FBMPUTC ((word >> 8) & 0x0ff, file);/* Put out 3rd byte */
  }
}

/****************************************************************
 * machine_byte_order: Return BIG or LITTLE for the current machine
 ****************************************************************/

machine_byte_order ()
{ short testshort;
  char *teststr = (char*) &testshort;

  teststr[0] = '\1'; teststr[1] = '\0';

  if (testshort == 1)
  { return (LITTLE); }
  else
  { return (BIG); }
}



