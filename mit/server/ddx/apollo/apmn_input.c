/******************************************************************
Copyright 1987 by Apollo Computer Inc., Chelmsford, Massachusetts.
Cotpyright 1989 by Hewlett-Packard Company.

                        All Rights Reserved

Permission to use, duplicate, change, and distribute this software and
its documentation for any purpose and without fee is granted, provided
that the above copyright notice appear in such copy and that this
copyright notice appear in all supporting documentation, and that the
names of Apollo Computer Inc., the Hewlett-Packard Company, or MIT not
be used in advertising or publicity pertaining to distribution of the
software without written prior permission.

HEWLETT-PACKARD MAKES NO WARRANTY OF ANY KIND WITH REGARD
TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  Hewlett-Packard shall not be liable for errors
contained herein or direct, indirect, special, incidental or
consequential damages in connection with the furnishing,
performance, or use of this material.

This software is not subject to any license of the American
Telephone and Telegraph Company or of the Regents of the
University of California.
******************************************************************/

/*
 * Functions implementing Apollo-keyboard-specific parts of the driver
 * having to do with input handling.
 *
 * This code handles the multinational keyboards, that is keyboard types
 * 3A, 3B, etc.
 */

#include "apollo.h"

/* ignore whole rest of file if pre-SR10.2 */
#ifndef NO_PHYS_KEYS

#define XK_MISCELLANY
#define XK_LATIN1
#include "keysymdef.h"
#include "ap_keysym.h"

#include <apollo/kbd.h>

typedef unsigned long idm_$keymap_t[4];

typedef unsigned short aws_$kbd_modifiers_t;
typedef unsigned short aws_$kbd_modifiers_set_t;

#define  aws_$shift     0
#define  aws_$lock      1
#define  aws_$control   2
#define  aws_$mod1      3
#define  aws_$mod2      4
#define  aws_$mod3      5
#define  aws_$mod4      6
#define  aws_$mod5      7
#define  aws_$repeating 8

extern void aws_$set_modifiers (
    aws_$kbd_modifiers_t       &modf,
    gpr_$keyset_t               map,
    status_$t                  *status
);

typedef short enum {
        aws_$repeat_on,
        aws_$repeat_off,
        aws_$repeat_default
} aws_$auto_repeat_mode_t;

typedef short enum {
        aws_$delay_fast,
        aws_$delay_normal,
        aws_$delay_default
} aws_$auto_repeat_delay_t;

extern void aws_$set_auto_repeat_mode (
    aws_$auto_repeat_mode_t    &mode,
    status_$t                  *status
);

extern void aws_$get_auto_repeat_mode (
    aws_$auto_repeat_mode_t    *mode,
    status_$t                  *status
);

extern void aws_$set_auto_repeat (
    aws_$auto_repeat_mode_t    &mode,
    idm_$keymap_t               map,
    status_$t                  *status
);

extern void aws_$get_auto_repeat (
    idm_$keymap_t               map,
    status_$t                  *status
);

extern void aws_$set_auto_repeat_delay (
    aws_$auto_repeat_delay_t   &delay,
    idm_$keymap_t               map,
    status_$t                  *status
);

extern void aws_$get_auto_repeat_delay (
    aws_$auto_repeat_delay_t   &delay,
    idm_$keymap_t               map,
    status_$t                  *status
);


/*
 * Save autorepeat settings here to put them back on exit.
 */
static aws_$auto_repeat_mode_t  savedARMode;
static idm_$keymap_t            savedAR;
static idm_$keymap_t            savedARDelay;

#define N_KEY_CODES 128

/* 
 * Entry in the keyboard description table (data for initial keymap and modmap).
 */
#define MN_GLYPHS_PER_KEY 4

typedef KeySym apMNKeySymList[MN_GLYPHS_PER_KEY];

typedef struct _keyDesc
{
    int             key_type;
    apMNKeySymList  key_syms;
} keyDescRec;

/* 
 * Values for the key_type field.
 */
#define KEYT_NONEXISTENT   0
#define KEYT_EXISTS        1

/*
 * The keyboard description tables, indexed by hardware keycode (hereafter called "action").
 * These tables, of which there is one for each multinational keyboard type (and one for the
 * "unknown" case), contain inherent hardware information about key codes on the multinational
 * keyboards, and also the info for the initial contents of the keymap.
 *
 * Meaning of the key_type field:
 *      KEYT_NONEXISTENT        Multinational keyboards do not have this hardware keycode.
 *      KEYT_EXISTS             Multinational keyboards have this hardware keycode.
 *
 * Meaning of the key_syms field:
 *      A list of the keysyms initially bound to this key.
 */

