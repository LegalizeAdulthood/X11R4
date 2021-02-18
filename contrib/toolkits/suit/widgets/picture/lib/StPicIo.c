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

/*
 *      Picture Widget File Access Functions.
 */

#include    <stdio.h>
#include    <X11/Xlib.h>
#include    <X11/Intrinsic.h>
#include    <X11/StringDefs.h>
#include    <St/StPictureP.h>

#define     FAILURE         -1
#define     SUCCESS         0

#define     FIGURE          1
#define     GROUP_START	    2
#define     GROUP_END       3

#define   O_ELLIPSE           1
#define   O_POLYLINE          2
#define   O_SPLINE            3
#define   O_TEXT              4
#define   O_ARC               5
#define   O_COMPOUND          6
#define   O_BITMAP            7
#define   O_END_COMPOUND      -O_COMPOUND
#define   O_ALL_OBJECT        99

#define   BUF_SIZE            80

#define   OBJ_START           6
#define   OBJ_END            -6

#define   POLYLINE            1
#define   POLYGON             3

#define   SOLID_LINE          0
#define   DASH_LINE           1
#define   DOTTED_LINE         2

#define   DEPTH               0
#define   STYLE_VAL           0.0

#define   ON                  1
#define   OFF                 0

#define   BOX                 2
#define   ANGLE               0.0

#define   PSIZ 10                    /*  Primitive Type Size  */

#define   XFIG_NAME_AND_VERSION		"#FIG 1.4"
#define   STPIC_NAME_AND_VERSION	"#StPicture 0.1"

#define   XFIG_RESOLUTION_80	80

#define   XFIG_COORD_SYSTEM_LOWER_LRFT	1

#define   XFIG_COORD_SYSTEM_UPPER_LRFT	2

static void         _ReadFile();
static StFigure     _ReadFigure();
static StFigure     _ReadGroup();
static int          _ReadObject();
static void         _WriteFile();
static int          _WriteGroup();
static int          _WriteObject();
static int          _ReadHeader();
static void         _WriteHeader();

static int          Get_Primtype();
static int          Read_Line();
static int          Read_Rectangle();
static int          Read_Arc();
static int          Read_Text();
static int          Read_Bitmap();
static int          Read_Group();

static int          Write_Line();
static int          Write_Rectangle();
static int          Write_Arc();
static int          Write_Text();
static int          Write_Bitmap();

static void	    AdjustArcInfo();

StIoFunctionDescRec iofunctiondesc = {
                    _ReadFile,
                    _ReadFigure,
                    _ReadGroup,
                    _ReadObject,
                    _WriteFile,
                    _WriteGroup,
                    _WriteObject,
                    _ReadHeader,
                    _WriteHeader,
};

static char         buf[BUF_SIZE];

static int          grp_nest = 0;
static StPoint      *offset;

typedef union {
    StArcPrimitive     *arc_packet;
    StLinePrimitive    *line_packet;
    StRectPrimitive    *rect_packet;
    StTextPrimitive    *text_packet;
    StBitmapPrimitive  *bitmap_packet;
} Packet;



static void     
_ReadFile(spw)
StPictureWidget  spw;
{
	Widget	w = (Widget)spw;
	StDrawControl *dc = GetDrawControl(w);
	StObject *obj;

	if ( spw->stPicture.read_flg == False ) { /* Override */
		StPicClearView(w);
		StPicRedrawAll(w);
	} else {				/* Merge */
		if (dc->n_selected > 0){
/* get current selected figre and collect disending order */
			SetupTergetFigures(w, True);
			SelectAll(w, StunsetSelect);  /* erase old handle */
		}
		obj = dc->last_grp;
	}
	if (StPicReadFile(w, spw->stPicture.read_file) != Success)
		return;
	if ( spw->stPicture.read_flg == False ) /* Override */
		StPicRedrawAll(w);
	else {				/* Merge */
		if (obj)
		    obj = obj->body.p_group.succ;
		else
		    obj = dc->first_grp;
		while (obj) {
			StPicDrawFigure(w, obj->obj_no);
			obj = obj->body.p_group.succ;
		}
	}
}

