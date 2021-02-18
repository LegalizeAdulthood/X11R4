/*
 * drawcard.c
 *
 * display cards on the table
 */

# include	"mille.h"
# include	"ui.h"
# include	"card.h"
# include	"background.h"

extern char	go_bits[], go_label_bits [], go_mask_bits[];
extern char	stop_bits[], stop_label_bits [], stop_mask_bits[];
extern char	right_bits[], right_label_bits[], right_mask_bits[];
extern char	speed_bits[], speed_mask_bits[];
extern char	end_bits[], end_mask_bits[];
extern char	accident_bits[], accident_label_bits [], accident_mask_bits[];
extern char	repairs_bits[], repairs_label_bits [], repairs_mask_bits[];
extern char	ace_bits[], ace_label_bits [], ace_mask_bits[];
extern char	flat_bits[], flat_label_bits[], flat_mask_bits[];
extern char	spare_bits[], spare_label_bits [], spare_mask_bits[];
extern char	puncture_bits[], puncture_label_bits [], puncture_mask_bits[];
extern char	out_bits[], out_label_bits[], out_mask_bits[];
extern char	gas_bits[], gas_mask_bits[];
extern char	extra_bits[], extra_label_bits [], extra_mask_bits[];
extern char	miles_mask_bits[];
extern char	_25_bits[], _50_bits[], _75_bits[], _100_bits[], _200_bits[];

extern char	deck_both_bits[];

struct card mono_cards[NUM_CARDS] = {
fill_bits,	WHITE_COLOR,	miles_mask_bits,	BLACK_COLOR,
_25_bits,	BLACK_COLOR,	0,			0,	0,

fill_bits,	WHITE_COLOR,	miles_mask_bits,	BLACK_COLOR,
_50_bits,	BLACK_COLOR, 	0, 			0,	0,

fill_bits,	WHITE_COLOR,	miles_mask_bits,	BLACK_COLOR,
_75_bits,	BLACK_COLOR,	0,			0,	0,

fill_bits,	WHITE_COLOR,	miles_mask_bits,	BLACK_COLOR,
_100_bits,	BLACK_COLOR,	0,			0,	0,

fill_bits,	WHITE_COLOR,	miles_mask_bits,	BLACK_COLOR,
_200_bits,	BLACK_COLOR,	0,			0,	0,

fill_bits,	WHITE_COLOR,	out_mask_bits,		BLACK_COLOR,
out_bits,	BLACK_COLOR,	out_label_bits,		BLACK_COLOR, 	0,

fill_bits,	WHITE_COLOR,	flat_mask_bits,		BLACK_COLOR,
flat_bits,	WHITE_COLOR,	flat_label_bits,	BLACK_COLOR, 	0,

fill_bits,	WHITE_COLOR,	accident_mask_bits,	BLACK_COLOR,
accident_bits,	WHITE_COLOR,	accident_label_bits,	BLACK_COLOR,	0,

fill_bits,	WHITE_COLOR,	stop_mask_bits,		BLACK_COLOR,
stop_bits,	WHITE_COLOR,	stop_label_bits,	BLACK_COLOR,	0,

fill_bits,	WHITE_COLOR,	speed_mask_bits,	BLACK_COLOR,
speed_bits,	BLACK_COLOR,	0,			0,		0,

fill_bits,	WHITE_COLOR,	gas_mask_bits,		BLACK_COLOR,
gas_bits,	BLACK_COLOR,	0,			0,		0,

fill_bits,	WHITE_COLOR,	spare_mask_bits,	BLACK_COLOR,
spare_bits,	WHITE_COLOR,	spare_label_bits,	BLACK_COLOR,	0,

fill_bits,	WHITE_COLOR,	repairs_mask_bits,	BLACK_COLOR,
repairs_bits,	WHITE_COLOR,	repairs_label_bits,	BLACK_COLOR,	0,

fill_bits,	WHITE_COLOR,	go_mask_bits,		BLACK_COLOR,
go_bits,	WHITE_COLOR,	go_label_bits,		BLACK_COLOR,	0,

fill_bits,	WHITE_COLOR,	end_mask_bits,		BLACK_COLOR,
end_bits,	BLACK_COLOR,	0,			0,	0,

fill_bits,	WHITE_COLOR,	extra_mask_bits,	BLACK_COLOR,
extra_bits,	WHITE_COLOR,	extra_label_bits,	BLACK_COLOR,	0,

fill_bits,	WHITE_COLOR,	puncture_mask_bits,	BLACK_COLOR,
puncture_bits,	BLACK_COLOR,	0,			0,		0,

fill_bits,	WHITE_COLOR,	ace_mask_bits,		BLACK_COLOR,
ace_bits,	WHITE_COLOR,	ace_label_bits,		BLACK_COLOR,	0,

fill_bits,	WHITE_COLOR,	right_mask_bits,	BLACK_COLOR,
right_bits,	WHITE_COLOR,	right_label_bits,	BLACK_COLOR,	0,
};

