/*
 * animation
 */

# include	"mille.h"
# include	"ui.h"
# include	"card.h"
# include	<math.h>

extern int	iscolor;

animate_move (player, orig_type, orig_arg, dest_type, dest_arg)
{
	int	ox, oy, dx, dy;

	compute_position (player, orig_type, orig_arg, &ox, &oy);
	compute_position (player, dest_type, dest_arg, &dx, &dy);
	do_animate (ox, oy, dx, dy);
}

# define abs(x)	((x) < 0 ? -(x) : (x))

/*
 * bigger numbers make it go faster
 */
 
double	animation_speed = .5;

# define accerate(v,r)	((v) + (animation_speed/10 * (r)))

static
do_animate (ox, oy, dx, dy)
{
	double	x, y;
	double	xc, yc;
	int	xd, yd;
	int	xp, yp;
	int	x1, y1, x2, y2, x3, y3, x4, y4;
	int	ix, iy;
	double	dist;
	double	rx, ry;

	x = ox;
	y = oy;
	xd = dx - ox;
	yd = dy - oy;
	dist = sqrt ((double) xd * xd + yd * yd);
	rx = (double) xd / dist;
	ry = (double) yd / dist;
	xc = animation_speed * rx;
	yc = animation_speed * ry;
	xp = yp = -32767;
	XFlush (dpy);
	while (abs(dx - x) > abs (xc) || abs(dy - y) > abs (yc)) {
		ix = x;
		iy = y;
		if (xp == -32767)
			draw_square (ix, iy, ix + WIDTH, iy + HEIGHT);
		else {
			if (xp < ix) {
				x1 = xp + WIDTH;
				x2 = ix + WIDTH;
				x3 = ix;
				x4 = ix + WIDTH;
			} else if (xp > ix) {
				x1 = ix;
				x2 = xp;
				x3 = ix;
				x4 = ix + WIDTH;
			} else {
				x1 = -32767;
				x2 = -32767;
				x3 = ix;
				x4 = ix + WIDTH;
			}
			if (yp < iy) {
				y1 = iy;
				y2 = yp + HEIGHT;
				y3 = yp + HEIGHT;
				y4 = iy + HEIGHT;
			} else if (yp > iy) {
				y1 = yp;
				y2 = iy + HEIGHT;
				y3 = iy;
				y4 = yp;
			} else {
				y1 = iy;
				y2 = iy + HEIGHT;
				y3 = -32767;
				y4 = -32767;
			}
			if (x1 != -32767 && y1 != -32767)
				draw_square (x1, y1, x2, y2);
			if (x3 != -32767 && y3 != -32767)
				draw_square (x3, y3, x4, y4);
			if (ix < xp) {
				x1 = ix + WIDTH;
				x2 = xp + WIDTH;
				x3 = xp;
				x4 = xp + WIDTH;
			} else if (ix > xp) {
				x1 = xp;
				x2 = ix;
				x3 = xp;
				x4 = xp + WIDTH;
			} else {
				x1 = -32767;
				x2 = -32767;
				x3 = xp;
				x4 = xp + WIDTH;
			}
			if (iy < yp) {
				y1 = yp;
				y2 = iy + HEIGHT;
				y3 = iy + HEIGHT;
				y4 = yp + HEIGHT;
			} else if (iy > yp) {
				y1 = iy;
				y2 = yp + HEIGHT;
				y3 = yp;
				y4 = iy;
			} else {
				y1 = yp;
				y2 = yp + HEIGHT;
				y3 = -32767;
				y4 = -32767;
			}
			if (x1 != -32767 && y1 != -32767)
				draw_square (x1, y1, x2, y2);
			if (x3 != -32767 && y3 != -32767)
				draw_square (x3, y3, x4, y4);
		}
		xp = ix;
		yp = iy;
		if (abs (dx - x) > xc)
			x += xc;
		if (abs (dy - y) > yc)
			y += yc;
		xc = accerate(xc, rx);
		yc = accerate(yc, ry);
	}
	draw_square (xp, yp, xp+WIDTH, yp+HEIGHT);
	XFlush (dpy);
}

static
draw_square (x1, y1, x2, y2)
{
	XSetFunction(dpy, cheap_gc, GXxor);
	XSetFillStyle(dpy, cheap_gc, FillSolid);
	XSetForeground (dpy, cheap_gc, foreground ^ background);
	XFillRectangle (dpy, xwindow, cheap_gc, x1, y1, x2-x1, y2-y1);
}

static
compute_position (player, type, arg, xp, yp)
int	*xp, *yp;
{
	switch (type) {
	case ANIMATE_HAND:
		switch (player) {
		case 0:
			*xp = HUM_HAND_X + (WIDTH + PAD_CARD) * arg;
			*yp = HUM_HAND_Y;
			break;
		case 1:
			*xp = COMP_HAND_X + (WIDTH + PAD_CARD) * arg;
			*yp = COMP_HAND_Y;
			break;
		}
		break;
	case ANIMATE_DECK:
		*xp = DECK_X;
		*yp = DECK_Y;
		break;
	case ANIMATE_DISC:
		*xp = DISCARD_X;
		*yp = DISCARD_Y;
		break;
	case ANIMATE_MILES:
		switch (player) {
		case 0:
			*xp = HUM_PLAY_X + (WIDTH + PAD_CARD) * (2 + C_200 - arg);
			*yp = HUM_PLAY_Y;
			break;
		case 1:
			*xp = COMP_PLAY_X + (WIDTH + PAD_CARD) * (2 + C_200 - arg);
			*yp = COMP_PLAY_Y;
			break;
		}
		break;
	case ANIMATE_BATTLE:
		switch (player) {
		case 0:
			*xp = HUM_PLAY_X + WIDTH + PAD_CARD;
			*yp = HUM_PLAY_Y;
			break;
		case 1:
			*xp = COMP_PLAY_X + WIDTH + PAD_CARD;
			*yp = COMP_PLAY_Y;
			break;
		}
		break;
	case ANIMATE_SPEED:
		switch (player) {
		case 0:
			*xp = HUM_PLAY_X;
			*yp = HUM_PLAY_Y;
			break;
		case 1:
			*xp = COMP_PLAY_X;
			*yp = COMP_PLAY_Y;
			break;
		}
		break;
	case ANIMATE_OBATTLE:
		switch (1-player) {
		case 0:
			*xp = HUM_PLAY_X + WIDTH + PAD_CARD;
			*yp = HUM_PLAY_Y;
			break;
		case 1:
			*xp = COMP_PLAY_X + WIDTH + PAD_CARD;
			*yp = COMP_PLAY_Y;
			break;
		}
		break;
	case ANIMATE_OSPEED:
		switch (1-player) {
		case 0:
			*xp = HUM_PLAY_X;
			*yp = HUM_PLAY_Y;
			break;
		case 1:
			*xp = COMP_PLAY_X;
			*yp = COMP_PLAY_Y;
			break;
		}
		break;
	case ANIMATE_SAFETY:
		switch (player) {
		case 0:
			*xp = HUM_SAFE_X + safety_offsets[arg - S_CONV].x;
			*yp = HUM_SAFE_Y + safety_offsets[arg - S_CONV].y;
			break;
		case 1:
			*xp = COMP_SAFE_X + safety_offsets[arg - S_CONV].x;
			*yp = COMP_SAFE_Y + safety_offsets[arg - S_CONV].y;
			break;
		}
		break;
	}
}