static StFigure
_ReadFigure(w, prim_ptr)
Widget	w;
char	*prim_ptr;
{

    StFigure        figure = StNullFigure;
    StCoordinate    coord = StCoordWidget;
    StLineAtr       *line_atr;
    StPaintAtr      *paint_atr;
    StTextAtr       *text_atr;

    switch ((int)((StArcPrimitive *)prim_ptr)->type) {
        case Strectangle:
            figure = StPicCreateRectangle(w,
                    &((StRectPrimitive *)prim_ptr)->rectangle, coord);
            line_atr = &((StRectPrimitive *)prim_ptr)->line_atr;
            paint_atr = &((StRectPrimitive *)prim_ptr)->paint_atr;
            break;

        case Stcircle:
        case Stellipse:
            figure = StPicCreateEllipse(w, 
                    &((StArcPrimitive *)prim_ptr)->arc, coord);    
            line_atr = &((StArcPrimitive *)prim_ptr)->line_atr;
            paint_atr = &((StArcPrimitive *)prim_ptr)->paint_atr;
            break;

        case Starc:
        case StellipticArc:
            figure = StPicCreateArc(w, 
                    &((StArcPrimitive *)prim_ptr)->arc, coord);    
            line_atr = &((StArcPrimitive *)prim_ptr)->line_atr;
            paint_atr = &((StArcPrimitive *)prim_ptr)->paint_atr;
            break;

        case Stpie:
        case StellipticPie:
            figure = StPicCreatePie(w, 
                    &((StArcPrimitive *)prim_ptr)->arc, coord);    
            line_atr = &((StArcPrimitive *)prim_ptr)->line_atr;
            paint_atr = &((StArcPrimitive *)prim_ptr)->paint_atr;
            break;

        case Stline:
            figure = StPicCreateLine(w,
                    ((StLinePrimitive *)prim_ptr)->point_list.points[0].x,
                    ((StLinePrimitive *)prim_ptr)->point_list.points[0].y,
                    ((StLinePrimitive *)prim_ptr)->point_list.points[1].x,
                    ((StLinePrimitive *)prim_ptr)->point_list.points[1].y, coord);
            line_atr = &((StLinePrimitive *)prim_ptr)->line_atr;
            paint_atr = &((StLinePrimitive *)prim_ptr)->paint_atr;

            free(((StLinePrimitive *)prim_ptr)->point_list.points);
            break;

        case Stpolyline:
            figure = StPicCreatePolyline(w, 
                    &((StLinePrimitive *)prim_ptr)->point_list, coord);
            line_atr = &((StLinePrimitive *)prim_ptr)->line_atr;
            paint_atr = &((StLinePrimitive *)prim_ptr)->paint_atr;

            free(((StLinePrimitive *)prim_ptr)->point_list.points);
            break;

        case Stfreehand:
            figure = StPicCreateFreehand(w, 
                    &((StLinePrimitive *)prim_ptr)->point_list, coord);
            line_atr = &((StLinePrimitive *)prim_ptr)->line_atr;
            paint_atr = &((StLinePrimitive *)prim_ptr)->paint_atr;

            free(((StLinePrimitive *)prim_ptr)->point_list.points);
            break;

        case Stpolygon:
            figure = StPicCreatePolygon(w, 
                    &((StLinePrimitive *)prim_ptr)->point_list, coord);
            line_atr = &((StLinePrimitive *)prim_ptr)->line_atr;
            paint_atr = &((StLinePrimitive *)prim_ptr)->paint_atr;

            free(((StLinePrimitive *)prim_ptr)->point_list.points);
            break;

        case Sttext:
            figure = StPicCreateText(w, 
                    &((StTextPrimitive *)prim_ptr)->text, coord);
            text_atr = &((StTextPrimitive *)prim_ptr)->text_atr;

            free(((StTextPrimitive *)prim_ptr)->text.string);
            break;

        case Stbitmap:
            figure = StPicCreateBitmap(w, 
                    &((StBitmapPrimitive *)prim_ptr)->bitmap, coord);
            paint_atr = &((StBitmapPrimitive *)prim_ptr)->paint_atr;

            free(((StBitmapPrimitive *)prim_ptr)->bitmap.bitmap_file);
            break;
    }

    if (figure != StNullFigure) {
        if (((StTextPrimitive *)prim_ptr)->type == Sttext)
            StPicSetTextAtr(w, figure, text_atr);

        else if (((StBitmapPrimitive *)prim_ptr)->type == Stbitmap)
            StPicSetPaintAtr(w, figure, paint_atr);

        else {
            StPicSetLineAtr(w, figure, line_atr);
            StPicSetPaintAtr(w, figure, paint_atr);
        }
    }

    free(prim_ptr);
    return figure;
}
 
static StFigure
_ReadGroup(w, fp)
Widget  w;
FILE    *fp;
{
    StFigure        figure;
    StFigureList    figure_list;
    int             flag;
    char            *prim_ptr;


    if (IsWidget(w) == False)
        return StNullFigure;

    if ((figure_list.figures = (StFigure *)malloc(sizeof(StFigure))) == NULL)
        return StNullFigure;

    for (figure_list.n_figures = 0;
            (flag = ReadObject(fp, &prim_ptr)) != GROUP_END;
            figure_list.n_figures++) {
        if (flag == FIGURE) {
            if ((figure_list.figures[figure_list.n_figures] = ReadFigure(w, prim_ptr)) ==
                    StNullFigure) {
                free(figure_list.figures);
                return StNullFigure;
            }
        }

        if (flag == GROUP_START) {
            if ((figure_list.figures[figure_list.n_figures] = ReadGroup(w, fp)) == 
                    StNullFigure) {
                free(figure_list.figures);
                return StNullFigure;
            }
        }

	if ((flag == FAILURE) || (flag == SUCCESS)) {
            free(figure_list.figures);
            return StNullFigure;
        }

        if ((figure_list.figures = (StFigure *)realloc(figure_list.figures,
                sizeof(StFigure) * (figure_list.n_figures + 2))) == NULL) {
            free(figure_list.figures);
            return StNullFigure;
        }
    }

    figure = StPicGroupFigure(w, &figure_list);
    free(figure_list.figures);
    return figure;
}
 

static void     
_WriteFile(spw)
StPictureWidget  spw;
{
	StPicWriteFile(spw, spw->stPicture.write_file);
}

static int
_WriteGroup(fp, grp_ptr)
FILE        *fp;
StObject    *grp_ptr;
{
    StObject    *work_ptr;

    if (grp_ptr->n_obj == 1) {
        if (WriteObject(fp, grp_ptr, GROUP_START) == FAILURE)
            return FAILURE;
    }

    else {
        if (WriteObject(fp, grp_ptr, GROUP_START) == FAILURE)
            return FAILURE;

        work_ptr = grp_ptr->body.p_group.first;

        while (1) {
            if (WriteGroup(fp, work_ptr) == FAILURE)
                return FAILURE;

            if (work_ptr->body.p_group.succ == NULL)
                break;
            work_ptr = work_ptr->body.p_group.succ;
        }

        if (WriteObject(fp, grp_ptr, GROUP_END) == FAILURE)
            return FAILURE;
    }

    return SUCCESS;
}
/* 
 *                      Read Figure Data
 *
 *    ret_flag = ReadObject(  fp,  prim_ptr  ) ;
 * 
 *      FILE  *fp         ...  Data File Pointer
 *
 *      int   *prim_ptr   ...  Figure_Primitive Pointer
 *
 *      int   ret_flag    ...  = 0  SUCCESS 
 *                             = 1  Figure_Data
 *                             = 2  Start_Group
 *                             = 3  End_Group
 *                             =-1  Error
 */


static int
_ReadObject(fp, prim_ptr)
FILE          *fp;
Packet       **prim_ptr;
{
    char  p_type[PSIZ];      /*  Primitive Type  */
    int   rc;
/* Start ReadObject */
    switch (Get_Primtype(fp, p_type)) {      /*  Get Primitive Type  */
        case SUCCESS:
            rc = SUCCESS;
            break;

        case FAILURE:
            rc = FAILURE;
            break;

        case FIGURE:
            if (strcmp(p_type, "Line") == 0)
                rc = Read_Line(fp, prim_ptr);

            else if (strcmp(p_type, "Rect") == 0)
                rc = Read_Rectangle(fp, prim_ptr);

            else if (strcmp(p_type, "Arc") == 0)
                rc = Read_Arc(fp, prim_ptr); 

            else if (strcmp(p_type, "Text") == 0)
                rc = Read_Text(fp, prim_ptr);

            else if (strcmp(p_type, "Bitmap") == 0)
                rc = Read_Bitmap(fp, prim_ptr);

            else if (strcmp(p_type, "Group") == 0)
                rc = Read_Group(fp);

            else
                rc = FAILURE;
            break;

        default:
            rc = FAILURE;
    }

     return rc;
}



