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
 * Functions implementing Apollo-display-independent parts of the driver
 * having to do with driver initialization.
 */

#include "apollo.h"
#include "servermd.h"

extern char *index();

/*
 * function prototypes and type defs for unreleased but used routines
 */
#ifdef PRE_SR10_2
typedef linteger gpr_$rect_id_t;
#endif

typedef struct {
        short unit;     /*unit number, currently must be 0*/
        short screen;   /*screen (head) number*/
} aws_$display_t;

extern void gpr_$init_wm (gpr_$display_mode_t &op,
                          stream_$id_t &unit_or_pad,
                          gpr_$offset_t &size,
                          gpr_$rgb_plane_t &hi_plane,
                          gpr_$bitmap_desc_t *init_bitmap,
                          status_$t *st);

extern void gpr_$free_wm(status_$t *st);

extern void *aws_$init(aws_$display_t &display,
                         gpr_$rect_id_t &rect,
                         status_$t *st);

extern gpr_$rect_id_t gpr_$$inq_rectangle_id(gpr_$bitmap_desc_t *init_bitmap,
                                            status_$t *st);

/*
 * defns for argument processing
 */

#define MIN_DISPLAY_UNIT 1
#define MAX_DISPLAY_UNIT 2
#define MAX_UNITS (MAX_DISPLAY_UNIT-MIN_DISPLAY_UNIT+1)

static char nullArgString[] = "";

/*
 * Statics holding info passed from InitOutput to apUnitScreenInit.
 * This hackery exists because InitOutput does not call apUnitScreenInit
 * directly, rather it passes the address of apUnitScreenInit to AddScreen,
 * which is the actual caller.  Thus we have no control over arguments passed
 * to apUnitScreenInit; they must be the standard ones.  So we store the stuff
 * we really want to pass here.
 */

typedef struct _Unit
{
    short               unit;           /* OS unit number */
    short               actualDepth;    /* actual depth to use for this unit */
    int                 driverType;     /* MFB_DRIVER or GPR_DRIVER */
    char               *argString;      /* pointer to arg string for this unit, or null string if none given */
    gpr_$disp_char_t    dispChar;       /* display characteristics for this unit */
} UnitRec;

static int      nUnits = 0;
static UnitRec  unitList[MAX_UNITS];
static int      currentUnitIndex;

/*
 * Put the actual display data here.
 */
apDisplayDataRec _apDisplayData[MAX_UNITS];

/*
 * Screen privates for display data go here.
 */
int                     apDisplayScreenIndex;
static unsigned long    apGeneration = 0;

/*
 * This flag is set when the server is running in place of the DM
 * (this requires us to do some extra initialization).
 */
Bool    apXSolo;

/*
 * We put the pointer to a screen wanting mi cursors here.
 */
ScreenPtr               apMICursorScreen = NULL;

/*
 * List of externs for the various display type initialization routines.
 */
extern Bool     apMonoScreenInit();
extern Bool     apClrScreenInit();

/*
 * apUnitScreenInit -- DDX interface (screen)
 *      Try to initialize the display described in the above unitList, at index
 *      given by currentUnitIndex, by figuring out which Apollo display-dependent
 *      initializer to call.
 */
