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



#include <X11/copyright.h>

/* $Header: XKeyBind.c,v 11.38 88/02/07 11:56:07 jim Exp $ */
/* Copyright 1985, 1987, Massachusetts Institute of Technology */

/* Beware, here be monsters (still under construction... - JG */

#define NEED_EVENTS
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

/* X10 XLookupMapping includes  */
#include <errno.h>		/* error file                  */
#include <fcntl.h>		/* for file open call          */
#include <sys/stat.h>		/* for size of file            */
#ifdef ATTACHSEG
#include <sys/ipc.h>		/* for                         */
#include <sys/shm.h>		/* for shared memory attach    */
#include <sys/hft.h>
#endif ATTACHSEG

#include "AIXkeymap.h"
#include "imkeybind.h"
#include "im.h"

typedef struct {
	struct _KeyBind	{
		char	*str;
		int	len;
	}	*tbl;
	uint	Max;
}	KeyBinds;

struct _IMMap {
	KeyMapFile	*file;		/* Point to header info */
	OldKeyMapElt	*keys;		/* key element array */
	KeyString	*strings;	/* keymap strings */
	KeyBinds	binds;		/* keymap rebinds */
	XLMSTRUCT	xlm;		/* data persistent between calls */
	char		buf[256];	/* temporary buffer */
	int		index;		/* current index into buffer */
	uint		statemask;	/* Mask for valid state */
};

#define GetKeyElement(p,s) ( m->keys + ( m->file->state[(s)] + (p) ) )

#define	key_assign	func.id

#define PAGE0	( GRAPHIC<<28 | NORM<<24 | P0<<8 )
#define PAGE1	( GRAPHIC<<28 | NORM<<24 | P1<<8 )
#define DIAKEY	(               DEAD<<24         )
#define CTLKEY	( CTLFUNC<<28 | NORM <<24        )
#define PFKEY	( CTLFUNC<<28 | CPFK <<24        )
#define ESCKEY	( ES_FUNC<<28 | NORM <<24        )

#define MetaMask	Mod1Mask
#define AltGraphMask	Mod2Mask

#define IS_CONTROL_KEYSYM(p)	(((p) & 0xff00) == 0xff00)
#define SHIFT_KEYSYM(p)		(((p) & 0xff) | 0x100)

char	*immalloc(), *imcalloc(), *imrealloc();
void	imfree();

/*
 * Note for reader.
 *
 * (1) sturcture of element.
 *
 * element ( of type OldKeyMapElt ) is union type. Its structure is,
 *
 *   +-----------------+-----------------+-----------------+-----------------+
 *   |                 |                 |                 |                 |
 *   +--------+--------+-----------------+-----------------+-----------------+
 * 1   type     stat     pad               page              point
 * 2   pad (ushort)                        id (ushort)
 * 3   pad               page              offset (ushort)
 *
 *    GRAPHIC   uses  1 and 2.
 *    SGL_CTL   uses  1.
 *    CHAR_STR  uses  3.
 *    ES_FUNC   uses  2.
 *    CTLFUNC   uses  2.
 *    BIND_STR  uses  3.
 *
 *    GRAPHIC uses 2, namely, func.id to compare the page/point pair with
 *    the tables. And BIND_STR and CHAR_STR uses str.offset in different
 *    way. Such a way to use union type makes source code to be very
 *    unreadable. Also naming is very bad.  This structure should be
 *    corrected to better form like as, for example,
 *    typedef union   {
 *            int     word;
 *            struct  {
 *                    uint    type    : 4;
 *                    uint    stat    : 4;
 *            }       element;
 *            struct  {
 *                    union   {
 *                            struct  {
 *                                    ushort  pad;
 *                                    char    page;
 *                                    char    point;
 *                            }       code;
 *                            struct  {
 *                                    ushort  pad;
 *                                    ushort  assign;
 *                            }       value;
 *                    }       character;
 *            }       graphic;
 *            struct  {
 *                    ushort  pad;
 *                    ushort  id;
 *            }       func;
 *            struct  {
 *                    ushort  pad;
 *                    ushort  offset;
 *            }       str;
 *            struct  {
 *                    ushort  pad;
 *                    ushort  index;
 *            }       bindstr;
 *    }       KeyElement;
 *
 *    Want Unnamed Union !!!
 *
 * (2) sturcture of functions.
 *
 *   +-----------------+------------------------------------------------+
 *   |IMSimpleMapping()|          IMAIXMapping()                        |
 *   |                 |           +-------------------+                |
 *   |                 |           |done_accumulation()|                |
 *   |                 |-----------+----------+--------+----------------+
 *   |                 |check_for_dead_key()  |dead_state_process()     |
 *   |                 |                      |           +-------------+
 *   |                 |                      |           |key_compose()|
 *   | (sscheck FALSE) |                      |           |             |
 *   +-----------------+----------------------+-----------+-------------+
 *   |                     handle_xlated_key()                          |
 *   +------------------------------------------------------------------+
 *   |                      process_key()                               |
 *   +---------------------------------+--------------------------------+
 *   |     data_stream()               |      expand()                  |
 *   +---------------------------------+--------------------------------+
 *
 */

