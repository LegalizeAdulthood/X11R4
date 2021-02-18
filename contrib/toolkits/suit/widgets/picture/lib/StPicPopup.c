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
 * StPicPopup.c - Create Popup 
 *
 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Cardinals.h>
#include <X11/Shell.h>
#include <X11/Scroll.h>
#include <X11/Label.h>
#include <X11/Command.h>
#include <X11/MenuBox.h>
#include <X11/Dialog.h>
#include <X11/Form.h>
#include <X11/List.h>
#include <St/StPictureP.h>
#include <St/StPicPopup.h>

static void       _CreatePopup();
static void       _CreatePopupDraw();
static void       _CreatePopupWrite();
static StLineAtr  _StMenuLineAtr;
static StPaintAtr _StMenuPaintAtr;
static StTextAtr  _StMenuTextAtr;

StPopupFunctionDescRec popupfunctiondesc = {
					    _CreatePopup,
					    _CreatePopupDraw,
					    _CreatePopupWrite,
					    &_StMenuLineAtr,
					    &_StMenuPaintAtr,
					    &_StMenuTextAtr,
};


static void 
_CreatePopupAttributes(w)
	Widget          w;	/* Command */
{
	Arg             args[12];
	Widget          popup, box, temp1, temp2, temp3, itemp;
	Widget          pop1, pop2, pop3, form1, form2, form3;
	StPictureWidget spw =
	(StPictureWidget) (w->core.parent->core.parent->core.parent);
	Widget          spww = (Widget) spw;
	XrmValue        from, to;
	Pixel           pix;
	String         *table;
	int             i;
	Pixmap          map;

	void            ParseLineType();
	void            ParseLineWidthS(), ParseLineWidthJ();
	void            ParseLineBegin(), ParseLineEnd(), ParseLineJoin();
	void            ParseLineColor(), ParseLineColorDoubleDash();
	void            ParseLineFunction();

	void            ParseFillTile(), ParseFillColorFore(), ParseFillColorBack();

	void            ParseCharKind(), ParseCharSize(), ParseCharColor(), ParseCharFunction();


	spw->stPicture.popup_attributes = popup =
		XtCreatePopupShell("popup_attributes", transientShellWidgetClass,
				   w, NULL, NULL);
	spw->stPicture.box_attributes = box =
		XtCreateManagedWidget("box_attributes", menuboxWidgetClass,
				      popup, NULL, NULL);
	XtSetArg(args[0], XtNlabel, "Line->");
	spw->stPicture.item_attributes[0] = temp1 =
		XtCreateManagedWidget("com_attributes", commandWidgetClass,
				      box, args, ONE);
	XtSetArg(args[0], XtNlabel, "Fill->");
	spw->stPicture.item_attributes[1] = temp2 =
		XtCreateManagedWidget("com_attributes", commandWidgetClass,
				      box, args, ONE);
	XtSetArg(args[0], XtNlabel, "Text->");
	spw->stPicture.item_attributes[2] = temp3 =
		XtCreateManagedWidget("com_attributes", commandWidgetClass,
				      box, args, ONE);
	XtOverrideTranslations(temp1,
			       XtParseTranslationTable((spw->stPicture.entries_trans_attributes)[0]));
	XtOverrideTranslations(temp2,
			       XtParseTranslationTable((spw->stPicture.entries_trans_attributes)[1]));
	XtOverrideTranslations(temp3,
			       XtParseTranslationTable((spw->stPicture.entries_trans_attributes)[2]));

	/**************
	* Third Popup *
	**************/
	spw->stPicture.popup_line = pop1 =
		XtCreatePopupShell("popup_line", transientShellWidgetClass,
				   temp1, NULL, NULL);
	spw->stPicture.form_line = form1 =
		XtCreateManagedWidget("form_line", formWidgetClass, pop1, NULL, NULL);

	/* type */
	XtSetArg(args[0], XtNcolumnSpacing, 2);
	XtSetArg(args[1], XtNrowSpacing, 0);
	XtSetArg(args[2], XtNinternalHeight, 0);
	XtSetArg(args[3], XtNinternalWidth, 2);
	XtSetArg(args[4], XtNforceColumns, True);
	XtSetArg(args[5], XtNdefaultDistance, 0);
	XtSetArg(args[6], XtNlist, spw->stPicture.linetypetable);
	XtSetArg(args[7], XtNdefaultColumns, 2);
	spw->stPicture.list_line_type =
		XtCreateManagedWidget("list_line_type", listWidgetClass,
				      form1, args, EIGHT);
	XtAddCallback(spw->stPicture.list_line_type, XtNcallback,
		      ParseLineType, NULL);

	/* width */
	XtSetArg(args[0], XtNlabel, "20");
	XtSetArg(args[1], XtNborderWidth, 0);
	XtSetArg(args[2], XtNfromHoriz, NULL);
	XtSetArg(args[3], XtNhorizDistance, 0);
	XtSetArg(args[4], XtNfromVert, spw->stPicture.list_line_type);
	XtSetArg(args[5], XtNvertDistance, 8);
	spw->stPicture.begin_line_width =
		XtCreateManagedWidget("lab_line_width", labelWidgetClass,
				      form1, args, SIX);
	XtSetArg(args[0], XtNwidth, 80);
	XtSetArg(args[1], XtNheight, 14);
	XtSetArg(args[2], XtNorientation, XtorientHorizontal);
	XtSetArg(args[3], XtNfromHoriz, spw->stPicture.begin_line_width);
	XtSetArg(args[4], XtNhorizDistance, 0);
	XtSetArg(args[5], XtNfromVert, spw->stPicture.list_line_type);
	XtSetArg(args[6], XtNvertDistance, 8);
	spw->stPicture.scr_line_width =
		XtCreateManagedWidget("scr_line_width", scrollbarWidgetClass,
				      form1, args, SEVEN);
	XtAddCallback(spw->stPicture.scr_line_width, XtNscrollProc,
		      ParseLineWidthS, NULL);
	XtAddCallback(spw->stPicture.scr_line_width, XtNjumpProc,
		      ParseLineWidthJ, NULL);

	XtSetArg(args[0], XtNlabel, "0");
	XtSetArg(args[1], XtNborderWidth, 0);
	XtSetArg(args[2], XtNfromHoriz, spw->stPicture.scr_line_width);
	XtSetArg(args[3], XtNhorizDistance, 0);
	XtSetArg(args[4], XtNfromVert, spw->stPicture.list_line_type);
	XtSetArg(args[5], XtNvertDistance, 8);
	spw->stPicture.end_line_width =
		XtCreateManagedWidget("lab_line_width", labelWidgetClass,
				      form1, args, SIX);
	XtSetArg(args[0], XtNlabel, "  ");
	XtSetArg(args[1], XtNfromHoriz, spw->stPicture.end_line_width);
	XtSetArg(args[2], XtNhorizDistance, 2);
	XtSetArg(args[3], XtNfromVert, spw->stPicture.list_line_type);
	XtSetArg(args[4], XtNvertDistance, 8);
	spw->stPicture.real_line_width =
		XtCreateManagedWidget("lab_line_width", labelWidgetClass,
				      form1, args, FIVE);

	/* cap-label */
	XtSetArg(args[0], XtNborderWidth, 0);
	XtSetArg(args[1], XtNfromHoriz, NULL);
	XtSetArg(args[2], XtNhorizDistance, 0);
	XtSetArg(args[3], XtNlabel, "Begin");
	XtSetArg(args[4], XtNfromVert, spw->stPicture.scr_line_width);
	XtSetArg(args[5], XtNvertDistance, 8);
	spw->stPicture.begin_cap_name =
		XtCreateManagedWidget("lab_line_cap", labelWidgetClass,
				      form1, args, SIX);
	XtSetArg(args[3], XtNlabel, " End ");
	XtSetArg(args[4], XtNfromVert, spw->stPicture.begin_cap_name);
	XtSetArg(args[5], XtNvertDistance, 1);
	spw->stPicture.end_cap_name =
		XtCreateManagedWidget("lab_line_cap", labelWidgetClass,
				      form1, args, SIX);

	/* begin-cap */
	XtSetArg(args[0], XtNcolumnSpacing, 2);
	XtSetArg(args[1], XtNrowSpacing, 0);
	XtSetArg(args[2], XtNinternalHeight, 0);
	XtSetArg(args[3], XtNinternalWidth, 2);
	XtSetArg(args[4], XtNforceColumns, True);
	XtSetArg(args[5], XtNdefaultDistance, 0);
	XtSetArg(args[6], XtNdefaultColumns, 3);
	XtSetArg(args[7], XtNlist, spw->stPicture.linecaptable);
	XtSetArg(args[8], XtNhorizDistance, 0);
	XtSetArg(args[9], XtNvertDistance, 8);
	XtSetArg(args[10], XtNfromHoriz, spw->stPicture.begin_cap_name);
	XtSetArg(args[11], XtNfromVert, spw->stPicture.scr_line_width);
	spw->stPicture.list_line_cap1 =
		XtCreateManagedWidget("list_line_cap", listWidgetClass,
				      form1, args, 12);
	XtAddCallback(spw->stPicture.list_line_cap1, XtNcallback,
		      ParseLineBegin, NULL);

	/* end-cap */
	XtSetArg(args[9], XtNvertDistance, 4);
	XtSetArg(args[10], XtNfromHoriz, spw->stPicture.end_cap_name);
	XtSetArg(args[11], XtNfromVert, spw->stPicture.list_line_cap1);
	spw->stPicture.list_line_cap2 =
		XtCreateManagedWidget("list_line_cap", listWidgetClass,
				      form1, args, 12);
	XtAddCallback(spw->stPicture.list_line_cap2, XtNcallback,
		      ParseLineEnd, NULL);

	/* join */
	XtSetArg(args[7], XtNlist, spw->stPicture.linejointable);
	XtSetArg(args[8], XtNfromVert, spw->stPicture.list_line_cap2);
	XtSetArg(args[9], XtNvertDistance, 8);
	spw->stPicture.list_line_join =
		XtCreateManagedWidget("list_line_join", listWidgetClass,
				      form1, args, TEN);
	XtAddCallback(spw->stPicture.list_line_join, XtNcallback,
		      ParseLineJoin, NULL);

	/* color */
	XtSetArg(args[0], XtNdefaultDistance, 0);
	XtSetArg(args[1], XtNfromVert, spw->stPicture.list_line_join);
	XtSetArg(args[2], XtNvertDistance, 8);
	spw->stPicture.form_line_color1 =
		XtCreateManagedWidget("form_line_color", formWidgetClass,
				      form1, args, THREE);

	if (XtScreen(w)->root_depth > 2)
		table = spw->stPicture.colortable1;
	else
		table = spw->stPicture.colortable2;
	XtSetArg(args[0], XtNwidth, 16);
	XtSetArg(args[1], XtNheight, 16);
	XtSetArg(args[2], XtNlabel, "");
	XtSetArg(args[3], XtNborderWidth, 0);
	for (i = 0; i < 16; i++) {
		from.addr = (caddr_t) table[i];
		from.size = strlen(from.addr);
		XtConvert((Widget) spw, "String", &from, "Pixel", &to);
		pix = *((Pixel *) to.addr);
		XtSetArg(args[4], XtNbackground, pix);

		switch (i) {
		case 0:
			XtSetArg(args[5], XtNfromVert, NULL);
			XtSetArg(args[6], XtNfromHoriz, NULL);
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			XtSetArg(args[5], XtNfromVert, NULL);
			XtSetArg(args[6], XtNfromHoriz,
				 spw->stPicture.line_color_item[i - 1]);
			break;
		case 8:
			XtSetArg(args[5], XtNfromVert,
				 spw->stPicture.line_color_item[0]);
			XtSetArg(args[6], XtNfromHoriz, NULL);
			break;
		default:
			XtSetArg(args[5], XtNfromVert,
				 spw->stPicture.line_color_item[0]);
			XtSetArg(args[6], XtNfromHoriz,
				 spw->stPicture.line_color_item[i - 1]);
		}
		spw->stPicture.line_color_item[i] =
			XtCreateManagedWidget("com_line_color", commandWidgetClass,
			      spw->stPicture.form_line_color1, args, SEVEN);
		spw->stPicture.realcolor[i] = pix;
		XtAddCallback(spw->stPicture.line_color_item[i], XtNcallback,
			      ParseLineColor, i);
	}

	XtSetArg(args[0], XtNfromVert, spw->stPicture.form_line_color1);
	XtSetArg(args[1], XtNvertDistance, 4);
	XtSetArg(args[2], XtNlabel, "");
	XtSetArg(args[3], XtNwidth, 128);
	XtSetArg(args[4], XtNheight, 8);
	spw->stPicture.line_color_name =
		XtCreateManagedWidget("lab_line_color", labelWidgetClass,
				      form1, args, FIVE);

	/* DoubleDash background-name */
	XtSetArg(args[0], XtNdefaultDistance, 0);
	XtSetArg(args[1], XtNfromVert, spw->stPicture.line_color_name);
	XtSetArg(args[2], XtNvertDistance, 2);
	XtSetArg(args[3], XtNlabel, "DoubleDash background");
	XtSetArg(args[4], XtNborderWidth, 0);
	spw->stPicture.dash_back_name =
		XtCreateManagedWidget("lab_line_doubledash", labelWidgetClass,
				      form1, args, FIVE);

	/* doubledash-background color */
	XtSetArg(args[0], XtNdefaultDistance, 0);
	XtSetArg(args[1], XtNfromVert, spw->stPicture.dash_back_name);
	XtSetArg(args[2], XtNvertDistance, 0);
	spw->stPicture.form_line_color2 =
		XtCreateManagedWidget("form_line_color", formWidgetClass,
				      form1, args, THREE);

	XtSetArg(args[0], XtNwidth, 16);
	XtSetArg(args[1], XtNheight, 16);
	XtSetArg(args[2], XtNlabel, "");
	XtSetArg(args[3], XtNborderWidth, 0);
	for (i = 0; i < 16; i++) {
		XtSetArg(args[4], XtNbackground, spw->stPicture.realcolor[i]);
		switch (i) {
		case 0:
			XtSetArg(args[5], XtNfromVert, NULL);
			XtSetArg(args[6], XtNfromHoriz, NULL);
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			XtSetArg(args[5], XtNfromVert, NULL);
			XtSetArg(args[6], XtNfromHoriz,
				 spw->stPicture.line_color_item2[i - 1]);
			break;
		case 8:
			XtSetArg(args[5], XtNfromVert,
				 spw->stPicture.line_color_item2[0]);
			XtSetArg(args[6], XtNfromHoriz, NULL);
			break;
		default:
			XtSetArg(args[5], XtNfromVert,
				 spw->stPicture.line_color_item2[0]);
			XtSetArg(args[6], XtNfromHoriz,
				 spw->stPicture.line_color_item2[i - 1]);
		}
		spw->stPicture.line_color_item2[i] =
			XtCreateManagedWidget("com_line_color", commandWidgetClass,
			      spw->stPicture.form_line_color2, args, SEVEN);
		XtAddCallback(spw->stPicture.line_color_item2[i], XtNcallback,
			      ParseLineColorDoubleDash, i);
	}

	XtSetArg(args[0], XtNfromVert, spw->stPicture.form_line_color2);
	XtSetArg(args[1], XtNvertDistance, 4);
	XtSetArg(args[2], XtNlabel, "");
	XtSetArg(args[3], XtNwidth, 128);
	XtSetArg(args[4], XtNheight, 8);
	spw->stPicture.line_color_name2 =
		XtCreateManagedWidget("lab_line_color", labelWidgetClass,
				      form1, args, FIVE);

	/* function */
	XtSetArg(args[0], XtNfromVert, spw->stPicture.line_color_name2);
	XtSetArg(args[1], XtNvertDistance, 8);
	XtSetArg(args[2], XtNlist, spw->stPicture.functiontable);
	XtSetArg(args[3], XtNdefaultColumns, 2);
	XtSetArg(args[4], XtNcolumnSpacing, 2);
	XtSetArg(args[5], XtNrowSpacing, 0);
	XtSetArg(args[6], XtNinternalHeight, 0);
	XtSetArg(args[7], XtNinternalWidth, 2);
	XtSetArg(args[8], XtNforceColumns, True);
	spw->stPicture.list_line_function =
		XtCreateManagedWidget("list_line_function", listWidgetClass,
				      form1, args, NINE);
	XtAddCallback(spw->stPicture.list_line_function, XtNcallback,
		      ParseLineFunction, NULL);

	/* ok */
	XtSetArg(args[0], XtNfromVert, spw->stPicture.list_line_function);
	XtSetArg(args[1], XtNvertDistance, 8);
	XtSetArg(args[2], XtNlabel, "OK");
	spw->stPicture.line_ok =
		XtCreateManagedWidget("com_line_ok", commandWidgetClass,
				      form1, args, THREE);
	XtOverrideTranslations(spw->stPicture.line_ok,
			       XtParseTranslationTable((spw->stPicture.entries_trans_attributes)[3]));

	/*--------------------------------------------------------------------*/
	spw->stPicture.popup_fill = pop2 =
		XtCreatePopupShell("popup_fill", transientShellWidgetClass,
				   temp2, NULL, NULL);
	spw->stPicture.form_fill = form2 =
		XtCreateManagedWidget("form_fill", formWidgetClass, pop2, NULL, NULL);

	/* tile */
	XtSetArg(args[0], XtNdefaultDistance, 0);
	spw->stPicture.form_fill_tile =
		XtCreateManagedWidget("form_fill_tile", formWidgetClass,
				      form2, args, ONE);
	XtSetArg(args[0], XtNwidth, 18);
	XtSetArg(args[1], XtNheight, 18);
	XtSetArg(args[2], XtNlabel, "");
	XtSetArg(args[3], XtNborderWidth, 0);
	XtSetArg(args[4], XtNresize, False);
	for (i = 0; i < 14; i++) {
		switch (i) {
		case 0:
			XtSetArg(args[5], XtNfromVert, NULL);
			XtSetArg(args[6], XtNfromHoriz, NULL);
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			XtSetArg(args[5], XtNfromVert, NULL);
			XtSetArg(args[6], XtNfromHoriz,
				 spw->stPicture.fill_tile_item[i - 1]);
			break;
		case 7:
			XtSetArg(args[5], XtNfromVert,
				 spw->stPicture.fill_tile_item[0]);
			XtSetArg(args[6], XtNfromHoriz, NULL);
			break;
		default:
			XtSetArg(args[5], XtNfromVert,
				 spw->stPicture.fill_tile_item[0]);
			XtSetArg(args[6], XtNfromHoriz,
				 spw->stPicture.fill_tile_item[i - 1]);
		}

		spw->stPicture.fill_tile_item[i] = itemp =
			XtCreateManagedWidget("com_fill_tile", commandWidgetClass,
				spw->stPicture.form_fill_tile, args, SEVEN);

		map = XCreatePixmapFromBitmapData(
						  XtDisplay(itemp),
						  RootWindow(
			 XtDisplay(itemp), DefaultScreen(XtDisplay(itemp))),
					    spw->stPicture.maptable[i].bits,
					   spw->stPicture.maptable[i].width,
					  spw->stPicture.maptable[i].height,
						BlackPixel(XtDisplay(itemp),
					   DefaultScreen(XtDisplay(itemp))),
						WhitePixel(XtDisplay(itemp),
					   DefaultScreen(XtDisplay(itemp))),
					       XtScreen(itemp)->root_depth);

		XtSetArg(args[7], XtNbackgroundPixmap, map);
		XtSetValues(itemp, &args[7], ONE);
		XtAddCallback(itemp, XtNcallback, ParseFillTile, i);
	}

	/* foreground-name */
	XtSetArg(args[0], XtNdefaultDistance, 0);
	XtSetArg(args[1], XtNfromVert, spw->stPicture.form_fill_tile);
	XtSetArg(args[2], XtNvertDistance, 0);
	XtSetArg(args[3], XtNlabel, "foreground");
	XtSetArg(args[4], XtNborderWidth, 0);
	spw->stPicture.fore_color_name =
		XtCreateManagedWidget("lab_fill_color", labelWidgetClass,
				      form2, args, FIVE);
	/* foreground-color */
	XtSetArg(args[0], XtNdefaultDistance, 0);
	XtSetArg(args[1], XtNfromVert, spw->stPicture.fore_color_name);
	XtSetArg(args[2], XtNvertDistance, 0);
	spw->stPicture.form_fill_color1 =
		XtCreateManagedWidget("form_fill_color", formWidgetClass,
				      form2, args, THREE);
	XtSetArg(args[0], XtNwidth, 16);
	XtSetArg(args[1], XtNheight, 16);
	XtSetArg(args[2], XtNlabel, "");
	XtSetArg(args[3], XtNborderWidth, 0);
	for (i = 0; i < 16; i++) {
		XtSetArg(args[4], XtNbackground, spw->stPicture.realcolor[i]);
		switch (i) {
		case 0:
			XtSetArg(args[5], XtNfromVert, NULL);
			XtSetArg(args[6], XtNfromHoriz, NULL);
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			XtSetArg(args[5], XtNfromVert, NULL);
			XtSetArg(args[6], XtNfromHoriz,
				 spw->stPicture.fill_color_item[i - 1]);
			break;
		case 8:
			XtSetArg(args[5], XtNfromVert,
				 spw->stPicture.fill_color_item[0]);
			XtSetArg(args[6], XtNfromHoriz, NULL);
			break;
		default:
			XtSetArg(args[5], XtNfromVert,
				 spw->stPicture.fill_color_item[0]);
			XtSetArg(args[6], XtNfromHoriz,
				 spw->stPicture.fill_color_item[i - 1]);
		}
		spw->stPicture.fill_color_item[i] =
			XtCreateManagedWidget("com_fill_color", commandWidgetClass,
			      spw->stPicture.form_fill_color1, args, SEVEN);
		XtAddCallback(spw->stPicture.fill_color_item[i], XtNcallback,
			      ParseFillColorFore, i);
	}

	/* background-name */
	XtSetArg(args[0], XtNdefaultDistance, 0);
	XtSetArg(args[1], XtNfromVert, spw->stPicture.form_fill_color1);
	XtSetArg(args[2], XtNvertDistance, 0);
	XtSetArg(args[3], XtNlabel, "background");
	XtSetArg(args[4], XtNborderWidth, 0);
	spw->stPicture.back_color_name =
		XtCreateManagedWidget("lab_fill_color", labelWidgetClass,
				      form2, args, FIVE);
	/* background-color */
	XtSetArg(args[0], XtNdefaultDistance, 0);
	XtSetArg(args[1], XtNfromVert, spw->stPicture.back_color_name);
	XtSetArg(args[2], XtNvertDistance, 0);
	spw->stPicture.form_fill_color2 =
		XtCreateManagedWidget("form_fill_color", formWidgetClass,
				      form2, args, THREE);
	XtSetArg(args[0], XtNwidth, 16);
	XtSetArg(args[1], XtNheight, 16);
	XtSetArg(args[2], XtNlabel, "");
	XtSetArg(args[3], XtNborderWidth, 0);
	for (i = 0; i < 16; i++) {
		XtSetArg(args[4], XtNbackground, spw->stPicture.realcolor[i]);
		switch (i) {
		case 0:
			XtSetArg(args[5], XtNfromVert, NULL);
			XtSetArg(args[6], XtNfromHoriz, NULL);
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			XtSetArg(args[5], XtNfromVert, NULL);
			XtSetArg(args[6], XtNfromHoriz,
				 spw->stPicture.fill_color_item2[i - 1]);
			break;
		case 8:
			XtSetArg(args[5], XtNfromVert,
				 spw->stPicture.fill_color_item2[0]);
			XtSetArg(args[6], XtNfromHoriz, NULL);
			break;
		default:
			XtSetArg(args[5], XtNfromVert,
				 spw->stPicture.fill_color_item2[0]);
			XtSetArg(args[6], XtNfromHoriz,
				 spw->stPicture.fill_color_item2[i - 1]);
		}
		spw->stPicture.fill_color_item2[i] =
			XtCreateManagedWidget("com_fill_color", commandWidgetClass,
			      spw->stPicture.form_fill_color2, args, SEVEN);
		XtAddCallback(spw->stPicture.fill_color_item2[i], XtNcallback,
			      ParseFillColorBack, i);
	}

	/* real */
	XtSetArg(args[0], XtNfromVert, spw->stPicture.form_fill_color2);
	XtSetArg(args[1], XtNvertDistance, 8);
	XtSetArg(args[2], XtNlabel, "");
	XtSetArg(args[3], XtNwidth, 128);
	XtSetArg(args[4], XtNheight, 16);
	spw->stPicture.fill_tile_name =
		XtCreateManagedWidget("lab_fill_tile", labelWidgetClass,
				      form2, args, FIVE);

	/* ok */
	XtSetArg(args[0], XtNfromVert, spw->stPicture.fill_tile_name);
	XtSetArg(args[1], XtNvertDistance, 8);
	XtSetArg(args[2], XtNlabel, "OK");
	spw->stPicture.fill_ok =
		XtCreateManagedWidget("com_fill_ok", commandWidgetClass,
				      form2, args, THREE);
	XtOverrideTranslations(spw->stPicture.fill_ok,
			       XtParseTranslationTable((spw->stPicture.entries_trans_attributes)[4]));

	/*--------------------------------------------------------------------*/
	spw->stPicture.popup_char = pop3 =
		XtCreatePopupShell("popup_char", transientShellWidgetClass,
				   temp3, NULL, NULL);
	spw->stPicture.form_char = form3 =
		XtCreateManagedWidget("form_char", formWidgetClass, pop3, NULL, NULL);

	/* kind */
	XtSetArg(args[0], XtNcolumnSpacing, 2);
	XtSetArg(args[1], XtNrowSpacing, 0);
	XtSetArg(args[2], XtNinternalHeight, 0);
	XtSetArg(args[3], XtNinternalWidth, 2);
	XtSetArg(args[4], XtNforceColumns, True);
	XtSetArg(args[5], XtNdefaultDistance, 0);
	XtSetArg(args[6], XtNlist, spw->stPicture.charkindtable);
	XtSetArg(args[7], XtNdefaultColumns, 2);
	spw->stPicture.list_char_kind =
		XtCreateManagedWidget("list_char_kind", listWidgetClass,
				      form3, args, EIGHT);
	XtAddCallback(spw->stPicture.list_char_kind, XtNcallback,
		      ParseCharKind, NULL);

	/* size */
	XtSetArg(args[6], XtNlist, spw->stPicture.charsizetable);
	XtSetArg(args[7], XtNdefaultColumns, 3);
	XtSetArg(args[8], XtNfromHoriz, spw->stPicture.list_char_kind);
	XtSetArg(args[9], XtNhorizDistance, 5);
	spw->stPicture.list_char_size =
		XtCreateManagedWidget("list_char_size", listWidgetClass,
				      form3, args, TEN);
	XtAddCallback(spw->stPicture.list_char_size, XtNcallback,
		      ParseCharSize, NULL);

	/* color */
	XtSetArg(args[0], XtNdefaultDistance, 0);
	XtSetArg(args[1], XtNfromVert, spw->stPicture.list_char_kind);
	XtSetArg(args[2], XtNvertDistance, 8);
	spw->stPicture.form_char_color1 =
		XtCreateManagedWidget("form_char_color", formWidgetClass,
				      form3, args, THREE);

	XtSetArg(args[0], XtNwidth, 16);
	XtSetArg(args[1], XtNheight, 16);
	XtSetArg(args[2], XtNlabel, "");
	XtSetArg(args[3], XtNborderWidth, 0);
	for (i = 0; i < 16; i++) {
		XtSetArg(args[4], XtNbackground, spw->stPicture.realcolor[i]);
		switch (i) {
		case 0:
			XtSetArg(args[5], XtNfromVert, NULL);
			XtSetArg(args[6], XtNfromHoriz, NULL);
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			XtSetArg(args[5], XtNfromVert, NULL);
			XtSetArg(args[6], XtNfromHoriz,
				 spw->stPicture.char_color_item[i - 1]);
			break;
		case 8:
			XtSetArg(args[5], XtNfromVert,
				 spw->stPicture.char_color_item[0]);
			XtSetArg(args[6], XtNfromHoriz, NULL);
			break;
		default:
			XtSetArg(args[5], XtNfromVert,
				 spw->stPicture.char_color_item[0]);
			XtSetArg(args[6], XtNfromHoriz,
				 spw->stPicture.char_color_item[i - 1]);
		}
		spw->stPicture.char_color_item[i] =
			XtCreateManagedWidget("com_char_color", commandWidgetClass,
			      spw->stPicture.form_char_color1, args, SEVEN);
		XtAddCallback(spw->stPicture.char_color_item[i], XtNcallback,
			      ParseCharColor, i);
	}

	XtSetArg(args[0], XtNfromVert, spw->stPicture.form_char_color1);
	XtSetArg(args[1], XtNvertDistance, 4);
	XtSetArg(args[2], XtNlabel, "");
	XtSetArg(args[3], XtNwidth, 128);
	XtSetArg(args[4], XtNheight, 8);
	spw->stPicture.char_color_name =
		XtCreateManagedWidget("lab_char_color", labelWidgetClass,
				      form3, args, FIVE);

	/* function */
	XtSetArg(args[0], XtNfromVert, spw->stPicture.char_color_name);
	XtSetArg(args[1], XtNvertDistance, 8);
	XtSetArg(args[2], XtNlist, spw->stPicture.functiontable);
	XtSetArg(args[3], XtNdefaultColumns, 2);
	XtSetArg(args[4], XtNcolumnSpacing, 2);
	XtSetArg(args[5], XtNrowSpacing, 0);
	XtSetArg(args[6], XtNinternalHeight, 0);
	XtSetArg(args[7], XtNinternalWidth, 2);
	XtSetArg(args[8], XtNforceColumns, True);
	spw->stPicture.list_char_function =
		XtCreateManagedWidget("list_char_function", listWidgetClass,
				      form3, args, NINE);
	XtAddCallback(spw->stPicture.list_char_function, XtNcallback,
		      ParseCharFunction, NULL);

	/* ok */
	XtSetArg(args[0], XtNfromVert, spw->stPicture.list_char_function);
	XtSetArg(args[1], XtNvertDistance, 8);
	XtSetArg(args[2], XtNlabel, "OK");
	spw->stPicture.char_ok =
		XtCreateManagedWidget("com_char_ok", commandWidgetClass,
				      form3, args, THREE);
	XtOverrideTranslations(spw->stPicture.char_ok,
			       XtParseTranslationTable((spw->stPicture.entries_trans_attributes)[5]));
}


