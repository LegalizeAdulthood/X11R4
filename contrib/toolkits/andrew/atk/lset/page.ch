/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/page.ch,v 1.2 89/08/05 15:29:25 tpn Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/page.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_page_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/lset/RCS/page.ch,v 1.2 89/08/05 15:29:25 tpn Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* The following may be use in place of a specific position  
	in the methods taking position arguments below  */
#define page_CURRENT		 -1
#define page_AFTERCURRENT	 -2
#define page_BEFORECURRENT	 -3
#define page_ATEND 		 -4
#define page_ATBEGINING	 -5

struct page_switchee {
    struct dataobject *d;
    char *viewname;
    char *label;
    struct page_switchee *next; 
};

class page : dataobject [dataobj] {
    classprocedures:
      InitializeObject(struct page *self) returns boolean;
      FinalizeObject(struct page *self);
    overrides:
      Read (FILE *file, long id) returns long;
      Write (FILE *file, long writeid, int level) returns long;
      ViewName() returns char *;
      GetModified() returns long;
    macromethods:
      GetPostMenus() ((self)->PostMenusFlag)
      SetPostMenus(foo) (((self)->PostMenusFlag) = foo)
      GetFirstObject() ((((self)->FirstSwitchee) == NULL) ? NULL: (self)->FirstSwitchee->dataobject)
      GetNowPlaying() ((((self)->NowPlaying) == NULL) ? NULL: (self)->NowPlaying->dataobject)

       /* The following macro methods are private */
      GetFirstSwitchee() ((self)->FirstSwitchee)
      GetNowPlayingSwitchee() ((self)->NowPlaying)
       
    methods:
      AddObject(struct dataobject *d, char *label,
		 char *viewname,long position) returns boolean;
      DeleteObject(struct dataobject *d) returns boolean;
      SetNowPlaying(struct dataobject *d) returns boolean;
      SetNowPlayingByName(char *name) returns boolean;
      GetNowPlayingName() returns char *;
      SetNowPlayingByPosition(long position) returns boolean;
      GetPositionOfObject(struct dataobject *d) returns long;
      GetNameOfObject(struct dataobject *d) returns char *;	
      GetObjectAtPosition(long position) returns struct dataobject *;
      GetObjectByName(char *name) returns struct dataobject *;
      GetObjectCount() returns long;
/* The following methods are private */
      GetSwitcheeName(struct page_switchee *sw) returns char *;
    data:
      struct page_switchee *FirstSwitchee, *NowPlaying;
      boolean PostMenusFlag,executingGetModified;
};