/************************************************************************
*
*  IMAIXMapping
*	(Originally, from XLookupMapping )
*
* PROCEDURE :   X ASCII Input Interpreter for RT keyboards
*
* DESCRIPTION : Process keyboard events.
*               Translate position codes + state bits to ASCII codes or
*               strings via the translate table.
*               Events that are assigned functions are expanded to the
*               data stream equivalent before being returned.
*
* INPUT :       Event (including position code and status bits)
*
* OUTPUT :      Pointer to an ASCII code or string of codes;
*               Pointer to a string length (returned as "nbytes", a
*                       parameter).
*************************************************************************
* CHANGE HISTORY:
*      09/22/86 - Created by A. Leonard.
*      04/04/87 - Changed INITIALIZE to call XUseKeymap function (fxr)
*      04/04/87 - Changed keyboard id to use dtype instead of fd of the
*                 display structure. (fxr)
*      04/07/87 - Removed shmat and chaned to read of entire keymap file.
*                 This is because 1) shmat broke? and 2) read has to
*                 be supported for dist. svc.
*      04/08/87 - Moved all ALT-NUMPAD processing to UNBOUND case.  This
*                 implies ALT-NUMPAD will only work if the NUMPAD keys
*                 are defined as UNBOUND.  Removed functions
*                 CHECK_ACCUMULATION and ACCUMULATION_OR_HANDLE.
*      04/08/87 - Changed UNBOUND to 0 rather then IC_IGNORE.
*      04/08/87 - Fixed minor NUMLOCK problems.
*      10/20/87 - Fixed offset calculation for key string assignment.
*                       see PTM27155
*      02/04/88 - Fixed pointer calculation for key string assignment.
*                       see APAR1213
*      05/10/88 - finished port of X10 XKeyBind.c to X11
*      04/26/89 - split into IM portions
*      06/??/89 - Turn off the Mod5Mask (NumLock) if it is on.
*      06/??/89 - Create and use a state mask bits (m->statemask) because
*		  this value varies among keymaps.
*      06/09/89 - Add checking whether the state is defined or not.
*      06/16/89 - Restructured.  Also beautified.
*      06/16/89 - Fix up the Rebinding function. It didn't work at all.
*      06/17/89 - Alt-NumPad processing encapsulated in IMAIXMapping().
*      06/17/89 - Corrected ss_effect processing.
*
************************************************************************/

char	*IMAIXMapping(immap, keysym, state, nbytes)
IMMap	immap;
KeySym	keysym;
uint	state;
int	*nbytes;			/* RETURN */
{
	struct _IMMap	*m;
	XLMSTRUCT	*xlm;
	int		addition;	/* ALTNUMPAD temp variable */
	OldKeyMapElt	*kptr;
	OldKeyMapElt	current;	/* current key component */
	uint	keypos;			/* key position */
	void	done_accumulating();

	*nbytes = 0;			/* initialize value */

	if ((m=(struct _IMMap *)immap) == (struct _IMMap *)NULL)
		return(NULL);	/* return since it's not initialized */
	xlm = &m->xlm;

	/* not LATIN1 or MISCELLANY keysym */
	if ((keysym & 0xff00) && ! (IS_CONTROL_KEYSYM(keysym)))
		return(NULL);			/* unsupported keysym value */

	if (IS_CONTROL_KEYSYM(keysym))		/* get keypos from keysym */
		keypos = SHIFT_KEYSYM(keysym);
	else
		keypos = keysym;

	if (keypos < m->file->hdr.min_pos || m->file->hdr.max_pos <= keypos)
		return(NULL);	/* return since keypos out of range for map */

	if (state & Mod5Mask) {
		/*
		 * Numlock is set. Numlock applies to KeyPad key only.
		 * Reverse the Shift bit.
		 */
		if (IsKeypadKey(keysym))
			state ^= ShiftMask;
		state &= ~Mod5Mask;	/* numlock bit, no longer needed */
	}

	state &= m->statemask;		/* mask the illegal state bits	*/

	/* get a pointer to element and check to see if it's defined or not */
	if ((char *)(kptr = GetKeyElement(keypos, state)) < (char *)m->keys ||
		(char *)m->strings <= (char *)kptr)
		return(NULL);

	/*
	 * Here is the start point of the real processing.
	 * Reset the index to the buffer into which the characters are stored.
	 */
	m->index = 0;		/* start out at 0; add if neede */

	if (current.element = (int)kptr->element) {
		/*
		 * This is a BOUND key.
		 * First, if Alt-NumPad accumulated, compose a character.
		 * Then, process the current input.
		 */
		if (xlm->accum_state) {		/* accumulated */
			done_accumulating(m, xlm->accumulation);
			xlm->accum_state = 0;
			xlm->accumulation = 0;
		}
		if (xlm->dead_state)		/* last key was a dead key */
			/* try to make a diacritic */
			dead_state_process(m, &current);
		else
			/* test the current key is a dead key */
			check_for_dead_key(m, &current);
		*nbytes = m->index;
		return(m->buf);
	}

	/*
	 * This is an UNBOUND key.
	 * If not alt ( or alt graph ) state, then return.
	 */
	if (!(state & (MetaMask | AltGraphMask))) {
		if (xlm->accum_state) {
			done_accumulating(m, xlm->accumulation);
			xlm->accum_state = FALSE;
			xlm->accumulation = 0;
		}
		*nbytes = m->index;
		return(m->buf);
	}

	/*
	 * Do the Alt Num key pad processing if applicable.
	 */
	switch (keysym) {
	case ALT_NUM_0 : addition = 0; break;
	case ALT_NUM_1 : addition = 1; break;
	case ALT_NUM_2 : addition = 2; break;
	case ALT_NUM_3 : addition = 3; break;
	case ALT_NUM_4 : addition = 4; break;
	case ALT_NUM_5 : addition = 5; break;
	case ALT_NUM_6 : addition = 6; break;
	case ALT_NUM_7 : addition = 7; break;
	case ALT_NUM_8 : addition = 8; break;
	case ALT_NUM_9 : addition = 9; break;
	default :				/* Not Alt-Num key */
		if (xlm->accum_state) {
			/* 
			 * keys other than Alt-Num key end the
			 * accumulation.
			 */
			done_accumulating(m, xlm->accumulation);
			xlm->accum_state = FALSE;
			xlm->accumulation = 0;
			*nbytes = m->index;
		}
		return(m->buf);
	}

	/*
	 * accumulate the addition to xlm->accumulation.
	 * This is a save area for accumulation.
	 */
	xlm->accumulation = 10 * xlm->accumulation + addition;

	if (++xlm->accum_state == 3) {
		/*
		 * Handle the case of 3 Alt-NumPad keys being pressed.
		 * Compose a char and sent it to the caller.
		 */
		done_accumulating(m, xlm->accumulation);
		xlm->accum_state = FALSE;
		xlm->accumulation = 0;
		*nbytes = m->index;
	}
	return(m->buf);
}

