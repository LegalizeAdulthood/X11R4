/* 
 * @(#) olgx.h 1.17 89/11/21 
 */

/*
 * OPEN LOOK object drawing package
 * Sun Microsystems, Inc.,
 */

#ifndef OL_PUBLIC_DEFINED
#define OL_PUBLIC_DEFINED

#include <X11/Xlib.h>

/*
 * Package constant definitions
 */

/* 	GC number definitions 	*/

#define NONE			-1	/* means "don't draw in this color" */
#define OLGX_WHITE		0
#define OLGX_BLACK		1
#define OLGX_BG1			2
#define OLGX_BG2			3
#define OLGX_BG3			4
#define OLGX_NUM_COLORS		5	/* 1 more than last *color* GC */

#define OLGX_TEXTGC			5
#define OLGX_GREY_OUT		6
#define OLGX_BUSYGC			7
#define OLGX_NUM_GCS			8	/* always 1 more than last GC */

/* 2D gc definitions */

#define OLGX_TEXTGC_REV              2

/*
 * State Flag Definitions
 */
#define OLGX_NORMAL		0x0000	/* normal button */
#define OLGX_INVOKED		0x0001	/* invoked button */
#define OLGX_MENU_ITEM		0x0002	/* menu item */
#define	OLGX_ERASE		0x0004	/* erase first (only for menu items) */
#define OLGX_BUSY		0x0008	/* busy item */
#define OLGX_DEFAULT		0x0010	/* default item */
#define OLGX_INACTIVE		0x0020	/* inactive item */
#define OLGX_VERT_MENU_MARK	0x0040	/* include a vertical menu mark */
#define OLGX_HORIZ_MENU_MARK	0x0080	/* include a horizontal menu mark */
#define OLGX_VERT_BACK_MENU_MARK 0x2000	/* include a vertical menu mark */
#define OLGX_HORIZ_BACK_MENU_MARK 0x4000/* include a horizontal menu mark */
#define OLGX_MENU_MARK		0x00c0	/* VERT_MENU_MARK | HORIZ_MENU_MARK */
#define OLGX_LABEL_IS_PIXMAP	0x0200	/* button label is a pixmap */
#define OLGX_VERTICAL 		0x0800	/* orientation is vertical */
#define OLGX_HORIZONTAL 	0x1000	/* orientation is horizontal */
#define OLGX_PUSHPIN_OUT	0x2000	/* pushpin is in */
#define OLGX_PUSHPIN_IN		0x4000	/* pushpin is out */
#define OLGX_UPDATE		0x8000	/* object is to be updated */
#define OLGX_ABBREV		0x0100	/* object is abbreviated */
#define OLGX_CHECKED		0x0002	/* object is checked (check boxes) */
 /* scrollbar -2d stateflag defns */

#define OLGX_SCROLL_FORWARD		0x0002	/*scroll forward */
#define OLGX_SCROLL_BACKWARD		0x2000	/*scroll backward */
#define OLGX_SCROLL_ABSOLUTE		0x4000	/*scroll absolute-
						  center darkened  */
#define OLGX_SCROLL_NO_FORWARD          0x0040  /* no scroll forward */
#define OLGX_SCROLL_NO_BACKWARD         0x0080  /* no scroll backward */
/*
 * corner definitions
 */
#define OLGX_UPPER_LEFT		0
#define OLGX_UPPER_RIGHT	1
#define OLGX_LOWER_RIGHT	2
#define OLGX_LOWER_LEFT		3

/*
 * character definitions
 */
