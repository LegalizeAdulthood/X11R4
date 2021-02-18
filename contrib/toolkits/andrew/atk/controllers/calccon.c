/* user code begins here for HeaderInfo */
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/controllers/RCS/calccon.c,v 2.4 89/02/17 15:18:26 ghoti Exp Locker: tpn $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/controllers/RCS/calccon.c,v $ */
#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/controllers/RCS/calccon.c,v 2.4 89/02/17 15:18:26 ghoti Exp Locker: tpn $ ";
#endif /* lint */
/* user code ends here for HeaderInfo */
#include "class.h"
#include "proctbl.ih"
#include "view.ih"
#include "arbiterv.ih"
#include "calccon.eh"
#include "celv.ih"
#include "buttonv.ih"
#include "controlv.ih"
#include "cel.ih"
#include "entrstrv.ih"
#include "lsetv.ih"
#include "lset.ih"
#include "value.ih"
#include "stringv.ih"
/* user code begins here for includes */
#include <math.h>
#define ADD 1
#define SUB 2
#define MULT 3
#define DIV 4
displayval(self)
struct calccon *self;
{
if(self->error){
    value_SetString(self->output,"Error");
    return;
}
sprintf(self->buf,"%12.6g",self->val);
value_SetString(self->output,self->buf);
}
clear(self)
struct calccon *self;
{
self->val = 0.0;
self->saveval = 0.0;
*(self->buf) = '\0';
self->error = FALSE;self->op = 0;
self->dec = 0.0;
self->clear = TRUE;
displayval(self);
}
/* user code ends here for includes */

