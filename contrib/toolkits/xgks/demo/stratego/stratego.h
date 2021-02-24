/*
 *              Copyright IBM Corporation 1989
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

#define NO_PIECE 0
#define MARSHAL 1
#define GENERAL 2
#define COLONEL 3
#define MAJOR 4
#define CAPTAIN 5
#define LIEUTENANT 6
#define SERGEANT 7
#define MINER 8
#define SCOUT 9
#define SPY 10
#define BOMB 11
#define FLAG 12

#define GREEN 0
#define RED 1
#define BLUE 2
#define BLACK 3
#define L_BLUE 4
#define WHITE 5
#define FLASH1 6
#define FLASH2 7

#define LOW 0
#define HIGH 1

#define QUIT -1
#define SKIPMOVE 99

#define FALSE 0
#define TRUE 1

#define READ 0
#define WRITE 1

/* variable declarations */

typedef struct
{
    int rank, color;
} SQUARE;

typedef struct
{
    int x, y;
} POSITION;

SQUARE board[10][10] = { 12, 1, 11, 1, 3, 1, 1, 1, 0, 0, 0, 0, 1, 2, 3, 2, 11, 2, 12, 2,
    11, 1, 11, 1, 3, 1, 2, 1, 0, 0, 0, 0, 2, 2, 3, 2, 11, 2, 11, 2,
    4, 1, 4, 1, 11, 1, 5, 1, 0, 0, 0, 0, 5, 2, 11, 2, 4, 2, 4, 2,
    4, 1, 5, 1, 5, 1, 11, 1, 0, 0, 0, 0, 11, 2, 5, 2, 5, 2, 4, 2,
    6, 1, 6, 1, 6, 1, 5, 1, 0, 0, 0, 0, 5, 2, 6, 2, 6, 2, 6, 2,
    8, 1, 8, 1, 7, 1, 11, 1, 0, 0, 0, 0, 11, 2, 7, 2, 8, 2, 8, 2,
    8, 1, 7, 1, 7, 1, 6, 1, 0, 0, 0, 0, 6, 2, 7, 2, 7, 2, 8, 2,
    9, 1, 9, 1, 8, 1, 8, 1, 0, 0, 0, 0, 8, 2, 8, 2, 9, 2, 9, 2,
    9, 1, 7, 1, 9, 1, 9, 1, 0, 0, 0, 0, 9, 2, 9, 2, 7, 2, 9, 2,
    10, 1, 9, 1, 9, 1, 9, 1, 0, 0, 0, 0, 9, 2, 9, 2, 9, 2, 10, 2 };

POSITION orig, dest; /* origination and destination squares positions */
