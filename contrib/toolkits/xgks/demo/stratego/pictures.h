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

#define CASTLEPNTS 21
typedef Gpoint WCPT;
WCPT castle[]= { 0.1, 0.05,  0.1,0.1,  0.15,0.15,  0.15,0.9,  0.2,0.9,
                0.2,0.8,  0.3,0.8,  0.3,0.85,  0.45,0.85,  0.45,0.8,
                0.55,0.8,  0.55,0.85,  0.7,0.85, 0.7,0.8,  0.8,0.8,  0.8,0.9,
                0.85,0.9,  0.85,0.15,  0.9,0.1,  0.9,0.05, 0.1,0.05 };

WCPT pieces[12][15]= { 0.4,0.5,  0.45,0.6,  0.65,0.45,  0.55,0.35,  0.4,0.5,
                0.4,0.45,  0.35,0.4,  0.4,0.35,  0.35,0.3,  0.35,0.2,
                0.55,0.35,  0.55,0.2,  0.35,0.2, 0.25,0.1, 0.55,0.2,

                0.4,0.5,  0.45,0.6,  0.65,0.45,  0.55,0.35,  0.4,0.5,
                0.4,0.45,  0.35,0.4,  0.4,0.35,  0.35,0.3,  0.35,0.2,
                0.55,0.35,  0.55,0.2,  0.35,0.2, 0.25,0.1, 0.55,0.2,

                0.4,0.5,  0.45,0.6,  0.65,0.45,  0.55,0.35,  0.4,0.5,
                0.4,0.45,  0.35,0.4,  0.4,0.35,  0.35,0.3,  0.35,0.2,
                0.55,0.35,  0.55,0.2,  0.35,0.2, 0.25,0.1,  0.55,0.2,

                0.4,0.5,  0.45,0.6,  0.65,0.45,  0.55,0.35,  0.4,0.5,
                0.4,0.45,  0.35,0.4,  0.4,0.35,  0.35,0.3,  0.35,0.2,
                0.55,0.35,  0.55,0.2,  0.35,0.2, 0.25,0.1,  0.55,0.2,

                0.4,0.5,  0.45,0.6,  0.65,0.45,  0.55,0.35,  0.4,0.5,
                0.4,0.45,  0.35,0.4,  0.4,0.35,  0.35,0.3,  0.35,0.2,
                0.55,0.35,  0.55,0.2,  0.35,0.2, 0.25,0.1,  0.55,0.2,

                0.4,0.5,  0.45,0.6,  0.65,0.45,  0.55,0.35,  0.4,0.5,
                0.4,0.45,  0.35,0.4,  0.4,0.35,  0.35,0.3,  0.35,0.2,
                0.55,0.35,  0.55,0.2,  0.35,0.2, 0.25,0.1,  0.55,0.2,

                0.4,0.5,  0.45,0.6,  0.65,0.45,  0.55,0.35,  0.4,0.5,
                0.4,0.45,  0.35,0.4,  0.4,0.35,  0.35,0.3,  0.35,0.2,
                0.55,0.35,  0.55,0.2,  0.35,0.2, 0.25,0.1,   0.55,0.2,

                0.4,0.5,  0.45,0.6,  0.65,0.45,  0.55,0.35,  0.4,0.5,
                0.4,0.45,  0.35,0.4,  0.4,0.35,  0.35,0.3,  0.35,0.2,
                0.55,0.35,  0.55,0.2,  0.35,0.2, 0.25,0.1,  0.55,0.2,

                0.4,0.5,  0.45,0.6,  0.65,0.45,  0.55,0.35,  0.4,0.5,
                0.4,0.45,  0.35,0.4,  0.4,0.35,  0.35,0.3,  0.35,0.2,
                0.55,0.35,  0.55,0.2,  0.35,0.2, 0.25,0.1,  0.55,0.2,

                0.4,0.5,  0.45,0.6,  0.65,0.45,  0.55,0.35,  0.4,0.5,
                0.4,0.45,  0.35,0.4,  0.4,0.35,  0.35,0.3,  0.35,0.2,
                0.55,0.35,  0.55,0.2,  0.35,0.2, 0.25,0.1,  0.55,0.2,

                0.45,0.5,  0.35,0.475, 0.3,0.35,  0.35,0.25, 0.45,0.225,
                0.54,0.25,  0.57,0.35,  0.56,0.45,  0.45,0.5, 0.5,0.55,
                0.55,0.53,  0.6,0.6,  0.55,0.53,  0.58,0.49, 0.56,0.45, 

                0.35,0.15, 0.35,0.6, 0.7,0.6,  0.7,0.375,  0.35,0.375,
                0.35,0.47, 0.55,0.47, 0.55,0.6, 0.35,0.47, 0.35,0.6,
                0.55,0.47, 0.45,0.47, 0.45,0.6, 0.45,0.47, 0.45,0.47        
                };

