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
extern Widget topPane;

/*
 *	Widgets related to file I/O
 */
extern Widget topFileBox;
extern Widget fileNameText;
extern Widget openButton;
extern Widget closeButton;
extern Widget copyButton;
extern Widget cdButton;
extern Widget quitButton;

extern Widget directoryLabel;
extern Widget fileLabel;

/*
 *	Widgets related to the pages table
 */
extern Widget pageButton;
extern Widget printAllButton;
extern Widget printMarkedButton;
extern Widget printUnmarkedButton;

extern Widget clearMarksButton;
extern Widget markMenuViewPort;
extern Widget markMenuList;

/*
 *	Error dialog widgets
 */

extern Widget errorMsgBox;
extern Widget errorText;
extern Widget clearButton;

/*
 *	Things that build widgets
 */

extern void BuildErrorBox();
extern void BuildFileLevel();
extern void BuildPageLevel();
