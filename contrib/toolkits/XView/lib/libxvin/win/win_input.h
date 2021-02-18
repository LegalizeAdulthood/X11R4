/*
 * @(#)win_input.h 20.31 89/11/21 
 *
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * XView related input definitions.
 */

#ifndef	xview_win_input_DEFINED
#define	xview_win_input_DEFINED

/*
 ***********************************************************************
 *				Include Files
 ***********************************************************************
 */

#include <sys/types.h>
#include <sys/time.h>
#include <X11/Xlib.h>
#include <xview/base.h>
#include <xview/win_event.h>

/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */


/*
 * PUBLIC #defines 
 */

/*
 * XView semantic events 
 */

/*
 *    This #ifndef can go away when XVIEW_DEVID is defined in
 *	<sundef/vuid_event.h>
 */
#ifndef XVIEW_DEVID
#define XVIEW_DEVID                   0x7C
#endif

#define XVIEW_FIRST                   vuid_first(XVIEW_DEVID)

#define ACTION_NULL_EVENT             	(XVIEW_FIRST+0)		/* 31744 */
#define ACTION_ERASE_CHAR_BACKWARD    	(XVIEW_FIRST+1)
#define ACTION_ERASE_CHAR_FORWARD     	(XVIEW_FIRST+2)
#define ACTION_ERASE_WORD_BACKWARD    	(XVIEW_FIRST+3)
#define ACTION_ERASE_WORD_FORWARD     	(XVIEW_FIRST+4)
#define ACTION_ERASE_LINE_BACKWARD    	(XVIEW_FIRST+5)
#define ACTION_ERASE_LINE_END         	(XVIEW_FIRST+6)
#define ACTION_ERASE_SPARE1           	(XVIEW_FIRST+7)

#define ACTION_GO_CHAR_BACKWARD         (XVIEW_FIRST+8)		/* 31752 */
#define ACTION_GO_CHAR_FORWARD          (XVIEW_FIRST+9)
#define ACTION_GO_WORD_BACKWARD         (XVIEW_FIRST+10)
#define ACTION_GO_WORD_FORWARD          (XVIEW_FIRST+11)
#define ACTION_GO_WORD_END            	(XVIEW_FIRST+12)
#define ACTION_GO_LINE_BACKWARD         (XVIEW_FIRST+13)
#define ACTION_GO_LINE_FORWARD          (XVIEW_FIRST+14)
#define ACTION_GO_LINE_END            	(XVIEW_FIRST+15)
#define ACTION_GO_LINE_START          	(XVIEW_FIRST+16)        /* 31760 */
#define ACTION_GO_COLUMN_BACKWARD     	(XVIEW_FIRST+17)
#define ACTION_GO_COLUMN_FORWARD      	(XVIEW_FIRST+18)
#define ACTION_GO_DOCUMENT_START      	(XVIEW_FIRST+19)
#define ACTION_GO_DOCUMENT_END          (XVIEW_FIRST+20)
#define ACTION_GO_PAGE_FORWARD          (XVIEW_FIRST+21)
#define ACTION_GO_PAGE_BACKWARD         (XVIEW_FIRST+22)

#define ACTION_STOP                   	(XVIEW_FIRST+23)        /* 31767 */
#define ACTION_AGAIN                  	(XVIEW_FIRST+24)
#define ACTION_PROPS                  	(XVIEW_FIRST+25)
#define ACTION_UNDO                   	(XVIEW_FIRST+26)
#define ACTION_REDO                   	(XVIEW_FIRST+27)
#define ACTION_FRONT                  	(XVIEW_FIRST+28)
#define ACTION_BACK                   	(XVIEW_FIRST+29)
#define ACTION_COPY                   	(XVIEW_FIRST+30)
#define ACTION_OPEN                   	(XVIEW_FIRST+31)
#define ACTION_CLOSE                  	(XVIEW_FIRST+32)
#define ACTION_PASTE                  	(XVIEW_FIRST+33)
#define ACTION_FIND_BACKWARD          	(XVIEW_FIRST+34)
#define ACTION_FIND_FORWARD           	(XVIEW_FIRST+35)
#define ACTION_REPLACE                  (XVIEW_FIRST+36)
#define ACTION_CUT                    	(XVIEW_FIRST+37)

