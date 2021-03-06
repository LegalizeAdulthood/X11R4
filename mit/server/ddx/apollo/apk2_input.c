/******************************************************************
Copyright 1987 by Apollo Computer Inc., Chelmsford, Massachusetts.
Copyright 1989 by Hewlett-Packard Company.

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
 * This code handles keyboard type 2.
 */

/*
 *  This driver code is for Apollo keyboard number 2 (with mouse, without numeric keypad or lighted
 *  CapsLock key).  (Keyboard number 1 is unsupported, since it can't have a mouse.  Keyboard
 *  number 3 is a superset of keyboard number 2.  It has the ability to generate raw key up/down
 *  transitions; this should be supported but isn't.  Keyboard number 2 cannot generate raw key
 *  up/downs.)
 *
 *  Only the white keys, the four basic arrow keys and F1-F8 are implemented now.
 *  Up transitions for the white keys are faked.
 *  Positions of the real control and shift keys are inferred from the raw input character;
 *      their transitions are faked if necessary.
 *  "Mouse" Control, Shift and Meta keys are as for Apollo V10 driver:
 *      Control:  KBD_$LD    Boxed up-arrow     Lower left corner of left-hand keypad
 *      Shift:    KBD_$LF    Boxed down-arrow   Lower right corner of left-hand keypad
 *      Meta:     KBD_$R1    "POP"              Lower right corner of main keyboard
 *
 *  You can bail out of the server by hitting the ABORT/EXIT key (KBD_$R5S/KBD_$R5).  Unshifted,
 *  it will exit the server in an orderly fashion.  If this doesn't work (i.e. server is wedged),
 *  the shifted version is the system quit character.
 */

#include "apollo.h"

/* ignore whole rest of file if SR10.2 (or later) */
#ifdef NO_PHYS_KEYS

#define XK_MISCELLANY
#define XK_LATIN1
#include "keysymdef.h"

#include <apollo/kbd.h>

#ifdef NO_GPR_QUIT
extern void smd_$set_quit_char(
    char       &c,
    status_$t  *status
);
#endif

/*
 * State variables which tell us whether the "mouse" control and shift keys
 * are currently down, so we can know whether to generate control and shift
 * transitions for "modified" white keys.
 */
static Bool     controlIsDown = FALSE;
static Bool     shiftIsDown = FALSE;

/*
 * Entry in the keyboard driver table, given below.
 */
typedef struct _key
{
    short   key_color;
    short   key_mods;
    int     base_key;
} keyRec;

/*
 * Values for the key_color field.
 */
#define KEYC_DISABLED 0
#define KEYC_WHITE 1
#define KEYC_BLACK 2

/*
 * Values for the key_mods field.
 */
#define KEYM_PLAIN 0
#define KEYM_CTRL 1
#define KEYM_SHFT 2
#define KEYM_UP 3

/*
 * The keyboard driver table, indexed by key code returned from the
 * actual keyboard.
 *
 * Meaning of the key_color field:
 *      KEYC_DISABLED   this key is not enabled, therefore not available from the server
 *      KEYC_WHITE      this key is white, so it cannot generate up transitions
 *      KEYC_BLACK      this key is black, so it can generate up transitions
 *
 * Meaning of the key_mods field:
 *      KEYM_PLAIN      this keycode is generated with no modifier down
 *      KEYM_CTRL       this keycode is generated with the white CTRL key down
 *      KEYM_SHFT       this keycode is generated with the white SHIFT key down
 *      KEYM_UP         this keycode is the up transition for a black key
 *
 * Meaning of the base_key field:
 *      This field contains the keycode which is generated by this
 *      key when no modifiers are down.
 */
