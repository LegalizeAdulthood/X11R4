/*
 * Name:	xv_canvas_pw_draw.c
 *
 * Description:
 *	Demonstrates some of the pixwin drawing functions on
 *	a canvas paint window. The drawing is done inside the 
 *	repaint procedure. All XView (and X11) applications 
 *	must always be able to repaint themselves on demand.
 *      Note that the current damage list is passed as an 
 *	argument to the repaint proc. The application may chose 
 *	to use this rectangle list to do "smart repainting" by 
 *	painting only those objects that lie within the damaged
 *	region. In any case, all graphics output done using the 
 *	pixwin routines will be clipped using the damage rectangles.
 *	
 */

#include <stdio.h>
#include <xview/xview.h>
#include <xview/canvas.h>
#include <xview/svrimage.h>

Server_image gray_image;
static short gray_image_bits[] = {0xAAAA, 0x5555};

/*
 *		main()
 *	Create frame and canvas windows.  
 *	Set a repaint proc for the canvas window.  
 *	Create a server image (used to fill in canvas_repaint_proc()).
 *	Start the event loop.
 */
main(argc,argv)
    int		argc;
    char	*argv[];
{
	Frame	frame;
	Canvas	canvas;
	void	canvas_repaint_proc();

	xv_init(XV_INIT_ARGS, argc,argv, 0);

	frame = xv_create(NULL,FRAME,
			  FRAME_LABEL,	"xv_canvas_pw_draw",
			  XV_WIDTH, 	400,
			  XV_HEIGHT, 	300,
			  0);

	canvas = xv_create(frame,CANVAS,
			   CANVAS_REPAINT_PROC,	canvas_repaint_proc,
			   0);

	gray_image = xv_create(NULL,SERVER_IMAGE,
			       XV_WIDTH,		16,
			       XV_HEIGHT,		2,
			       SERVER_IMAGE_DEPTH,	1,
			       SERVER_IMAGE_BITS,	gray_image_bits,
			       0);

	/* Start event loop */
	xv_main_loop(frame);
	return(0);
}

/*
 * 		canvas_repaint_proc()
 *	Called to repaint the canvas window.  Draw into the window 
 *	using various pixwin drawing functions.
 */
void
canvas_repaint_proc(canvas, paint_window, repaint_area)
    Canvas	canvas;
    Xv_Window	paint_window;
    Rectlist	*repaint_area;
{
	static	Pr_brush brush = { 5 };
	static	short dashed[] = { 5, 3, 0 };
	static	Pr_texture tex = {dashed, 0, 0, 0, 0, 0, 0, 0 };
	static	struct pr_pos lines[] = { {0,0}, {100,100}, {100,0}, {0,100} };
	static	struct pr_pos boxes[] = { {0,0},   {50,0},   {100,0},
					  {0,50},  {50,50},  {100,50},
					  {0,100}, {50,100}, {100,100} };

	pw_text(paint_window, 30, 20, PIX_SRC, NULL, "pw_rop");
	pw_rop(paint_window, 25, 25, 100, 100, PIX_SET, NULL, 0, 0);
	pw_rop(paint_window, 50, 50, 50, 50, PIX_CLR, NULL, 0, 0);

	pw_text(paint_window, 155, 20, PIX_SRC, NULL, "pw_replrop");
	pw_replrop(paint_window, 150, 25, 100, 100, PIX_SRC, gray_image, 0, 0);
	
	pw_text(paint_window, 280, 20, PIX_SRC, NULL, "pw_polypoint");
	pw_polypoint(paint_window, 275, 25, 9, boxes, PIX_SET);
	
	pw_text(paint_window, 30, 145, PIX_SRC, NULL, "pw_vector");
	pw_vector(paint_window, 25, 150, 125, 250, PIX_SRC, 1);
	pw_vector(paint_window, 25, 250, 125, 150, PIX_SRC, 1);
	
	pw_text(paint_window, 155, 145, PIX_SRC, NULL, "pw_line");
	pw_line(paint_window, 150, 150, 250, 250, &brush, &tex, PIX_SET);
	pw_line(paint_window, 150, 250, 250, 150, &brush, &tex, PIX_SET);
	
	pw_text(paint_window, 280, 145, PIX_SRC, NULL, "pw_polyline");
	pw_polyline(paint_window, 275, 150, 4, lines, POLY_CLOSE, NULL, NULL, PIX_SET);
}

