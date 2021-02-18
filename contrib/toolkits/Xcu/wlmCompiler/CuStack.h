#ifndef _Cu_Stack_h
#define _Cu_Stack_h

#include "CuGrow.h"

#define STACK_NEXT_POP  -1
#define STACK_NEXT_PULL -2

typedef
struct s_stack
    {
    char *name ;
    int	n ;
    Grow *grow ;
    } Stack ;

void Cu_stack_create () ;
void Cu_stack_push () ;
int Cu_stack_pull () ;
int Cu_stack_pop () ;
int Cu_stack_peek () ;
void Cu_stack_kill () ;
int Cu_stack_size () ;

#endif _Cu_Stack_h
