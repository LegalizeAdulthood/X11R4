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

#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include "xmoon.h"

/*  JDATE  --  Convert internal GMT date and time to Julian day
	       and fraction.  */
long
jdate( t )
struct tm *t;
{
	long c, m, y;

	y = t->tm_year + 1900;
	m = t->tm_mon + 1;
	if (m > 2)
	   m = m - 3;
	else {
	   m = m + 9;
	   y--;
	}
	c = y / 100L;		   /* Compute century */
	y -= 100L * c;
	return t->tm_mday + (c * 146097L) / 4 + (y * 1461L) / 4 +
	    (m * 153L + 2) / 5 + 1721119L;
}

/* JTIME --    Convert internal GMT date and time to astronomical Julian
	       time (i.e. Julian date plus day fraction, expressed as
	       a double).  */

double
jtime(t)
struct tm *t;
{
	return (jdate(t) - 0.5) + 
	   (t->tm_sec + 60 * (t->tm_min + 60 * t->tm_hour)) / 86400.0;
}

/*  JYEAR  --  Convert Julian date to year, month, day, which are
	       returned via integer pointers to integers.  */

void
jyear(td, yy, mm, dd)
double td;
int *yy, *mm, *dd;
{
	double j, d, y, m;

	td += 0.5;		   /* Astronomical to civil */
	j = floor(td);
	j = j - 1721119.0;
	y = floor(((4 * j) - 1) / 146097.0);
	j = (j * 4.0) - (1.0 + (146097.0 * y));
	d = floor(j / 4.0);
	j = floor(((4.0 * d) + 3.0) / 1461.0);
	d = ((4.0 * d) + 3.0) - (1461.0 * j);
	d = floor((d + 4.0) / 4.0);
	m = floor(((5.0 * d) - 3) / 153.0);
	d = (5.0 * d) - (3.0 + (153.0 * m));
	d = floor((d + 5.0) / 5.0);
	y = (100.0 * y) + j;
	if (m < 10.0)
	   m = m + 3;
	else {
	   m = m - 9;
	   y = y + 1;
	}
	*yy = y;
	*mm = m;
	*dd = d;
}

/*  JHMS  --  Convert Julian time to hour, minutes, and seconds.  */

void
jhms(j, h, m, s)
double j;
int *h, *m, *s;
{
	long ij;

	j += 0.5;		   /* Astronomical to civil */
	ij = (j - floor(j)) * 86400.0;
	*h = ij / 3600L;
	*m = (ij / 60L) % 60L;
	*s = ij % 60L;
}

/*  MEANPHASE  --  Calculates mean phase of the Moon for a given
		   base date and desired phase:
		       0.0   New Moon
		       0.25  First quarter
		       0.5   Full moon
		       0.75  Last quarter
		    Beware!!!  This routine returns meaningless
                    results for any other phase arguments.  Don't
		    attempt to generalise it without understanding
		    that the motion of the moon is far more complicated
		    that this calculation reveals. */

double
meanphase(sdate, phase, usek)
double sdate, phase;
double *usek;
{
	int yy, mm, dd;
	double k, t, t2, t3, nt1;

	jyear(sdate, &yy, &mm, &dd);

	k = (yy + ((mm - 1) * (1.0 / 12.0)) - 1900) * 12.3685;

	/* Time in Julian centuries from 1900 January 0.5 */
	t = (sdate - 2415020.0) / 36525;
	t2 = t * t;		   /* Square for frequent use */
	t3 = t2 * t;		   /* Cube for frequent use */

	*usek = k = floor(k) + phase;
	nt1 = 2415020.75933 + synmonth * k
	      + 0.0001178 * t2
	      - 0.000000155 * t3
	      + 0.00033 * dsin(166.56 + 132.87 * t - 0.009173 * t2);

	return nt1;
}

/*  TRUEPHASE  --  Given a K value used to determine the
		   mean phase of the new moon, and a phase
		   selector (0.0, 0.25, 0.5, 0.75), obtain
		   the true, corrected phase time.  */

