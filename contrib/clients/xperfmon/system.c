/*
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

/* This file contains only system functions - that is the functions that
 * get the information the performance monitor is monitoring.  No calls
 * to any X routines should be made here.  The reason for doing this is
 * so that as the X toolkit becomes available and the X window system 
 * improves no changes will have to be made to this file, and as this
 * program is made available for a new type of machine, only this file
 * will need to be changed.
 */

#include "system.h"

#include <stdio.h>
#include <strings.h>

#ifdef PCS
#include <sys/types.h>
#include <sys/sysinfo.h>
#endif /* PCS */
#include <sys/param.h>
#include <sys/socket.h>
#ifndef PCS
#include <sys/vm.h>
#include <sys/dk.h>
#endif /* !PCS */
#include <nlist.h>
#include <sys/buf.h>
#ifdef vax
#include <vaxuba/ubavar.h>
#include <vaxmba/mbavar.h>
#endif vax
#ifdef sun
#include <sundev/mbvar.h>
#endif sun
#ifdef ibm032	/* IBM RT/PC */
#include <caio/ioccvar.h>
#endif ibm032
#include <net/if.h>
#include <netinet/in.h>
#include <sys/file.h>

struct packet {
    int input, output, collisions;
};
static struct packet packets, old_packets;

/*
 * The array stats always has valid info for stats[i], 0 <= i < num_stats.
 * The info for the k-th possible statistic of interest is recorded, if it
 * is recorded at all, in stats[poss_stats[k]].
 */

int poss_stats[NUM_POSSIBLE_STATS];
#define WANT_STAT(x) (poss_stats[(x)] != NO_STAT)

stat_type stats;

static int num_stats;

#define max(a,b) (a>b ? a:b)

#if defined(PCS) && !defined(RCU) /* PCS M68 */
struct nlist nl[] = {
#define X_SYSINFO	0
	{ "_sysinfo" },
#define X_MAXFREE       1
	{ "_maxfree" },
#define X_FREEMEM	2
	{ "_freemem"},
#define X_BOOTTIME      3
	{ "_bootime" },
#define X_HZ		4
	{ "_hz" },
#define N_IFNET		5
	{ "_ifnet" },
	{ "" },
};
#else /* PCS && !RCU */
#if defined(PCS) && defined(RCU) /* PCS RCU */
struct nlist nl[] = {
#define X_SYSINFO	0
	{ "sysinfo" },
#define X_MAXFREE       1
	{ "maxfree" },
#define X_FREEMEM	2
	{ "freemem"},
#define X_BOOTTIME      3
	{ "bootime" },
#define X_HZ		4
	{ "hz" },
#define N_IFNET		5
	{ "ifnet" },
	{ "" },
};
#else /* PCS && RCU */
struct nlist nl[] = {
#define X_CPTIME	0
	{ "_cp_time" },
#define X_RATE	  1
	{ "_rate" },
#define X_TOTAL	 2
	{ "_total" },
#define X_DEFICIT       3
	{ "_deficit" },
#define X_FORKSTAT      4
	{ "_forkstat" },
#define X_SUM	   5
	{ "_sum" },
#define X_FIRSTFREE     6
	{ "_firstfree" },
#define X_MAXFREE       7
	{ "_maxfree" },
#define X_BOOTTIME      8
	{ "_boottime" },
#define X_DKXFER	9
	{ "_dk_xfer" },
#define X_REC	   10
	{ "_rectime" },
#define X_PGIN	  11
	{ "_pgintime" },
#define X_HZ	    12
	{ "_hz" },
#define X_MBDINIT       13
	{ "_mbdinit" },
#define N_IFNET       14
	{ "_ifnet" },
#define X_UBDINIT	15
	{ "_ubdinit" },
#define X_IOCINIT	16
	{ "_ioccdinit" },
	{ "" },
};
#endif /* PCS && RCU */
#endif /* PCS && !RCU */

char dr_name[DK_NDRIVE][10];
char dr_unit[DK_NDRIVE];
double  stat1();
int     maxfree;
int     hz;
#ifdef PCS
struct vmtotal
{
     long t_free;
};
#endif /* PCS */
struct {
	int     busy;
	long    time[CPUSTATES];
	long    xfer[DK_NDRIVE];
	struct  vmtotal Total;
#ifndef PCS
	struct  vmmeter Sum;
	struct  vmmeter Rate;
	struct  forkstat Forkstat;
	unsigned rectime;
	unsigned pgintime;
#endif /* !PCS */
} 
s, s1;
#define total s.Total
#ifndef PCS
#define rate s.Rate
#define sum  s.Sum
#define forksta	s.Forkstat

