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
#include "const.h"
#include "font.h"
#include "paintop.h"

extern int		errno;
extern int		receiving_msg;
extern int		action_on;
extern			null_proc();

extern int		CANVAS_WIDTH;
extern char		prefix[], suffix[];
extern int		leng_prefix, leng_suffix;
extern int		cur_x, cur_y;

extern			(*canvas_kbd_proc)();
extern			(*canvas_locmove_proc)();
extern			(*canvas_leftbut_proc)();
extern			(*canvas_middlebut_proc)();
extern			(*canvas_rightbut_proc)();

/*****************  exported  variables and procedures   *****************/

			put_msg();
			init_msgreceiving();

/************************  local variables and routines ******************/

#define			BUF_SIZE		80

static			(*msgsw_kbd_proc)();
static			(*msgsw_leftbut_proc)();
static			(*msgsw_rightbut_proc)();
static			(*recipient)();

static char		msg_received[BUF_SIZE];
static char		prompt[BUF_SIZE];
static int		len_prompt;
static int		base_x = 5, base_y = 18;
static int		input_base_x;

static			msg_selected();
static			msg_sighandler();

static			(*saved_canvas_kbd_proc)();
static			(*saved_canvas_locmove_proc)();
static			(*saved_canvas_leftbut_proc)();
static			(*saved_canvas_middlebut_proc)();
static			(*saved_canvas_rightbut_proc)();

#ifndef	X11
int
init_msg(tool)
TOOL		tool;
{
	INPUTMASK		mask;

	msg_sw = tool_createsubwindow(tool, "", -1, -1);
	if (msg_sw == (TOOLSW)0) return(0);
	msgswfd = msg_sw->ts_windowfd;
	(void)fcntl(msgswfd, F_SETFL, fcntl(msgswfd, F_GETFL, 0) | O_NDELAY);

	msg_pixwin = pw_open(msgswfd);
	msg_sw->ts_io.tio_selected = msg_selected;
	msg_sw->ts_io.tio_handlesigwinch = msg_sighandler;
	msg_sw->ts_destroy = null_proc;

	input_imnull(&mask);
	win_setinputcodebit(&mask, MS_LEFT);
	win_setinputcodebit(&mask, MS_RIGHT);
	mask.im_flags |= IM_ASCII | IM_INTRANSIT;
	win_setinputmask(msgswfd, &mask, NULL, WIN_NULLLINK);
	win_setcursor(msgswfd, &pencil_cursor);
	msgsw_kbd_proc = msgsw_leftbut_proc = msgsw_rightbut_proc = null_proc;
	return(1);
	}
#else
static Arg      msg_args[] =
{
	{ XtNx, (XtArgVal)0 },
	{ XtNy, (XtArgVal)0 },
	{ XtNwidth, (XtArgVal)0 },
	{ XtNheight, (XtArgVal)MSG_HEIGHT },
	{ XtNlabel, (XtArgVal)"" },
	{ XtNfromHoriz, (XtArgVal)NULL },
	{ XtNhorizDistance, (XtArgVal)0 },
	{ XtNfromVert, (XtArgVal)NULL },
	{ XtNvertDistance, (XtArgVal)0 },
};

int init_msg(tool)
	TOOL		tool;
{
	msg_args[2].value = CANVAS_WIDTH;
	msg_args[5].value = (XtArgVal)panel_sw;
	msg_args[7].value = (XtArgVal)canvas_sw;
	msg_sw = XtCreateManagedWidget("message", commandWidgetClass, tool,
		msg_args, XtNumber(msg_args));
	return (1);
}

/*
**	We have to do this after realizing the widget otherwise
**	the width is computed wrong and you get a tiny text box.
*/
setup_msg()
{
	static Arg      addn_args[] =
	{
		{ XtNfont, (XtArgVal)NULL },
	};

	if (bold_font != NULL)
	{
		addn_args[0].value = (XtArgVal)bold_font;
		XtSetValues(msg_sw, addn_args, XtNumber(addn_args));
	}
	msgswfd = XtWindow(msg_sw);
	XDefineCursor(tool_d, msgswfd, (Cursor)pencil_cursor.bitmap);
}
#endif	X11

#ifndef	X11
static
msg_selected(nullsw, ibits, obits, ebits, timer)
caddr_t		*nullsw;
int		*ibits, *obits, *ebits;
struct timeval	*timer;
{
	INPUTEVENT	   ie;

	*ibits = *obits = *ebits = 0;
	if (action_on) return;
	if (input_readevent(msgswfd, &ie) == -1) {
	    printf("error %d while read input event in msg window\n", errno);
	    return;
	    }
	if (win_inputnegevent(&ie)) return;
	if (! receiving_msg) return;

	if (ie.ie_code <= ASCII_LAST) {
	    msgsw_kbd_proc(ie.ie_code);
	    }
	else if (ie.ie_code == MS_LEFT) {
	    msgsw_leftbut_proc(ie.ie_locx, ie.ie_locy);
	    }
	else if (ie.ie_code == MS_RIGHT) {
	    msgsw_rightbut_proc(ie.ie_locx, ie.ie_locy);
	    }
	}

