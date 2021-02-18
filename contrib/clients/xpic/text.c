#include "xpic.h"
#include "windows.h"

#include "assert.h"

static Buf textbuf = {
	NULL, 0
};
static XTextItem *textitems = NULL;
static int maxitems = 0;

#define ITEM_CHUNK 64

/*
 *  Draws a string 'str' in the window, putting 'space' pixels of
 *  padding for every space in the string.
 */
drawtext(w, x, y, str, len, gc, space)
Window w;
int x, y;
char *str;
int len;
GC gc;
int space;
{
	register char *s;
	register char *s1;
	register int nitems;
	int pad;
	
	if (index(str, ' ') == NULL) {
		XDrawString(picDpy, w, gc, x, y, str, len);
		return;
	}
	while (textbuf.size <= len + 1) {
		if (textbuf.size == 0) {
			textbuf.size = BUF_CHUNK;
			textbuf.buf = XtMalloc((unsigned) BUF_CHUNK);
		} else {
			textbuf.size += BUF_CHUNK;
			textbuf.buf = XtRealloc((char *) textbuf.buf,
			 (unsigned) textbuf.size);
		}
	}
	bcopy(str, textbuf.buf, len);
	/* sentinel blank at the end to simplify the following loop */
	textbuf.buf[len] = ' ';
	textbuf.buf[len+1] = '\0';
	s = textbuf.buf;
	nitems = 0;
	pad = 0;
	s1 = index(s, ' ');
	do {
		if (nitems >= maxitems) {
			if (maxitems == 0) {
				maxitems = ITEM_CHUNK;
				textitems = (XTextItem *) 
				 XtMalloc(ITEM_CHUNK * sizeof(XTextItem));
			} else {
				maxitems += ITEM_CHUNK;
				textitems = (XTextItem *) XtRealloc((char *) textitems, 
				 (unsigned) (maxitems * sizeof(XTextItem)));
			}
		}
		textitems[nitems].chars = s;
		textitems[nitems].nchars = s1 - s;
		textitems[nitems].delta = pad;
		textitems[nitems].font = None;
		*s1++ = '\0';
		for(pad = space; *s1 == ' '; s1++)
			pad += space;
		nitems++;
		s = s1;
	} while((s1 = index(s, ' ')) != NULL);
	XDrawText(picDpy, w, gc, x, y, textitems, nitems);
}