#define N_KEY_CODES 256
static keyRec Keys[N_KEY_CODES] = {
    KEYC_WHITE,     KEYM_CTRL,  0x20,   /* 0x00 : ^SP  */
    KEYC_WHITE,     KEYM_CTRL,  0x61,   /* 0x01 : ^A   */
    KEYC_WHITE,     KEYM_CTRL,  0x62,   /* 0x02 : ^B   */
    KEYC_WHITE,     KEYM_CTRL,  0x63,   /* 0x03 : ^C   */
    KEYC_WHITE,     KEYM_CTRL,  0x64,   /* 0x04 : ^D   */
    KEYC_WHITE,     KEYM_CTRL,  0x65,   /* 0x05 : ^E   */
    KEYC_WHITE,     KEYM_CTRL,  0x66,   /* 0x06 : ^F   */
    KEYC_WHITE,     KEYM_CTRL,  0x67,   /* 0x07 : ^G   */
    KEYC_WHITE,     KEYM_CTRL,  0x68,   /* 0x08 : ^H   */
    KEYC_WHITE,     KEYM_CTRL,  0x69,   /* 0x09 : ^I   */
    KEYC_WHITE,     KEYM_CTRL,  0x6A,   /* 0x0A : ^J   */
    KEYC_WHITE,     KEYM_CTRL,  0x6B,   /* 0x0B : ^K   */
    KEYC_WHITE,     KEYM_CTRL,  0x6C,   /* 0x0C : ^L   */
    KEYC_WHITE,     KEYM_CTRL,  0x6D,   /* 0x0D : ^M   */
    KEYC_WHITE,     KEYM_CTRL,  0x6E,   /* 0x0E : ^N   */
    KEYC_WHITE,     KEYM_CTRL,  0x6F,   /* 0x0F : ^O   */
    KEYC_WHITE,     KEYM_CTRL,  0x70,   /* 0x10 : ^P   */
    KEYC_WHITE,     KEYM_CTRL,  0x71,   /* 0x11 : ^Q   */
    KEYC_WHITE,     KEYM_CTRL,  0x72,   /* 0x12 : ^R   */
    KEYC_WHITE,     KEYM_CTRL,  0x73,   /* 0x13 : ^S   */
    KEYC_WHITE,     KEYM_CTRL,  0x74,   /* 0x14 : ^T   */
    KEYC_WHITE,     KEYM_CTRL,  0x75,   /* 0x15 : ^U   */
    KEYC_WHITE,     KEYM_CTRL,  0x76,   /* 0x16 : ^V   */
    KEYC_WHITE,     KEYM_CTRL,  0x77,   /* 0x17 : ^W   */
    KEYC_WHITE,     KEYM_CTRL,  0x78,   /* 0x18 : ^X   */
    KEYC_WHITE,     KEYM_CTRL,  0x79,   /* 0x19 : ^Y   */
    KEYC_WHITE,     KEYM_CTRL,  0x7A,   /* 0x1A : ^Z   */
    KEYC_WHITE,     KEYM_PLAIN, 0x1B,   /* 0x1B : ESC  */
    KEYC_WHITE,     KEYM_CTRL,  0x5C,   /* 0x1C : ^\   */
    KEYC_WHITE,     KEYM_CTRL,  0x5D,   /* 0x1D : ^]   */
    KEYC_WHITE,     KEYM_CTRL,  0x60,   /* 0x1E : ^`   */
    KEYC_WHITE,     KEYM_CTRL,  0x2F,   /* 0x1F : ^/   */
    KEYC_WHITE,     KEYM_PLAIN, 0x20,   /* 0x20 : SP   */
    KEYC_WHITE,     KEYM_SHFT,  0x31,   /* 0x21 : !    */
    KEYC_WHITE,     KEYM_SHFT,  0x27,   /* 0x22 : "    */
    KEYC_WHITE,     KEYM_SHFT,  0x33,   /* 0x23 : #    */
    KEYC_WHITE,     KEYM_SHFT,  0x34,   /* 0x24 : $    */
    KEYC_WHITE,     KEYM_SHFT,  0x35,   /* 0x25 : %    */
    KEYC_WHITE,     KEYM_SHFT,  0x37,   /* 0x26 : &    */
    KEYC_WHITE,     KEYM_PLAIN, 0x27,   /* 0x27 : '    */
    KEYC_WHITE,     KEYM_SHFT,  0x39,   /* 0x28 : (    */
    KEYC_WHITE,     KEYM_SHFT,  0x30,   /* 0x29 : )    */
    KEYC_WHITE,     KEYM_SHFT,  0x38,   /* 0x2A : *    */
    KEYC_WHITE,     KEYM_SHFT,  0x3D,   /* 0x2B : +    */
    KEYC_WHITE,     KEYM_PLAIN, 0x2C,   /* 0x2C : ,    */
    KEYC_WHITE,     KEYM_PLAIN, 0x2D,   /* 0x2D : -    */
    KEYC_WHITE,     KEYM_PLAIN, 0x2E,   /* 0x2E : .    */
    KEYC_WHITE,     KEYM_PLAIN, 0x2F,   /* 0x2F : /    */
    KEYC_WHITE,     KEYM_PLAIN, 0x30,   /* 0x30 : 0    */
    KEYC_WHITE,     KEYM_PLAIN, 0x31,   /* 0x31 : 1    */
    KEYC_WHITE,     KEYM_PLAIN, 0x32,   /* 0x32 : 2    */
    KEYC_WHITE,     KEYM_PLAIN, 0x33,   /* 0x33 : 3    */
    KEYC_WHITE,     KEYM_PLAIN, 0x34,   /* 0x34 : 4    */
    KEYC_WHITE,     KEYM_PLAIN, 0x35,   /* 0x35 : 5    */
    KEYC_WHITE,     KEYM_PLAIN, 0x36,   /* 0x36 : 6    */
    KEYC_WHITE,     KEYM_PLAIN, 0x37,   /* 0x37 : 7    */
    KEYC_WHITE,     KEYM_PLAIN, 0x38,   /* 0x38 : 8    */
    KEYC_WHITE,     KEYM_PLAIN, 0x39,   /* 0x39 : 9    */
    KEYC_WHITE,     KEYM_SHFT,  0x3B,   /* 0x3A : :    */
    KEYC_WHITE,     KEYM_PLAIN, 0x3B,   /* 0x3B : ;    */
    KEYC_WHITE,     KEYM_SHFT,  0x2C,   /* 0x3C : <    */
    KEYC_WHITE,     KEYM_PLAIN, 0x3D,   /* 0x3D : =    */
    KEYC_WHITE,     KEYM_SHFT,  0x2E,   /* 0x3E : >    */
    KEYC_WHITE,     KEYM_SHFT,  0x2F,   /* 0x3F : ?    */
    KEYC_WHITE,     KEYM_SHFT,  0x32,   /* 0x40 : @    */
    KEYC_WHITE,     KEYM_SHFT,  0x61,   /* 0x41 : A    */
    KEYC_WHITE,     KEYM_SHFT,  0x62,   /* 0x42 : B    */
    KEYC_WHITE,     KEYM_SHFT,  0x63,   /* 0x43 : C    */
    KEYC_WHITE,     KEYM_SHFT,  0x64,   /* 0x44 : D    */
    KEYC_WHITE,     KEYM_SHFT,  0x65,   /* 0x45 : E    */
    KEYC_WHITE,     KEYM_SHFT,  0x66,   /* 0x46 : F    */
    KEYC_WHITE,     KEYM_SHFT,  0x67,   /* 0x47 : G    */
    KEYC_WHITE,     KEYM_SHFT,  0x68,   /* 0x48 : H    */
    KEYC_WHITE,     KEYM_SHFT,  0x69,   /* 0x49 : I    */
    KEYC_WHITE,     KEYM_SHFT,  0x6A,   /* 0x4A : J    */
    KEYC_WHITE,     KEYM_SHFT,  0x6B,   /* 0x4B : K    */
    KEYC_WHITE,     KEYM_SHFT,  0x6C,   /* 0x4C : L    */
    KEYC_WHITE,     KEYM_SHFT,  0x6D,   /* 0x4D : M    */
    KEYC_WHITE,     KEYM_SHFT,  0x6E,   /* 0x4E : N    */
    KEYC_WHITE,     KEYM_SHFT,  0x6F,   /* 0x4F : O    */
    KEYC_WHITE,     KEYM_SHFT,  0x70,   /* 0x50 : P    */
    KEYC_WHITE,     KEYM_SHFT,  0x71,   /* 0x51 : Q    */
    KEYC_WHITE,     KEYM_SHFT,  0x72,   /* 0x52 : R    */
    KEYC_WHITE,     KEYM_SHFT,  0x73,   /* 0x53 : S    */
    KEYC_WHITE,     KEYM_SHFT,  0x74,   /* 0x54 : T    */
    KEYC_WHITE,     KEYM_SHFT,  0x75,   /* 0x55 : U    */
    KEYC_WHITE,     KEYM_SHFT,  0x76,   /* 0x56 : V    */
    KEYC_WHITE,     KEYM_SHFT,  0x77,   /* 0x57 : W    */
    KEYC_WHITE,     KEYM_SHFT,  0x78,   /* 0x58 : X    */
    KEYC_WHITE,     KEYM_SHFT,  0x79,   /* 0x59 : Y    */
    KEYC_WHITE,     KEYM_SHFT,  0x7A,   /* 0x5A : Z    */
    KEYC_WHITE,     KEYM_PLAIN, 0x5B,   /* 0x5B : [    */
    KEYC_WHITE,     KEYM_PLAIN, 0x5C,   /* 0x5C : \    */
    KEYC_WHITE,     KEYM_PLAIN, 0x5D,   /* 0x5D : ]    */
    KEYC_WHITE,     KEYM_SHFT,  0x36,   /* 0x5E : ^    */
    KEYC_WHITE,     KEYM_SHFT,  0x2D,   /* 0x5F : _    */
    KEYC_WHITE,     KEYM_PLAIN, 0x60,   /* 0x60 : `    */
    KEYC_WHITE,     KEYM_PLAIN, 0x61,   /* 0x61 : a    */
    KEYC_WHITE,     KEYM_PLAIN, 0x62,   /* 0x62 : b    */
    KEYC_WHITE,     KEYM_PLAIN, 0x63,   /* 0x63 : c    */
    KEYC_WHITE,     KEYM_PLAIN, 0x64,   /* 0x64 : d    */
    KEYC_WHITE,     KEYM_PLAIN, 0x65,   /* 0x65 : e    */
    KEYC_WHITE,     KEYM_PLAIN, 0x66,   /* 0x66 : f    */
    KEYC_WHITE,     KEYM_PLAIN, 0x67,   /* 0x67 : g    */
    KEYC_WHITE,     KEYM_PLAIN, 0x68,   /* 0x68 : h    */
    KEYC_WHITE,     KEYM_PLAIN, 0x69,   /* 0x69 : i    */
    KEYC_WHITE,     KEYM_PLAIN, 0x6A,   /* 0x6A : j    */
    KEYC_WHITE,     KEYM_PLAIN, 0x6B,   /* 0x6B : k    */
    KEYC_WHITE,     KEYM_PLAIN, 0x6C,   /* 0x6C : l    */
    KEYC_WHITE,     KEYM_PLAIN, 0x6D,   /* 0x6D : m    */
    KEYC_WHITE,     KEYM_PLAIN, 0x6E,   /* 0x6E : n    */
    KEYC_WHITE,     KEYM_PLAIN, 0x6F,   /* 0x6F : o    */
    KEYC_WHITE,     KEYM_PLAIN, 0x70,   /* 0x70 : p    */
    KEYC_WHITE,     KEYM_PLAIN, 0x71,   /* 0x71 : q    */
    KEYC_WHITE,     KEYM_PLAIN, 0x72,   /* 0x72 : r    */
    KEYC_WHITE,     KEYM_PLAIN, 0x73,   /* 0x73 : s    */
    KEYC_WHITE,     KEYM_PLAIN, 0x74,   /* 0x74 : t    */
    KEYC_WHITE,     KEYM_PLAIN, 0x75,   /* 0x75 : u    */
    KEYC_WHITE,     KEYM_PLAIN, 0x76,   /* 0x76 : v    */
    KEYC_WHITE,     KEYM_PLAIN, 0x77,   /* 0x77 : w    */
    KEYC_WHITE,     KEYM_PLAIN, 0x78,   /* 0x78 : x    */
    KEYC_WHITE,     KEYM_PLAIN, 0x79,   /* 0x79 : y    */
    KEYC_WHITE,     KEYM_PLAIN, 0x7A,   /* 0x7A : z    */
    KEYC_WHITE,     KEYM_SHFT,  0x5B,   /* 0x7B : {    */
    KEYC_WHITE,     KEYM_SHFT,  0x5C,   /* 0x7C : |    */
    KEYC_WHITE,     KEYM_SHFT,  0x5D,   /* 0x7D : }    */
    KEYC_WHITE,     KEYM_SHFT,  0x60,   /* 0x7E : ~    */
    KEYC_WHITE,     KEYM_PLAIN, 0x7F,   /* 0x7F : DEL  */
    KEYC_DISABLED,  KEYM_PLAIN, 0x80,   /* 0x80 :      */
    KEYC_DISABLED,  KEYM_PLAIN, 0x81,   /* 0x81 : L1   */
    KEYC_DISABLED,  KEYM_PLAIN, 0x82,   /* 0x82 : L2   */
    KEYC_DISABLED,  KEYM_PLAIN, 0x83,   /* 0x83 : L3   */
    KEYC_DISABLED,  KEYM_PLAIN, 0x84,   /* 0x84 : L4   */
    KEYC_DISABLED,  KEYM_PLAIN, 0x85,   /* 0x85 : L5   */
    KEYC_DISABLED,  KEYM_PLAIN, 0x86,   /* 0x86 : L6   */
    KEYC_DISABLED,  KEYM_PLAIN, 0x87,   /* 0x87 : L7   */
    KEYC_BLACK,     KEYM_PLAIN, 0x88,   /* 0x88 : L8   */
    KEYC_DISABLED,  KEYM_PLAIN, 0x89,   /* 0x89 : L9   */
    KEYC_BLACK,     KEYM_PLAIN, 0x8A,   /* 0x8A : LA   */
    KEYC_DISABLED,  KEYM_PLAIN, 0x8B,   /* 0x8B : LB   */
    KEYC_BLACK,     KEYM_PLAIN, 0x8C,   /* 0x8C : LC   */
    KEYC_BLACK,     KEYM_PLAIN, 0x8D,   /* 0x8D : LD   */
    KEYC_BLACK,     KEYM_PLAIN, 0x8E,   /* 0x8E : LE   */
    KEYC_BLACK,     KEYM_PLAIN, 0x8F,   /* 0x8F : LF   */
    KEYC_BLACK,     KEYM_PLAIN, 0x90,   /* 0x90 : R1   */
    KEYC_DISABLED,  KEYM_PLAIN, 0x91,   /* 0x91 : R2   */
    KEYC_DISABLED,  KEYM_PLAIN, 0x92,   /* 0x92 : R3   */
    KEYC_DISABLED,  KEYM_PLAIN, 0x93,   /* 0x93 : R4   */
    KEYC_BLACK,     KEYM_PLAIN, 0x94,   /* 0x94 : R5   */
    KEYC_WHITE,     KEYM_PLAIN, 0x95,   /* 0x95 : BS   */
    KEYC_WHITE,     KEYM_PLAIN, 0x96,   /* 0x96 : CR   */
    KEYC_WHITE,     KEYM_PLAIN, 0x97,   /* 0x97 : TAB  */
    KEYC_WHITE,     KEYM_SHFT,  0x97,   /* 0x98 : STAB */
    KEYC_WHITE,     KEYM_CTRL,  0x97,   /* 0x99 : CTAB */
    KEYC_DISABLED,  KEYM_PLAIN, 0x9A,   /* 0x9A :      */
    KEYC_DISABLED,  KEYM_PLAIN, 0x9B,   /* 0x9B :      */
    KEYC_DISABLED,  KEYM_PLAIN, 0x9C,   /* 0x9C :      */
    KEYC_DISABLED,  KEYM_PLAIN, 0x9D,   /* 0x9D :      */
    KEYC_DISABLED,  KEYM_PLAIN, 0x9E,   /* 0x9E :      */
    KEYC_DISABLED,  KEYM_PLAIN, 0x9F,   /* 0x9F :      */
    KEYC_DISABLED,  KEYM_PLAIN, 0xA0,   /* 0xA0 :      */
    KEYC_DISABLED,  KEYM_UP,    0x81,   /* 0xA1 : L1U  */
    KEYC_DISABLED,  KEYM_UP,    0x82,   /* 0xA2 : L2U  */
    KEYC_DISABLED,  KEYM_UP,    0x83,   /* 0xA3 : L3U  */
    KEYC_DISABLED,  KEYM_UP,    0x84,   /* 0xA4 : L4U  */
    KEYC_DISABLED,  KEYM_UP,    0x85,   /* 0xA5 : L5U  */
    KEYC_DISABLED,  KEYM_UP,    0x86,   /* 0xA6 : L6U  */
    KEYC_DISABLED,  KEYM_UP,    0x87,   /* 0xA7 : L7U  */
    KEYC_BLACK,     KEYM_UP,    0x88,   /* 0xA8 : L8U  */
    KEYC_DISABLED,  KEYM_UP,    0x89,   /* 0xA9 : L9U  */
    KEYC_BLACK,     KEYM_UP,    0x8A,   /* 0xAA : LAU  */
    KEYC_DISABLED,  KEYM_UP,    0x8B,   /* 0xAB : LBU  */
    KEYC_BLACK,     KEYM_UP,    0x8C,   /* 0xAC : LCU  */
    KEYC_BLACK,     KEYM_UP,    0x8D,   /* 0xAD : LDU  */
    KEYC_BLACK,     KEYM_UP,    0x8E,   /* 0xAE : LEU  */
    KEYC_BLACK,     KEYM_UP,    0x8F,   /* 0xAF : LFU  */
    KEYC_BLACK,     KEYM_UP,    0x90,   /* 0xB0 : R1U  */
    KEYC_DISABLED,  KEYM_UP,    0x91,   /* 0xB1 : R2U  */
    KEYC_DISABLED,  KEYM_UP,    0x92,   /* 0xB2 : R3U  */
    KEYC_DISABLED,  KEYM_UP,    0x93,   /* 0xB3 : R4U  */
    KEYC_DISABLED,  KEYM_UP,    0x94,   /* 0xB4 : R5U  */
    KEYC_DISABLED,  KEYM_SHFT,  0x91,   /* 0xB5 : R2S  */
    KEYC_DISABLED,  KEYM_SHFT,  0x92,   /* 0xB6 : R3S  */
    KEYC_DISABLED,  KEYM_SHFT,  0x93,   /* 0xB7 : R4S  */
    KEYC_BLACK,     KEYM_SHFT,  0x94,   /* 0xB8 : R5S  */
    KEYC_DISABLED,  KEYM_PLAIN, 0xB9,   /* 0xB9 :      */
    KEYC_DISABLED,  KEYM_PLAIN, 0xBA,   /* 0xBA :      */
    KEYC_DISABLED,  KEYM_PLAIN, 0xBB,   /* 0xBB :      */
    KEYC_DISABLED,  KEYM_PLAIN, 0xBC,   /* 0xBC :      */
    KEYC_DISABLED,  KEYM_PLAIN, 0xBD,   /* 0xBD :      */
    KEYC_DISABLED,  KEYM_PLAIN, 0xBE,   /* 0xBE :      */
    KEYC_DISABLED,  KEYM_PLAIN, 0xBF,   /* 0xBF :      */
    KEYC_BLACK,     KEYM_PLAIN, 0xC0,   /* 0xC0 : F1   */
    KEYC_BLACK,     KEYM_PLAIN, 0xC1,   /* 0xC1 : F2   */
    KEYC_BLACK,     KEYM_PLAIN, 0xC2,   /* 0xC2 : F3   */
    KEYC_BLACK,     KEYM_PLAIN, 0xC3,   /* 0xC3 : F4   */
    KEYC_BLACK,     KEYM_PLAIN, 0xC4,   /* 0xC4 : F5   */
    KEYC_BLACK,     KEYM_PLAIN, 0xC5,   /* 0xC5 : F6   */
    KEYC_BLACK,     KEYM_PLAIN, 0xC6,   /* 0xC6 : F7   */
    KEYC_BLACK,     KEYM_PLAIN, 0xC7,   /* 0xC7 : F8   */
    KEYC_BLACK,     KEYM_SHFT,  0x90,   /* 0xC8 : R1S  */
    KEYC_DISABLED,  KEYM_SHFT,  0x81,   /* 0xC9 : L1S  */
    KEYC_DISABLED,  KEYM_SHFT,  0x82,   /* 0xCA : L2S  */
    KEYC_DISABLED,  KEYM_SHFT,  0x83,   /* 0xCB : L3S  */
    KEYC_DISABLED,  KEYM_SHFT,  0x84,   /* 0xCC : L4S  */
    KEYC_DISABLED,  KEYM_SHFT,  0x85,   /* 0xCD : L5S  */
    KEYC_DISABLED,  KEYM_SHFT,  0x86,   /* 0xCE : L6S  */
    KEYC_DISABLED,  KEYM_SHFT,  0x87,   /* 0xCF : L7S  */
    KEYC_BLACK,     KEYM_SHFT,  0xC0,   /* 0xD0 : F1S  */
    KEYC_BLACK,     KEYM_SHFT,  0xC1,   /* 0xD1 : F2S  */
    KEYC_BLACK,     KEYM_SHFT,  0xC2,   /* 0xD2 : F3S  */
    KEYC_BLACK,     KEYM_SHFT,  0xC3,   /* 0xD3 : F4S  */
    KEYC_BLACK,     KEYM_SHFT,  0xC4,   /* 0xD4 : F5S  */
    KEYC_BLACK,     KEYM_SHFT,  0xC5,   /* 0xD5 : F6S  */
    KEYC_BLACK,     KEYM_SHFT,  0xC6,   /* 0xD6 : F7S  */
    KEYC_BLACK,     KEYM_SHFT,  0xC7,   /* 0xD7 : F8S  */
    KEYC_BLACK,     KEYM_SHFT,  0x88,   /* 0xD8 : L8S  */
    KEYC_DISABLED,  KEYM_SHFT,  0x89,   /* 0xD9 : L9S  */
    KEYC_BLACK,     KEYM_SHFT,  0x8A,   /* 0xDA : LAS  */
    KEYC_DISABLED,  KEYM_SHFT,  0x8B,   /* 0xDB : LBS  */
    KEYC_BLACK,     KEYM_SHFT,  0x8C,   /* 0xDC : LCS  */
    KEYC_BLACK,     KEYM_SHFT,  0x8D,   /* 0xDD : LDS  */
    KEYC_BLACK,     KEYM_SHFT,  0x8E,   /* 0xDE : LES  */
    KEYC_BLACK,     KEYM_SHFT,  0x8F,   /* 0xDF : LFS  */
    KEYC_BLACK,     KEYM_UP,    0xC0,   /* 0xE0 : F1U  */
    KEYC_BLACK,     KEYM_UP,    0xC1,   /* 0xE1 : F2U  */
    KEYC_BLACK,     KEYM_UP,    0xC2,   /* 0xE2 : F3U  */
    KEYC_BLACK,     KEYM_UP,    0xC3,   /* 0xE3 : F4U  */
    KEYC_BLACK,     KEYM_UP,    0xC4,   /* 0xE4 : F5U  */
    KEYC_BLACK,     KEYM_UP,    0xC5,   /* 0xE5 : F6U  */
    KEYC_BLACK,     KEYM_UP,    0xC6,   /* 0xE6 : F7U  */
    KEYC_BLACK,     KEYM_UP,    0xC7,   /* 0xE7 : F8U  */
    KEYC_DISABLED,  KEYM_PLAIN, 0xE8,   /* 0xE8 : L1A  */
    KEYC_DISABLED,  KEYM_PLAIN, 0xE9,   /* 0xE9 : L2A  */
    KEYC_DISABLED,  KEYM_PLAIN, 0xEA,   /* 0xEA : L3A  */
    KEYC_DISABLED,  KEYM_PLAIN, 0xEB,   /* 0xEB : R6   */
    KEYC_DISABLED,  KEYM_SHFT,  0xE8,   /* 0xEC : L1AS */
    KEYC_DISABLED,  KEYM_SHFT,  0xE9,   /* 0xED : L2AS */
    KEYC_DISABLED,  KEYM_SHFT,  0xEA,   /* 0xEE : L3AS */
    KEYC_DISABLED,  KEYM_SHFT,  0xEB,   /* 0xEF : R6S  */
    KEYC_BLACK,     KEYM_CTRL,  0xC0,   /* 0xF0 : F1C  */
    KEYC_BLACK,     KEYM_CTRL,  0xC1,   /* 0xF1 : F2C  */
    KEYC_BLACK,     KEYM_CTRL,  0xC2,   /* 0xF2 : F3C  */
    KEYC_BLACK,     KEYM_CTRL,  0xC3,   /* 0xF3 : F4C  */
    KEYC_BLACK,     KEYM_CTRL,  0xC4,   /* 0xF4 : F5C  */
    KEYC_BLACK,     KEYM_CTRL,  0xC5,   /* 0xF5 : F6C  */
    KEYC_BLACK,     KEYM_CTRL,  0xC6,   /* 0xF6 : F7C  */
    KEYC_BLACK,     KEYM_CTRL,  0xC7,   /* 0xF7 : F8C  */
    KEYC_DISABLED,  KEYM_UP,    0xE8,   /* 0xF8 : L1AU */
    KEYC_DISABLED,  KEYM_UP,    0xE9,   /* 0xF9 : L2AU */
    KEYC_DISABLED,  KEYM_UP,    0xEA,   /* 0xFA : L3AU */
    KEYC_DISABLED,  KEYM_UP,    0xEB,   /* 0xFB : R6U  */
    KEYC_DISABLED,  KEYM_PLAIN, 0xFC,   /* 0xFC :      */
    KEYC_DISABLED,  KEYM_PLAIN, 0xFD,   /* 0xFD :      */
    KEYC_DISABLED,  KEYM_PLAIN, 0xFE,   /* 0xFE :      */
    KEYC_DISABLED,  KEYM_PLAIN, 0xFF    /* 0xFF :      */
    };

