/************************************************************
Copyright 1989 by Information-Technology Promotion Agency Japan (IPA),
Mitsubishi Research Institute, Inc. (MRI), and Fuji Xerox Co., Ltd. (FX)
  
                        All Rights Reserved
  
Permission to use, copy, modify and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of IPA, MRI or FX not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  
  
IPA, MRI or FX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL IPA, MRI or FX BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.
  
********************************************************/

#include  <X11/Intrinsic.h>
#include  <X11/StringDefs.h>
#include  <St/StPictureP.h>

#define FAILURE -1
#define SUCCESS 0;

#define	FIRST	  1
#define	SUCC	  2
#define	PARENT	  3
#define	END	  4
#define	ENDFIRST  5
#define	ENDLAST	  6
#define	ENDMIDDLE 7

static Boolean  _AllocObject();
static void     _FreeObject();
static void     _LinkObject();
static void     _BreakObject();
static void     _UnlinkObject();
static Boolean  _NewGroup();
static Boolean  _NewFigure();
static Boolean  _CopyObjectTree();
static void     _TransformObject();

StObjFunctionDescRec objfunctiondesc = {
					_AllocObject,
					_FreeObject,
					_LinkObject,
					_BreakObject,
					_UnlinkObject,
					_NewGroup,
					_NewFigure,
					_CopyObjectTree,
					_TransformObject
};

static Boolean
_AllocObject(w, size, obj_ptr)
	Widget          w;
	int             size;
	StObject      **obj_ptr;
{
	StDrawControl  *dc = GetDrawControl(w);
	int             obj_size;

	obj_size = size +
		((char *) &(dc->dummy_object.body) - (char *) &(dc->dummy_object));
	if ((*obj_ptr = (StObject *) calloc((unsigned int) 1, obj_size)) == NULL)
		return FAILURE;

	return SUCCESS;
}

static void
_FreeObject(obj_ptr)
	StObject       *obj_ptr;
{
	if ((obj_ptr->type == Stline) || (obj_ptr->type == Stpolyline) ||
	    (obj_ptr->type == Stpolygon) || (obj_ptr->type == Stfreehand))
		free(obj_ptr->body.pd_line.point_list.points);

	if (obj_ptr->type == Sttext)
		free(obj_ptr->body.pd_text.text.string);

	free(obj_ptr);
}

static int
UnlinkJobGet(obj_ptr)
	StObject       *obj_ptr;
{
	static int      work_job[2][2][2] = {{END, ENDFIRST},
					     {ENDLAST, ENDMIDDLE},
					     {PARENT, SUCC},
					     {PARENT, SUCC}};
	if (obj_ptr->n_obj == 1) {
		FreeObject(obj_ptr->body.p_group.first);
		obj_ptr->body.p_group.first = NULL;
	}
	if (obj_ptr->body.p_group.first != NULL)
		return FIRST;

	return work_job[!(!obj_ptr->parent)]
		[!(!obj_ptr->body.p_group.pred)]
		[!(!obj_ptr->body.p_group.succ)];
}

static void
_LinkObject(first, last, succ_ptr, grp_ptr)
	StObject       **first;
	StObject       **last;
	StObject       *succ_ptr;	/* inserting point in the chain */
	StObject       *grp_ptr;
{
	StObject       *side_ptr;

	if (succ_ptr != (StObject *) NULL) {
		side_ptr = succ_ptr;
		grp_ptr->body.p_group.pred = side_ptr->body.p_group.pred;
		grp_ptr->body.p_group.succ = side_ptr;
		side_ptr->body.p_group.pred = grp_ptr;
	} else {
		grp_ptr->body.p_group.pred = *last;
		grp_ptr->body.p_group.succ = (StObject *) NULL;
		*last = grp_ptr;
	}

	if (grp_ptr->body.p_group.pred != (StObject *) NULL) {
		side_ptr = grp_ptr->body.p_group.pred;
		side_ptr->body.p_group.succ = grp_ptr;
	} else {
		*first = grp_ptr;
	}
}

