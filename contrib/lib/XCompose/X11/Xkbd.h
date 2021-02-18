/* $Header: Xkbd.h,v 1.2 89/11/13 09:02:23 glennw Exp $ */
/* $XConsortium: Xkbd.h,v 11.43 88/09/06 16:07:14 jim Exp $ */

/*
 * Copyright 1989 by the Massachusetts Institute of Technology, 
 * Cambridge, Massachusetts, and Tektronix, Inc. Beaverton, Oregon.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Tektronix or M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Tektronix and M.I.T. make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * TEKTRONIX AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL TEKTRONIX OR M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 */

#ifndef _XKBD_H_
#define _XKBD_H_

typedef struct {
    unsigned short  num_encodings;  /* number of encoding names per modifier */
    unsigned short  *encoding_ids;  /* 2-dimensional table of encoding ids */
    char	    **names;	    /* 32 modifiers * num_encodings names */
    int		    len_names;	    /* total # chars in names */
} XKBEncodings;
Status XGetKBEncodings();
Status XGetKBIndex();

typedef struct {
    unsigned short  *string_offsets; /* offsets of strings in string table */
    unsigned short  *string_lengths; /* lengths of strings in string table */
    char	    *strings;	      /* string table - not NULL-terminated! */
    unsigned short  num_strings;      /* size in bytes of string table */
} XKBStrings;
Status XGetKBStrings();

typedef struct {
    unsigned short  num_composes;   /* number of compose sequences */
    unsigned short  max_keycodes;   /* max keycodes per compose sequence */
    KeyCode	    numeric_keycodes[48]; /* numeric compose keycodes */
    unsigned short  *modifiers;	    /* list of sequence modifiers */
    KeyCode	    *sequences;	 /* 2-dimensional table of compose sequences */
    unsigned int    *output_modifiers;	    /* list of output modifier masks */
    KeyCode	    *output_keycodes;	    /* list of output keycodes */
    KeyCode	    *cancel_keycodes;	    /* list of cancel keycodes */
    int		    num_cancel_keycodes;    /* count */
    KeyCode	    *abort_keycodes;	    /* list of abort keycodes */
    int		    num_abort_keycodes;    /* count */
} XKBCompose;
Status XGetKBCompose();

/* fake keycode values to define numeric compose */
#define BINARY_COMPOSE 1
#define OCTAL_COMPOSE 2
#define DECIMAL_COMPOSE 3
#define HEX_COMPOSE 4

#endif	/* _XKBD_H_ */
