/*
 * Copyright 1988 Massachusetts Institute of Technology
 * Based on an optimized version provided by Jim Becker, Auguest 5, 1988
 *     XmuReadBitmapDataFromFile    read X10 or X11 format bitmap files
 *                  and return data
 */
/**********************************************************************/
/*  file:   $Source: /mit/phils/X/Lib/RCS/ximRdBit.c,v $      */
/*  adapted: Philip R. Thompson, ($Author: phils $)                  */
/*          $Date: 89/11/02 16:08:48 $      $State: Exp $           */
/*  Copyright (c) 1989, Massachusetts Institute of Technology         */
/*                Philip R. Thompson   (phils@athena.mit.edu)         */
/*                Computer Resource Laboratory (CRL)                  */
/*                Dept. of Architecture and Planning                  */
/*                M.I.T., Rm 9-526                                    */
/*                Cambridge, MA  02139                                */
/**********************************************************************/
#if (!defined(lint) && !defined(SABER))
static char ximRdBit_rcsid[] =
        "$Header: /mit/phils/X/Lib/RCS/ximRdBit.c,v 1.2 89/11/02 16:08:48 phils Exp Locker: phils $";
#endif lint

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <ctype.h>
#include "XimHeader.h"

#define MAX_SIZE 255
#define RETURN(code) { if (data) free((char *)data); return code; }

/* shared data for the image read/parse logic */
static short hexTable[256];        /* conversion value */
static Bool initialized = False;   /* easier to fill in at run time */
extern char *malloc();

/*  Table index for the hex values. Initialized once, first time.
 *  Used for translation value or delimiter significance lookup.
 */
static void initHexTable()
{
    /* We build the table at run time for several reasons: 1.
     * portable to non-ASCII machines. 2. still reentrant since we
     * set the init flag after setting table. 3. easier to extend.
     * 4. less prone to bugs. */
    hexTable['0'] = 0; hexTable['1'] = 1;
    hexTable['2'] = 2; hexTable['3'] = 3;
    hexTable['4'] = 4; hexTable['5'] = 5;
    hexTable['6'] = 6; hexTable['7'] = 7;
    hexTable['8'] = 8; hexTable['9'] = 9;
    hexTable['A'] = 10; hexTable['B'] = 11;
    hexTable['C'] = 12; hexTable['D'] = 13;
    hexTable['E'] = 14; hexTable['F'] = 15;
    hexTable['a'] = 10; hexTable['b'] = 11;
    hexTable['c'] = 12; hexTable['d'] = 13;
    hexTable['e'] = 14; hexTable['f'] = 15;

    /* delimiters of significance are flagged w/ negative value */
    hexTable[' '] = -1; hexTable[','] = -1;
    hexTable['}'] = -1; hexTable['\n'] = -1;
    hexTable['\t'] = -1;
    initialized = True;
}

/*  read next hex value in the input stream, return -1 if EOF
*/
static NextInt(fp)
FILE *fp;
{
    int ch, value = 0;
    int gotone = 0, done = 0;

    /* loop, accumulate hex value until find delimiter  */
    /* skip any initial delimiters found in read stream */

    while (!done) {
        ch = getc(fp);
        if (ch == EOF) {
            value = -1;
            done++;
        } else {
            /* trim high bits, check type and accumulate */
            ch &= 0xff;
            if (isascii(ch) && isxdigit(ch)) {
                value = (value << 4) + hexTable[ch];
                gotone++;
            } else if ((hexTable[ch]) < 0 && gotone)
                done++;
        }
    }
    return value;
}


/* The data returned by the following routine is always in left-most
 * byte first and left-most bit first.  If it doesn't return
 * BitmapSuccess then its arguments won't have been touched.  This
 * routine should look as much like the Xlib routine XReadBitmapfile as
 * possible.
 */
