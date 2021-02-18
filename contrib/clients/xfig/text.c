/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	January 1985.
 *	1st revision : Aug 1985.
 *	2nd revision : Feb 1988.
 *
 *	%W%	%G%
*/
#include "fig.h"
#include "resources.h"
#include "alloc.h"
#include "const.h"
#include "font.h"
#include "func.h"
#include "object.h"
#include "paintop.h"

extern			(*canvas_kbd_proc)();
extern			(*canvas_locmove_proc)();
extern			(*canvas_leftbut_proc)();
extern			(*canvas_middlebut_proc)();
extern			(*canvas_rightbut_proc)();
extern			null_proc();
extern char		*calloc();
extern			set_popupmenu();
extern F_text		*text_search();

extern			char_handler();
extern int		cur_x, cur_y;
extern char		prefix[], suffix[];
extern int		leng_prefix, leng_suffix;
extern int		cur_color;
extern int		cur_textstyle;
extern float		cur_angle;

extern F_compound	objects;

static int		char_ht, char_wid;
static int		base_x, base_y;
static PR_SIZE		tsize;
static F_text		*cur_text;

F_text			*create_text();
static			finish_text_input();
static			finish_n_start();
static			init_text_input();

static
finish_n_start(x, y)
{
	wrap_up();
	init_text_input(x, y);
	}

static
finish_text_input()
{
	wrap_up();
	text_drawing_selected();
	}

static
wrap_up()
{
	PR_SIZE		size;
	int		kbd_received;

	reset_action_on();
	kbd_received = terminate_char_handler();
	if ( ! kbd_received) return;

	if (cur_text == NULL) {	/* a brand new text */
	    if (leng_prefix == 0) return;
	    pw_text(canvas_pixwin, base_x, base_y, PAINT, canvas_font, prefix);
	    cur_text = create_text();
	    insert_text(&objects.texts, cur_text);
	    }
	else {			/* existing text modified */
	    strcat(prefix, suffix);
	    leng_prefix += leng_suffix;
	    if (leng_prefix == 0) {
		delete_text(&objects.texts, cur_text);
		cfree(cur_text->cstring);
		free((char*)cur_text);
		return;
		}
	    if (strlen(cur_text->cstring) >= leng_prefix) { 
		strcpy(cur_text->cstring, prefix);
		}
	    else { /* free old and allocate new */
		cfree(cur_text->cstring);
		cur_text->cstring = calloc((unsigned)(leng_prefix+1), sizeof(char));
		strcpy(cur_text->cstring, prefix);
		}
	    size = pf_textwidth(leng_prefix, canvas_font, prefix);
	    cur_text->height = size.y;
	    cur_text->length = size.x;  /* in pixels */
	    }
	clean_up();
	set_action_object(F_TEXT, O_TEXT);
	set_latesttext(cur_text);
	set_modifiedflag();
	}

static
init_text_input(x, y)
int	x, y;
{
	cur_x = x;
	cur_y = y;

	set_action_on();
	canvas_kbd_proc = char_handler;
	canvas_middlebut_proc = finish_text_input;
	canvas_leftbut_proc = finish_n_start;
	canvas_rightbut_proc = null_proc;

	if ((cur_text = text_search(cur_x, cur_y)) == NULL) {
	    leng_prefix = leng_suffix = 0;
	    *suffix = 0;
	    prefix[leng_prefix] = '\0';
	    base_x = cur_x;
	    base_y = cur_y;
	    }
	else { /* leng_prefix is # of char in the text before the cursor */
	    leng_suffix = strlen(cur_text->cstring);
	    leng_prefix = prefix_length(roman_font, cur_text->cstring, 
					cur_x - cur_text->base_x);
	    leng_suffix -= leng_prefix;
	    cpy_n_char(prefix, cur_text->cstring, leng_prefix);
	    strcpy(suffix, &cur_text->cstring[leng_prefix]);
	    tsize = pf_textwidth(leng_prefix, canvas_font, prefix);
	    cur_x = base_x = cur_text->base_x;
	    cur_y = base_y = cur_text->base_y;
	    cur_x += tsize.x;
	    }
	initialize_char_handler(canvas_pixwin, canvas_font,
			finish_text_input, base_x, base_y);
	}

text_drawing_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_middlebut_proc = null_proc;
	canvas_leftbut_proc = init_text_input;
	canvas_rightbut_proc = set_popupmenu;
	char_ht = char_height(canvas_font);
	char_wid = char_width(canvas_font);
	set_cursor(&pencil_cursor);
	}

F_text *
create_text()
{
	F_text		*text;
	PR_SIZE		size;

	if ((Text_malloc(text)) == NULL) {
	    put_msg(Err_mem);
	    return(NULL);
	    }
	text->cstring = calloc((unsigned)(leng_prefix+1), sizeof(char));
	text->type = T_LEFT_JUSTIFIED;
	text->font = 0;
	text->angle = cur_angle;
	text->style = cur_textstyle;
	text->color = cur_color;
	text->depth = 0;
	text->pen = NULL;
	size = pf_textwidth(leng_prefix, canvas_font, prefix);
	text->size = size.y;
	text->length = size.x;	/* in pixels */
	text->height = size.y;	/* in pixels */
	text->base_x = base_x;
	text->base_y = base_y;
	strcpy(text->cstring, prefix);
	text->next = NULL;
	return(text);
	}

cpy_n_char(dst, src, n)
char	*dst, *src;
int	 n;
{
	/* src must be longer than n chars */

	while (n--) *dst++ = *src++;
	*dst = '\0';
	}

int
prefix_length(font, string, where_p)
PIX_FONT		font;
char		*string;
int		 where_p;
{
	/* c stands for character unit and p for pixel unit */
	int		l, len_c, len_p;
	int		char_wid, where_c;
	PR_SIZE		size;

	if (font == NULL) font = roman_font;
	len_c = strlen(string);
	size = pf_textwidth(len_c, font, string);
	len_p = size.x;
	if (where_p >= len_p) return(len_c); /* entire string is the preffix */

	char_wid = char_width(font);
	where_c = where_p / char_wid;	/* estimated char position */
	size = pf_textwidth(where_c, font, string);
	l = size.x;	/* actual lenght (pixels) of string of where_c chars */
	if (l < where_p) {
	    do {	/* add the width of next char to l */
		l += (char_wid = char_advance(font,string[where_c++]));
		} while (l < where_p);
	    if (l-(char_wid>>1) >= where_p) where_c--;
	    }
	else if (l > where_p) {
	    do {	/* subtract the width of last char from l */
		l -= (char_wid = char_advance(font,string[--where_c]));
		} while (l > where_p);
	    if (l+(char_wid>>1) >= where_p) where_c++;
	    }
	return(where_c);
	}

draw_text(text, op)
F_text	*text;
int	op;
{
	pw_text(canvas_pixwin, text->base_x, text->base_y, 
		op, canvas_font, text->cstring);
	}
