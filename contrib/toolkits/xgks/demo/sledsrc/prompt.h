#if !defined(PROMPT_H)
#define PROMPT_H

#include "changes.h"

void prompt(IDX msgno);
void reprompt(IDX msgno);
void full_prompt(IDX new1, IDX new2);
void restore_msg_area(void);
void rm_msg_area(void);

#endif