struct  vmmeter osum;
int     deficit;
#endif /* !PCS */
double  etime;
int     mf;
int     swflag;

int nintv;
long t;

#define steal(where, var) lseek(mf, where, 0); read(mf, &var, sizeof var);
#define pgtok(a) ((a)*NBPG/1024)

/* From X11/Intrinsic.h */
typedef char    Boolean;

int
sys_setup(sys_params_used)
    Boolean *sys_params_used;
/* This routine does all necessary setting up of structures
 * that will handle system calls.  It returns num_stats.
 */
{
    int stat;
    int have_disk;
    double atof();
  
#ifdef PCS
    nintv = get_namelist("/unix", "/dev/kmem");
#else /* !PCS */
    nintv = get_namelist("/vmunix", "/dev/kmem");
#endif /* PCS */
    collect_stats();
    etime = 1.0;
    have_disk = (total_disk_transfers() ? 1 : 0);
  
    /* Initialize stats */
    FORALLPOSSIBLESTATS(stat)
	poss_stats[stat] = NO_STAT;
    num_stats = 0;
  
    if (num_stats == 0)
	FORALLPOSSIBLESTATS(stat) {
	    if ((stat == DISK_TRANSFERS) && (have_disk == 0)) 
		continue;
	    if (sys_params_used[stat])
		poss_stats[stat] = num_stats++;
	    if (num_stats == MAX_STATS) 
		break;
	}
    have_disk = 0;	/* so max # of packets = 40 */
    init_stat(USER_CPU_PERCENTAGE, 0, 100, "User", " CPU");
    init_stat(SYSTEM_CPU_PERCENTAGE, 0, 100, "System", " CPU");
    init_stat(IDLE_CPU_PERCENTAGE, 0, 100, "Idle", " CPU");
    init_stat(FREE_MEM, 0, pgtok(maxfree), "Free", " memory");
    init_stat(DISK_TRANSFERS, 0, 40, "Disk", " transfers");
    init_stat(INTERRUPTS, 0, 60, "Interrupts", "");
    init_stat(INPUT_PACKETS, 0, (have_disk ? 20 : 40), "Input", " packets");
    init_stat(OUTPUT_PACKETS, 0, (have_disk ? 20 : 40), "Output", " packets");
    init_stat(COLLISION_PACKETS, 0, 10, "Collision", " packets");
  
    return(num_stats);
}

void
update_stats()
{
    int *target[CPUSTATES-1], trash;

    collect_stats();
    for (trash = 0; trash < CPUSTATES-1; trash++)
	target[trash] = &trash;
    if WANT_STAT(USER_CPU_PERCENTAGE)
	target[0] = &stats[poss_stats[USER_CPU_PERCENTAGE]].value;
    if WANT_STAT(SYSTEM_CPU_PERCENTAGE)
	target[1] = &stats[poss_stats[SYSTEM_CPU_PERCENTAGE]].value;
    if WANT_STAT(IDLE_CPU_PERCENTAGE)
	target[2] = &stats[poss_stats[IDLE_CPU_PERCENTAGE]].value;
    copy_cpu_stats(target);
    if WANT_STAT(FREE_MEM)
	stats[poss_stats[FREE_MEM]].value = pgtok(total.t_free);
#ifndef PCS
    if WANT_STAT(DISK_TRANSFERS)
	stats[poss_stats[DISK_TRANSFERS]].value = total_disk_transfers();
    if WANT_STAT(INTERRUPTS)
	stats[poss_stats[INTERRUPTS]].value = (rate.v_intr/nintv) - hz;
    if WANT_STAT(INPUT_PACKETS)
	stats[poss_stats[INPUT_PACKETS]].value
	    = packets.input - old_packets.input;
    if WANT_STAT(OUTPUT_PACKETS)
	stats[poss_stats[OUTPUT_PACKETS]].value
	    = packets.output - old_packets.output;
    if WANT_STAT(COLLISION_PACKETS)
	stats[poss_stats[COLLISION_PACKETS]].value
	    = packets.collisions - old_packets.collisions;
#endif /* !PCS */
}

