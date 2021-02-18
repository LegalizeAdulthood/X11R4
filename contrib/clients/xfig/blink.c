/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1988 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	Febuary 1988.
 *
 *	%W%	%G%
*/
#include "fig.h"

static int		cursor_on, cursor_is_moving;
static int		cursor_x, cursor_y;
static int		blink();
static int		(*erase)();
static int		(*draw)();
static struct itimerval	blink_timer;

turn_on_blinking_cursor(draw_cursor, erase_cursor, x, y, sec, usec)
int	(*draw_cursor)();
int	(*erase_cursor)();
int	x, y;
long	sec, usec;
{
	draw = draw_cursor;
	erase = erase_cursor;
	cursor_is_moving = 0;
	cursor_x = x;
	cursor_y = y;
	blink_timer.it_interval.tv_usec = usec;
	blink_timer.it_interval.tv_sec = sec;
	blink_timer.it_value.tv_usec = usec;
	blink_timer.it_value.tv_sec = sec;
	draw(x, y);
	cursor_on = 1;
	signal(SIGALRM, blink);
	setitimer(ITIMER_REAL, &blink_timer, (struct itimerval *)0);
	/*
	(void) notify_set_itimer_func(&me, blink, ITIMER_REAL, &blink_timer, 0);
	*/
	}

turn_off_blinking_cursor()
{
	if (cursor_on) erase(cursor_x, cursor_y);
	blink_timer.it_interval.tv_usec = 0;
	blink_timer.it_interval.tv_sec = 0;
	blink_timer.it_value.tv_usec = 0;
	blink_timer.it_value.tv_sec = 0;
	setitimer(ITIMER_REAL, &blink_timer, (struct itimerval *)0);
	/*
	(void) notify_set_itimer_func(&me, blink, ITIMER_REAL, 0, 0);
	*/
	draw = erase = NULL;
	}

static
blink()
{
	if (cursor_is_moving) return(0);
	if (cursor_on) {
	    erase(cursor_x, cursor_y);
	    cursor_on = 0;
	    }
	else {
	    draw(cursor_x, cursor_y);
	    cursor_on = 1;
	    }
	return(0);
	}

move_blinking_cursor(x, y)
int	x, y;
{
	cursor_is_moving = 1;
	if (cursor_on) erase(cursor_x, cursor_y);
	cursor_x = x;
	cursor_y = y;
	draw(cursor_x, cursor_y);
	cursor_on = 1;
	cursor_is_moving = 0;
	}
