/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1988 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	Febuary 1988.
 *
 *	%W%	%G%
*/
#include "fig.h"
#include "resources.h"
#include "font.h"
#include "paintop.h"

extern int		cur_x, cur_y;

#define			BUF_SIZE	80
#define			CR		13
#define			CTRL_U		21
#define			SP		32
#define			DEL		127
#define			CTRL_H		8
	
char			prefix[BUF_SIZE], suffix[BUF_SIZE];
int			leng_prefix, leng_suffix;

static PIX_FONT		font;
static PIXWIN		pw;
static int		char_ht, char_wid;
static int		char_received;
static int		base_x, base_y;

static			(*cr_proc)();

static
draw_cursor(x, y)
int	x, y;
{
	pw_vector(pw, x, y, x, y-char_ht, INV_PAINT, 1);
	}

initialize_char_handler(p, f, cr, bx, by)
PIXWIN		p;
PIX_FONT		f;
int		(*cr)();
int		bx, by;
{
	pw = p;
	font = f;
	cr_proc = cr;
	base_x = bx;
	base_y = by;

	char_wid = char_width(f);
	char_ht = char_height(f);
	char_received = 0;
	turn_on_blinking_cursor(draw_cursor, draw_cursor,
				cur_x, cur_y, (long)0, (long)700000);
	}

terminate_char_handler()
{
	turn_off_blinking_cursor();
	cr_proc = NULL;
	return(char_received);
	}

erase_char_string()
{
	pw_text(pw, base_x, base_y, INV_PAINT, font, prefix);
	if (leng_suffix) pw_text(pw, cur_x, base_y, INV_PAINT, font, suffix);
	}

draw_char_string()
{
	pw_text(pw, base_x, base_y, INV_PAINT, font, prefix);
	if (leng_suffix) pw_text(pw, cur_x, base_y, INV_PAINT, font, suffix);
	move_blinking_cursor(cur_x, cur_y);
	}

char_handler(c)
char	c;
{
	if (c == CR) {
	    cr_proc();
	    }
	else if (c == DEL || c == CTRL_H) {
	    if (leng_prefix > 0) {
		erase_char_string();
		char_received = 1;
		cur_x -= char_wid;
		prefix[--leng_prefix] = '\0';
		draw_char_string();
		}
	    }
	else if (c == CTRL_U) {
	    if (leng_prefix > 0) {
		erase_char_string();
		char_received = 1;
		cur_x = base_x;
		leng_prefix = 0;
		*prefix = '\0';
		draw_char_string();
		}
	    }
	else if (c < SP) {
	    }
	else if (leng_prefix+leng_suffix == BUF_SIZE) {
	    put_msg("Text buffer is full, character is ignored");
	    }
	else {
	    erase_char_string();
	    char_received = 1;
	    cur_x += char_wid;
	    prefix[leng_prefix++] = c;
	    prefix[leng_prefix] = '\0';
	    draw_char_string();
	    }
	}