static Bool
apUnitScreenInit(screenIndex, pScreen, argc, argv)
    int         screenIndex;
    ScreenPtr   pScreen;
    int         argc;   /* these two may NOT be changed */
    char      **argv;
{
    apDisplayDataPtr    pDisp;
    UnitRec            *pUnit;
    char               *arg_p;
    Bool                retval;

    pUnit = &unitList[currentUnitIndex];

    if ((pUnit->dispChar.controller_type) == gpr_$ctl_none)
            /* whoops, made a BIG mistake! No display here! */
        return (FALSE);

    pDisp = &_apDisplayData[currentUnitIndex];
    pScreen->devPrivates[apDisplayScreenIndex].ptr = (pointer) pDisp;

    pDisp->display_unit = pUnit->unit;
    pDisp->display_char = pUnit->dispChar;
    pDisp->depth = pUnit->actualDepth;
    pDisp->driver_type = pUnit->driverType;

/*
 *  Init gpr -
 *      Given a pointer to the display data record, do all the screen
 *      hardware initialization work.
 */
    if (!(pDisp->bitmap_ptr))
    {
        status_$t           status;
        gpr_$offset_t       disp;
        short               wpl;
        short               n_entries;
        gpr_$pixel_value_t  start_index;
        gpr_$color_vector_t colors;
        gpr_$rgb_plane_t    hi_plane;
        aws_$display_t      scr;

        scr.unit = 0;
        scr.screen = pDisp->display_unit - 1;

        disp.x_size = pDisp->display_char.x_visible_size;
        disp.y_size = pDisp->display_char.y_visible_size;

        hi_plane = pDisp->depth - 1;
        
        gpr_$init_wm (gpr_$borrow, (short)(pDisp->display_unit),
                      disp, hi_plane, &pDisp->display_bitmap, &status);

        apXSolo = (status.all == status_$ok);
        if (apXSolo)
        {
            gpr_$rect_id_t  rectID;
            
            /* Get rectangle ID from gpr bitmap for root rectangle */
            rectID = gpr_$$inq_rectangle_id (&pDisp->display_bitmap, &status);
            if (status.all != status_$ok)
            {
                xfree (pDisp);
                return FALSE;
            }
            
            /* Init aws */
            (void) aws_$init (scr, rectID, &status);
            if (status.all != status_$ok)
            {
                ErrorF("apUnitScreenInit: aws_$init failed. %08x\n", status.all);
                xfree (pDisp);
                return FALSE;
            }
        }
        else if (status.all == gpr_$already_initialized )
            gpr_$init (gpr_$borrow, (short)(pDisp->display_unit),
                       disp, hi_plane, &pDisp->display_bitmap, &status);

        if (status.all != status_$ok)
        {
            ErrorF("apUnitScreenInit: gpr_$init failed. %08x\n", status.all);
            xfree (pDisp);
            return FALSE;
        }

        colors[0] = gpr_$black;
        colors[1] = gpr_$white;
        start_index = 0;
        n_entries = 2;
        gpr_$set_color_map (start_index, n_entries, colors, &status);
        if (status.all != status_$ok)
        {
            xfree (pDisp);
            return FALSE;
        }

        gpr_$inq_bitmap_pointer (pDisp->display_bitmap, (char **) &pDisp->bitmap_ptr, &wpl, &status);
        if (status.all != status_$ok)
        {
            xfree (pDisp);
            return FALSE;
        }
        pDisp->words_per_line = wpl;
        
        gpr_$enable_direct_access (&status);
        if (status.all != status_$ok)
        {
            xfree (pDisp);
            return FALSE;
        }
    }
    
/*
 *  Here we call the proper ScreenInit, depending on the driver selected.
 */
 
    switch (pDisp->driver_type)
    {
    case MFB_DRIVER:
        return apMonoScreenInit(screenIndex, pScreen, argc, argv);
    case GPR_DRIVER:
        return apClrScreenInit(screenIndex, pScreen, argc, argv);
    default:
        xfree (pDisp);
        return FALSE;
    }
}

/*
 * InitOutput -- DDX interface (server)
 *      Establish the pixmap bit and byte orders, and scanline data,
 *      and the supported pixmap formats.
 *
 *      Then try to initialize screens.  If no "-D" options were given
 *      on the command line, initialize all display units that exist,
 *      using the default screen behaviors.
 *      If there are any -D options, initialize only the units explicitly
 *      named.
 *
 *      We save our work in static data, so that on the second and subsequent
 *      times through, we skip the argument parsing and hardware inquiry.
 *      However, we do reconstruct the format list each time through.  Why?
 *      Because we have to copy it to the *pScreenInfo every time anyway, so
 *      we wouldn't save much by squirreling it away.
 */
