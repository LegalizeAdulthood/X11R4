#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: kbd_ext.c,v 1.4 89/11/13 09:02:11 glennw Exp $";
#endif RCS_ID
#endif LINT

/*
 * Copyright 1989 by the Massachusetts Institute of Technology, 
 * Cambridge, Massachusetts, and Tektronix, Inc. Beaverton, Oregon.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Tektronix or M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Tektronix and M.I.T. make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * TEKTRONIX AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL TEKTRONIX OR M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 *
 *	NAME
 *		kbd_ext.c - device-independent non-standard keyboard code
 *
 *	DESCRIPTION
 *	    This file contains the code for server-side compose and 
 *	    multilingual keyboards.
 *
 *
 */

#include "X.h"
#include "misc.h"
#define NEED_EVENTS
#include "Xproto.h"
#include "inputstr.h"
#include "dispatch.h"

/* #include <stdio.h> */

static int ComposeMatchSeq = -1;

/* keep ranlib happy by leaving one symbol defined */
#ifdef SERVER_COMPOSE

static int ComposeKeyCount = 0;
static int HexComposeKeyCount = 0;
static KeyCode HexComposeKeysReceived[8];	/* max is for binary */
#define NUM_CHORDED_COMPOSE_KEYS 12 /* in case someone has 12 fingers!! */
static KeyCode KeyPresses[NUM_CHORDED_COMPOSE_KEYS]; /* assume init 0 */
	    /* list of compose keys still down */
/* new DDX interface */
extern void SetComposeLED();

TossKeyRelease(keycode) 
KeyCode keycode;
{
    int i;
    for (i=0; i < NUM_CHORDED_COMPOSE_KEYS; i++)
	if (KeyPresses[i] == 0) {
	    KeyPresses[i] = keycode;
	    break;
	}
	/* if we get here, we will violate the protocol, and send a 
	   KeyRelease without a key press.  Oh well... */
}

extern int ResetComposeSequence()
{
    ComposeMatchSeq = -1;
    HexComposeKeyCount = ComposeKeyCount = 0;
    SetComposeLED(FALSE);
}

Bool CancelComposeSequence(x, keybd)
    xEvent	 x;
    DeviceIntPtr keybd;
{
    int i, j;

    ResetComposeSequence();
    for (i = 0; i < Compose.num_cancel_keycodes; i++)
	if (x.u.u.detail == Compose.cancel_keycodes[i])
	    break;
    for (j = 0; j < Compose.num_abort_keycodes; j++)
	if (x.u.u.detail == Compose.abort_keycodes[j])
	    break;
    /* ring bell with default volume (may be zero) if it appears in neither
       or abort list */
    if (((i == Compose.num_cancel_keycodes) && 
	(j == Compose.num_abort_keycodes)) || 
	((i == Compose.num_cancel_keycodes) && 
	(j != Compose.num_abort_keycodes))) {
/* fprintf(stderr, "ringing bell at volume %d\n", keybd->u.keybd.ctrl.bell);
*/
	(*keybd->u.keybd.BellProc)(keybd->u.keybd.ctrl.bell, keybd); 
    }
    /* If keycode appears in neither or both lists, it is to be discarded.
       Otherwise, see if we are starting another compose sequence */
    if (((i == Compose.num_cancel_keycodes) && 
	(j == Compose.num_abort_keycodes)) || 
	((i != Compose.num_cancel_keycodes) && 
	(j != Compose.num_abort_keycodes)))
	/* discard key */
	return (TRUE);
    else
	return (CheckForCompose(x, keybd));
}

