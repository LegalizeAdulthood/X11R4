/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

#ifndef lint
static char *rcsidchartp = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/chart/RCS/chartp.c,v 1.6 89/09/14 14:16:56 tom Exp $";
#endif

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Chart View-object Palette

MODULE	chartp.c

VERSION	0.0

AUTHOR	TC Peters
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Chart View-object Palette.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  08/28/89	Created (TCP)
  08/30/89	Remove redundant matrix.ih include (TCP)
  08/31/89	Change static long to long for VAX compiler (TCP)
  09/01/89	Upgrade Suite attribute spellings (TCP)
  09/14/89	Suppress references to un-released Matrix class (TCP)

END-SPECIFICATION  ************************************************************/

#include  <andrewos.h>
#include  <view.ih>
#include  <im.ih>
#include  <frame.ih>
#include  <menulist.ih>
#include  <keymap.ih>
#include  <keystate.ih>
#include  <message.ih>
#include  <apt.h>
#include  <apt.ih>
#include  <chart.h>
#include  <chart.ih>
#include  <chartv.ih>
/*===#include  <matrix.ih>===*/
#include  <suite.ih>

#define  add_code		    1
#define  delete_code		    2
#define  sort_ascend_value_code	    3
#define  sort_descend_value_code    4
#define  sort_ascend_label_code	    5
#define  sort_descend_label_code    6
#define  sort_ascend_position_code  7
#define  sort_descend_position_code 8
#define  histogram_code		   10 /*=== Must be dynamic===*/
#define  dot_code		   11
#define  pie_code		   12
#define  map_code		   13
#define  stack_code		   14
#define  cartesian_code		   15
#define  line_code		   16
#define  print_code		   20
#define  save_code		   21
#define  top_title_code		   22
#define  bottom_title_code	   23
#define  left_title_code	   24
#define  right_title_code	   25

suite_Specification		add_button[] =
  {
  suite_ItemCaptionFontName( ItemCaptionFontNamePhrase ),
  suite_ItemCaption("Add"),
  suite_ItemDatum(add_code),  0
  };
suite_Specification		delete_button[] =
  {
  suite_ItemCaption("Delete"),
  suite_ItemDatum(delete_code),  0
  };
suite_Specification		histogram_button[] =
  {
  suite_ItemCaption("Histogram"), /*===NAMES MUST BE DYNAMIC===*/
  suite_ItemDatum(histogram_code),  0
  };
suite_Specification		line_button[] =
  {
  suite_ItemCaption("Line"),
  suite_ItemDatum(line_code),  0
  };
suite_Specification		dot_button[] =
  {
  suite_ItemCaption("Dot"),
  suite_ItemDatum(dot_code),  0
  };
suite_Specification		pie_button[] =
  {
  suite_ItemCaption("Pie"),
  suite_ItemDatum(pie_code),  0
  };
suite_Specification		map_button[] =
  {
  suite_ItemCaption("Map"),
  suite_ItemDatum(map_code),  0
  };
suite_Specification		stack_button[] =
  {
  suite_ItemCaption("Stack"),
  suite_ItemDatum(stack_code),  0
  };
suite_Specification		cartesian_button[] =
  {
  suite_ItemCaption("Cartesian"),
  suite_ItemDatum(cartesian_code),  0
  };
suite_Specification		sort_ascend_label_button[] =
  {
  suite_ItemCaption(AscendPhrase),
  suite_ItemDatum(sort_ascend_label_code),  0
  };
suite_Specification		sort_descend_label_button[] =
  {
  suite_ItemCaption(DescendPhrase),
  suite_ItemDatum(sort_descend_label_code),  0
  };
suite_Specification		sort_ascend_value_button[] =
  {
  suite_ItemCaption(AscendPhrase),
  suite_ItemDatum(sort_ascend_value_code),  0
  };
suite_Specification		sort_descend_value_button[] =
  {
  suite_ItemCaption(DescendPhrase),
  suite_ItemDatum(sort_descend_value_code),  0
  };
suite_Specification		sort_ascend_position_button[] =
  {
  suite_ItemCaption(AscendPhrase),
  suite_ItemDatum(sort_ascend_position_code),  0
  };
