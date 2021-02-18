/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 */ 

typedef enum {printAll, printMarked, printUnmarked } PrintDirective;
typedef enum {markNone,markSet, markClear, markToggle} MarkDirective;

extern char *printProgram;

#ifdef _STDC_
  void TeXMarkNewFile();
  void TeXMarkPush();
  void TeXMarkPop();
  Bool TeXMark( MarkDirective, int );
  void TeXMarkAll( MarkDirective );
  void TeXMarkPrint(PrintDirective);
  void TeXMarkPrintUsing(char *, PrintDirective);
#else
  void TeXMarkNewFile();
  void TeXMarkPush();
  void TeXMarkPop();
  Bool TeXMark();
  void TeXMarkAll();
  void TeXMarkPrint();
  void TeXMarkPrintUsing();
#endif
