/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *      November 1985.
 *
 *	%W%	%G%
*/

/******************  Functions  ******************/
#define			F_NULL		 	0
#define			F_ELLIPSE_BY_RAD 	1
#define			F_ELLIPSE_BY_DIA 	2
#define			F_CIRCLE_BY_RAD 	3
#define			F_CIRCLE_BY_DIA 	4
#define			F_CIRCULAR_ARC		5
#define			F_POLYLINE		6
#define			F_BOX			7
#define			F_POLYGON		8
#define			F_TEXT			9
#define			F_SPLINE		10
#define			F_CLOSED_SPLINE		11
#define			F_COMPOUND		13
#define			F_INTSPLINE		14
#define			F_CLOSED_INTSPLINE	15

#define			F_CREATE		30
#define			F_COPY			31
#define			F_MOVE			32
#define			F_REPEAT		33
#define			F_REMOVE		34
#define			F_MOVE_POINT		35
#define			F_DELETE_POINT		36
#define			F_ADD_POINT		37
#define			F_DELETE_ARROW_HEAD	38
#define			F_ADD_ARROW_HEAD	39
#define			F_FLIP_XAXIS		40
#define			F_FLIP_YAXIS		41
#define			F_ROTATE90		42
#define			F_ROTATE270		43
#define			F_ROTATE		45
#define			F_FLIP			46
#define			F_AUTOARROW		47
#define			F_TURN			48
#define			F_GLUE			51
#define			F_BREAK			52
#define			F_SCALE			53

#define			F_SET_PEN_SIZE		70   /*  set pen size    */
#define			F_SET_LINE_STYLE	71   /*  set line style  */
#define			F_SET_DASH_LINE		72
#define			F_SET_DOTTED_LINE	73
#define			F_SET_SOLID_LINE	74
	
#define			F_READ			101
#define			F_EDIT			103
#define			F_SAVE_IN		104
#define			F_SAVE			105
#define			F_UNDO			107
#define			F_REDISPLAY		109
#define			F_QUIT			111
#define			F_REMOVE_ALL		113
#define			F_SAVE_N_EXIT		115
#define			F_CHDIR			117
#define			F_PWDIR			118
#define			F_SAVE_BITMAP		121
#define			F_STATUS		123
	
/****************  Drawing aids *****************/
#define			F_GRID1			250
#define			F_GRID2			251
#define			F_RULER			252
#define			F_UNCONSTRAINT		260  /*fs*/
#define			F_MOUNTHATTAN		261  /*fs*/
#define			F_MANHATTAN		253
#define			F_MOUNTAIN		254
#define			F_LATEX_LINE		262  /*fs*/
#define			F_LATEX_ARROW		263  /*fs*/
#define			F_AUTOF_ARROW		255
#define			F_AUTOB_ARROW		256
#define			F_MAGNET		257
