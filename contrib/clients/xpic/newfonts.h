/* $Header: newfonts.h,v 1.2 89/07/02 15:58:39 moraes Exp $ */
extern FontFamily *InitFonts();
extern XFontStruct *ChangeFont();
/*
 *  changefont calls ChangeFont and then changes gcNormal and gcInvert.
 *  It's in updown.c purely because I want to keep curFont local to that
 *  module. Sigh!
 */
extern void changefont();
/*
 *  defaultFontFamily and defaultFont aren't related. defaultFont is
 *  used as a placemark when we can't load a given font (lack of server
 *  memory?) while defaultFontFamily is used when we don't have that
 *  particluar font available at all
 */
extern XFontStruct *defaultFont;	/* font information needed */
extern FontFamily *defaultFontFamily;
extern FontFamily *findfont();
extern int findsize();
extern int fontChanged;