suite_Specification		sort_descend_position_button[] =
  {
  suite_ItemCaption(DescendPhrase),
  suite_ItemDatum(sort_descend_position_code),  0
  };
suite_Specification		print_button[] =
  {
  suite_ItemCaption(PrintPhrase),
  suite_ItemDatum(print_code),  0
  };
suite_Specification		save_button[] =
  {
  suite_ItemCaption(SavePhrase),
  suite_ItemDatum(save_code),  0
  };
long				Palette_Titles_Handler();
suite_Specification		top_title_button[] =
  {
  suite_ItemTitleCaption( TopPhrase ),
  suite_ItemAccessMode( suite_ReadWrite ),
  suite_ItemHitHandler( Palette_Titles_Handler ),
  suite_ItemDatum(top_title_code),  0
  };
suite_Specification		bottom_title_button[] =
  {
  suite_ItemTitleCaption( BottomPhrase ),
  suite_ItemAccessMode( suite_ReadWrite ),
  suite_ItemHitHandler( Palette_Titles_Handler ),
  suite_ItemDatum(bottom_title_code),  0
  };
suite_Specification		left_title_button[] =
  {
  suite_ItemTitleCaption( LeftPhrase ),
  suite_ItemAccessMode( suite_ReadWrite ),
  suite_ItemHitHandler( Palette_Titles_Handler ),
  suite_ItemDatum(left_title_code),  0
  };
suite_Specification		right_title_button[] =
  {
  suite_ItemTitleCaption( RightPhrase ),
  suite_ItemAccessMode( suite_ReadWrite ),
  suite_ItemHitHandler( Palette_Titles_Handler ),
  suite_ItemDatum(right_title_code),  0
  };
long				Palette_Hit();
suite_Specification		sort_label_buttons[] =
  {
  suite_TitleCaption( "By Label" ),
  suite_TitleCaptionFontName( TitleFontNamePhrase ),
  suite_TitleCaptionFontHigh( 12 ),
  suite_ItemCaptionFontName( ItemCaptionFontNamePhrase ),
  suite_ItemCaptionFontHigh( 10 ),
  suite_Arrangement( suite_Matrix ),
  suite_HitHandler( Palette_Hit ),
  suite_Item( sort_ascend_label_button ),
  suite_Item( sort_descend_label_button ),
  0
  };
suite_Specification		sort_value_buttons[] =
  {
  suite_TitleCaption( "By Value" ),
  suite_TitleCaptionFontName( TitleFontNamePhrase ),
  suite_TitleCaptionFontHigh( 12 ),
  suite_ItemCaptionFontName( ItemCaptionFontNamePhrase ),
  suite_ItemCaptionFontHigh( 10 ),
  suite_Arrangement( suite_Matrix ),
  suite_HitHandler( Palette_Hit ),
  suite_Item( sort_ascend_value_button ),
  suite_Item( sort_descend_value_button ),
  0
  };
suite_Specification		sort_position_buttons[] =
  {
  suite_TitleCaption( "By Position" ),
  suite_TitleCaptionFontName( TitleFontNamePhrase ),
  suite_TitleCaptionFontHigh( 12 ),
  suite_ItemCaptionFontName( ItemCaptionFontNamePhrase ),
  suite_ItemCaptionFontHigh( 10 ),
  suite_Arrangement( suite_Matrix ),
  suite_HitHandler( Palette_Hit ),
  suite_Item( sort_ascend_position_button ),
  suite_Item( sort_descend_position_button ),
  0
  };
suite_Specification		type_buttons[] =
  {
  suite_TitleCaption( "Chart Types" ),
  suite_TitleCaptionFontName( TitleFontNamePhrase ),
  suite_TitleCaptionFontHigh( 12 ),
  suite_ItemCaptionFontName( ItemCaptionFontNamePhrase ),
  suite_ItemCaptionFontHigh( 10 ),
  suite_Arrangement( suite_Matrix ),
  suite_HitHandler( Palette_Hit ),
  suite_Item( histogram_button ),
  suite_Item( dot_button ),
  suite_Item( line_button ),
  suite_Item( pie_button ),
  suite_Item( map_button ),
  suite_Item( stack_button ),
  suite_Item( cartesian_button ),
  0
  };