/*
 * This procedure is called whenever the termination of a ALT-NUMPAD
 * sequence is detected.  It will process the key code as a P0 code pt.
 * Treat it same as the ordinary input, that is, check whether in dead
 * state or not is needed.
 */

static void	done_accumulating(m, accumulation)
struct _IMMap	*m;
char	accumulation;
{
	OldKeyMapElt	accumulated;
	XLMSTRUCT	*xlm = &m->xlm;

	if ((accumulated.key.point = accumulation) < 32)
		accumulated.key.type = SGL_CTL;	/* ctrl-X */
	else
		accumulated.key.type = GRAPHIC;	/* a character */
	accumulated.key.stat = NORM;
	accumulated.key.page = CHARSET_P0;	/* always use P0 */
	if (xlm->dead_state)		/* last key was a dead key */
		/* try to make a diacritic */
		dead_state_process(m, &accumulated);
	else
		/* test the current key is a dead key */
		check_for_dead_key(m, &accumulated);
	return;
}

/*
 * IMSimpleMapping() does a simple keysym/state to string mapping.
 */

char	*IMSimpleMapping(immap, keysym, state, nbytes)
IMMap	immap;
KeySym	keysym;
uint	state;
int	*nbytes;		/* RETURN */
{
	struct _IMMap	*m;
	OldKeyMapElt	*kptr;
	OldKeyMapElt	current;	/* current key component */
	uint		keypos;		/* key position */

	*nbytes = 0;			/* initial value */

	if ((m=(struct _IMMap *)immap) == (struct _IMMap *)NULL)
		return(NULL);	/* return since it's not initialized */

	/* not LATIN1 or MISCELLANY keysym */
	if ((keysym & 0xff00) && ! (IS_CONTROL_KEYSYM(keysym)))
		return(NULL);			/* unsupported keysym value */

	if (IS_CONTROL_KEYSYM(keysym))		/* get keypos from keysym */
		keypos = SHIFT_KEYSYM(keysym);
	else
		keypos = keysym;

	if (keypos < m->file->hdr.min_pos || m->file->hdr.max_pos <= keypos)
		return(NULL);	/* return since keypos out of range for map */

	if (state & Mod5Mask) {
		/*
		 * Numlock is set. Numlock applies to KeyPad key only.
		 * Reverse the Shift bit.
		 */
		if (IsKeypadKey(keysym))
			state ^= ShiftMask;
		state &= ~Mod5Mask;	/* numlock bit, no longer needed*/
	}

	state &= m->statemask;		/* mask the illegal state bits	*/

	/* get a pointer to element and check to see if it's defined or not */
	if ((char *)(kptr = GetKeyElement(keypos, state)) < (char *)m->keys ||
		(char *)m->strings <= (char *)kptr)
		return(NULL);

	if (!(current.element = (int)kptr->element))	/* UNBOUND */
		return(NULL);

	/*
	 * Here is the start point of the real processing.
	 * Reset the index to the buffer into which the characters are stored.
	 */
	m->index = 0;			/* start out at 0; add if neede */

	m->xlm.dead_state = FALSE;	/* turn off the AIXMapping()	*/
	m->xlm.accum_state = FALSE;	/* special states.		*/
	m->xlm.ss_effect = FALSE;

	handle_xlated_key(m, &current, FALSE);	/* make up a string */

	*nbytes = m->index;
	return(m->buf);
}

/************************************************************************/
/* This procedure is called ONLY if the terminal is already in dead     */
/* state.  An attempt will be made to combine this second keystroke     */
/* with the previous dead key.  If successful, the combined key will    */
/* be returned.  If no combination can be made, both the previous       */
/* dead key and this key will be returned.                              */
/* The only exception to this rule is if this second is itself dead.    */
/* In this case, the previously saved dead key is returned and dead     */
/* key state is re-entered on account of the second dead key, which     */
/* is then held for processing until the next call.                     */
/* Note: This procedure assumes that all characters flagged as dead     */
/* are actually valid dead keys.  This depends on keycomp doing its     */
/* job correctly.  If an invalid key is marked dead, results will be    */
/* unpredictable.                                                       */
/* The following state diagram explains processing within this          */
/* module:                                                              */
/************************************************************************

INPUT ---------------------|PROCESS---------------------|OUTPUT STATE
			   |                            |
state | valid     | valid  |                            |
flag  |diacritic  |combin. |                            |
------|-----------|--------|----------------------------|--------------
!dead |   n/a     |   T    | 1. replace this cd. pt. w/ |  !dead state
      |           |        |    combined cd. pt.        |
      |           |        | 2. reset dead key state    |
------|-----------|--------|----------------------------|--------------
!dead |   n/a     |   F    | 1. reset dead key state.   |  !dead state
      |           |        | 2. process previously      |
      |           |        |    saved dead key.         |
      |           |        | 3. process this key.       |
------|-----------|--------|----------------------------|-------------
 dead |    T      |  n/a   | 1. reset dead key state.   |   dead state
      |           |        | 2. process previously      |
      |           |        |    saved dead key.         |
      |           |        | 3. save this key as the    |
      |           |        |    dead key.               |
      |           |        | 4. set dead key state.     |
 ***********************************************************************/