/*
 * apK2LegalModifier -- Driver internal code
 *      This is the implementation of LegalModifier for keyboard 2.
 */
Bool
apK2LegalModifier(key)
    BYTE    key;
{
    if ((key == KBD_$LD) || (key == KBD_$LF) || (key == KBD_$R1))
        return TRUE;
    return FALSE;
}

/*
 * apK2HWInitKbd -- Driver internal code
 *      Perform all "hardware" operations needed to initialize keyboard.
 */
void
apK2HWInitKbd()
{
    short           i;
    gpr_$keyset_t   keyset;
    status_$t       status;

    lib_$init_set (keyset, N_KEY_CODES);
    for (i=0; i<N_KEY_CODES; i++)
        if (Keys[i].key_color != KEYC_DISABLED)
            lib_$add_to_set (keyset, N_KEY_CODES, i);

    gpr_$enable_input(gpr_$keystroke, keyset, &status);
#ifdef NO_GPR_QUIT
    smd_$set_quit_char((char) 0, &status);
#else
    gpr_$set_quit_event(gpr_$no_event, 0, &status);
#endif
}

/*
 * apK2HWCloseKbd -- Driver internal code
 *      Perform all "hardware" operations needed to close down keyboard.
 *      In this case, nothing.
 */
void
apK2HWCloseKbd()
{
}

