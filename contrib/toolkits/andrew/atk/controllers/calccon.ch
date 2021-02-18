/* user code begins here for HeaderInfo */
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/controllers/RCS/calccon.ch,v 2.4 89/02/17 15:18:34 ghoti Exp Locker: tpn $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/controllers/RCS/calccon.ch,v $ */
#ifndef lint
static char *rcsid_calccon_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/controllers/RCS/calccon.ch,v 2.4 89/02/17 15:18:34 ghoti Exp Locker: tpn $ ";
#endif /* lint */
/* user code ends here for HeaderInfo */
class calccon { 
classprocedures :
        InitializeClass() returns boolean;
/* user code begins here for classprocedures */
/* user code ends here for classprocedures */
data:
	struct value *calc_4;
	struct buttonV *calc_4View;
	struct value *calc_5;
	struct buttonV *calc_5View;
	struct value *valenter;
	struct enterstrV *valenterView;
	struct value *decimal;
	struct buttonV *decimalView;
	struct value *digit_0;
	struct buttonV *digit_0View;
	struct value *digit_1;
	struct buttonV *digit_1View;
	struct value *digit_2;
	struct buttonV *digit_2View;
	struct value *digit_3;
	struct buttonV *digit_3View;
	struct value *digit_4;
	struct buttonV *digit_4View;
	struct value *digit_5;
	struct buttonV *digit_5View;
	struct value *digit_6;
	struct buttonV *digit_6View;
	struct value *digit_7;
	struct buttonV *digit_7View;
	struct value *digit_8;
	struct buttonV *digit_8View;
	struct value *digit_9;
	struct buttonV *digit_9View;
	struct value *output;
	struct stringV *outputView;
	struct value *calc_1;
	struct buttonV *calc_1View;
	struct value *calc_2;
	struct buttonV *calc_2View;
	struct value *calc_3;
	struct buttonV *calc_3View;
/* user code begins here for classdata */
	char buf[64];
	double val,saveval,dec;
	boolean error,clear;
	long op;
/* user code ends here for classdata */
	struct view *v;
	struct calccon *next;
};

