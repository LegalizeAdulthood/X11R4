#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: XSetKbd.c,v 1.2 89/11/13 08:20:39 glennw Exp $";
#endif RCS_ID
#endif LINT

/* $XConsortium:$ */

/*
 * Copyright 1989 by Tektronix, Inc. Beaverton, Oregon,
 * and the Massachusetts Institute of Technology, Cambridge, Massachusetts.
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

#include <stdio.h>
#include "Xlib.h"   /* includes Xkbd.h */
#include "Xlibint.h"
#include <X11/Xos.h>

static Atom	Kb_encoding_table;
static Atom	Kb_encoding_names;
static Atom	Kb_modifier_index;
static Atom	Kb_string_table;
static Atom	Kb_string_length;
static Atom	Kb_string_offset;
static Atom	Kb_compose_table;
static Atom	Kb_compose_table_shorts;
static Bool	uninitialized = True;

InitKB(dpy)
    Display *dpy;
{
    /* Warning!!! This prototype works for only a single display! */
    /* Need to put atoms on the dpy struct */
    if (uninitialized) {
        Kb_encoding_table = XInternAtom(dpy, "KB_ENCODING_TABLE", False);
        Kb_encoding_names = XInternAtom(dpy, "KB_ENCODING_NAMES", False);
        Kb_modifier_index = XInternAtom(dpy, "KB_MODIFIER_INDEX", False);
        Kb_string_table = XInternAtom(dpy, "KB_STRING_TABLE", False);
        Kb_string_length = XInternAtom(dpy, "KB_STRING_LENGTH", False);
        Kb_string_offset = XInternAtom(dpy, "KB_STRING_OFFSET", False);
	Kb_compose_table = XInternAtom(dpy, "KB_COMPOSE_TABLE", False);
	Kb_compose_table_shorts = XInternAtom(dpy, "KB_COMPOSE_TABLE_SHORTS", 
					      False);
	uninitialized = False;
    }
}

/* 
 * XSetKBEncodings sets the properties
 *	KB_ENCODING_TABLE 	type: KB_ENCODING_TABLE
 *	KB_ENCODING_NAMES 	type: KB_ENCODING_NAMES
 */

XSetKBEncodings(dpy, w, encodings, num_indices)
    Display *dpy;
    Window w;
    XKBEncodings *encodings;
    unsigned char num_indices;	    /* number of modifier indices defined */
{
    CARD8	*table = NULL;
    int		i;
    int		table_size;

    if (uninitialized)
	InitKB(dpy);

    table_size = ((dpy->max_keycode - dpy->min_keycode + 1) * num_indices) + 1;
        /* keycodes per each of modifier indices, plus data[0] */
    table = (CARD8 *) Xmalloc(table_size);
    table[0] = encodings->num_encodings;
    for (i = 1; i < table_size; i++)
	table[i] = encodings->encoding_ids[i];
	/* no overrange check here... */
    XChangeProperty(dpy, w, Kb_encoding_table, Kb_encoding_table, 8, 
		    PropModeReplace, table, table_size);
    XChangeProperty(dpy, w, Kb_encoding_names, Kb_encoding_names, 8, 
		    PropModeReplace, (unsigned char *) encodings->names[0],
		    encodings->len_names);
	/* Caution - assumes that the names are all contiguous, starting with
	   names[0] */
	/* could allow short props, but why bother? */
    Xfree((char *)table);
}

/* 
 * XSetKBStrings sets the properties
 *	KB_STRING_LENGTH 	type: KB_STRING_LENGTH
 *	KB_STRING_OFFSET 	type: KB_STRING_OFFSET
 *	KB_STRING_TABLE 	type: KB_STRING_TABLE
 */

XSetKBStrings (dpy, w, strings, num_indices)
    Display *dpy;
    Window w;
    XKBStrings *strings; 
    unsigned char num_indices;	    /* number of modifier indices defined */
{
    int		prop_size = ((dpy->max_keycode - dpy->min_keycode + 1) << 1)
			     * num_indices;

    if (uninitialized)
	InitKB(dpy);

    XChangeProperty (dpy, w, Kb_string_length, Kb_string_length, 16,
		     PropModeReplace, strings->string_offsets, prop_size);
    /* Warning: we assume short = 16! */
    XChangeProperty (dpy, w, Kb_string_offset, Kb_string_offset, 16,
		     PropModeReplace, strings->string_lengths, prop_size);
    XChangeProperty (dpy, w, Kb_string_table, Kb_string_table, 8,
		     PropModeReplace, (unsigned char *) strings->strings, 
		     strings->num_strings);
}

