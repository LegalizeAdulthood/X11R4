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

extern int XtUnspecifiedPixmap;

/*
 *
 *  A few definitions to make StPopupSheetView.c easier to read.
 *   
 */
#define Shadowwidth                            10
#define Titleheight                            24

  /* Yes, this is gross, but the code will be easier to read.
     Trust me.  */
#define ViewWx                            vw->core.x
#define ViewWy                            vw->core.y
#define ViewWdepth                        vw->core.depth
#define ViewWwidth                        vw->core.width
#define ViewWheight                       vw->core.height
#define ViewWparent                       vw->core.parent
#define ViewWborder_width                 vw->core.border_width
#define ViewWbackground                   vw->core.background_pixel
#define ViewWbackgroundpixmap             vw->core.background_pixmap
#define ViewWbordercolor                  vw->core.border_pixel
#define ViewWborderpixmap                 vw->core.border_pixmap
#define ViewWchildren                     vw->composit.children
#define ViewWnumchildren                  vw->composit.num_children
#define ViewWshadowwidth		  vw->stpopupsheetview.shadow_width  
#define ViewWnormalGC		  	  vw->stpopupsheetview.normalGC
#define ViewWgrayGC		  	  vw->stpopupsheetview.grayGC
#define ViewWmainW                   	  vw->stpopupsheetview.mainW
#define ViewWmainwidth                    vw->stpopupsheetview.mainwidth
#define ViewWmainheight                   vw->stpopupsheetview.mainheight
#define ViewWmainborder_width 		  vw->stpopupsheetview.mainborder_width 
#define ViewWmainX                        0
#define ViewWmainY                        0
#define ViewWmaxheight 			  vw->stpopupsheetview.maxheight
#define ViewWmaxwidth 			  vw->stpopupsheetview.maxwidth
#define ViewWminheight 			  vw->stpopupsheetview.minheight
#define ViewWminwidth 			  vw->stpopupsheetview.minwidth
#define ViewWtitleW                   	  vw->stpopupsheetview.titleW
#define ViewWtitleforeground              vw->stpopupsheetview.titleforeground
#define ViewWtitlebackground              vw->stpopupsheetview.titlebackground
#define ViewWtitlebackgroundpixmap        vw->stpopupsheetview.titlebackgroundpixmap
#define ViewWtitlefont                    vw->stpopupsheetview.titlefont
#define ViewWtitle                        vw->stpopupsheetview.titlename
#define ViewWtitlebordercolor             vw->stpopupsheetview.titlebordercolor
#define ViewWtitleborderpixmap            vw->stpopupsheetview.titleborderpixmap
#define ViewWtitlejustify                 vw->stpopupsheetview.titlejustify
#define ViewWtitleborderwidth             vw->stpopupsheetview.titleborderwidth
#define ViewWtitlewidth                   ViewWmainwidth + ViewWmainborder_width*2 - ViewWtitleborderwidth*2 
#define ViewWtitleheight                  (ViewWtitle ? Titleheight : 0)
#define ViewWtitleX                       0
#define ViewWtitleY                       0
#define ViewWmarginW                   	  vw->stpopupsheetview.marginW
#define ViewWbottommargin                 vw->stpopupsheetview.bottommargin 
#define ViewWtopmargin                    vw->stpopupsheetview.topmargin 
#define ViewWleftmargin                   vw->stpopupsheetview.leftmargin 
#define ViewWrightmargin                  vw->stpopupsheetview.rightmargin 
#define ViewWmarginX                      ViewWleftmargin + ViewWmainborder_width
#define ViewWmarginY                      ViewWtitleheight + ViewWtopmargin  + ViewWtitleborderwidth*2
#define ViewWmarginwidth                  ((ViewWmainwidth - ViewWleftmargin - ViewWrightmargin) < 0) ? 1 : (ViewWmainwidth - ViewWleftmargin - ViewWrightmargin)  
#define ViewWmarginheight                 ((ViewWmainheight + ViewWmainborder_width - ViewWtitleborderwidth*2  - ViewWtitleheight - ViewWtopmargin - ViewWbottommargin) < 0) ? 1 : (ViewWmainheight + ViewWmainborder_width - ViewWtitleborderwidth*2  - ViewWtitleheight - ViewWtopmargin - ViewWbottommargin) 
#define ViewWwindcomW                  	  vw->stpopupsheetview.windcomW
#define ViewWwindcomwidth                 vw->stpopupsheetview.windcomwidth
#define ViewWwindcomheight                vw->stpopupsheetview.windcomheight
#define ViewWwindcomX                     ViewWmainwidth - 20 + ViewWmainborder_width*2
#define ViewWwindcomY                     ViewWmainheight - 20 + ViewWmainborder_width*2
#define ViewWgrayGC                       vw->stpopupsheetview.grayGC
#define ViewWcomPixmap                    vw->stpopupsheetview.compixmap
#define ViewWcallback                     vw->stpopupsheetview.callback
#define ViewWwindowable                   vw->stpopupsheetview.windowable
#define ViewWsetval                       vw->stpopupsheetview.setval
#define ViewWiswindow                     vw->stpopupsheetview.iswindow
#define ViewWreparent                  	  vw->stpopupsheetview.reparent
#define ViewWchildW                  	  vw->stpopupsheetview.childW

#define XtVField(vw,field)   vw->stpopupsheetview.field
#define XtCField(vw,field)   vw->core.field