static void 
_CreatePopupImage(w)
	Widget          w;	/* Command */
{
	Arg             args[2];
	Widget          popup, box, temp1;
	StPictureWidget spw =
	(StPictureWidget) (w->core.parent->core.parent->core.parent);

	spw->stPicture.popup_image = popup =
		XtCreatePopupShell("popup_image", transientShellWidgetClass,
				   w, NULL, NULL);
	XtSetArg(args[0], XtNlabel, "   Enter Filename   ");
	XtSetArg(args[1], XtNvalue, "");
	spw->stPicture.box_image = box =
		XtCreateManagedWidget("box_image", dialogWidgetClass,
				      popup, args, TWO);
	XtSetArg(args[0], XtNlabel, " OK ");
	spw->stPicture.item_image = temp1 =
		XtCreateManagedWidget("com_image", commandWidgetClass,
				      box, args, ONE);
	XtOverrideTranslations(temp1,
	  XtParseTranslationTable((spw->stPicture.entries_trans_image)[0]));
}


static void 
_CreatePopupRead(w)
	Widget          w;	/* Command */
{
	Arg             args[2];
	Widget          popup, box, temp1, temp2;
	StPictureWidget spw =
	(StPictureWidget) (w->core.parent->core.parent->core.parent);

	spw->stPicture.popup_read = popup =
		XtCreatePopupShell("popup_read", transientShellWidgetClass,
				   w, NULL, NULL);
	XtSetArg(args[0], XtNlabel, "   Enter Filename   ");
	XtSetArg(args[1], XtNvalue, "");
	spw->stPicture.box_read = box =
		XtCreateManagedWidget("box_read", dialogWidgetClass,
				      popup, args, TWO);
	XtSetArg(args[0], XtNlabel, "Merge");
	spw->stPicture.item_read[0] = temp1 =
		XtCreateManagedWidget("com_read", commandWidgetClass,
				      box, args, ONE);
	XtSetArg(args[0], XtNlabel, "Override");
	spw->stPicture.item_read[1] = temp2 =
		XtCreateManagedWidget("com_read", commandWidgetClass,
				      box, args, ONE);
	XtOverrideTranslations(temp1,
	   XtParseTranslationTable((spw->stPicture.entries_trans_read)[0]));
	XtOverrideTranslations(temp2,
	   XtParseTranslationTable((spw->stPicture.entries_trans_read)[1]));
}


