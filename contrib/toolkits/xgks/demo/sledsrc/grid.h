#if !defined(GRID_H)
#define GRID_H

#include <xgks.h>

void init_grid(void);
void draw_grid(void);
void erase_grid(void);
int get_override_state(void);
void snap_to_grid( Gpoint *pt);

#endif
