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
 * flgifr.c: FBM Library 0.91 (Beta test) 26-Apr-89  Michael Mauldin
 *
 * Copyright (C) 1989 by Michael Mauldin.  Permission is granted to
 * use this file in whole or in part provided that you do not sell it
 * for profit and that this copyright notice is retained unchanged.
 *
 * Derived from 'giftorle', written by David Koblas
 *
 * +------------------------------------------------------------------+ 
 * | Copyright 1989, David Koblas.                                    | 
 * |   You may copy this file in whole or in part as long as you      | 
 * |   don't try to make money off it, or pretend that you wrote it.  | 
 * +------------------------------------------------------------------+ 
 *
 * flgifr.c:
 *
 * CONTENTS
 *	read_gif (image, stream, mstr, mlen)
 *
 * HISTORY
 * 26-Apr-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *	Beta release (version 0.91) mlm@cs.cmu.edu
 *
 * 19-Feb-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *	Changed name to flgifr from flgif (since there is now an flgifw)
 *
 * 13-Feb-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *	Modified from gif2rle program Copyright 1989 by David Koblas.
 *	Converted to produce FBM image format in memory.
 *
 *
 *****************************************************************/

#define TK_TKI_fbm
#define TK_IMPL_fbm
#define TK_GVARS_flgifr

#include <fbm.h>
#include <rgsite.h>

#undef TK_GVARS_flgifr
#undef TK_IMPL_fbm
#undef TK_TKI_fbm

#ifndef USE_LONG_NAMES

#define FReadRaster	frdras
#define GetCode		getcod
#define IgnoreExtention	ignext
#define LWZReadByte	lrdbyt
#define ReadColorMap	rdclmp
#define ReadInterlaced  rdinlc

#define code_size	codsiz
#define end_size	endsiz
#define firstcode	fircod
#define last_byte	lstbyt
#define max_code	maxcod
#define max_code_size	mxcdsz
#define old_code	oldcod
#define set_code_size	stcosz
#define clear_code	clrcod
#define end_code	endcod
#define old_code	oldcod

#define FLGIFR_strcmp	rlstcm

#endif


#define MAXCOLORMAPSIZE         256

#define TRUE    1
#define FALSE   0
#define BAD     0
#define OK      1

#define MAX_LWZ_BITS            12

#define ReadOK(file,buffer,len) (fbmread(buffer,len,1,file)!=0)
#define EasyFail(str,status)    while(1){return(status);}
#define HardFail(str,status)    while(1){return(status);}

#define LM_to_uint(a,b)                 (((b)<<8)|(a))

extern int (*fbm_GetByte)();
extern char *fbm_Parameter;