/* 
 * XSetKBIndex sets the property
 *	KB_MODIFIER_INDEX 	type: KB_MODIFIER_INDEX
 */

XSetKBIndex (dpy, w, indices)
    Display *dpy;
    Window w;
    unsigned short *indices;
{
    CARD8	local_indices[256];
    int		i;

    if (uninitialized)
	InitKB(dpy);

    for (i = 0; i < 256; i++)
	local_indices[i] = indices[i];
	/* no overrange check here... */
	/* could allow short props, but why bother? */
    XChangeProperty (dpy, w, Kb_modifier_index, Kb_modifier_index, 8,
		     PropModeReplace, (unsigned char *) local_indices, 
		     256);
}

Status XGetKBEncodings (dpy, w, encodings)
	Display *dpy;
	Window w;
	XKBEncodings *encodings;
{
    CARD8	*table = NULL;
    char	*names = NULL;
    Atom	actual_type;
    int		actual_format;
    unsigned long leftover;
    unsigned long table_size;
    unsigned long name_size;
    int		i;
    int		num_names;
    char	*string_ptr;

    if (uninitialized)
	InitKB(dpy);
    if (XGetWindowProperty(dpy, w, Kb_encoding_table, 0L,
		    (long) dpy->max_request_size,
		    False, Kb_encoding_table, &actual_type, &actual_format,
		    &table_size, &leftover, (unsigned char **)&table)
	!= Success) return (0);
    if ((actual_type != Kb_encoding_table) ||
	(actual_format != 8) || leftover) {
	if (leftover)
	    fprintf(stderr, "XGetKBEncodings: Warning; prototype limit of max_request_size for encoding table exceeded\n");
	if (table != NULL) Xfree ((char *)table);
	return(0);
    }
    if (XGetWindowProperty(dpy, w, Kb_encoding_names, 0L,
		    (long) dpy->max_request_size,
		    False, Kb_encoding_names, &actual_type, &actual_format,
		    &name_size, &leftover, (unsigned char **)&names)
	!= Success) return (0);
    if ((actual_type != Kb_encoding_names) ||
	(actual_format != 8) || leftover) {
	if (leftover)
	    fprintf(stderr, "XGetKBEncodings: Warning; prototype limit of max_request_size for encoding names exceeded\n");
	if (table != NULL) Xfree ((char *)table);
	if (names != NULL) Xfree ((char *)names);
	return(0);
    }
    num_names = table[0] << 5;	    /* times 32 */
    encodings->num_encodings = table[0];
    encodings->len_names = name_size;
    encodings->encoding_ids = (unsigned short *) 
			Xmalloc((table_size - 1) * sizeof(unsigned short *));
    encodings->names = (char **) Xmalloc(num_names * sizeof(char *));
    for (i = 1; i < table_size; i++)
	encodings->encoding_ids[i-1] = table[i];
    string_ptr = names;
    for (i = 0; i < num_names; i++) {
	encodings->names[i] = string_ptr;
	string_ptr = index(names, NULL) + 1;
    }
    Xfree((char *)table);
    return(1);
}

XFreeKBEncodings (encodings)
    XKBEncodings *encodings;
{
    XFree(encodings->names[0]);	/* all name strings malloced at once */
    XFree(encodings->names);
    XFree(encodings->encoding_ids);
    XFree(encodings);	    /* client must have Xmalloc'ed struct */
}