double
truephase(k, phase)
double k, phase;
{
	double t, t2, t3, pt, m, mprime, f;
	int apcor = FALSE;
k += phase;		   /* Add phase to new moon time */ t = k / 1236.85;	   /* Time in Julian centuries from
				      1900 January 0.5 */
	t2 = t * t;		   /* Square for frequent use */
	t3 = t2 * t;		   /* Cube for frequent use */
	pt = 2415020.75933	   /* Mean time of phase */
	     + synmonth * k
	     + 0.0001178 * t2
	     - 0.000000155 * t3
	     + 0.00033 * dsin(166.56 + 132.87 * t - 0.009173 * t2);

        m = 359.2242               /* Sun's mean anomaly */
	    + 29.10535608 * k
	    - 0.0000333 * t2
	    - 0.00000347 * t3;
        mprime = 306.0253          /* Moon's mean anomaly */
	    + 385.81691806 * k
	    + 0.0107306 * t2
	    + 0.00001236 * t3;
        f = 21.2964                /* Moon's argument of latitude */
	    + 390.67050646 * k
	    - 0.0016528 * t2
	    - 0.00000239 * t3;
	if ((phase < 0.01) || (abs(phase - 0.5) < 0.01)) {

	   /* Corrections for New and Full Moon */

	   pt +=     (0.1734 - 0.000393 * t) * dsin(m)
		    + 0.0021 * dsin(2 * m)
		    - 0.4068 * dsin(mprime)
		    + 0.0161 * dsin(2 * mprime)
		    - 0.0004 * dsin(3 * mprime)
		    + 0.0104 * dsin(2 * f)
		    - 0.0051 * dsin(m + mprime)
		    - 0.0074 * dsin(m - mprime)
		    + 0.0004 * dsin(2 * f + m)
		    - 0.0004 * dsin(2 * f - m)
		    - 0.0006 * dsin(2 * f + mprime)
		    + 0.0010 * dsin(2 * f - mprime)
		    + 0.0005 * dsin(m + 2 * mprime);
	   apcor = TRUE;
	} else if ((abs(phase - 0.25) < 0.01 || (abs(phase - 0.75) < 0.01))) {
	   pt +=     (0.1721 - 0.0004 * t) * dsin(m)
		    + 0.0021 * dsin(2 * m)
		    - 0.6280 * dsin(mprime)
		    + 0.0089 * dsin(2 * mprime)
		    - 0.0004 * dsin(3 * mprime)
		    + 0.0079 * dsin(2 * f)
		    - 0.0119 * dsin(m + mprime)
		    - 0.0047 * dsin(m - mprime)
		    + 0.0003 * dsin(2 * f + m)
		    - 0.0004 * dsin(2 * f - m)
		    - 0.0006 * dsin(2 * f + mprime)
		    + 0.0021 * dsin(2 * f - mprime)
		    + 0.0003 * dsin(m + 2 * mprime)
		    + 0.0004 * dsin(m - 2 * mprime)
		    - 0.0003 * dsin(2 * m + mprime);
	   if (phase < 0.5)
	      /* First quarter correction */
	      pt += 0.0028 - 0.0004 * dcos(m) + 0.0003 * dcos(mprime);
	   else
	      /* Last quarter correction */
	      pt += -0.0028 + 0.0004 * dcos(m) - 0.0003 * dcos(mprime);
	   apcor = TRUE;
	}
	if (!apcor) {
           fprintf(stderr, "TRUEPHASE called with invalid phase selector.\n");
	   abort();
	}
	return pt;
}

/*  PHASEHUNT  --  Find time of phases of the moon which surround
		   the current date.  Five phases are found, starting
		   and ending with the new moons which bound the
		   current lunation.  */

void phasehunt(sdate, phases)
double sdate;
double phases[5];
{
	double adate, k1, k2, nt1, nt2;

	adate = sdate - 45;
	nt1 = meanphase(adate, 0.0, &k1);
	while (TRUE) {
	   adate += synmonth;
	   nt2 = meanphase(adate, 0.0, &k2);
	   if (nt1 <= sdate && nt2 > sdate)
	      break;
	   nt1 = nt2;
	   k1 = k2;
	}
	phases[0] = truephase(k1, 0.0);
	phases[1] = truephase(k1, 0.25);
	phases[2] = truephase(k1, 0.5);
	phases[3] = truephase(k1, 0.75);
	phases[4] = truephase(k2, 0.0);
}

/*  KEPLER  --	Solve the equation of Kepler.  */

double kepler(m, ecc)
double m, ecc;
{
	double e, delta;
#define EPSILON 1E-6

	e = m = torad(m);
	do {
	   delta = e - ecc * sin(e) - m;
	   e -= delta / (1 - ecc * cos(e));
	} while (abs(delta) > EPSILON);
	return e;
}

/*  PHASE  --  Calculate phase of moon as a fraction:

	The argument is the time for which the phase is requested,
	expressed as a Julian date and fraction.  Returns the terminator
	phase angle as a percentage of a full circle (i.e., 0 to 1),
	and stores into pointer arguments the illuminated fraction of
        the Moon's disc, the Moon's age in days and fraction, the
	distance of the Moon from the centre of the Earth, and the
	angular diameter subtended by the Moon as seen by an observer
	at the centre of the Earth.

*/

