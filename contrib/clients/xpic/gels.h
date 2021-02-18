/* $Header: gels.h,v 1.3 89/07/02 15:58:15 moraes Exp $ */
BOOL contains();
BOOL intersects();
void GelDraw(), GelHilite(), GelUnHilite(), PicRedraw();
void FreePtList(), WriteList();
PointList *NewPtList(), *ReadPtList();
void FreeCell();
Cell *NewCell(), *ReadCell();
void FreeGel(), WriteGel();
Gel *NewGel(), *ReadGel(), *PopGel();
int PushGel(), CountGel();
int WriteCell();
Gel *FindIntersectingGels(), *FindContainedGels();
Box *GetBBox();

/* Returns the smallest bounding box enclosing b1 and b2 in b1 */
#define bigger_box(b1, b2)	b1.ll.x = MIN(b1.ll.x, b2.ll.x), \
							b1.ll.y = MIN(b1.ll.y, b2.ll.y), \
							b1.ur.x = MAX(b1.ur.x, b2.ur.x), \
							b1.ur.y = MAX(b1.ur.y, b2.ur.y)
#define update_box(b1,nx,ny)	b1.ll.x = MIN(b1.ll.x, nx), \
							b1.ll.y = MIN(b1.ll.y, ny), \
							b1.ur.x = MAX(b1.ur.x, nx), \
							b1.ur.y = MAX(b1.ur.y, ny)

void AddTextGel(), AddLineGel(), AddConicGel(), AddBoxGel();
Gel *FindContainedGels(), *FindIntersectingGels();
Gel *RemoveGel(), *CopyGel(), *SelectGel();
void AdjustGel(), MoveGel();
