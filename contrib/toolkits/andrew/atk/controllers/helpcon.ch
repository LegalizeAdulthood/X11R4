/* user code begins here for HeaderInfo */
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/controllers/RCS/helpcon.ch,v 2.4 89/02/17 15:18:45 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/controllers/RCS/helpcon.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_helpcon_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/controllers/RCS/helpcon.ch,v 2.4 89/02/17 15:18:45 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */
/* user code ends here for HeaderInfo */
class helpcon { 
classprocedures :
        InitializeClass() returns boolean;
/* user code begins here for classprocedures */
/* user code ends here for classprocedures */
data:
	struct text *topics;
	struct cltextview *topicsView;
	struct text *choice;
	struct cltextview *choiceView;
	struct value *topicschoice;
	struct stringV *topicschoiceView;
	struct text *body;
	struct textview *bodyView;
	struct value *choicelabel;
	struct stringV *choicelabelView;
/* user code begins here for classdata */
	struct text *historytext;
	int ShowHistory;
	char CurrentName[256];
	char CurrentType[256];
	struct direct **dl;
	long count;
/* user code ends here for classdata */
	struct view *v;
	struct arbiterview *arbv;
	struct helpcon *next;
};