suite_Specification		title_buttons[] =
  {
  suite_TitleCaption( TitlesPhrase ),
  suite_TitleCaptionFontName( TitleFontNamePhrase ),
  suite_TitleCaptionFontHigh( 12 ),
  suite_ItemCaptionFontName( ItemCaptionFontNamePhrase ),
  suite_ItemCaptionFontHigh( 10 ),
  suite_Arrangement( suite_Matrix ),
  suite_Item( top_title_button ),
  suite_Item( bottom_title_button ),
  suite_Item( left_title_button ),
  suite_Item( right_title_button ),
  0
  };
suite_Specification		control_buttons[] =
  {
  suite_TitleCaption( "Controls" ),
  suite_TitleCaptionFontName( TitleFontNamePhrase ),
  suite_TitleCaptionFontHigh( 12 ),
  suite_ItemCaptionFontName( ItemCaptionFontNamePhrase ),
  suite_ItemCaptionFontHigh( 10 ),
  suite_Arrangement( suite_Matrix ),
  suite_HitHandler( Palette_Hit ),
  suite_Item( add_button ),
  suite_Item( delete_button ),
  suite_Item( print_button ),
  suite_Item( save_button ),
  0
  };
/*===
matrix_Specification		  palette_matrix[] =
  {
  matrix_Rows( 2 ),
  matrix_Columns( 2 ),
  0
  };

matrix_Specification		  sort_matrix[] =
  {
  matrix_Rows( 1 ),
  matrix_Columns( 3 ),
  matrix_TitleCaption( "Sort" ),
  matrix_TitleCaptionFontName( "Andysans12b" ),
  0
  };
===*/


#define  SetCellView(m,r,c,v) \
      matrix_SetCellAttribute(m,matrix_CellAtPosition(m,r,c),matrix_CellViewObject(v))

long
Initialize_Palette( self ) /*=== CONVERT TO REAL FORM ===*/
  register struct chartv	 *self;
  {
  register long			  status = ok;

  IN(Initialize_Palette);
/*===
  if ( ! PaletteInitialized )
    {
    PaletteInitialized = true;
    if ( ((ControlSuite = suite_Create( control_buttons, self )) == NULL)  ||
       ((SortLabelSuite = suite_Create( sort_label_buttons, self )) == NULL )  ||
       ((SortValueSuite = suite_Create( sort_value_buttons, self )) == NULL )  ||
       ((SortPositionSuite = suite_Create( sort_position_buttons, self )) == NULL )  ||
       ((TypeSuite = suite_Create( type_buttons, self )) == NULL )  ||
       ((TitleSuite = suite_Create( title_buttons, self )) == NULL )  ||
       ((Palette = matrix_Create( palette_matrix, self )) == NULL )  ||
       ((SortForm = matrix_Create( sort_matrix, self )) == NULL )  )
      { DEBUG(Object Creation Failed);
      status = failure;
      printf( "ChartV: Failed to Create Palette Object\n" );
      }
      else
      { DEBUG(Objects Created);
      DEBUGst(LeftAreaTitle,chart_AreaTitle( Chart, apt_LeftArea ));
      DEBUGst(TopAreaTitle,chart_AreaTitle( Chart, apt_TopArea ));
      DEBUGst(RightAreaTitle,chart_AreaTitle( Chart, apt_RightArea ));
      DEBUGst(BottomAreaTitle,chart_AreaTitle( Chart, apt_BottomArea ));
      suite_SetItemAttribute( TitleSuite, suite_ItemOfDatum( TitleSuite, left_title_code ),
	suite_ItemCaption( chart_AreaTitle( Chart, apt_LeftArea ) ) );
      suite_SetItemAttribute( TitleSuite, suite_ItemOfDatum( TitleSuite, top_title_code ),
	suite_ItemCaption( chart_AreaTitle( Chart, apt_TopArea ) ) );
      suite_SetItemAttribute( TitleSuite, suite_ItemOfDatum( TitleSuite, right_title_code ),
	suite_ItemCaption( chart_AreaTitle( Chart, apt_RightArea ) ) );
      suite_SetItemAttribute( TitleSuite, suite_ItemOfDatum( TitleSuite, bottom_title_code ),
	suite_ItemCaption( chart_AreaTitle( Chart, apt_BottomArea ) ) );
      SetCellView( Palette,  1, 1, ControlSuite );
      SetCellView( Palette,  1, 2, TitleSuite );
      SetCellView( Palette,  2, 1, SortForm );
      SetCellView( Palette,  2, 2, TypeSuite );
      SetCellView( SortForm, 1, 1, SortLabelSuite );
      SetCellView( SortForm, 1, 2, SortValueSuite );
      SetCellView( SortForm, 1, 3, SortPositionSuite );
      PaletteFrame = frame_New();
      frame_SetView( PaletteFrame, Palette );
      frame_SetCommandEnable( PaletteFrame, TRUE );
      frame_PostDefaultHandler( PaletteFrame, "message",
	    frame_WantHandler( PaletteFrame, "message" ) );
      }
    }
===*/
  OUT(Initialize_Palette);
  return  status;
  }

