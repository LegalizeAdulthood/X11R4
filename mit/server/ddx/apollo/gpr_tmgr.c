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
 * A rudimentary type manager for the keyboard and mouse.  The gpr_io_$get
 * operation is not used to actually get input.  It will only be called from
 * the DOMAIN/OS implementation of the select() call (which is made in
 * WaitForSomething), and only with the preview option.  This allows the
 * server to wake up when an event occurs.
 */

#include "apollo.h"

#include <apollo/ec2.h>
#include <apollo/ios.h>
#include <apollo/io_traits.h>
#include <apollo/trait.h>

#ifndef NO_PHYS_KEYS
typedef short enum {
    cal_$relative, cal_$absolute
} cal_$rel_abs_t;

extern void  cal_$clock_to_timeval (
    cal_$rel_abs_t  &rel_abs,
    time_$clock_t   &clock,
    time_$timeval_t *timeval
);
#endif

#ifdef PRE_SR10_2
typedef short enum {smd_$input_ec, smd_$scroll_blt_ec} smd_$ec_key_t;

extern void * smd_$get_ec
    ( smd_$ec_key_t     & c
    , ec2_$ptr_t        * ecp
    , status_$t         * status
    ) ;
#else
#include <apollo/smdu.h>
#endif

/*
 * The actual GPR event varibles go here.
 */
gpr_$event_t    apEventType;
unsigned char   apEventData[1];
gpr_$position_t apEventPosition;
#ifndef NO_PHYS_KEYS
time_$timeval_t apEventTimestamp;
#endif

/*
 * Pointer to the value field of the GPR eventcount,
 * and pointer to a cell which contains its last known value.
 * These are used by the SetInputCheck trick.
 */
long    *apECV;         /* points to value in GPR_ec */
long    *apLastECV;     /* points to lastGPR_ecValue */

static ec2_$ptr_t   GPR_ec;
static long         lastGPR_ecValue = -1;

static ec2_$ptr_t           SMD_ec;
static ec2_$eventcount_t    dead_ec;

/*
 * GetGPREvent -- Driver internal code
 *      Try to find a GPR input event.
 *      If cond is TRUE, don't block; instead, return TRUE if we got one,
 *      FALSE if we didn't.  If cond is FALSE, block if necessary (and
 *      then return TRUE).
 *      If consume is TRUE, don't return the same event next time.
 *      If consume is FALSE, we must return the same event next time
 *      (i.e. we are previewing the event).
 *
 *      Unfortunately gpr_$[cond_]event_wait always forces the current GPR
 *      bitmap to be the screen bitmap, even though we may have set it to
 *      something else.  Thus we must bend over backwards to put it right again.
 *      We do this by just setting the bitmap ourselves to the screen bitmap
 *      before the event_wait call does it for us, so we can remember what it
 *      used to be.  Then we put it back afterwards.
 *
 *      This approach is reasonably cheap when the current bitmap is actually
 *      already the screen, is cheaper than invalidating and revalidating a GC
 *      when it isn't, and is driver-independent.
 */
Bool
GetGPREvent (cond, consume)
    Bool    cond;
    Bool    consume;
{
    static Bool         needNewData = TRUE;

    long                newGPR_ecValue;
    status_$t           status;
    int                 index;
    ScreenPtr           pScreen;
    apDisplayDataPtr    pDisp;
    gpr_$bitmap_desc_t  valid_bitmap;
#ifndef NO_PHYS_KEYS
    gpr_$event_data_t   buffer;
    short               buf_len;
    time_$clock_t       clock;
#endif

    if (needNewData)
    {
        /* Get new ec value. This only works with level 2 ec's. */
        newGPR_ecValue = GPR_ec->value;

	/* grit teeth and screw around with the current bitmap */
        index = ((apPrivPointrPtr) apPointer->devicePrivate)->numCurScreen;
        pScreen = screenInfo.screens[index];
        pDisp = (apDisplayDataPtr) pScreen->devPrivates[apDisplayScreenIndex].ptr;
        valid_bitmap = apSetBitmap(pDisp->display_bitmap);

        /* make the @#!&% event_wait call */
        if (cond)
            gpr_$cond_event_wait (&apEventType, &apEventData[0], &apEventPosition, &status);
        else
            gpr_$event_wait (&apEventType, &apEventData[0], &apEventPosition, &status);

        /* put back the nice bitmap */
        apSetBitmap(valid_bitmap);

        /* check for nasty failure */
        if (status.all != status_$ok)
            return FALSE;

        /* can get gpr_$no_event only from gpr_$cond_event_wait --
           if so, we are all caught up on events */
        if (apEventType == gpr_$no_event)
        {
            lastGPR_ecValue = newGPR_ecValue;
            return FALSE;
        }
#ifndef NO_PHYS_KEYS
        gpr_$inq_event_data (apEventType, sizeof(buffer), &buffer, &buf_len,
                             &clock, &status);
        cal_$clock_to_timeval (cal_$absolute, clock, &apEventTimestamp);
#endif
    }

    needNewData = consume;
    return TRUE;
}