struct card color_cards[NUM_CARDS] = {
fill_bits,	WHITE_COLOR,	miles_mask_bits,	BLACK_COLOR,
_25_bits,	BLUE_COLOR,	0,			0,	0,

fill_bits,	WHITE_COLOR,	miles_mask_bits,	BLACK_COLOR,
_50_bits,	BLUE_COLOR,	0,			0,	0,

fill_bits,	WHITE_COLOR,	miles_mask_bits,	BLACK_COLOR,
_75_bits,	BLUE_COLOR,	0,			0,	0,

fill_bits,	WHITE_COLOR,	miles_mask_bits,	BLACK_COLOR,
_100_bits,	BLUE_COLOR,	0,			0,	0,

fill_bits,	WHITE_COLOR,	miles_mask_bits,	BLACK_COLOR,
_200_bits,	BLUE_COLOR,	0,			0,	0,

fill_bits,	WHITE_COLOR,	out_mask_bits,		BLACK_COLOR,
out_bits,	RED_COLOR,	out_label_bits,		RED_COLOR,	0,

fill_bits,	WHITE_COLOR,	flat_mask_bits,		BLACK_COLOR,
flat_bits,	RED_COLOR,	flat_label_bits,	RED_COLOR,	0,

fill_bits,	WHITE_COLOR,	accident_mask_bits,	BLACK_COLOR,
accident_bits,	RED_COLOR,	accident_label_bits,	RED_COLOR,	0,

fill_bits,	WHITE_COLOR,	stop_mask_bits,		BLACK_COLOR,
stop_bits,	RED_COLOR,	stop_label_bits,	RED_COLOR,	0,

fill_bits,	WHITE_COLOR,	speed_mask_bits,	BLACK_COLOR,
speed_bits,	RED_COLOR,	0,			0,		0,

fill_bits,	WHITE_COLOR,	gas_mask_bits,		BLACK_COLOR,
gas_bits,	GREEN_COLOR,	0,			0,		0,

fill_bits,	WHITE_COLOR,	spare_mask_bits,	BLACK_COLOR,
spare_bits,	GREEN_COLOR,	spare_label_bits,	GREEN_COLOR,	0,

fill_bits,	WHITE_COLOR,	repairs_mask_bits,	BLACK_COLOR,
repairs_bits,	GREEN_COLOR,	repairs_label_bits,	GREEN_COLOR,	0,

fill_bits,	WHITE_COLOR,	go_mask_bits,		BLACK_COLOR,
go_bits,	GREEN_COLOR,	go_label_bits,		GREEN_COLOR,	0,

fill_bits,	WHITE_COLOR,	end_mask_bits,		BLACK_COLOR,
end_bits,	GREEN_COLOR,	0,			0,	0,

fill_bits,	WHITE_COLOR,	extra_mask_bits,	BLACK_COLOR,
extra_bits,	BLUE_COLOR,	extra_label_bits,	BLUE_COLOR,	0,

fill_bits,	WHITE_COLOR,	puncture_mask_bits,	BLACK_COLOR,
puncture_bits,	BLUE_COLOR,	0,			0,		0,

fill_bits,	WHITE_COLOR,	ace_mask_bits,		BLACK_COLOR,
ace_bits,	BLUE_COLOR,	ace_label_bits,		BLUE_COLOR,	0,

fill_bits,	WHITE_COLOR,	right_mask_bits,	BLACK_COLOR,
right_bits,	RED_COLOR,	right_label_bits,	RED_COLOR,	0,
};

