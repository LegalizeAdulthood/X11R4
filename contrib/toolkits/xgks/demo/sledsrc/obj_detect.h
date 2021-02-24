#if !defined(OBJ_DETECT_H)
#define OBJ_DETECT_H

#include <xgks.h>

#include "changes.h"
#include "objects.h"

void get_extent( Gpoint *pts, int npts, Gpoint *extent);
void get_text_extent( Gchar ch, Gpoint center, IDX font, Gfloat height, Gfloat expansion, Gpoint up_vec, Gpoint *extent);
void detect_object( BOOLEAN *hit, COMB_OB **comb_ob, OBJECT **object, Gpoint pt);
BOOLEAN is_object_inbounds( OBJECT *object, Gpoint *extent);

#endif