init_stat(index, minval, maxval, label_1, label_2)
    int index, minval, maxval;
    char *label_1, *label_2;
{
    if WANT_STAT(index) {
	index = poss_stats[index];
	stats[index].min_val = minval;
	stats[index].max_val = maxval;
	stats[index].label[0] = label_1;
	stats[index].label[1] = label_2;
    }
}

#define	TIMER_EXPIRED(timer) \
    (*timer && ((*timer)->tv_sec == 0) && ((*timer)->tv_usec == 0))

int
total_disk_transfers()
{
    register int i, total_xfers = 0;

    for(i=0; i < DK_NDRIVE; i++)
	total_xfers += s.xfer[i];
    return(total_xfers/etime);
}

copy_cpu_stats(stat)
    int *stat[CPUSTATES-1];
{
    register int i;

    for(i=0; i<CPUSTATES; i++) {
	float f = stat1(i);
	if (i == 0) {           /* US+NI */
	    i++;
	    f += stat1(i);
	}
	if (stat[i-1] != 0)
	    *stat[i-1] = f;
    }
}

collect_stats()
{
    off_t ifnetaddr = (long)nl[N_IFNET].n_value;
    register int i;

#ifdef PCS
    long memspace[2];
    unsigned long cpu_time[5];

    lseek(mf, (long)nl[X_SYSINFO].n_value, 0);
    read(mf, (char *)cpu_time, sizeof cpu_time);

    s.time[USER] = cpu_time[CPU_USER];
    s.time[NICE] = 0;
    s.time[SYS] = cpu_time[CPU_KERNEL];
    s.time[IDLE] = cpu_time[CPU_IDLE] + cpu_time[CPU_WAIT] +
	cpu_time[CPU_SXBRK];

    lseek(mf, (long)nl[X_FREEMEM].n_value, 0);
    read(mf, memspace, sizeof memspace);
    s.Total.t_free = memspace[0];
#else /* !PCS */
    lseek(mf, (long)nl[X_CPTIME].n_value, 0);
    read(mf, s.time, sizeof s.time);
    lseek(mf, (long)nl[X_DKXFER].n_value, 0);
    read(mf, s.xfer, sizeof s.xfer);
    if (nintv != 1) {
	steal((long)nl[X_SUM].n_value, rate);
    } else {
	steal((long)nl[X_RATE].n_value, rate);
    }
    steal((long)nl[X_TOTAL].n_value, total);
    osum = sum;
    steal((long)nl[X_SUM].n_value, sum);
    steal((long)nl[X_DEFICIT].n_value, deficit);
#endif /* PCS */
    etime = 0;
    for (i=0; i < DK_NDRIVE; i++) {
	t = s.xfer[i];
	s.xfer[i] -= s1.xfer[i];
	s1.xfer[i] = t;
    }
    for (i=0; i < CPUSTATES; i++) {
	t = s.time[i];
	s.time[i] -= s1.time[i];
	s1.time[i] = t;
	etime += s.time[i];
    }
    if(etime == 0.)
	etime = 1.;
    etime /= 60.;
    nintv = 1;

    if (nl[N_IFNET].n_value != 0) {
	struct ifnet ifnet;
	steal((long)nl[N_IFNET].n_value, ifnetaddr);
	old_packets = packets;
	packets.input = packets.output = packets.collisions = 0;
	while (ifnetaddr) {
	    steal(ifnetaddr, ifnet);
	    packets.input += ifnet.if_ipackets;
	    packets.output += ifnet.if_opackets;
	    packets.collisions += ifnet.if_collisions;
	    ifnetaddr = (off_t) ifnet.if_next;
	}
    }

}

min (a, b)
    int a,b;
{
    return(a<b ? a:b);
}