read_gif (image, GetByte, Parameter, mstr, mlen)
FBM *image;
int (*GetByte)();
char *Parameter;
char *mstr;
int mlen;
{
FILE *fd;
  unsigned char   buf[16];
  unsigned char   c;
  int             use_global_colormap;
  int             bit_pixel;
  int             count = 0;
  int             rows, cols, rowlen, depth, colors;

  fd = (FILE *) 0;
  fbm_GetByte = GetByte;
  fbm_Parameter = Parameter;

  /* Read magic number 'GIF87a' */
  buf[0] = NEXTMCH (fd, mstr, mlen) & 0xff;
  buf[1] = NEXTMCH (fd, mstr, mlen) & 0xff;
  buf[2] = NEXTMCH (fd, mstr, mlen) & 0xff;
  buf[3] = NEXTMCH (fd, mstr, mlen) & 0xff;
  buf[4] = NEXTMCH (fd, mstr, mlen) & 0xff;
  buf[5] = NEXTMCH (fd, mstr, mlen) & 0xff;
  buf[6] = 0;


  if (FLGIFR_strcmp (buf, GIF_MAGIC) != 0)
    EasyFail("bad magic number (version mismatch?)\n", BAD);

  if (!ReadOK (fd, buf, 7))
    EasyFail("error reading screen descriptor\n", BAD);

  /* Read image data */
  image->hdr.cols = cols = LM_to_uint(buf[0], buf[1]);
  image->hdr.rows = rows = LM_to_uint(buf[2], buf[3]);
  image->hdr.planes = 1;
  image->hdr.bits = depth = (buf[4] & 0x07) + 1;
  image->hdr.physbits = 8;

  /* Pad to even byte */
  if (depth == 1)
  { image->hdr.rowlen = rowlen = 16 * ((cols + 15) / 16); }
  else
  { image->hdr.rowlen = rowlen = 2 * ((cols + 1) / 2); }

  image->hdr.plnlen = rowlen * rows;
  colors = 1 << depth;
  image->hdr.clrlen = 3 * colors;
  image->hdr.title[0] = '\0';
  image->hdr.credits[0] = '\0';
  
  /* Try to guess aspect ratio */
  if (cols == 320 && rows == 200)	{ image->hdr.aspect = 1.2; }
  else if (cols == 320 && rows == 175)	{ image->hdr.aspect = 1.2; }
  else if (cols == 320 && rows == 400)	{ image->hdr.aspect = 0.6; }
  else if (cols == 320 && rows == 350)	{ image->hdr.aspect = 0.6; }
  else if (cols == 640 && rows == 200)	{ image->hdr.aspect = 2.4; }
  else if (cols == 640 && rows == 175)	{ image->hdr.aspect = 2.4; }
  else if (cols == 640 && rows == 400)	{ image->hdr.aspect = 1.2; }
  else if (cols == 640 && rows == 350)	{ image->hdr.aspect = 1.2; }
  else					{ image->hdr.aspect = 1.0; }
  
/************
# ifdef DEBUG
  fprintf (stderr, "\nImage file header:\n\n");
  fprintf (stderr, "cols       %d\n", image->hdr.cols);
  fprintf (stderr, "rows       %d\n", image->hdr.rows);
  fprintf (stderr, "planes     %d\n", image->hdr.planes);
  fprintf (stderr, "bits       %d\n", image->hdr.bits);
  fprintf (stderr, "physbits   %d\n", image->hdr.physbits);
  fprintf (stderr, "rowlen     %d\n", image->hdr.rowlen);
  fprintf (stderr, "plnlen     %d\n", image->hdr.plnlen);
  fprintf (stderr, "clrlen     %d\n", image->hdr.clrlen);
  fprintf (stderr, "aspect     %1.3lf\n", image->hdr.aspect);
  fprintf (stderr, "title      '%s'\n", image->hdr.title);
  fprintf (stderr, "credits    '%s'\n", image->hdr.credits);
# endif
************/
  
  /* Allocate image */
  alloc_fbm (image);
    GetCode(0,0,1);	/* Clear things up?  - A.Werth, 8/16/89 */

  /* Read colormap if given */
  if ((buf[4] & 0x80) == 0x80)
  { if (ReadColorMap (fd, colors, image->cm) == BAD)
      return (BAD);
  }

  while (1)
  { if (!ReadOK (fd, &c, 1))
      EasyFail("No image data -- EOF\n", BAD);

    if (c == ';')
      return OK;

    if (c == '!')
    { if (!ReadOK (fd, &c, 1))
        EasyFail("No extention function code -- EOF\n", BAD);
      if (IgnoreExtention(fd) == BAD)
        return (BAD);
    }

    if (c != ',')
    { 
      /* fprintf(stderr, "Bogus character ignoring '%c'\n", c);*/
      continue;
    }

    if (count == 1)
      HardFail("This file contains more than one image! FAILING\n", 1);
    count++;

    if (!ReadOK (fd, buf, 9))
      EasyFail("Couldn't read left/top/width/height\n", TRUE);

    if ((buf[8] & 0x80) == 0x80)
      use_global_colormap = FALSE;
    else
      use_global_colormap = TRUE;

    bit_pixel = 2 << (buf[8] & 0x07);
    
    if (!use_global_colormap && bit_pixel > colors)
    { HardFail ("Local colormap has more colors than global!\n", BAD); }

    if (!use_global_colormap)
    { 
	/*	fprintf (stderr,
		"Overriding global colormap (%d) with local (%d)\n",
		colors, bit_pixel);
	 */

      colors = bit_pixel;
      image->hdr.clrlen = 3 * colors;
      
      if (ReadColorMap(fd, colors, image->cm) == BAD)
        return BAD;
    }

    if ((buf[8] & 0x40) == 0x40)
    { 
      if (ReadInterlaced(fd,
			 LM_to_uint(buf[4],buf[5]),
			 LM_to_uint(buf[6],buf[7]),
			 image->bm,
			 rowlen) == BAD)
	return BAD;
    }
    else
    { if (FReadRaster(fd,
                     LM_to_uint(buf[4], buf[5]),
                     LM_to_uint(buf[6], buf[7]),
                     image->bm,
                     rowlen) == BAD)
        return BAD;
      else
        return OK;
    }
  }
}

