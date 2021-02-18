/***********************************************************
Copyright International Business Machines Corporation 1989

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#ifndef AIXKEYMAP_H
#define AIXKEYMAP_H

#include <X11/AIX.h>		/* AIX_GLYPHS_PER_KEY 	*/
#include <X11/X.h>		/* KeySym	      	*/
#include <X11/keysym.h>		/* keysym defs		*/

/* from X10 Xkeymap.h header file       */
/*
 *  Some keyboards may require more then 5 bits of state information or
 *  32 possible states.  As such, keymaps whose magic number ( first byte )
 *  of file) is RT256_KEYMAP_MAGIC will have 256 possible states  in the
 *  state array.  Map_States will define how many states are actually in the
 *  state array and therefore the maximum number of states for the keymap file.
 *  This will provide the ability to have 8 bits of state information.
 *  --------------------  NOTE: --------------------
 *  The US XLookupMapping function will only support 32 states.
 */
#define RT256_KEYMAP_MAGIC      0375  /* magic number which must be first byte
				      of a keymap file */
#define AIX_KEYMAP_MAGIC        0376  /* new version number - w/ IM */

/*------------------------ KEYCOMP DEFINES -----------------------------*/

			     /* Max number of states /key       */
#define X_MAX_STATES          32
#define X_MAX_STATES_256      256

/*
key values in keycomp source file are KeySym values.
only XK_LATIN1 (0x020 - 0x0ff) and XK_MISCELLANY (0xff00 - 0xffff)
KeySyms are supported.
*/
#define X_MAX_KEYCODES          0x200

/************ shifts for keyboard fields ****************************/

/* key.type field */

#define  GRAPHIC   0                    /* graphic character           */
#define  SGL_CTL   1                    /* Single byte control         */
#define  CHAR_STR  4                    /* Character string ( Xlib fix)*/
#define  ES_FUNC   5                    /* ESC function                */
#define  CTLFUNC   6                    /* CTRL function               */
#define  BIND_STR  7                    /* Rebind'd string             */

/* key.stat field */

#define  NORM      0                    /* NORMAL                      */
#define  CURS      3                    /* cursor key                  */
#define  CPFK      4                    /* PF key                      */
#define  DEAD      5                    /* dead key                    */

/* valid code pages */

#define  P0        0x3c
#define  P1        0x3d
#define  P2        0x3e

/* valid code points */

#define  IC_SS1    0x1f
#define  IC_SS2    0x1e
#define  IC_SS3    0x1d
#define  IC_SS4    0x1c
#define  IC_IGNORE 0xfe

/*----------------------- TYPEDEFs ------------------------------------*/

/* KeyMapElt: contains single-byte character bindings, function binding
   or indications that a keycode is actually bound in the string extension
   or in the runtime table */

typedef union {
      int               element ;       /* each element is 4 bytes     */
      struct     {
	 unsigned       type    :4;     /* key.type - type of key      */
	 unsigned       stat    :4;     /* key.stat - status of key    */
	 unsigned       pad     :8;     /* NOT USED                    */
	 unsigned       page    :8;     /* key.page - code page of key */
	 unsigned       point   :8;     /* key.point - code point of ky*/
	 }              key ;
      struct  {
	 unsigned short pad ;           /* NOT USED - see key.type     */
	 unsigned short id  ;           /* func.id  - function id      */
	 }              func ;
      struct  {
	unsigned char   pad ;           /* NOT USED - see key.type     */
	unsigned char   page ;          /* str.page - code page of str */
	unsigned short  offset ;        /* str.offset - offset to str  */
	}               str ;
      }                 OldKeyMapElt ;

/* LockTable: bitmap of which keys are to handle Lock shift state.  This is
   for private use within XLookupMapping.  To utilize Lock column all
   bits should be set ON */

typedef  int  LockTable[8] ;

/* Keystring: contain multi-byte character strings.  Not modified at
   runtime.  Follows immediately after KeyMap. */

typedef struct {
    unsigned char length;
    unsigned char c[1] ;
    } KeyString ;

/* KeyMapFile: contains image of keycomp binary file structure. */

#define KEYPADDING      509

typedef struct KEYMAPFILE {
    struct KEYMAPHEADER {
    char                magic ;         /* hdr.magic - magic number    */
    unsigned char       max_state ;     /* hdr.max_state - # of states */
					/*    in the element array     */
    unsigned short      max_pos ;       /* hdr.max_pos - # of key posnt*/
    unsigned short      min_pos ;       /* hdr.min_pos - 1st keycode   */
    }                   hdr ;           /* hdr                         */
    unsigned char       Max_States ;    /* Used only if magic =        */
					/*  RT256_KEYMAP_MAGIC         */
    char                pad[KEYPADDING];/* padding for future use      */
    short               state[X_MAX_STATES] ; /* state mapping table   */
    union KEYMAPELT {
    OldKeyMapElt        elt[1] ;        /* map.elt[i] - key code elemet*/
    KeyString           s ;             /* map.s - string array        */
    char                c[1] ;          /* map.c - character address   */
    }                   map ;
    }                   KeyMapFile ;

