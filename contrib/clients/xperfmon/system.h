/*
 * $XConsortium: system.h,v 1.4 89/09/19 14:43:58 jim Exp $
 *
 * Perfmon Performance Monitor
 *
 * Copyright 1985, Massachusetts Institute of Technology
 * Copyright 1989 PCS Computer Systeme GmbH, West Germany
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of PCS not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  PCS makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL PCS
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Original Author:  Emanuel Jay Berkenbilt, MIT Project Athena
 * Author:           Thomas A. Baghli, PCS Computer Systeme GmbH, West Germany
 *                   tom@meepmeep.pcs.com
 */



/* This file information needed by both system.c and Perfmon.c */

#define NUM_POSSIBLE_STATS     9
#define MAX_STATS NUM_POSSIBLE_STATS + 1

#define LINES_PER_LABEL 2

#ifdef PCS
#define CPUSTATES	4
#define DK_NDRIVE	4
#define NBPG		4096

#define USER		0
#define NICE		1
#define SYS		2
#define IDLE		3
#endif PCS

#define FORALLPOSSIBLESTATS(stat) \
    for (stat = 0; stat < NUM_POSSIBLE_STATS; stat++)
#define FORALLSTATS(stat) \
    for (stat = 0; stat < num_stats; stat++)

#define	NO_STAT			-1
#define USER_CPU_PERCENTAGE	0
#define SYSTEM_CPU_PERCENTAGE	1
#define IDLE_CPU_PERCENTAGE	2
#define FREE_MEM		3
#define DISK_TRANSFERS		4
#define INTERRUPTS		5
#define INPUT_PACKETS		6
#define OUTPUT_PACKETS		7
#define COLLISION_PACKETS	8

struct statistic {
    int min_val, max_val;
    int value;
    char *label[LINES_PER_LABEL];
};

typedef struct statistic stat_type[MAX_STATS];


/* For setting and adjusting the timer. */

#define HUNDRED 100

/* These are passed to adjust_timeout() and represent
 * numbers of seconds.
 */
#define LARGE_ADD_TIME HUNDRED
#define LARGE_SUBTRACT_TIME -LARGE_ADD_TIME
#define SMALL_ADD_TIME LARGE_ADD_TIME/10
#define SMALL_SUBTRACT_TIME -SMALL_ADD_TIME
#define MAX_TIME 3 * HUNDRED /* five minutes */

#define VALID_UPDATE(x) ((x <= MAX_TIME) && (x > 0))

