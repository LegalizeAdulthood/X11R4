/*
** -----------------------------------------------------------------------
** Name:	xv_panel_color_buttons.c
**
** Category:	XView
**
** Subcategory:	Panel
**
** Description:
**
** 	This program demonstrates how to use PANEL_ITEM_COLOR
** 	to specify a color for a panel item - specifically buttons.
**
** BUG ALERT: 	Does not work in 1.0beta2 - bug filed.
**
** -----------------------------------------------------------------------
**/

#include <stdio.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/cms.h>

/*
** -----------------------------------------------------------------------
** main()
**
**	Initialize RGB color arrays.
**
**	Create a frame.
**
**	Intialize the CMS data for a white/red/green/blue colormap.
**
**	Create a panel - setting the colormap.
**
**	Create several color panel buttons.
**
** -----------------------------------------------------------------------
*/

Frame	baseframe;
Panel	panel;
Xv_cmsdata cmsdata;

#define NUM_COLORS 4

static 	u_char	red[]   = {0,255,  0,  0};
static 	u_char	green[] = {0,  0,255,  0};
static 	u_char	blue[]  = {0,  0,  0,255};

#define WHITE	0
#define RED	1
#define GREEN	2
#define BLUE	3

main(argc,argv)
int	argc;
char	*argv[];
{
	/* 
	**	Create the frame
	*/
	xv_init(XV_INIT_ARGS,	argc,argv, 
		0);
	baseframe = xv_create(0,FRAME,
		FRAME_LABEL,	"xv_panel_color_buttons",
		0);

	/*
	**	Initialize the CMS
	*/
	cmsdata.type = XV_STATIC_CMS;
	cmsdata.size = NUM_COLORS;
	cmsdata.index = 0;
	cmsdata.rgb_count = NUM_COLORS;
	cmsdata.red = red;
	cmsdata.green = green;
	cmsdata.blue = blue;

	/*
	**	Create the panel using the CMS
	*/
	panel = xv_create(baseframe,PANEL,
		CANVAS_PAINTWINDOW_ATTRS,
			WIN_CMS_NAME, "ColorPanel",
			WIN_CMS_DATA, &cmsdata,
			0,
		0);

	/*
	**	Create the colored panel buttons
	*/
	(void)xv_create(panel,PANEL_BUTTON,
		PANEL_LABEL_STRING,	"Red",
		PANEL_ITEM_COLOR,	RED,
		0);
	(void)xv_create(panel,PANEL_BUTTON,
		PANEL_LABEL_STRING,	"Green",
		PANEL_ITEM_COLOR,	GREEN,
		0);
	(void)xv_create(panel,PANEL_BUTTON,
		PANEL_LABEL_STRING,	"Blue",
		PANEL_ITEM_COLOR,	BLUE,
		0);

	/*
	**	Start the event loop
	*/
	window_fit(panel);
	window_fit(baseframe);
	xv_main_loop(baseframe);
	return(0);
}