static int
Get_Primtype(fp, p_type)
FILE  *fp;
char  *p_type;      /*  Primitive Type  */
{
    int     i, j;

    if (feof(fp))
        return SUCCESS;  /*  EOF  */

    if (fgets(buf, BUF_SIZE, fp) == NULL)
        return SUCCESS;  /*  EOF  */

    if (feof(fp))
        return SUCCESS;  /*  EOF  */
    else if ((buf[0] != '#'))
        return FAILURE;

    for (i = 0; buf[i] != ' '; i++);  /*  Skip "#StFIG_0.1"  */
    for (     ; buf[i] == ' '; i++);  /*  Skip Space         */
    for (j = 0; buf[i] != ' '; i++, j++)
        p_type[j] = buf[i];

    p_type[j] = NULL;

    return FIGURE;                    /*  Success  */
}



static int
Read_Line(fp, prim_ptr)
FILE     *fp;
Packet   **prim_ptr;
{
    StLinePrimitive   *l;

    int     forward_arrow;
    int     backward_arrow;
    int     i;
    int     dmy_int;
    float   dmy_float;
    char    dmy_char;
    char    dmy_arr[80];
    int     width, fore, func;
    float   fill;
    int     px, py;

    if ((l = (StLinePrimitive *)malloc(sizeof(StLinePrimitive))) == NULL)
        return FAILURE;

/* Set Attributes  */
                                                    /*  Pass Comment  */
    if (fgets(buf, BUF_SIZE, fp) == NULL)
        return FAILURE;
    fscanf(fp, "%s %d ", dmy_arr, &l->type);
                                                    /*  Pass Comment  */
    if (fgets(buf, BUF_SIZE, fp) == NULL)
        return FAILURE;
    fscanf(fp, "%s %d %d %d %d %d %d %d %d ",
            dmy_arr,        /* #! */
            &l->line_atr.line_style,
            &l->line_atr.line_width,
            &l->line_atr.begin_cap,
            &l->line_atr.end_cap,
            &l->line_atr.join_style,
            &l->line_atr.line_foreground,
            &l->line_atr.line_background,
            &l->line_atr.function);
                                                    /*  Pass Comment  */
    if (fgets(buf, BUF_SIZE, fp) == NULL)
        return FAILURE;
    fscanf(fp, "%s %d %d %d ",
            dmy_arr,        /* #! */
            &l->paint_atr.fill_style,
            &l->paint_atr.fill_foreground,
            &l->paint_atr.fill_background);

/* Get Attributes from figure_file */
    fscanf(fp, "%d %d %d %d %d %d %d %d %f %d %d ",
            &dmy_int,                          /*  O_POLYLINE     */
            &dmy_int,                          /*  sub_type       */
            &dmy_int,                          /*  style          */
            &l->line_atr.line_width,
            &l->line_atr.line_foreground,
            &dmy_int,                          /*  DEPTH          */
            &l->line_atr.function,
            &l->paint_atr.fill_style,
            &dmy_float,                        /*  STYLE_VAL      */
            &forward_arrow,                    /*  forward_arrow  */
            &backward_arrow);                  /*  backward_arrow */

                                             /*  Pass Arrow_Cap Data  */
    if (forward_arrow == 1)
        if (fgets(buf, BUF_SIZE, fp) == NULL)
            return FAILURE;
                                             /*  Pass Arrow_Cap Data  */
    if (backward_arrow == 1)
        if (fgets(buf, BUF_SIZE, fp) == NULL)
            return FAILURE;

/* Get Point_List  */
    if ((l->point_list.points  = (StPoint *)malloc(sizeof(StPoint))) == NULL)
        return FAILURE;

    for (i = 1; ;i++) {
        fscanf(fp, "%hd %hd ",
                &l->point_list.points[i - 1].x,
                &l->point_list.points[i - 1].y);

        if (l->point_list.points[i- 1].x == 9999) {
            l->point_list.n_points = i - 1;
            break;
        }
        else {
            if ((l->point_list.points = (StPoint *)realloc(l->point_list.points,
                    sizeof(StPoint) * (i + 1))) == NULL)
                return FAILURE;
        }
    }

    (StLinePrimitive *)*prim_ptr = l;
    return FIGURE;
} /* Read_Line end */



