#if !defined(UNDO_H)
#define UNDO_H

#include "changes.h"

void initialize_undo(void);
void check_undo_flag(void);
BOOLEAN test_undo_flag(void);
void log_last_area(AREA area, IDX transno, Gpoint pt);

#endif