static	dead_state_process(m, this)
struct _IMMap *m;
register OldKeyMapElt	*this;
{
	XLMSTRUCT	*xlm = &m->xlm;
	char		i;		/* a counter                     */
	struct shdiac	*diacrptr;	/* points to diacritic table     */

	/*
	 * Shift, Control, Alt, Alt-graf keys must be disregarded by
	 * this routine so that dead state can be maintained until the
	 * REAL combining key comes along.
	 */
	if (this->key.type == ES_FUNC && this->func.id == KF_IGNORE)
		return;

	/*
	 * if this key is not dead key, try to compose with the saved
	 * nonspacing character.
	 */
	if (this->key.stat != DEAD) {
		key_compose(m, this);	/* try to compose w/ previous	*/
		xlm->dead_state = FALSE;/* dead_state is satisfied	*/
		return;
	}

	/*
	 * This key is dead, too !!! Have a double-dead situation.
	 * Check to see if this key is also nonspacing character.
	 * (nonspacing character is GRAPHIC.)
	 * If not, it should be disregarded.
	 * Anyway, still in dead_state.
	 */
	if (this->key.type != GRAPHIC)
		return;
	diacrptr = (struct shdiac *)DIAC;
	for (i = 0; i < NUMDIACRITICS; i++) {
		if (diacrptr[i].SYMB == this->key_assign) { 
			/*
			 * 'nonspacing - nonspacing' causes the first
			 * nonspacing character to be passed to, and the
			 * next one starts a new nonspacing character
			 * sequence.
			 * (Keyboard Description and Character Reference)
			 */
			handle_xlated_key(m, &xlm->deadkey, TRUE);
			xlm->ns.searchword = diacrptr[i].INDEX;
			xlm->deadkey.element = this->element;
			break;
		}
	}
	return;
}

/*
 * This procedure checks a compostion table in the range of the
 * diacritic (indicated by "start, stop"). If a match is found for the
 * second key in the dead key sequence, then the composite code page/
 * point is written into key_assign (a side-effect of the proc).
 * If the composition was not successful, the saved dead key is pro-
 * cessed, and then the second key is processed.
 */

static	key_compose(m, this)
struct _IMMap	*m;
register OldKeyMapElt	*this;
{
	XLMSTRUCT	*xlm = &m->xlm;
	int	found;			/* loop control variable	*/
	char	i;			/* a counter			*/
	struct shcomp	*compptr;	/* points to composition table	*/

	/*
	 * Looking into the COMP structure, checks if it's valid to
	 * compose a diacritical character.
	 * (Only GRAPHIC could be composed.)
	 * If it's O.K., the 'this' KEY component is replaced by
	 * the found diacritical character.
	 * If not, the diacritic saved the previous key-in is returned
	 * to the caller following the 'this' KEY component.
	 */
	if (this->key.type == GRAPHIC) {
		compptr = (struct shcomp *)COMP;
		found = FALSE;
		i = xlm->ns.search.start;
		while (i <= xlm->ns.search.stop && !found) {
			if (compptr[i].COMPIN == this->key_assign) {
				/*
				 * Found. Replace it by the diacritical char.
				 */
				found = TRUE;
				this->key_assign = compptr[i].COMPOUT;
			}
			i++;
		}
	}
	else
		found = FALSE;

	if (!found)	/* composition UNSUCCESSFUL. process saved dead key */
		handle_xlated_key(m, &xlm->deadkey, TRUE);

	/*
	 * send either the composed character or the last keystroke.
	 */
	handle_xlated_key(m, this, TRUE);
	return;
}

/************************************************************************/
/* This procedure is called ONLY if the terminal is NOT already in      */
/* dead state, a condition under which ALL keys are sent here to be     */
/* checked for "vitality".  All keys entering this procedure will       */
/* continue to be processed in the usual manner.  If a key is found     */
/* to be dead here, the setting of the dead_state will prevent          */
/* its being returned at this time.  However, such a key will be        */
/* sent on for processing by the accumulation checker.  This procedure  */
/* relies on the keycomp program to flag as dead ONLY those keys that   */
/* can be found in the table!!!                                         */
/************************************************************************
INPUT                PROCESS                         OUTPUT STATE
----------------------------------------------------------------------
    diacritic       | 1. set dead key state.        |   dead state
		    | 2. save this key as dead key. |
		    | 3. (dead key state will pre-  |
		    |    vent further processing of |
		    |    this key.)                 |
--------------------|-------------------------------|-----------------
 not a diacritic    | 1. process this key as usual. |  !dead state
************************************************************************/

static	check_for_dead_key(m, this)
struct _IMMap	*m;
register OldKeyMapElt	*this;
{
	XLMSTRUCT	*xlm = &m->xlm;
	char	i;			/* a counter			*/
	struct shdiac	*diacrptr;	/* ptr to diacritic table	*/

	if (this->key.type != GRAPHIC || this->key.stat != DEAD) {
		/* this key is not a dead key.	*/
		handle_xlated_key(m, this, TRUE);
		return;
	}

	xlm->ss_effect = FALSE;		/* SS satisfied */

	/*
	 * This key is a dead key.
	 * Check if it's a valid nonspacing character looking into
	 * the DIAC structure ( define in imkeybind.h ).
	 * If the entry found, save the start/stop pointer which is
	 * used in later when looking into the COMP structure for
	 * valid diacritic sequence. ( See key_compose() )
	 */
	diacrptr = (struct shdiac *)DIAC;
	for (i = 0; i < NUMDIACRITICS; i++) {
		if (diacrptr[i].SYMB == this->key_assign) { 
			xlm->ns.searchword = diacrptr[i].INDEX;
			xlm->dead_state = TRUE;
			xlm->deadkey.element = this->element;
			break;
		}
	}
	return;
}