double
phase(pdate, pphase, mage, dist, angdia, sudist, suangdia)
double pdate;
double *pphase; 		   /* Illuminated fraction */
double *mage;			   /* Age of moon in days */
double *dist;			   /* Distance in kilometres */
double *angdia; 		   /* Angular diameter in degrees */
double *sudist; 		   /* Distance to Sun */
double *suangdia;                  /* Sun's angular diameter */
{

	double Day, N, M, Ec, Lambdasun, ml, MM, MN, Ev, Ae, A3, MmP,
	       mEc, A4, lP, V, lPP, NP, y, x, Lambdamoon, BetaM,
	       MoonAge, MoonPhase,
	       MoonDist, MoonDFrac, MoonAng, MoonPar,
	       F, SunDist, SunAng;

        /* Calculation of the Sun's position */

	Day = pdate - epoch;	    /* Date within epoch */
	N = fixangle((360 / 365.2422) * Day); /* Mean anomaly of the Sun */
	M = fixangle(N + elonge - elongp);    /* Convert from perigee
				       co-ordinates to epoch 1980.0 */
	Ec = kepler(M, eccent);     /* Solve equation of Kepler */
	Ec = sqrt((1 + eccent) / (1 - eccent)) * tan(Ec / 2);
	Ec = 2 * todeg(atan(Ec));   /* True anomaly */
        Lambdasun = fixangle(Ec + elongp);  /* Sun's geocentric ecliptic
					       longitude */
	/* Orbital distance factor */
	F = ((1 + eccent * cos(torad(Ec))) / (1 - eccent * eccent));
	SunDist = sunsmax / F;	    /* Distance to Sun in km */
        SunAng = F * sunangsiz;     /* Sun's angular size in degrees */


        /* Calculation of the Moon's position */

        /* Moon's mean longitude */
	ml = fixangle(13.1763966 * Day + mmlong);

        /* Moon's mean anomaly */
	MM = fixangle(ml - 0.1114041 * Day - mmlongp);

        /* Moon's ascending node mean longitude */
	MN = fixangle(mlnode - 0.0529539 * Day);

	/* Evection */
	Ev = 1.2739 * sin(torad(2 * (ml - Lambdasun) - MM));

	/* Annual equation */
	Ae = 0.1858 * sin(torad(M));

	/* Correction term */
	A3 = 0.37 * sin(torad(M));

	/* Corrected anomaly */
	MmP = MM + Ev - Ae - A3;

	/* Correction for the equation of the centre */
	mEc = 6.2886 * sin(torad(MmP));

	/* Another correction term */
	A4 = 0.214 * sin(torad(2 * MmP));

	/* Corrected longitude */
	lP = ml + Ev + mEc - Ae + A4;

	/* Variation */
	V = 0.6583 * sin(torad(2 * (lP - Lambdasun)));

	/* True longitude */
	lPP = lP + V;

	/* Corrected longitude of the node */
	NP = MN - 0.16 * sin(torad(M));

	/* Y inclination coordinate */
	y = sin(torad(lPP - NP)) * cos(torad(minc));

	/* X inclination coordinate */
	x = cos(torad(lPP - NP));

	/* Ecliptic longitude */
	Lambdamoon = todeg(atan2(y, x));
	Lambdamoon += NP;

	/* Ecliptic latitude */
	BetaM = todeg(asin(sin(torad(lPP - NP)) * sin(torad(minc))));

	/* Calculation of the phase of the Moon */

	/* Age of the Moon in degrees */
	MoonAge = lPP - Lambdasun;

	/* Phase of the Moon */
	MoonPhase = (1 - cos(torad(MoonAge))) / 2;

	/* Calculate distance of moon from the centre of the Earth */

	MoonDist = (msmax * (1 - mecc * mecc)) /
	   (1 + mecc * cos(torad(MmP + mEc)));

        /* Calculate Moon's angular diameter */

	MoonDFrac = MoonDist / msmax;
	MoonAng = mangsiz / MoonDFrac;

        /* Calculate Moon's parallax */

	MoonPar = mparallax / MoonDFrac;

	*pphase = MoonPhase;
	*mage = synmonth * (fixangle(MoonAge) / 360.0);
	*dist = MoonDist;
	*angdia = MoonAng;
	*sudist = SunDist;
	*suangdia = SunAng;
	return fixangle(MoonAge) / 360.0;
}
