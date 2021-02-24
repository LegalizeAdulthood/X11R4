#if !defined(DUMMIES_H)
#define DUMMIES_H

#include <xgks.h>

#include "changes.h"

BOOLEAN in_bounds_delta(Gpoint *ext, Gpoint point, Gfloat delt);
void init_locator(WSID id, int dev, Gpoint *pos, IDX ntrans, int prompt, Gfloat xmin, Gfloat xmax, Gfloat ymin, Gfloat ymax);
void req_locator(WSID id, int dev, Gistat *status, IDX *ntrans, Gpoint *pos);

#endif