int
get_namelist(kernel_name, memory_name)
    char *kernel_name, *memory_name;
{
    time_t now;
    time_t boottime;
    register int i;
    int nintv;

    nlist(kernel_name, nl);
#ifdef PCS
    if(nl[0].n_type == 0 && nl[0].n_value == 0) {
#else /* !PCS */
    if(nl[0].n_type == 0) {
#endif /* PCS */
	fprintf(stderr, "no %s namelist\n", kernel_name);
	exit(1);
    }
    mf = open(memory_name, 0);
    if (mf < 0) {
	fprintf(stderr, "cannot open %s\n", memory_name);
	exit(1);
    }
    steal((long)nl[X_MAXFREE].n_value, maxfree);
    steal((long)nl[X_BOOTTIME].n_value, boottime);
    steal((long)nl[X_HZ].n_value, hz);
    for (i = 0; i < DK_NDRIVE; i++) {
	strcpy(dr_name[i], "xx");
	dr_unit[i] = i;
    }
#ifndef PCS
    /* I don't see a way to obtain the information that
     * read_names gets so it is commented it out for now.
     */
    read_names();
#endif /* !PCS */
    time(&now);
    nintv = now - boottime;
    if (nintv <= 0 || nintv > 60*60*24*365*10) {
	fprintf(stderr,
	    "Time makes no sense... namelist must be wrong.\n");
	exit(1);
    }
    return(nintv);
}

double
stat1(row)
{
    double t;
    register i;

    t = 0;
    for(i=0; i<CPUSTATES; i++)
	t += s.time[i];
    if(t == 0.)
	t = 1.;
    return(s.time[row]*100./t);
}

#ifdef vax
read_names()
{
    struct mba_device mdev;
    register struct mba_device *mp;
    struct mba_driver mdrv;
    short two_char;
    char *cp = (char *) &two_char;
    struct uba_device udev, *up;
    struct uba_driver udrv;

    mp = (struct mba_device *) nl[X_MBDINIT].n_value;
    up = (struct uba_device *) nl[X_UBDINIT].n_value;
    if (up == 0) {
	fprintf(stderr, "perfmon: Disk init info not in namelist\n");
	exit(1);
    }
    if(mp) for (;;) {
	steal(mp++, mdev);
	if (mdev.mi_driver == 0)
	    break;
	if (mdev.mi_dk < 0 || mdev.mi_alive == 0)
	    continue;
	steal(mdev.mi_driver, mdrv);
	steal(mdrv.md_dname, two_char);
	sprintf(dr_name[mdev.mi_dk], "%c%c", cp[0], cp[1]);
	dr_unit[mdev.mi_dk] = mdev.mi_unit;
    }
    if(up) for (;;) {
	steal(up++, udev);
	if (udev.ui_driver == 0)
	    break;
	if (udev.ui_dk < 0 || udev.ui_alive == 0)
	    continue;
	steal(udev.ui_driver, udrv);
	steal(udrv.ud_dname, two_char);
	sprintf(dr_name[udev.ui_dk], "%c%c", cp[0], cp[1]);
	dr_unit[udev.ui_dk] = udev.ui_unit;
    }
}
#endif vax

#ifdef sun
read_names()
{
   struct mb_device mdev;
   register struct mb_device *mp;
   struct mb_driver mdrv;
   short two_char;
   char *cp = (char *) &two_char;

   mp = (struct mb_device *) nl[X_MBDINIT].n_value;
   if (mp == 0) {
	fprintf(stderr, "vmstat: Disk init info not in namelist\n");
	exit(1);
   }
   for (;;) {
	steal(mp++, mdev);
	if (mdev.md_driver == 0)
	    break;
	if (mdev.md_dk < 0 || mdev.md_alive == 0)
	    continue;
	steal(mdev.md_driver, mdrv);
	steal(mdrv.mdr_dname, two_char);
	sprintf(dr_name[mdev.md_dk], "%c%c", cp[0], cp[1]);
	dr_unit[mdev.md_dk] = mdev.md_unit;
   }
}
#endif sun

#ifdef ibm032
read_names()
{
    struct iocc_device mdev;
    register struct iocc_device *mp;
    struct iocc_driver mdrv;
    short two_char;
    char *cp = (char *) &two_char;

    mp = (struct iocc_device *) nl[X_IOCINIT].n_value;
    if (mp == 0) {
	fprintf(stderr, "vmstat: Disk init info not in namelist\n");
	exit(1);
    }
    for (;;) {
	steal(mp++, mdev);
	if (mdev.iod_driver == 0)
	    break;
	if (mdev.iod_dk < 0 || mdev.iod_alive == 0)
	    continue;
	steal(mdev.iod_driver, mdrv);
	steal(mdrv.idr_dname, two_char);
	sprintf(dr_name[mdev.iod_dk], "%c%c", cp[0], cp[1]);
	dr_unit[mdev.iod_dk] = mdev.iod_unit;
    }
}
#endif ibm032