#define OLG_VSB_ELEVATOR        1
#define OLG_VSB_ELEVATOR_LINE_BACKWARD  2
#define OLG_VSB_ELEVATOR_ABSOLUTE               3
#define OLG_VSB_ELEVATOR_LINE_FORWARD   4
#define OLG_VSB_REDUCED_ELEVATOR        5
#define OLG_VSB_REDUCED_ELEVATOR_LINE_BACKWARD  6
#define OLG_VSB_REDUCED_ELEVATOR_LINE_FORWARD   7
#define OLG_VSB_ANCHOR  8
#define OLG_VSB_ANCHOR_INVERTED 9
#define OLG_HSB_ELEVATOR        10
#define OLG_HSB_ELEVATOR_LINE_BACKWARD  11
#define OLG_HSB_ELEVATOR_ABSOLUTE               12
#define OLG_HSB_ELEVATOR_LINE_FORWARD   13
#define OLG_HSB_REDUCED_ELEVATOR        14
#define OLG_HSB_REDUCED_ELEVATOR_LINE_BACKWARD  15
#define OLG_HSB_REDUCED_ELEVATOR_LINE_FORWARD   16
#define OLG_HSB_ANCHOR  17
#define OLG_HSB_ANCHOR_INVERTED 18
#define OLG_MENU_PIN_OUT        19
#define OLG_MENU_PIN_IN         20
#define OLG_MENU_DEFAULT_PIN_OUT        21
#define OLG_ABBREV_MENU_BUTTON  22
#define OLG_ABBREV_MENU_BUTTON_INVERTED 23
/* new extension */
#define BUTTON_UL				24
#define BUTTON_LL				25
#define BUTTON_LEFT_ENDCAP_FILL			26
#define BUTTON_LR				27
#define BUTTON_UR				28
#define BUTTON_RIGHT_ENDCAP_FILL		29
#define BUTTON_TOP_1				30
#define BUTTON_TOP_2				31
#define BUTTON_TOP_4				32
#define BUTTON_TOP_8				33
#define BUTTON_TOP_16				34
#define BUTTON_BOTTOM_1				35
#define BUTTON_BOTTOM_2				36
#define BUTTON_BOTTOM_4				37
#define BUTTON_BOTTOM_8				38
#define BUTTON_BOTTOM_16			39
#define BUTTON_FILL_1				40
#define BUTTON_FILL_2				41
#define BUTTON_FILL_4				42
#define BUTTON_FILL_8				43
#define BUTTON_FILL_16				44
#define VERT_MENU_MARK_UL			45
#define VERT_MENU_MARK_LR			46
#define VERT_MENU_MARK_FILL			47
#define HORIZ_MENU_MARK_UL			48
#define HORIZ_MENU_MARK_LR			49
#define HORIZ_MENU_MARK_FILL			50
#define ABBREV_MENU_UL				51
#define ABBREV_MENU_LR				52
#define ABBREV_MENU_FILL			53
#define VERT_SB_UL				54
#define VERT_SB_LR				55
#define VERT_SB_TOPBOX_FILL			56
#define HORIZ_SB_UL				57
#define HORIZ_SB_LR				58
#define VERT_SB_BOTBOX_FILL			59
#define HORIZ_SLIDER_CONTROL_UL			60
#define HORIZ_SLIDER_CONTROL_LR			61
#define HORIZ_SLIDER_CONTROL_FILL		62
#define HORIZ_SLIDER_UL				63
#define HORIZ_SLIDER_LL				64
#define HORIZ_SLIDER_UR				65
#define HORIZ_SLIDER_LR				66
#define HORIZ_SLIDER_BOTTOM_1			67
#define HORIZ_SLIDER_BOTTOM_2			68
#define HORIZ_SLIDER_BOTTOM_4			69
#define HORIZ_SLIDER_BOTTOM_8			70
#define HORIZ_SLIDER_BOTTOM_16			71
#define HORIZ_SLIDER_FILL_1			72
#define HORIZ_SLIDER_FILL_2			73
#define HORIZ_SLIDER_FILL_4			74
#define HORIZ_SLIDER_FILL_8			75
#define HORIZ_SLIDER_FILL_16			76
#define HORIZ_SLIDER_LEFT_ENDCAP_FILL		77
#define HORIZ_SLIDER_RIGHT_ENDCAP_FILL		78
#define VERT_SLIDER_UL				79
#define VERT_SLIDER_UR				80
#define VERT_SLIDER_TOP_ENDCAP_FILL		81
#define VERT_SLIDER_LL				82
#define VERT_SLIDER_LR				83
#define VERT_SLIDER_BOTTOM_ENDCAP_FILL		84
#define VERT_SLIDER_CONTROL_UL			85
#define VERT_SLIDER_CONTROL_LR			86
#define VERT_SLIDER_CONTROL_FILL		87
#define UL_RESIZE_UL				88
#define UL_RESIZE_LR				89
#define UL_RESIZE_FILL				90
#define UR_RESIZE_UL				91
#define UR_RESIZE_LR				92
#define UR_RESIZE_FILL				93
#define LR_RESIZE_UL				94
#define LR_RESIZE_LR				95
#define LR_RESIZE_FILL				96
#define LL_RESIZE_UL				97
#define LL_RESIZE_LR				98
#define LL_RESIZE_FILL				99
#define PUSHPIN_OUT_TOP				100
#define PUSHPIN_OUT_BOTTOM			101
#define PUSHPIN_OUT_MIDDLE			102
#define PUSHPIN_IN_TOP				103
#define PUSHPIN_IN_BOTTOM			104
#define PUSHPIN_IN_MIDDLE			105
#define DFLT_BUTTON_LEFT_ENDCAP			106
#define DFLT_BUTTON_RIGHT_ENDCAP		107
#define DFLT_BUTTON_MIDDLE_1			108
#define DFLT_BUTTON_MIDDLE_2			109
#define DFLT_BUTTON_MIDDLE_4			110
#define DFLT_BUTTON_MIDDLE_8			111
#define DFLT_BUTTON_MIDDLE_16			112
#define BASE_OFF_SPECIALCHAR			113 /*special char */
#define UNCHECKED_BOX_UL			114
#define UNCHECKED_BOX_LR			115
#define UNCHECKED_BOX_FILL			116
#define CHECK_MARK				117
#define CHECKED_BOX_FILL			118
#define UNCHECKED_BOX_OUTLINE			119
#define HORIZ_GAUGE_UL				120
#define HORIZ_GAUGE_LL				121
#define HORIZ_GAUGE_UR				122
#define HORIZ_GAUGE_LR				123
#define HORIZ_GAUGE_BOTTOM_1			124
#define HORIZ_GAUGE_BOTTOM_2			125
#define HORIZ_GAUGE_BOTTOM_4			126
#define HORIZ_GAUGE_BOTTOM_8			127
#define HORIZ_GAUGE_BOTTOM_16			128
#define VERT_GAUGE_UL				129
#define VERT_GAUGE_UR				130
#define VERT_GAUGE_LL				131
#define VERT_GAUGE_LR				132
#define VERT_ABBREV_SB_UL			133
#define VERT_ABBREV_SB_LR			134
#define HORIZ_SB_RIGHTBOX_FILL			135
#define HORIZ_ABBREV_SB_UL			136
#define HORIZ_ABBREV_SB_LR			137
#define HORIZ_SB_LEFTBOX_FILL			138
#define BUTTON_OUTLINE_LEFT_ENDCAP		139
#define BUTTON_OUTLINE_RIGHT_ENDCAP		140
#define BUTTON_OUTLINE_MIDDLE_1			141
#define BUTTON_OUTLINE_MIDDLE_2			142
#define BUTTON_OUTLINE_MIDDLE_4			143
#define BUTTON_OUTLINE_MIDDLE_8			144
#define BUTTON_OUTLINE_MIDDLE_16		145
#define BUTTON_FILL_2D_LEFTENDCAP		146
#define BUTTON_FILL_2D_RIGHTENDCAP      	147
#define BUTTON_FILL_2D_MIDDLE_1     		148
#define BUTTON_FILL_2D_MIDDLE_2 		149
#define BUTTON_FILL_2D_MIDDLE_4 		150
#define BUTTON_FILL_2D_MIDDLE_8 		151
#define BUTTON_FILL_2D_MIDDLE_16		152
#define MENU_DFLT_OUTLINE_LEFT_ENDCAP           153
#define MENU_DFLT_OUTLINE_RIGHT_ENDCAP          154
#define MENU_DFLT_OUTLINE_MIDDLE_1              155
#define MENU_DFLT_OUTLINE_MIDDLE_2              156
#define MENU_DFLT_OUTLINE_MIDDLE_4              157
#define MENU_DFLT_OUTLINE_MIDDLE_8              158
#define MENU_DFLT_OUTLINE_MIDDLE_16             159
#define HORIZ_MENU_MARK_OUTLINE			160 /* available slot */
#define VERT_MENU_MARK_OUTLINE			161 /* available slot */
#define UL_RESIZE_OUTLINE			162
#define UR_RESIZE_OUTLINE			163
#define LR_RESIZE_OUTLINE			164
#define LL_RESIZE_OUTLINE			165
#define VERT_SB_NO_BACK_OUTLINE                 166
#define VERT_SB_NO_FWD_OUTLINE                  167
#define VERT_SB_INACTIVE_OUTLINE                168
#define HORIZ_SB_NO_BACK_OUTLINE                169
#define HORIZ_SB_NO_FWD_OUTLINE                 170
#define HORIZ_SB_INACTIVE_OUTLINE               171
#define HORIZ_SLIDER_CONTROL_OUTLINE		172
#define HORIZ_SLIDER_LEFT_ENDCAP_OUTLINE	173
#define	HORIZ_SLIDER_RIGHT_ENDCAP_OUTLINE	174
#define HORIZ_SLIDER_OUTLINE_1			175
#define HORIZ_SLIDER_OUTLINE_2			176
#define HORIZ_SLIDER_OUTLINE_4			177
#define HORIZ_SLIDER_OUTLINE_8			178
#define HORIZ_SLIDER_OUTLINE_16			179
#define VERT_SLIDER_TOP_ENDCAP_OUTLINE		180
#define VERT_SLIDER_BOTTOM_ENDCAP_OUTLINE	181
#define VERT_SLIDER_CONTROL_OUTLINE		182
#define PUSHPIN_OUT_DEFAULT_TOP 		183
#define PUSHPIN_OUT_DEFAULT_BOTTOM 		184
#define PUSHPIN_OUT_DEFAULT_MIDDLE 		185
#define HORIZ_GAUGE_LEFT_ENDCAP_OUTLINE		186
#define HORIZ_GAUGE_RIGHT_ENDCAP_OUTLINE	187
#define HORIZ_GAUGE_OUTLINE_MIDDLE_1		188
#define HORIZ_GAUGE_OUTLINE_MIDDLE_2		189
#define HORIZ_GAUGE_OUTLINE_MIDDLE_4		190
#define HORIZ_GAUGE_OUTLINE_MIDDLE_8		191
#define HORIZ_GAUGE_OUTLINE_MIDDLE_16		192
#define CHECK_BOX_CLEAR_FILL			193
#define VERT_SB_BOX_UL 				194
#define VERT_SB_BOX_LR 				195
#define DIMPLE_UL				196
#define DIMPLE_LR				197
#define DIMPLE_FILL				198
#define SLIDER_CHANNEL_OFFSET			199 /* special char */
#define HORIZ_SB_BOX_UL				200
#define HORIZ_SB_BOX_LR				201
#define VERT_BACK_MENU_MARK_UL			202
#define VERT_BACK_MENU_MARK_LR			203
#define VERT_BACK_MENU_MARK_FILL		204
#define HORIZ_BACK_MENU_MARK_UL			205
#define HORIZ_BACK_MENU_MARK_LR			206
#define HORIZ_BACK_MENU_MARK_FILL		207