static void
_BreakObject(first, last, pred_ptr, succ_ptr)
	StObject       **first;
	StObject       **last;
	StObject       *pred_ptr;	/* inserting point in the chain */
	StObject       *succ_ptr;	/* inserting point in the chain */
{
	if (succ_ptr != (StObject *) NULL) {
		succ_ptr->body.p_group.pred = pred_ptr;
	} else {
		if (pred_ptr != (StObject *) NULL)
			pred_ptr->body.p_group.succ = (StObject *) NULL;
		*last = pred_ptr;
	}
	if (pred_ptr != (StObject *) NULL) {
		pred_ptr->body.p_group.succ = succ_ptr;
	} else {
		if (succ_ptr != (StObject *) NULL)
			succ_ptr->body.p_group.pred = (StObject *) NULL;
		*first = succ_ptr;
	}
}

static void
_UnlinkObject(w, obj_ptr)
	Widget          w;
	StObject       *obj_ptr;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *save_ptr = obj_ptr;

	switch (UnlinkJobGet(obj_ptr)) {
	case FIRST:
		obj_ptr = obj_ptr->body.p_group.first;
		break;

	case SUCC:
		obj_ptr = obj_ptr->body.p_group.succ;
		free(save_ptr);
		break;

	case PARENT:
		obj_ptr->parent->body.p_group.first = NULL;
		obj_ptr = obj_ptr->parent;
		free(save_ptr);
		break;

	case END:
		free(save_ptr);
		dc->first_grp = NULL;
		dc->last_grp = NULL;
		return;

	case ENDFIRST:
		obj_ptr->body.p_group.succ->body.p_group.pred = NULL;
		dc->first_grp = obj_ptr->body.p_group.succ;
		free(save_ptr);
		return;

	case ENDLAST:
		obj_ptr->body.p_group.pred->body.p_group.succ = NULL;
		dc->last_grp = obj_ptr->body.p_group.pred;
		free(save_ptr);
		return;

	case ENDMIDDLE:
		obj_ptr->body.p_group.pred->body.p_group.succ =
			obj_ptr->body.p_group.succ;
		obj_ptr->body.p_group.succ->body.p_group.pred =
			obj_ptr->body.p_group.pred;
		free(save_ptr);
		return;
	}

	UnlinkObject(w, obj_ptr);
}

static Boolean
_NewGroup(w, x, y, width, height, first, last, new_grp, fno, need_number)
	Widget          w;
	short           x, y;	/* widget coordinates */
	unsigned short  width, height;	/* widget coordinates */
	StObject       *first, *last, **new_grp;
	StFigure       *fno;
	Boolean         need_number;
{
	StObject       *grp_ptr;

	*new_grp = (StObject *) NULL;
	if (AllocObject(w, sizeof(StGroup), &grp_ptr))
		return FAILURE;	/* error */

	if (need_number) {
		if (NewObjNumber(w, fno)) {
			free((char *) grp_ptr);
			return FAILURE;	/* error */
		}
		SetObjAddr(w, *fno, grp_ptr);	/* set hash-table */
	} else
		*fno = StNullFigure;

	grp_ptr->type = Stgroup;
	grp_ptr->obj_no = *fno;
	grp_ptr->n_obj = 1;
	grp_ptr->parent = (StObject *) NULL;

	grp_ptr->body.p_group.is_selected = StunsetSelect;
	grp_ptr->body.p_group.is_handled = StunsetHandle;
	grp_ptr->body.p_group.obj_pos.x = x;
	grp_ptr->body.p_group.obj_pos.y = y;
	grp_ptr->body.p_group.w = width;
	grp_ptr->body.p_group.h = height;
	grp_ptr->body.p_group.succ = (StObject *) NULL;
	grp_ptr->body.p_group.pred = (StObject *) NULL;
	grp_ptr->body.p_group.first = first;
	grp_ptr->body.p_group.last = last;

	*new_grp = grp_ptr;
	return SUCCESS;
}

