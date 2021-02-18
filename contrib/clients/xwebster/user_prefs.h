/* -*-C-*-
********************************************************************************
*
* File:         user_prefs.h
* RCS:          $Header: user_prefs.h,v 1.5 89/03/06 01:42:00 mayer Exp $
* Description:  type definition and global datastructure for user preferences
* Author:       Niels Mayer, HPLabs
* Created:      Thu Aug 11 00:06:04 1988
* Modified:     Tue Sep 13 15:34:34 1988 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       G-Job
*
* xwebster - dictionary browser
*
* Copyright 1988 Hewlett-Packard Company
*
* Permission to use, copy, modify, and distribute this software and its
* documentation for any purpose and without fee is hereby granted, provided
* that the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of HP not be used in advertising or
* publicity pertaining to distribution of the software without specific,
* written prior permission.  HP makes no representations about the
* suitability of this software for any purpose.  It is provided "as is"
* without express or implied warranty.
*
* Please send any improvements, bug fixes, useful modifications, and comments
* to mayer@hplabs.hp.com.
********************************************************************************
*/
static char rcs_identity_user_prefs[] = "@(#)$Header: user_prefs.h,v 1.5 89/03/06 01:42:00 mayer Exp $";

typedef struct {
  String help_text;
  String host_addr_list;	/* a list of whitespace separated host addrs */
  int    host_port;
  String host_service_name;
  int    word_input_Wgt_width;
  int    use_editor_as_display;	/* if TRUE use TextEdit, else StaticText */
  int    display_width;
  int    display_height;
  int    num_words_before_refresh;
} USER_PREFS_DATA, *USER_PREFS_DATA_PTR;

extern USER_PREFS_DATA user_prefs;