static void 
_CreatePopupWrite(w)
	Widget          w;	/* Command */
{
	Arg             args[2];
	Widget          popup, box, temp1;
	String          st;
	StPictureWidget spw =
	(StPictureWidget) (w->core.parent->core.parent->core.parent);

	spw->stPicture.popup_write = popup =
		XtCreatePopupShell("popup_write", transientShellWidgetClass,
				   w, NULL, NULL);
	XtSetArg(args[0], XtNlabel, "   Enter Filename   ");
	if (!spw->stPicture.file_name)
		st = "";
	else
		st = spw->stPicture.file_name;
	XtSetArg(args[1], XtNvalue, st);
	spw->stPicture.box_write = box =
		XtCreateManagedWidget("box_write", dialogWidgetClass,
				      popup, args, TWO);
	XtSetArg(args[0], XtNlabel, " OK ");
	spw->stPicture.item_write = temp1 =
		XtCreateManagedWidget("com_write", commandWidgetClass,
				      box, args, ONE);
	XtOverrideTranslations(temp1,
	  XtParseTranslationTable((spw->stPicture.entries_trans_write)[0]));
}


static void 
_CreatePopupRotate(w)
	Widget          w;	/* Command */
{
	Arg             args[1];
	Widget          popup, box, temp;
	StPictureWidget spw =
	(StPictureWidget) (w->core.parent->core.parent->core.parent);

	spw->stPicture.popup_rotate = popup =
		XtCreatePopupShell("popup_rotate", transientShellWidgetClass,
				   w, NULL, NULL);
	spw->stPicture.box_rotate = box =
		XtCreateManagedWidget("box_rotate", menuboxWidgetClass,
				      popup, NULL, NULL);
	XtSetArg(args[0], XtNlabel, "+90");
	spw->stPicture.item_rotate[0] = temp =
		XtCreateManagedWidget("com_rotate", commandWidgetClass,
				      box, args, ONE);
	XtOverrideTranslations(temp,
	 XtParseTranslationTable((spw->stPicture.entries_trans_rotate)[0]));

	XtSetArg(args[0], XtNlabel, "+180");
	spw->stPicture.item_rotate[1] = temp =
		XtCreateManagedWidget("com_rotate", commandWidgetClass,
				      box, args, ONE);
	XtOverrideTranslations(temp,
	 XtParseTranslationTable((spw->stPicture.entries_trans_rotate)[1]));

	XtSetArg(args[0], XtNlabel, "+270");
	spw->stPicture.item_rotate[2] = temp =
		XtCreateManagedWidget("com_rotate", commandWidgetClass,
				      box, args, ONE);
	XtOverrideTranslations(temp,
	 XtParseTranslationTable((spw->stPicture.entries_trans_rotate)[2]));
}