/*
 * Type and Structure Definitions
 */

typedef unsigned char Boolean;

/*
 * Graphics information structure
 */
typedef struct graphics_info {
	Display *dpy;
	int scrn;
	unsigned int depth;		/* depth in which we are drawing */
	XFontStruct *glyphfont;		/* ptr to glyph font info*/
	XFontStruct *textfont;		/* ptr to text font info */
	Boolean	three_d;		/* true if drawing in 3D */
	GC 	gc[OLGX_NUM_GCS];
	unsigned long pixvals[OLGX_NUM_COLORS];
	Pixmap	busy_stipple;		/* stipple for busy pattern */
	Pixmap	grey_stipple;		/* stipple for inactive pattern */

	/*
	 * important OPEN LOOK values associated with this glyph font
	 */
	short	button_height;			/* height of buttons */
	short	endcap_width;			/* size of button endcap */
	short	sb_width, sb_height;		/* scrollbar elevator size */
	short	abbrev_width, abbrev_height;	/* abbrev menu button size */
	short	slider_width, slider_height;	/* slider control size */
	short	se_width, se_height;		/* slider endcap size */
	short	mm_width, mm_height;		/* menu mark size */
	short   base_off           ;            /* Text base_off */
	short   slider_offset           ;       /*SliderChanneloffset */
	short	cb_width, cb_height;		/* check box size */
	short	pp_width, pp_height;		/* push pin  size*/
} Graphics_info;