InitOutput(pScreenInfo, argc, argv)
    ScreenInfo *pScreenInfo;
    int         argc;
    char       *argv[];
{
    int         i, j, depth, num_formats;
    char       *arg_p;
    short       unit;
    short       disp_len_ret;
    status_$t   status;

    if (!nUnits)        /* First time, must parse args and look for hardware */
    {

/*
 * Try to find -D<unit> options, and write down the unit number and a pointer to the arg string
 * (or a null string if there is no arg string) for each one found.
 *
 * Argument syntax recognized here is:
 *      <arg> ::= -D<unit> <arg-string>
 *      <arg-string> ::= { d<depth> | gpr<depth> | mfb }
 * where <unit> is an integer representing the display hardware unit number (1 or greater).
 * and <depth> is an integer representing the number of bitplanes to use (up to the number
 * actually provided by the hardware).  Only depths of "reasonable" round numbers are supported.
 * d<depth> and gpr<depth> both mean to use the GPR driver.  mfb means to use the mfb driver.
 * For example,
 *      -D1 d4
 * means initialize display unit 1 to depth 4, using GPR.
 */
        for (i = 1; i < argc; i++)
        {
            if ((argv[i][0] ==  '-') && (argv[i][1] == 'D'))
            {
                unit = unitList[nUnits].unit = atoi(argv[i]+2);
                if ((unit >= MIN_DISPLAY_UNIT) &&
                    (unit <= MAX_DISPLAY_UNIT))
                {
                    if (++i >= argc)
                        unitList[nUnits].argString = nullArgString;
                    else if (argv[i][0] == '-')
                    {
                        unitList[nUnits].argString = nullArgString;
                        i--;
                    }
                    else
                        unitList[nUnits].argString = argv[i];
                    gpr_$inq_disp_characteristics (gpr_$borrow, unit, sizeof(gpr_$disp_char_t),
                                                   &unitList[nUnits].dispChar, &disp_len_ret, &status);
                    if (unitList[nUnits].dispChar.controller_type != gpr_$ctl_none)
                        nUnits++;
                }
            }
        }

/*
 * If we didn't find any -D<unit> options, find and enter all hardware units into
 * the unit list.
 */
        if (!nUnits)
        {
            for (unit = MIN_DISPLAY_UNIT; unit <= MAX_DISPLAY_UNIT; unit++)
            {
                gpr_$inq_disp_characteristics (gpr_$borrow, unit, sizeof(gpr_$disp_char_t),
                                               &unitList[nUnits].dispChar, &disp_len_ret, &status);
                if (unitList[nUnits].dispChar.controller_type != gpr_$ctl_none)
                {
                    unitList[nUnits].unit = unit;
                    unitList[nUnits].argString = nullArgString;
                    nUnits++;
                }
            }
        }

/*
 * OK, now figure out the desired driver and depth for each unit.  By default, that's
 * GPR and the full depth of the device.  A "d<depth>" or "gpr<depth>" argument may give
 * a different depth, which will be reduced to the actual depth, and further reduced
 * to a "reasonable" value of 8, 4, or 1 if it isn't already.  An "mfb" argument means
 * to use mfb (obviously with a depth of 1).
 */
        for (i = 0; i < nUnits; i++)
        {
            depth = min(MAX_DEPTH_SUPPORTED, unitList[i].dispChar.n_planes);
            if (strcmp(unitList[i].argString, "mfb") == 0)
            {
                unitList[i].driverType = MFB_DRIVER;
                depth = 1;
            }
            else
            {
                unitList[i].driverType = GPR_DRIVER;
                if (strncmp(unitList[i].argString, "d", 1) == 0)
                {
                    arg_p = unitList[i].argString + 1;
                    depth = min(depth, atoi(arg_p));
    		}
                else if (strncmp(unitList[i].argString, "gpr", 3) == 0)
                {
                    arg_p = unitList[i].argString + 3;
                    depth = min(depth, atoi(arg_p));
    		}
            }
            depth &= ~(0x3);
            if (!depth) depth = 1;
            unitList[i].actualDepth = depth;
        }
    }
/*
 * At this point, we have a valid set of unit data in our trusty static storage.
 */

/*
 * Set up constant *pScreenInfo stuff, including the first format (bitmap).
 */
    pScreenInfo->imageByteOrder = IMAGE_BYTE_ORDER;
    pScreenInfo->bitmapScanlineUnit = BITMAP_SCANLINE_UNIT;
    pScreenInfo->bitmapScanlinePad = BITMAP_SCANLINE_PAD;
    pScreenInfo->bitmapBitOrder = BITMAP_BIT_ORDER;

    pScreenInfo->formats[0].depth = 1;
    pScreenInfo->formats[0].bitsPerPixel = 1;
    pScreenInfo->formats[0].scanlinePad = BITMAP_SCANLINE_PAD;
    num_formats = 1;

/*
 * Based on the unit list, fill in any additional pixmap formats supported.
 */

    for (i = 0; i < nUnits; i++)
    {
        depth = unitList[i].actualDepth;
        for (j = 0; j < num_formats; j++)
            if (pScreenInfo->formats[j].depth == depth) break;
        if ((j == num_formats) && (j < MAXFORMATS))
        {
            pScreenInfo->formats[num_formats].depth = depth;
            depth = (depth + 7) & ~(0x7);
            pScreenInfo->formats[num_formats].bitsPerPixel = depth;
            pScreenInfo->formats[num_formats].scanlinePad = BITMAP_SCANLINE_PAD;
            num_formats++;
        }
    }
    pScreenInfo->numPixmapFormats = num_formats;

/*
 * Allocate a screen private index to hold display data pointers (pDisp's)
 */

    if (apGeneration != serverGeneration)   /* actually should never be equal? */
    {
        if ((apDisplayScreenIndex = AllocateScreenPrivateIndex()) < 0)
            return FALSE;
        apGeneration = serverGeneration;
    }

/*
 * Finally, loop over the unit list and call AddScreen for each one.
 */

    for (i = 0; i < nUnits; i++)
    {
        currentUnitIndex = i;
        AddScreen (apUnitScreenInit, argc, argv);
    }
}