static ReadColorMap (fd, number, buffer)
  FILE           *fd;
  int             number;
  unsigned char   *buffer;
{
  int             i;
  unsigned char   rgb[3], *red, *grn, *blu;

  red = buffer;
  grn = buffer + number;
  blu = grn + number;

  for (i = 0; i < number; i++)
  { if (!ReadOK (fd, rgb, sizeof(rgb)))
      EasyFail("Bogus colormap\n", BAD);

    red[i] = rgb[0];
    grn[i] = rgb[1];
    blu[i] = rgb[2];
  }

  return OK;
}

static IgnoreExtention(fd)
  FILE           *fd;
{
  static char     buf[256];
  unsigned char   c;

  while (1)
  {
    if (!ReadOK (fd, &c, 1))
      EasyFail("EOF in extention\n", BAD);

    if (c == 0)
      return OK;
/* 
    Note:  I don't see how this code can work, so I have changed
    it to use fread().  - Andrew Werth

    if (read(fd, buf, (int) c) != (int) c)
      EasyFail("EOF in extention\n", BAD);
*/

    if (fread(buf, (int) c, 1, fd) != (int) c)
      EasyFail("EOF in extention\n", BAD);

  }
}

static GetCode(fd, code_size, flag)
  FILE           *fd;
  int             code_size;
  int             flag;
{
  static unsigned char buf[280];
  static int      curbit, lastbit, done, last_byte;
  int             i, j, ret;
  unsigned char   count;

  if (flag)
  {
    curbit = 0;
    lastbit = 0;
    last_byte = 0;  /* Line added by A.Werth 8/16/89 */
    done = FALSE;
    return 0;
  }

  if ((curbit + code_size) >= lastbit)
  { if (done)
    { if (curbit >= lastbit)
        EasyFail("Ran off the end of my bits\n", -1);
    }
    buf[0] = buf[last_byte - 2];
    buf[1] = buf[last_byte - 1];

    if (!ReadOK (fd, &count, 1))
    { EasyFail("Error in getting buffer size\n", -1); }

    if (count == 0)
    { done = TRUE; } 

    else if (!ReadOK (fd, &buf[2], count))
      EasyFail("Error in getting buffer\n", -1);

    last_byte = 2 + count;
    curbit = (curbit - lastbit) + 16;
    lastbit = (2 + count) * 8;
  }

  ret = 0;

  for (i = curbit, j = 0; j < code_size; i++, j++)
    ret |= ((buf[i / 8] & (1 << (i % 8))) != 0) << j;

  curbit += code_size;

  return ret;
}

static LWZReadByte(fd, flag, input_code_size)
  FILE           *fd;
  int             flag;
  int             input_code_size;
{
  static int      fresh = FALSE;
  int             code, incode;
  static int      code_size, set_code_size;
  static int      max_code, max_code_size;
  static int      firstcode, oldcode;
  static int      clear_code, end_code;
  static int      table[2][(1 << MAX_LWZ_BITS)];
  static int      stack[(1 << (MAX_LWZ_BITS)) * 2], *sp;
  register int    i;

  if (flag)
  {
    set_code_size = input_code_size;
    code_size = set_code_size + 1;
    clear_code = 1 << set_code_size;
    end_code = clear_code + 1;
    max_code_size = 2 * clear_code;
    max_code = clear_code + 2;

    GetCode(fd, RG_NULL, RG_NULL);

    fresh = TRUE;

    for (i = 0; i < clear_code; i++)
    {
      table[0][i] = 0;
      table[1][i] = i;
    }

    for (; i < (1 << MAX_LWZ_BITS); i++)
      table[0][i] = table[1][0] = 0;

    sp = stack;
    return 0;
  }
  else if (fresh)
  { fresh = FALSE;
    do
    { firstcode = oldcode =
        GetCode(fd, code_size, FALSE);
    } while (firstcode == clear_code);

    return firstcode;
  }

  if (sp > stack)
    return *--sp;

  while ((code = GetCode(fd, code_size, FALSE)) >= 0)
  { if (code == clear_code)
    { for (i = 0; i < clear_code; i++)
      {table[0][i] = 0;
        table[1][i] = i;
      }

      for (; i < (1 << MAX_LWZ_BITS); i++)
        table[0][i] = table[1][i] = 0;

      code_size = set_code_size + 1;
      max_code_size = 2 * clear_code;
      max_code = clear_code + 2;
      sp = stack;
      firstcode = oldcode =
        GetCode(fd, code_size, FALSE);
      return firstcode;
    }
    else if (code == end_code)
    { unsigned char   count;
      unsigned char   junk;

      while (ReadOK (fd, &count, 1) && (count != 0))
        while (count-- != 0 && ReadOK (fd, &junk, 1));

      if (count != 0)
        EasyFail("missing EOD in data stream (common occurance)\n", -3);

      return -2;
    }

    incode = code;

    if (code >= max_code)
    { *sp++ = firstcode;
      code = oldcode;
    }

    while (code >= clear_code)
    {
      *sp++ = table[1][code];
      if (code == table[0][code])
        EasyFail("Circular table entry BIG ERROR\n", -1);
      code = table[0][code];
    }

    *sp++ = firstcode = table[1][code];

    if ((code = max_code) < (1 << MAX_LWZ_BITS))
    {
      table[0][code] = oldcode;
      table[1][code] = firstcode;
      max_code++;

      if ((max_code >= max_code_size) &&
          (max_code_size < (1 << MAX_LWZ_BITS)))
      {
        max_code_size *= 2;
        code_size++;
      }
    }
    oldcode = incode;

    if (sp > stack)
      return *--sp;
  }

  return code;
}