/*
 * apK2GetMappings -- Driver internal code
 *      This code creates the default keymap for keyboard 2.
 */
Bool
apK2GetMappings(pKeySyms, pModMap)
    KeySymsPtr  pKeySyms;
    CARD8      *pModMap;
{
    short   i;
    KeySym *map;
    int     min_k2_key, max_k2_key;

#define APOLLO_GLYPHS_PER_KEY 2

    min_k2_key = N_KEY_CODES;
    max_k2_key = 0;
    for (i=0; i<N_KEY_CODES; i++)
        if ((Keys[i].key_color != KEYC_DISABLED) && (Keys[i].key_mods == KEYM_PLAIN))
            {
                max_k2_key = i;
                if (min_k2_key > i) min_k2_key = i;
            }

    for (i = 0; i < MAP_LENGTH; i++)
        pModMap[i] = NoSymbol;  /* make sure it is restored */
    pModMap[ KBD_$LF ] = ShiftMask;
    pModMap[ KBD_$LD ] = ControlMask;
    pModMap[ KBD_$R1 ] = Mod1Mask;

    map = (KeySym *) xalloc(sizeof(KeySym) *
                            (MAP_LENGTH * APOLLO_GLYPHS_PER_KEY));
    if (!map) return FALSE;

    pKeySyms->minKeyCode = min_k2_key;
    pKeySyms->maxKeyCode = max_k2_key;
    pKeySyms->mapWidth = APOLLO_GLYPHS_PER_KEY;
    pKeySyms->map = map;

    for (i = 0; i < (MAP_LENGTH * APOLLO_GLYPHS_PER_KEY); i++)
        map[i] = NoSymbol;      /* make sure it is restored */

#define INDEX(in) ((in - min_k2_key) * APOLLO_GLYPHS_PER_KEY)

    map[INDEX(KBD_$F1)] = XK_F1;
    map[INDEX(KBD_$F2)] = XK_F2;
    map[INDEX(KBD_$F3)] = XK_F3;
    map[INDEX(KBD_$F4)] = XK_F4;
    map[INDEX(KBD_$F5)] = XK_F5;
    map[INDEX(KBD_$F6)] = XK_F6;
    map[INDEX(KBD_$F7)] = XK_F7;
    map[INDEX(KBD_$F8)] = XK_F8;

    map[INDEX(KBD_$UP_ARROW)] = XK_Up;
    map[INDEX(KBD_$LEFT_ARROW)] = XK_Left;
    map[INDEX(KBD_$RIGHT_ARROW)] = XK_Right;
    map[INDEX(KBD_$DOWN_ARROW)] = XK_Down;

    map[INDEX(KBD_$LD)] = XK_Control_L;
    map[INDEX(KBD_$LF)] = XK_Shift_L;
    map[INDEX(KBD_$R1)] = XK_Meta_L;

    map[INDEX(0x1B)] = XK_Escape;
    map[INDEX(KBD_$TAB)] = XK_Tab;
    map[INDEX(KBD_$BS)] = XK_BackSpace;
    map[INDEX(0x7F)] = XK_Delete;
    map[INDEX(KBD_$CR)] = XK_Return;
    map[INDEX(' ')] = XK_space;

    map[INDEX('1')] = XK_1;
    map[INDEX('1') + 1] = XK_exclam;
    map[INDEX('2')] = XK_2;
    map[INDEX('2') + 1] = XK_at;
    map[INDEX('3')] = XK_3;
    map[INDEX('3') + 1] = XK_numbersign;
    map[INDEX('4')] = XK_4;
    map[INDEX('4') + 1] = XK_dollar;
    map[INDEX('5')] = XK_5;
    map[INDEX('5') + 1] = XK_percent;
    map[INDEX('6')] = XK_6;
    map[INDEX('6') + 1] = XK_asciicircum;
    map[INDEX('7')] = XK_7;
    map[INDEX('7') + 1] = XK_ampersand;
    map[INDEX('8')] = XK_8;
    map[INDEX('8') + 1] = XK_asterisk;
    map[INDEX('9')] = XK_9;
    map[INDEX('9') + 1] = XK_parenleft;
    map[INDEX('0')] = XK_0;
    map[INDEX('0') + 1] = XK_parenright;
    map[INDEX('-')] = XK_minus;
    map[INDEX('-') + 1] = XK_underscore;
    map[INDEX('=')] = XK_equal;
    map[INDEX('=') + 1] = XK_plus;
    map[INDEX('`')] = XK_quoteleft;
    map[INDEX('`') + 1] = XK_asciitilde;

    map[INDEX('q')] = XK_q;
    map[INDEX('q') + 1] = XK_Q;
    map[INDEX('w')] = XK_w;
    map[INDEX('w') + 1] = XK_W;
    map[INDEX('e')] = XK_e;
    map[INDEX('e') + 1] = XK_E;
    map[INDEX('r')] = XK_r;
    map[INDEX('r') + 1] = XK_R;
    map[INDEX('t')] = XK_t;
    map[INDEX('t') + 1] = XK_T;
    map[INDEX('y')] = XK_y;
    map[INDEX('y') + 1] = XK_Y;
    map[INDEX('u')] = XK_u;
    map[INDEX('u') + 1] = XK_U;
    map[INDEX('i')] = XK_i;
    map[INDEX('i') + 1] = XK_I;
    map[INDEX('o')] = XK_o;
    map[INDEX('o') + 1] = XK_O;
    map[INDEX('p')] = XK_p;
    map[INDEX('p') + 1] = XK_P;
    map[INDEX('[')] = XK_bracketleft;
    map[INDEX('[') + 1] = XK_braceleft;
    map[INDEX(']')] = XK_bracketright;
    map[INDEX(']') + 1] = XK_braceright;

    map[INDEX('a')] = XK_a;
    map[INDEX('a') + 1] = XK_A;
    map[INDEX('s')] = XK_s;
    map[INDEX('s') + 1] = XK_S;
    map[INDEX('d')] = XK_d;
    map[INDEX('d') + 1] = XK_D;
    map[INDEX('f')] = XK_f;
    map[INDEX('f') + 1] = XK_F;
    map[INDEX('g')] = XK_g;
    map[INDEX('g') + 1] = XK_G;
    map[INDEX('h')] = XK_h;
    map[INDEX('h') + 1] = XK_H;
    map[INDEX('j')] = XK_j;
    map[INDEX('j') + 1] = XK_J;
    map[INDEX('k')] = XK_k;
    map[INDEX('k') + 1] = XK_K;
    map[INDEX('l')] = XK_l;
    map[INDEX('l') + 1] = XK_L;
    map[INDEX(';')] = XK_semicolon;
    map[INDEX(';') + 1] = XK_colon;
    map[INDEX('\'')] = XK_quoteright;
    map[INDEX('\'') + 1] = XK_quotedbl;
    map[INDEX('\\')] = XK_backslash;
    map[INDEX('\\') + 1] = XK_bar;

    map[INDEX('z')] = XK_z;
    map[INDEX('z') + 1] = XK_Z;
    map[INDEX('x')] = XK_x;
    map[INDEX('x') + 1] = XK_X;
    map[INDEX('c')] = XK_c;
    map[INDEX('c') + 1] = XK_C;
    map[INDEX('v')] = XK_v;
    map[INDEX('v') + 1] = XK_V;
    map[INDEX('b')] = XK_b;
    map[INDEX('b') + 1] = XK_B;
    map[INDEX('n')] = XK_n;
    map[INDEX('n') + 1] = XK_N;
    map[INDEX('m')] = XK_m;
    map[INDEX('m') + 1] = XK_M;
    map[INDEX(',')] = XK_comma;
    map[INDEX(',') + 1] = XK_less;
    map[INDEX('.')] = XK_period;
    map[INDEX('.') + 1] = XK_greater;
    map[INDEX('/')] = XK_slash;
    map[INDEX('/') + 1] = XK_question;

#undef INDEX

    return TRUE;
}