/*
 * InitInput -- DDX interface (server)
 *      Initialize and register the keyboard and mouse.
 */
void
InitInput(argc, argv)
    int     argc;
    char   *argv[];
{
    DevicePtr   ptr_dev, kbd_dev;

    ptr_dev = AddInputDevice (apMouseProc, TRUE);
    kbd_dev = AddInputDevice (apKeybdProc, TRUE);

    RegisterPointerDevice (ptr_dev);
    if (apMICursorScreen)
        miRegisterPointerDevice (apMICursorScreen, ptr_dev);
    RegisterKeyboardDevice (kbd_dev);
}

/*
 * ddxGiveUp -- DDX interface (server)
 *      DDX-specific normal termination routine.
 *      Nothing to do unless we are running in place of the DM.
 *      In that case, we must release the wm resources.
 */
void
ddxGiveUp()
{
    status_$t   status;

    if (!nUnits)
        return;

    if (apXSolo)
      gpr_$free_wm (&status);
}

/*
 * AbortDDX -- DDX interface (server)
 *      DDX-specific abort routine.  Called by AbortServer().
 */
void
AbortDDX()
{
    ddxGiveUp();
}

/*
 * ddxProcessArgument -- DDX interface (server)
 *      "Claim" an argument, with following tokens, as belonging to DDX.
 *      We recognize "-D<anything>" here, and take the following word as well.
 */
int
ddxProcessArgument (argc, argv, i)
    int	    argc;
    char   *argv[];
    int	    i;
{
    extern void UseMsg();

    if (strncmp(argv[i], "-D", 2) == 0)
    {
        if (++i >= argc) UseMsg ();
        return 2;
    }
    else
        return 0;
}

/*
 * ddxUseMsg -- DDX interface (server)
 *      Describe DDX-specific arguments to the user.
 */
void
ddxUseMsg()
{
    ErrorF("-D<n> mfb              use mfb driver on display at unit number <n>\n");
    ErrorF("-D<n> gpr<depth>       use GPR driver on display at unit number <n>, at depth <depth>\n");
    ErrorF("-D<n> d<depth>         synonymous with above (for backward compatibility)\n");
}