static keyDescRec kbdDescTable_default[N_KEY_CODES] = {         /* default table, if keyboard type is unrecognized */
/* 0x00 = ESC  */   KEYT_EXISTS       , { XK_Escape         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x01 = L1   */   KEYT_EXISTS       , { XK_Select         , XK_Insert         , NoSymbol          , NoSymbol           },
/* 0x02 = L2   */   KEYT_EXISTS       , { apXK_LineDel      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x03 = L3   */   KEYT_EXISTS       , { apXK_CharDel      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x04 = L1A  */   KEYT_EXISTS       , { apXK_Copy         , apXK_Cut          , NoSymbol          , NoSymbol           },
/* 0x05 = L2A  */   KEYT_EXISTS       , { apXK_Paste        , XK_Undo           , NoSymbol          , NoSymbol           },
/* 0x06 = L3A  */   KEYT_EXISTS       , { apXK_Grow         , apXK_Move         , NoSymbol          , NoSymbol           },
/* 0x07 = L4   */   KEYT_EXISTS       , { apXK_LeftBar      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x08 = L5   */   KEYT_EXISTS       , { apXK_Cmd          , apXK_Shell        , NoSymbol          , NoSymbol           },
/* 0x09 = L6   */   KEYT_EXISTS       , { apXK_RightBar     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0a = L7   */   KEYT_EXISTS       , { apXK_LeftBox      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0b = L8   */   KEYT_EXISTS       , { XK_Up             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0c = L9   */   KEYT_EXISTS       , { apXK_RightBox     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0d = LA   */   KEYT_EXISTS       , { XK_Left           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0e = LB   */   KEYT_EXISTS       , { XK_Next           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0f = LC   */   KEYT_EXISTS       , { XK_Right          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x10 = LD   */   KEYT_EXISTS       , { apXK_UpBox        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x11 = LE   */   KEYT_EXISTS       , { XK_Down           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x12 = LF   */   KEYT_EXISTS       , { apXK_DownBox      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x13 = F0   */   KEYT_EXISTS       , { XK_F10            , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x14 = F1   */   KEYT_EXISTS       , { XK_F1             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x15 = F2   */   KEYT_EXISTS       , { XK_F2             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x16 = F3   */   KEYT_EXISTS       , { XK_F3             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x17 = F4   */   KEYT_EXISTS       , { XK_F4             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x18 = F5   */   KEYT_EXISTS       , { XK_F5             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x19 = F6   */   KEYT_EXISTS       , { XK_F6             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1a = F7   */   KEYT_EXISTS       , { XK_F7             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1b = F8   */   KEYT_EXISTS       , { XK_F8             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1c = F9   */   KEYT_EXISTS       , { XK_F9             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1d = R1   */   KEYT_EXISTS       , { apXK_Pop          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1e = R2   */   KEYT_EXISTS       , { XK_Redo           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1f = R3   */   KEYT_EXISTS       , { apXK_Read         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x20 = R4   */   KEYT_EXISTS       , { apXK_Edit         , apXK_Save         , NoSymbol          , NoSymbol           },
/* 0x21 = R5   */   KEYT_EXISTS       , { apXK_Exit         , XK_Cancel         , NoSymbol          , NoSymbol           },
/* 0x22 = R6   */   KEYT_EXISTS       , { XK_Pause          , XK_Help           , NoSymbol          , NoSymbol           },
/* 0x23 = NP0  */   KEYT_EXISTS       , { XK_KP_0           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x24 = NP1  */   KEYT_EXISTS       , { XK_KP_1           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x25 = NP2  */   KEYT_EXISTS       , { XK_KP_2           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x26 = NP3  */   KEYT_EXISTS       , { XK_KP_3           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x27 = NP4  */   KEYT_EXISTS       , { XK_KP_4           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x28 = NP5  */   KEYT_EXISTS       , { XK_KP_5           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x29 = NP6  */   KEYT_EXISTS       , { XK_KP_6           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2a = NP7  */   KEYT_EXISTS       , { XK_KP_7           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2b = NP8  */   KEYT_EXISTS       , { XK_KP_8           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2c = NP9  */   KEYT_EXISTS       , { XK_KP_9           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2d = NPA  */   KEYT_EXISTS       , { XK_KP_Divide      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2e = NPB  */   KEYT_EXISTS       , { XK_KP_Multiply    , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2f = NPC  */   KEYT_EXISTS       , { XK_KP_Subtract    , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x30 = NPD  */   KEYT_EXISTS       , { XK_KP_Add         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x31 = NPE  */   KEYT_EXISTS       , { XK_KP_Enter       , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x32 = NPF  */   KEYT_EXISTS       , { apXK_KP_parenright, NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x33 = NPG  */   KEYT_EXISTS       , { apXK_KP_parenleft , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x34 = NPP  */   KEYT_EXISTS       , { XK_KP_Decimal     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x35 = ALTL */   KEYT_EXISTS       , { XK_Alt_L          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x36 = ALTR */   KEYT_EXISTS       , { XK_Alt_R          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x37 = SHFL */   KEYT_EXISTS       , { XK_Shift_L        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x38 = SHFR */   KEYT_EXISTS       , { XK_Shift_R        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x39 = LOCK */   KEYT_EXISTS       , { XK_Caps_Lock      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3a = CTRL */   KEYT_EXISTS       , { XK_Control_L      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3b = REPT */   KEYT_EXISTS       , { apXK_Repeat       , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3c = TAB  */   KEYT_EXISTS       , { XK_Tab            , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3d = RET  */   KEYT_EXISTS       , { XK_Return         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3e = BS   */   KEYT_EXISTS       , { XK_BackSpace      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3f = DEL  */   KEYT_EXISTS       , { XK_Delete         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x40 = B00  */   KEYT_EXISTS       , { XK_less           , XK_greater        , NoSymbol          , NoSymbol           },
/* 0x41 = B01  */   KEYT_EXISTS       , { XK_z              , XK_Z              , NoSymbol          , NoSymbol           },
/* 0x42 = B02  */   KEYT_EXISTS       , { XK_x              , XK_X              , NoSymbol          , NoSymbol           },
/* 0x43 = B03  */   KEYT_EXISTS       , { XK_c              , XK_C              , NoSymbol          , NoSymbol           },
/* 0x44 = B04  */   KEYT_EXISTS       , { XK_v              , XK_V              , NoSymbol          , NoSymbol           },
/* 0x45 = B05  */   KEYT_EXISTS       , { XK_b              , XK_B              , NoSymbol          , NoSymbol           },
/* 0x46 = B06  */   KEYT_EXISTS       , { XK_n              , XK_N              , NoSymbol          , NoSymbol           },
/* 0x47 = B07  */   KEYT_EXISTS       , { XK_m              , XK_M              , NoSymbol          , NoSymbol           },
/* 0x48 = B08  */   KEYT_EXISTS       , { XK_comma          , XK_semicolon      , NoSymbol          , NoSymbol           },
/* 0x49 = B09  */   KEYT_EXISTS       , { XK_period         , XK_colon          , NoSymbol          , NoSymbol           },
/* 0x4a = B10  */   KEYT_EXISTS       , { XK_minus          , XK_underscore     , NoSymbol          , NoSymbol           },
/* 0x4b = B11  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4c = B12  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4d = SP   */   KEYT_EXISTS       , { XK_space          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4e = B98  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4f = B99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x50 = C00  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x51 = C01  */   KEYT_EXISTS       , { XK_a              , XK_A              , NoSymbol          , NoSymbol           },
/* 0x52 = C02  */   KEYT_EXISTS       , { XK_s              , XK_S              , NoSymbol          , NoSymbol           },
/* 0x53 = C03  */   KEYT_EXISTS       , { XK_d              , XK_D              , NoSymbol          , NoSymbol           },
/* 0x54 = C04  */   KEYT_EXISTS       , { XK_f              , XK_F              , NoSymbol          , NoSymbol           },
/* 0x55 = C05  */   KEYT_EXISTS       , { XK_g              , XK_G              , NoSymbol          , NoSymbol           },
/* 0x56 = C06  */   KEYT_EXISTS       , { XK_h              , XK_H              , NoSymbol          , NoSymbol           },
/* 0x57 = C07  */   KEYT_EXISTS       , { XK_j              , XK_J              , NoSymbol          , NoSymbol           },
/* 0x58 = C08  */   KEYT_EXISTS       , { XK_k              , XK_K              , NoSymbol          , NoSymbol           },
/* 0x59 = C09  */   KEYT_EXISTS       , { XK_l              , XK_L              , NoSymbol          , NoSymbol           },
/* 0x5a = C10  */   KEYT_EXISTS       , { XK_bar            , XK_backslash      , NoSymbol          , NoSymbol           },
/* 0x5b = C11  */   KEYT_EXISTS       , { XK_braceleft      , XK_bracketleft    , NoSymbol          , NoSymbol           },
/* 0x5c = C12  */   KEYT_EXISTS       , { XK_quoteright     , XK_asterisk       , NoSymbol          , NoSymbol           },
/* 0x5d = C13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x5e = C14  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x5f = C99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x60 = D00  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x61 = D01  */   KEYT_EXISTS       , { XK_q              , XK_Q              , NoSymbol          , NoSymbol           },
/* 0x62 = D02  */   KEYT_EXISTS       , { XK_w              , XK_W              , NoSymbol          , NoSymbol           },
/* 0x63 = D03  */   KEYT_EXISTS       , { XK_e              , XK_E              , NoSymbol          , NoSymbol           },
/* 0x64 = D04  */   KEYT_EXISTS       , { XK_r              , XK_R              , NoSymbol          , NoSymbol           },
/* 0x65 = D05  */   KEYT_EXISTS       , { XK_t              , XK_T              , NoSymbol          , NoSymbol           },
/* 0x66 = D06  */   KEYT_EXISTS       , { XK_y              , XK_Y              , NoSymbol          , NoSymbol           },
/* 0x67 = D07  */   KEYT_EXISTS       , { XK_u              , XK_U              , NoSymbol          , NoSymbol           },
/* 0x68 = D08  */   KEYT_EXISTS       , { XK_i              , XK_I              , NoSymbol          , NoSymbol           },
/* 0x69 = D09  */   KEYT_EXISTS       , { XK_o              , XK_O              , NoSymbol          , NoSymbol           },
/* 0x6a = D10  */   KEYT_EXISTS       , { XK_p              , XK_P              , NoSymbol          , NoSymbol           },
/* 0x6b = D11  */   KEYT_EXISTS       , { XK_braceright     , XK_bracketright   , NoSymbol          , NoSymbol           },
/* 0x6c = D12  */   KEYT_EXISTS       , { XK_asciitilde     , XK_asciicircum    , NoSymbol          , NoSymbol           },
/* 0x6d = D13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x6e = D98  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x6f = D99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x70 = E00  */   KEYT_EXISTS       , { XK_underscore     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x71 = E01  */   KEYT_EXISTS       , { XK_1              , XK_exclam         , NoSymbol          , NoSymbol           },
/* 0x72 = E02  */   KEYT_EXISTS       , { XK_2              , XK_quotedbl       , NoSymbol          , NoSymbol           },
/* 0x73 = E03  */   KEYT_EXISTS       , { XK_3              , XK_numbersign     , NoSymbol          , NoSymbol           },
/* 0x74 = E04  */   KEYT_EXISTS       , { XK_4              , XK_dollar         , NoSymbol          , NoSymbol           },
/* 0x75 = E05  */   KEYT_EXISTS       , { XK_5              , XK_percent        , NoSymbol          , NoSymbol           },
/* 0x76 = E06  */   KEYT_EXISTS       , { XK_6              , XK_ampersand      , NoSymbol          , NoSymbol           },
/* 0x77 = E07  */   KEYT_EXISTS       , { XK_7              , XK_slash          , NoSymbol          , NoSymbol           },
/* 0x78 = E08  */   KEYT_EXISTS       , { XK_8              , XK_parenleft      , NoSymbol          , NoSymbol           },
/* 0x79 = E09  */   KEYT_EXISTS       , { XK_9              , XK_parenright     , NoSymbol          , NoSymbol           },
/* 0x7a = E10  */   KEYT_EXISTS       , { XK_0              , XK_equal          , NoSymbol          , NoSymbol           },
/* 0x7b = E11  */   KEYT_EXISTS       , { XK_plus           , XK_question       , NoSymbol          , NoSymbol           },
/* 0x7c = E12  */   KEYT_EXISTS       , { XK_quoteleft      , XK_at             , NoSymbol          , NoSymbol           },
/* 0x7d = E13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x7e = E14  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x7f = E99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
};

static keyDescRec kbdDescTable_a[N_KEY_CODES] = {       /* German */
/* 0x00 = ESC  */   KEYT_EXISTS       , { XK_Escape         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x01 = L1   */   KEYT_EXISTS       , { XK_Select         , XK_Insert         , NoSymbol          , NoSymbol           },
/* 0x02 = L2   */   KEYT_EXISTS       , { apXK_LineDel      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x03 = L3   */   KEYT_EXISTS       , { apXK_CharDel      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x04 = L1A  */   KEYT_EXISTS       , { apXK_Copy         , apXK_Cut          , NoSymbol          , NoSymbol           },
/* 0x05 = L2A  */   KEYT_EXISTS       , { apXK_Paste        , XK_Undo           , NoSymbol          , NoSymbol           },
/* 0x06 = L3A  */   KEYT_EXISTS       , { apXK_Grow         , apXK_Move         , NoSymbol          , NoSymbol           },
/* 0x07 = L4   */   KEYT_EXISTS       , { apXK_LeftBar      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x08 = L5   */   KEYT_EXISTS       , { apXK_Cmd          , apXK_Shell        , NoSymbol          , NoSymbol           },
/* 0x09 = L6   */   KEYT_EXISTS       , { apXK_RightBar     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0a = L7   */   KEYT_EXISTS       , { apXK_LeftBox      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0b = L8   */   KEYT_EXISTS       , { XK_Up             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0c = L9   */   KEYT_EXISTS       , { apXK_RightBox     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0d = LA   */   KEYT_EXISTS       , { XK_Left           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0e = LB   */   KEYT_EXISTS       , { XK_Next           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0f = LC   */   KEYT_EXISTS       , { XK_Right          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x10 = LD   */   KEYT_EXISTS       , { apXK_UpBox        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x11 = LE   */   KEYT_EXISTS       , { XK_Down           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x12 = LF   */   KEYT_EXISTS       , { apXK_DownBox      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x13 = F0   */   KEYT_EXISTS       , { XK_F10            , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x14 = F1   */   KEYT_EXISTS       , { XK_F1             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x15 = F2   */   KEYT_EXISTS       , { XK_F2             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x16 = F3   */   KEYT_EXISTS       , { XK_F3             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x17 = F4   */   KEYT_EXISTS       , { XK_F4             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x18 = F5   */   KEYT_EXISTS       , { XK_F5             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x19 = F6   */   KEYT_EXISTS       , { XK_F6             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1a = F7   */   KEYT_EXISTS       , { XK_F7             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1b = F8   */   KEYT_EXISTS       , { XK_F8             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1c = F9   */   KEYT_EXISTS       , { XK_F9             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1d = R1   */   KEYT_EXISTS       , { apXK_Pop          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1e = R2   */   KEYT_EXISTS       , { XK_Redo           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1f = R3   */   KEYT_EXISTS       , { apXK_Read         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x20 = R4   */   KEYT_EXISTS       , { apXK_Edit         , apXK_Save         , NoSymbol          , NoSymbol           },
/* 0x21 = R5   */   KEYT_EXISTS       , { apXK_Exit         , XK_Cancel         , NoSymbol          , NoSymbol           },
/* 0x22 = R6   */   KEYT_EXISTS       , { XK_Pause          , XK_Help           , NoSymbol          , NoSymbol           },
/* 0x23 = NP0  */   KEYT_EXISTS       , { XK_KP_0           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x24 = NP1  */   KEYT_EXISTS       , { XK_KP_1           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x25 = NP2  */   KEYT_EXISTS       , { XK_KP_2           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x26 = NP3  */   KEYT_EXISTS       , { XK_KP_3           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x27 = NP4  */   KEYT_EXISTS       , { XK_KP_4           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x28 = NP5  */   KEYT_EXISTS       , { XK_KP_5           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x29 = NP6  */   KEYT_EXISTS       , { XK_KP_6           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2a = NP7  */   KEYT_EXISTS       , { XK_KP_7           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2b = NP8  */   KEYT_EXISTS       , { XK_KP_8           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2c = NP9  */   KEYT_EXISTS       , { XK_KP_9           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2d = NPA  */   KEYT_EXISTS       , { XK_KP_Divide      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2e = NPB  */   KEYT_EXISTS       , { XK_KP_Multiply    , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2f = NPC  */   KEYT_EXISTS       , { XK_KP_Subtract    , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x30 = NPD  */   KEYT_EXISTS       , { XK_KP_Add         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x31 = NPE  */   KEYT_EXISTS       , { XK_KP_Enter       , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x32 = NPF  */   KEYT_EXISTS       , { apXK_KP_parenright, NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x33 = NPG  */   KEYT_EXISTS       , { apXK_KP_parenleft , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x34 = NPP  */   KEYT_EXISTS       , { XK_KP_Decimal     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x35 = ALTL */   KEYT_EXISTS       , { XK_Alt_L          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x36 = ALTR */   KEYT_EXISTS       , { XK_Alt_R          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x37 = SHFL */   KEYT_EXISTS       , { XK_Shift_L        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x38 = SHFR */   KEYT_EXISTS       , { XK_Shift_R        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x39 = LOCK */   KEYT_EXISTS       , { XK_Caps_Lock      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3a = CTRL */   KEYT_EXISTS       , { XK_Control_L      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3b = REPT */   KEYT_EXISTS       , { apXK_Repeat       , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3c = TAB  */   KEYT_EXISTS       , { XK_Tab            , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3d = RET  */   KEYT_EXISTS       , { XK_Return         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3e = BS   */   KEYT_EXISTS       , { XK_BackSpace      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3f = DEL  */   KEYT_EXISTS       , { XK_Delete         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x40 = B00  */   KEYT_EXISTS       , { XK_less           , XK_greater        , NoSymbol          , NoSymbol           },
/* 0x41 = B01  */   KEYT_EXISTS       , { XK_y              , XK_Y              , NoSymbol          , NoSymbol           },
/* 0x42 = B02  */   KEYT_EXISTS       , { XK_x              , XK_X              , NoSymbol          , NoSymbol           },
/* 0x43 = B03  */   KEYT_EXISTS       , { XK_c              , XK_C              , NoSymbol          , NoSymbol           },
/* 0x44 = B04  */   KEYT_EXISTS       , { XK_v              , XK_V              , NoSymbol          , NoSymbol           },
/* 0x45 = B05  */   KEYT_EXISTS       , { XK_b              , XK_B              , NoSymbol          , NoSymbol           },
/* 0x46 = B06  */   KEYT_EXISTS       , { XK_n              , XK_N              , NoSymbol          , NoSymbol           },
/* 0x47 = B07  */   KEYT_EXISTS       , { XK_m              , XK_M              , NoSymbol          , NoSymbol           },
/* 0x48 = B08  */   KEYT_EXISTS       , { XK_comma          , XK_semicolon      , NoSymbol          , NoSymbol           },
/* 0x49 = B09  */   KEYT_EXISTS       , { XK_period         , XK_colon          , NoSymbol          , NoSymbol           },
/* 0x4a = B10  */   KEYT_EXISTS       , { XK_minus          , XK_underscore     , NoSymbol          , NoSymbol           },
/* 0x4b = B11  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4c = B12  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4d = SP   */   KEYT_EXISTS       , { XK_space          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4e = B98  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4f = B99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x50 = C00  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x51 = C01  */   KEYT_EXISTS       , { XK_a              , XK_A              , NoSymbol          , NoSymbol           },
/* 0x52 = C02  */   KEYT_EXISTS       , { XK_s              , XK_S              , NoSymbol          , NoSymbol           },
/* 0x53 = C03  */   KEYT_EXISTS       , { XK_d              , XK_D              , NoSymbol          , NoSymbol           },
/* 0x54 = C04  */   KEYT_EXISTS       , { XK_f              , XK_F              , NoSymbol          , NoSymbol           },
/* 0x55 = C05  */   KEYT_EXISTS       , { XK_g              , XK_G              , NoSymbol          , NoSymbol           },
/* 0x56 = C06  */   KEYT_EXISTS       , { XK_h              , XK_H              , NoSymbol          , NoSymbol           },
/* 0x57 = C07  */   KEYT_EXISTS       , { XK_j              , XK_J              , NoSymbol          , NoSymbol           },
/* 0x58 = C08  */   KEYT_EXISTS       , { XK_k              , XK_K              , NoSymbol          , NoSymbol           },
/* 0x59 = C09  */   KEYT_EXISTS       , { XK_l              , XK_L              , NoSymbol          , NoSymbol           },
/* 0x5a = C10  */   KEYT_EXISTS       , { XK_bar            , XK_backslash      , XK_odiaeresis     , XK_Odiaeresis      },
/* 0x5b = C11  */   KEYT_EXISTS       , { XK_braceleft      , XK_bracketleft    , XK_adiaeresis     , XK_Adiaeresis      },
/* 0x5c = C12  */   KEYT_EXISTS       , { XK_numbersign     , XK_asciicircum    , NoSymbol          , NoSymbol           },
/* 0x5d = C13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x5e = C14  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x5f = C99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x60 = D00  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x61 = D01  */   KEYT_EXISTS       , { XK_q              , XK_Q              , NoSymbol          , NoSymbol           },
/* 0x62 = D02  */   KEYT_EXISTS       , { XK_w              , XK_W              , NoSymbol          , NoSymbol           },
/* 0x63 = D03  */   KEYT_EXISTS       , { XK_e              , XK_E              , NoSymbol          , NoSymbol           },
/* 0x64 = D04  */   KEYT_EXISTS       , { XK_r              , XK_R              , NoSymbol          , NoSymbol           },
/* 0x65 = D05  */   KEYT_EXISTS       , { XK_t              , XK_T              , NoSymbol          , NoSymbol           },
/* 0x66 = D06  */   KEYT_EXISTS       , { XK_z              , XK_Z              , NoSymbol          , NoSymbol           },
/* 0x67 = D07  */   KEYT_EXISTS       , { XK_u              , XK_U              , NoSymbol          , NoSymbol           },
/* 0x68 = D08  */   KEYT_EXISTS       , { XK_i              , XK_I              , NoSymbol          , NoSymbol           },
/* 0x69 = D09  */   KEYT_EXISTS       , { XK_o              , XK_O              , NoSymbol          , NoSymbol           },
/* 0x6a = D10  */   KEYT_EXISTS       , { XK_p              , XK_P              , NoSymbol          , NoSymbol           },
/* 0x6b = D11  */   KEYT_EXISTS       , { XK_braceright     , XK_bracketright   , XK_udiaeresis     , XK_Udiaeresis      },
/* 0x6c = D12  */   KEYT_EXISTS       , { XK_plus           , XK_asterisk       , NoSymbol          , NoSymbol           },
/* 0x6d = D13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x6e = D98  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x6f = D99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x70 = E00  */   KEYT_EXISTS       , { XK_underscore     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x71 = E01  */   KEYT_EXISTS       , { XK_1              , XK_exclam         , NoSymbol          , NoSymbol           },
/* 0x72 = E02  */   KEYT_EXISTS       , { XK_2              , XK_quotedbl       , NoSymbol          , NoSymbol           },
/* 0x73 = E03  */   KEYT_EXISTS       , { XK_3              , XK_at             , NoSymbol          , XK_section         },
/* 0x74 = E04  */   KEYT_EXISTS       , { XK_4              , XK_dollar         , NoSymbol          , NoSymbol           },
/* 0x75 = E05  */   KEYT_EXISTS       , { XK_5              , XK_percent        , NoSymbol          , NoSymbol           },
/* 0x76 = E06  */   KEYT_EXISTS       , { XK_6              , XK_ampersand      , NoSymbol          , NoSymbol           },
/* 0x77 = E07  */   KEYT_EXISTS       , { XK_7              , XK_slash          , NoSymbol          , NoSymbol           },
/* 0x78 = E08  */   KEYT_EXISTS       , { XK_8              , XK_parenleft      , NoSymbol          , NoSymbol           },
/* 0x79 = E09  */   KEYT_EXISTS       , { XK_9              , XK_parenright     , NoSymbol          , NoSymbol           },
/* 0x7a = E10  */   KEYT_EXISTS       , { XK_0              , XK_equal          , NoSymbol          , NoSymbol           },
/* 0x7b = E11  */   KEYT_EXISTS       , { XK_asciitilde     , XK_question       , XK_ssharp         , NoSymbol           },
/* 0x7c = E12  */   KEYT_EXISTS       , { XK_quoteright     , XK_quoteleft      , NoSymbol          , NoSymbol           },
/* 0x7d = E13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x7e = E14  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x7f = E99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
};

static keyDescRec kbdDescTable_b[N_KEY_CODES] = {       /* French */
/* 0x00 = ESC  */   KEYT_EXISTS       , { XK_Escape         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x01 = L1   */   KEYT_EXISTS       , { XK_Select         , XK_Insert         , NoSymbol          , NoSymbol           },
/* 0x02 = L2   */   KEYT_EXISTS       , { apXK_LineDel      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x03 = L3   */   KEYT_EXISTS       , { apXK_CharDel      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x04 = L1A  */   KEYT_EXISTS       , { apXK_Copy         , apXK_Cut          , NoSymbol          , NoSymbol           },
/* 0x05 = L2A  */   KEYT_EXISTS       , { apXK_Paste        , XK_Undo           , NoSymbol          , NoSymbol           },
/* 0x06 = L3A  */   KEYT_EXISTS       , { apXK_Grow         , apXK_Move         , NoSymbol          , NoSymbol           },
/* 0x07 = L4   */   KEYT_EXISTS       , { apXK_LeftBar      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x08 = L5   */   KEYT_EXISTS       , { apXK_Cmd          , apXK_Shell        , NoSymbol          , NoSymbol           },
/* 0x09 = L6   */   KEYT_EXISTS       , { apXK_RightBar     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0a = L7   */   KEYT_EXISTS       , { apXK_LeftBox      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0b = L8   */   KEYT_EXISTS       , { XK_Up             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0c = L9   */   KEYT_EXISTS       , { apXK_RightBox     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0d = LA   */   KEYT_EXISTS       , { XK_Left           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0e = LB   */   KEYT_EXISTS       , { XK_Next           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0f = LC   */   KEYT_EXISTS       , { XK_Right          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x10 = LD   */   KEYT_EXISTS       , { apXK_UpBox        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x11 = LE   */   KEYT_EXISTS       , { XK_Down           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x12 = LF   */   KEYT_EXISTS       , { apXK_DownBox      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x13 = F0   */   KEYT_EXISTS       , { XK_F10            , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x14 = F1   */   KEYT_EXISTS       , { XK_F1             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x15 = F2   */   KEYT_EXISTS       , { XK_F2             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x16 = F3   */   KEYT_EXISTS       , { XK_F3             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x17 = F4   */   KEYT_EXISTS       , { XK_F4             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x18 = F5   */   KEYT_EXISTS       , { XK_F5             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x19 = F6   */   KEYT_EXISTS       , { XK_F6             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1a = F7   */   KEYT_EXISTS       , { XK_F7             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1b = F8   */   KEYT_EXISTS       , { XK_F8             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1c = F9   */   KEYT_EXISTS       , { XK_F9             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1d = R1   */   KEYT_EXISTS       , { apXK_Pop          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1e = R2   */   KEYT_EXISTS       , { XK_Redo           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1f = R3   */   KEYT_EXISTS       , { apXK_Read         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x20 = R4   */   KEYT_EXISTS       , { apXK_Edit         , apXK_Save         , NoSymbol          , NoSymbol           },
/* 0x21 = R5   */   KEYT_EXISTS       , { apXK_Exit         , XK_Cancel         , NoSymbol          , NoSymbol           },
/* 0x22 = R6   */   KEYT_EXISTS       , { XK_Pause          , XK_Help           , NoSymbol          , NoSymbol           },
/* 0x23 = NP0  */   KEYT_EXISTS       , { XK_KP_0           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x24 = NP1  */   KEYT_EXISTS       , { XK_KP_1           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x25 = NP2  */   KEYT_EXISTS       , { XK_KP_2           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x26 = NP3  */   KEYT_EXISTS       , { XK_KP_3           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x27 = NP4  */   KEYT_EXISTS       , { XK_KP_4           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x28 = NP5  */   KEYT_EXISTS       , { XK_KP_5           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x29 = NP6  */   KEYT_EXISTS       , { XK_KP_6           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2a = NP7  */   KEYT_EXISTS       , { XK_KP_7           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2b = NP8  */   KEYT_EXISTS       , { XK_KP_8           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2c = NP9  */   KEYT_EXISTS       , { XK_KP_9           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2d = NPA  */   KEYT_EXISTS       , { XK_KP_Divide      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2e = NPB  */   KEYT_EXISTS       , { XK_KP_Multiply    , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2f = NPC  */   KEYT_EXISTS       , { XK_KP_Subtract    , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x30 = NPD  */   KEYT_EXISTS       , { XK_KP_Add         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x31 = NPE  */   KEYT_EXISTS       , { XK_KP_Enter       , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x32 = NPF  */   KEYT_EXISTS       , { apXK_KP_parenright, NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x33 = NPG  */   KEYT_EXISTS       , { apXK_KP_parenleft , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x34 = NPP  */   KEYT_EXISTS       , { XK_KP_Decimal     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x35 = ALTL */   KEYT_EXISTS       , { XK_Alt_L          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x36 = ALTR */   KEYT_EXISTS       , { XK_Alt_R          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x37 = SHFL */   KEYT_EXISTS       , { XK_Shift_L        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x38 = SHFR */   KEYT_EXISTS       , { XK_Shift_R        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x39 = LOCK */   KEYT_EXISTS       , { XK_Caps_Lock      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3a = CTRL */   KEYT_EXISTS       , { XK_Control_L      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3b = REPT */   KEYT_EXISTS       , { apXK_Repeat       , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3c = TAB  */   KEYT_EXISTS       , { XK_Tab            , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3d = RET  */   KEYT_EXISTS       , { XK_Return         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3e = BS   */   KEYT_EXISTS       , { XK_BackSpace      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3f = DEL  */   KEYT_EXISTS       , { XK_Delete         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x40 = B00  */   KEYT_EXISTS       , { XK_less           , XK_greater        , NoSymbol          , NoSymbol           },
/* 0x41 = B01  */   KEYT_EXISTS       , { XK_w              , XK_W              , NoSymbol          , NoSymbol           },
/* 0x42 = B02  */   KEYT_EXISTS       , { XK_x              , XK_X              , NoSymbol          , NoSymbol           },
/* 0x43 = B03  */   KEYT_EXISTS       , { XK_c              , XK_C              , NoSymbol          , NoSymbol           },
/* 0x44 = B04  */   KEYT_EXISTS       , { XK_v              , XK_V              , NoSymbol          , NoSymbol           },
/* 0x45 = B05  */   KEYT_EXISTS       , { XK_b              , XK_B              , NoSymbol          , NoSymbol           },
/* 0x46 = B06  */   KEYT_EXISTS       , { XK_n              , XK_N              , NoSymbol          , NoSymbol           },
/* 0x47 = B07  */   KEYT_EXISTS       , { XK_comma          , XK_question       , NoSymbol          , NoSymbol           },
/* 0x48 = B08  */   KEYT_EXISTS       , { XK_semicolon      , XK_period         , NoSymbol          , NoSymbol           },
/* 0x49 = B09  */   KEYT_EXISTS       , { XK_colon          , XK_slash          , NoSymbol          , NoSymbol           },
/* 0x4a = B10  */   KEYT_EXISTS       , { XK_equal          , XK_plus           , NoSymbol          , NoSymbol           },
/* 0x4b = B11  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4c = B12  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4d = SP   */   KEYT_EXISTS       , { XK_space          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4e = B98  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4f = B99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x50 = C00  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x51 = C01  */   KEYT_EXISTS       , { XK_q              , XK_Q              , NoSymbol          , NoSymbol           },
/* 0x52 = C02  */   KEYT_EXISTS       , { XK_s              , XK_S              , NoSymbol          , NoSymbol           },
/* 0x53 = C03  */   KEYT_EXISTS       , { XK_d              , XK_D              , NoSymbol          , NoSymbol           },
/* 0x54 = C04  */   KEYT_EXISTS       , { XK_f              , XK_F              , NoSymbol          , NoSymbol           },
/* 0x55 = C05  */   KEYT_EXISTS       , { XK_g              , XK_G              , NoSymbol          , NoSymbol           },
/* 0x56 = C06  */   KEYT_EXISTS       , { XK_h              , XK_H              , NoSymbol          , NoSymbol           },
/* 0x57 = C07  */   KEYT_EXISTS       , { XK_j              , XK_J              , NoSymbol          , NoSymbol           },
/* 0x58 = C08  */   KEYT_EXISTS       , { XK_k              , XK_K              , NoSymbol          , NoSymbol           },
/* 0x59 = C09  */   KEYT_EXISTS       , { XK_l              , XK_L              , NoSymbol          , NoSymbol           },
/* 0x5a = C10  */   KEYT_EXISTS       , { XK_m              , XK_M              , NoSymbol          , NoSymbol           },
/* 0x5b = C11  */   KEYT_EXISTS       , { XK_bar            , XK_percent        , XK_ugrave         , NoSymbol           },
/* 0x5c = C12  */   KEYT_EXISTS       , { XK_quoteleft      , XK_numbersign     , NoSymbol          , XK_sterling        },
/* 0x5d = C13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x5e = C14  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x5f = C99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x60 = D00  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x61 = D01  */   KEYT_EXISTS       , { XK_a              , XK_A              , NoSymbol          , NoSymbol           },
/* 0x62 = D02  */   KEYT_EXISTS       , { XK_z              , XK_Z              , NoSymbol          , NoSymbol           },
/* 0x63 = D03  */   KEYT_EXISTS       , { XK_e              , XK_E              , NoSymbol          , NoSymbol           },
/* 0x64 = D04  */   KEYT_EXISTS       , { XK_r              , XK_R              , NoSymbol          , NoSymbol           },
/* 0x65 = D05  */   KEYT_EXISTS       , { XK_t              , XK_T              , NoSymbol          , NoSymbol           },
/* 0x66 = D06  */   KEYT_EXISTS       , { XK_y              , XK_Y              , NoSymbol          , NoSymbol           },
/* 0x67 = D07  */   KEYT_EXISTS       , { XK_u              , XK_U              , NoSymbol          , NoSymbol           },
/* 0x68 = D08  */   KEYT_EXISTS       , { XK_i              , XK_I              , NoSymbol          , NoSymbol           },
/* 0x69 = D09  */   KEYT_EXISTS       , { XK_o              , XK_O              , NoSymbol          , NoSymbol           },
/* 0x6a = D10  */   KEYT_EXISTS       , { XK_p              , XK_P              , NoSymbol          , NoSymbol           },
/* 0x6b = D11  */   KEYT_EXISTS       , { XK_asciicircum    , XK_asciitilde     , NoSymbol          , XK_diaeresis       },
/* 0x6c = D12  */   KEYT_EXISTS       , { XK_dollar         , XK_asterisk       , NoSymbol          , NoSymbol           },
/* 0x6d = D13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x6e = D98  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x6f = D99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x70 = E00  */   KEYT_EXISTS       , { XK_bracketleft    , NoSymbol          , XK_degree         , NoSymbol           },
/* 0x71 = E01  */   KEYT_EXISTS       , { XK_ampersand      , XK_1              , NoSymbol          , NoSymbol           },
/* 0x72 = E02  */   KEYT_EXISTS       , { XK_braceleft      , XK_2              , XK_eacute         , NoSymbol           },
/* 0x73 = E03  */   KEYT_EXISTS       , { XK_quotedbl       , XK_3              , NoSymbol          , NoSymbol           },
/* 0x74 = E04  */   KEYT_EXISTS       , { XK_quoteright     , XK_4              , NoSymbol          , NoSymbol           },
/* 0x75 = E05  */   KEYT_EXISTS       , { XK_parenleft      , XK_5              , NoSymbol          , NoSymbol           },
/* 0x76 = E06  */   KEYT_EXISTS       , { XK_bracketright   , XK_6              , XK_section        , NoSymbol           },
/* 0x77 = E07  */   KEYT_EXISTS       , { XK_braceright     , XK_7              , XK_egrave         , NoSymbol           },
/* 0x78 = E08  */   KEYT_EXISTS       , { XK_exclam         , XK_8              , NoSymbol          , NoSymbol           },
/* 0x79 = E09  */   KEYT_EXISTS       , { XK_backslash      , XK_9              , XK_ccedilla       , NoSymbol           },
/* 0x7a = E10  */   KEYT_EXISTS       , { XK_at             , XK_0              , XK_agrave         , NoSymbol           },
/* 0x7b = E11  */   KEYT_EXISTS       , { XK_parenright     , XK_degree         , NoSymbol          , NoSymbol           },
/* 0x7c = E12  */   KEYT_EXISTS       , { XK_minus          , XK_underscore     , NoSymbol          , NoSymbol           },
/* 0x7d = E13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x7e = E14  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x7f = E99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
};

static keyDescRec kbdDescTable_c[N_KEY_CODES] = {       /* Norwegian/Danish */
/* 0x00 = ESC  */   KEYT_EXISTS       , { XK_Escape         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x01 = L1   */   KEYT_EXISTS       , { XK_Select         , XK_Insert         , NoSymbol          , NoSymbol           },
/* 0x02 = L2   */   KEYT_EXISTS       , { apXK_LineDel      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x03 = L3   */   KEYT_EXISTS       , { apXK_CharDel      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x04 = L1A  */   KEYT_EXISTS       , { apXK_Copy         , apXK_Cut          , NoSymbol          , NoSymbol           },
/* 0x05 = L2A  */   KEYT_EXISTS       , { apXK_Paste        , XK_Undo           , NoSymbol          , NoSymbol           },
/* 0x06 = L3A  */   KEYT_EXISTS       , { apXK_Grow         , apXK_Move         , NoSymbol          , NoSymbol           },
/* 0x07 = L4   */   KEYT_EXISTS       , { apXK_LeftBar      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x08 = L5   */   KEYT_EXISTS       , { apXK_Cmd          , apXK_Shell        , NoSymbol          , NoSymbol           },
/* 0x09 = L6   */   KEYT_EXISTS       , { apXK_RightBar     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0a = L7   */   KEYT_EXISTS       , { apXK_LeftBox      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0b = L8   */   KEYT_EXISTS       , { XK_Up             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0c = L9   */   KEYT_EXISTS       , { apXK_RightBox     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0d = LA   */   KEYT_EXISTS       , { XK_Left           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0e = LB   */   KEYT_EXISTS       , { XK_Next           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0f = LC   */   KEYT_EXISTS       , { XK_Right          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x10 = LD   */   KEYT_EXISTS       , { apXK_UpBox        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x11 = LE   */   KEYT_EXISTS       , { XK_Down           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x12 = LF   */   KEYT_EXISTS       , { apXK_DownBox      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x13 = F0   */   KEYT_EXISTS       , { XK_F10            , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x14 = F1   */   KEYT_EXISTS       , { XK_F1             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x15 = F2   */   KEYT_EXISTS       , { XK_F2             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x16 = F3   */   KEYT_EXISTS       , { XK_F3             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x17 = F4   */   KEYT_EXISTS       , { XK_F4             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x18 = F5   */   KEYT_EXISTS       , { XK_F5             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x19 = F6   */   KEYT_EXISTS       , { XK_F6             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1a = F7   */   KEYT_EXISTS       , { XK_F7             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1b = F8   */   KEYT_EXISTS       , { XK_F8             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1c = F9   */   KEYT_EXISTS       , { XK_F9             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1d = R1   */   KEYT_EXISTS       , { apXK_Pop          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1e = R2   */   KEYT_EXISTS       , { XK_Redo           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1f = R3   */   KEYT_EXISTS       , { apXK_Read         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x20 = R4   */   KEYT_EXISTS       , { apXK_Edit         , apXK_Save         , NoSymbol          , NoSymbol           },
/* 0x21 = R5   */   KEYT_EXISTS       , { apXK_Exit         , XK_Cancel         , NoSymbol          , NoSymbol           },
/* 0x22 = R6   */   KEYT_EXISTS       , { XK_Pause          , XK_Help           , NoSymbol          , NoSymbol           },
/* 0x23 = NP0  */   KEYT_EXISTS       , { XK_KP_0           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x24 = NP1  */   KEYT_EXISTS       , { XK_KP_1           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x25 = NP2  */   KEYT_EXISTS       , { XK_KP_2           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x26 = NP3  */   KEYT_EXISTS       , { XK_KP_3           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x27 = NP4  */   KEYT_EXISTS       , { XK_KP_4           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x28 = NP5  */   KEYT_EXISTS       , { XK_KP_5           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x29 = NP6  */   KEYT_EXISTS       , { XK_KP_6           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2a = NP7  */   KEYT_EXISTS       , { XK_KP_7           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2b = NP8  */   KEYT_EXISTS       , { XK_KP_8           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2c = NP9  */   KEYT_EXISTS       , { XK_KP_9           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2d = NPA  */   KEYT_EXISTS       , { XK_KP_Divide      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2e = NPB  */   KEYT_EXISTS       , { XK_KP_Multiply    , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2f = NPC  */   KEYT_EXISTS       , { XK_KP_Subtract    , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x30 = NPD  */   KEYT_EXISTS       , { XK_KP_Add         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x31 = NPE  */   KEYT_EXISTS       , { XK_KP_Enter       , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x32 = NPF  */   KEYT_EXISTS       , { apXK_KP_parenright, NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x33 = NPG  */   KEYT_EXISTS       , { apXK_KP_parenleft , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x34 = NPP  */   KEYT_EXISTS       , { XK_KP_Decimal     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x35 = ALTL */   KEYT_EXISTS       , { XK_Alt_L          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x36 = ALTR */   KEYT_EXISTS       , { XK_Alt_R          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x37 = SHFL */   KEYT_EXISTS       , { XK_Shift_L        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x38 = SHFR */   KEYT_EXISTS       , { XK_Shift_R        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x39 = LOCK */   KEYT_EXISTS       , { XK_Caps_Lock      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3a = CTRL */   KEYT_EXISTS       , { XK_Control_L      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3b = REPT */   KEYT_EXISTS       , { apXK_Repeat       , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3c = TAB  */   KEYT_EXISTS       , { XK_Tab            , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3d = RET  */   KEYT_EXISTS       , { XK_Return         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3e = BS   */   KEYT_EXISTS       , { XK_BackSpace      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3f = DEL  */   KEYT_EXISTS       , { XK_Delete         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x40 = B00  */   KEYT_EXISTS       , { XK_less           , XK_greater        , NoSymbol          , NoSymbol           },
/* 0x41 = B01  */   KEYT_EXISTS       , { XK_z              , XK_Z              , NoSymbol          , NoSymbol           },
/* 0x42 = B02  */   KEYT_EXISTS       , { XK_x              , XK_X              , NoSymbol          , NoSymbol           },
/* 0x43 = B03  */   KEYT_EXISTS       , { XK_c              , XK_C              , NoSymbol          , NoSymbol           },
/* 0x44 = B04  */   KEYT_EXISTS       , { XK_v              , XK_V              , NoSymbol          , NoSymbol           },
/* 0x45 = B05  */   KEYT_EXISTS       , { XK_b              , XK_B              , NoSymbol          , NoSymbol           },
/* 0x46 = B06  */   KEYT_EXISTS       , { XK_n              , XK_N              , NoSymbol          , NoSymbol           },
/* 0x47 = B07  */   KEYT_EXISTS       , { XK_m              , XK_M              , NoSymbol          , NoSymbol           },
/* 0x48 = B08  */   KEYT_EXISTS       , { XK_comma          , XK_semicolon      , NoSymbol          , NoSymbol           },
/* 0x49 = B09  */   KEYT_EXISTS       , { XK_period         , XK_colon          , NoSymbol          , NoSymbol           },
/* 0x4a = B10  */   KEYT_EXISTS       , { XK_minus          , XK_underscore     , NoSymbol          , NoSymbol           },
/* 0x4b = B11  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4c = B12  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4d = SP   */   KEYT_EXISTS       , { XK_space          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4e = B98  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4f = B99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x50 = C00  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x51 = C01  */   KEYT_EXISTS       , { XK_a              , XK_A              , NoSymbol          , NoSymbol           },
/* 0x52 = C02  */   KEYT_EXISTS       , { XK_s              , XK_S              , NoSymbol          , NoSymbol           },
/* 0x53 = C03  */   KEYT_EXISTS       , { XK_d              , XK_D              , NoSymbol          , NoSymbol           },
/* 0x54 = C04  */   KEYT_EXISTS       , { XK_f              , XK_F              , NoSymbol          , NoSymbol           },
/* 0x55 = C05  */   KEYT_EXISTS       , { XK_g              , XK_G              , NoSymbol          , NoSymbol           },
/* 0x56 = C06  */   KEYT_EXISTS       , { XK_h              , XK_H              , NoSymbol          , NoSymbol           },
/* 0x57 = C07  */   KEYT_EXISTS       , { XK_j              , XK_J              , NoSymbol          , NoSymbol           },
/* 0x58 = C08  */   KEYT_EXISTS       , { XK_k              , XK_K              , NoSymbol          , NoSymbol           },
/* 0x59 = C09  */   KEYT_EXISTS       , { XK_l              , XK_L              , NoSymbol          , NoSymbol           },
/* 0x5a = C10  */   KEYT_EXISTS       , { XK_bar            , XK_backslash      , XK_oslash         , XK_Ooblique        },
/* 0x5b = C11  */   KEYT_EXISTS       , { XK_braceleft      , XK_bracketleft    , XK_ae             , XK_AE              },
/* 0x5c = C12  */   KEYT_EXISTS       , { XK_quoteright     , XK_asterisk       , NoSymbol          , NoSymbol           },
/* 0x5d = C13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x5e = C14  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x5f = C99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x60 = D00  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x61 = D01  */   KEYT_EXISTS       , { XK_q              , XK_Q              , NoSymbol          , NoSymbol           },
/* 0x62 = D02  */   KEYT_EXISTS       , { XK_w              , XK_W              , NoSymbol          , NoSymbol           },
/* 0x63 = D03  */   KEYT_EXISTS       , { XK_e              , XK_E              , NoSymbol          , NoSymbol           },
/* 0x64 = D04  */   KEYT_EXISTS       , { XK_r              , XK_R              , NoSymbol          , NoSymbol           },
/* 0x65 = D05  */   KEYT_EXISTS       , { XK_t              , XK_T              , NoSymbol          , NoSymbol           },
/* 0x66 = D06  */   KEYT_EXISTS       , { XK_y              , XK_Y              , NoSymbol          , NoSymbol           },
/* 0x67 = D07  */   KEYT_EXISTS       , { XK_u              , XK_U              , NoSymbol          , NoSymbol           },
/* 0x68 = D08  */   KEYT_EXISTS       , { XK_i              , XK_I              , NoSymbol          , NoSymbol           },
/* 0x69 = D09  */   KEYT_EXISTS       , { XK_o              , XK_O              , NoSymbol          , NoSymbol           },
/* 0x6a = D10  */   KEYT_EXISTS       , { XK_p              , XK_P              , NoSymbol          , NoSymbol           },
/* 0x6b = D11  */   KEYT_EXISTS       , { XK_braceright     , XK_bracketright   , XK_aring          , XK_Aring           },
/* 0x6c = D12  */   KEYT_EXISTS       , { XK_asciitilde     , XK_asciicircum    , XK_diaeresis      , NoSymbol           },
/* 0x6d = D13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x6e = D98  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x6f = D99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x70 = E00  */   KEYT_EXISTS       , { XK_underscore     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x71 = E01  */   KEYT_EXISTS       , { XK_1              , XK_exclam         , NoSymbol          , NoSymbol           },
/* 0x72 = E02  */   KEYT_EXISTS       , { XK_2              , XK_quotedbl       , NoSymbol          , NoSymbol           },
/* 0x73 = E03  */   KEYT_EXISTS       , { XK_3              , XK_numbersign     , NoSymbol          , NoSymbol           },
/* 0x74 = E04  */   KEYT_EXISTS       , { XK_4              , XK_dollar         , NoSymbol          , NoSymbol           },
/* 0x75 = E05  */   KEYT_EXISTS       , { XK_5              , XK_percent        , NoSymbol          , NoSymbol           },
/* 0x76 = E06  */   KEYT_EXISTS       , { XK_6              , XK_ampersand      , NoSymbol          , NoSymbol           },
/* 0x77 = E07  */   KEYT_EXISTS       , { XK_7              , XK_slash          , NoSymbol          , NoSymbol           },
/* 0x78 = E08  */   KEYT_EXISTS       , { XK_8              , XK_parenleft      , NoSymbol          , NoSymbol           },
/* 0x79 = E09  */   KEYT_EXISTS       , { XK_9              , XK_parenright     , NoSymbol          , NoSymbol           },
/* 0x7a = E10  */   KEYT_EXISTS       , { XK_0              , XK_equal          , NoSymbol          , NoSymbol           },
/* 0x7b = E11  */   KEYT_EXISTS       , { XK_plus           , XK_question       , NoSymbol          , NoSymbol           },
/* 0x7c = E12  */   KEYT_EXISTS       , { XK_quoteleft      , XK_at             , NoSymbol          , NoSymbol           },
/* 0x7d = E13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x7e = E14  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x7f = E99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
};

static keyDescRec kbdDescTable_d[N_KEY_CODES] = {       /* Swedish/Finnish */
/* 0x00 = ESC  */   KEYT_EXISTS       , { XK_Escape         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x01 = L1   */   KEYT_EXISTS       , { XK_Select         , XK_Insert         , NoSymbol          , NoSymbol           },
/* 0x02 = L2   */   KEYT_EXISTS       , { apXK_LineDel      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x03 = L3   */   KEYT_EXISTS       , { apXK_CharDel      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x04 = L1A  */   KEYT_EXISTS       , { apXK_Copy         , apXK_Cut          , NoSymbol          , NoSymbol           },
/* 0x05 = L2A  */   KEYT_EXISTS       , { apXK_Paste        , XK_Undo           , NoSymbol          , NoSymbol           },
/* 0x06 = L3A  */   KEYT_EXISTS       , { apXK_Grow         , apXK_Move         , NoSymbol          , NoSymbol           },
/* 0x07 = L4   */   KEYT_EXISTS       , { apXK_LeftBar      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x08 = L5   */   KEYT_EXISTS       , { apXK_Cmd          , apXK_Shell        , NoSymbol          , NoSymbol           },
/* 0x09 = L6   */   KEYT_EXISTS       , { apXK_RightBar     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0a = L7   */   KEYT_EXISTS       , { apXK_LeftBox      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0b = L8   */   KEYT_EXISTS       , { XK_Up             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0c = L9   */   KEYT_EXISTS       , { apXK_RightBox     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0d = LA   */   KEYT_EXISTS       , { XK_Left           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0e = LB   */   KEYT_EXISTS       , { XK_Next           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0f = LC   */   KEYT_EXISTS       , { XK_Right          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x10 = LD   */   KEYT_EXISTS       , { apXK_UpBox        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x11 = LE   */   KEYT_EXISTS       , { XK_Down           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x12 = LF   */   KEYT_EXISTS       , { apXK_DownBox      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x13 = F0   */   KEYT_EXISTS       , { XK_F10            , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x14 = F1   */   KEYT_EXISTS       , { XK_F1             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x15 = F2   */   KEYT_EXISTS       , { XK_F2             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x16 = F3   */   KEYT_EXISTS       , { XK_F3             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x17 = F4   */   KEYT_EXISTS       , { XK_F4             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x18 = F5   */   KEYT_EXISTS       , { XK_F5             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x19 = F6   */   KEYT_EXISTS       , { XK_F6             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1a = F7   */   KEYT_EXISTS       , { XK_F7             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1b = F8   */   KEYT_EXISTS       , { XK_F8             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1c = F9   */   KEYT_EXISTS       , { XK_F9             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1d = R1   */   KEYT_EXISTS       , { apXK_Pop          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1e = R2   */   KEYT_EXISTS       , { XK_Redo           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1f = R3   */   KEYT_EXISTS       , { apXK_Read         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x20 = R4   */   KEYT_EXISTS       , { apXK_Edit         , apXK_Save         , NoSymbol          , NoSymbol           },
/* 0x21 = R5   */   KEYT_EXISTS       , { apXK_Exit         , XK_Cancel         , NoSymbol          , NoSymbol           },
/* 0x22 = R6   */   KEYT_EXISTS       , { XK_Pause          , XK_Help           , NoSymbol          , NoSymbol           },
/* 0x23 = NP0  */   KEYT_EXISTS       , { XK_KP_0           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x24 = NP1  */   KEYT_EXISTS       , { XK_KP_1           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x25 = NP2  */   KEYT_EXISTS       , { XK_KP_2           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x26 = NP3  */   KEYT_EXISTS       , { XK_KP_3           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x27 = NP4  */   KEYT_EXISTS       , { XK_KP_4           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x28 = NP5  */   KEYT_EXISTS       , { XK_KP_5           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x29 = NP6  */   KEYT_EXISTS       , { XK_KP_6           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2a = NP7  */   KEYT_EXISTS       , { XK_KP_7           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2b = NP8  */   KEYT_EXISTS       , { XK_KP_8           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2c = NP9  */   KEYT_EXISTS       , { XK_KP_9           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2d = NPA  */   KEYT_EXISTS       , { XK_KP_Divide      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2e = NPB  */   KEYT_EXISTS       , { XK_KP_Multiply    , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2f = NPC  */   KEYT_EXISTS       , { XK_KP_Subtract    , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x30 = NPD  */   KEYT_EXISTS       , { XK_KP_Add         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x31 = NPE  */   KEYT_EXISTS       , { XK_KP_Enter       , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x32 = NPF  */   KEYT_EXISTS       , { apXK_KP_parenright, NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x33 = NPG  */   KEYT_EXISTS       , { apXK_KP_parenleft , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x34 = NPP  */   KEYT_EXISTS       , { XK_KP_Decimal     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x35 = ALTL */   KEYT_EXISTS       , { XK_Alt_L          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x36 = ALTR */   KEYT_EXISTS       , { XK_Alt_R          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x37 = SHFL */   KEYT_EXISTS       , { XK_Shift_L        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x38 = SHFR */   KEYT_EXISTS       , { XK_Shift_R        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x39 = LOCK */   KEYT_EXISTS       , { XK_Caps_Lock      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3a = CTRL */   KEYT_EXISTS       , { XK_Control_L      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3b = REPT */   KEYT_EXISTS       , { apXK_Repeat       , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3c = TAB  */   KEYT_EXISTS       , { XK_Tab            , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3d = RET  */   KEYT_EXISTS       , { XK_Return         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3e = BS   */   KEYT_EXISTS       , { XK_BackSpace      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3f = DEL  */   KEYT_EXISTS       , { XK_Delete         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x40 = B00  */   KEYT_EXISTS       , { XK_less           , XK_greater        , NoSymbol          , NoSymbol           },
/* 0x41 = B01  */   KEYT_EXISTS       , { XK_z              , XK_Z              , NoSymbol          , NoSymbol           },
/* 0x42 = B02  */   KEYT_EXISTS       , { XK_x              , XK_X              , NoSymbol          , NoSymbol           },
/* 0x43 = B03  */   KEYT_EXISTS       , { XK_c              , XK_C              , NoSymbol          , NoSymbol           },
/* 0x44 = B04  */   KEYT_EXISTS       , { XK_v              , XK_V              , NoSymbol          , NoSymbol           },
/* 0x45 = B05  */   KEYT_EXISTS       , { XK_b              , XK_B              , NoSymbol          , NoSymbol           },
/* 0x46 = B06  */   KEYT_EXISTS       , { XK_n              , XK_N              , NoSymbol          , NoSymbol           },
/* 0x47 = B07  */   KEYT_EXISTS       , { XK_m              , XK_M              , NoSymbol          , NoSymbol           },
/* 0x48 = B08  */   KEYT_EXISTS       , { XK_comma          , XK_semicolon      , NoSymbol          , NoSymbol           },
/* 0x49 = B09  */   KEYT_EXISTS       , { XK_period         , XK_colon          , NoSymbol          , NoSymbol           },
/* 0x4a = B10  */   KEYT_EXISTS       , { XK_minus          , XK_underscore     , NoSymbol          , NoSymbol           },
/* 0x4b = B11  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4c = B12  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4d = SP   */   KEYT_EXISTS       , { XK_space          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4e = B98  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4f = B99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x50 = C00  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x51 = C01  */   KEYT_EXISTS       , { XK_a              , XK_A              , NoSymbol          , NoSymbol           },
/* 0x52 = C02  */   KEYT_EXISTS       , { XK_s              , XK_S              , NoSymbol          , NoSymbol           },
/* 0x53 = C03  */   KEYT_EXISTS       , { XK_d              , XK_D              , NoSymbol          , NoSymbol           },
/* 0x54 = C04  */   KEYT_EXISTS       , { XK_f              , XK_F              , NoSymbol          , NoSymbol           },
/* 0x55 = C05  */   KEYT_EXISTS       , { XK_g              , XK_G              , NoSymbol          , NoSymbol           },
/* 0x56 = C06  */   KEYT_EXISTS       , { XK_h              , XK_H              , NoSymbol          , NoSymbol           },
/* 0x57 = C07  */   KEYT_EXISTS       , { XK_j              , XK_J              , NoSymbol          , NoSymbol           },
/* 0x58 = C08  */   KEYT_EXISTS       , { XK_k              , XK_K              , NoSymbol          , NoSymbol           },
/* 0x59 = C09  */   KEYT_EXISTS       , { XK_l              , XK_L              , NoSymbol          , NoSymbol           },
/* 0x5a = C10  */   KEYT_EXISTS       , { XK_bar            , XK_backslash      , XK_odiaeresis     , XK_Odiaeresis      },
/* 0x5b = C11  */   KEYT_EXISTS       , { XK_braceleft      , XK_bracketleft    , XK_adiaeresis     , XK_Adiaeresis      },
/* 0x5c = C12  */   KEYT_EXISTS       , { XK_quoteright     , XK_asterisk       , NoSymbol          , NoSymbol           },
/* 0x5d = C13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x5e = C14  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x5f = C99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x60 = D00  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x61 = D01  */   KEYT_EXISTS       , { XK_q              , XK_Q              , NoSymbol          , NoSymbol           },
/* 0x62 = D02  */   KEYT_EXISTS       , { XK_w              , XK_W              , NoSymbol          , NoSymbol           },
/* 0x63 = D03  */   KEYT_EXISTS       , { XK_e              , XK_E              , NoSymbol          , NoSymbol           },
/* 0x64 = D04  */   KEYT_EXISTS       , { XK_r              , XK_R              , NoSymbol          , NoSymbol           },
/* 0x65 = D05  */   KEYT_EXISTS       , { XK_t              , XK_T              , NoSymbol          , NoSymbol           },
/* 0x66 = D06  */   KEYT_EXISTS       , { XK_y              , XK_Y              , NoSymbol          , NoSymbol           },
/* 0x67 = D07  */   KEYT_EXISTS       , { XK_u              , XK_U              , NoSymbol          , NoSymbol           },
/* 0x68 = D08  */   KEYT_EXISTS       , { XK_i              , XK_I              , NoSymbol          , NoSymbol           },
/* 0x69 = D09  */   KEYT_EXISTS       , { XK_o              , XK_O              , NoSymbol          , NoSymbol           },
/* 0x6a = D10  */   KEYT_EXISTS       , { XK_p              , XK_P              , NoSymbol          , NoSymbol           },
/* 0x6b = D11  */   KEYT_EXISTS       , { XK_braceright     , XK_bracketright   , XK_aring          , XK_Aring           },
/* 0x6c = D12  */   KEYT_EXISTS       , { XK_asciitilde     , XK_asciicircum    , XK_udiaeresis     , XK_Udiaeresis      },
/* 0x6d = D13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x6e = D98  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x6f = D99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x70 = E00  */   KEYT_EXISTS       , { XK_underscore     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x71 = E01  */   KEYT_EXISTS       , { XK_1              , XK_exclam         , NoSymbol          , NoSymbol           },
/* 0x72 = E02  */   KEYT_EXISTS       , { XK_2              , XK_quotedbl       , NoSymbol          , NoSymbol           },
/* 0x73 = E03  */   KEYT_EXISTS       , { XK_3              , XK_numbersign     , NoSymbol          , NoSymbol           },
/* 0x74 = E04  */   KEYT_EXISTS       , { XK_4              , XK_dollar         , NoSymbol          , XK_currency        },
/* 0x75 = E05  */   KEYT_EXISTS       , { XK_5              , XK_percent        , NoSymbol          , NoSymbol           },
/* 0x76 = E06  */   KEYT_EXISTS       , { XK_6              , XK_ampersand      , NoSymbol          , NoSymbol           },
/* 0x77 = E07  */   KEYT_EXISTS       , { XK_7              , XK_slash          , NoSymbol          , NoSymbol           },
/* 0x78 = E08  */   KEYT_EXISTS       , { XK_8              , XK_parenleft      , NoSymbol          , NoSymbol           },
/* 0x79 = E09  */   KEYT_EXISTS       , { XK_9              , XK_parenright     , NoSymbol          , NoSymbol           },
/* 0x7a = E10  */   KEYT_EXISTS       , { XK_0              , XK_equal          , NoSymbol          , NoSymbol           },
/* 0x7b = E11  */   KEYT_EXISTS       , { XK_plus           , XK_question       , NoSymbol          , NoSymbol           },
/* 0x7c = E12  */   KEYT_EXISTS       , { XK_quoteleft      , XK_at             , XK_eacute         , XK_Eacute          },
/* 0x7d = E13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x7e = E14  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x7f = E99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
};

static keyDescRec kbdDescTable_e[N_KEY_CODES] = {       /* United Kingdom */
/* 0x00 = ESC  */   KEYT_EXISTS       , { XK_Escape         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x01 = L1   */   KEYT_EXISTS       , { XK_Select         , XK_Insert         , NoSymbol          , NoSymbol           },
/* 0x02 = L2   */   KEYT_EXISTS       , { apXK_LineDel      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x03 = L3   */   KEYT_EXISTS       , { apXK_CharDel      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x04 = L1A  */   KEYT_EXISTS       , { apXK_Copy         , apXK_Cut          , NoSymbol          , NoSymbol           },
/* 0x05 = L2A  */   KEYT_EXISTS       , { apXK_Paste        , XK_Undo           , NoSymbol          , NoSymbol           },
/* 0x06 = L3A  */   KEYT_EXISTS       , { apXK_Grow         , apXK_Move         , NoSymbol          , NoSymbol           },
/* 0x07 = L4   */   KEYT_EXISTS       , { apXK_LeftBar      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x08 = L5   */   KEYT_EXISTS       , { apXK_Cmd          , apXK_Shell        , NoSymbol          , NoSymbol           },
/* 0x09 = L6   */   KEYT_EXISTS       , { apXK_RightBar     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0a = L7   */   KEYT_EXISTS       , { apXK_LeftBox      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0b = L8   */   KEYT_EXISTS       , { XK_Up             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0c = L9   */   KEYT_EXISTS       , { apXK_RightBox     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0d = LA   */   KEYT_EXISTS       , { XK_Left           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0e = LB   */   KEYT_EXISTS       , { XK_Next           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0f = LC   */   KEYT_EXISTS       , { XK_Right          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x10 = LD   */   KEYT_EXISTS       , { apXK_UpBox        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x11 = LE   */   KEYT_EXISTS       , { XK_Down           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x12 = LF   */   KEYT_EXISTS       , { apXK_DownBox      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x13 = F0   */   KEYT_EXISTS       , { XK_F10            , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x14 = F1   */   KEYT_EXISTS       , { XK_F1             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x15 = F2   */   KEYT_EXISTS       , { XK_F2             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x16 = F3   */   KEYT_EXISTS       , { XK_F3             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x17 = F4   */   KEYT_EXISTS       , { XK_F4             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x18 = F5   */   KEYT_EXISTS       , { XK_F5             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x19 = F6   */   KEYT_EXISTS       , { XK_F6             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1a = F7   */   KEYT_EXISTS       , { XK_F7             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1b = F8   */   KEYT_EXISTS       , { XK_F8             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1c = F9   */   KEYT_EXISTS       , { XK_F9             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1d = R1   */   KEYT_EXISTS       , { apXK_Pop          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1e = R2   */   KEYT_EXISTS       , { XK_Redo           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1f = R3   */   KEYT_EXISTS       , { apXK_Read         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x20 = R4   */   KEYT_EXISTS       , { apXK_Edit         , apXK_Save         , NoSymbol          , NoSymbol           },
/* 0x21 = R5   */   KEYT_EXISTS       , { apXK_Exit         , XK_Cancel         , NoSymbol          , NoSymbol           },
/* 0x22 = R6   */   KEYT_EXISTS       , { XK_Pause          , XK_Help           , NoSymbol          , NoSymbol           },
/* 0x23 = NP0  */   KEYT_EXISTS       , { XK_KP_0           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x24 = NP1  */   KEYT_EXISTS       , { XK_KP_1           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x25 = NP2  */   KEYT_EXISTS       , { XK_KP_2           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x26 = NP3  */   KEYT_EXISTS       , { XK_KP_3           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x27 = NP4  */   KEYT_EXISTS       , { XK_KP_4           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x28 = NP5  */   KEYT_EXISTS       , { XK_KP_5           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x29 = NP6  */   KEYT_EXISTS       , { XK_KP_6           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2a = NP7  */   KEYT_EXISTS       , { XK_KP_7           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2b = NP8  */   KEYT_EXISTS       , { XK_KP_8           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2c = NP9  */   KEYT_EXISTS       , { XK_KP_9           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2d = NPA  */   KEYT_EXISTS       , { XK_KP_Divide      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2e = NPB  */   KEYT_EXISTS       , { XK_KP_Multiply    , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2f = NPC  */   KEYT_EXISTS       , { XK_KP_Subtract    , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x30 = NPD  */   KEYT_EXISTS       , { XK_KP_Add         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x31 = NPE  */   KEYT_EXISTS       , { XK_KP_Enter       , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x32 = NPF  */   KEYT_EXISTS       , { apXK_KP_parenright, NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x33 = NPG  */   KEYT_EXISTS       , { apXK_KP_parenleft , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x34 = NPP  */   KEYT_EXISTS       , { XK_KP_Decimal     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x35 = ALTL */   KEYT_EXISTS       , { XK_Alt_L          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x36 = ALTR */   KEYT_EXISTS       , { XK_Alt_R          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x37 = SHFL */   KEYT_EXISTS       , { XK_Shift_L        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x38 = SHFR */   KEYT_EXISTS       , { XK_Shift_R        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x39 = LOCK */   KEYT_EXISTS       , { XK_Caps_Lock      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3a = CTRL */   KEYT_EXISTS       , { XK_Control_L      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3b = REPT */   KEYT_EXISTS       , { apXK_Repeat       , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3c = TAB  */   KEYT_EXISTS       , { XK_Tab            , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3d = RET  */   KEYT_EXISTS       , { XK_Return         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3e = BS   */   KEYT_EXISTS       , { XK_BackSpace      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3f = DEL  */   KEYT_EXISTS       , { XK_Delete         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x40 = B00  */   KEYT_EXISTS       , { XK_backslash      , XK_bar            , NoSymbol          , NoSymbol           },
/* 0x41 = B01  */   KEYT_EXISTS       , { XK_z              , XK_Z              , NoSymbol          , NoSymbol           },
/* 0x42 = B02  */   KEYT_EXISTS       , { XK_x              , XK_X              , NoSymbol          , NoSymbol           },
/* 0x43 = B03  */   KEYT_EXISTS       , { XK_c              , XK_C              , NoSymbol          , NoSymbol           },
/* 0x44 = B04  */   KEYT_EXISTS       , { XK_v              , XK_V              , NoSymbol          , NoSymbol           },
/* 0x45 = B05  */   KEYT_EXISTS       , { XK_b              , XK_B              , NoSymbol          , NoSymbol           },
/* 0x46 = B06  */   KEYT_EXISTS       , { XK_n              , XK_N              , NoSymbol          , NoSymbol           },
/* 0x47 = B07  */   KEYT_EXISTS       , { XK_m              , XK_M              , NoSymbol          , NoSymbol           },
/* 0x48 = B08  */   KEYT_EXISTS       , { XK_comma          , XK_less           , NoSymbol          , NoSymbol           },
/* 0x49 = B09  */   KEYT_EXISTS       , { XK_period         , XK_greater        , NoSymbol          , NoSymbol           },
/* 0x4a = B10  */   KEYT_EXISTS       , { XK_slash          , XK_question       , NoSymbol          , NoSymbol           },
/* 0x4b = B11  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4c = B12  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4d = SP   */   KEYT_EXISTS       , { XK_space          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4e = B98  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4f = B99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x50 = C00  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x51 = C01  */   KEYT_EXISTS       , { XK_a              , XK_A              , NoSymbol          , NoSymbol           },
/* 0x52 = C02  */   KEYT_EXISTS       , { XK_s              , XK_S              , NoSymbol          , NoSymbol           },
/* 0x53 = C03  */   KEYT_EXISTS       , { XK_d              , XK_D              , NoSymbol          , NoSymbol           },
/* 0x54 = C04  */   KEYT_EXISTS       , { XK_f              , XK_F              , NoSymbol          , NoSymbol           },
/* 0x55 = C05  */   KEYT_EXISTS       , { XK_g              , XK_G              , NoSymbol          , NoSymbol           },
/* 0x56 = C06  */   KEYT_EXISTS       , { XK_h              , XK_H              , NoSymbol          , NoSymbol           },
/* 0x57 = C07  */   KEYT_EXISTS       , { XK_j              , XK_J              , NoSymbol          , NoSymbol           },
/* 0x58 = C08  */   KEYT_EXISTS       , { XK_k              , XK_K              , NoSymbol          , NoSymbol           },
/* 0x59 = C09  */   KEYT_EXISTS       , { XK_l              , XK_L              , NoSymbol          , NoSymbol           },
/* 0x5a = C10  */   KEYT_EXISTS       , { XK_semicolon      , XK_plus           , NoSymbol          , NoSymbol           },
/* 0x5b = C11  */   KEYT_EXISTS       , { XK_colon          , XK_asterisk       , NoSymbol          , NoSymbol           },
/* 0x5c = C12  */   KEYT_EXISTS       , { XK_bracketright   , XK_braceright     , NoSymbol          , NoSymbol           },
/* 0x5d = C13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x5e = C14  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x5f = C99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x60 = D00  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x61 = D01  */   KEYT_EXISTS       , { XK_q              , XK_Q              , NoSymbol          , NoSymbol           },
/* 0x62 = D02  */   KEYT_EXISTS       , { XK_w              , XK_W              , NoSymbol          , NoSymbol           },
/* 0x63 = D03  */   KEYT_EXISTS       , { XK_e              , XK_E              , NoSymbol          , NoSymbol           },
/* 0x64 = D04  */   KEYT_EXISTS       , { XK_r              , XK_R              , NoSymbol          , NoSymbol           },
/* 0x65 = D05  */   KEYT_EXISTS       , { XK_t              , XK_T              , NoSymbol          , NoSymbol           },
/* 0x66 = D06  */   KEYT_EXISTS       , { XK_y              , XK_Y              , NoSymbol          , NoSymbol           },
/* 0x67 = D07  */   KEYT_EXISTS       , { XK_u              , XK_U              , NoSymbol          , NoSymbol           },
/* 0x68 = D08  */   KEYT_EXISTS       , { XK_i              , XK_I              , NoSymbol          , NoSymbol           },
/* 0x69 = D09  */   KEYT_EXISTS       , { XK_o              , XK_O              , NoSymbol          , NoSymbol           },
/* 0x6a = D10  */   KEYT_EXISTS       , { XK_p              , XK_P              , NoSymbol          , NoSymbol           },
/* 0x6b = D11  */   KEYT_EXISTS       , { XK_at             , XK_quoteleft      , NoSymbol          , NoSymbol           },
/* 0x6c = D12  */   KEYT_EXISTS       , { XK_bracketleft    , XK_braceleft      , NoSymbol          , NoSymbol           },
/* 0x6d = D13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x6e = D98  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x6f = D99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x70 = E00  */   KEYT_EXISTS       , { XK_underscore     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x71 = E01  */   KEYT_EXISTS       , { XK_1              , XK_exclam         , NoSymbol          , NoSymbol           },
/* 0x72 = E02  */   KEYT_EXISTS       , { XK_2              , XK_quotedbl       , NoSymbol          , NoSymbol           },
/* 0x73 = E03  */   KEYT_EXISTS       , { XK_3              , XK_numbersign     , NoSymbol          , XK_sterling        },
/* 0x74 = E04  */   KEYT_EXISTS       , { XK_4              , XK_dollar         , NoSymbol          , NoSymbol           },
/* 0x75 = E05  */   KEYT_EXISTS       , { XK_5              , XK_percent        , NoSymbol          , NoSymbol           },
/* 0x76 = E06  */   KEYT_EXISTS       , { XK_6              , XK_ampersand      , NoSymbol          , NoSymbol           },
/* 0x77 = E07  */   KEYT_EXISTS       , { XK_7              , XK_quoteright     , NoSymbol          , NoSymbol           },
/* 0x78 = E08  */   KEYT_EXISTS       , { XK_8              , XK_parenleft      , NoSymbol          , NoSymbol           },
/* 0x79 = E09  */   KEYT_EXISTS       , { XK_9              , XK_parenright     , NoSymbol          , NoSymbol           },
/* 0x7a = E10  */   KEYT_EXISTS       , { XK_0              , XK_underscore     , NoSymbol          , NoSymbol           },
/* 0x7b = E11  */   KEYT_EXISTS       , { XK_minus          , XK_equal          , NoSymbol          , NoSymbol           },
/* 0x7c = E12  */   KEYT_EXISTS       , { XK_asciicircum    , XK_asciitilde     , NoSymbol          , NoSymbol           },
/* 0x7d = E13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x7e = E14  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x7f = E99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
};

static keyDescRec kbdDescTable_g[N_KEY_CODES] = {       /* Swiss */
/* 0x00 = ESC  */   KEYT_EXISTS       , { XK_Escape         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x01 = L1   */   KEYT_EXISTS       , { XK_Select         , XK_Insert         , NoSymbol          , NoSymbol           },
/* 0x02 = L2   */   KEYT_EXISTS       , { apXK_LineDel      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x03 = L3   */   KEYT_EXISTS       , { apXK_CharDel      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x04 = L1A  */   KEYT_EXISTS       , { apXK_Copy         , apXK_Cut          , NoSymbol          , NoSymbol           },
/* 0x05 = L2A  */   KEYT_EXISTS       , { apXK_Paste        , XK_Undo           , NoSymbol          , NoSymbol           },
/* 0x06 = L3A  */   KEYT_EXISTS       , { apXK_Grow         , apXK_Move         , NoSymbol          , NoSymbol           },
/* 0x07 = L4   */   KEYT_EXISTS       , { apXK_LeftBar      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x08 = L5   */   KEYT_EXISTS       , { apXK_Cmd          , apXK_Shell        , NoSymbol          , NoSymbol           },
/* 0x09 = L6   */   KEYT_EXISTS       , { apXK_RightBar     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0a = L7   */   KEYT_EXISTS       , { apXK_LeftBox      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0b = L8   */   KEYT_EXISTS       , { XK_Up             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0c = L9   */   KEYT_EXISTS       , { apXK_RightBox     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0d = LA   */   KEYT_EXISTS       , { XK_Left           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0e = LB   */   KEYT_EXISTS       , { XK_Next           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x0f = LC   */   KEYT_EXISTS       , { XK_Right          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x10 = LD   */   KEYT_EXISTS       , { apXK_UpBox        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x11 = LE   */   KEYT_EXISTS       , { XK_Down           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x12 = LF   */   KEYT_EXISTS       , { apXK_DownBox      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x13 = F0   */   KEYT_EXISTS       , { XK_F10            , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x14 = F1   */   KEYT_EXISTS       , { XK_F1             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x15 = F2   */   KEYT_EXISTS       , { XK_F2             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x16 = F3   */   KEYT_EXISTS       , { XK_F3             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x17 = F4   */   KEYT_EXISTS       , { XK_F4             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x18 = F5   */   KEYT_EXISTS       , { XK_F5             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x19 = F6   */   KEYT_EXISTS       , { XK_F6             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1a = F7   */   KEYT_EXISTS       , { XK_F7             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1b = F8   */   KEYT_EXISTS       , { XK_F8             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1c = F9   */   KEYT_EXISTS       , { XK_F9             , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1d = R1   */   KEYT_EXISTS       , { apXK_Pop          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1e = R2   */   KEYT_EXISTS       , { XK_Redo           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x1f = R3   */   KEYT_EXISTS       , { apXK_Read         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x20 = R4   */   KEYT_EXISTS       , { apXK_Edit         , apXK_Save         , NoSymbol          , NoSymbol           },
/* 0x21 = R5   */   KEYT_EXISTS       , { apXK_Exit         , XK_Cancel         , NoSymbol          , NoSymbol           },
/* 0x22 = R6   */   KEYT_EXISTS       , { XK_Pause          , XK_Help           , NoSymbol          , NoSymbol           },
/* 0x23 = NP0  */   KEYT_EXISTS       , { XK_KP_0           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x24 = NP1  */   KEYT_EXISTS       , { XK_KP_1           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x25 = NP2  */   KEYT_EXISTS       , { XK_KP_2           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x26 = NP3  */   KEYT_EXISTS       , { XK_KP_3           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x27 = NP4  */   KEYT_EXISTS       , { XK_KP_4           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x28 = NP5  */   KEYT_EXISTS       , { XK_KP_5           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x29 = NP6  */   KEYT_EXISTS       , { XK_KP_6           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2a = NP7  */   KEYT_EXISTS       , { XK_KP_7           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2b = NP8  */   KEYT_EXISTS       , { XK_KP_8           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2c = NP9  */   KEYT_EXISTS       , { XK_KP_9           , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2d = NPA  */   KEYT_EXISTS       , { XK_KP_Divide      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2e = NPB  */   KEYT_EXISTS       , { XK_KP_Multiply    , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x2f = NPC  */   KEYT_EXISTS       , { XK_KP_Subtract    , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x30 = NPD  */   KEYT_EXISTS       , { XK_KP_Add         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x31 = NPE  */   KEYT_EXISTS       , { XK_KP_Enter       , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x32 = NPF  */   KEYT_EXISTS       , { apXK_KP_parenright, NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x33 = NPG  */   KEYT_EXISTS       , { apXK_KP_parenleft , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x34 = NPP  */   KEYT_EXISTS       , { XK_KP_Decimal     , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x35 = ALTL */   KEYT_EXISTS       , { XK_Alt_L          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x36 = ALTR */   KEYT_EXISTS       , { XK_Alt_R          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x37 = SHFL */   KEYT_EXISTS       , { XK_Shift_L        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x38 = SHFR */   KEYT_EXISTS       , { XK_Shift_R        , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x39 = LOCK */   KEYT_EXISTS       , { XK_Caps_Lock      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3a = CTRL */   KEYT_EXISTS       , { XK_Control_L      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3b = REPT */   KEYT_EXISTS       , { apXK_Repeat       , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3c = TAB  */   KEYT_EXISTS       , { XK_Tab            , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3d = RET  */   KEYT_EXISTS       , { XK_Return         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3e = BS   */   KEYT_EXISTS       , { XK_BackSpace      , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x3f = DEL  */   KEYT_EXISTS       , { XK_Delete         , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x40 = B00  */   KEYT_EXISTS       , { XK_less           , XK_greater        , NoSymbol          , NoSymbol           },
/* 0x41 = B01  */   KEYT_EXISTS       , { XK_y              , XK_Y              , NoSymbol          , NoSymbol           },
/* 0x42 = B02  */   KEYT_EXISTS       , { XK_x              , XK_X              , NoSymbol          , NoSymbol           },
/* 0x43 = B03  */   KEYT_EXISTS       , { XK_c              , XK_C              , NoSymbol          , NoSymbol           },
/* 0x44 = B04  */   KEYT_EXISTS       , { XK_v              , XK_V              , NoSymbol          , NoSymbol           },
/* 0x45 = B05  */   KEYT_EXISTS       , { XK_b              , XK_B              , NoSymbol          , NoSymbol           },
/* 0x46 = B06  */   KEYT_EXISTS       , { XK_n              , XK_N              , NoSymbol          , NoSymbol           },
/* 0x47 = B07  */   KEYT_EXISTS       , { XK_m              , XK_M              , NoSymbol          , NoSymbol           },
/* 0x48 = B08  */   KEYT_EXISTS       , { XK_comma          , XK_semicolon      , NoSymbol          , NoSymbol           },
/* 0x49 = B09  */   KEYT_EXISTS       , { XK_period         , XK_colon          , NoSymbol          , NoSymbol           },
/* 0x4a = B10  */   KEYT_EXISTS       , { XK_minus          , XK_underscore     , NoSymbol          , NoSymbol           },
/* 0x4b = B11  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4c = B12  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4d = SP   */   KEYT_EXISTS       , { XK_space          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4e = B98  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x4f = B99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x50 = C00  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x51 = C01  */   KEYT_EXISTS       , { XK_a              , XK_A              , NoSymbol          , NoSymbol           },
/* 0x52 = C02  */   KEYT_EXISTS       , { XK_s              , XK_S              , NoSymbol          , NoSymbol           },
/* 0x53 = C03  */   KEYT_EXISTS       , { XK_d              , XK_D              , NoSymbol          , NoSymbol           },
/* 0x54 = C04  */   KEYT_EXISTS       , { XK_f              , XK_F              , NoSymbol          , NoSymbol           },
/* 0x55 = C05  */   KEYT_EXISTS       , { XK_g              , XK_G              , NoSymbol          , NoSymbol           },
/* 0x56 = C06  */   KEYT_EXISTS       , { XK_h              , XK_H              , NoSymbol          , NoSymbol           },
/* 0x57 = C07  */   KEYT_EXISTS       , { XK_j              , XK_J              , NoSymbol          , NoSymbol           },
/* 0x58 = C08  */   KEYT_EXISTS       , { XK_k              , XK_K              , NoSymbol          , NoSymbol           },
/* 0x59 = C09  */   KEYT_EXISTS       , { XK_l              , XK_L              , NoSymbol          , NoSymbol           },
/* 0x5a = C10  */   KEYT_EXISTS       , { XK_bar            , XK_bracketleft    , XK_odiaeresis     , XK_eacute          },
/* 0x5b = C11  */   KEYT_EXISTS       , { XK_braceleft      , XK_bracketright   , XK_adiaeresis     , XK_agrave          },
/* 0x5c = C12  */   KEYT_EXISTS       , { XK_dollar         , XK_numbersign     , NoSymbol          , XK_sterling        },
/* 0x5d = C13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x5e = C14  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x5f = C99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x60 = D00  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x61 = D01  */   KEYT_EXISTS       , { XK_q              , XK_Q              , NoSymbol          , NoSymbol           },
/* 0x62 = D02  */   KEYT_EXISTS       , { XK_w              , XK_W              , NoSymbol          , NoSymbol           },
/* 0x63 = D03  */   KEYT_EXISTS       , { XK_e              , XK_E              , NoSymbol          , NoSymbol           },
/* 0x64 = D04  */   KEYT_EXISTS       , { XK_r              , XK_R              , NoSymbol          , NoSymbol           },
/* 0x65 = D05  */   KEYT_EXISTS       , { XK_t              , XK_T              , NoSymbol          , NoSymbol           },
/* 0x66 = D06  */   KEYT_EXISTS       , { XK_z              , XK_Z              , NoSymbol          , NoSymbol           },
/* 0x67 = D07  */   KEYT_EXISTS       , { XK_u              , XK_U              , NoSymbol          , NoSymbol           },
/* 0x68 = D08  */   KEYT_EXISTS       , { XK_i              , XK_I              , NoSymbol          , NoSymbol           },
/* 0x69 = D09  */   KEYT_EXISTS       , { XK_o              , XK_O              , NoSymbol          , NoSymbol           },
/* 0x6a = D10  */   KEYT_EXISTS       , { XK_p              , XK_P              , NoSymbol          , NoSymbol           },
/* 0x6b = D11  */   KEYT_EXISTS       , { XK_braceright     , XK_asciitilde     , XK_udiaeresis     , XK_egrave          },
/* 0x6c = D12  */   KEYT_EXISTS       , { XK_diaeresis      , XK_acute          , NoSymbol          , NoSymbol           },
/* 0x6d = D13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x6e = D98  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x6f = D99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x70 = E00  */   KEYT_EXISTS       , { XK_at             , XK_exclam         , XK_section        , NoSymbol           },
/* 0x71 = E01  */   KEYT_EXISTS       , { XK_1              , XK_plus           , NoSymbol          , NoSymbol           },
/* 0x72 = E02  */   KEYT_EXISTS       , { XK_2              , XK_quotedbl       , NoSymbol          , NoSymbol           },
/* 0x73 = E03  */   KEYT_EXISTS       , { XK_3              , XK_asterisk       , NoSymbol          , NoSymbol           },
/* 0x74 = E04  */   KEYT_EXISTS       , { XK_4              , XK_backslash      , NoSymbol          , XK_ccedilla        },
/* 0x75 = E05  */   KEYT_EXISTS       , { XK_5              , XK_percent        , NoSymbol          , NoSymbol           },
/* 0x76 = E06  */   KEYT_EXISTS       , { XK_6              , XK_ampersand      , NoSymbol          , NoSymbol           },
/* 0x77 = E07  */   KEYT_EXISTS       , { XK_7              , XK_slash          , NoSymbol          , NoSymbol           },
/* 0x78 = E08  */   KEYT_EXISTS       , { XK_8              , XK_parenleft      , NoSymbol          , NoSymbol           },
/* 0x79 = E09  */   KEYT_EXISTS       , { XK_9              , XK_parenright     , NoSymbol          , NoSymbol           },
/* 0x7a = E10  */   KEYT_EXISTS       , { XK_0              , XK_equal          , NoSymbol          , NoSymbol           },
/* 0x7b = E11  */   KEYT_EXISTS       , { XK_quoteright     , XK_question       , NoSymbol          , NoSymbol           },
/* 0x7c = E12  */   KEYT_EXISTS       , { XK_asciicircum    , XK_quoteleft      , NoSymbol          , NoSymbol           },
/* 0x7d = E13  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x7e = E14  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
/* 0x7f = E99  */   KEYT_NONEXISTENT  , { NoSymbol          , NoSymbol          , NoSymbol          , NoSymbol           },
};


/*
 * Pointer to the proper table for the actual keyboard in use.
 */
static keyDescRec *pKbdDescTable;

/*
 * Table describing what keys exist for the server (may differ between keyboards).
 * Indexed by action, as above.
 */
static Bool keyExists[N_KEY_CODES];
static idm_$keymap_t    idmKeyset;

/*
 * Macros translating between "actions" (range 0-127) and protocol keysyms
 * (range 128-255).
 */
#define AP_ACTION_TO_KEYCODE(action) ((action)+128)
#define AP_KEYCODE_TO_ACTION(keycode) ((keycode)-128)


/*
 * apMNLegalModifier -- Driver internal code
 *      This is the implementation of LegalModifier for multinational kbds.
 */
Bool
apMNLegalModifier(keycode)
    BYTE keycode;
{
    if (!keyExists[AP_KEYCODE_TO_ACTION(keycode)])
        return FALSE;

    return TRUE;
}

/*
 * apMNHWInitKbd -- Driver internal code
 *      Perform all "hardware" operations needed to initialize keyboard.
 *      Input argument is the Apollo keyboard type code (as a lowercase
 *      alphabetic character).
 */
void
apMNHWInitKbd (kbd_subtype)
    char        kbd_subtype;
{
    int             action, kt;
    KeySym          ks;
    gpr_$keyset_t   gprKeyset;
    idm_$keymap_t   idmKeysetNull;
    status_$t       status;

    switch (kbd_subtype)
    {
    case 'a':
        pKbdDescTable = kbdDescTable_a;
        break;
    case 'b':
        pKbdDescTable = kbdDescTable_b;
        break;
    case 'c':
        pKbdDescTable = kbdDescTable_c;
        break;
    case 'd':
        pKbdDescTable = kbdDescTable_d;
        break;
    case 'e':
        pKbdDescTable = kbdDescTable_e;
        break;
    case 'g':
        pKbdDescTable = kbdDescTable_g;
        break;
    default:
        pKbdDescTable = kbdDescTable_default;
        break;
    }

    for (action = 0 ; action < N_KEY_CODES ; action++)
    {
        kt = pKbdDescTable[action].key_type;
        keyExists[action] = (kt == KEYT_EXISTS);
    }

    /*
     * Workaround for bug in beta versions of SR10.2 (fixed in release version):
     *
     * We must enable at least one other key-type event for the
     * gpr_$physical_keys to work.  Since physical_keys events take precedence
     * over function_keys events, the function_keys event here will never be
     * delivered.
     */
    lib_$init_set (gprKeyset, sizeof(gprKeyset)*8);
    lib_$add_to_set (gprKeyset, sizeof(gprKeyset)*8, (short) KBD3_$ESC);
    gpr_$enable_input (gpr_$function_keys, gprKeyset, &status);

    lib_$init_set (gprKeyset, sizeof(gprKeyset)*8);
    lib_$init_set (idmKeyset, sizeof(idmKeyset)*8);
    for (action = 0 ; action < N_KEY_CODES ; action++)
        if (keyExists[action])
        {
            lib_$add_to_set (gprKeyset, sizeof(gprKeyset)*8, (short) action);
            lib_$add_to_set (gprKeyset, sizeof(gprKeyset)*8, (short) (action + KBD3_$KEY_UP));
            lib_$add_to_set (idmKeyset, sizeof(idmKeyset)*8, (short) action);
        }
    gpr_$enable_input (gpr_$physical_keys, gprKeyset, &status);

    aws_$get_auto_repeat_mode (&savedARMode, &status);
    aws_$get_auto_repeat (savedAR, &status);
    aws_$get_auto_repeat_delay (aws_$delay_fast, savedARDelay, &status);

    aws_$set_auto_repeat_delay (aws_$delay_normal, idmKeyset, &status);

    lib_$init_set (idmKeysetNull, sizeof(idmKeysetNull)*8);
    aws_$set_modifiers (aws_$shift, idmKeysetNull, &status);
    aws_$set_modifiers (aws_$lock, idmKeysetNull, &status);
    aws_$set_modifiers (aws_$control, idmKeysetNull, &status);
    aws_$set_modifiers (aws_$mod1, idmKeysetNull, &status);
    aws_$set_modifiers (aws_$mod2, idmKeysetNull, &status);
    aws_$set_modifiers (aws_$mod3, idmKeysetNull, &status);
    aws_$set_modifiers (aws_$mod4, idmKeysetNull, &status);
    aws_$set_modifiers (aws_$mod5, idmKeysetNull, &status);
    aws_$set_modifiers (aws_$repeating, idmKeysetNull, &status);

#ifdef NO_GPR_QUIT
    smd_$set_quit_char((char) 0, &status);
#else
    gpr_$set_quit_event(gpr_$no_event, 0, &status);
#endif
}

/*
 * apMNHWCloseKbd -- Driver internal code
 *      Perform all "hardware" operations needed to close down keyboard.
 *      In this case, put back autorepeat as we found it (DM should do this).
 */
void
apMNHWCloseKbd()
{
    status_$t   status;

    aws_$set_auto_repeat_mode (savedARMode, &status);
    aws_$set_auto_repeat (aws_$repeat_off, idmKeyset, &status);
    aws_$set_auto_repeat (aws_$repeat_on, savedAR, &status);
    aws_$set_auto_repeat_delay (aws_$delay_fast, savedARDelay, &status);
}

/*
 * apMNGetMappings -- Driver internal code
 *      This code creates the default keymap for multinational keyboards.
 */
Bool
apMNGetMappings(pKeySyms, pModMap)
    KeySymsPtr  pKeySyms;
    CARD8      *pModMap;
{
    int             action, min_action, max_action;
    int             kc, min_kc, max_kc, i;
    apMNKeySymList *map;

    min_action = N_KEY_CODES;
    max_action = 0;
    for (action = 0 ; action < N_KEY_CODES ; action++)
        if (keyExists[action])
        {
            max_action = action;
            if (min_action > action) min_action = action;
        }

    map = (apMNKeySymList *) xalloc (sizeof(apMNKeySymList) * MAP_LENGTH);
    if (!map)
        return FALSE;

    pKeySyms->minKeyCode = min_kc = AP_ACTION_TO_KEYCODE(min_action);
    pKeySyms->maxKeyCode = max_kc = AP_ACTION_TO_KEYCODE(max_action);
    pKeySyms->mapWidth = MN_GLYPHS_PER_KEY;
    pKeySyms->map = (KeySym *) map;

    for (kc = 0; kc < MAP_LENGTH; kc++)
    {
        for (i = 0 ; i < MN_GLYPHS_PER_KEY ; i++)
            map[kc][i] = NoSymbol;
    }

    for (action = min_action ; action <= max_action ; action++)
        if (keyExists[action])
        {
            for (i = 0 ; i < MN_GLYPHS_PER_KEY ; i++)
                map[AP_ACTION_TO_KEYCODE(action) - min_kc][i] = pKbdDescTable[action].key_syms[i];
        }

    for (kc = 0; kc < MAP_LENGTH; kc++)
    {
        if ((kc < min_kc) || (kc > max_kc))
            pModMap[kc] = NoSymbol;
        else
            switch (map[kc - min_kc][0])
            {
            case XK_Shift_L:
            case XK_Shift_R:
                pModMap[kc] = ShiftMask;
                break;
            case XK_Control_L:
                pModMap[kc] = ControlMask;
                break;
            case XK_Caps_Lock:
                pModMap[kc] = LockMask;
                break;
            case XK_Alt_L:
            case XK_Alt_R:
                pModMap[kc] = Mod1Mask;
                break;
            default:
                pModMap[kc] = NoSymbol;
                break;
            }
    }
    return TRUE;
}

/*
 * apMNHandleKey -- Driver internal code
 *      Given the action, generate the X event.
 */
void
apMNHandleKey (xEp, action)
    xEvent         *xEp;
    unsigned char   action;
{
    if (action >= KBD3_$KEY_UP)
    {
        xEp->u.u.type = KeyRelease;
        xEp->u.u.detail = AP_ACTION_TO_KEYCODE(action - KBD3_$KEY_UP);
    }
    else
    {
        xEp->u.u.type = KeyPress;
        xEp->u.u.detail = AP_ACTION_TO_KEYCODE(action);
    }

    (*apKeyboard->processInputProc) (xEp, apKeyboard, 1);
}

/*
 * apMNSetRepeat -- Driver internal code
 *      Set autorepeat functions for the keyboard.
 */
void
apMNSetRepeat (ctrl)
    KeybdCtrl  *ctrl;
{
    idm_$keymap_t           repeatKeyset;
    int                     kc, action;
    int                     i, bit;
    aws_$auto_repeat_mode_t mode;
    status_$t               status;

    aws_$set_auto_repeat (aws_$repeat_off, idmKeyset, &status);

    lib_$init_set (repeatKeyset, sizeof(repeatKeyset)*8);
    for (kc = 0; kc < MAP_LENGTH; kc++)
    {
        i = kc >> 3;
        bit = 1 << (kc & 7);
        if ((ctrl->autoRepeats[i]) & bit)
            lib_$add_to_set (repeatKeyset, sizeof(repeatKeyset)*8,
                             (short) AP_KEYCODE_TO_ACTION(kc));
    }
    aws_$set_auto_repeat (aws_$repeat_on, repeatKeyset, &status);

    mode = (ctrl->autoRepeat) ? aws_$repeat_on : aws_$repeat_off;
    aws_$set_auto_repeat_mode (mode, &status);
}

#endif  /* NO_PHYS_KEYS */
