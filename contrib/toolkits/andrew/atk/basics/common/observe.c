/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/observe.c,v 2.8 89/06/23 16:46:59 wjh Exp $ */
/* $ACIS:observe.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/observe.c,v $ */

#ifndef lint
char *observe_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/observe.c,v 2.8 89/06/23 16:46:59 wjh Exp $";
#endif /* lint */

#include <class.h>
#include <observe.eh>
#include <atom.ih>
#include <atomlist.ih>

#define INITIALNUMOBSERVERS 4


/* a triggerinstance is a particular function to be called when a trigger fires */
struct triggerinstance {
	struct triggerinstance *next;
	void(*func)();
	struct basicobject *rcvr;
	long rock;
};

/* a triggerhousing is the intersection of a trigger atom with a particular object
	it contains the list of functions to be called when the trigger is pulled */
struct triggerhousing {
	struct triggerhousing *next;
	struct atom *trigger;
	struct triggerinstance *instances;
	long disablecount;	/* enable if zero. disabled if > 0 */
	boolean firepending;	/* set True if fired while disabled */
};

/* a triggerclass is the list of all triggers defined for a class */
struct triggerclass {
	struct triggerclass *next;
	struct classinfo *class;   /* class_GetType(instance) */
	struct atomlist *triggers;
};


static struct triggerclass *Triggers = NULL;	/* the list of defined triggers */




boolean observable__InitializeObject(classID, self)
struct classheader *classID;
struct observable *self;
{
    self->nObservers = 0;
    self->maxObservers = 0;
    self->observers = NULL;
    self->triggers = NULL;
    return TRUE;
}

void observable__FinalizeObject(classID, self)
struct classheader *classID;
struct observable *self;
{
    struct triggerinstance *ti, *tit;
    struct triggerhousing *th, *tht;
    observable_NotifyObservers(self, observable_OBJECTDESTROYED);
    if (self->observers)
	free(self->observers);

    /* free all triggerinstances and triggerhousings hanging from self->triggers */
    for (th = self->triggers; th != NULL; th = tht) {
	for (ti = th->instances; ti != NULL; ti = tit) {
		tit = ti->next;
		free(ti);
	}
	tht = th->next;
	free(th);
    }
}

/* Finds the index of the observer in self observers table.  Returns -1 if observer is not in the list
 */
static int FindObserver(self, observer)
register struct observable *self;
register struct observable *observer;  {
    register int i = 0;
    struct observable **observers;

    for (i = 0, observers = self->observers; i < self->nObservers; i++, observers++)
	if (*observers == observer) return i;
    
    return -1;
}

void observable__AddObserver(self, observer)
register struct observable *self;
register struct observable *observer;  {
    if (self->maxObservers == 0)  {
	self->maxObservers = INITIALNUMOBSERVERS;
	self->observers = (struct observable **) malloc (INITIALNUMOBSERVERS * sizeof(struct observable *));
    }
    else if (FindObserver(self, observer) != -1) return;
    else if (self->nObservers == self->maxObservers)  {
	self->maxObservers += self->maxObservers / 2;
	self->observers = (struct observable **) realloc(self->observers, self->maxObservers * sizeof(struct observable *));
    }
    self->observers[self->nObservers++] = observer;
}

void observable__RemoveObserver(self, observer)
register struct observable *self;
register struct observable *observer;  {
    register int i;

    if ((i = FindObserver(self, observer)) != -1)  {
	while (i++ < self->nObservers)
	    self->observers[i - 1] = self->observers[i];
	self->nObservers -= 1;
    }
}

void observable__NotifyObservers(self, value)
register struct observable *self;
long value;  {
    register struct observable **observers;
    register int i;

    
    for (i = 0, observers = self->observers;  i < self->nObservers; i++, observers++)
	observable_ObservedChanged(*observers, self, value);
}

void observable__ObservedChanged(self, changed, value)
struct observable *self;
struct observable *changed;
long value;  {
}



	/* the following methods implement a scheme for "triggers", 
		a set of named messages,  each trigger must first be
		defined with a call to DefineTrigger*/



/* observable__DefineTrigger(classID, classinstance, trigger)
	associate the atom as a possible trigger for the class 
*/
	void
observable__DefineTrigger(classID, classinstance, trigger)
	struct classheader *classID;
	struct basicobject *classinstance;
	struct atom *trigger;
{
	struct triggerclass *tc;
	struct classinfo *info;
	info = class_GetType(classinstance);
	for (tc = Triggers; 
			tc != NULL && tc->class != info;
			tc = tc->next)
		{};
	if (tc == NULL) {
		tc = (struct triggerclass *)malloc(sizeof (struct triggerclass));
		tc->class = info;
		tc->triggers = atomlist_New();
		tc->next = Triggers;
		Triggers = tc;
	}
	if ( ! atomlist_Memberp(tc->triggers, trigger))
		atomlist_Prepend(tc->triggers, trigger);
}

/* observable__ListTriggers(classID, classinstance)
	returns a list of the triggers defined for the class 
	the caller must destroy the list 
*/
	struct atomlist *
observable__ListTriggers(classID, classinstance)
	struct classheader *classID;
	struct basicobject *classinstance;
{
	struct triggerclass *tc;
	struct classinfo *info;
	struct atomlist *result;
	result = atomlist_New();
	info = class_GetType(classinstance);