struct card	*cards;

extern char	deck_red_bits[], deck_blue_bits[], deck_mask_bits[];

struct card color_deck = {
	fill_bits,		GREEN_COLOR,
	deck_mask_bits,		BLACK_COLOR,
	deck_blue_bits,		BLUE_COLOR,
	deck_red_bits,		RED_COLOR,
	0,
};

struct card mono_deck = {
	fill_bits,		WHITE_COLOR,
	deck_mask_bits,		BLACK_COLOR,
	deck_blue_bits,		BLACK_COLOR,
	deck_red_bits,		BLACK_COLOR,
	0,
};

extern char	blank_bits[];

struct card color_blank = { blank_bits, GREY_COLOR,	0,0,0,0,0,0,0 };

struct card mono_blank = { blank_bits, WHITE_COLOR,	0,0,0,0,0,0,0 };

struct card	deck;
struct card	blank;

BitmapBitsPut (dpy, d, gc, mask, mask_gc, color, bits)
Display		*dpy;
Drawable	d;
Pixmap		mask;
GC		gc, mask_gc;
unsigned long	color;
char		*bits;
{
	XGCValues	gc_values;
	XImage		ximage;

	ximage.height = HEIGHT;
	ximage.width = WIDTH;
	ximage.depth = 1;
	ximage.xoffset = 0;
	ximage.format = XYPixmap;
	ximage.data = bits;
	ximage.byte_order = LSBFirst;
	ximage.bitmap_unit = 8;
	ximage.bitmap_bit_order = LSBFirst;
	ximage.bitmap_pad = 8;
	ximage.bytes_per_line = (WIDTH+7) / 8;
	XPutImage (dpy, mask, mask_gc, &ximage, 0, 0, 0, 0, WIDTH, HEIGHT);
	gc_values.clip_mask = mask;
	gc_values.foreground = color;
	gc_values.function = GXcopy;
	XChangeGC (dpy, gc,
		GCFunction|GCClipMask|GCForeground,
		&gc_values);
	XFillRectangle (dpy, d, gc, 0, 0, WIDTH, HEIGHT);
}


init_card (card)
struct card	*card;
{
	int	i;
	GC		gc;
	XGCValues	gc_values;
	GC		mask_gc;
	XGCValues	mask_values;
	Pixmap		mask;

	mask = XCreatePixmap (dpy, xwindow, WIDTH, HEIGHT, 1);
	gc = XCreateGC (dpy, xwindow, 0, 0);

	mask_gc = XCreateGC (dpy, mask, 0, 0);

	card->bits = XCreatePixmap(dpy, xwindow, WIDTH, HEIGHT,
				      DefaultDepth(dpy, screen));
	XSetForeground (dpy, gc, background);
	XFillRectangle (dpy, card->bits, gc, 0, 0, WIDTH, HEIGHT);
	for (i = 0; i < MAXBITMAPS && card->init[i].bits != 0; i++) {
		BitmapBitsPut (dpy, card->bits, gc, mask, mask_gc,
			colorMap[card->init[i].color].pixel, card->init[i].bits);
	}
	XFreeGC (dpy, gc);
	XFreeGC (dpy, mask_gc);
	XFreePixmap (dpy, mask);
}