/* KeyMapPtr: general purpose pointer to KeyMapElt, KeyString or
   character pointer */

typedef union {
    OldKeyMapElt       *elt ;           /* p.elt - key code elemet     */
    KeyString          *s ;             /* p.s - string array          */
    char               *c ;             /* p.c - character address     */
    }                   KeyMapPtr ;

/*------------------------ MACROS --------------------------------------*/

#define ROW_OFFSET(p,code,m) ( &( (p)->map.elt[ code * (m)] )

#define MAP_LENGTH	256	/* in  server include 	*/

#ifdef RTMAP

KeySym rtmap[MAP_LENGTH*AIX_GLYPHS_PER_KEY] = {
    /* 0x00 */  NoSymbol,       NoSymbol,
    /* 0x01 */  XK_quoteleft,   XK_asciitilde,
    /* 0x02 */  XK_1,           XK_exclam,
    /* 0x03 */  XK_2,           XK_at,
    /* 0x04 */  XK_3,           XK_numbersign,
    /* 0x05 */  XK_4,           XK_dollar,
    /* 0x06 */  XK_5,           XK_percent,
    /* 0x07 */  XK_6,           XK_asciicircum,
    /* 0x08 */  XK_7,           XK_ampersand,
    /* 0x09 */  XK_8,           XK_asterisk,
    /* 0x0a */  XK_9,           XK_parenleft,
    /* 0x0b */  XK_0,           XK_parenright,
    /* 0x0c */  XK_minus,       XK_underscore,
    /* 0x0d */  XK_equal,       XK_plus,
    /* 0x0e */  NoSymbol,       NoSymbol,
    /* 0x0f */  XK_BackSpace,   NoSymbol,
    /* 0x10 */  XK_Tab,         NoSymbol,
    /* 0x11 */  XK_q,           XK_Q,
    /* 0x12 */  XK_w,           XK_W,
    /* 0x13 */  XK_e,           XK_E,
    /* 0x14 */  XK_r,           XK_R,
    /* 0x15 */  XK_t,           XK_T,
    /* 0x16 */  XK_y,           XK_Y,
    /* 0x17 */  XK_u,           XK_U,
    /* 0x18 */  XK_i,           XK_I,
    /* 0x19 */  XK_o,           XK_O,
    /* 0x1a */  XK_p,           XK_P,
    /* 0x1b */  XK_bracketleft, XK_braceleft,
    /* 0x1c */  XK_bracketright,XK_braceright,
    /* 0x1d */  XK_backslash,   XK_bar,
    /* 0x1e */  XK_Caps_Lock,   NoSymbol,
    /* 0x1f */  XK_a,           XK_A,
    /* 0x20 */  XK_s,           XK_S,
    /* 0x21 */  XK_d,           XK_D,
    /* 0x22 */  XK_f,           XK_F,
    /* 0x23 */  XK_g,           XK_G,
    /* 0x24 */  XK_h,           XK_H,
    /* 0x25 */  XK_j,           XK_J,
    /* 0x26 */  XK_k,           XK_K,
    /* 0x27 */  XK_l,           XK_L,
    /* 0x28 */  XK_semicolon,   XK_colon,
    /* 0x29 */  XK_quoteright,  XK_quotedbl,
    /* 0x2a */  XK_asterisk,    XK_mu,
    /* 0x2b */  XK_Return,      XK_Return,
    /* 0x2c */  XK_Shift_L,     NoSymbol,
    /* 0x2d */  XK_less,        XK_greater,
    /* 0x2e */  XK_z,           XK_Z,
    /* 0x2f */  XK_x,           XK_X,
    /* 0x30 */  XK_c,           XK_C,
    /* 0x31 */  XK_v,           XK_V,
    /* 0x32 */  XK_b,           XK_B,
    /* 0x33 */  XK_n,           XK_N,
    /* 0x34 */  XK_m,           XK_M,
    /* 0x35 */  XK_comma,       XK_less,
    /* 0x36 */  XK_period,      XK_greater,
    /* 0x37 */  XK_slash,       XK_question,
    /* 0x38 */  XK_underscore,  XK_underscore,
    /* 0x39 */  XK_Shift_R,     NoSymbol,
    /* 0x3a */  XK_Control_L,   NoSymbol,
    /* 0x3b */  NoSymbol,       NoSymbol,
    /* 0x3c */  XK_Alt_L,       NoSymbol,
    /* 0x3d */  XK_space,       NoSymbol,
    /* 0x3e */  XK_Alt_R,       NoSymbol,
    /* 0x3f */  NoSymbol,       NoSymbol,
    /* 0x40 */  XK_Execute,     NoSymbol,       /* Action - XXX */
    /* 0x41 */  NoSymbol,       NoSymbol,
    /* 0x42 */  NoSymbol,       NoSymbol,
    /* 0x43 */  NoSymbol,       NoSymbol,
    /* 0x44 */  NoSymbol,       NoSymbol,
    /* 0x45 */  NoSymbol,       NoSymbol,
    /* 0x46 */  NoSymbol,       NoSymbol,
    /* 0x47 */  NoSymbol,       NoSymbol,
    /* 0x48 */  NoSymbol,       NoSymbol,
    /* 0x49 */  NoSymbol,       NoSymbol,
    /* 0x4a */  NoSymbol,       NoSymbol,
    /* 0x4b */  XK_Insert,      NoSymbol,
    /* 0x4c */  XK_Delete,      NoSymbol,
    /* 0x4d */  NoSymbol,       NoSymbol,
    /* 0x4e */  NoSymbol,       NoSymbol,
    /* 0x4f */  XK_Left,        NoSymbol,
    /* 0x50 */  XK_Home,        NoSymbol,
    /* 0x51 */  XK_End,         NoSymbol,
    /* 0x52 */  NoSymbol,       NoSymbol,
    /* 0x53 */  XK_Up,          NoSymbol,       /* up arrow */
    /* 0x54 */  XK_Down,        NoSymbol,       /* down arrow */
    /* 0x55 */  XK_Prior,       NoSymbol,       /* page up */
    /* 0x56 */  XK_Next,        NoSymbol,       /* page down */
    /* 0x57 */  NoSymbol,       NoSymbol,
    /* 0x58 */  NoSymbol,       NoSymbol,
    /* 0x59 */  XK_Right,       NoSymbol,       /* right arrow */
    /* 0x5a */  XK_Num_Lock,    NoSymbol,       /* num lock */
    /* 0x5b */  XK_KP_7,        NoSymbol,
    /* 0x5c */  XK_KP_4,        NoSymbol,
    /* 0x5d */  XK_KP_1,        NoSymbol,
    /* 0x5e */  NoSymbol,       NoSymbol,
    /* 0x5f */  XK_KP_Divide,   NoSymbol,
    /* 0x60 */  XK_KP_8,        NoSymbol,
    /* 0x61 */  XK_KP_5,        NoSymbol,
    /* 0x62 */  XK_KP_2,        NoSymbol,
    /* 0x63 */  XK_KP_0,        NoSymbol,
    /* 0064 */  XK_KP_Multiply, NoSymbol,
    /* 0x65 */  XK_KP_9,        NoSymbol,
    /* 0x66 */  XK_KP_6,        NoSymbol,
    /* 0x67 */  XK_KP_3,        NoSymbol,
    /* 0x68 */  XK_KP_Decimal,  NoSymbol,
    /* 0x69 */  XK_KP_Subtract, NoSymbol,
    /* 0x6a */  XK_KP_Add,      NoSymbol,
    /* 0x6b */  NoSymbol,       NoSymbol,
    /* 0x6c */  XK_KP_Enter,    NoSymbol,
    /* 0x6d */  NoSymbol,       NoSymbol,
    /* 0x6e */  XK_Escape,      NoSymbol,
    /* 0x6f */  NoSymbol,       NoSymbol,
    /* 0x70 */  XK_F1,          NoSymbol,
    /* 0x71 */  XK_F2,          NoSymbol,
    /* 0x72 */  XK_F3,          NoSymbol,
    /* 0x73 */  XK_F4,          NoSymbol,
    /* 0x74 */  XK_F5,          NoSymbol,
    /* 0x75 */  XK_F6,          NoSymbol,
    /* 0x76 */  XK_F7,          NoSymbol,
    /* 0x77 */  XK_F8,          NoSymbol,
    /* 0x78 */  XK_F9,          NoSymbol,
    /* 0x79 */  XK_F10,         NoSymbol,
    /* 0x7a */  XK_F11,         NoSymbol,
    /* 0x7b */  XK_F12,         NoSymbol,
    /* 0x7c */  XK_Print,       NoSymbol,       /* print screen */
    /* 0x7d */  XK_Cancel,      NoSymbol,       /* scroll lock - XXX */
    /* 0x7e */  XK_Pause,       NoSymbol,       /* pause */
    /* 0x7f */  NoSymbol,       NoSymbol,
    /* 0x80 */  NoSymbol,       NoSymbol,
    /* 0x81 */  NoSymbol,       NoSymbol,
    /* 0x82 */  NoSymbol,       NoSymbol,
    /* 0x83 */  XK_Multi_key,   NoSymbol,
    /* 0x84 */  XK_Kanji,       NoSymbol,
    /* 0x85 */  XK_Mode_switch, NoSymbol
};

#endif RTMAP

#endif AIXKEYMAP_H
