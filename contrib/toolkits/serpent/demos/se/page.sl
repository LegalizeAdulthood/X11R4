/* $Log:	page.sl,v $
% Revision 1.1  89/10/10  22:18:15  serpent
% Initial revision
% 
Revision 1.3  89/06/19  16:21:30  ljb
fix up rcs header
   */
/*--------------------------------------------------------------------------*\
|
|  Name:  page_selector
|
|  Description:
|     This page_selector puts the page selector/indicator buttons at 
|     the top of forms.
|
|  Parameters:
|     page_no - Page number to be displayed and to set page to on notify.
|     fparent - Parent form.
|     fhoriz - Horizontal distance from the parent form.
\*--------------------------------------------------------------------------*/
#define page_selector(page_no, fparent, fhoriz)\
page/**/page_no/**/_selector: command_widget {\
  Attributes:\
    parent: fparent;\
    vert_distance: 2;\
    horiz_distance: fhoriz;\
    label_text: page_no;\
    width: 13;\
    height: 14;\
    justify: CENTER;\
    background_color: {\
      if (page = page_no) then\
        background_color := "white";\
      else\
        background_color := "black";\
      end if;\
    }\
    foreground_color: {\
      if (page = page_no) then\
        foreground_color := "black";\
      else\
        foreground_color := "white";\
      end if;\
    }\
    border_color: {\
      if (page = page_no) then\
        border_color := "black";\
      else\
        border_color := "white";\
      end if;\
    }\
 Methods:\
   notify: {\
     page := page_no;\
   }\
}