Status
XGetKBStrings (dpy, w, strings)
    Display *dpy;
    Window w;
    XKBStrings *strings;
{
    CARD16	*lengths = NULL;
    CARD16	*offsets = NULL;
    char	*table = NULL;
    Atom	actual_type;
    int		actual_format;
    unsigned long leftover;
    unsigned long nitems;
	
    if (uninitialized)
	InitKB(dpy);
    if (XGetWindowProperty(dpy, w, Kb_string_length, 0L, 
	    (long) dpy->max_request_size,
	    False, Kb_string_length, &actual_type, &actual_format,
            &nitems, &leftover, (unsigned char **)&lengths)
	!= Success) return (0);
    if ((actual_type != Kb_string_table) ||
	(actual_format != 16) || leftover) {
	if (leftover)
	    fprintf(stderr, "XGetKBStrings: Warning; prototype limit of max_request_size for string table exceeded\n");
	/* we blindly assume that the tables are full size, i.e. that
	   MAX(KB_MODIFIER_INDEX[0:255]) * table_size is in the table */
	if (lengths != NULL) Xfree ((char *)lengths);
	return(0);
    }
    if ((XGetWindowProperty(dpy, w, Kb_string_offset, 0L, 
	    (long) dpy->max_request_size,
	    False, Kb_string_offset, &actual_type, &actual_format,
            &nitems, &leftover, (unsigned char **)&offsets)
	!= Success) || (actual_type != Kb_string_table) ||
		       (actual_format != 16) || leftover) {
	if (leftover)
	    fprintf(stderr, "XGetKBStrings: Warning; prototype limit of max_request_size for string table exceeded\n");
	if (lengths != NULL) Xfree ((char *)lengths);
	if (offsets != NULL) Xfree ((char *)offsets);
	return(0);
    }
    if ((XGetWindowProperty(dpy, w, Kb_string_table, 0L, 
	    (long) dpy->max_request_size,
	    False, Kb_string_table, &actual_type, &actual_format,
            &nitems, &leftover, (unsigned char **)&table)
	!= Success) || (actual_type != Kb_string_table) ||
		       (actual_format != 8) || leftover) {
	if (leftover)
	    fprintf(stderr, "XGetKBStrings: Warning; prototype limit of max_request_size for string table exceeded\n");
	if (lengths != NULL) Xfree ((char *)lengths);
	if (offsets != NULL) Xfree ((char *)offsets);
	if (table != NULL) Xfree ((char *)table);
	return(0);
    }
    strings->string_lengths = lengths;
    strings->string_offsets = offsets;
    strings->strings = table;
    return(1);
}

Status
XGetKBIndex (dpy, w, indices)
    Display *dpy;
    Window w;
    unsigned short *indices;
{
    CARD8	*local_indices = NULL;
    Atom	actual_type;
    int		actual_format;
    unsigned long leftover;
    unsigned long num_indices;
    int		i;

    if (uninitialized)
	InitKB(dpy);
    if (XGetWindowProperty(dpy, w, Kb_modifier_index, 0L,
		    (long) 256,
		    False, Kb_modifier_index, &actual_type, &actual_format,
		    &num_indices, &leftover, (unsigned char **)&local_indices)
	!= Success) return (0);
    if ((actual_type != Kb_modifier_index) ||
        (actual_format != 8) || num_indices != 256 || leftover) {
	fprintf(stderr, "XGetKBStrings: Warning; got bad KB_MODIFIER_INDEX property\n");
	if (local_indices != NULL) Xfree ((char *)local_indices);
	return(0);
    }
    for (i = 0; i < 256; i++)
	indices[i] = local_indices[i];
    Xfree ((char *)local_indices);
    return(1);
}


/* 
 * XSetKBCompose sets the property
 *	KB_COMPOSE_TABLE 	type: KB_COMPOSE_TABLE
 */