/*
 * Checks for character strings.
 * If the input is character string ( CHAR_STR or BIND_STR ), calls
 * process_key() multiple times.
 * If sscheck flag is on, character < 0x20 are treated as SGL_CTL.
 * Otherwise, GRAPHIC. Not all keyboard treats 0x1c-0x1f as SS.
 */

static	handle_xlated_key(m, this, sscheck)
struct _IMMap *m;
register OldKeyMapElt	*this;
int	sscheck;
{
	OldKeyMapElt	each;
	unsigned char	length;
	int		i;
	KeyString	*kstr;
	char		*bstr;		/* rebound string */

	switch (this->key.type) {
	case	GRAPHIC :
	case	SGL_CTL :
		each.element = this->element;
		if (sscheck && each.key.point < 32)
			each.key.type = SGL_CTL;	/* ctrl-X */
		else
			each.key.type = GRAPHIC;	/* character */
		process_key(m, &each);
		break;

	case	CHAR_STR :
		/* get string */
		kstr = (KeyString *)((char *)m->strings + this->str.offset);
		length = kstr->length;	/* first byte contains its length */
		/* Set code page for codes in character string */
		each.key.page = this->str.page;
		each.key.stat = NORM;
		for (i = 0; i < length; i++) {	/* process each code of kstr */
			each.key.point = kstr->c[i];
			if (sscheck && each.key.point < 32)
				each.key.type = SGL_CTL;	/* ctrl-X */
			else
				each.key.type = GRAPHIC;	/* character */
			process_key(m, &each);
		}
		break;

	case	BIND_STR :
		/* get rebound string */
		bstr = m->binds.tbl[this->str.offset].str;
		length = m->binds.tbl[this->str.offset].len;
		/* Set code page for codes in character string */
		each.key.page = this->str.page;
		each.key.stat = NORM;
		for (i = 0; i < length; i++) {	/* process each code of kstr */
			each.key.point = bstr[i];
			if (sscheck && each.key.point < 32)
				each.key.type = SGL_CTL;	/* ctrl-X */
			else
				each.key.type = GRAPHIC;	/* character */
			process_key(m, &each);
		}
		break;

	default :	/* ES_FUNC or CTLFUNC */
		if (this->func.id != KF_IGNORE)		/* treated as UNBOUND */
			process_key(m, this);
		break;
	}
	return;
}

/*
 * This function processes one primitive, namely, GRAPHIC, SGL_CTL,
 * ES_FUNC and CTLFUNC at a time.  CHAR_STR and BIND_STR are broken into
 * primitives by handle_xlated_key() and it calles this function appropriate
 * times with key.type GRAPHIC or SGL_CTL.
 */

static	process_key(m, this)
struct _IMMap	*m;
register OldKeyMapElt	*this;
{
	XLMSTRUCT	*xlm = &m->xlm;

	switch (this->key.type) {
	case	GRAPHIC :
		data_stream(m, this);		/* fix up buffer */
		xlm->ss_effect = FALSE;		/* SS satisfied */
		break;

	case	SGL_CTL :
		/* any SGL_CTL will satisfy any SS wait state	*/
		/* but this may be another SS!			*/
		if (this->key.point >= IC_SS4 && this->key.point <= IC_SS1)
			xlm->ss_effect = SS_IN_EFFECT;	/* SS in effect */
		else
			xlm->ss_effect = FALSE;		/* SS satisfied */
		m->buf[m->index++] = this->key.point;	/* append to buffer */
		break;

	case	ES_FUNC :
	case	CTLFUNC :
		expand(m, this);	/* a function that needs expansion */
		xlm->ss_effect = FALSE;			/* SS satisfied */
		break;

	default :	/* NEVER */
		xlm->ss_effect = FALSE;			/* SS satisfied */
		break;
	} 
	return;
}

/*
 * Store the character into the buffer.
 * Called by process_key() only when GRAPHIC.
 * If the code page is P0, turn on MSB if Single_Shift (SS) is in effect.
 * If the code page is P1 or P2, insert an appropriate SS.
 */

static	data_stream(m, this)
struct _IMMap	*m;
register OldKeyMapElt	*this;
{
	XLMSTRUCT	*xlm = &m->xlm;
#define	FORCE_HIGH_MASK	0x80;

	switch (this->key.page) {
	case CHARSET_P0 :
		/*
		 * if preceding keystroke was a SS, turn on high bit.
		 */
		if (xlm->ss_effect)
			this->key.point |= FORCE_HIGH_MASK;
		break;

	case CHARSET_P1 :
		if (this->key.point <= 127)	/* in lower half */
			m->buf[m->index++] = IC_SS1;	/* insert SS1 */
		else
			m->buf[m->index++] = IC_SS2;	/* insert SS2 */
		this->key.point |= FORCE_HIGH_MASK;	/* MSB on */
		break;

	case CHARSET_P2 :
		if (this->key.point <= 127)	/* in lower half */
			m->buf[m->index++] = IC_SS3;	/* insert SS3 */
		else
			m->buf[m->index++] = IC_SS4;	/* insert SS4 */
		this->key.point |= FORCE_HIGH_MASK;	/* MSB on */
		break;

	default :	/* never */
		break;
	}
	m->buf[m->index++] = this->key.point;	/* append to buffer */
	return;
}

/*
 * Expand supported function ids to ASCII character code strings.
 * Called by process_key() only when ES_FUNC or CTRFUNC.
 */