static void 
_CreatePopupPriority(w)
	Widget          w;	/* Command */
{
	Arg             args[1];
	Widget          popup, box, temp1, temp2;
	StPictureWidget spw =
	(StPictureWidget) (w->core.parent->core.parent->core.parent);

	spw->stPicture.popup_priority = popup =
		XtCreatePopupShell("popup_priority", transientShellWidgetClass,
				   w, NULL, NULL);
	spw->stPicture.box_priority = box =
		XtCreateManagedWidget("box_priority", menuboxWidgetClass,
				      popup, NULL, NULL);
	XtSetArg(args[0], XtNlabel, "Raise");
	spw->stPicture.item_priority[0] = temp1 =
		XtCreateManagedWidget("com_priority", commandWidgetClass,
				      box, args, ONE);
	XtSetArg(args[0], XtNlabel, "Lower");
	spw->stPicture.item_priority[1] = temp2 =
		XtCreateManagedWidget("com_priority", commandWidgetClass,
				      box, args, ONE);
	XtOverrideTranslations(temp1,
	XtParseTranslationTable((spw->stPicture.entries_trans_priority)[0]));
	XtOverrideTranslations(temp2,
	XtParseTranslationTable((spw->stPicture.entries_trans_priority)[1]));
}


static void 
_CreatePopupGroup(w)
	Widget          w;	/* Command */
{
	Arg             args[1];
	Widget          popup, box, temp1, temp2;
	StPictureWidget spw =
	(StPictureWidget) (w->core.parent->core.parent->core.parent);

	spw->stPicture.popup_group = popup =
		XtCreatePopupShell("popup_group", transientShellWidgetClass,
				   w, NULL, NULL);
	spw->stPicture.box_group = box =
		XtCreateManagedWidget("box_draw", menuboxWidgetClass,
				      popup, NULL, NULL);
	XtSetArg(args[0], XtNlabel, "Set");
	spw->stPicture.item_group[0] = temp1 =
		XtCreateManagedWidget("com_group", commandWidgetClass,
				      box, args, ONE);
	XtSetArg(args[0], XtNlabel, "Reset");
	spw->stPicture.item_group[1] = temp2 =
		XtCreateManagedWidget("com_group", commandWidgetClass,
				      box, args, ONE);
	XtOverrideTranslations(temp1,
	  XtParseTranslationTable((spw->stPicture.entries_trans_group)[0]));
	XtOverrideTranslations(temp2,
	  XtParseTranslationTable((spw->stPicture.entries_trans_group)[1]));
}