XSetKBCompose(dpy, w, compose)
    Display *dpy;
    Window w;
    XKBCompose *compose;
{
    CARD8	*table = NULL;
    CARD16	*shorts_table = NULL;
    int		table_size, arraysize;
    int		i;

    if (uninitialized)
	InitKB(dpy);

    table_size = 50 + compose->num_composes * (compose->max_keycodes + 2) +
		 compose->num_cancel_keycodes + compose->num_abort_keycodes;
    /* max keycodes plus modifier and keycode per compose sequence, plus
       data[0], data[1-48], cancel keycode count, cancel and abort keycodes */
    /* Warning: we assume KeyCode=CARD8=8bits (probably a safe assumption) */
    table = (CARD8 *) Xmalloc(table_size);
    arraysize = compose->num_composes * compose->max_keycodes;
    shorts_table = (CARD16 *) Xmalloc(sizeof(CARD16) * (1 + arraysize));
    shorts_table[0] = compose->num_composes;
    bcopy(compose->modifiers, shorts_table + 1, sizeof(unsigned short) * 
	  arraysize);

    /* Warning -we assume unsigned short = CARD16 */

    table[0] = compose->max_keycodes;
    bcopy(compose->numeric_keycodes, table + 1, 48);
    bcopy(compose->sequences, table + 49, 
	  compose->num_composes * compose->max_keycodes);
    for (i = 0; i < compose->num_composes; i++)
	table[49 + arraysize + i] = compose->output_modifiers[i];
	/* convert from unsigned int to byte */
    bcopy(compose->output_keycodes, table + 49 + (compose->num_composes *
	  (compose->max_keycodes + 1)), compose->num_composes);
    table[49 + (compose->num_composes * (compose->max_keycodes + 2))] =
	compose->num_cancel_keycodes;
    bcopy(compose->cancel_keycodes, table + 50 + (compose->num_composes *
	  (compose->max_keycodes + 2)), compose->num_cancel_keycodes);
    bcopy(compose->abort_keycodes, table + 50 + (compose->num_composes *
	  (compose->max_keycodes + 2)) + compose->num_cancel_keycodes,
	  compose->num_abort_keycodes);
    /* Be sure shorts prop is changed first so server doesn't get incomplete
       data */
    XChangeProperty(dpy, w, Kb_compose_table_shorts, Kb_compose_table_shorts, 
		    16, 
		    PropModeReplace, shorts_table, 1 + arraysize);
    XChangeProperty(dpy, w, Kb_compose_table, Kb_compose_table, 8, 
		    PropModeReplace, table, table_size);
    Xfree((char *)table);
    Xfree((char *)shorts_table);
}

