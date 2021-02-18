/*
	Copyright
		Kyoto University Research Institute for Mathematical Sciences
		1987, 1988, 1989
	Copyright OMRON TATEISI ELECTRONICS CO. 1987, 1988, 1989
	Copyright ASTEC, Inc. 1987, 1988, 1989


    Permission to use, copy, modify, and distribute this software
    and its documentation for any purpose and without any fee is
    hereby granted, subject to the following restrictions:

    The above copyright notice and this permission notice must appear
    in all versions of this software;

    The name of "Wnn" may not be changed; 

    All documentations of software based on "Wnn" must contain the wording
    "This software is based on the original version of Wnn developed by
    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
    OMRON TATEISI ELECTRONICS CO. and
    ASTEC, Inc.", followed by the above copyright notice;

    The name of KURIMS, OMRON and ASTEC may not be used
    for any purpose related to the marketing or advertising
    of any product based on this software.

    KURIMS, OMRON and ASTEC make no representations about
    the suitability of this software for any purpose.
    It is provided "as is" without express or implied warranty.

    KURIMS, OMRON and ASTEC DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
    SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
    IN NO EVENT SHALL KURIMS, OMRON AND ASTEC BE LIABLE FOR ANY SPECIAL,
    INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
    LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
    OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.

    Author: Hideki Tsuiki	Kyoto University
				tsuiki%kaba.or.jp@uunet.uu.net

	    Hiroshi Kuribayashi	OMRON TATEISI ELECTRONICS CO.
				kuri@frf.omron.co.jp
				uunet!nff.ncl.omron.co.jp!kuri

	    Naouki Nide		Kyoto University
				nide%kaba.or.jp@uunet.uu.net

	    Shozo Takeoka	ASTEC, Inc.
				take%astec.co.jp@uunet.uu.net

	    Takashi Suzuki	Advanced Software Technology & Mechatronics
				Research Institute of KYOTO
				suzuki%astem.or.jp@uunet.uu.net

*/
/*	Version 4.0
 */
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"

#ifdef TERMINFO
#include <stdio.h>
#include <curses.h>
#include <term.h>
#undef putchar
extern int putchar();

extern char Term_Name[];
extern char    *Term_UnderScoreStart;
extern char    *Term_UnderScoreEnd;
extern char	*Term_ClrScreen;
extern char 	*Term_ClrEofLine;
extern char 	*Term_ThrowCursor;
extern char 	*Term_StandOutStart;
extern char 	*Term_StandOutEnd;
extern char 	*Term_BoldOutStart;	/* add by KUWA */
extern char 	*Term_BoldOutEnd;	/* add by KUWA */
static int	bold_mode_fun = 0;	/* add KURI */

openTermData()
{
    char *cp, *getenv(), *get_kbd_env();
    int status;
    int k;
    char lcode[10];
    char termchar[20];

	/* for convert_key --- added by Nide 10/3 */
    if (NULL == (cp = get_kbd_env()) ||
        0 != convert_getterm(cp, (0 != verbose_option))){
	fprintf(stderr, "Cannot get keyboard information.\n");
	return(-1); 
    }

    if ((cp = getenv("TERM")) == NULL){
        fprintf(stderr, "Cannot get terminal name.");
        return(-1);
    }
    strcpy(Term_Name, cp);

    if ((strlen(Term_Name) > 2) && (strcmp(Term_Name + (strlen(Term_Name) - 2),"-j") == 0)) {
	fprintf(stderr,"Uum:ｕｕｍからｕｕｍはおこせません。\n");
	return(-1);
    }
    setupterm(0,1,&status);
    reset_shell_mode();
    if (status != 1){
        return(-1);
    }
#ifdef SX8870
    if(jterm < 2) {	/* kanji terminal */
        fprintf(stderr, "Unn:Not kanji terminal. Goodbye !\n");
        return(-1);
    }
#endif
    if(save_cursor == (char *)NULL || *save_cursor == NULL ||
       restore_cursor == (char *)NULL || *restore_cursor == NULL ||
       change_scroll_region == (char *)NULL || *change_scroll_region == NULL) {
         fprintf(stderr, "Unn:Your terminal is not strong enough. Goodbye !\n");
         return(-1);
    }
    termchar[0] = 0;
    strcat(termchar,cp);
    strcat(termchar,"-j");
    setenv("TERM", termchar);

    sprintf(lcode,"%d", lines - conv_lines);
    setenv("LINES", lcode);

    if(cursor_normal && cursor_invisible){
	cursor_invisible_fun = 1;
    }else{
	cursor_invisible_fun = 0;
    }
    Term_UnderScoreEnd = exit_underline_mode;
    Term_UnderScoreStart = enter_underline_mode;
    Term_StandOutEnd = exit_standout_mode;
    Term_StandOutStart = enter_standout_mode;
    if (enter_bold_mode && exit_attribute_mode)
	bold_mode_fun = 1;
    else
	bold_mode_fun = 0;

    Term_BoldOutStart = enter_bold_mode;	/* add by KUWA */
    Term_BoldOutEnd = exit_attribute_mode;	/* add by KUWA */
    return(0);
}


closeTermData()

{
    resetterm();
    reset_shell_mode();
}

set_scroll_region(start, end)
int start, end;
{
    tputs(tparm(change_scroll_region, start, end), 1 , putchar);
}

clr_end_screen()
{
    tputs(clr_eos ,1,putchar);
}


clr_screen()
{
    tputs(clear_screen, lines, putchar);
    Term_ClrScreen = clear_screen;
}

clr_line1()
{
    tputs(clr_eol, 1, putchar);
    Term_ClrEofLine = clr_eol;
}

throw_cur_raw(col,row)
int col, row;
{
    tputs(tparm(cursor_address, row, col), 1, putchar);
}

h_r_on_raw()
{
    tputs(enter_standout_mode, 1, putchar);
}

h_r_off_raw()
{
    tputs(exit_standout_mode, 1, putchar);
}

u_s_on_raw()
{
    tputs(enter_underline_mode, 1, putchar);
}

u_s_off_raw()
{
    tputs(exit_underline_mode, 1, putchar);
}

b_s_on_raw()	/* add by KUWA */
{
    if (bold_mode_fun)
	tputs(enter_bold_mode, 1, putchar);
    else
	tputs(enter_underline_mode, 1, putchar);
}

b_s_off_raw()	/* add by KUWA */
{
    if (bold_mode_fun)
	tputs(exit_attribute_mode, 1, putchar);
    else
	tputs(exit_underline_mode, 1, putchar);
}

ring_bell()
{
    tputs(bell, 1, putchar);
    flush();
}

save_cursor_raw()
{
  tputs(save_cursor, 1, putchar);
}

restore_cursor_raw()
{
    tputs(restore_cursor, 1, putchar);
}

cursor_invisible_raw()
{
  tputs(cursor_invisible , 1 ,putchar);
}
 
cursor_normal_raw()
{
  tputs(cursor_normal , 1 , putchar);
}

#endif                          /* TERMINFO */
