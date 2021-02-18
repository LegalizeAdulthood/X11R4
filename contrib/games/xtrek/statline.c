/* statline.c */

/*
 * Copyright 1989 Jon Bennett, Mike Bolotski, David Gagne, Dan Lovinger
 * Copyright 1986 Chris Gutherie
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The copyright holders make no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include <X11/Xlib.h>
#include <stdio.h>
#include <signal.h>
#include <math.h>
#include "defs.h"
#include "data.h"



GLOBAL void statline (p, j, self)
  register struct player *p;		/* who we're looking at */
  register struct player *j;		/* who's displaying */
  int             self;

{
  static char     buf[80];
  static char    *hb1 = 
   "Flags        warp dam shd torps kills armies  fuel wtemp etemp failed    ";
  static char    *hb2 = 
   "Enemy                                                                    ";
  int             bufpos;
  char           *hb;

  /* Instead of one sprintf, we do all this by hand for optimization */

  buf[0] = ISSHIELD(p) ? 'S' : ' ';
  buf[1] = colorlet[(int)p->p_alert];
  buf[2] = ISPLLOCK(p) || ISPLOCK(p) ? 'L' : ' ';
  buf[3] = ISREPAIR(p) ? 'R' : ' ';
  buf[4] = ISBOMB(p) ? 'B' : ' ';
  buf[5] = ISORBIT(p) ? 'O' : ' ';
  buf[6] = ISCLOAK(p) ? 'C' : ' ';
  buf[7] = ISWEP(p) ? 'W' : ' ';
  buf[8] = ISENG(p) ? 'E' : ' ';
  buf[9] = ISBEAMUP(p) ? 'u' : ' ';
  buf[10] = ISBEAMDOWN(p) ? 'd' : ' ';
  buf[11] = ISCOPILOT(p) ? 'P' : ' ';
  buf[12] = ' ';
  buf[13] = ' ';
  {
    int             speed = p->p_speed;

    buf[14] = speed / 10 + '0';
    buf[15] = '.';
    buf[16] = speed % 10 + '0';
  }
  buf[17] = ' ';
  {
    int             damage;

    damage = p->p_damage;
    if (damage <= 0)
    {
      buf[18] = ' ';
      buf[19] = ' ';
      buf[20] = ' ';
    }
    else if (damage < 10)
    {
      buf[18] = ' ';
      buf[19] = ' ';
      buf[20] = damage + '0';
    }
    else if (damage < 100)
    {
      buf[18] = ' ';
      buf[19] = damage / 10 + '0';
      buf[20] = damage % 10 + '0';
    }
    else
    {
      buf[18] = damage / 100 + '0';
      buf[19] = damage / 10 % 10 + '0';
      buf[20] = damage % 10 + '0';
    }
  }
  buf[21] = ' ';
  {
    int             shield;

    shield = p->p_shield;
    if (shield <= 0)
    {
      buf[22] = ' ';
      buf[23] = ' ';
      buf[24] = ' ';
    }
    else if (shield < 10)
    {
      buf[22] = ' ';
      buf[23] = ' ';
      buf[24] = shield + '0';
    }
    else if (shield < 100)
    {
      buf[22] = ' ';
      buf[23] = shield / 10 + '0';
      buf[24] = shield % 10 + '0';
    }
    else
    {
      buf[22] = shield / 100 + '0';
      buf[23] = shield / 10 % 10 + '0';
      buf[24] = shield % 10 + '0';
    }
  }
  buf[25] = ' ';
  buf[26] = ' ';
  if (self)
  {
    int             ntorp;

    if (!self || (ntorp = p->p_ntorp) <= 0)
    {
      buf[27] = ' ';
      buf[28] = ' ';
    }
    else if (ntorp < 10)
    {
      buf[27] = ' ';
      buf[28] = ntorp + '0';
    }
    else
    {
      buf[27] = ntorp / 10 + '0';
      buf[28] = ntorp % 10 + '0';
    }
  }
  buf[29] = ' ';
  buf[30] = ' ';
  buf[31] = ' ';
  {
    float           kills;

    if (!self || (kills = p->p_kills) < 0.01)
    {
      buf[32] = ' ';
      buf[33] = ' ';
      buf[34] = ' ';
      buf[35] = ' ';
      buf[36] = ' ';
    }
    else
    {
      int             temp = (int) kills;

      buf[32] = '0' + temp / 10;
      buf[33] = '0' + temp % 10;
      buf[34] = '.';
      kills = (kills - temp) * 10;
      temp = (int) kills;
      buf[35] = '0' + temp;
      kills = (kills - temp) * 10;
      temp = (int) kills;
      buf[36] = '0' + temp;
    }
  }
  buf[37] = ' ';
  buf[38] = ' ';
  buf[39] = ' ';
  buf[40] = ' ';
  {
    int             armies;

    armies = p->p_armies;
    if (armies <= 0)
    {
      buf[41] = ' ';
      buf[42] = ' ';
    }
    else if (armies < 10)
    {
      buf[41] = ' ';
      buf[42] = armies + '0';
    }
    else
    {
      buf[41] = armies / 10 + '0';
      buf[42] = armies % 10 + '0';
    }
  }

  buf[43] = ' ';
  buf[44] = ' ';

  buf[45] = '0' + (p->p_fuel / 10000);
  if (buf[45] == '0')
    buf[45] = ' ';
  buf[46] = '0' + ((p->p_fuel % 10000) / 1000);
  if ((buf[46] == '0') && (p->p_fuel < 10000))
    buf[46] = ' ';
  buf[47] = '0' + ((p->p_fuel % 1000) / 100);
  if ((buf[47] == '0') && (p->p_fuel < 1000))
    buf[47] = ' ';
  buf[48] = '0' + ((p->p_fuel % 100) / 10);
  if ((buf[48] == '0') && (p->p_fuel < 100))
    buf[48] = ' ';
  buf[49] = '0' + (p->p_fuel % 10);
  buf[50] = ' ';
  buf[51] = ' ';

  buf[52] = '0' + ((p->p_wtemp / 10) / 100);
  if (buf[52] == '0')
    buf[52] = ' ';
  buf[53] = '0' + (((p->p_wtemp / 10) % 100) / 10);
  if ((buf[53] == '0') && (p->p_wtemp < 1000))
    buf[53] = ' ';
  buf[54] = '0' + ((p->p_wtemp / 10) % 10);

  buf[55] = ' ';
  buf[56] = ' ';
  buf[57] = ' ';

  buf[58] = '0' + ((p->p_etemp / 10) / 100);
  if (buf[58] == '0')
    buf[58] = ' ';
  buf[59] = '0' + (((p->p_etemp / 10) % 100) / 10);
  if ((buf[59] == '0') && (p->p_etemp < 1000))
    buf[59] = ' ';
  buf[60] = '0' + ((p->p_etemp / 10) % 10);
  buf[61] = ' ';
  buf[62] = ' ';
  buf[63] = ' ';
  {

    buf[64] = FAILED(p, Cloak)   ? 'C' : ' ';
    buf[65] = FAILED(p, LRS)     ? 'L' : ' ';
    buf[66] = FAILED(p, Phaser)  ? 'P' : ' ';
    buf[67] = FAILED(p, SRS)     ? 'S' : ' ';
    buf[68] = FAILED(p, Torp)    ? 'T' : ' ';
    buf[69] = FAILED(p, Cooling) ? 'c' : ' ';
    buf[70] = FAILED(p, Lock)    ? 'l' : ' ';
    buf[71] = FAILED(p, Shield)  ? 's' : ' ';
    buf[72] = FAILED(p, Trans)   ? 't' : ' ';
  }
  buf[73] = '\0';
  /* Draw status line */
  if (self)
  {
    hb = hb1;
    bufpos = 1;
  }
  else
  {
    hb = hb2;
    bufpos = 21;
  }
  if (!j->ts_offset)
    j->ts_offset = (WINSIDE - XTextWidth (j->dfont, hb, STATLINESIZE)) / 2;
  if (!(udcounter % 16))
    XDrawImageString (j->display, j->tstatw, j->dfgc, j->ts_offset, bufpos + j->dfont->ascent, hb, STATLINESIZE);
  if (self)
    bufpos = 11;
  else
    bufpos = 31;
  if (!self || bcmp (j->last_msg, buf, STATLINESIZE) != 0)
  {
    XDrawImageString (j->display, j->tstatw, j->dfgc, j->ts_offset, bufpos + j->dfont->ascent, buf, STATLINESIZE);
    if (self)
      bcopy (buf, j->last_msg, STATLINESIZE);
  }
}