static int
Read_Rectangle(fp, prim_ptr)
FILE     *fp;
Packet   **prim_ptr;
{
    StRectPrimitive  *r;
    StLinePrimitive  *rr;

    int     forward_arrow;
    int     backward_arrow;
    int     i;
    int     dmy_int;
    float   dmy_float;
    char    dmy_char;
    char    dmy_arr[80];
    int     width, fore, func;
    float   fill;
    int     px, py;

    if ((r = (StRectPrimitive *)malloc(sizeof(StRectPrimitive))) == NULL)
        return FAILURE;

    if ((rr = (StLinePrimitive *)malloc(sizeof(StLinePrimitive))) == NULL)
        return FAILURE;

/* Set Attributes  */
    if (fgets(buf, BUF_SIZE, fp) == NULL)
        return FAILURE;
    fscanf(fp, "%s %d ",dmy_arr, &r->type);

    if (fgets(buf, BUF_SIZE, fp) == NULL)
        return FAILURE;
    fscanf(fp, "%s %d %d %d %d %d %d %d %d ",
            dmy_arr,        /* #! */
            &r->line_atr.line_style,
            &r->line_atr.line_width,
            &r->line_atr.begin_cap,
            &r->line_atr.end_cap,
            &r->line_atr.join_style,
            &r->line_atr.line_foreground,
            &r->line_atr.line_background,
            &r->line_atr.function);

                                                   /*  Pass Comment  */
    if (fgets(buf, BUF_SIZE, fp) == NULL)
        return FAILURE;
    fscanf(fp, "%s %d %d %d ",
            dmy_arr,        /* #! */
            &r->paint_atr.fill_style,
            &r->paint_atr.fill_foreground,
            &r->paint_atr.fill_background);

/* Set xfig data  */
    fscanf(fp, "%d %d %d %d %d %d %d %d %f %d %d ",
            &dmy_int,                          /*  O_POLYLINE     */
            &dmy_int,                          /*  sub_type       */
            &dmy_int,                          /*  style          */
            &r->line_atr.line_width,
            &r->line_atr.line_foreground,
            &dmy_int,                          /*  DEPTH          */
            &r->line_atr.function,
            &r->paint_atr.fill_style,
            &dmy_float,                        /*  STYLE_VAL      */
            &forward_arrow,                    /*  forward_arrow  */
            &backward_arrow);                  /*  backward_arrow */

                                             /*  Pass Arrow_Cap Data  */
    if (forward_arrow == 1)
        if (fgets(buf, BUF_SIZE, fp) == NULL)
            return FAILURE;

                                             /*  Pass Arrow_Cap Data  */
    if (backward_arrow == 1)
        if (fgets(buf, BUF_SIZE, fp) == NULL)
            return FAILURE;

/* Get Point_List  */
    if ((rr->point_list.points  = (StPoint *)malloc(sizeof(StPoint))) == NULL)
        return FAILURE;

    for (i=1; ; i++) {
        fscanf(fp, "%hd %hd ",
                &rr->point_list.points[i - 1].x,
                &rr->point_list.points[i - 1].y);

        if (rr->point_list.points[i - 1].x == 9999) {
            rr->point_list.n_points = i - 1;
            break;
        }
        else
            if ((rr->point_list.points = (StPoint *)realloc(rr->point_list.points, sizeof(StPoint) * (i + 1))) == NULL)
                  return FAILURE;
    }

    r->rectangle.x      = rr->point_list.points[0].x;
    r->rectangle.y      = rr->point_list.points[0].y;
    r->rectangle.width  = rr->point_list.points[1].x - r->rectangle.x;
    r->rectangle.height = r->rectangle.y - rr->point_list.points[3].y;

    free(rr->point_list.points);
    free(rr);

    (StRectPrimitive *)*prim_ptr = r;
    return FIGURE;
} /* Read_Rectangle end  */



static int
Read_Arc(fp, prim_ptr )
FILE     *fp;
Packet   **prim_ptr;
{
     StArcPrimitive   *a;

     int        forward_arrow;
     int        backward_arrow;
     StPoint    point[3];
     float      center_x, center_y;
     int        i;
     int        dmy_int;
     float      dmy_float;
     char       dmy_char;
     char       dmy_arr[80];

     if ((a = (StArcPrimitive *)malloc(sizeof(StArcPrimitive))) == NULL)
         return FAILURE;

/* Set Attributes */
                                                    /*  Pass Comment  */
     if (fgets(buf, BUF_SIZE, fp) == NULL)
         return FAILURE;
     fscanf(fp, "%s %d ",dmy_arr, &a->type);
                                                    /*  Pass Comment  */
     if (fgets(buf, BUF_SIZE, fp) == NULL)
         return FAILURE;
     fscanf(fp, "%s %d %d %d %d %d %d %d %d ",
             dmy_arr,        /* #! */
             &a->line_atr.line_style,
             &a->line_atr.line_width,
             &a->line_atr.begin_cap,
             &a->line_atr.end_cap,
             &a->line_atr.join_style,
             &a->line_atr.line_foreground,
             &a->line_atr.line_background,
             &a->line_atr.function);
                                                    /*  Pass Comment  */
     if (fgets(buf, BUF_SIZE, fp) == NULL)
         return FAILURE;
     fscanf(fp, "%s %d %d %d ",
             dmy_arr,        /* #! */
             &a->paint_atr.fill_style,
             &a->paint_atr.fill_foreground,
             &a->paint_atr.fill_background);
                                                    /*  Pass Comment  */
     if (fgets(buf, BUF_SIZE, fp) == NULL)
         return FAILURE;
     fscanf(fp, "%s %d %hd %hd %hd %hd %hd %hd ",
             dmy_arr,
             &a->arc.direction,
             &a->arc.x,
             &a->arc.y,
             &a->arc.width,
             &a->arc.height,
             &a->arc.angle1,
             &a->arc.angle2);

/* Set xfig data  */
     fscanf(fp, "%d %d %d %d %d %d %d %d %f ",
            &dmy_int,                           /*  O_ARC         */
            &dmy_int,                           /*  SUB_TYPE      */
            &dmy_int,                           /*  LINE_STYLE    */
            &a->line_atr.line_width,
            &a->line_atr.line_foreground,
            &dmy_int,                           /*  DEPTH         */
            &a->line_atr.function,
            &a->paint_atr.fill_style,
            &dmy_float);                       /*  STYLE_VAL      */

     fscanf(fp, "%d %d %d %f  %f  %hd %hd %hd %hd   %hd %hd ",
            &a->arc.direction,
            &forward_arrow,                    /*  forward_arrow  */
            &backward_arrow,                   /*  backward_arrow */
            &center_x,   &center_y,
            &point[0].x, &point[0].y,
            &point[1].x, &point[1].y,
            &point[2].x, &point[2].y);

                                             /*  Pass Arrow_Cap Data  */
     if (forward_arrow == 1)
         if (fgets(buf, BUF_SIZE, fp) == NULL)
             return FAILURE;
                                             /*  Pass Arrow_Cap Data  */
     if (backward_arrow == 1)
         if (fgets(buf, BUF_SIZE, fp) == NULL)
             return FAILURE;

     (StArcPrimitive *)*prim_ptr = a;

     return FIGURE;
} /* Read_Arc end  */