static Boolean
_NewFigure(w, x, y, width, height, obj_ptr, fno)
	Widget          w;
	short           x, y;	/* widget coordinate */
	unsigned short  width, height;	/* widget coordinate */
	StObject       *obj_ptr;
	StFigure       *fno;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *grp_ptr;

	if (NewGroup(w, x, y, width, height, obj_ptr, obj_ptr, &grp_ptr, fno, True)) {
		FreeObject(obj_ptr);
		return FAILURE;	/* error */
	}
	obj_ptr->parent = grp_ptr;

	LinkObject(&dc->first_grp, &dc->last_grp, (StObject *) NULL, grp_ptr);

	return SUCCESS;
}

static Boolean
CopyPrimObject(w, src, new)
	Widget          w;
	StObject       *src;
	StObject       **new;
{
	register        i;

	AllocObject(w, GetSize(src->type), new);

	(*new)->type = src->type;
	(*new)->obj_no = src->obj_no;
	(*new)->n_obj = src->n_obj;
	(*new)->parent = src->parent;	/* reset in CopyObjectTree */
	switch ((int) src->type) {
	case Strectangle:
		Memcpy(&((*new)->body), &(src->body), sizeof(StRectPrimitive));
		break;
	case Stcircle:
	case Stellipse:
	case Starc:
	case StellipticArc:
	case Stpie:
	case StellipticPie:
		Memcpy(&((*new)->body), &(src->body), sizeof(StArcPrimitive));
		break;
	case Stline:
	case Stpolygon:
	case Stpolyline:
	case Stfreehand:
		Memcpy(&((*new)->body), &(src->body), sizeof(StLinePrimitive));
		(*new)->body.pd_line.point_list.points = (StPoint *) malloc(
		   sizeof(StPoint) * src->body.pd_line.point_list.n_points);
		for (i = 0; i < src->body.pd_line.point_list.n_points; i++) {
			(*new)->body.pd_line.point_list.points[i].x =
				src->body.pd_line.point_list.points[i].x;
			(*new)->body.pd_line.point_list.points[i].y =
				src->body.pd_line.point_list.points[i].y;
		}
		break;
	case Sttext:
		Memcpy(&((*new)->body), &(src->body), sizeof(StTextPrimitive));
		(*new)->body.pd_text.text.string = (char *) malloc(
					src->body.pd_text.text.n_bytes + 1);
		for (i = 0; i < src->body.pd_text.text.n_bytes; i++) {
			(*new)->body.pd_text.text.string[i] =
				src->body.pd_text.text.string[i];
		}
		(*new)->body.pd_text.text.string[i] = '\0';
		break;
	case Stbitmap:
		Memcpy(&((*new)->body), &(src->body), sizeof(StBitmapPrimitive));
		(*new)->body.pd_bitmap.bitmap.bitmap_file = (char *) malloc(
				    src->body.pd_bitmap.bitmap.n_bytes + 1);
		for (i = 0; i < src->body.pd_bitmap.bitmap.n_bytes; i++) {
			(*new)->body.pd_bitmap.bitmap.bitmap_file[i] =
				src->body.pd_bitmap.bitmap.bitmap_file[i];
		}
		(*new)->body.pd_bitmap.bitmap.bitmap_file[i] = '\0';
		break;
	}

	return SUCCESS;
}

static Boolean
_CopyObjectTree(w, src, f, node_depth)
	Widget          w;
	StObject       *src;
	StObject       **f;
	int             node_depth;
{
	StObject       *first = (StObject *) NULL;
	StObject       *last = (StObject *) NULL;
	StObject       *grp;
	StObject       *side_ptr;
	StFigure        fno;
	int             obj_cnt;
	Boolean         need_number;

	if (src->n_obj > 1) {
		CopyObjectTree(w, src->body.p_group.first, &first, 1);
		last = first;
		while (last->body.p_group.succ)
			last = last->body.p_group.succ;
	} else {
		CopyPrimObject(w, src->body.p_group.first, &first);
		last = first;
	}

	if (node_depth)
		need_number = False;
	else
		need_number = True;

	NewGroup(w, 0, 0, src->body.p_group.w, src->body.p_group.h,
		 first, last, &grp, &fno, need_number);
	grp->body.p_group.obj_pos.x = src->body.p_group.obj_pos.x;
	grp->body.p_group.obj_pos.y = src->body.p_group.obj_pos.y;
	side_ptr = grp->body.p_group.first;
	obj_cnt = 0;
	if (side_ptr->type == Stgroup) {
		while (side_ptr) {
			side_ptr->parent = grp;
			side_ptr = side_ptr->body.p_group.succ;
			obj_cnt++;
		}
	} else {
		side_ptr->parent = grp;
		obj_cnt = 1;
	}

	grp->n_obj = obj_cnt;
	*f = grp;

	if (node_depth && src->body.p_group.succ) {
		CopyObjectTree(w, src->body.p_group.succ,
			       &(grp->body.p_group.succ), 1);
		side_ptr = grp->body.p_group.succ;
		side_ptr->body.p_group.pred = grp;
	}
	return SUCCESS;
}