static void 
_CreatePopupDraw(w)
	Widget          w;	/* Command */
{
	Arg             args[1];
	int             d1, i, num_make;
	Widget         *itemPtr, *itemPtr2;
	Widget          popup, box;
	StDrawFunction *st;
	StPictureWidget spw =
	(StPictureWidget) (w->core.parent->core.parent->core.parent);

	if (!spw->stPicture.popup_draw) {
		XtSetArg(args[0], XtNallowShellResize, True);
		spw->stPicture.popup_draw = popup =
			XtCreatePopupShell("popup_draw", transientShellWidgetClass,
					   w, args, ONE);
	} else
		popup = spw->stPicture.popup_draw;

	if (!spw->stPicture.box_draw)
		spw->stPicture.box_draw = box =
			XtCreateManagedWidget("box_draw", menuboxWidgetClass, popup, NULL, NULL);
	else
		box = spw->stPicture.box_draw;

	if (!spw->stPicture.dummy_draw) {
		spw->stPicture.dummy_draw =
			XtCreateWidget(" ", labelWidgetClass, box, NULL, NULL);
	}
	if (!spw->stPicture.lab_draw)
		spw->stPicture.lab_draw =
			XtCreateWidget("lab_draw", labelWidgetClass, box, NULL, NULL);

	num_make =
		spw->stPicture.num_entries_draw - spw->stPicture.maked_entries_draw;
	if (num_make > 0) {	/* add */
		spw->stPicture.item_draw =
			(Widget *) XtRealloc((char *) spw->stPicture.item_draw,
			sizeof(Widget) * (spw->stPicture.num_entries_draw));
		for (itemPtr = spw->stPicture.item_draw, i = 0;
		     i < spw->stPicture.maked_entries_draw; i++, itemPtr++);
		itemPtr2 = itemPtr;

		for (i = 0; i < num_make; i++, itemPtr++) {
			*itemPtr =
				XtCreateWidget("com_draw", commandWidgetClass,
					       box, NULL, NULL);
		}

		spw->stPicture.maked_entries_draw =
			spw->stPicture.num_entries_draw;
	}
	/* set  item-name & new-translation */
	if (spw->stPicture.functions_draw) {
		for (d1 = 0, st = spw->stPicture.functions_draw;
		     *st != StdrawEnd; st++, d1++);
	} else
		d1 = 0;

	itemPtr = spw->stPicture.item_draw;
	for (i = 0;
	(i < spw->stPicture.num_entries_draw) && (i < d1); i++, itemPtr++) {
		XtSetArg(args[0], XtNlabel, (spw->stPicture.entries_draw)[i]);
		XtSetValues(*itemPtr, args, ONE);

		switch ((int) (spw->stPicture.functions_draw)[i]) {
		case StdrawRectangle:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
					 (spw->stPicture.entries_trans_draw)
						  [(int) StdrawRectangle]));
			break;
		case StdrawCircle:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
					 (spw->stPicture.entries_trans_draw)
						     [(int) StdrawCircle]));
			break;
		case StdrawEllipse:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
					 (spw->stPicture.entries_trans_draw)
						    [(int) StdrawEllipse]));
			break;
		case StdrawLine:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
					 (spw->stPicture.entries_trans_draw)
						       [(int) StdrawLine]));
			break;
		case StdrawArc:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
					 (spw->stPicture.entries_trans_draw)
							[(int) StdrawArc]));
			break;
		case StdrawEllipticArc:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
					 (spw->stPicture.entries_trans_draw)
						[(int) StdrawEllipticArc]));
			break;
		case StdrawPie:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
					 (spw->stPicture.entries_trans_draw)
							[(int) StdrawPie]));
			break;
		case StdrawEllipticPie:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
					 (spw->stPicture.entries_trans_draw)
						[(int) StdrawEllipticPie]));
			break;
		case StdrawPolyline:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
					 (spw->stPicture.entries_trans_draw)
						   [(int) StdrawPolyline]));
			break;
		case StdrawPolygon:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
					 (spw->stPicture.entries_trans_draw)
						    [(int) StdrawPolygon]));
			break;
		case StdrawFreehand:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
					 (spw->stPicture.entries_trans_draw)
						   [(int) StdrawFreehand]));
			break;
		case StdrawText:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
					 (spw->stPicture.entries_trans_draw)
						       [(int) StdrawText]));
			break;
		case StdrawNull:
		default:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
					 (spw->stPicture.entries_trans_draw)
						       [(int) StdrawNull]));
		}
	}
	for (; i < spw->stPicture.num_entries_draw; i++, itemPtr++) {
		XtSetArg(args[0], XtNlabel, (spw->stPicture.entries_draw)[i]);
		XtSetValues(*itemPtr, args, ONE);
		XtOverrideTranslations(*itemPtr,
				       XtParseTranslationTable(
					 (spw->stPicture.entries_trans_draw)
						       [(int) StdrawNull]));
	}

	/* manage dummy for MenuBox Widget */
	XtManageChild(spw->stPicture.dummy_draw);

	/* unmanage all title & item */
	XtUnmanageChild(spw->stPicture.lab_draw);
	XtUnmanageChildren(spw->stPicture.item_draw,
			   spw->stPicture.maked_entries_draw);

	/* manage title */
	if (spw->stPicture.title_draw) {
		XtSetArg(args[0], XtNlabel, spw->stPicture.title_draw);
		XtSetValues(spw->stPicture.lab_draw, args, ONE);
		XtManageChild(spw->stPicture.lab_draw);
	}
	/* manage item */
	XtManageChildren(spw->stPicture.item_draw,
			 spw->stPicture.num_entries_draw);

	/* unmanage dummy for MenuBox Widget */
	XtUnmanageChild(spw->stPicture.dummy_draw);
}