Pixmap	card_border;

init_cards ()
{
	int		i;
	GC		gc;
	XGCValues	gc_values;
	Pixmap		rest;
	GC		mask_gc;
	XGCValues	mask_values;
	Pixmap		mask;

	setbuf(stderr, NULL);

	card_border = XCreateBitmapFromData (dpy, xwindow, fill_bits, WIDTH, HEIGHT);
	rest = XCreateBitmapFromData (dpy, xwindow, deck_mask_bits, WIDTH, HEIGHT);
	gc_values.function = GXor;
	gc = XCreateGC (dpy, card_border, GCFunction, &gc_values);
	XCopyArea (dpy, rest, card_border, gc, 0, 0, WIDTH, HEIGHT, 0, 0);
	XFreePixmap (dpy, rest);
	XFreeGC (dpy, gc);

	init_card (&blank);
	init_card (&deck);

	for (i = 0; i < (NUM_CARDS - 1); i++) {
		init_card (&cards[i]);
	}
}

init_color_cards ()
{
	deck = color_deck;
	blank = color_blank;
	cards = color_cards;
	init_cards ();
}


init_mono_cards()
{
	deck = mono_deck;
	blank = mono_blank;
	cards = mono_cards;
	init_cards ();
}

displayCard (card, x, y)
int	card;
int	x, y;
{
	if (card < 0 || card >= NUM_CARDS) {
		cardDisplay (&blank, x, y);
	} else {
		cardDisplay (&cards[card], x, y);
	}
}

struct displayed {
	struct displayed	*next;
	struct card		*card;
	int			x, y;
	int			flag;
};

static struct displayed	*onscreen;

cardDisplay (c, x, y)
struct card	*c;
{
	int	i;
	struct displayed	*d, *p;
	char			*malloc ();

	p = 0;
	for (d = onscreen; d; d = d->next) {
		if (d->x == x && d->y == y) {
			if (d->card == c)
				return;
			if (p) {
				p->next = d->next;
				d->next = onscreen;
				onscreen = d;
			}
			goto gotim;
		}
		p = d;
	}
	d = (struct displayed *) malloc (sizeof (struct displayed));
	d->x = x;
	d->y = y;
	if (p)
		p->next = d;
	else
		onscreen = d;
	d->next = 0;
gotim:	;
	d->card = c;
	drawIm (c, x, y);
}

static
drawIm (c, x, y)
struct card	*c;
int		x, y;
{
	int	i;
	GC	gc;
	XGCValues	gc_values;

	if (!c->bits)
		init_card (c);
	gc_values.clip_mask = card_border;
	gc_values.clip_x_origin = x;
	gc_values.clip_y_origin = y;
	gc = XCreateGC (dpy, xwindow, 0, 0);
	XCopyArea(dpy, c->bits, xwindow, gc, 0, 0, WIDTH, HEIGHT,
		  x, y);
	XFreeGC (dpy, gc);
}

cardRedisplay (x, y, w, h)
{
	struct displayed	*d;

	for (d = onscreen; d; d = d->next)
		d->flag = 0;
	redisplaybelow (onscreen, x, y, w, h);
}

static
redisplaybelow (d, x, y, w, h)
struct displayed	*d;
{
	int			x2, y2;

	x2 = x + w;
	y2 = y + h;
	for (; d; d = d->next) {
		if ((d->x <= x2 && x <= (d->x + WIDTH)) &&
		    (d->y <= y2 && y <= (d->y + HEIGHT)))
 		{
		    	if (d->flag == 0) {
				drawIm (d->card, d->x, d->y);
				d->flag = 1;
				redisplaybelow (d->next, d->x, d->y, WIDTH, HEIGHT);
			}
		}
	}

}

cardEraseAll ()
{
	struct displayed	*d, *n;

	for (d = onscreen; d; d = n) {
		n = d->next;
		free (d);
	}
	onscreen = 0;
}