#define ACTION_SELECT_FIELD_BACKWARD  	(XVIEW_FIRST+38)        /* 31782 */
#define ACTION_SELECT_FIELD_FORWARD   	(XVIEW_FIRST+39)
#define ACTION_COPY_THEN_PASTE          (XVIEW_FIRST+40)
#define ACTION_STORE                  	(XVIEW_FIRST+41)
#define ACTION_LOAD                   	(XVIEW_FIRST+42)
#define ACTION_INCLUDE_FILE           	(XVIEW_FIRST+43)
#define ACTION_GET_FILENAME           	(XVIEW_FIRST+44)
#define ACTION_SET_DIRECTORY          	(XVIEW_FIRST+45)
#define ACTION_DO_IT                  	(XVIEW_FIRST+46)
#define ACTION_HELP                   	(XVIEW_FIRST+47)
#define ACTION_INSERT                 	(XVIEW_FIRST+48)
#define ACTION_INVOKE                 	(XVIEW_FIRST+49)
#define ACTION_EXPAND                 	(XVIEW_FIRST+50)
#define ACTION_MATCH_DELIMITER          (XVIEW_FIRST+51)        /* 31795 */
#define ACTION_CAPS_LOCK                (XVIEW_FIRST+52)
#define ACTION_QUOTE                  	(XVIEW_FIRST+53)
#define ACTION_EMPTY                  	(XVIEW_FIRST+54)
#define ACTION_SELECT                 	(XVIEW_FIRST+55)
#define ACTION_ADJUST                 	(XVIEW_FIRST+56)
#define ACTION_MENU                   	(XVIEW_FIRST+57)
#define ACTION_DRAG_MOVE              	(XVIEW_FIRST+58)        /* 31802 */
#define ACTION_DRAG_COPY              	(XVIEW_FIRST+59)        /* 31803 */
#define ACTION_DRAG_LOAD              	(XVIEW_FIRST+60)        /* 31804 */


#define ACTION_SPLIT_HORIZONTAL         (XVIEW_FIRST+61)        /* 31805 */
#define ACTION_SPLIT_VERTICAL         	(XVIEW_FIRST+62)        /* 31806 */
#define ACTION_SPLIT_INIT             	(XVIEW_FIRST+63)        /* 31807 */
#define ACTION_SPLIT_DESTROY          	(XVIEW_FIRST+64)        /* 31808 */


/*
 * OPEN LOOK events 
 */
#define ACTION_RESCALE                  (XVIEW_FIRST+65)        /* 31809 */
#define ACTION_PININ                  	(XVIEW_FIRST+66)        /* 31810 */
#define ACTION_PINOUT                 	(XVIEW_FIRST+67)        /* 31811 */
#define ACTION_DISMISS                  (XVIEW_FIRST+68)        /* 31812 */
#define ACTION_TAKE_FOCUS             	(XVIEW_FIRST+71)        /* 31815 */

#define ACTION_DEBUG1                 	(XVIEW_FIRST+72)        /* 31816 */
#define ACTION_DEBUG2                 	(XVIEW_FIRST+73)        /* 31817 */

#define XVIEW_LAST                    	ACTION_TAKE_FOCUS       /* 31815 */

/*
 * Server related definitions
 */