Bool CheckForCompose(x, keybd)
    xEvent	 x;
    DeviceIntPtr keybd;
{
    KeyCode keycode = x.u.u.detail;
    int	    i, j;
    KeyCode cur_sequence_keycode;
    int TempMatchSeq = ComposeMatchSeq;
    int max_char, num_digits;
    CARD16 tmp_keyButtonState;

    if (keyModifiersList[keycode])
	/* ignore modifier keys */
	return (FALSE);
    if (x.u.u.type == KeyRelease) {
	/* toss the KeyRelease if it matches a KeyPress received so far */
	for (i=0; i < NUM_CHORDED_COMPOSE_KEYS; i++)
	    if (keycode == KeyPresses[i]) {
		KeyPresses[i] = 0;
/* fprintf(stderr, "tossing a compose key release\n"); */
		return (TRUE);
	    }
	return (FALSE);
    }
    /* else type == KeyPress */
    if (ComposeMatchSeq != -1) {
/* fprintf(stderr, "processing next compose sequence keycode\n"); */
	/* find first sequence that matches keycodes so far */
	while (TempMatchSeq < Compose.num_composes) {
	    i = (TempMatchSeq * Compose.max_keycodes) + ComposeKeyCount;
	    cur_sequence_keycode = Compose.sequences[i];
	    if ((keycode == cur_sequence_keycode) && 
		((Compose.modifiers[i] & AnyModifier) ||
		 ((keyButtonState & AllModifiersMask) 
		/* modifier state before event; ignore Button bits */
		  == Compose.modifiers[i]))) {
		/* new key event matches, move to next key in sequence */
/* fprintf(stderr, "found next key event\n"); */
		TossKeyRelease(keycode);
		if ((++ComposeKeyCount == Compose.max_keycodes) ||
		    (Compose.sequences[i + 1] == NULL)) {
/* fprintf(stderr, "completed sequence - emit KeyPress/Release pair\n"); */
		    /* completed sequence - emit KeyPress/Release pair */
		    x.u.u.detail = Compose.output_keycodes[TempMatchSeq];
		    tmp_keyButtonState = keyButtonState;
		    /* Must change "modifier state before event".  This is a
		     * "virtual" modifier state, as there is no modifier
		     * KeyPress or KeyRelease sent.
		     */
		    /* Retain button state */
		    keyButtonState = (keyButtonState & ~AllModifiersMask) |
				   Compose.output_modifiers[TempMatchSeq];
		    (*keybd->public.processInputProc)(&x, keybd);
		    x.u.u.type = KeyRelease;
		    (*keybd->public.processInputProc)(&x, keybd);
		    /* restore actual keyButtonState */
		    keyButtonState = tmp_keyButtonState;
		    ResetComposeSequence();
		}
		/* continue to toss sequence key events */
		return (TRUE);
	    }
	    else {
		if ((cur_sequence_keycode > 0) && (cur_sequence_keycode < 5)) {
		    /* we are looking for a numeric compose key */
		    switch (cur_sequence_keycode) {
			case 1: /* binary */
			    max_char = 2; num_digits = 8;
			    break;
			case 2: /* octal */
			    max_char = 8; num_digits = 3;
			    break;
			case 3: /* decimal */
			    max_char = 10; num_digits = 3;
			    break;
			case 4: /* hex */
			default:    /* invalid - use hex */
			    max_char = 16; num_digits = 2;
			    break;
		    }
		    for (i = 0; i < max_char; i++) {
			for (j = 0; j < 3; j++) {
			    /* hardwired three keycodes per digit */
			    if (Compose.numeric_keycodes[(i * 3) + j] == 
				keycode)
				goto found;
			}
		    }
		    if (HexComposeKeyCount != 0)
			/* we don't have a matching numeric compose key */
			return (CancelComposeSequence(x, keybd));
			/* if we already had a numeric key, cancel.  Note that
			   we could search for a matching mnemonic compose, or
			   a numeric compose of higher radix, but it hardly
			   seems worthwhile.  Keep looking for other
			   matches on the first key. */
		    goto not_found;

found:		    /* we got a matching numeric compose key - stash it */
		    HexComposeKeysReceived[HexComposeKeyCount++] = keycode;
		    TossKeyRelease(keycode);
		    if (HexComposeKeyCount == num_digits)
		    {
		        /* send sequence keycodes (converted in Xlib) */
			tmp_keyButtonState = keyButtonState;
		        for (i = 0; i < ComposeKeyCount; i++) {
			    if (Compose.modifiers[TempMatchSeq *
				Compose.max_keycodes + i] != AnyModifier)
				/* Must change "modifier state before event".
				 * This is a "virtual" modifier state, as there
				 * is no modifier KeyPress or KeyRelease sent.
				 * If AnyModifier, modifier is a don't care;
				 * send current state.
				 */
				/* Retain button state */
				keyButtonState = 
				    (keyButtonState & ~AllModifiersMask) |
				    Compose.modifiers[TempMatchSeq * 
						     Compose.max_keycodes + i];
			    x.u.u.detail = Compose.sequences[
			    TempMatchSeq * Compose.max_keycodes + i];
			    x.u.u.type = KeyPress;
			    (*keybd->public.processInputProc)(&x, keybd);
			    x.u.u.type = KeyRelease;
			    (*keybd->public.processInputProc)(&x, keybd);
			}			
			for (i = 0; i < HexComposeKeyCount; i++) {
			    /* modifiers are whatever the last input
			     * sequence modifier was; Xlib ignores them.
			     */
			    x.u.u.detail = HexComposeKeysReceived[i];
			    x.u.u.type = KeyPress;
			    (*keybd->public.processInputProc)(&x, keybd);
			    x.u.u.type = KeyRelease;
			    (*keybd->public.processInputProc)(&x, keybd);
			}
		        ResetComposeSequence();
			/* restore actual keyButtonState */
			keyButtonState = tmp_keyButtonState;
		    }			
		    /* if incomplete, just return */
		    return(TRUE);
		}
not_found:
		/* else keycode doesn't match seq; look for one that does */
/* fprintf(stderr, "match failed, look for another sequence\n"); */
		TempMatchSeq++;
		while (TempMatchSeq < Compose.num_composes) {
		    for (i = 0; (i < ComposeKeyCount) &&
	     (Compose.sequences[ComposeMatchSeq * Compose.max_keycodes + i] ==
	      Compose.sequences[TempMatchSeq * Compose.max_keycodes + i]) &&
	     (Compose.modifiers[ComposeMatchSeq * Compose.max_keycodes + i] ==
	      Compose.modifiers[TempMatchSeq * Compose.max_keycodes + i]);
			 i++);
		    if (i == ComposeKeyCount) {
/* fprintf(stderr, "found a new sequence\n"); */
			/* found a new match for previous keys */
			ComposeMatchSeq = TempMatchSeq;
			break;  /* go back to check newest key */
		    }
		    TempMatchSeq++;
		}
	    }
	}
	/* TempMatchSeq >= Compose.num_composes:
	   invalid compose sequence found */
/* fprintf(stderr, "invalid compose sequence found\n"); */
	return (CancelComposeSequence(x, keybd));
    }
    else if (InitComposeKeys) {
/* fprintf(stderr, "checking for compose sequence: keycode %d, modifier 0x%x\n", keycode, keyButtonState);
*/
	/* QUICKLY see if we are starting one */
        for (i = 0; InitComposeKeys[i] != (KeyCode) NULL; i++) {
	    /* check current keyButtonState to see if modifiers match */
	    if ((keycode == InitComposeKeys[i]) &&
		((InitComposeModifiers[i] & AnyModifier) ||
		 ((keyButtonState & AllModifiersMask)
		/* modifier state before event; ignore Button bits */
		  == InitComposeModifiers[i]))) {
		/* we have a match; find first candidate */
		for (i = 0; i < Compose.num_composes * Compose.max_keycodes;
		     i += Compose.max_keycodes) {
		    if ((keycode == Compose.sequences[i]) &&
			((Compose.modifiers[i] & AnyModifier) ||
			 ((keyButtonState & AllModifiersMask) 
			  == Compose.modifiers[i]))) {
			/* initialize match_ptr to first candidate */
			ComposeMatchSeq = i / Compose.max_keycodes;
			SetComposeLED(TRUE);
			/* compose sequence with single keycode not allowed */
			ComposeKeyCount = 1;
			TossKeyRelease(keycode);
/* fprintf(stderr, "got initial compose sequence keycode from sequence number %d\n", ComposeMatchSeq);
*/
			return(TRUE);
		    }
		}
		/* if (i == (Compose.num_composes * Compose.max_keycodes)) */
/* fprintf(stderr, "FatalErrnr: cached Compose sequences conflict with actual ones!\n");
*/
		FatalError("CheckForCompose: cached Compose sequences conflict with actual ones!");
	    }
	}
	/* fall through; Compose not found */
	return(FALSE);
    }
    /* if (!InitComposeKeys), there are no Compose sequences */
    return (FALSE);
}
#endif /* SERVER_COMPOSE */