Destroy_Palette( self )
  register struct chartv	  *self;
  {
/*===
  if ( ControlSuite )	        suite_Destroy( ControlSuite );
  if ( TitleSuite )		suite_Destroy( TitleSuite );
  if ( TypeSuite )		suite_Destroy( TypeSuite );
  if ( SortValueSuite )		suite_Destroy( SortValueSuite );
  if ( SortLabelSuite )		suite_Destroy( SortLabelSuite );
  if ( SortPositionSuite )	suite_Destroy( SortPositionSuite );
  if ( Palette )		matrix_Destroy( Palette );
  if ( SortForm )		matrix_Destroy( SortForm );
===*/
  }

void
Expose_Palette( self )
  register struct chartv	  *self;
  {
  IN(Expose_Palette);
  if ( ! PaletteExposed  &&  Initialize_Palette( self ) == ok )
    { DEBUG(Expose);
    PaletteIm = im_Create( NULL );
    im_SetView( PaletteIm, PaletteFrame );
    im_SetTitle( PaletteIm, "Auxiliary Chart Palette" );
    menulist_SetMask( Menu, (menulist_GetMask( Menu ) &
		      ~menu_palette_hidden) | menu_palette_exposed );
    chartv_PostMenus( self, Menu );
    PaletteExposed = true;
    }
  OUT(Expose_Palette);
  }

void
Hide_Palette( self )
  register struct chartv	  *self;
  {
  IN(Hide_Palette);
  if ( PaletteExposed  &&  PaletteIm )
    { DEBUG(Hide);
    im_Destroy( PaletteIm );
    PaletteIm = NULL;
    menulist_SetMask( Menu, (menulist_GetMask( Menu ) &
		      ~menu_palette_exposed) | menu_palette_hidden );
    chartv_PostMenus( self, Menu );
    PaletteExposed = false;
    }
  OUT(Hide_Palette);
  }