#define	LOC_MOVE			(VKEY_FIRSTPSEUDO+0)	/* 32512 */
#define	LOC_WINENTER			(VKEY_FIRSTPSEUDO+1)	/* 32513 */
#define	LOC_WINEXIT			(VKEY_FIRSTPSEUDO+2)	/* 32514 */
#define	LOC_MOVEWHILEBUTDOWN		(VKEY_FIRSTPSEUDO+3)	/* 32515 */
#define	LOC_DRAG			LOC_MOVEWHILEBUTDOWN	/* 32515 */
#define	WIN_REPAINT			(VKEY_FIRSTPSEUDO+4)	/* 32516 */
#define	WIN_RESIZE			(VKEY_FIRSTPSEUDO+5)	/* 32517 */
#define	WIN_STRUCTURE_NOTIFY		WIN_RESIZE		/* 32517 */
#define	WIN_MAP_NOTIFY			(VKEY_FIRSTPSEUDO+6)	/* 32518 */
#define	WIN_UNMAP_NOTIFY		(VKEY_FIRSTPSEUDO+7)	/* 32519 */
#define	KBD_USE				(VKEY_FIRSTPSEUDO+8)	/* 32520 */
#define	KBD_DONE			(VKEY_FIRSTPSEUDO+9)	/* 32521 */
#define WIN_CLIENT_MESSAGE      	(VKEY_FIRSTPSEUDO+10)   /* 32522 */

#define KBD_MAP                         (VKEY_FIRSTPSEUDO+11)   /* 32523 */
#define WIN_GRAPHICS_EXPOSE             (VKEY_FIRSTPSEUDO+12)   /* 32524 */
#define WIN_NO_EXPOSE                   (VKEY_FIRSTPSEUDO+13)   /* 32525 */
#define WIN_VISIBILITY_NOTIFY           (VKEY_FIRSTPSEUDO+14)   /* 32526 */
#define WIN_CREATE_NOTIFY               (VKEY_FIRSTPSEUDO+15)   /* 32527 */
#define WIN_SUBSTRUCTURE_NOTIFY         WIN_CREATE_NOTIFY       /* 32527 */
#define WIN_DESTROY_NOTIFY              (VKEY_FIRSTPSEUDO+16)   /* 32528 */
#define WIN_MAP_REQUEST                 (VKEY_FIRSTPSEUDO+17)   /* 32529 */
#define WIN_REPARENT_NOTIFY             (VKEY_FIRSTPSEUDO+18)   /* 32530 */
#define WIN_GRAVITY_NOTIFY              (VKEY_FIRSTPSEUDO+19)   /* 32531 */
#define WIN_RESIZE_REQUEST              (VKEY_FIRSTPSEUDO+20)   /* 32532 */
#define WIN_CONFIGURE_REQUEST           (VKEY_FIRSTPSEUDO+21)   /* 32533 */
#define WIN_SUBSTRUCTURE_REDIRECT       WIN_CONFIGURE_REQUEST   /* 32533 */
#define WIN_CIRCULATE_REQUEST           (VKEY_FIRSTPSEUDO+22)   /* 32534 */
#define WIN_CIRCULATE_NOTIFY            (VKEY_FIRSTPSEUDO+23)   /* 32535 */
#define WIN_PROPERTY_NOTIFY             (VKEY_FIRSTPSEUDO+24)   /* 32536 */

#ifdef SELN_EVENTS
					/* Future road work */ 
#define SEL_CLEAR                       (VKEY_FIRSTPSEUDO+25)   /* 32537 */
#define SEL_REQUEST                     (VKEY_FIRSTPSEUDO+26)   /* 32538 */
#define SEL_NOTIFY                      (VKEY_FIRSTPSEUDO+27)   /* 32539 */

#endif SELN_EVENTS

#define WIN_COLORMAP_NOTIFY             (VKEY_FIRSTPSEUDO+28)   /* 32540 */
#define MAPPING_NOTIFY                  (VKEY_FIRSTPSEUDO+29)   /* 32541 */

#define WIN_UNUSED_11                   (VKEY_FIRSTPSEUDO+30)   /* 32542 */

/*
 * In XVIEW, there is no way to generate SIGURG. We will
 * generate WIN_STOP every time L1 function key event happens
 */
#define	WIN_STOP			KEY_LEFT(1)

/*
 * The inputmask consists of a input code array + flags that indicates those
 * user actions to be placed in the input queue.
 */

#define	BITSPERBYTE			8

#define KEY_CODES      	(VKEY_LAST-VKEY_FIRST)
				/* The number of the virtual keyboard device
				   codes that are contained within the input
				   mask. */

