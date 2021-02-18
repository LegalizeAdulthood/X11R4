/*
 * Copyright 1989 Waseda University.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Waseda University not be used
 * in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.
 * Waseda University makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * WASEDA DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL WASEDA
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:	Nobuhiko Nakajima
 *		Waseda University
 *
 *	61-505, Sci&Eng department, Waseda University
 *	3-1-9 shinjyuku-ku ookubo, Tokyo, Japan	169
 *	(03) 232-0551
 *
 *	Junet	hiko@ohara.waseda.ac.jp
 */
/*

    A Moon for the Sun

    Release 2.0

    Designed and implemented by John Walker in December 1987,
    revised and updated in February of 1988.

    Make with:

    Adding  appropriate  floating  point  options  to your hardware.  This
    program is a SunView tool which displays, as the  icon  for  a  closed
    window,  the  current phase of the Moon.  A subtitle in the icon gives
    the age of the Moon in days  and  hours.   If  called  with  the  "-t"
    switch,  it  rapidly  increments  forward  through time to display the
    cycle of phases.

    If you open the window, additional information is displayed  regarding
    the  Moon.	 The  information  is  generally  accurate  to	within ten
    minutes.

    The algorithms used in this program to calculate the positions Sun and
    Moon as seen from the Earth are given in the book "Practical Astronomy
    With  Your  Calculator"  by  Peter  Duffett-Smith,   Second   Edition,
    Cambridge University Press, 1981.  Ignore the word "Calculator" in the
    title;  this  is  an  essential  reference  if  you're  interested  in
    developing	software  which  calculates  planetary	positions, orbits,
    eclipses, and  the  like.   If  you're  interested  in  pursuing  such
    programming, you should also obtain:

    "Astronomical  Formulae for Calculators" by Jean Meeus, Third Edition,
    Willmann-Bell, 1985.  A must-have.

    "Planetary  Programs  and  Tables  from  -4000  to  +2800"  by  Pierre
    Bretagnon  and Jean-Louis Simon, Willmann-Bell, 1986.  If you want the
    utmost  (outside  of  JPL)  accuracy  for  the  planets,  it's   here.

    "Celestial BASIC" by Eric Burgess, Revised Edition, Sybex, 1985.  Very
    cookbook oriented, and many of the algorithms are hard to dig  out	of
    the turgid BASIC code, but you'll probably want it anyway.

    Many of these references can be obtained from Willmann-Bell, P.O.  Box
    35025,  Richmond,  VA 23235, USA.  Phone: (804) 320-7016.  In addition
    to their own publications, they stock most of the standard	references
    for mathematical and positional astronomy.

    This program was written by:

       John Walker
       Autodesk, Inc.
       2320 Marinship Way
       Sausalito, CA  94965
       (415) 332-2344 Ext. 829

       Usenet: {sun!well}!acad!kelvin

    This  program is in the public domain: "Do what thou wilt shall be the
    whole of the law".  I'd appreciate  receiving  any  bug  fixes  and/or
    enhancements,  which  I'll  incorporate  in  future  versions  of  the
    program.  Please leave the original attribution information intact	so
    that credit and blame may be properly apportioned.

*/

/*  Astronomical constants  */

#define epoch	    2444238.5	   /* 1980 January 0.0 */

/*  Constants defining the Sun's apparent orbit  */

#define elonge	    278.833540	   /* Ecliptic longitude of the Sun
				      at epoch 1980.0 */
#define elongp	    282.596403	   /* Ecliptic longitude of the Sun at
				      perigee */
#define eccent      0.016718       /* Eccentricity of Earth's orbit */
#define sunsmax     1.495985e8     /* Semi-major axis of Earth's orbit, km */
#define sunangsiz   0.533128       /* Sun's angular size, degrees, at
				      semi-major axis distance */

/*  Elements of the Moon's orbit, epoch 1980.0  */

#define mmlong      64.975464      /* Moon's mean lonigitude at the epoch */
#define mmlongp     349.383063	   /* Mean longitude of the perigee at the
				      epoch */
#define mlnode	    151.950429	   /* Mean longitude of the node at the
				      epoch */
#define minc        5.145396       /* Inclination of the Moon's orbit */
#define mecc        0.054900       /* Eccentricity of the Moon's orbit */
#define mangsiz     0.5181         /* Moon's angular size at distance a
				      from Earth */
#define msmax       384401.0       /* Semi-major axis of Moon's orbit in km */
#define mparallax   0.9507	   /* Parallax at distance a from Earth */
#define synmonth    29.53058868    /* Synodic month (new Moon to new Moon) */
#define lunatbase   2423436.0      /* Base date for E. W. Brown's numbered
				      series of lunations (1923 January 16) */

/*  Properties of the Earth  */

#define earthrad    6378.16	   /* Radius of Earth in kilometres */

/* Icon definition.  This is just a black field with rounded corners
   that blend into the root desktop pattern. The image of the moon and
   the text are added by the program later. */

#define MoonWidth 64
#define MoonHeight 64

#define FALSE 0
#define TRUE 1

#define PI 3.14159265358979323846  /* Assume not near black hole nor in
				      Tennessee */

/*  Handy mathematical functions  */

#define sgn(x) (((x) < 0) ? -1 : ((x) > 0 ? 1 : 0))	  /* Extract sign */
#define abs(x) ((x) < 0 ? (-(x)) : (x)) 		  /* Absolute val */
#define fixangle(a) ((a) - 360.0 * (floor((a) / 360.0)))  /* Fix angle	  */
#define torad(d) ((d) * (PI / 180.0))			  /* Deg->Rad	  */
#define todeg(d) ((d) * (180.0 / PI))			  /* Rad->Deg	  */
#define dsin(x) (sin(torad((x))))			  /* Sin from deg */
#define dcos(x) (cos(torad((x))))			  /* Cos from deg */
