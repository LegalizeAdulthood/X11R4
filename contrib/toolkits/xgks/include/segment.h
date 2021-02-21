#if !defined(XGKS_SEGMENT_H)
#define XGKS_SEGMENT_H

void XgksSetFillPatAttrMo(WS_STATE_PTR ws, Gflattr *flattr, PTATTR *ptattr);
void XgksSetLineAttrMo(WS_STATE_PTR ws, Glnattr *lnattr);
void XgksSetMarkAttrMo(WS_STATE_PTR ws, Gmkattr *mkattr);
void XgksSetTextAttrMo(WS_STATE_PTR ws, Gtxattr *txattr, CHATTR *chattr);

#endif
