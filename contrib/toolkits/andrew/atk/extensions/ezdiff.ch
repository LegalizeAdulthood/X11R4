/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/ezdiff.ch,v 2.7 89/10/09 14:46:27 tpn Exp $ */
/* $ACIS:ezdiff.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/ezdiff.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidezdiff_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/extensions/RCS/ezdiff.ch,v 2.7 89/10/09 14:46:27 tpn Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class ezdiff : observable[observe] {
    overrides:
      ObservedChanged (struct thisobject *changed, long value);
    classprocedures:
      InitializeClass()returns boolean;
      InitializeObject(struct ezdiff *self) returns boolean;    
      FinalizeObject();
      Create(struct buffer *buf1, struct buffer *buf2, boolean ignoreblanks) returns struct ezdiff *;
    data:
      struct mark **m1;
      struct mark **m2;
      struct buffer *buf[2];
      char *bname[2];
      int numbufs;
      int nummarks;
      int cmark;
      struct ezdiff *next;
};