static int
Read_Text(fp, prim_ptr)
FILE     *fp;
Packet   **prim_ptr;
{
     StTextPrimitive   *t;

     int     i;
     int     dmy_int;
     float   dmy_float;
     char    dmy_char;
     char    dmy_arr[80];

     if ((t = (StTextPrimitive *)malloc(sizeof(StTextPrimitive))) == NULL)
         return FAILURE;

/* Get Attributes from figure_file */
                                                    /*  Pass Comment  */
     if (fgets(buf, BUF_SIZE, fp) == NULL)
         return FAILURE;

     fscanf(fp, "%s %d ", dmy_arr, &t->type);

     fscanf(fp, "%d %d %d %d %d %d %d %f %d %d %d %hd %hd ",
             &dmy_int,                             /*  O_TEXT     */
             &dmy_int,                             /*  sub_type   */
             &t->text_atr.font,                    /*  Font Name  */
             &t->text_atr.font,                    /*  Font Size  */
             &t->text_atr.function,
             &t->text_atr.text_color,
             &dmy_int,                             /*  DEPTH      */
             &dmy_float,                           /*  ANGLE      */
             &t->text_atr.font,                    /*  Font Style */
             &dmy_int,                             /*  height     */
             &t->text.n_bytes,
             &t->text.x,                           /*  cp->x      */
             &t->text.y);                          /*  cp->y      */

     if ((t->text.string = (char *)malloc(t->text.n_bytes + 1)) == NULL)
         return FAILURE;

     /*
     if (fgets(t->text.string, t->text.n_bytes + 1, fp) == NULL)
         return FAILURE;
     */
     for(i = 0; i < t->text.n_bytes; ++i)
	t->text.string[i] = getc(fp);

     if (fgets(buf, BUF_SIZE, fp) == NULL)
         return FAILURE;

/* convert bell to newline *
     for(i = 0; i < t->text.n_bytes; ++i)
	if (t->text.string[i] == '\007')
		t->text.string[i] = '\n';
 */
                                                 /*  xfig data        */
     t->text.string[t->text.n_bytes] = '\0';

     (StTextPrimitive *)*prim_ptr = t ;
     return FIGURE;
} /* Read_Text end  */



static int
Read_Bitmap(fp, prim_ptr )
FILE     *fp;
Packet   **prim_ptr;
{
    StBitmapPrimitive *b;

    int     i;
    int     dmy_int;
    float   dmy_float;
    char    dmy_char;
    char    dmy_arr[80];

    if ((b = (StBitmapPrimitive *)malloc(sizeof(StBitmapPrimitive))) == NULL)
        return FAILURE;

/* Get Attributes from figure_file */
                                                    /*  Pass Comment  */
    if (fgets(buf, BUF_SIZE, fp) == NULL)
        return FAILURE;
    fscanf(fp, "%s %d ", dmy_arr, &b->type);
                                                    /*  Pass Comment  */
    if (fgets(buf, BUF_SIZE, fp) == NULL)
        return FAILURE;
    fscanf(fp, "%s %d %d %d ",
            dmy_arr,        /* #! */
            &b->paint_atr.fill_style,
            &b->paint_atr.fill_foreground,
            &b->paint_atr.fill_background);
                                                    /*  Pass Comment  */
    if (fgets(buf, BUF_SIZE, fp) == NULL)
        return FAILURE;
    fscanf(fp, "%s %d %hd %hd ",
            dmy_arr,        /* #! */
            &b->bitmap.n_bytes,
            &b->bitmap.x,
            &b->bitmap.y);

    if ((b->bitmap.bitmap_file = (char *)malloc(b->bitmap.n_bytes + 1)) == NULL)
        return FAILURE;

    fscanf(fp, "%s ", b->bitmap.bitmap_file);

     (StBitmapPrimitive *)*prim_ptr = b;
     return FIGURE;
} /* Read_Bitmap end  */



static int
Read_Group(fp )
FILE     *fp;
{
    int    group_flag;

/* Get Group_Flag */
    fscanf(fp, "%d ", &group_flag);

    if (group_flag == OBJ_START)
        return GROUP_START;

    else if (group_flag == OBJ_END)
        return GROUP_END;

    else 
            return FAILURE;
} /* Read_Group end  */



/*
 *                   Write out Figure_Data
 *
 *    ret_flag = WriteObject(fp, grp_ptr, grp_flag);
 *
 *      FILE  *fp         ...  Data File Pointer
 *
 *      int   *grp_ptr    ...  Figure_Primitive Pointer
 *
 *      int   grp_flag    ...  = 0  Figure_Data
 *                             = 1  Start_Group
 *                             = 2  End_Group
 */



static int
_WriteObject(fp, grp_ptr, grp_flag )
FILE        *fp;
StObject    *grp_ptr;
int         grp_flag;
{
    int         i;
    int         rc;
    StObject    *obj_ptr = grp_ptr->body.p_group.first;
    StPoint     grp_pos, local_g_pos;
    StArcPrimitive local_arc;

/* Start WriteObject  */
                                                  /* Check Group Flag */
    if (obj_ptr->type == Stgroup && grp_flag == GROUP_START) {
        if (grp_nest <= 0) {
            if ((offset = (StPoint *)malloc(sizeof(StPoint))) == NULL)
                return FAILURE;

            grp_nest = 0;
        }
        else {
            if ((offset = (StPoint*)realloc(offset, sizeof(StPoint)*(grp_nest+1))) == NULL)
                return FAILURE;
        }

        grp_nest++;

        offset[grp_nest-1].x = grp_ptr->body.p_group.obj_pos.x;
        offset[grp_nest-1].y = grp_ptr->body.p_group.obj_pos.y;

        fprintf(fp, "#StFIG_0.1  Group Start\n");
        fprintf(fp, "%d\n", OBJ_START);
        return SUCCESS;
    }


    if (obj_ptr->type == Stgroup && grp_flag == GROUP_END) {

       grp_nest--;

       if (grp_nest <= 0){
           free (offset);
           grp_nest = 0;
       }

       fprintf(fp, "#StFIG_0.1  Group End\n");
       fprintf(fp, "%d\n", OBJ_END);
       return SUCCESS;
    }

                                               /* Write Figure Object */
    grp_pos.x = 0;
    grp_pos.y = 0;
    for (i=0 ; i<grp_nest ; i++) {
        grp_pos.x += offset[i].x;
        grp_pos.y += offset[i].y;
    }
    grp_ptr->body.p_group.obj_pos.x += grp_pos.x;
    grp_ptr->body.p_group.obj_pos.y += grp_pos.y;

    switch ((int)obj_ptr->type) {
        case Strectangle:
            rc = Write_Rectangle(fp, &obj_ptr->body.pd_rect,
                    &grp_ptr->body.p_group.obj_pos);
            break;

        case Stcircle:
        case Stellipse:
            rc = Write_Arc(fp, &obj_ptr->body.pd_arc,
                    &grp_ptr->body.p_group.obj_pos);
            break;

        case Starc:
        case StellipticArc:
        case Stpie:
        case StellipticPie:
	    local_arc = obj_ptr->body.pd_arc;
	    local_g_pos = grp_ptr->body.p_group.obj_pos;

	    AdjustArcInfo(&local_arc, &local_g_pos);
            rc = Write_Arc(fp, &local_arc, &local_g_pos);
            break;

        case Stpolygon:
        case Stpolyline:
        case Stfreehand:
        case Stline:
            rc = Write_Line(fp, &obj_ptr->body.pd_line,
                    &grp_ptr->body.p_group.obj_pos);
            break;

        case Sttext:
            rc = Write_Text(fp, &obj_ptr->body.pd_text,
                    &grp_ptr->body.p_group.obj_pos);
            break;

        case Stgroup: 
            return FAILURE;

        case Stbitmap:
            rc = Write_Bitmap(fp, &obj_ptr->body.pd_bitmap,
                    &grp_ptr->body.p_group.obj_pos);
            break;

        default:
            return FAILURE;     /*  Unknoun Figure Type  */
            break;

    }

    return SUCCESS;
} /* End WriteObject  */