/* Public macro definitions to get info from the Ginfo struct  */

#define ScrollbarElevator_Height(info)  ((info)->sb_height)
#define ScrollbarElevator_Width(info)   ((info)->sb_width)
#define HorizSliderControl_Width(info)  ((info)->slider_width)
#define HorizSliderControl_Height(info) ((info)->slider_height)
#define SliderEndCap_Width(info)   	((info)->se_width)
#define SliderEndCap_Height(info)   	((info)->se_height)
#define CheckBox_Height(info)   	((info)->cb_height)
#define CheckBox_Width(info)    	((info)->cb_width)
#define PushPinOut_Width(info)    	((info)->pp_width)
#define PushPinOut_Height(info)    	((info)->pp_height)
#define Button_Height(info) 	   	((info)->button_height)
#define ButtonEndcap_Width(info) 	((info)->endcap_width)
#define MenuMark_Width(info) 		((info)->mm_width)
#define MenuMark_Height(info) 		((info)->mm_height)
#define Abbrev_MenuButton_Height(info) 	((info)->abbrev_height)
#define Abbrev_MenuButton_Width(info) 	((info)->abbrev_width)


/*
 * Public function declarations
 */

Graphics_info	*olgx_initialize();

void		olgx_set_text_font(),
		olgx_set_glyph_font(),
		olgx_set_color(),
		olgx_error(),
		olgx_create_GCs(),
		olgx_draw_button(),
		olgx_draw_menu_mark(),
		olgx_draw_abbrev_button(),
		olgx_stipple_rect(),
		olgx_draw_horizontal_slider(),
		olgx_draw_vertical_slider(),
		olgx_update_horizontal_slider(),
		olgx_update_vertical_slider(),
		olgx_draw_resize_corner(),
		olgx_draw_pushpin(),
		olgx_draw_box(),
		olgx_draw_chiseled_box(),
		olgx_draw_text(),
		olgx_destroy();

#endif	!OL_PUBLIC_DEFINED