static void 
_CreatePopup(w)
	StPictureWidget w;
{
	Arg             args[1];
	int             d1, i, j, num_make;
	String         *str;
	Widget         *itemPtr, *itemPtr2;
	Widget          popup, box;
	StPictureFunction *st;

	if (!w->stPicture.popup) {
		XtSetArg(args[0], XtNallowShellResize, True);
		w->stPicture.popup = popup =
			XtCreatePopupShell("popup", transientShellWidgetClass,
					   w, args, ONE);
	} else
		popup = w->stPicture.popup;

	if (!w->stPicture.box)
		w->stPicture.box = box =
			XtCreateManagedWidget("box", menuboxWidgetClass, popup, NULL, NULL);
	else
		box = w->stPicture.box;

	if (!w->stPicture.dummy)
		w->stPicture.dummy =
			XtCreateWidget(" ", labelWidgetClass, box, NULL, NULL);

	if (!w->stPicture.lab)
		w->stPicture.lab =
			XtCreateWidget("lab", labelWidgetClass, box, NULL, NULL);

	num_make = w->stPicture.num_entries - w->stPicture.maked_entries;
	if (num_make > 0) {	/* add */
		w->stPicture.item =
			(Widget *) XtRealloc((char *) w->stPicture.item,
			       sizeof(Widget) * (w->stPicture.num_entries));
		for (itemPtr = w->stPicture.item, i = 0;
		     i < w->stPicture.maked_entries; i++, itemPtr++);
		itemPtr2 = itemPtr;

		for (i = 0; i < num_make; i++, itemPtr++) {
			*itemPtr =
				XtCreateWidget("com", commandWidgetClass, box, NULL, NULL);
		}

		w->stPicture.maked_entries = w->stPicture.num_entries;
	}
	/* set  item-name & new-translation */
	if (w->stPicture.functions) {
		for (d1 = 0, st = w->stPicture.functions;
		     *st != StpictureEnd; st++, d1++);
	} else
		d1 = 0;

	itemPtr = w->stPicture.item;
	for (i = 0; (i < w->stPicture.num_entries) && (i < d1); i++, itemPtr++) {
		XtSetArg(args[0], XtNlabel, (w->stPicture.entries)[i]);
		XtSetValues(*itemPtr, args, ONE);

		switch ((int) (w->stPicture.functions)[i]) {
		case StpictureDraw:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
						(w->stPicture.entries_trans)
						    [(int) StpictureDraw]));

			if (!w->stPicture.popup_draw) {
				if (w->stPicture.entries_draw) {
					for (j = 0, str = w->stPicture.entries_draw;
					     *str != NULL; str++, j++);
					w->stPicture.num_entries_draw = j;
				} else
					w->stPicture.num_entries_draw = 0;
				if (w->stPicture.num_entries_draw)
					_CreatePopupDraw(*itemPtr);
			}
			break;
		case StpictureDrawEnd:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
						(w->stPicture.entries_trans)
						 [(int) StpictureDrawEnd]));
			break;
		case StpictureAttributes:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
						(w->stPicture.entries_trans)
					      [(int) StpictureAttributes]));
			if (!w->stPicture.popup_attributes)
				_CreatePopupAttributes(*itemPtr);
			break;
		case StpictureGroup:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
						(w->stPicture.entries_trans)
						   [(int) StpictureGroup]));
			if (!w->stPicture.popup_group)
				_CreatePopupGroup(*itemPtr);
			break;
		case StpicturePriority:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
						(w->stPicture.entries_trans)
						[(int) StpicturePriority]));
			if (!w->stPicture.popup_priority)
				_CreatePopupPriority(*itemPtr);
			break;
		case StpictureTopBottom:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
						(w->stPicture.entries_trans)
					       [(int) StpictureTopBottom]));
			break;
		case StpictureLeftRight:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
						(w->stPicture.entries_trans)
					       [(int) StpictureLeftRight]));
			break;
		case StpictureRotate:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
						(w->stPicture.entries_trans)
						  [(int) StpictureRotate]));
			if (!w->stPicture.popup_rotate)
				_CreatePopupRotate(*itemPtr);
			break;
		case StpictureDelete:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
						(w->stPicture.entries_trans)
						  [(int) StpictureDelete]));
			break;
		case StpictureCopy:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
						(w->stPicture.entries_trans)
						    [(int) StpictureCopy]));
			break;
		case StpictureMove:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
						(w->stPicture.entries_trans)
						    [(int) StpictureMove]));
			break;
		case StpictureResize:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
						(w->stPicture.entries_trans)
						  [(int) StpictureResize]));
			break;
		case StpictureWrite:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
						(w->stPicture.entries_trans)
						   [(int) StpictureWrite]));
			if (!w->stPicture.popup_write)
				_CreatePopupWrite(*itemPtr);
			break;
		case StpictureRead:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
						(w->stPicture.entries_trans)
						    [(int) StpictureRead]));
			if (!w->stPicture.popup_read)
				_CreatePopupRead(*itemPtr);
			break;
		case StpictureImage:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
						(w->stPicture.entries_trans)
						   [(int) StpictureImage]));
			if (!w->stPicture.popup_image)
				_CreatePopupImage(*itemPtr);
			break;
		case StpictureUndo:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
						(w->stPicture.entries_trans)
						    [(int) StpictureUndo]));
			break;
		case StpictureRedraw:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
						(w->stPicture.entries_trans)
						  [(int) StpictureRedraw]));
			break;
		case StpictureNull:
		default:
			XtOverrideTranslations(*itemPtr,
					       XtParseTranslationTable(
						(w->stPicture.entries_trans)
						    [(int) StpictureNull]));
		}
	}
	for (; i < w->stPicture.num_entries; i++, itemPtr++) {
		XtSetArg(args[0], XtNlabel, (w->stPicture.entries)[i]);
		XtSetValues(*itemPtr, args, ONE);
		XtOverrideTranslations(*itemPtr,
				       XtParseTranslationTable(
						(w->stPicture.entries_trans)
						    [(int) StpictureNull]));
	}

	/* manage dummy for MenuBox Widget */
	XtManageChild(w->stPicture.dummy);

	/* unmanage all title & item */
	XtUnmanageChild(w->stPicture.lab);
	XtUnmanageChildren(w->stPicture.item, w->stPicture.maked_entries);

	/* manage title */
	if (w->stPicture.title) {
		XtSetArg(args[0], XtNlabel, w->stPicture.title);
		XtSetValues(w->stPicture.lab, args, ONE);
		XtManageChild(w->stPicture.lab);
	}
	/* manage item */
	XtManageChildren(w->stPicture.item, w->stPicture.num_entries);

	/* unmanage dummy for MenuBox Widget */
	XtUnmanageChild(w->stPicture.dummy);
}


