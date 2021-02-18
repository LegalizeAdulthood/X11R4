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

/* This is everything that's in a TeXPage */

#include "xtex.h"

typedef struct TeXPageStruct
{
  int pageNumber;
  int logicalPageNumber;
  char * pageData;
  int userMag;
  Bool active;
  Bool realized;
#define MAXPAGENUMBERBUFFER 10
  char pageNumberBuffer[MAXPAGENUMBERBUFFER];

  /* action entered numbers */

  Bool inputNumberValid;
  Bool inputNumberMinus;
  int inputNumber;

  /* if you add any widgets, change page.c::TeXPageLocate */

  Widget popUpPage;
  Widget dviPageBox;
  Widget singleDviPage;
  Widget viewPort;
  Widget buttonBox;
  Widget forewardButton;
  Widget backwardButton;
  Widget markButton;
  Widget dupButton;
  struct TeXPageStruct *tied;
  struct TeXPageStruct *tiedTo;
  Widget tieButton;
  Widget largeButton;
  Widget smallButton;
  Widget quitButton;
  Widget pageNumberText;
  Widget gotoButton;
  Widget reopenButton;

  /* these are pulled out of the ViewPort widget */
  Widget vertBar;
  Widget horizBar;
} TeXPage;

extern int TeXPages;
#define MAX_TEX_PAGES 10
extern TeXPage *TheTeXPages [ MAX_TEX_PAGES ];

#ifdef _STDC_
 extern void TeXPageBuild();
 extern void TeXPageCloseFile();
 extern void TeXPageSlamPages();
 extern void TeXPageOpenFile();
 extern TeXPage *TeXPageLocate();
 extern void TeXPageRef(TeXPage *, int);
 extern void TeXPageDeref(TeXPage *);
 extern void TeXPageRealize( TeXPage *);

 extern void TeXPageInstallActions();

#else
 extern void TeXPageBuild();
 extern void TeXPageCloseFile();
 extern void TeXPageSlamPages();
 extern void TeXPageOpenFile();
 extern TeXPage *TeXPageLocate();
 extern void TeXPageRef();
 extern void TeXPageDeref();

 extern void TeXPageInstallActions();
#endif
