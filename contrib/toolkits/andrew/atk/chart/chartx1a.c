/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*****************************************\

  Chart Programming Guide --  Example #1

\*****************************************/

#include  <andrewos.h>
#include  <im.ih>
#include  <frame.ih>
#include  <chart.ih>
#include  <chartv.ih>
#include  <chartx1a.eh>


#define  Chart		    (self->chart_data_object)
#define  ChartView	    (self->chart_view_object)
#define  Frame		    (self->frame)
#define  Im		    (self->im)

boolean 
chartx1app__InitializeObject( classID, self )
  struct classheader	 *classID;
  struct chartx1app	 *self;
  {
  chartx1app_SetMajorVersion( self, 0 );
  chartx1app_SetMinorVersion( self, 0 );
  Chart = chart_New();
  ChartView = chartv_New();
  return TRUE;
  }

boolean
chartx1app__Start( self )
  struct chartx1app	  *self;
  {
  long			   status = TRUE, mortgage, food, insurance,
			   entertainment, savings, education, vacation;

  super_Start( self );
  if ( Chart  &&  ChartView )
    {
    mortgage =	    Query( "Mortgage" );
    food =	    Query( "Food" );
    insurance =	    Query( "Insurance" );
    entertainment = Query( "Entertainment" );
    savings =	    Query( "Savings" );
    education =	    Query( "Education" );
    vacation =	    Query( "Vacation" );

    chart_SetChartAttribute( Chart, chart_Type( "Pie" ) );
    chart_SetChartAttribute( Chart, chart_TitleCaption( "Home Budget" ) );
    chart_SetItemAttribute( Chart, chart_CreateItem( Chart, "Mortgage", NULL ),
	chart_ItemValue( mortgage ) );
    chart_SetItemAttribute( Chart, chart_CreateItem( Chart, "Food", NULL ),
	chart_ItemValue( food ) );
    chart_SetItemAttribute( Chart, chart_CreateItem( Chart, "Insurance", NULL ),
	chart_ItemValue( insurance ) );
    chart_SetItemAttribute( Chart, chart_CreateItem( Chart, "Entertainment", NULL ),
	chart_ItemValue( entertainment ) );
    chart_SetItemAttribute( Chart, chart_CreateItem( Chart, "Savings", NULL ),
	chart_ItemValue( savings ) );
    chart_SetItemAttribute( Chart, chart_CreateItem( Chart, "Education", NULL ),
	chart_ItemValue( education ) );
    chart_SetItemAttribute( Chart, chart_CreateItem( Chart, "Vacation", NULL ),
	chart_ItemValue( vacation ) );
    Frame = frame_New();
    frame_SetView( Frame, ChartView );
    Im = im_Create(NULL);
    im_SetView( Im, Frame );
    chartv_SetDataObject( ChartView, Chart );
    chartv_WantInputFocus( ChartView, ChartView );
    return  status;
    }
    else 
    {
    printf( "ChartX1: Failed to Create objects\n" );
    return FALSE;
    }
  }


Query( topic )
  char			     *topic;
  {
  char			      response[255];

  printf( "Enter %s: ", topic );
  gets( response );
  return  atoi( response );
  }