#define	IM_NEGEVENT	(0x01)	/* send input negative events too */
#define	IM_POSASCII	(0x02)	/* OBSOLETE:no neg ASCII even if IM_NEGEVENT */
#define	IM_ANSI		(0x04)	/* OBSOLETE: ansi with funcs encoded in ESC[ */
#define	IM_UNENCODED	(0x08)	/* OBSOLETE: pure device codes */
#define	IM_ASCII	(0x10)	/* enable ASCII codes 0-127 */
#define	IM_META		(0x20)	/* enable META codes 128-255 */
#define	IM_NEGASCII	(0x40)	/* enable negative ASCII codes 0-127 */
#define	IM_NEGMETA	(0x80)	/* enable negative META codes 128-255 */
#define	IM_TOP		(0x100)	/* enable TOP codes 256-511 */
#define	IM_NEGTOP	(0x200)	/* enable negative TOP codes 256-511 */
#define	IM_INTRANSIT	(0x400)	/* don't surpress locator events when
				   in-transit over window */
				   
/* This number has to be rounded up in whole number */ 
#define	IM_MASKSIZE		(((KEY_CODES + ((sizeof(char))*BITSPERBYTE)) - 1)  \
				 / ((sizeof(char))*BITSPERBYTE))

#define	INPUTMASK_NULL	((Inputmask *)0)

#define isworkstationdevid(code) \
	( (( (code) >> 8 ) & 0xff) == WORKSTATION_DEVID )

#define	win_im_slot(code)	(((code)-VKEY_FIRST)/BITSPERBYTE)


/* VKEY_FIRST is defined as 32512 */

#define	win_setinputcodebit(im,code) \
	(im)->im_keycode[win_im_slot(code)] |= \
		(isworkstationdevid(code))? \
	    (1<<((BITSPERBYTE-1)-(((code)-VKEY_FIRST)%BITSPERBYTE))): \
	    0
#define	win_unsetinputcodebit(im,code) \
	(im)->im_keycode[win_im_slot(code)] &= \
		(isworkstationdevid(code))? \
	    (~(1<<((BITSPERBYTE-1)-(((code)-VKEY_FIRST)%BITSPERBYTE)))): \
	    0xff
#define	win_getinputcodebit(im,code) \
	((isworkstationdevid(code))? \
	((im)->im_keycode[win_im_slot(code)] & \
	    (1<<((BITSPERBYTE-1)-(((code)-VKEY_FIRST)%BITSPERBYTE)))): 0)

/* Mouse mask has to start from the 7th bit because META_SHIFT_MASK
 * uses the 6th bit.
 */
#define	MS_LEFT_MASK	(1L << 7)
#define	MS_MIDDLE_MASK	(1L << 8)
#define	MS_RIGHT_MASK	(1L << 9)
#define	MS_BUTTON_MASK	(MS_LEFT_MASK | MS_MIDDLE_MASK | MS_RIGHT_MASK)

/*
 * To convert an ascii key code to meta code 
 */
#define META_KEY_MASK	0x80

#define KEYBOARD_KYSM_MASK    0xFF00
#define KEYBOARD_KYSM         0xFF00
                      /* I really should use XVIEW_FIRST here, but since
                       * that involves shifting bits, I would rather just
                       * hardcode in the value here for speed.
                       */
#define XVIEW_SEMANTIC                0x7C00          /* XVIEW_FIRST */

/*
 * Definitions required for input event structures 
 */

#define	IE_NEGEVENT	(0x01)		/* input code is negative 	*/

/*
 * Values for ie_shiftmask:
 * Same as in sundev/kbd.h 
 */
#ifndef	CAPSLOCK	
#define	CAPSLOCK	0		/* Caps Lock key 		*/
#define	CAPSMASK	0x0001
#define	SHIFTLOCK	1		/* Shift Lock key 		*/
#define	LEFTSHIFT	2		/* Left-hand shift key 		*/
#define	RIGHTSHIFT	3		/* Right-hand shift key 	*/
#define	SHIFTMASK	0x000E
#define	LEFTCTRL	4		/* Left-hand (or only) ctrl key	*/
#define	RIGHTCTRL	5		/* Right-hand control key 	*/
#define	CTRLMASK	0x0030
#endif	CAPSLOCK

