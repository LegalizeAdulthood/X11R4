#if !defined(TRANS_SUBS_H)
#define TRANS_SUBS_H

#include <xgks.h>

void ident( Gfloat *trans);
void s_trans_mat(Gfloat *trans, Gfloat x,Gfloat y);
void mat_mult(Gfloat *mat1, Gfloat *mat2, Gfloat *result);
void pt_mult(Gpoint *pts, int nopts, Gfloat *trans);
void s_rot_mat(Gfloat *trans, Gpoint pt, Gfloat radians);

#endif