/*
 *                      Save StLinePrimitive
 */


static int 
Write_Line(fp, l, cp)
FILE             *fp;
StLinePrimitive  *l;
StPoint          *cp;
{
        /* Variable  */

    int    sub_type;
    int    style;

    int    forward_arrow;
    int    backward_arrow;

    int    arrow_type      = 0;
    int    arrow_style     = 0;
    int    arrow_thickness = 0;
    int    arrow_width     = 0;
    int    arrow_height    = 0;

    int    i;

        /* Set Sub_Type  */

    if (l->type == Stpolygon)
        sub_type = POLYGON;
    else
        sub_type = POLYLINE;

        /* Set Line_Style  */

    switch ((int)l->line_atr.line_style) {
        case StlineSolid: 
            style = SOLID_LINE;
            break;
        case StlineOnOffDash:
            style = DOTTED_LINE;
            break;
        case StlineDoubleDash:
            style = DASH_LINE;
            break;
        default:
            style = SOLID_LINE;
            break;

    }

        /* Set Arc_Cap */

    if (l->line_atr.begin_cap == StcapArrow)
        forward_arrow = ON;
    else
        forward_arrow = OFF;

    if (l->line_atr.end_cap == StcapArrow)
        backward_arrow = ON;
    else
        backward_arrow = OFF;

        /*                  Save  Polyline_Data                       */
    fprintf(fp, "#StFIG_0.1  Line Primitive\n");

    fprintf(fp, "#!!  figure_type\n");     /*  Sub Information  */
    fprintf(fp, "#!   %d\n", l->type);

    fprintf(fp, "#!!  line_atr\n");        /*  Sub Information  */
    fprintf(fp, "#!   %d %d %d %d %d %d %d %d\n",
            l->line_atr.line_style,
            l->line_atr.line_width,
            l->line_atr.begin_cap,
            l->line_atr.end_cap,
            l->line_atr.join_style,
            l->line_atr.line_foreground,
            l->line_atr.line_background,
            l->line_atr.function);

    fprintf(fp, "#!!  paint_atr\n");       /*  Sub Information  */
    fprintf(fp, "#!   %d %d %d\n",
            l->paint_atr.fill_style,
            l->paint_atr.fill_foreground,
            l->paint_atr.fill_background);

                                                 /*  xfig data        */
    fprintf(fp, "%d %d %d %d %d %d %d %d %.3f %d %d\n",
            O_POLYLINE,
            sub_type,
            style,
            l->line_atr.line_width,
            l->line_atr.line_foreground,
            DEPTH,
            l->line_atr.function,
            l->paint_atr.fill_style,
            STYLE_VAL,
            forward_arrow,
            backward_arrow);

    if (forward_arrow) {
        fprintf(fp, "\t%d %d %.3f %.3f %.3f\n",
                arrow_type,
                arrow_style,
                arrow_thickness,
                arrow_width,
                arrow_height);
    }
    if (backward_arrow) {
        fprintf(fp, "\t%d %d %.3f %.3f %.3f\n",
                arrow_type,
                arrow_style,
                arrow_thickness,
                arrow_width,
                arrow_height);
    }

    fprintf(fp, "\t");

    for (i = 0; i < l->point_list.n_points; i++) {
         fprintf(fp, " %d %d",
                cp->x + l->point_list.points[i].x,
                cp->y + l->point_list.points[i].y);
	 if ((i%5) == 4)
		fprintf(fp, "\n\t");
    };

    fprintf(fp, " 9999 9999\n");

    return SUCCESS;

} /* s_line end  */

/*
 * Save StRectPrimitive
 */


