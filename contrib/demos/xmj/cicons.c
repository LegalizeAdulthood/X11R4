/*
 * $XConsortium: cicons.c,v 1.3 89/05/05 12:18:42 jim Exp $
 *
 * Original source Copyright 1988 Exceptions
 * X11 portions Copyright 1989 Concurrent Computer Corporation
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Exceptions and Concurrent not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Exceptions and Concurrent make
 * no representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 * 
 * EXCEPTIONS AND CONCURRENT DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL EXCEPTIONS OR CONCURRENT BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Original Author:  Mark Holm, Exceptions, nosun!tessi!exc!markh@sun.com
 * 
 * Acknowledgments to Dorothy Robinson for her artistic abilities in drawing
 * the icons and to Jim Batch for technical support and graphical concepts 
 * Jim Batch for techical support and graphical concepts.
 * 
 * X11 port by:  Jay Estabrook, Concurrent Computer Corporation
 */

extern char cDOT1_bits[], cDOT2_bits[], cDOT3_bits[], cDOT4_bits[], cDOT5_bits[];
extern char cDOT6_bits[], cDOT7_bits[], cDOT8_bits[], cDOT9_bits[];
extern char cBAM1_bits[], cBAM2_bits[], cBAM3_bits[], cBAM4_bits[], cBAM5_bits[];
extern char cBAM6_bits[], cBAM7_bits[], cBAM8_bits[], cBAM9_bits[];
extern char cCHA1_bits[], cCHA2_bits[], cCHA3_bits[], cCHA4_bits[], cCHA5_bits[];
extern char cCHA6_bits[], cCHA7_bits[], cCHA8_bits[], cCHA9_bits[];
extern char cGRED_bits[], cREDD_bits[], cWHTD_bits[];
extern char cEAST_bits[], cWEST_bits[], cSOUT_bits[], cNORT_bits[];
extern char cAUT_bits[], cSUM_bits[], cSPR_bits[], cWIN_bits[];
extern char cORC_bits[], cMUM_bits[], cBAM_bits[], cPLM_bits[];

char *Color_tile_bitmaps[] =
{
	cDOT1_bits, cDOT2_bits, cDOT3_bits, cDOT4_bits, cDOT5_bits,
	cDOT6_bits, cDOT7_bits, cDOT8_bits, cDOT9_bits,
	cBAM1_bits, cBAM2_bits, cBAM3_bits, cBAM4_bits, cBAM5_bits,
	cBAM6_bits, cBAM7_bits, cBAM8_bits, cBAM9_bits,
	cCHA1_bits, cCHA2_bits, cCHA3_bits, cCHA4_bits, cCHA5_bits,
	cCHA6_bits, cCHA7_bits, cCHA8_bits, cCHA9_bits,
	cGRED_bits, cREDD_bits, cWHTD_bits,
	cEAST_bits, cWEST_bits, cSOUT_bits, cNORT_bits,
	cAUT_bits, cSUM_bits, cSPR_bits, cWIN_bits,
	cORC_bits, cMUM_bits, cBAM_bits, cPLM_bits
};