static
msg_sighandler()
{
	pw_damaged(msg_pixwin);
	pw_writebackground(msg_pixwin, 0, 0, 2048, 2048, PAINT);
	pw_text(msg_pixwin, base_x, base_y, PAINT, bold_font, prompt);
	pw_donedamaged(msg_pixwin);
	}

/*VARARGS1*/
put_msg(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
char   *format, *arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7, *arg8;
{
	clear_message();
	sprintf(prompt, format, arg1, arg2, arg3, arg4, arg5, 
		arg6, arg7, arg8);
	pw_text(msg_pixwin, base_x, base_y, PAINT, bold_font, prompt);
	}

clear_message()
{
	pw_writebackground(msg_pixwin, 0, 0, 2048, 2048, PAINT);
	}
#else
/*VARARGS1*/
put_msg(format, arg1, arg2, arg3, arg4, arg5)
	char	*format;
	int	arg1, arg2, arg3, arg4, arg5;
{
	sprintf(prompt, format, arg1, arg2, arg3, arg4, arg5);
	msg_args[4].value = (XtArgVal)prompt;
	XtSetValues(msg_sw, &msg_args[4], 1);
}

clear_message()
{
	msg_args[4].value = (XtArgVal)"";
	XtSetValues(msg_sw, &msg_args[4], 1);
}
#endif	X11

static
end_char_input()
{
	receiving_msg = 0;  /*  msg input has been received  */
	terminate_char_handler();
	strcpy(&prefix[leng_prefix], suffix);
	strcpy(msg_received, prefix);
	strcpy(&prompt[len_prompt], prefix);
	if (recipient != NULL) (* recipient) (msg_received);
	restore_event_proc();
	return;
	}

static
reposition_cursor(x, y)
int	x, y;
{
	int		leng;
	PR_SIZE		s;

	strcpy(&prefix[leng_prefix], suffix);
	leng = leng_prefix + leng_suffix; 
	s = pf_textwidth(leng, bold_font, prefix);
	if ( ! (input_base_x <= x && x <= input_base_x+s.x
		&& base_y-s.y <= y && y <= base_y)) {
	    prefix[leng_prefix] = '\0';
	    return;
	    }

	leng_suffix = 0;
	erase_char_string();
	leng_prefix = prefix_length(bold_font, prefix, x-input_base_x);
	strcpy(suffix, &prefix[leng_prefix]);
	prefix[leng_prefix] = '\0';
	leng_suffix = leng - leng_prefix;
	s = pf_textwidth(leng_prefix, bold_font, prefix);
	cur_x = input_base_x + s.x;
	draw_char_string();
	}

#ifndef	X11
init_msg_receiving(msg_recipient, p)
int	(*msg_recipient)();
char	*p;			/* prompt */
{
	extern		char_handler();
	PR_SIZE		prompt_size;

	saved_canvas_kbd_proc = canvas_kbd_proc;
	saved_canvas_locmove_proc = canvas_locmove_proc;
	saved_canvas_leftbut_proc = canvas_leftbut_proc;
	saved_canvas_middlebut_proc = canvas_middlebut_proc;
	saved_canvas_rightbut_proc = canvas_rightbut_proc;

	receiving_msg = 1;
	msgsw_kbd_proc = char_handler;
	msgsw_leftbut_proc = reposition_cursor;
	canvas_kbd_proc = char_handler;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = null_proc;
	canvas_rightbut_proc = null_proc;
	recipient = msg_recipient;
	blink_msg();
	clear_message();
	strcpy(prompt, p);
	pw_text(msg_pixwin, base_x, base_y, PAINT, bold_font, prompt);

	len_prompt = strlen(prompt);
	prompt_size = pf_textwidth(len_prompt, bold_font, prompt);
	cur_x = input_base_x = base_x + prompt_size.x;
	cur_y = base_y;
	leng_prefix = leng_suffix = 0;
	*prefix = *suffix = '\0';
	initialize_char_handler(msg_pixwin, bold_font, end_char_input, cur_x, base_y);
	}
#else
init_msg_receiving(msg_recipient, p)
	int		(*msg_recipient)();
	char		*p;
{
	char		reply[100];

	prompt_string(p, reply);
	(*msg_recipient)(reply);
}
#endif	X11

restore_event_proc()
{
	canvas_kbd_proc = saved_canvas_kbd_proc;
	canvas_locmove_proc = saved_canvas_locmove_proc;
	canvas_leftbut_proc = saved_canvas_leftbut_proc;
	canvas_middlebut_proc = saved_canvas_middlebut_proc;
	canvas_rightbut_proc = saved_canvas_rightbut_proc;
	}

blink_msg()
{
	int	i;

#ifndef	X11
	pw_writebackground(msg_pixwin, 0, 0, 2048, 2048, ERASE);
#else
#endif	X11
	for (i = 0; i < 2000; i++);
	clear_message();
	}
