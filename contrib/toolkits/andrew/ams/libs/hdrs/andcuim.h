/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/hdrs/RCS/andcuim.h,v 2.3 89/02/08 15:30:12 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/hdrs/RCS/andcuim.h,v $
*/
/*   Machine dependent include file for CUI.C  -- Andrew Version    */

/* For instructional purposes only */
#define EOF_STRING "press control-d"

/* Number of snapshots to request from ms in one gulp */
#define CUIMACH_GULPSIZE 100

#define CUI_KEEPALIVE_WITHOUT_SNAP

/* For systems where popen is available */

#define ENABLEFILTERING 1