static	expand(m, this)
struct _IMMap	*m;
register OldKeyMapElt	*this;
{
	char	*base;			/* current base of buffer */
	int	add_index;		/* addition for index */

	base = &m->buf[m->index];	/* current buffer top */
	base[0] = IC_ESC;		/* top is escape */

	if (this->key.stat == CPFK) {	/* PF Key sequence */
		base[1] = FE_CSI;	/* left bracket */
		/* convert to ascii 3 digits */
		(void)sprintf(&base[2], "%0.3d", this->key.point + 1);
		base[5] = CSEQ_F_PFK;
		m->index += 6;
		return;
	}

	if (this->key.type == ES_FUNC) { /* Escape sequence */
		switch (this->func.id) {
		case KF_RI :	/* Reverse index */
			base[1] = FE_RI;
			break;

		case KF_INIT :	/* Reset to Initial State */
			base[1] = FS_RIS;
			break;

		case KF_IND :	/* Index */
			base[1] = FE_IND;
			break;

		default :	/* unsupported */
			return;
		}
		m->index += 2;
		return;
	}

	if (this->key.type == CTLFUNC) {	/* control sequences */
		base[1] = FE_CSI;
		add_index = 3;
		switch (this->func.id) {
		case KF_CUU :	/* Cursor up 1 */
			base[2] = CSEQ_F_CUU;
			break;

		case KF_CUD :	/* Cursor down 1 */
			base[2] = CSEQ_F_CUD;
			break;

		case KF_CUF :	/* Cursor forward 1 */
			base[2] = CSEQ_F_CUF;
			break;

		case KF_CUB :	/* Cursor back 1 */
			base[2] = CSEQ_F_CUB;
			break;

		case KF_CBT :	/* Move cursor back to previous horizontal tab*/
			base[2] = CSEQ_F_CBT;
			break;

		case KF_CHT :	/* Move cursor forward to next horizontal tab */
			base[2] = CSEQ_F_CHT;
			break;

		case KF_HOM :	/* Cursor to home position */
			base[2] = CSEQ_F_CUP;
			break;

		case KF_CNL :	/* Move cursor to 1st char of next line */
			base[2] = CSEQ_F_CNL;
			break;

		case KF_DCH :	/* Delete cursored char */
			base[2] = CSEQ_F_DCH;
			break;

		case KF_IL :	/* Insert line */
			base[2] = CSEQ_F_IL;
			break;

		case KF_DL :	/* Delete line */
			base[2] = CSEQ_F_DL;
			break;

		case KF_EEOL :	/* Erase to end of line */
			base[2] = ic_0;
			base[3] = CSEQ_F_EL;
			add_index = 4;
			break;

		case KF_EEOF :	/* Erase to next tab stop */
			base[2] = ic_0;
			base[3] = CSEQ_F_EF;
			add_index = 4;
			break;

		case KF_CLR :	/* Erase entire PS */
			base[2] = ic_2;
			base[3] = CSEQ_F_ED;
			add_index = 4;
			break;
		 
		default :	/* unsupported */
			add_index = 0;
			return;
		}
		m->index += add_index;
	}
	return;
} 

/*
 *	Rebind the specified string to the specified keysym/state.
 */

