#if !defined(OBJECTS_DRAW_H)
#define OBJECTS_DRAW_H

#include "objects.h"

void draw_comb_ob( COMB_OB *comb_ob);
void erase_comb_ob( COMB_OB *comb_ob);
void draw_object(OBJECT *object);
void erase_object(OBJECT *object);
void draw_line( OBJECT *object);
void erase_line( OBJECT *object);
void draw_poly( OBJECT *object);
void erase_poly( OBJECT *object);
void draw_text( OBJECT *object);
void erase_text( OBJECT *object);
void display_text( OBJECT *object, int draw_erase);

#endif
