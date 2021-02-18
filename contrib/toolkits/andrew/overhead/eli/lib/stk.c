/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/*
 * $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/eli/lib/RCS/stk.c,v 2.3 89/02/10 23:12:53 ghoti Exp $ 
 *
 * $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/eli/lib/RCS/stk.c,v $ 
 */

#include  <stk.h>

/* Initialize a stack.  S must point to an existing structure */

void            eliEvalStk_Init(s)
eliEvalStack_t *s;
{
    s->top = 0;
    s->size = 0;
    s->stack = NULL;
}

/* Push the given stknode onto the stack */

int eliEvalStk_Push(st, s, node)
EliState_t *st;
eliEvalStack_t *s;
EliSym_t *node;
{
    if (s->top == s->size) {
        if (!eliEvalStk_Grow(s))
            return (FALSE);
        else
            st->numTotalStkNodes += GROW_STACK_SIZE;
    }
    s->stack[(s->top)++] = node;
    eliSym_IncrRefcount(node);
    ++(st->numStkNodes);
    return (TRUE);
}

/* return the datum of the top node on the stack */
/* ASSUMES There is an element on the stack */

EliSym_t      *eliEvalStk_Top(s)
eliEvalStack_t *s;
{
    return (s->stack[s->top - 1]);
}

/* Pop top item from stack -- DOESN'T RETURN A VALUE */
/* Nothing popped if stack is empty (maybe should return an error) */

void            eliEvalStk_Pop(st, s)
EliState_t     *st;
eliEvalStack_t *s;
{
    eliSym_DecrRefcount(st, s->stack[s->top - 1]);
    --(s->top);
    --(st->numStkNodes);
}

/*
 * SPECIAL FUNCTION: Assumes the elements in the stack are symnodes.
 * Traverses the stack, looking for a symnode whose name is given by "name". 
 */

EliSym_t       *eliEvalStk_FindSym(s, name)
eliEvalStack_t *s;
char           *name;
{
    int i, notfound = TRUE;
    EliSym_t *result = NULL;

    for (i = s->top - 1; (i >= 0) && notfound; --i) {
        if (!(notfound = strcmp(EliStr_GetString(EliSym_GetName(s->stack[i])), name)))
            result = s->stack[i];
    }
    return (result);
}

static int eliEvalStk_Grow(s)
eliEvalStack_t *s;
{
    EliSym_t **tmp = (s->stack) ? (EliSym_t **) realloc(s->stack, (s->size + GROW_STACK_SIZE) * sizeof (EliSym_t *)) : (EliSym_t **) malloc(GROW_STACK_SIZE * sizeof (EliSym_t *));

    if (tmp) {
        s->stack = tmp;
        s->size += GROW_STACK_SIZE;
        return (TRUE);
    }
    return (FALSE);
}