#ifndef	META_SHIFT_MASK
#define	META_SHIFT_MASK	0x0040		/* Meta shift mask: Same as 
					 * 'unused...	0x0040' in
					 * sundev/kbd.h
					 */
#endif	META_SHIFT_MASK

#define	EVENT_NULL	((Event *)0)


#define	win_inputnegevent(ie)	((ie)->ie_flags&IE_NEGEVENT)
#define	win_inputposevent(ie)	(!((ie)->ie_flags&IE_NEGEVENT))

#define event_init(ie)	 	 ((ie)->ie_flags = \
				  (ie)->ie_shiftmask = (ie)->ie_locx = \
				  (ie)->ie_locy = (ie)->ie_time.tv_sec = \
				  (ie)->ie_time.tv_usec = 0, \
				  (ie)->ie_win = NULL, \
				  (ie)->ie_xevent = NULL, \
                    		  (ie)->ie_string = NULL, \
				  (ie)->ie_code = (ie)->action  =  \
				  ACTION_NULL_EVENT)


#define	event_id(event)			((event)->ie_code)
#define event_flags(event)		((event)->ie_flags)
#define	event_shiftmask(event)		((event)->ie_shiftmask)
#define	event_x(event)			((event)->ie_locx)
#define	event_y(event)			((event)->ie_locy)
#define	event_time(event)		((event)->ie_time)
#define event_window(event)		((event)->ie_win)
#define event_string(event)             ((event)->ie_string)
#define event_xevent(event)		((event)->ie_xevent)

/* event action defaults to event id if not set */
#define	event_action(event)		\
	(((event)->action != ACTION_NULL_EVENT) ? ((event)->action) : \
	   event_id((event)))

#define event_xevent_type(event) \
		(((event)->ie_xevent != NULL) ? \
		    ((((XEvent *)(event)->ie_xevent))->type) : \
			XV_ERROR)

#define	event_is_up(event)		(win_inputnegevent((event)))
#define	event_is_down(event)		(win_inputposevent((event)))
#define	event_shift_is_down(event)	(event_shiftmask(event) & SHIFTMASK)
#define	event_ctrl_is_down(event)	(event_shiftmask(event) & CTRLMASK)
#define	event_meta_is_down(event)	(event_shiftmask(event)&META_SHIFT_MASK)
#define	event_left_is_down(event)	(event_shiftmask(event)&MS_LEFT_MASK)
#define	event_middle_is_down(event)	(event_shiftmask(event)&MS_MIDDLE_MASK)
#define	event_right_is_down(event)	(event_shiftmask(event)&MS_RIGHT_MASK)
#define	event_button_is_down(event)	(event_shiftmask(event)&MS_BUTTON_MASK)
#define event_is_string(event)          (event_string(event)!=NULL)
/*
 * BUG: The following three macros should change when we have remappable
 *	mouse keys.
 */
#define action_select_is_down(event) 	event_left_is_down(event)
#define action_adjust_is_down(event) 	event_middle_is_down(event)
#define action_menu_is_down(event)   	event_right_is_down(event)

#define	event_set_action(event, code)	(event)->action = (code)

#define	event_set_id(event, code) 	\
   event_id(event) = (code), event_set_action(event, ACTION_NULL_EVENT)
					 
#define	event_set_flags(event, flags)	event_flags(event) = (flags)
#define	event_set_shiftmask(event, shiftmask)	event_shiftmask(event) = (shiftmask)
#define	event_set_x(event, x)		event_x(event) = (x)
#define	event_set_y(event, y)		event_y(event) = (y)
#define	event_set_time(event, time)	event_time(event) = (time)
#define	event_set_window(event, win)	event_window(event) = (win)
#define	event_set_xevent(event, xevent)	event_xevent(event) = (xevent)
#define event_set_string(event, string) event_string(event) = (string)

#define	event_set_up(event)		(event)->ie_flags |= IE_NEGEVENT
#define	event_set_down(event)		(event)->ie_flags &= ~IE_NEGEVENT


