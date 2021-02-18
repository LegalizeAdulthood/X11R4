#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: neaten.def.h,v 5.2 88/11/10 14:43:20 glennw Exp $";
#endif RCS_ID
#endif LINT

#include "X11/copyright.h"
/*
 *
 * Copyright 1987, 1988 by Ardent Computer Corporation, Sunnyvale, Ca.
 *
 * Copyright 1987 by Jordan Hubbard.
 *
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Ardent Computer
 * Corporation or Jordan Hubbard not be used in advertising or publicity
 * pertaining to distribution of the software without specific, written
 * prior permission.
 *
 */

#define NEATEN_DEFINE

typedef enum {
    Place_Closest,
    Place_Top,
    Place_Bottom,
    Place_Left,
    Place_Right,
    Place_Center
} Placement;

#define NEATEN_TRUE 1
#define NEATEN_FALSE 0

#define MAX_PRIORITY 100
#define MIN_PRIORITY -100