long
Palette_Hit( self, suite, item, type, action, x, y, clicks )
  register struct chartv	  *self;
  register struct suite		  *suite;
  register struct suite_item	  *item;
  register enum view_MouseAction   action;
  register long			   x, y, clicks;
  {
  char				   msg[512];

  IN(Palette_Hit);
  DEBUGdt(Action,action);
  if ( type == suite_ItemObject  &&  action == view_LeftUp )
    {
    switch ( suite_ItemAttribute( suite, item, suite_ItemDatum(0) ) )
      {
      case  add_code:			DEBUG(Add);
	Add_Command( self );
	break;
      case  delete_code:		DEBUG(Delete);
	Delete_Command( self );
	break;
      case  histogram_code:
      case  dot_code:
      case  line_code:
      case  pie_code:
      case  map_code:
      case  stack_code:
      case  cartesian_code:		DEBUG(Types);
	ReChart( self, suite_ItemAttribute( suite, item, suite_ItemCaption(0) ) );
	break;
      case  print_code:			DEBUG(Print);
	Print_Command( self );
	break;
      case  sort_ascend_value_code:	DEBUG(SortAscendValue);
	chart_Sort( Chart, chart_ByValue | chart_Ascend, NULL );
        chart_NotifyObservers( Chart, chart_ItemsSorted );
	break;
      case  sort_descend_value_code:	DEBUG(SortDescendValue);
	chart_Sort( Chart, chart_ByValue | chart_Descend, NULL );
        chart_NotifyObservers( Chart, chart_ItemsSorted );
	break;
      case  sort_ascend_label_code:	DEBUG(SortAscendLabel);
	chart_Sort( Chart, chart_ByLabel | chart_Ascend, NULL );
        chart_NotifyObservers( Chart, chart_ItemsSorted );
	break;
      case  sort_descend_label_code:    DEBUG(SortDescendLabel);
	chart_Sort( Chart, chart_ByLabel | chart_Descend, NULL );
        chart_NotifyObservers( Chart, chart_ItemsSorted );
	break;
      case  sort_ascend_position_code:	DEBUG(SortAscendPosition);
	chart_Sort( Chart, chart_ByPosition | chart_Ascend, NULL );
        chart_NotifyObservers( Chart, chart_ItemsSorted );
	break;
      case  sort_descend_position_code: DEBUG(SortDescendPosition);
	chart_Sort( Chart, chart_ByPosition | chart_Descend, NULL );
        chart_NotifyObservers( Chart, chart_ItemsSorted );
	break;
      case  save_code:			DEBUG(Save);
	Save_Command( self );
	break;
      default:
	sprintf( msg, "ChartV: ERROR -- Unknown control-code (%d)",
		    suite_ItemAttribute( suite, item, suite_ItemDatum(0) ) );
	chartv_Announce( self, msg );
      } 
/*===    suite_NormalizeItem( suite, item );*/
    if ( !InputFocus )  chartv_WantInputFocus( self, self );
    }
  OUT(Palette_Hit);
  return 0;
  }

long
Palette_Titles_Handler( self, suite, item, action )
  register struct chartv	  *self;
  register struct suite		  *suite;
  register struct suite_item	  *item;
  register long			   action;
  {
  register char			  *title;
  register long			   area = NULL;

  IN(Palette_Titles_Handler);
  DEBUGxt(suite==TitleSuite,suite==TitleSuite);
  title = (char *)suite_ItemAttribute( suite, item, suite_ItemCaption(0) );
  DEBUGst(Title,title);
  DEBUGdt(Code,suite_ItemAttribute( suite, item, suite_ItemDatum(0) ) );
  if ( /*===action == */ 1 /*===s/b/suite_EndEntry,Also suite_BeginEntry is 0===*/ )
    {
    switch ( suite_ItemAttribute( suite, item, suite_ItemDatum(0) ) )
      {
      case  top_title_code:      area = apt_TopArea;    break;
      case  bottom_title_code:   area = apt_BottomArea; break;
      case  left_title_code:     area = apt_LeftArea;   break;
      case  right_title_code:    area = apt_RightArea;  break;
      default: DEBUG(Error: Bad Code); break;
      }
    DEBUGdt(Area,area);
    chart_SetAreaTitle( Chart, title, area );
    chart_NotifyObservers( Chart, chart_EnclosureModified );
    chart_SetModified( Chart );
    }
  OUT(Palette_Titles_Handler);
  return 0;
  }

Activate_Viewer( self )
  register struct chartv	 *self;
  {
  Activate( self, delete_code );
  Activate( self, print_code );
  Activate( self, save_code );
  }

Passivate( self, code )
  register struct chartv	 *self;
  register long			  code;
  {
  suite_PassivateItem( ControlSuite, suite_ItemOfDatum( ControlSuite, code ) );
  }

Activate( self, code )
  register struct chartv	 *self;
  register long			  code;
  {
  suite_ActivateItem( ControlSuite, suite_ItemOfDatum( ControlSuite, code ) );
  }