static ReadInterlaced(fd, len, height, buffer, rowlen)
  FILE           *fd;
  int             len, height;
  unsigned char	 *buffer;
  int		  rowlen;
{
  unsigned char   c;
  unsigned char *bmp;
  register int    v;
  register int    xpos = 0;
  int    ypos = 0, pass = 0;
  register int    maxypos = 0;


  if (!ReadOK(fd, &c, 1))
    EasyFail("Bogus image data -- EOF\n", BAD);
  if (LWZReadByte(fd, TRUE, c) < 0)
    return BAD;

  while ((v = LWZReadByte(fd, FALSE, c)) >= 0)
  {
    if (xpos == 0)
    { bmp = &(buffer[ypos * rowlen]);
      if (ypos > height)
      {
	return(OK);
	/* "Wanring Too much data, started to read line %d\n" */
      }
    }
    
    *bmp++ = v;

    if (++xpos == len)
    {
      xpos = 0;
      switch (pass)
      {
       case 0: case 1:		ypos += 8; break;
       case 2:			ypos += 4; break;
       case 3:			ypos += 2; break;
      }
      
      if (ypos > maxypos)	maxypos = ypos;

      if (ypos >= height)
      {
	switch (++pass)
	{
	 case 1:		ypos = 4; break;
	 case 2:		ypos = 2; break;
	 case 3:		ypos = 1; break;
	}
      }
    }
  }

  if (maxypos >= height) return OK;

  if (v == (-2))
    return OK;
  return BAD;
}

static FReadRaster (fd, len, height, buffer, rowlen)
  FILE           *fd;
  int             len, height;
  unsigned char  *buffer;
  int             rowlen;
{
  unsigned char   c;
  unsigned char  *bmp;
  register int    v;
  register int    xpos = 0;
  register int    ypos = 0;

  if (!ReadOK (fd, &c, 1))
    EasyFail("Bogus image data -- EOF\n", TRUE);

  if (LWZReadByte(fd, TRUE, c) < 0)
    return BAD;

  /* Read the raster data and dump it into the FBM bitmap */
  while ((v = LWZReadByte(fd, FALSE, c)) >= 0)
  {
    if (xpos == 0)
    { bmp = &(buffer[ypos++ * rowlen]);
      if (ypos > height)
      { 
	/* "Warning: too much data, started to read line %d\n" */
        return (OK);
      }
    }
    
    *bmp++ = v;
    
    if (++xpos == len)

    if (xpos == len) xpos = 0;
  }

  if (ypos >= height) return OK;

  if (v == (-2))
    return OK;
  return BAD;
}

INT_type FLGIFR_strcmp( first, second )
    IN char *first;
    IN char *second;
{

    /*
	Whimppy version of strcmp for this source file.
	Return (INT_type) 0 if first and second are
	    equal; otherwise returns (INT_type) -1.
    */


    char *fp;
    char *sp;

    for( fp = first, sp = second;
	 *fp != '\0' && *sp != '\0' && *fp == *sp ;
	 fp++, sp++ ) ;
    /* N.B. relys on order semantics of && */
    

    if( *fp == '\0' && *sp == '\0' ){
	return( (INT_type) 0 );
    }
    else {
	return( (INT_type) -1 );
    }

}
