/*
 *	control.h
 *
 *	defines for control manager
 */

# define CONTROL_BUTTON	0		/* named button */
# define CONTROL_SCROLL	1		/* scroll bar */
# define CONTROL_RADIO	2		/* radio buttons */
# define CONTROL_CHECK	3		/* check boxes */
# define CONTROL_SIZE	4		/* resize box */
# define CONTROL_EDIT	5		/* text edit box */

typedef int	Button;

typedef int	Check;

typedef int	Editbox;

# define EDIT_VOID	0
# define EDIT_RETURN	1

typedef int	Scrollbar;

# define SCROLL_LEFT	0
# define SCROLL_TOP	1
# define SCROLL_RIGHT	2
# define SCROLL_BOTTOM	3

# define SCROLLWIDTH	25

# define SCROLL_UP_BUTTON	-1
# define SCROLL_DOWN_BUTTON	-2
# define SCROLL_UP_AREA		-3
# define SCROLL_DOWN_AREA	-4
# define SCROLL_BUTTON_RELEASE	-5

# define SCROLL_EVENTS	(ButtonPressed|ButtonReleased|ExposeWindow| \
			 RightDownMotion|LeftDownMotion|MiddleDownMotion)