static struct calccon *firstcalccon;
static struct calccon *FindSelf(v)
struct view *v;
{
	struct calccon *self,*last = NULL;
	for(self= firstcalccon; self != NULL; self = self->next){
		if(self->v == v) return self;
		last = self;
		}
	self = calccon_New();
	initself(self,v);
	if(last == NULL) firstcalccon = self;
	else last->next = self;
	return self;
}
static void calcCallBack(self,val,r1,r2)
struct calccon *self;
struct value *val;
long r1,r2;
{
/* user code begins here for calcCallBack */
self->error = FALSE;
switch (self->op){
	case ADD:
	    self->val = self->val + self->saveval;
	    break;
	case SUB:
	    self->val =  self->saveval - self->val;
	    break;
	case MULT:
	    self->val = self->val * self->saveval;
	    break;
	case DIV:
	    if(self->val == 0.0){
		self->error = TRUE;
	    }
	    else self->val =  self->saveval / self->val;
	    break;
	default:
	    break;
}
self->saveval = self->val;
self->op = r1;
self->dec = 0.0;
self->clear = TRUE;
displayval(self);
/* user code ends here for calcCallBack */
}
static void valenterCallBack(self,val,r1,r2)
struct calccon *self;
struct value *val;
long r1,r2;
{
/* user code begins here for valenterCallBack */
{
    double f ;
    char *c;
    c  = value_GetString(val);
    switch (*c){
	case 0:
	    return;
	case 'c':
	case 'C':
	    clear(self);
	    break;
	case '=':
	    calcCallBack(self,NULL,0,0);
	    break;
	case '+':
	    calcCallBack(self,NULL,ADD,0);
	    break;
	case '-':
	    calcCallBack(self,NULL,SUB,0);
	    break;
	case '*':
	case 'X':
	    calcCallBack(self,NULL,MULT,0);
	    break;
	case '/':
	    calcCallBack(self,NULL,DIV,0);
	    break;
	default:
	    f = atof(c);
	    if(f != self->val){
		self->val = f;
		displayval(self);
	    }
    }
    value_SetString(val,"");
}
/* user code ends here for valenterCallBack */
}
static void decimalCallBack(self,val,r1,r2)
struct calccon *self;
struct value *val;
long r1,r2;
{
/* user code begins here for decimalCallBack */
    if(!self->dec) self->dec = 10.0;
/* user code ends here for decimalCallBack */
}
static void digitCallBack(self,val,r1,r2)
struct calccon *self;
struct value *val;
long r1,r2;
{
/* user code begins here for digitCallBack */
    if(self->clear){
	self->val = 0.0;
	self->clear = FALSE;
    }
    if(self->dec){
	self->val = self->val + (r1 / self->dec);
	self->dec *= 10.0;
    }
    else{
	self->val = self->val * 10.0 + r1;
    }
    displayval(self);
/* user code ends here for digitCallBack */
}
static void outputCallBack(self,val,r1,r2)
struct calccon *self;
struct value *val;
long r1,r2;
{
/* user code begins here for outputCallBack */
/* user code ends here for outputCallBack */
}
static initself(self,v)
struct calccon *self;
struct view *v;
{
	self->v = v;
	self->calc_4View = (struct buttonV *)arbiterview_GetNamedView(v,"calc-4");
	self->calc_4 = (struct value *)arbiterview_GetNamedObject(v,"calc-4");
	if(self->calc_4) value_AddCallBackObserver(self->calc_4, self,calcCallBack,4);
	self->calc_5View = (struct buttonV *)arbiterview_GetNamedView(v,"calc-5");
	self->calc_5 = (struct value *)arbiterview_GetNamedObject(v,"calc-5");
	if(self->calc_5) value_AddCallBackObserver(self->calc_5, self,calcCallBack,5);
	self->valenterView = (struct enterstrV *)arbiterview_GetNamedView(v,"valenter");
	self->valenter = (struct value *)arbiterview_GetNamedObject(v,"valenter");
	if(self->valenter) value_AddCallBackObserver(self->valenter, self,valenterCallBack,0);
	self->decimalView = (struct buttonV *)arbiterview_GetNamedView(v,"decimal");
	self->decimal = (struct value *)arbiterview_GetNamedObject(v,"decimal");
	if(self->decimal) value_AddCallBackObserver(self->decimal, self,decimalCallBack,0);
	self->digit_0View = (struct buttonV *)arbiterview_GetNamedView(v,"digit-0");
	self->digit_0 = (struct value *)arbiterview_GetNamedObject(v,"digit-0");
	if(self->digit_0) value_AddCallBackObserver(self->digit_0, self,digitCallBack,0);
	self->digit_1View = (struct buttonV *)arbiterview_GetNamedView(v,"digit-1");
	self->digit_1 = (struct value *)arbiterview_GetNamedObject(v,"digit-1");
	if(self->digit_1) value_AddCallBackObserver(self->digit_1, self,digitCallBack,1);
	self->digit_2View = (struct buttonV *)arbiterview_GetNamedView(v,"digit-2");
	self->digit_2 = (struct value *)arbiterview_GetNamedObject(v,"digit-2");
	if(self->digit_2) value_AddCallBackObserver(self->digit_2, self,digitCallBack,2);
	self->digit_3View = (struct buttonV *)arbiterview_GetNamedView(v,"digit-3");
	self->digit_3 = (struct value *)arbiterview_GetNamedObject(v,"digit-3");
	if(self->digit_3) value_AddCallBackObserver(self->digit_3, self,digitCallBack,3);
	self->digit_4View = (struct buttonV *)arbiterview_GetNamedView(v,"digit-4");
	self->digit_4 = (struct value *)arbiterview_GetNamedObject(v,"digit-4");
	if(self->digit_4) value_AddCallBackObserver(self->digit_4, self,digitCallBack,4);
	self->digit_5View = (struct buttonV *)arbiterview_GetNamedView(v,"digit-5");
	self->digit_5 = (struct value *)arbiterview_GetNamedObject(v,"digit-5");
	if(self->digit_5) value_AddCallBackObserver(self->digit_5, self,digitCallBack,5);
	self->digit_6View = (struct buttonV *)arbiterview_GetNamedView(v,"digit-6");
	self->digit_6 = (struct value *)arbiterview_GetNamedObject(v,"digit-6");
	if(self->digit_6) value_AddCallBackObserver(self->digit_6, self,digitCallBack,6);
	self->digit_7View = (struct buttonV *)arbiterview_GetNamedView(v,"digit-7");
	self->digit_7 = (struct value *)arbiterview_GetNamedObject(v,"digit-7");
	if(self->digit_7) value_AddCallBackObserver(self->digit_7, self,digitCallBack,7);
	self->digit_8View = (struct buttonV *)arbiterview_GetNamedView(v,"digit-8");
	self->digit_8 = (struct value *)arbiterview_GetNamedObject(v,"digit-8");
	if(self->digit_8) value_AddCallBackObserver(self->digit_8, self,digitCallBack,8);
	self->digit_9View = (struct buttonV *)arbiterview_GetNamedView(v,"digit-9");
	self->digit_9 = (struct value *)arbiterview_GetNamedObject(v,"digit-9");
	if(self->digit_9) value_AddCallBackObserver(self->digit_9, self,digitCallBack,9);
	self->outputView = (struct stringV *)arbiterview_GetNamedView(v,"output");
	self->output = (struct value *)arbiterview_GetNamedObject(v,"output");
	if(self->output) value_AddCallBackObserver(self->output, self,outputCallBack,0);
	self->calc_1View = (struct buttonV *)arbiterview_GetNamedView(v,"calc-1");
	self->calc_1 = (struct value *)arbiterview_GetNamedObject(v,"calc-1");
	if(self->calc_1) value_AddCallBackObserver(self->calc_1, self,calcCallBack,1);
	self->calc_2View = (struct buttonV *)arbiterview_GetNamedView(v,"calc-2");
	self->calc_2 = (struct value *)arbiterview_GetNamedObject(v,"calc-2");
	if(self->calc_2) value_AddCallBackObserver(self->calc_2, self,calcCallBack,2);
	self->calc_3View = (struct buttonV *)arbiterview_GetNamedView(v,"calc-3");
	self->calc_3 = (struct value *)arbiterview_GetNamedObject(v,"calc-3");
	if(self->calc_3) value_AddCallBackObserver(self->calc_3, self,calcCallBack,3);
}
calccon_clear(v,dat)
struct view *v;
 long dat;
{
struct calccon *self;
if((self = FindSelf(v)) == NULL) return;
/* user code begins here for calccon_clear */
arbiterview_SetIgnoreUpdates(v, TRUE);
clear(self);
/* user code ends here for calccon_clear */
}
boolean calccon__InitializeClass(ClassID)
struct classheader *ClassID;
{
struct classinfo *viewtype = class_Load("view");
firstcalccon = NULL;
proctable_DefineProc("calccon-clear",calccon_clear, viewtype,NULL,"calccon clear");
/* user code begins here for InitializeClass */
/* user code ends here for InitializeClass */
return TRUE;
}
boolean calccon__InitializeObject(ClassID,self)
struct classheader *ClassID;
struct calccon *self;
{
self->calc_4 = NULL;
self->calc_4View = NULL;
self->calc_5 = NULL;
self->calc_5View = NULL;
self->valenter = NULL;
self->valenterView = NULL;
self->decimal = NULL;
self->decimalView = NULL;
self->digit_0 = NULL;
self->digit_0View = NULL;
self->digit_1 = NULL;
self->digit_1View = NULL;
self->digit_2 = NULL;
self->digit_2View = NULL;
self->digit_3 = NULL;
self->digit_3View = NULL;
self->digit_4 = NULL;
self->digit_4View = NULL;
self->digit_5 = NULL;
self->digit_5View = NULL;
self->digit_6 = NULL;
self->digit_6View = NULL;
self->digit_7 = NULL;
self->digit_7View = NULL;
self->digit_8 = NULL;
self->digit_8View = NULL;
self->digit_9 = NULL;
self->digit_9View = NULL;
self->output = NULL;
self->outputView = NULL;
self->calc_1 = NULL;
self->calc_1View = NULL;
self->calc_2 = NULL;
self->calc_2View = NULL;
self->calc_3 = NULL;
self->calc_3View = NULL;
self->v = NULL;
self->next = NULL;
/* user code begins here for InitializeObject */
/* user code ends here for InitializeObject */
return TRUE;}
/* user code begins here for Other Functions */
/* user code ends here for Other Functions */
