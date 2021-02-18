#include "CuGrow.h"

void
Cu_grow_create (grow, chunk_size)
    Grow **grow ;
    int chunk_size ;
{
(*grow) = (Grow *) malloc (sizeof (Grow)) ;
(*grow)->chunk = chunk_size ;
(*grow)->s = (int *) malloc (chunk_size * sizeof (int)) ;
(*grow)->size = chunk_size ;
}

void
Cu_grow_check (grow, n)
    Grow *grow ;
    int n ;
{
int old_size ;
int *temp ;

if (n < grow->size)
    return ;

old_size = grow->size ;

while (n >= grow->size)
    grow->size += grow->chunk ;

temp = (int *) malloc (grow->size * sizeof (int)) ;
memcpy (temp, grow->s, old_size * sizeof (int)) ; 
grow->s = temp ;

return ;
}