static int  
Write_Rectangle(fp, r, cp)
FILE             *fp;
StRectPrimitive  *r;
StPoint          *cp;
{
    /* Variable  */
    StPoint  p1, p2, p3, p4;
    int  sub_type;
    int  style;

    int  forward_arrow;
    int  backward_arrow;

    int  arrow_type      = 0;
    int  arrow_style     = 0;
    int  arrow_thickness = 0;
    int  arrow_width     = 0;
    int  arrow_height    = 0;

    int  i;

    /* Set Sub_Type  */
    sub_type = BOX;

    /* Set Line_Style  */
    switch ((int)r->line_atr.line_style) {
        case StlineSolid:
            style = SOLID_LINE;
            break;
        case StlineOnOffDash:
            style = DOTTED_LINE;
            break;
        case StlineDoubleDash:
            style = DASH_LINE;
            break;
        default:
            style = SOLID_LINE;
            break;
    }

    /* Set Arc_Cap  */
    if (r->line_atr.begin_cap == StcapArrow)
        forward_arrow = ON;
    else
        forward_arrow = OFF;

    if (r->line_atr.end_cap == StcapArrow)
        backward_arrow = ON;
    else
        backward_arrow = OFF;

    /* Save  Polyline_Data */

    fprintf(fp, "#StFIG_0.1  Rect Primitive\n");

    fprintf(fp, "#!!  figure_type \n");    /*  Sub Information  */
    fprintf(fp, "#!   %d \n", r->type);

    fprintf(fp, "#!!  line_atr \n");       /*  Sub Information  */
    fprintf(fp, "#!   %d %d %d %d %d %d %d %d \n",
            r->line_atr.line_style,
            r->line_atr.line_width,
            r->line_atr.begin_cap,
            r->line_atr.end_cap,
            r->line_atr.join_style,
            r->line_atr.line_foreground,
            r->line_atr.line_background,
            r->line_atr.function);

    fprintf(fp, "#!!  paint_atr \n");      /*  Sub Information  */
    fprintf(fp, "#!   %d %d %d \n",
            r->paint_atr.fill_style,
            r->paint_atr.fill_foreground,
            r->paint_atr.fill_background);

                                                 /*  xfig data        */
    fprintf(fp, "%d %d %d %d %d %d %d %d %.3f %d %d\n",
            O_POLYLINE,
            sub_type,
            style,
            r->line_atr.line_width,
            r->line_atr.line_foreground,
            DEPTH,
            r->line_atr.function,
            r->paint_atr.fill_style,
            STYLE_VAL,
            forward_arrow,
            backward_arrow);

    if (forward_arrow) {
        fprintf(fp, "\t%d %d %.3f %.3f %.3f\n",
            arrow_type,
            arrow_style,
            arrow_thickness,
            arrow_width,
            arrow_height);
    }
    if (backward_arrow) {
        fprintf(fp, "\t%d %d %.3f %.3f %.3f\n",
                arrow_type,
                arrow_style,
                arrow_thickness,
                arrow_width,
                arrow_height);
    }

    fprintf(fp, "\t");

/* Rect Points */
    p1.x = cp->x + r->rectangle.x;
    p1.y = cp->y + r->rectangle.y;

    p2.x = cp->x + r->rectangle.x + r->rectangle.width;
    p2.y = cp->y + r->rectangle.y;

    p3.x = cp->x + r->rectangle.x + r->rectangle.width;
    p3.y = cp->y + r->rectangle.y - r->rectangle.height;

    p4.x = cp->x + r->rectangle.x;
    p4.y = cp->y + r->rectangle.y - r->rectangle.height;

    fprintf(fp, " %d %d %d %d %d %d %d %d %d %d",
            p1.x, p1.y,
            p2.x, p2.y,
            p3.x, p3.y,
            p4.x, p4.y,
            p1.x, p1.y);

    fprintf(fp, " 9999 9999\n");

    return SUCCESS;

} /* Write_Line end */

/*
/* Save StArcPrimitive
 */



static int  
Write_Arc(fp, a, cp)
FILE             *fp;
StArcPrimitive   *a;
StPoint          *cp;
{
    int      forward_arrow;
    int      backward_arrow;
    int      style;
    StPoint  point[3];
    double   center_x, center_y;

    int  sub_type        = 1;
    int  arrow_type      = 0;
    int  arrow_style     = 0;
    int  arrow_thickness = 0;
    int  arrow_width     = 0;
    int  arrow_height    = 0;

    /* Set Line_Style */

    switch ((int)a->line_atr.line_style) {
        case StlineSolid:
            style = SOLID_LINE;
            break;
        case StlineOnOffDash:
            style = DOTTED_LINE;
            break;
        case StlineDoubleDash:
            style = DASH_LINE;
            break;
        default:
            style = SOLID_LINE;
            break;
    }

    /* Convert Arrow_Cap Style  */

    if (a->line_atr.begin_cap == StcapArrow)
        forward_arrow = ON;
    else
        forward_arrow = OFF;

    if (a->line_atr.end_cap == StcapArrow)
        backward_arrow = ON;
    else
        backward_arrow = OFF;

    /* Make Center Position  */
    center_x = (double)((double)cp->x + (double)a->arc.x +
		(double)((double)a->arc.width / (double)2) );

    center_y = (double)((double)cp->y + (double)a->arc.y +
		(double)((double)a->arc.height / (double)2) );

    /* Make Arc Points  */

    if (a->type == Starc || a->type == StellipticArc ||
            a->type == Stpie || a->type == StellipticPie) {
        point[0].x = cp->x;
        point[0].y = cp->y;

        point[1].x = cp->x;
        point[1].y = cp->y;

        point[2].x = cp->x;
        point[2].y = cp->y;
    }
    else if (a->type == Stcircle || a->type == Stellipse) {
        point[0].x = cp->x;
        point[0].y = cp->y;

        point[1].x = cp->x;
        point[1].y = cp->y;

        point[2].x = point[0].x;
        point[2].y = point[0].y;
    }

    fprintf(fp, "#StFIG_0.1  Arc Primitive\n");

    fprintf(fp, "#!!  figure_type\n");     /*  Sub Information  */
    fprintf(fp, "#!   %d\n", a->type);

    fprintf(fp, "#!!  line_atr\n");        /*  Sub Information  */
    fprintf(fp, "#!   %d %d %d %d %d %d %d %d\n",
            a->line_atr.line_style,
            a->line_atr.line_width,
            a->line_atr.begin_cap,
            a->line_atr.end_cap,
            a->line_atr.join_style,
            a->line_atr.line_foreground,
            a->line_atr.line_background,
            a->line_atr.function);

    fprintf(fp, "#!!  paint_atr\n");       /*  Sub Information  */
    fprintf(fp, "#!   %d %d %d\n",
            a->paint_atr.fill_style,
            a->paint_atr.fill_foreground,
            a->paint_atr.fill_background);

    fprintf(fp, "#!!  Arc Data\n");        /*  Sub Information  */
    fprintf(fp, "#!   %d %d %d %d %d %d %d\n",
            a->arc.direction,
            (a->arc.x + cp->x),
            (a->arc.y + cp->y),
            a->arc.width,
            a->arc.height,
            a->arc.angle1,
            a->arc.angle2);

                                                 /*  xfig data        */
    fprintf(fp,
            "%d %d %d %d %d %d %d %d %.3f ",
            O_ARC,
            sub_type,
            style,
            a->line_atr.line_width,
            a->line_atr.line_foreground,
            DEPTH,
            a->line_atr.function,
            a->paint_atr.fill_style,
            STYLE_VAL);

    fprintf(fp,
            "%d %d %d %.3f  %.3f  %d %d %d %d   %d %d\n",
            a->arc.direction,
            forward_arrow,
            backward_arrow,
            center_x, center_y, 
            point[0].x, point[0].y, 
            point[1].x, point[1].y, 
            point[2].x, point[2].y);

    if (forward_arrow) {
        fprintf(fp, "\t%d %d %.3f %.3f %.3f\n",
                arrow_type,
                arrow_style,
                arrow_thickness,
                arrow_width,
                arrow_height);
    }
    if (backward_arrow) {
        fprintf(fp, "\t%d %d %.3f %.3f %.3f\n",
                arrow_type,
                arrow_style,
                arrow_thickness,
                arrow_width,
                arrow_height);
    }

    return SUCCESS;

}/* s_arc end */