IMRebindCode(immap, keysym, state, str, nbytes)
IMMap	immap;
register uint	keysym, state;
register char	*str;
register int	nbytes;
{
	struct _IMMap	*m;
	register OldKeyMapElt	*kptr;
	register int	ofs;
	uint	keypos;			/* key position */
	void	UnbindString();
	int	i;

	if ((m=(struct _IMMap *)immap) == (struct _IMMap *)NULL)
		return(NULL);	/* return since it's not initialized */

	/* not LATIN1 or MISCELLANY keysym */
	if ((keysym & 0xff00) && ! (IS_CONTROL_KEYSYM(keysym)))
		return(NULL);			/* unsupported keysym value */

	if (IS_CONTROL_KEYSYM(keysym))		/* get keypos from keysym */
		keypos = SHIFT_KEYSYM(keysym);
	else
		keypos = keysym;

	if (keypos < m->file->hdr.min_pos || m->file->hdr.max_pos <= keypos)
		return(NULL);	/* return since keypos out of range for map */

	if (state & Mod5Mask) {
		/*
		 * NumLock on. Unusual, but if it's been set, conver to
		 * the corespoinding entry.
		 */
		if (IsKeypadKey(keysym))
			state ^= ShiftMask;
		state &= ~Mod5Mask;
	}

	if (state & ~m->statemask)	/* if illegal state then return	*/
		return(NULL);

	/* get a pointer to element and check to see if it's valid or not */
	if ((char *)(kptr = GetKeyElement(keypos, state)) < (char *)m->keys ||
		(char *)m->strings <= (char *)kptr)
		return(NULL);


	/*
	 * 1 byte character. majority of the cases.
	 */
	if (nbytes == 1 && str) {
		if (kptr->key.type == BIND_STR)	/* if previously binded */
			UnbindString(m, kptr);
		kptr->element = PAGE0 | *str;
		return(1);
	}

	/*
	 * UNBOUND if neither str nor nbytes is given.
	 */
	if (!nbytes && !str) {
		if (kptr->key.type == BIND_STR)	/* if previously bound */
			UnbindString(m, kptr);
		kptr->element = 0;		/* UNBOUND */
		return(1);
	}

	/*
	 * Dead key definition if nbytes == 0.
	 */
	if (!nbytes) {
		struct diacrentry	*diacrptr;/* ptr to diacritic table */
		char	page, point;
		OldKeyMapElt	binding;

		if (*str == IC_SS1 || *str == IC_SS2) {
			page = charset_p1;
			point = str[1];
			binding.element = DIAKEY | PAGE1 | point;
		}
		else {
			page = charset_p0;
			point = str[0];
			binding.element = DIAKEY | PAGE0 | point;
		}

		/*
		 * check to see if it's valid or not, by looking into
		 * DIAC table.
		 */
		for (diacrptr = DIAC, i = 0; i < NUMDIACRITICS; i++) {
			if (diacrptr[i].POINT == point &&
				diacrptr[i].PAGE == page) { 
				/* FOUND, so it's valid */
				if (kptr->key.type == BIND_STR) /* if bound, */
					UnbindString(m, kptr);	/* unbound   */
				kptr->element = binding.element;
				return(1);
			}
		}

		/*
		 * Couldn't found, it's invalid. So no action taken.
		 */
		return(NULL);
	}

	/*
	 * Function key definition if str == 0.
	 * nbytes holds function id.
	 * See AIX OSTR, hft for information about function id.
	 * NOTE : There is one design error. According to AIX X-Windows
	 *        User's Guide, nbytes can not be zero ( and this routine
	 *	  does so ). So it's not possible to rebind to PF1 key
	 *	  since its function id == 0.
	 */

	if (!str) {
		/*
		 * PF Key.  0x0000 < nbytes < 0x00ff
		 */
		if (nbytes < 0xff) {
			if (kptr->key.type == BIND_STR)	/* if bound */
				UnbindString(m, kptr);
			kptr->element = PFKEY | nbytes;
			return(1);
		}

		/*
		 * ESC FUNC
		 */
		if (nbytes == KF_INIT || nbytes == KF_RI || nbytes == KF_IND) {
			if (kptr->key.type == BIND_STR)	/* if bound */
				UnbindString(m, kptr);
			kptr->element = ESCKEY | nbytes;
			return(1);
		}

		/*
		 * CTRL FUNC
		 * Note : Is this sufficient ?  OSTR says more but expand()
		 *	  processes only these.
		 */
		switch (nbytes) {
		case KF_CUU :	/* Cursor up 1 */
		case KF_CUD :	/* Cursor down 1 */
		case KF_CUF :	/* Cursor forward 1 */
		case KF_CUB :	/* Cursor back 1 */
		case KF_CBT :	/* Cursor back tab*/
		case KF_CHT :	/* Cursor horizontal tab */
		case KF_HOM :	/* Cursor to home position */
		case KF_CNL :	/* Cursor next line */
		case KF_DCH :	/* Delete cursored char */
		case KF_IL :	/* Insert line */
		case KF_DL :	/* Delete line */
		case KF_EEOL :	/* Erase to end of line */
		case KF_EEOF :	/* Erase to next tab stop */
		case KF_CLR :	/* Erase entire PS */
			if (kptr->key.type == BIND_STR)	/* if bound */
				UnbindString(m, kptr);
			kptr->element = CTLKEY | nbytes;
			return(1);
		}

		/*
		 * Invalid function specified.
		 */

		return(NULL);
	}

	/*
	 * Otherwise (nbytes > 1 && str > 0), bind the string.
	 * note : ofs >= 0.
	 */

	if ((ofs = BindString(m, str, nbytes)) >= 0) {
		kptr->key.type = BIND_STR;
		kptr->str.page = P0;
		kptr->str.offset = ofs;
		return(1);
	}

	/*
	 * bind error.
	 */

	return(NULL);
}

/*
 * BindString - Handles all dynamic strings being Binded to keymap.
 */

static	BindString(m, str, nbytes)
struct _IMMap	*m;
register char		*str;
register int		nbytes;
{
	uint	i;
	uint	freeslot;

	if (!str)		/* never */
		return(-1);

	if (!m->binds.tbl) {	/* Create Bound Table if not one yet */
		m->binds.Max = 10;
		m->binds.tbl = (struct _KeyBind *)
			 imcalloc(m->binds.Max, sizeof(struct _KeyBind));
	}

	/* search for empty slot */
	for (i = 0; i < m->binds.Max && m->binds.tbl[i].len; ++i);

	freeslot = i;

	/* Out of slots then realloc for more */
	if (i == m->binds.Max) {
		m->binds.Max += 10;	/* add 10 vacant */
		m->binds.tbl = (struct _KeyBind *)imrealloc(m->binds.tbl,
			m->binds.Max * sizeof(struct _KeyBind));
		/* mark all new entries as clean */
		while (i++ < m->binds.Max)
			m->binds.tbl[i].len = 0;
	}

	/* add string to table */
	m->binds.tbl[freeslot].len = nbytes;
	m->binds.tbl[freeslot].str = str;

	return(freeslot);
}

/*
 * UnbindString
 */

static void	UnbindString(m, kptr)
struct _IMMap	*m;
register OldKeyMapElt	*kptr;
{
	if (m->binds.tbl)
		m->binds.tbl[kptr->str.offset].len = 0;
}

/***********************************************************************
* This procedure is to open a file and read in the keyboard map.
* It WILL set the following global variables if it finds a file.
*    RT_FILE
*    XLMKFILE
*    XLMKMAP
*    XLMKSTRPTR
*
* Currently, there is no support for MIT format files, yet it would be
* very easy to put in a mode to support it.
***********************************************************************/

