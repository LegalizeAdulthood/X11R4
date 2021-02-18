#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: XRdBitFD.c,v 5.4 89/11/22 15:15:09 glennw Exp $";
#endif /* RCS_ID */
#endif /* LINT */

/*
 * Copyright 1987, 1988 by Digital Equipment Corporation, Maynard,
 * Massachusetts, and the Massachusetts Institute of Technology, Cambridge,
 * Massachusetts.  Copyright 1987 by Jordan Hubbard.  Copyright 1987, 1988 by
 * Ardent Computer Corporation, Sunnyvale, Ca.  Copyright 1989 by Tektronix,
 * Inc., Beaverton, Oregon.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Ardent Computer Corporation, Jordan
 * Hubbard, Digital Equipment, Tektronix or M.I.T.  not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Ardent Computer Corporation,
 * Jordan Hubbard, Digital Equipment, Tektronix and M.I.T.  make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL EQUIPMENT, TEKTRONIX,
 * AND M.I.T.  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL
 * EQUIPMENT, TEKTRONIX, OR M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 */

/* Copyright, 1988, Massachusetts Institute of Theology */

/* Jim says Xos is in X11 now, so... */
#include "X11/Xos.h"
#include "X11/Xlib.h"
#include "X11/Xutil.h"
#include "Xlibint.h"
#include <stdio.h>

#define MAX_LINE 1000

int XReadBitmapFileData(filename, width, height, data, x_hot, y_hot)
     char *filename;
     unsigned int *width, *height;   /* RETURNED */
     char **data;        /* RETURNED */
     int *x_hot, *y_hot;    /* RETURNED */
{
  FILE *stream;
  char *bits = 0;
  char *ptr;
  char line[MAX_LINE];
  int size, bytes;
  char name_and_type[MAX_LINE];
  char *type;
  int value;
  int version10p;
  int padding;
  int bytes_per_line;
  unsigned int ww = 0;
  unsigned int hh = 0;
  int hx = -1;
  int hy = -1;

  if (!filename || !(stream = fopen(filename, "r")))
    return(BitmapOpenFailed);

  for (;;) {
    if (!fgets(line, MAX_LINE, stream))
      break;
    if (strlen(line) == MAX_LINE-1) {
      fclose(stream);
      return(BitmapFileInvalid);
    }

    if (sscanf(line, "#define %s %d", name_and_type, &value) == 2) {
      if (!(type = rindex(name_and_type, '_')))
	type = name_and_type;
      else
	type++;
      if (!strcmp("width", type))
	ww=(unsigned int) value;
      if (!strcmp("height", type))
	hh=(unsigned int) value;
      if (!strcmp("hot", type)) {
	if (type--==name_and_type || type--==name_and_type)
	  continue;
	if (!strcmp("x_hot", type))
	  hx = value;
	if (!strcmp("y_hot", type))
	  hy = value;
      }
      continue;
    }
    
    if (sscanf(line, "static short %s = {", name_and_type) == 1)
      version10p = 1;
    else if (sscanf(line, "static unsigned char %s = {", name_and_type) == 1)
      version10p = 0;
    else if (sscanf(line, "static char %s = {", name_and_type) == 1)
      version10p = 0;
    else continue;

    if (!(type = rindex(name_and_type, '_')))
      type = name_and_type;
    else
      type++;
    if (strcmp("bits[]", type))
      continue;
    
    if (!ww || !hh) {
      fclose(stream);
      return(BitmapFileInvalid);
    }

    padding = 0;
    if ((ww % 16) && ((ww % 16) < 9) && version10p)
      padding = 1;

    bytes_per_line = (ww+7)/8 + padding;
    
    size = bytes_per_line * hh;
    bits = (char *) Xmalloc( size );
    if (!bits) {
      fclose(stream);
      return(BitmapNoMemory);
    }

    if (version10p)
      for (bytes=0, ptr=bits; bytes<size; (bytes += 2)) {
	if (fscanf(stream, " 0x%x%*[,}]%*[ \n]", &value) != 1) {
	  Xfree(bits);
	  fclose(stream);
	  return(BitmapFileInvalid);
	}
	*(ptr++) = value & 0xff;
	if (!padding || ((bytes+2) % bytes_per_line))
	  *(ptr++) = value >> 8;
      }
    else
      for (bytes=0, ptr=bits; bytes<size; bytes++, ptr++) {
	if (fscanf(stream, " 0x%x%*[,}]%*[ \n]", &value) != 1) {
	  Xfree(bits);
	  fclose(stream);
	  return(BitmapFileInvalid);
	}
	*ptr=value;
      }
    
  }

  if (!bits) {
    fclose(stream);
    return(BitmapFileInvalid);
  }
  *data = bits;
  *width = ww;
  *height = hh;

  if (x_hot)
    *x_hot = hx;
  if (y_hot)
    *y_hot = hy;

  fclose(stream);
  return(BitmapSuccess);
}
