/*
 *		Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 * FORTRAN to C binding for XGKS
 *
 * Todd Gill
 * Dave Owens
 * TCS Development
 * Cambridge MA
 *
 * $Header: fortmac.h,v 4.0 89/09/01 09:55:11 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/fortmac.h,v $
 *
 * $Log:	fortmac.h,v $
 * Revision 4.0  89/09/01  09:55:11  amy
 * Fixed copyright.
 * 
 * Revision 4.0  89/09/01  09:52:02  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.5  89/06/26  16:39:49  amy
 * DCR d1	Change references to errfpp to errfp.
 * 
 * Revision 1.4  89/04/06  16:27:40  bruce
 * PTR# c2043:	corrected TYPEOFRETURNEDVALUES macro to set errind.
 * 
 * Revision 1.3  89/03/14  20:31:37  amy
 * PTR c2043	Changed VALIDTYPE and VALIDMEMBER macros to
 * 		take errind parameter.
 * 
 * PTR c2061	Fixed the MIN macro.
 * 
 * Revision 1.2  89/03/07  16:37:38  amy
 * No change.
 * 
 * Revision 1.1  89/02/03  16:32:42  amy
 * Initial revision
 * 
 */

#define ASPECTSOURCE(x,y) \
          if (((x)<FORT_GBUNDL)||((x)>FORT_GINDIV)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define CLEARCONTROLFLAG(x,y) \
          if (((x)<FORT_GCONDI)||((x)>FORT_GALWAY)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define CLIPPINGINDICATOR(x,y) \
          if (((x)<FORT_GNCLIP)||((x)>FORT_GCLIP)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define COLOURAVAILABLE(x,y) \
          if (((x)<FORT_GMONOC)||((x)>FORT_GCOLOR)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define COORDINATESWITCH(x,y) \
          if (((x)<FORT_GWC)||((x)>FORT_GNDC)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define DEFERRALMODE(x,y) \
          if (((x)<FORT_GASAP)||((x)>FORT_GASTI)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define DETECTABILITY(x,y) \
          if (((x)<FORT_GUNDET)||((x)>FORT_GDETEC)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define DEVICECOORDINATEUNITS(x,y) \
          if (((x)<FORT_GMETRE)||((x)>FORT_GOTHU))  \
             {gerrorhand(2000,y,(errfp)); return;}
#define DISPLAYSURFACEEMPTY(x,y) \
          if (((x)<FORT_GNEMPT)||((x)>FORT_GEMPTY)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define DYNAMICMODIFICATION(x,y) \
          if (((x)<FORT_GIRG)||((x)>FORT_GIMM)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define ECHOSWITCH(x,y) \
          if (((x)<FORT_GNECHO)||((x)>FORT_GECHO)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define FILLAREAINTERIORSTYLE(x,y) \
          if (((x)<FORT_GHOLLO)||((x)>FORT_GHATCH)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define HIGHLIGHTING(x,y) \
          if (((x)<FORT_GNORML)||((x)>FORT_GHILIT)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define INPUTDEVICESTATUS(x,y) \
          if (((x)<FORT_GNONE)||((x)>FORT_GNCHOI)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define INPUTCLASS(x,y) \
          if (((x)<FORT_GNCLAS)||((x)>FORT_GSTRIN)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define IMPLICITREGENERATIONMODE(x,y) \
          if (((x)<FORT_GSUPPD)||((x)>FORT_GALLOW)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define LEVELOFGKS(x,y) \
          if (((x)<(FORT_GLMA))||((x)>FORT_GL2C)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define NEWFRAMEACTIONNECESSARY(x,y) \
          if (((x)<FORT_GNO)||((x)>FORT_GYES)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define OPERATINGMODE(x,y) \
          if (((x)<FORT_GREQU)||((x)>FORT_GEVENT)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define OPERATINGSTATEVALUE(x,y) \
          if (((x)<FORT_GGKCL)||((x)>FORT_GSGOP)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define PRESENCEOFINVALIDVALUES(x,y) \
          if (((x)<FORT_GABSNT)||((x)>FORT_GPRSNT)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define REGENERATIONFLAG(x,y) \
          if (((x)<FORT_GPOSTP)||((x)>FORT_GPERFO)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define RELATIVEINPUTPRIORITY(x,y) \
          if (((x)<FORT_GHIGHR)||((x)>FORT_GLOWER)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define SIMULTANEOUSEVENTSFLAG(x,y) \
          if (((x)<FORT_GNMORE)||((x)>FORT_GMORE)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define STOREAGEFLAG(x,y)\
          if (((x)<FORT_GNO) || ((x)>FORT_GYES)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define TEXTALIGNMENTHORIZONTAL(x,y) \
          if (((x)<FORT_GAHNOR)||((x)>FORT_GARITE)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define TEXTALIGNMENTVERTICAL(x,y) \
          if (((x)<FORT_GAVNOR)||((x)>FORT_GABOTT)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define TEXTPATH(x,y) \
          if (((x)<FORT_GRIGHT)||((x)>FORT_GDOWN)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define TEXTPRECISION(x,y) \
          if (((x)<FORT_GSTRP)||((x)>FORT_GSTRKP)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define TYPEOFRETURNEDVALUES(errind,x,y) \
          if (((x)<FORT_GSET)||((x)>FORT_GREALI)) \
             {if ((errind)) *(errind) = (int) (2000);gerrorhand(2000,y,(errfp)); return;}/*c2043*/
#define UPDATESTATENOTPENDING(x,y) \
          if (((x)<FORT_GNPEND)||((x)>FORT_GPEND)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define VECTORRASTEROTHERTYPE(x,y) \
          if (((x)<FORT_GVECTR)||((x)>FORT_GOTHWK)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define VISIBILITY(x,y) \
          if (((x)<FORT_GINVIS)||((x)>FORT_GVISI)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define WORKSTATIONCATEGORY(x,y) \
          if (((x)<FORT_GOUTPT)||((x)>FORT_GMI)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define WORKSTATIONSTATE(x,y) \
          if (((x)<FORT_GINACT)||((x)>FORT_GACTIV)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define LISTOFGDPATTRIBUTES(x,y) \
          if (((x)<FORT_GPLATT)||((x)>FORT_GFAATT)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define LINETYPE(x,y) \
          if (((x)<FORT_GLSOLI)||((x)>FORT_GLDASD)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define MARKERTYPE(x,y) \
          if (((x)<FORT_GPOINT)||((x)>FORT_GXMARK)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define ATTRIBUTECONTROLFLAG(x,y) \
          if (((x)<FORT_GCURNT)||((x)>FORT_GSPEC)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define POLYLINEFILLAREACONTROLFLAG(x,y) \
          if (((x)<FORT_GPLINE)||((x)>FORT_GFILLA)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define INITIALCHOICEPROMPTFLAG(x,y) \
          if (((x)<FORT_GPROFF)||((x)>FORT_GPRON)) \
             {gerrorhand(2000,y,(errfp)); return;}
#define VALIDTYPE(errind,x,min,max,fctid) \
          if (((x)<min)||((x)>max)) \
             {if ((errind)) *(errind) = (int) (22);gerrorhand(22,fctid,(errfp)); return;} /*c2043 */
#define VALIDMEMBER(errind,x,min,max,fctid) \
          if (((x)<min)||((x)>max)) \
             {if (errind) *(errind) = (int) (2002);gerrorhand(2002,fctid,(errfp)); return;} /*c2043 */
#define CHECKMAGICNUM(datarec,fctid)           \
          if ( ((Gdatarec *) datarec)->magicnum != MAGICNUMBER) \
             {gerrorhand(2003,fctid,(errfp));return;}
#define MIN(x,y)         \
          (x > y) ? y : x;  /* 2061 */

/* move values from a 2X3 array into a 1X6 array */
#define MOVE_ARRAY_2X3_TO_1X6(from,to) \
   (to)[0] = (float) (from)[0][0];               \
   (to)[1] = (float) (from)[1][0];                \
   (to)[2] = (float) (from)[0][1];                 \
   (to)[3] = (float) (from)[1][1];                  \
   (to)[4] = (float) (from)[0][2];                   \
   (to)[5] = (float) (from)[1][2];

/* move values from a 1X6 array into a 2X3 array */
#define MOVE_ARRAY_1X6_TO_2X3(from,to) \
   (to)[0][0] = (Gfloat) (from)[0];     \
   (to)[1][0] = (Gfloat) (from)[1];      \
   (to)[0][1] = (Gfloat) (from)[2];       \
   (to)[1][1] = (Gfloat) (from)[3];        \
   (to)[0][2] = (Gfloat) (from)[4];         \
   (to)[1][2] = (Gfloat) (from)[5];

/* convert column major matrix to row major matrix */
/* NOTE: dimx & dimy are the dimentions of "to"    */
#define CHANGE_ROW_TO_COL_MAJOR_I(from,to,dimx,dimy) \
  {                                                   \
  int col,row;                                         \
  for (col = 0; col < (dimy); col++)                    \
    for (row = 0; row < (dimx); row++)                   \
      *((to) + (col*(dimx)+row)) = (int) *((from) + (row*(dimy)+col)); \
  }

/* convert column major matrix to row major matrix */
/* NOTE: dimx & dimy are the dimentions of "to"    */
#define CHANGE_ROW_TO_COL_MAJOR_F(from,to,dimx,dimy) \
  {                                                   \
  int col,row;                                         \
  for (col = 0; col < (dimy); col++)                    \
    for (row = 0; row < (dimx); row++)                   \
      *((to) + (col*(dimx)+row)) = (float) *((from) + (row*(dimy)+col)); \
  }

/* c2044: Amy and Mike found these were wrong and changed them. */
/* convert column major matrix to row major matrix             */ /*c2044*/
/* NOTE: rows & cols are the FORTRAN dimentions: TO(ROWS,COLS) */ /*c2044*/
#define CHANGE_COL_TO_ROW_MAJOR_I(from,to,rows,cols) /*c2044*/ \
  {                                                  /*c2044*/  \
  int c,r;                                           /*c2044*/   \
  for (c = 0; c < (cols); c++)                       /*c2044*/    \
    for (r = 0; r < (rows); r++)                     /*c2044*/     \
      *((to) + (c*(rows)+r)) = (Gint) *((Gint *)(from) + (r*(cols)+c)); /*c2044*/ \
  }

/* convert column major matrix to row major matrix             */ /*c2044*/
/* NOTE: rows & cols are the FORTRAN dimentions: TO(ROWS,COLS) */ /*c2044*/
#define CHANGE_COL_TO_ROW_MAJOR_F(from,to,rows,cols) /*c2044*/ \
  {                                                  /*c2044*/  \
  int c,r;                                           /*c2044*/   \
  for (c = 0; c < (cols); c++)                       /*c2044*/    \
    for (r = 0; r < (rows); r++)                     /*c2044*/     \
      *((to) + (c*(rows)+r)) = (Gfloat) *((Gfloat *)(from) + (r*(cols)+c)); /*c2044*/ \
  }