static void
TransformPrimObject(w, obj_ptr, x_mul, x_div, y_mul, y_div)
	Widget          w;
	StObject       *obj_ptr;
	unsigned short  x_mul, x_div, y_mul, y_div;
{
	register        i;
	short           x, y;

	switch ((int) obj_ptr->type) {
	case Strectangle:
		ScaleObject(x_mul, x_div, &(obj_ptr->body.pd_rect.rectangle.width));
		ScaleObject(y_mul, y_div, &(obj_ptr->body.pd_rect.rectangle.height));
		break;
	case Stcircle:
	case Stellipse:
		ScaleObject(x_mul, x_div, &(obj_ptr->body.pd_arc.arc.width));
		ScaleObject(y_mul, y_div, &(obj_ptr->body.pd_arc.arc.height));
		break;
	case Starc:
	case StellipticArc:
	case Stpie:
	case StellipticPie:
		ScaleObject(x_mul, x_div, &(obj_ptr->body.pd_arc.arc.width));
		ScaleObject(y_mul, y_div, &(obj_ptr->body.pd_arc.arc.height));
		/* transform angles */
		break;
	case Stline:
	case Stpolygon:
	case Stpolyline:
	case Stfreehand:
		for (i = 0; i < obj_ptr->body.pd_line.point_list.n_points; i++) {
			x = obj_ptr->body.pd_line.point_list.points[i].x;
			y = Abs(obj_ptr->body.pd_line.point_list.points[i].y);
			ScaleObject(x_mul, x_div, &x);
			ScaleObject(y_mul, y_div, &y);
			obj_ptr->body.pd_line.point_list.points[i].x = x;
			obj_ptr->body.pd_line.point_list.points[i].y = -(y);
		}
		break;
	case Sttext:
		/* do nothing */
		break;
	case Stbitmap:
		/* do nothing */
		break;
	}

}

static void
_TransformObject(w, grp_ptr, x_mul, x_div, y_mul, y_div, node_depth)
	Widget          w;
	StObject       *grp_ptr;
	unsigned short  x_mul, x_div, y_mul, y_div;
	int             node_depth;
{
	short           tx, ty;

	if (grp_ptr->n_obj > 1)
		TransformObject(w, grp_ptr->body.p_group.first,
				x_mul, x_div, y_mul, y_div, 1);
	else
		TransformPrimObject(w, grp_ptr->body.p_group.first,
				    x_mul, x_div, y_mul, y_div);
	if (node_depth) {
		tx = grp_ptr->body.p_group.obj_pos.x;
		ty = Abs(grp_ptr->body.p_group.obj_pos.y);
		ScaleObject(x_mul, x_div, &tx);
		ScaleObject(y_mul, y_div, &ty);
		grp_ptr->body.p_group.obj_pos.x = tx;
		grp_ptr->body.p_group.obj_pos.y = -(ty);
	}
	ScaleObject(x_mul, x_div, &(grp_ptr->body.p_group.w));
	ScaleObject(y_mul, y_div, &(grp_ptr->body.p_group.h));

	if (node_depth && grp_ptr->body.p_group.succ)
		TransformObject(w, grp_ptr->body.p_group.succ,
				x_mul, x_div, y_mul, y_div, 1);
}