/*******************************************************
*
*  ParseAttributes Callback Procedures
*
*******************************************************/

static void 
ParseLineType(w, closure, callData)
	Widget          w;
	caddr_t         closure;
	caddr_t         callData;
{
	StPictureWidget spw = (StPictureWidget) w->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent->
	core.parent->core.parent->core.parent;

	switch (((XtListReturnStruct *) callData)->index) {
	case 0:
		_StMenuLineAtr.line_style = StlineSolid;
		break;
	case 1:
		_StMenuLineAtr.line_style = StlineOnOffDash;
		break;
	default:		/* 2 */
		_StMenuLineAtr.line_style = StlineDoubleDash;
	}
}


static void 
ParseLineWidthS(w, closure, callData)
	Widget          w;
	caddr_t         closure;
	int             callData;
{
	Arg             args[1];
	float           f1;
	int             i;
	StPictureWidget spw = (StPictureWidget) w->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent->
	core.parent->core.parent->core.parent;

	i = callData;
	if (i < 0)
		i *= -1;
	f1 = (float) i / (float) w->core.width;
	XtScrollBarSetThumb(w, f1, 1.0);

	f1 = 1.0 - f1;
	i = (int) (20.0 * f1);
	XtSetArg(args[0], XtNlabel, spw->stPicture.widthtable[i]);
	XtSetValues(spw->stPicture.real_line_width, args, 1);

	_StMenuLineAtr.line_width = i;
}


static void 
ParseLineWidthJ(w, closure, callData)
	Widget          w;
	caddr_t         closure;
	float          *callData;
{
	Arg             args[1];
	float           f1;
	int             i;
	StPictureWidget spw = (StPictureWidget) w->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent->
	core.parent->core.parent->core.parent;

	f1 = 1.0 - *callData;

	i = (int) (20.0 * f1);
	XtSetArg(args[0], XtNlabel, spw->stPicture.widthtable[i]);
	XtSetValues(spw->stPicture.real_line_width, args, 1);

	_StMenuLineAtr.line_width = i;
}


static void 
ParseLineBegin(w, closure, callData)
	Widget          w;
	caddr_t         closure;
	caddr_t         callData;
{
	StPictureWidget spw = (StPictureWidget) w->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent->
	core.parent->core.parent->core.parent;

	switch (((XtListReturnStruct *) callData)->index) {
	case 0:
		_StMenuLineAtr.begin_cap = StcapButt;
		break;
	case 1:
		_StMenuLineAtr.begin_cap = StcapRound;
		break;
	default:		/* 2 */
		_StMenuLineAtr.begin_cap = StcapArrow;
	}
}


static void 
ParseLineEnd(w, closure, callData)
	Widget          w;
	caddr_t         closure;
	caddr_t         callData;
{
	StPictureWidget spw = (StPictureWidget) w->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent->
	core.parent->core.parent->core.parent;

	switch (((XtListReturnStruct *) callData)->index) {
	case 0:
		_StMenuLineAtr.end_cap = StcapButt;
		break;
	case 1:
		_StMenuLineAtr.end_cap = StcapRound;
		break;
	default:		/* 2 */
		_StMenuLineAtr.end_cap = StcapArrow;
	}
}


static void 
ParseLineJoin(w, closure, callData)
	Widget          w;
	caddr_t         closure;
	caddr_t         callData;
{
	StPictureWidget spw = (StPictureWidget) w->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent->
	core.parent->core.parent->core.parent;

	switch (((XtListReturnStruct *) callData)->index) {
	case 0:
		_StMenuLineAtr.join_style = StjoinMiter;
		break;
	case 1:
		_StMenuLineAtr.join_style = StjoinRound;
		break;
	default:		/* 2 */
		_StMenuLineAtr.join_style = StjoinBevel;
	}
}


static void 
ParseLineColor(w, closure, callData)
	Widget          w;
	int             closure;
	caddr_t         callData;
{
	Arg             args[1];
	StPictureWidget spw = (StPictureWidget) w->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent;

	XtSetArg(args[0], XtNbackground, spw->stPicture.realcolor[closure]);
	XtSetValues(spw->stPicture.line_color_name, args, 1);

	_StMenuLineAtr.line_foreground = closure;
}


static void 
ParseLineColorDoubleDash(w, closure, callData)
	Widget          w;
	int             closure;
	caddr_t         callData;
{
	Arg             args[1];
	StPictureWidget spw = (StPictureWidget) w->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent;

	XtSetArg(args[0], XtNbackground, spw->stPicture.realcolor[closure]);
	XtSetValues(spw->stPicture.line_color_name2, args, 1);

	_StMenuLineAtr.line_background = closure;
}


static void 
ParseLineFunction(w, closure, callData)
	Widget          w;
	caddr_t         closure;
	caddr_t         callData;
{
	StPictureWidget spw = (StPictureWidget) w->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent->
	core.parent->core.parent->core.parent;

	_StMenuLineAtr.function =
		((XtListReturnStruct *) callData)->index;
}

/*----------------------------------------------------------------------------*/
static void 
ParseFillTile(w, closure, callData)
	Widget          w;
	int             closure;
	caddr_t         callData;
{
	int             tiletile, forefore, backback;
	Arg             args[1];
	StPictureWidget spw = (StPictureWidget) w->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent;

	switch (closure) {
	case 0:
		_StMenuPaintAtr.fill_style = StfillBoxes;
		break;
	case 1:
		_StMenuPaintAtr.fill_style = StfillCrossWeave;
		break;
	case 2:
		_StMenuPaintAtr.fill_style = StfillDimple1;
		break;
	case 3:
		_StMenuPaintAtr.fill_style = StfillDimple3;
		break;
	case 4:
		_StMenuPaintAtr.fill_style = StfillFlippedGray;
		break;
	case 5:
		_StMenuPaintAtr.fill_style = StfillGray1;
		break;
	case 6:
		_StMenuPaintAtr.fill_style = StfillGray3;
		break;
	case 7:
		_StMenuPaintAtr.fill_style = StfillLightGray;
		break;
	case 8:
		_StMenuPaintAtr.fill_style = StfillRootWeave;
		break;
	case 9:
		_StMenuPaintAtr.fill_style = StfillScales;
		break;
	case 10:
		_StMenuPaintAtr.fill_style = StfillWideWeave;
		break;
	case 11:
		_StMenuPaintAtr.fill_style = StfillStipple;
		break;
	case 12:
		_StMenuPaintAtr.fill_style = StfillSolid;
		break;
	default:		/* 13 */
		_StMenuPaintAtr.fill_style = StfillHollow;
	}

	tiletile = (int) _StMenuPaintAtr.fill_style;
	forefore = _StMenuPaintAtr.fill_foreground;
	backback = _StMenuPaintAtr.fill_background;

	if (spw->stPicture.localmap)
		XFreePixmap(XtDisplay(w), spw->stPicture.localmap);

	if (_StMenuPaintAtr.fill_style == StfillHollow)
		spw->stPicture.localmap = XCreatePixmapFromBitmapData(
							       XtDisplay(w),
		      RootWindow(XtDisplay(w), DefaultScreen(XtDisplay(w))),
				     spw->stPicture.maptable[tiletile].bits,
				    spw->stPicture.maptable[tiletile].width,
				   spw->stPicture.maptable[tiletile].height,
			    spw->stPicture.form_fill->core.background_pixel,
			    spw->stPicture.form_fill->core.background_pixel,
						   XtScreen(w)->root_depth);
	else
		spw->stPicture.localmap = XCreatePixmapFromBitmapData(
							       XtDisplay(w),
		      RootWindow(XtDisplay(w), DefaultScreen(XtDisplay(w))),
				     spw->stPicture.maptable[tiletile].bits,
				    spw->stPicture.maptable[tiletile].width,
				   spw->stPicture.maptable[tiletile].height,
					 spw->stPicture.realcolor[forefore],
					 spw->stPicture.realcolor[backback],
						   XtScreen(w)->root_depth);
	XtSetArg(args[0], XtNbackgroundPixmap, spw->stPicture.localmap);
	XtSetValues(spw->stPicture.fill_tile_name, args, 1);
}


