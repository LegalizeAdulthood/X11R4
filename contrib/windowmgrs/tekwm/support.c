#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: support.c,v 5.4 89/11/22 15:14:56 glennw Exp $";
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

#include "support.h"
#include <stdio.h>
     
     int _rtn_level;
int _rtn_trace;

static struct func_stack {
     char *rtn_name;
     struct func_stack *next;
} *Head;

char *curr_rtn()
{
     if (!Head)
	  return((char *)0);
     else
	  return(Head->rtn_name);
}

void push_rtn(s)
register char *s;
{
     if (!Head) {
	  Head = (struct func_stack *)malloc(sizeof(struct func_stack));
	  if (!Head) {
	       fprintf(stderr, "Couldn't malloc new func_stack entry!\n");
	       exit(1);
	  }
	  Head->rtn_name = s;
	  Head->next = 0;
     }
     else {
	  struct func_stack *ptr;
	  
	  ptr =  (struct func_stack *)malloc(sizeof(struct func_stack));
	  if (!ptr) {
	       fprintf(stderr, "Couldn't malloc new func_stack entry!\n");
	       exit(1);
	  }
	  ptr->rtn_name = s;
	  ptr->next = Head;
	  Head = ptr;
     }
     _rtn_level++;
     if (_rtn_trace) {
	  int i;
	  
	  for (i = 0; i < _rtn_level; i++)
	       putchar('\t');
	  printf("%s(%d)\n", Head->rtn_name, _rtn_level);
     }
}

void pop_rtn()
{
     struct func_stack *ptr;

     if (!Head)
	  return;
     ptr = Head;
     Head = Head->next;
     free(ptr);
     _rtn_level--;
}