IMMap	IMUseKeymap(path)
char	*path;
{
	int		bytesread;	/* temp counter                  */
	int		fd;		/* file descriptor               */
	struct stat	st;		/* file stats                    */
	KeyMapFile	*km;
	register int	i, num_pos;
	struct _IMMap	*m;
	uint	maxstate, statemask;	/* used to calc the State Mask */
	char	*ptr;			/* buffer pointer */

	if ((fd = open(path,O_RDONLY,0)) < 0)	/* file can be opened */
		return (NULL);

#ifdef ATTACHSEG
	/* attempt to attach file */
	/* NOTE : ATTACHSEG must not be specified because the code	*/
	/* following here has not been taken it into accout.		*/
	if ((km = (KeyMapFile*)shmat(fd,0,SHM_MAP | SHM_RDONLY))
			== (KeyMapFile *) -1) {
#endif ATTACHSEG
	/*
	 * FILE CAN NOT BE ATTACHED
	 * This code was put here to support Dist. Srvc. which does not support
	 * mapping of remote files,  therefore try to read it in via mormal RW
	 */

	(void)fstat(fd,&st);
	m = (struct _IMMap *)
		immalloc((uint)(sizeof(struct _IMMap) + st.st_size));
	bzero(m, sizeof(struct _IMMap));
	ptr = (char *)(km = (KeyMapFile *)((char *)m + sizeof(struct _IMMap)));

	/* read in the file */
	while ((bytesread = read(fd, ptr, (uint)st.st_size)) != st.st_size) {
		if (bytesread > 0) {
			st.st_size -= bytesread;
			ptr += bytesread;
		}
		else if (bytesread == 0) {
			imfree((char *)m);
			(void)close(fd);
			return(NULL);
		}
		else if (errno != EINTR) {
			imfree((char *)m);
			(void)close(fd);
			return(NULL);
		}
	}
	(void)close(fd);

#ifdef ATTACHSEG
	}
#endif ATTACHSEG

	switch (km->hdr.magic) { 
	case 0373 :		/* Old version */
		/*
		 * - RT_KEYMAP_MAGIC - 32 predefined states
		 */
	case 0375 :		/* X2.1 version */
		/*
		 * Japanese keymap uses this type. 256 states.
		 */
	case AIX_KEYMAP_MAGIC:	/* New version */
		/*
		 * Max_States defines number of states
		 */
		break;

	default :	/* file not known */
		/*
		 * Thought should be made to support MIT format files,
		 * if so just grab MIT source and place it here.
		 * Also may have other changes!
		 */
		/*
		 * so what?  RT_FILE not set so BackstopLookupMapping
		 * will be used
		 */
		imfree((char *)m);
		return(NULL);
	}

	/*	
	 * Handle old versions which predefined number of states to size
	 * of struct.  New method is to define the max # of states in
	 * Max_States.
	 */
	if (km->Max_States == 0) 
		km->Max_States = 32 - 1; /* Adjust for addition in loop */

	/*
	 * calculate the valid state mask from km->Max_States.
	 */
	maxstate = km->Max_States;
	for (statemask = 0; maxstate != 0; maxstate >>= 1)
		statemask = (statemask << 1) | 1;
	m->statemask = statemask;

	/* convert each state index into a state offset based on the  */
	/* the number of positions per state                          */
	/* Note : states in which all keysym are UNBOUND have a       */
	/*        negative offset value, -num_pos - km->hdr.min_pos.  */
	/*        state[] entry for those states are originaly -1.    */

	num_pos = km->hdr.max_pos - km->hdr.min_pos;
	for (i = 0; i < km->Max_States + 1; ++i)
		km->state[i] = km->state[i] * num_pos - km->hdr.min_pos;


	m->keys = (OldKeyMapElt *)	/* ptr to Key Element array */
		   (((char *) km) +
		   sizeof(struct KEYMAPHEADER) +
		   sizeof(unsigned char) +
		   KEYPADDING * sizeof(char) +
		   (km->Max_States + 1) * sizeof(short));
	m->file = km;			/* ptr to top of file	*/
	m->strings = (KeyString*)	/* ptr to strings	*/
		(m->keys + m->file->hdr.max_state * num_pos);

#ifdef DEBUG
	printf("m->file   = %x\n", m->file );
	printf("m->keys   = %x\n", m->file->map.elt);
	printf("magic     = %x\n", &(m->file->hdr.magic) );
	printf("max_state = %x\n", &(m->file->hdr.max_state) );
	printf("max_pos   = %x\n", &(m->file->hdr.max_pos) );
	printf("min_pos   = %x\n", &(m->file->hdr.min_pos) );
	printf("Max_states= %x\n", &(m->file->Max_States) );
	printf("pad       = %x\n", &(m->file->pad[0]));
	printf("state     = %x\n", &(m->file->state[0]) );
	printf("map.elt   = %x\n", &(m->file->map.elt[0]) );
#endif DEBUG

	return((IMMap)m);
}

IMMap IMInitializeKeymap(language)
IMLanguage language;
{
	static char	*paths[4] = { NULL, NULL, NULL, IMDIRECTORY };
	IMMap	immap = NULL;
	char	*fullpath;
	caddr_t	*path;
	char	*NLgetenv();

	if (!language)
		language = IMDEFAULTLANG;	/* defined in imkeybind.h */

	if (!paths[0]) {
		paths[0] = NLgetenv("IMKEYMAPPATH");
		paths[1] = NLgetenv("HOME");
		paths[2] = NLgetenv("IMPATH");
	}

	for (path = paths; (path - paths) != 4; ++path) {
		if (!*path) continue;
		fullpath = immalloc((uint)(strlen(*path) + strlen(language) +
			sizeof IMKEYMAPNAME + 2));
		(void)sprintf(fullpath, "%s/%s/%s",
			*path, language, IMKEYMAPNAME);
		immap = IMUseKeymap(fullpath);
		imfree(fullpath);
		if (immap)
			break;
	}
	return immap;
}


void	IMFreeKeymap(immap) 
IMMap	immap;
{
	struct _IMMap	*m;

	/* Check if initialized */
	if ((m = (struct _IMMap *)immap) != (struct _IMMap *)NULL) {
		if (m->binds.tbl) 
			imfree(m->binds.tbl);
		imfree(immap);
	}

	return;
}