/*
 * apK2HandleKey -- Driver internal code
 *      Given the keycode kc, generate the one or more X events
 *      necessary to simulate possible modifier transitions, as well as down
 *      and up transitions for the key itself.
 */
void
apK2HandleKey (xEp, kc)
    xEvent         *xEp;
    unsigned char   kc;
{
    keyRec  kr;

    switch (kc)
    {
        case KBD_$EXIT:
	    GiveUp();
            return;
        case KBD_$LD:
        case KBD_$LDS:
            controlIsDown = TRUE;
            break;
        case KBD_$LDU:
            controlIsDown = FALSE;
            break;
        case KBD_$LF:
        case KBD_$LFS:
            shiftIsDown = TRUE;
            break;
        case KBD_$LFU:
            shiftIsDown = FALSE;
            break;
        default:
            break;
    }

    kr = Keys[kc];

    switch (kr.key_mods)
    {
        case KEYM_PLAIN:
            xEp->u.u.type = KeyPress;
            xEp->u.u.detail = kr.base_key;
            (*apKeyboard->processInputProc) (xEp, apKeyboard, 1);
            if (kr.key_color == KEYC_WHITE)
            {
                xEp->u.u.type = KeyRelease;
                (*apKeyboard->processInputProc) (xEp, apKeyboard, 1);
            }
            break;
        case KEYM_CTRL:
            xEp->u.u.type = KeyPress;
            if (!controlIsDown)
            {
                xEp->u.u.detail = KBD_$LD;
                (*apKeyboard->processInputProc) (xEp, apKeyboard, 1);
            }
            xEp->u.u.detail = kr.base_key;
            (*apKeyboard->processInputProc) (xEp, apKeyboard, 1);
            xEp->u.u.type = KeyRelease;
            if (kr.key_color == KEYC_WHITE)
            {
                (*apKeyboard->processInputProc) (xEp, apKeyboard, 1);
            }
            if (!controlIsDown)
            {
                xEp->u.u.detail = KBD_$LD;
                (*apKeyboard->processInputProc) (xEp, apKeyboard, 1);
            }
            break;
        case KEYM_SHFT:
            xEp->u.u.type = KeyPress;
            if (!shiftIsDown)
            {
                xEp->u.u.detail = KBD_$LF;
                (*apKeyboard->processInputProc) (xEp, apKeyboard, 1);
            }
            xEp->u.u.detail = kr.base_key;
            (*apKeyboard->processInputProc) (xEp, apKeyboard, 1);
            xEp->u.u.type = KeyRelease;
            if (kr.key_color == KEYC_WHITE)
            {
                (*apKeyboard->processInputProc) (xEp, apKeyboard, 1);
            }
            if (!shiftIsDown)
            {
                xEp->u.u.detail = KBD_$LF;
                (*apKeyboard->processInputProc) (xEp, apKeyboard, 1);
            }
            break;
        case KEYM_UP:
            xEp->u.u.type = KeyRelease;
            xEp->u.u.detail = kr.base_key;
            (*apKeyboard->processInputProc) (xEp, apKeyboard, 1);
            break;
    }

}

/*
 * apK2SetRepeat -- Driver internal code
 *      Set autorepeat functions for the keyboard.
 *      Can't do anything here.
 */
void
apK2SetRepeat (ctrl)
    KeybdCtrl  *ctrl;
{
}

#endif  /* NO_PHYS_KEYS */
