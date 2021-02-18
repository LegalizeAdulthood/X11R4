/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/msclients/vui/RCS/vuimsgs.c,v 1.4 89/07/20 16:14:18 cfe Exp $ */

/* vui error messages */

char *VUIMSG_NOMEM = "Out of Memory...";
char *VUIMSG_MAPFILE = "Could not open/read subscription map file";
char *VUIMSG_READFILE = "Message server could not open/read file";
char *VUIMSG_NOHELP = "There is no help available at this time";
char *VUIMSG_MBCHECK = "Problem checking your mailbox";
char *VUIMSG_MFOLDER = "Sorry; that folder is for your incoming mail";
char *VUIMSG_FCHECK = "Could not check folder";
char *VUIMSG_SUBENTRY = "Could not get/set the subscription entry";
char *VUIMSG_MSGREAD = "The body of the message could not be completely read";
char *VUIMSG_MSSTORE = "Message server could not store file";
char *VUIMSG_AOHEAD = "Ambigious add/omit headers";
char *VUIMSG_NMFOLDER = "No more bboards/folders";
char *VUIMSG_ALLOCPAGE = "Can not allocate another page";
char *VUIMSG_NOMOREMSG = "No more messages";
char *VUIMSG_SNAPSHOT = "Could not get snapshot";
char *VUIMSG_FILLPAGE = "Fill in this page first";
char *VUIMSG_BENTLIST = "You are at the beginning of the list/message";
char *VUIMSG_BMSGFILE = "You are the beginning of this message/file";

/* vui common strings */

#ifndef IBMPC
 char *VUI_HOST	    = "Andrew";
 char *F2_STRING    = "<Esc>2";
 char *PGDN_STRING  = "^v";
 char *PGUP_STRING  = "<Esc>v";
#endif /* IBMPC */

#ifdef IBMPC
 char *VUI_HOST	    = "PC";
 char *F2_STRING    = "F2";
 char *PGDN_STRING  = "PgDn";
 char *PGUP_STRING  = "PgUp";
#endif /* IBMPC */

char *ACK_STRING	    = "Ack-To: ";
char *VOTE_CHOICE_STRING    = "Vote-Choices: ";
char *VOTE_TO_STRING	    = "Vote-To: ";
char *VOTE_ID_STRING	    = "Vote-Request: ";
char *INVITE_STRING	    = "X-Andrew-DirectoryCreation: ";
char *REDIST_STRING	    = "X-Andrew-Redistribution-To: ";
