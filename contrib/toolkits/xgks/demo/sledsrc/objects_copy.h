#if !defined(OBJECTS_COPY_H)
#define OBJECTS_COPY_H

#include "objects.h"

COMB_OB *copy_comb_ob(COMB_OB *comb_ob);
OBJECT *copy_object(OBJECT *object);
void copy_line(OBJECT *object1, OBJECT *duplicate);
void copy_poly(OBJECT *object1, OBJECT *duplicate);
void copy_text(OBJECT *object1, OBJECT *duplicate);

#endif