/*
 * gpr_io_$get -- Driver internal code
 *      Implement the get operation of the ios trait for the GPR type manager.
 */
static int
gpr_io_$get (handle_p_p, options_p, buffer_p, buffer_len_p, status_p)
    char                  **handle_p_p;
    ios_$put_get_opts_t    *options_p;
    char                   *buffer_p;
    long                   *buffer_len_p;
    status_$t              *status_p;
{
    if ((ios_$preview_opt & *options_p) == 0)
        FatalError ("Gack!  Not preview in gpr_io_$get\n");

    if (GetGPREvent ( ((ios_$cond_opt & *options_p) != 0), FALSE))
    {
        status_p->all = status_$ok;
        return 1;
    }
    else
    {
        status_p->all = ios_$get_conditional_failed;
        return 0;
    }
}

/*
 * gpr_io_$put -- Driver internal code
 *      Implement the put operation of the ios trait for the GPR type manager.
 */
static int
gpr_io_$put (handle_p_p, options_p, buffer_p, buffer_len_p, status_p)
    char                  **handle_p_p;
    ios_$put_get_opts_t    *options_p;
    char                   *buffer_p;
    long                   *buffer_len_p;
    status_$t              *status_p;
{
    if ((ios_$preview_opt & *options_p) == 0)
        FatalError ("Gack!  Not preview in gpr_io_$put\n");

    if (GetGPREvent ( ((ios_$cond_opt & *options_p) != 0), FALSE))
    {
        status_p->all = status_$ok;
        return 1;
    }
    else
    {
        status_p->all = ios_$put_conditional_failed;
        return 0;
    }
}

/*
 * gpr_io_$close -- Driver internal code
 *      Implement the close operation of the ios trait for the GPR type manager.
 */
static boolean
gpr_io_$close (handle_p_p, status_p)
    char      **handle_p_p;
    status_$t  *status_p;
{
    status_p->all = status_$ok;
    return false;
}

/*
 * gpr_io_$inq_conn_flags -- Driver internal code
 *      Implement the inq_conn_flags operation of the ios trait for the GPR type manager.
 */
static ios_$conn_flag_set
gpr_io_$inq_conn_flags (handle_p_p, status_p)
    char      **handle_p_p;
    status_$t  *status_p;
{
    status_p->all = status_$ok;
    return (ios_$cf_tty_mask | ios_$cf_vt_mask);
}

/*
 * gpr_io_$get_ec -- Driver internal code
 *      Implement the get_ec operation of the ios trait for the GPR type manager.
 */
static void
gpr_io_$get_ec (handle_p_p, stream_key_p, ecp_p, status_p)
    char          **handle_p_p;
    ios_$ec_key_t  *stream_key_p;
    ec2_$ptr_t     *ecp_p;
    status_$t      *status_p;
{
    status_p->all = status_$ok;

    switch (*stream_key_p)
    {
    case ios_$get_ec_key:
            *ecp_p = GPR_ec;
            break;
    case ios_$put_ec_key:
            *ecp_p = SMD_ec;
            break;
    default:
            status_p->all = ios_$illegal_operation;
            break;
    }
}

/*
 * This is the entry point vector (EPV) implementing the ios trait for the GPR type manager.
 */
#define NULL_PROCEDURE_PTR 0
static io_$epv gpr_io_$epv = {
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        gpr_io_$close,
        gpr_io_$get_ec,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        gpr_io_$inq_conn_flags,
        NULL_PROCEDURE_PTR,
        gpr_io_$get,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        NULL_PROCEDURE_PTR,
        gpr_io_$put
        };

/*
 * MakeGPRStream -- Driver internal code
 *      Initialize the GPR type manager.
 */
int
MakeGPRStream ()
{
    extern Bool     apXSolo;
    static uid_$t   gpr_io_$uid = { 0x3527949C, 0xF0009BB1 };
    status_$t       status;
    io_$epv_ptr     epv_p;

    if (apXSolo)
    {
        smd_$get_ec (smd_$input_ec, &SMD_ec, &status);
        gpr_$get_ec (gpr_$input_ec, &GPR_ec, &status);
    }
    else
    {
        gpr_$get_ec (gpr_$input_ec, &GPR_ec, &status);
        ec2_$init (&dead_ec);
        SMD_ec = &dead_ec;
    }
    
    apECV = &(GPR_ec->value);
    apLastECV = &lastGPR_ecValue;

    epv_p = &gpr_io_$epv;
    trait_$mgr_dcl (gpr_io_$uid, &io_$trait, trait_$kind_local, epv_p, &status);
    return (ios_$connect ("", (short) 0, gpr_io_$uid, (void *)NULL, epv_p, &status) );
}