int XimReadBitmapFile(fp, xim)
XimImage *xim;
FILE  *fp;
{
    byte *data = NULL;          /* working variable */
    char line[MAX_SIZE];        /* input line from file */
    char name_and_type[MAX_SIZE];  /* an input line */
    char *type;                 /* for parsing */
    int value;                  /* from an input line */
    int version10p;             /* boolean, old format */
    int padding;                /* to handle alignment */
    int ww = 0, hh = 0;        /* height */

    /* first time initialization */
    if (initialized == False)
        initHexTable();

    /* error cleanup and return macro    */

    while (fgets(line, MAX_SIZE, fp)) {
        if (strlen(line) == MAX_SIZE - 1)
            RETURN(BitmapFileInvalid);
        if (sscanf(line, "#define %s %d", name_and_type, &value) == 2) {
            if (!(type = rindex(name_and_type, '_')))
                type = name_and_type;
            else
                type++;

            if (!strcmp("width", type))
                ww = value;
            if (!strcmp("height", type))
                hh = (unsigned int)value;
            if (!strcmp("hot", type)) {
                if (type-- == name_and_type || type-- == name_and_type)
                    continue;
            }
            continue;
        }
        if (sscanf(line, "static short %s = {", name_and_type) == 1)
            version10p = 1;
        else if (sscanf(line, "static unsigned char %s = {", name_and_type) == 1)
            version10p = 0;
        else if (sscanf(line, "static char %s = {", name_and_type) == 1)
            version10p = 0;
        else
            continue;

        if (!(type = rindex(name_and_type, '_')))
            type = name_and_type;
        else
            type++;
        if (strcmp("bits[]", type))
            continue;
        if (!ww || !hh)
            RETURN(0);

        if ((ww % 16) && ((ww % 16) < 9) && version10p)
            padding = 1;
        else
            padding = 0;

        xim->bytes_per_line = (ww + 7) / 8 + padding;

        xim->datasize = xim->bytes_per_line * hh;
        if (!(data = (byte *)malloc(xim->datasize)))
            RETURN(0);

        if (version10p) {
            byte *ptr;
            int bytes;

            for (bytes=0, ptr=data; bytes < xim->datasize; (bytes+=2)) {
                if ((value = NextInt(fp)) < 0)
                    RETURN(0);
                *(ptr++) = value;
                if (!padding || ((bytes + 2) % xim->bytes_per_line))
                    *(ptr++) = value >> 8;
            }
        } else {
            byte *ptr;
            int bytes;

            for (bytes = 0, ptr = data; bytes < xim->datasize; bytes++,
                    ptr++) {
                if ((value = NextInt(fp)) < 0)
                    RETURN(0);
                *ptr = (byte)value;
            }
        }
    }   /* end while */
    xim->data = data;
    xim->width = ww;
    xim->height = hh;
    xim->nchannels = 1;
    xim->bits_channel = 1;
    xim->ncolors = 0;
    xim->colors = (Color *)0;
    xim->npics = 1;
    return(1);
}

#define BYTES_PER_OUTPUT_LINE  12

XimWriteBitmapFile(stream, xim)
FILE *stream;
XimImage *xim;
{
    register int i;
    register byte *ptr;
    char *name = "xim";

    if (xim->nchannels != 1 || xim->bits_channel != 1) {
        XimWarn("XimWriteBitmapFile: image not in bitmap form");
        return(0);
    }
    /* Write out standard header */
    fprintf(stream, "#define %s_width %d\n", name, xim->width);
    fprintf(stream, "#define %s_height %d\n", name, xim->height);
    fprintf(stream, "#define %s_x_hot %d\n", name, -1);
    fprintf(stream, "#define %s_y_hot %d\n", name, -1);
    
    /* Print out the data itself */
    ptr = xim->data;
    fprintf(stream, "static char %s_bits[] = {", name);
    fprintf(stream, "\n   ");
    fprintf(stream, "0x%02x", *ptr);
    ptr++;
    for (i=1; i < xim->datasize; i++, ptr++){
        if (i % BYTES_PER_OUTPUT_LINE)
            fprintf(stream, ", ");
        else
            fprintf(stream, ",\n   ");
        fprintf(stream, "0x%02x", *ptr);
    }
    fprintf(stream, "};\n");
    return(1);
}

/*** end ximRdBit.c ***/
