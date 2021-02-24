/*
 *		Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 */

#if !defined(IO_H)
#define IO_H

#include <xgks.h>

#include <stdio.h>

#include "changes.h"

#define	VERSION_NUM	"version 1.1"

char *get_char_str( FILE *fd, IDX errno);
char *read_char_str( FILE *fd, BOOLEAN *error);
void get_int( FILE *fd, int *i, IDX errno);
void get_real3( FILE *fd, Gfloat *r1,Gfloat *r2,Gfloat *r3, IDX errno);
void get_real( FILE *fd, Gfloat *r, IDX errno);
void read_char( FILE *fd, char *ch, BOOLEAN *error);
void read_int( FILE *fd, int *i, BOOLEAN *error);
void read_real2( FILE *fd, Gfloat *r1,Gfloat *r2, BOOLEAN *error);
void read_real3( FILE *fd, Gfloat *r1,Gfloat *r2,Gfloat *r3, BOOLEAN *error);
void read_real( FILE *fd, Gfloat *r, BOOLEAN *error);
void write_char( FILE *fd, char ch);
void write_char_str( FILE *fd, char *str);
void write_int( FILE *fd, int i);
void write_real2( FILE *fd, Gfloat r1,Gfloat r2);
void write_real3( FILE *fd, Gfloat r1, Gfloat r2, Gfloat r3);
void write_real( FILE *fd, Gfloat r);

#endif