static void 
ParseFillColorFore(w, closure, callData)
	Widget          w;
	int             closure;
	caddr_t         callData;
{
	int             tiletile, forefore, backback;
	Arg             args[1];
	StPictureWidget spw = (StPictureWidget) w->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent;

	_StMenuPaintAtr.fill_foreground = closure;
	if (_StMenuPaintAtr.fill_style == StfillHollow)
		return;

	tiletile = (int) _StMenuPaintAtr.fill_style;
	forefore = _StMenuPaintAtr.fill_foreground;
	backback = _StMenuPaintAtr.fill_background;

	if (spw->stPicture.localmap)
		XFreePixmap(XtDisplay(w), spw->stPicture.localmap);

	spw->stPicture.localmap = XCreatePixmapFromBitmapData(
							      XtDisplay(w),
		      RootWindow(XtDisplay(w), DefaultScreen(XtDisplay(w))),
				     spw->stPicture.maptable[tiletile].bits,
				    spw->stPicture.maptable[tiletile].width,
				   spw->stPicture.maptable[tiletile].height,
					 spw->stPicture.realcolor[forefore],
					 spw->stPicture.realcolor[backback],
						   XtScreen(w)->root_depth);
	XtSetArg(args[0], XtNbackgroundPixmap, spw->stPicture.localmap);
	XtSetValues(spw->stPicture.fill_tile_name, args, 1);
}


static void 
ParseFillColorBack(w, closure, callData)
	Widget          w;
	int             closure;
	caddr_t         callData;
{
	int             tiletile, forefore, backback;
	Arg             args[1];
	StPictureWidget spw = (StPictureWidget) w->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent;

	_StMenuPaintAtr.fill_background = closure;
	if (_StMenuPaintAtr.fill_style == StfillSolid ||
	    _StMenuPaintAtr.fill_style == StfillHollow)
		return;

	tiletile = (int) _StMenuPaintAtr.fill_style;
	forefore = _StMenuPaintAtr.fill_foreground;
	backback = _StMenuPaintAtr.fill_background;

	if (spw->stPicture.localmap)
		XFreePixmap(XtDisplay(w), spw->stPicture.localmap);

	spw->stPicture.localmap = XCreatePixmapFromBitmapData(
							      XtDisplay(w),
		      RootWindow(XtDisplay(w), DefaultScreen(XtDisplay(w))),
				     spw->stPicture.maptable[tiletile].bits,
				    spw->stPicture.maptable[tiletile].width,
				   spw->stPicture.maptable[tiletile].height,
					 spw->stPicture.realcolor[forefore],
					 spw->stPicture.realcolor[backback],
						   XtScreen(w)->root_depth);
	XtSetArg(args[0], XtNbackgroundPixmap, spw->stPicture.localmap);
	XtSetValues(spw->stPicture.fill_tile_name, args, 1);
}

/*----------------------------------------------------------------------------*/
static void 
ParseCharKind(w, closure, callData)
	Widget          w;
	caddr_t         closure;
	caddr_t         callData;
{
	StPictureWidget spw = (StPictureWidget) w->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent->
	core.parent->core.parent->core.parent;

	switch (((XtListReturnStruct *) callData)->index) {
	case 0:
		switch (_StMenuTextAtr.font) {
		case StfontRoman8:
		case StfontItalic8:
		case StfontBold8:
		case StfontBoldItalic8:
			_StMenuTextAtr.font = StfontRoman8;
			break;
		case StfontRoman16:
		case StfontItalic16:
		case StfontBold16:
		case StfontBoldItalic16:
			_StMenuTextAtr.font = StfontRoman16;
			break;
		default:	/* ***24 */
			_StMenuTextAtr.font = StfontRoman24;
		}
		break;
	case 1:
		switch (_StMenuTextAtr.font) {
		case StfontRoman8:
		case StfontItalic8:
		case StfontBold8:
		case StfontBoldItalic8:
			_StMenuTextAtr.font = StfontItalic8;
			break;
		case StfontRoman16:
		case StfontItalic16:
		case StfontBold16:
		case StfontBoldItalic16:
			_StMenuTextAtr.font = StfontItalic16;
			break;
		default:	/* ***24 */
			_StMenuTextAtr.font = StfontItalic24;
		}
		break;
	case 2:
		switch (_StMenuTextAtr.font) {
		case StfontRoman8:
		case StfontItalic8:
		case StfontBold8:
		case StfontBoldItalic8:
			_StMenuTextAtr.font = StfontBold8;
			break;
		case StfontRoman16:
		case StfontItalic16:
		case StfontBold16:
		case StfontBoldItalic16:
			_StMenuTextAtr.font = StfontBold16;
			break;
		default:	/* ***24 */
			_StMenuTextAtr.font = StfontBold24;
		}
		break;
	default:		/* 3 */
		switch (_StMenuTextAtr.font) {
		case StfontRoman8:
		case StfontItalic8:
		case StfontBold8:
		case StfontBoldItalic8:
			_StMenuTextAtr.font = StfontBoldItalic8;
			break;
		case StfontRoman16:
		case StfontItalic16:
		case StfontBold16:
		case StfontBoldItalic16:
			_StMenuTextAtr.font = StfontBoldItalic16;
			break;
		default:	/* ***24 */
			_StMenuTextAtr.font = StfontBoldItalic24;
		}
	}
}


static void 
ParseCharSize(w, closure, callData)
	Widget          w;
	caddr_t         closure;
	caddr_t         callData;
{
	StPictureWidget spw = (StPictureWidget) w->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent->
	core.parent->core.parent->core.parent;

	switch (((XtListReturnStruct *) callData)->index) {
	case 0:
		switch (_StMenuTextAtr.font) {
		case StfontRoman8:
		case StfontRoman16:
		case StfontRoman24:
			_StMenuTextAtr.font = StfontRoman8;
			break;
		case StfontItalic8:
		case StfontItalic16:
		case StfontItalic24:
			_StMenuTextAtr.font = StfontItalic8;
			break;
		case StfontBold8:
		case StfontBold16:
		case StfontBold24:
			_StMenuTextAtr.font = StfontBold8;
			break;
		default:	/* BoldItalic** */
			_StMenuTextAtr.font = StfontBoldItalic8;
		}
		break;
	case 1:
		switch (_StMenuTextAtr.font) {
		case StfontRoman8:
		case StfontRoman16:
		case StfontRoman24:
			_StMenuTextAtr.font = StfontRoman16;
			break;
		case StfontItalic8:
		case StfontItalic16:
		case StfontItalic24:
			_StMenuTextAtr.font = StfontItalic16;
			break;
		case StfontBold8:
		case StfontBold16:
		case StfontBold24:
			_StMenuTextAtr.font = StfontBold16;
			break;
		default:	/* BoldItalic** */
			_StMenuTextAtr.font = StfontBoldItalic16;
		}
		break;
	default:		/* 2 */
		switch (_StMenuTextAtr.font) {
		case StfontRoman8:
		case StfontRoman16:
		case StfontRoman24:
			_StMenuTextAtr.font = StfontRoman24;
			break;
		case StfontItalic8:
		case StfontItalic16:
		case StfontItalic24:
			_StMenuTextAtr.font = StfontItalic24;
			break;
		case StfontBold8:
		case StfontBold16:
		case StfontBold24:
			_StMenuTextAtr.font = StfontBold24;
			break;
		default:	/* BoldItalic** */
			_StMenuTextAtr.font = StfontBoldItalic24;
		}
	}
}


static void 
ParseCharColor(w, closure, callData)
	Widget          w;
	int             closure;
	caddr_t         callData;
{
	Arg             args[1];
	StPictureWidget spw = (StPictureWidget) w->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent;

	XtSetArg(args[0], XtNbackground, spw->stPicture.realcolor[closure]);
	XtSetValues(spw->stPicture.char_color_name, args, 1);

	_StMenuTextAtr.text_color = closure;
}


static void 
ParseCharFunction(w, closure, callData)
	Widget          w;
	caddr_t         closure;
	caddr_t         callData;
{
	StPictureWidget spw = (StPictureWidget) w->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent->
	core.parent->core.parent->core.parent;

	_StMenuTextAtr.function =
		((XtListReturnStruct *) callData)->index;
}

