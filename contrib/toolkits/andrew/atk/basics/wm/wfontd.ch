/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/wm/RCS/wfontd.ch,v 1.6 89/08/27 19:47:46 zs01 Exp $ */
/* $ACIS:fontdesc.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/wm/RCS/wfontd.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidwmfontdesc_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/wm/RCS/wfontd.ch,v 1.6 89/08/27 19:47:46 zs01 Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


class wmfontdesc[wfontd] : fontdesc {
overrides:
    CvtCharToGraphic(struct graphic *gr, char SpecialChar) returns struct graphic *;
    GetRealFontDesc(struct graphic *gr) returns struct font *;

    StringSize(struct graphic *gr, char * string, long * XWidth, long * YWidth) returns long;
    TextSize(struct graphic *gr, char * text, long TextLength, long *XWidth, long *YWidth)
		returns long;
    WidthTable(struct graphic *gr) returns  short*; /* actually an array of short's */
    HeightTable(struct graphic *gr) returns short *; /* actually an array of short's */
    CharSummary(struct graphic *gr, char LookUpChar, struct fontdesc_charInfo * returnedInfo);

classprocedures:
    InitializeObject(/* struct classhdr *ClassID, */  struct wmfontdesc *self ) returns boolean;
    FinalizeObject(/* struct classhdr *ClassID, */  struct wmfontdesc *self );
    Allocate() returns struct wmfontdesc *;
    Deallocate(struct wmfontdesc *);
};