#define	event_is_button(event)		\
   ((event_id(event) >= BUT_FIRST) && (event_id(event) <= BUT_LAST))
				 	 
#define	event_is_ascii(event)		\
   ((event_id(event) >= ASCII_FIRST) && (event_id(event) <= ASCII_LAST))
				 	 
#define	event_is_meta(event)		\
   ((event_id(event) >= META_FIRST) && (event_id(event) <= META_LAST))
				 	 
#define	event_is_key_left(event)	\
   ((event_id(event) >= KEY_LEFTFIRST) && (event_id(event) <= KEY_LEFTLAST))
				 	 
#define	event_is_key_right(event)	\
   ((event_id(event) >= KEY_RIGHTFIRST) && (event_id(event) <= KEY_RIGHTLAST))
 
#define	event_is_key_top(event)	\
   ((event_id(event) >= KEY_TOPFIRST) && (event_id(event) <= KEY_TOPLAST))
				 	 
#define	event_is_key_bottom(event)	\
   ((event_id(event) == KEY_BOTTOMLEFT) || (event_id(event) == KEY_BOTTOMRIGHT))
 

/*
 *	for mouse control (button order & scaling)
 */

#define	WS_ORDER_LMR		0
#define	WS_ORDER_LRM		1
#define	WS_ORDER_MLR		2
#define	WS_ORDER_MRL		3
#define	WS_ORDER_RLM		4
#define	WS_ORDER_RML		5

/*
 * For the Ws_scale structure 
 */
#define WS_SCALE_MAX_COUNT      16
#define WS_SCALE_MAX_CEILING 0xffff

/*
 * BUG: get rid of these 
 * common left keypad keys
 */
#define	WIN_KEY_STOP		KEY_LEFT(1)
#define	WIN_KEY_AGAIN		KEY_LEFT(2)
#define	WIN_KEY_PROPS		KEY_LEFT(3)
#define	WIN_KEY_UNDO		KEY_LEFT(4)
#define	WIN_KEY_EXPOSE		KEY_LEFT(5)
#define	WIN_KEY_PUT		KEY_LEFT(6)
#define	WIN_KEY_OPEN		KEY_LEFT(7)
#define	WIN_KEY_GET		KEY_LEFT(8)
#define	WIN_KEY_FIND		KEY_LEFT(9)
#define	WIN_KEY_DELETE		KEY_LEFT(10)

#define WIN_KEY_CAPS		KEY_TOP(1)

#define	XV_POINTER_WINDOW	0L		/* PointerWindow in
X.h */
#define	XV_FOCUS_WINDOW		1L		/* InputFocus in
X.h */


/*
 ***********************************************************************
 *		Typedefs, Enumerations, and Structures
 ***********************************************************************
 */

typedef	struct inputmask {
	short	im_flags;
	char	im_keycode[IM_MASKSIZE];
} Inputmask;

/*
 * An input event structure is that which is filled in on a readrecord call.
 * ie_code contains the input code involved.  IE_NEGEVENT is used to indicate
 * a negative event.
 *
 * ie_shiftmask contains selected shift state.
 *
 * The locator position (window relative) is placed in ie_loc*.
 * The time of the input event is put in ie_time (seconds & microseconds).
 * Time zone and daylight savings flag available via system call.
 */

typedef	struct	inputevent {
	short	ie_code;		/* input code */
	short	ie_flags;
	short	ie_shiftmask;		/* input code shift state */
	short	ie_locx, ie_locy;	/* locator (usually a mouse) position */
	struct	timeval ie_time;	/* time of event */
	short	action;			/* keymapped version of ie_code */
	Xv_object	ie_win; 	/* window the event is directed to */
        char    *ie_string;             /* String returned from XLookupString
                                         * or language translation string.    */
	XEvent	*ie_xevent;		/* pointer to actual XEvent struct */
} Event;

typedef struct {
    unsigned short  ceiling;
    unsigned short  factor;
} Ws_scale;

typedef struct {
    Ws_scale        scales[WS_SCALE_MAX_COUNT];
} Ws_scale_list;


extern struct rectlist *win_get_damage();
#endif	~xview_win_input_DEFINED