/*
/* Save  StTextPrimitive
 */

static int 
Write_Text(fp, t, cp)
FILE             *fp;
StTextPrimitive  *t;
StPoint          *cp;
{
    int   sub_type = 0;
    int   height   = 0;
    register i;

    fprintf(fp, "#StFIG_0.1  Text Primitive\n");

    fprintf(fp, "#!!  figure_type\n");     /*  Sub Information  */
    fprintf(fp, "#!   %d\n", t->type);

/* convert newline to bell *
    for(i = 0; i < t->text.n_bytes; ++i)
	if (t->text.string[i] == '\n')
		t->text.string[i] = '\007';
*/
                                                 /*  xfig data        */
    t->text.string[t->text.n_bytes] = '\0';

    fprintf(fp, "%d %d %d %d %d %d %d %.3f %d %d %d %d %d %s\n", 
            O_TEXT,
            sub_type,
            t->text_atr.font,                  /* Font Name */
            t->text_atr.font,                  /* Font Size */
            t->text_atr.function,
            t->text_atr.text_color,
            DEPTH,
            ANGLE,
            t->text_atr.font,                  /* Font Style */
            height,
            t->text.n_bytes, 
            (t->text.x + cp->x),
            (t->text.y + cp->y),
            t->text.string);

    return SUCCESS;

}/* s_text end  */

/*
 * Save StBitmapPrimitive
 */
static int 
Write_Bitmap(fp, b, cp)
FILE               *fp;
StBitmapPrimitive  *b;
StPoint            *cp;
{
    fprintf(fp, "#StFIG_0.1  Bitmap Primitive\n");

    fprintf(fp, "#!!  figure_type\n");     /*  Sub Information  */
    fprintf(fp, "#!   %d\n", b->type );

    fprintf(fp, "#!!  paint_atr\n");       /*  Sub Information  */
    fprintf(fp, "#!   %d %d %d\n",
            b->paint_atr.fill_style,
            b->paint_atr.fill_foreground,
            b->paint_atr.fill_background);

    b->bitmap.n_bytes = strlen(b->bitmap.bitmap_file);

    fprintf(fp, "#!!  bitmap\n");         /*  Sub Information  */
    fprintf(fp, "#!   %d %d %d %s\n",
            b->bitmap.n_bytes, 
            (b->bitmap.x + cp->x),
            (b->bitmap.y + cp->y),
            b->bitmap.bitmap_file);


    return SUCCESS;


}/* s_bitmap end */

#define Dgree0		0
#define Dgree90		5760		/* 90 * 64 */
#define Dgree180	11520		/* 180 * 64 */
#define Dgree270	17280		/* 270 * 64 */

static void
AdjustArcInfo(arc_prm, pos)
StArcPrimitive   *arc_prm;
StPoint          *pos;
{
	unsigned short	width = arc_prm->arc.width,
			height = arc_prm->arc.height;

	arc_prm->arc.width *= 2;
	arc_prm->arc.height *= 2;
	switch (arc_prm->arc.angle1) {
		case Dgree0:		/* 0 */
			pos->x -= width;
			return;
		case Dgree90:		/* 90 * 64 */
			return;
		case Dgree180:		/* 180 * 64 */
			pos->y -= height;
			return;
		case Dgree270:		/* 270 * 64 */
			pos->x -= width;
			pos->y -= height;
			return;
	}
}

static int
_ReadHeader(fp)
FILE  *fp;
{
    int	reso, coord_sys, len;
 
    if (feof(fp))
        return FAILURE;

/* read a comment line containing version number for `xfig' */
    if ( (fgets(buf, BUF_SIZE, fp) == NULL) 
	|| strncmp(buf, XFIG_NAME_AND_VERSION, strlen(XFIG_NAME_AND_VERSION)) )
        return FAILURE;

/* read figure resolution and coordinate system constant using `xfig' */
    if ( (fgets(buf, BUF_SIZE, fp) == NULL) )
        return FAILURE;
    sscanf(buf, "%d %d", &reso, &coord_sys);
    if (reso != XFIG_RESOLUTION_80 || coord_sys != XFIG_COORD_SYSTEM_UPPER_LRFT)
        return FAILURE;

/* read a comment line containing version number for `StPicture' */
    if ( (fgets(buf, BUF_SIZE, fp) == NULL) 
	|| strncmp(buf, STPIC_NAME_AND_VERSION, strlen(STPIC_NAME_AND_VERSION)) )
        return FAILURE;

    return SUCCESS;
}

static void
_WriteHeader(fp)
FILE  *fp;
{
/* write a comment line containing version number for `xfig' */
    fprintf(fp, "%s\n", XFIG_NAME_AND_VERSION);

/* write figure resolution and coordinate system constant using `xfig' */
    fprintf(fp, "%d %d\n", XFIG_RESOLUTION_80, XFIG_COORD_SYSTEM_UPPER_LRFT);

/* write a comment line containing version number for `StPicture' */
    fprintf(fp, "%s\n", STPIC_NAME_AND_VERSION);
}