	for (tc = Triggers; tc != NULL; tc = tc->next)
		if (class_IsType(info, tc->class))
			atomlist_JoinToEnd(result, tc->triggers);
	return result;
}


/* observable__AddRecipient(self, trigger, rcvr, func, rock)
	when the trigger is Pull'ed, the func will be called thus:
		func(rcvr, self, rock)
*/
	boolean
observable__AddRecipient(self, trigger, rcvr, func, rock)
	struct observable *self;
	struct atom *trigger;
	void (*func)();
	struct basicobject *rcvr;
	long rock;
{
	struct triggerclass *tc;
	struct triggerhousing *th;
	struct triggerinstance *ti;
	for (th = self->triggers; th != NULL && th->trigger != trigger; th = th->next) 
		{};
	if (th == NULL) {
		/* find out if the trigger is defined */
		for (tc = Triggers;  tc != NULL && ! class_IsType(self, tc->class);
				tc = tc->next)
			{};
		if (tc == NULL || ! atomlist_Memberp(tc->triggers, trigger))
			/* not defined */
			return FALSE;
		/* make a new trigger housing: this is self's first recipient for this trigger */
		th = (struct triggerhousing *)malloc(sizeof(struct triggerhousing));
		th->trigger = trigger;
		th->instances = NULL;
		th->disablecount = 0;
		th->firepending = FALSE;
		th->next = self->triggers;
		self->triggers = th;
	}
	/* th now points to an appropriate triggerhousing */

	/* find out if this is a redefinition */
	for (ti = th->instances; ti != NULL; ti = ti->next)
		if (ti->rcvr == rcvr) {
			ti->func = func;
			ti->rock = rock;
			return TRUE;
		}
	/* add a new trigger instacne */
	ti = (struct triggerinstance *)malloc(sizeof(struct triggerinstance));
	ti->func = func;
	ti->rcvr = rcvr;
	ti->rock = rock;
	ti->next = th->instances;
	th->instances = ti;
	return TRUE;
}

/* observable__DeleteRecipient(self, trigger, rcvr)
	removes the receiver from the list of recipients
*/
	void
observable__DeleteRecipient(self, trigger, rcvr)
	struct observable *self;
	struct atom *trigger;
	struct basicobject *rcvr;
{
	struct triggerhousing *th;
	struct triggerinstance *ti, *pi;
	for (th = self->triggers; th != NULL && th->trigger != trigger; th = th->next) 
		{};
	if (th == NULL)  return;
	
	for (pi = NULL, ti = th->instances; ti != NULL && ti->rcvr != rcvr; pi = ti, ti = ti->next)
		{};
	if (ti == NULL) return;

	if (pi == NULL)
		th->instances = ti->next;
	else
		pi->next = ti->next;
	free(ti);
}


/* observable__PullTrigger(self, trigger)
	call all funcs associated with this trigger on this object
*/
	void
observable__PullTrigger(self, trigger)
	struct observable *self;
	struct atom *trigger;
{
	struct triggerhousing *th;
	struct triggerinstance *ti;
	for (th = self->triggers; th != NULL && th->trigger != trigger; th = th->next) 
		{};
	if (th == NULL)
		return;
	if (th->disablecount > 0) {
		th->firepending = TRUE;
		return;
	}
	for (ti = th->instances; ti != NULL; ti = ti->next)
		(ti->func)(ti->rcvr, self, ti->rock);
}

	/* if a client is calling a number of operations which would pull a trigger 
		too many times, it can disable the trigger temporarily.  
		It must later Enable the trigger.  At that time one call back 
		is made for the trigger if it has been Pulled one or more times in the interim.  */

/* observable__DisableTrigger(self, trigger)
	until Enabled, this trigger will no produce call backs 
			Enable MUST be called once 
			for each time Disable has been called.
*/
	void
observable__DisableTrigger(self, trigger)
	struct observable *self;
	struct atom *trigger;
{
	struct triggerhousing *th;
	for (th = self->triggers; th != NULL && th->trigger != trigger; th = th->next) 
		{};
	if (th == NULL)
		return;
	if (th->disablecount == 0)
		th->firepending = FALSE;
	th->disablecount ++;
}

/* observable__EnableTrigger(self, trigger)
	this trigger will once again produce call backs
*/
	void
observable__EnableTrigger(self, trigger)
	struct observable *self;
	struct atom *trigger;
{
	struct triggerhousing *th;
	for (th = self->triggers; th != NULL && th->trigger != trigger; th = th->next) 
		{};
	if (th == NULL)
		return;
	if (th->disablecount > 0)
		th->disablecount --;
	else th->disablecount = 0;
	if (th->disablecount == 0  &&  th->firepending)
		observable_PullTrigger(self, trigger);
}

/* observable__DisableCount(self, trigger)
	return the number of outstanding DisableTrigger calls
*/
	long
observable__DisableCount(self, trigger)
	struct observable *self;
	struct atom *trigger;
{
	struct triggerhousing *th;
	struct triggerclass *tc;

	for (th = self->triggers; th != NULL && th->trigger != trigger; th = th->next) 
		{};
	if (th != NULL)
		return th->disablecount;

	/* find out if the trigger is defined */
	for (tc = Triggers;  tc != NULL && tc->class != class_GetType(self);
			tc = tc->next)
		{};
	if (tc == NULL || ! atomlist_Memberp(tc->triggers, trigger))
		/* not defined */
		return -1;
	/* is defined, but has no recipients.  Is not disabled */
	return 0;
}