Status XGetKBCompose (dpy, w, compose)
	Display *dpy;
	Window w;
	XKBCompose *compose;
{
    CARD8	*table = NULL;
    CARD16	*shorts_table = NULL;
    unsigned long leftover;
    unsigned long table_size;
    unsigned long shorts_table_size;
    Atom	actual_type;
    int		actual_format;
    int		num_cancel_keycodes, num_abort_keycodes, i;

    if (uninitialized)
	InitKB(dpy);

    if (XGetWindowProperty(dpy, w, Kb_compose_table_shorts, 0L,
			   (long) dpy->max_request_size, False, 
			   Kb_compose_table_shorts, &actual_type, 
			   &actual_format, &shorts_table_size, &leftover,
			   (unsigned char **)&shorts_table)
	!= Success) return (0);
    if ((actual_type != Kb_compose_table_shorts) ||
        (actual_format != 16) || leftover) {
	if (leftover)
	    fprintf(stderr, "XGetKBCompose: Warning; prototype limit of max_request_size for compose shorts table exceeded\n");
	if (table != NULL) Xfree ((char *)shorts_table);
	return(0);
    }

    if (XGetWindowProperty(dpy, w, Kb_compose_table, 0L,
		    (long) dpy->max_request_size,
		    False, Kb_compose_table, &actual_type, &actual_format,
		    &table_size, &leftover, (unsigned char **)&table)
	!= Success) return (0);
    if ((actual_type != Kb_compose_table) ||
        (actual_format != 8) || leftover) {
	if (leftover)
	    fprintf(stderr, "XGetKBCompose: Warning; prototype limit of max_request_size for compose table exceeded\n");
	if (table != NULL) Xfree ((char *)table);
	return(0);
    }

    if (table_size < 50 + shorts_table[0] * (table[0] + 2)) {
        fprintf(stderr, "XGetKBCompose: Warning; compose table property is a bogus size: is %d, should be at least %d\n", table_size, 50 + shorts_table[0] * (table[0] + 2));
	Xfree ((char *)table);
	Xfree ((char *)shorts_table);
	return(0);
    }
    num_cancel_keycodes = table[49 + shorts_table[0] * (table[0] + 2)];
    num_abort_keycodes = table_size - (50 + shorts_table[0] * (table[0] + 2)
				       + num_cancel_keycodes);
    if (num_abort_keycodes < 0) {
        fprintf(stderr, "XGetKBCompose: Warning; compose table property is a bogus size: is %d, should be at least %d\n", table_size, 50 + shorts_table[0] * (table[0] + 2) + num_cancel_keycodes);
	Xfree ((char *)table);
	Xfree ((char *)shorts_table);
	return(0);
    }

    if (shorts_table_size != (shorts_table[0] * table[0]) + 1) {
        fprintf(stderr, "XGetKBCompose: Warning; shorts compose table property is a bogus size: is %d, should be %d\n", shorts_table_size, (shorts_table[0] * table[0]) + 1);
	Xfree ((char *)table);
	Xfree ((char *)shorts_table);
	return(0);
    }

    compose->num_composes = shorts_table[0];
    compose->max_keycodes = table[0];

    compose->modifiers = (unsigned short *) Xmalloc(sizeof(unsigned short) *
			 compose->num_composes * compose->max_keycodes);
    bcopy(shorts_table + 1, compose->modifiers, sizeof(unsigned short) * 
	  compose->num_composes * compose->max_keycodes);
    bcopy(table + 1, compose->numeric_keycodes, 48);
    compose->sequences = (KeyCode *) Xmalloc(
			 compose->num_composes * compose->max_keycodes);
    bcopy(table + 49, compose->sequences, 
	  compose->num_composes * compose->max_keycodes);
    /* Warning: we assume KeyCode=CARD8=8bits (probably a safe assumtion) */

    compose->output_modifiers = (unsigned int *) Xmalloc(
				sizeof(unsigned int) * compose->num_composes);
    for (i = 0; i < compose->num_composes; i++) {
	compose->output_modifiers[i] =
	table[49 + (compose->num_composes * compose->max_keycodes) + i];
	/* convert from byte to unsigned int */
    }

    compose->output_keycodes = (KeyCode *) Xmalloc(compose->num_composes);
    bcopy(table + 49 + (compose->num_composes * (compose->max_keycodes + 1)),
	  compose->output_keycodes, compose->num_composes);

    compose->num_cancel_keycodes = num_cancel_keycodes;
    if (num_cancel_keycodes > 0) {
	compose->cancel_keycodes = (KeyCode *) Xmalloc(num_cancel_keycodes);
	bcopy(table + 50 + (compose->num_composes * (compose->max_keycodes + 2)),
	  compose->cancel_keycodes, num_cancel_keycodes);
    }
    else
	compose->cancel_keycodes = (KeyCode *) NULL;
    compose->num_abort_keycodes = num_abort_keycodes;
    if (num_abort_keycodes > 0) {
	compose->abort_keycodes = (KeyCode *) Xmalloc(num_abort_keycodes);
	bcopy(table + 50 + (compose->num_composes * (compose->max_keycodes + 2) + num_cancel_keycodes),
	  compose->abort_keycodes, num_abort_keycodes);
    }
    else
	compose->abort_keycodes = (KeyCode *) NULL;
    Xfree((char *)table);
    return(1);
}

XClearKBCompose (dpy)
	Display *dpy;
{
    int i, n_props;
    Atom *props;

    if (uninitialized)
	InitKB(dpy);
    props = XListProperties(dpy, DefaultRootWindow(dpy), &n_props);
    for (i = 0; i < n_props; i++) {
	if (props[i] == Kb_compose_table)
	    XDeleteProperty(dpy, DefaultRootWindow(dpy), Kb_compose_table);
	if (props[i] == Kb_compose_table_shorts)
	    XDeleteProperty(dpy, DefaultRootWindow(dpy), 
			    Kb_compose_table_shorts);
    }
}

XFreeKBCompose (compose, free_struct)
    XKBCompose *compose;
    Bool free_struct;
{
    XFree(compose->modifiers);
    XFree(compose->sequences);
    XFree(compose->output_modifiers);
    XFree(compose->output_keycodes);
    if (compose->cancel_keycodes)
	XFree(compose->cancel_keycodes);
    if (compose->abort_keycodes)
	XFree(compose->abort_keycodes);
    if (free_struct)
	XFree(compose);	    /* client must have Xmalloc'ed struct */
    else
	compose->modifiers = (unsigned short *)NULL;
	compose->output_modifiers = (unsigned int *)NULL;
	compose->sequences =
	compose->output_keycodes = 
	compose->cancel_keycodes =
	compose->abort_keycodes = (KeyCode *)NULL;
}

