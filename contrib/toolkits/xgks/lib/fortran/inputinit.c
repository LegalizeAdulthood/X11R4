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
 *
 * FORTRAN to C binding for XGKS
 *
 * GKS Input device initialization functions :
 *  
 *	ginlc()
 *	ginsk()
 *	ginvl()
 *	ginch()
 *	ginpk()
 *	ginst()
 *
 *        
 * David Berkowitz
 * Bruce Haimowitz
 * Todd Gill
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: inputinit.c,v 4.0 89/08/31 18:30:57 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/inputinit.c,v $
 * 
 * $Log:	inputinit.c,v $
 * Revision 4.0  89/08/31  18:30:57  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.25  89/06/23  17:33:15  amy
 * DCR d1	Changed references to errfpp to errfp.
 * 
 * Revision 1.24  89/06/19  11:06:35  amy
 * PTR c2074	ginsk:  corrected PDR validity check.
 * 
 * Revision 1.23  89/06/15  17:23:24  bruce
 * PTR# c1179:	Changed variable declarations so they are different from
 * 		variables types so AIX is happy.
 * PTR# c1175:	Corrected calls to realloc so memory is not lost.
 * 
 * Revision 1.22  89/06/13  13:44:55  todd
 * The AIX hc compiler does not allow identical variable and
 * typedef names. Any instance of this was corrected.
 * ptr c1179
 * 
 * Revision 1.21  89/04/03  09:54:59  bruce
 * PTR# c2063:	Changed variable declaration for stroke points
 * 
 * Revision 1.20  89/03/16  09:35:22  amy
 * PTR c2049	ginvl:  added break in case 2 of switch.
 * 
 * Revision 1.19  89/03/14  20:09:30  amy
 * PTR c2054	Put parentheses around if expressions that check
 * 		validity of packed data records.
 * 		Reorder pdr integer order to be consistent with other 
 * 		functions.
 * 		Add code to copy initial points in the stroke record.
 * 
 * Revision 1.18  89/03/07  17:02:10  amy
 * PTR c2040	ginch:  changed switch statement on device status
 * 		to use correct enum. type values.
 * 
 * Revision 1.17  89/01/24  10:33:50  todd
 * Fixed ptr #2029.
 * ginitval was passing a pointer to ival not ival.
 * 
 * Revision 1.16  89/01/20  13:24:21  todd
 * Fixed call to ginitval.
 * 
 * Revision 1.15  88/12/30  09:51:18  todd
 * more code review changes.
 * 
 * Revision 1.14  88/12/05  15:18:52  owens
 * changed errfp to errfpp
 * 
 * Revision 1.12  88/12/05  14:32:13  todd
 * Changes for code review II.
 * 
 * Revision 1.11  88/11/16  10:16:37  todd
 * Removed extra call to gerrorhand.
 * Added check for ok return code so
 * a return is done before output parameters
 * are set.
 * 
 * Revision 1.10  88/11/07  11:33:45  todd
 * Checked out for testing.
 * 
 * Revision 1.1  88/10/14  12:00:01  david
 * Initial revision
 * 
 * Revision 1.8  88/09/30  10:00:28  bhaim
 * Added comments for default cases
 * 
 * Revision 1.7  88/09/30  08:55:39  todd
 * Removed unused variables.
 * 
 * Revision 1.6  88/09/29  11:05:31  todd
 * Added index declaration.
 * 
 * Revision 1.5  88/09/29  06:59:11  bhaim
 * *** empty log message ***
 * 
 * Revision 1.4  88/09/28  13:15:50  bhaim
 * No change
 * 
 * Revision 1.3  88/09/28  11:20:10  bhaim
 * Added prolouges
 * 
 * Revision 1.2  88/09/27  13:04:28  todd
 * Fixed comment.
 * 
 * Revision 1.1  88/09/27  08:16:58  todd
 * Initial revision
 * 
 */

static char *rcsid = "$Header: inputinit.c,v 4.0 89/08/31 18:30:57 amy Exp $";

#include "fortxgks.h"
#include <xgks.h>
#include "pdrutils.h"

extern FILE *errfp;  		/* d1 */
extern char *forttext;
extern int *fortint;
extern Gpoint *fortpoints;
extern char currforttext;
extern int currfortint;
extern int currfortpoints; /* c2063*/

/*$F
 * ginlc -  Initialise locator
 *
 * int *wkid                              workstation identifier   
 * int *lcdnr                             locator device number    
 * int *tnr                               initial transformation #
 * float *ipx,*ipy                        initial locator poistion
 * int *pet                               prompt/echo type         
 * float *xmin, *xmax, *ymin, *ymax       echo area in device coord
 * int *ldr                               dimension of data record 
 * char *datarec                          data record               
 *
 * Returns: Error 2003 in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.74
 */
ginlc_ (wkid,lcdnr,tnr,ipx,ipy,pet,xmin,xmax,ymin,ymax,ldr,datarec)
int *wkid;                            
int *lcdnr;                          
int *tnr;                           
float *ipx,*ipy;                   
int *pet;                         
float *xmin, *xmax, *ymin, *ymax;
int *ldr;                       
char *datarec;                 
{
    Glimit glimit;		/* c1179 */
    Glocrec glocrec;		/* c1179 */
    Gloc    gloc;		/* c1179 */
    
    debug ( ("Initialize locator %d %d \n", *wkid, *lcdnr) );
    debug ( ("    echo area (%7.2f, %7.2f) (%7.2f, %7.2f) \n", *xmin, *ymin, *xmax, *ymax) );        
    
    
    gloc.transform = *tnr;		/* c1179 */
    gloc.position.x  = *ipx;		/* c1179 */
    gloc.position.y  = *ipy;		/* c1179 */

    glimit.xmin = *xmin;		/* c1179 */
    glimit.xmax = *xmax;		/* c1179 */
    glimit.ymin = *ymin;		/* c1179 */
    glimit.ymax = *ymax;		/* c1179 */

    switch (*pet)
        {
	case 4:

	    {
             Gacf attrcontrolflag;
	     CHECKMAGICNUM( datarec,errginitloc);
             if (pdrintnum(datarec) < 1)
		{
		  gerrorhand(2003,errginitloc,(errfp));		/* d1 */
		  return;
                }
             attrcontrolflag = (Gacf)pdrintindex(datarec,1);
             glocrec.pet4.acf = attrcontrolflag;		/* c1179 */
             glocrec.pet4.data = NULL;		/* c1179 */
             if (attrcontrolflag == GSPECIFIED)
               {
                 if ((pdrintnum(datarec) != 7) || (pdrrealnum(datarec) != 1)) /* c2054 */
                   {
                     gerrorhand(2003,errginitloc,(errfp));	/* d1 */
                     return;
                   }
                 glocrec.pet4.ln.type   = pdrintindex(datarec,2);		/* c1179 */
                 glocrec.pet4.ln.width  = pdrintindex(datarec,3);		/* c1179 */
                 glocrec.pet4.ln.colour = pdrintindex(datarec,4);		/* c1179 */
                 glocrec.pet4.ln.line   = pdrintindex(datarec,5);		/* c1179 */
                 glocrec.pet4.ln.bundl.type = pdrintindex(datarec,6);		/* c1179 */
                 glocrec.pet4.ln.bundl.colour = pdrintindex(datarec,7);		/* c1179 */
                 glocrec.pet4.ln.bundl.width  = pdrrealindex(datarec,1);	/* c1179 */
               }
	    }

	    break;

	case 5:
            {
             Gacf  attrcontrolflag;
             Gpfcf areacontrolflag;
	     CHECKMAGICNUM( datarec,errginitloc);
             if (pdrintnum(datarec) < 2)
		{
		  gerrorhand(2003,errginitloc,(errfp));		/* d1 */
		  return;
                }
             attrcontrolflag = (Gacf)pdrintindex(datarec,1);	/* c2054 */
             areacontrolflag = (Gpfcf)pdrintindex(datarec,2);	/* c2054 */
             glocrec.pet5.acf  = attrcontrolflag;		/* c1179 */
             glocrec.pet5.pfcf = areacontrolflag;		/* c1179 */
             glocrec.pet5.data = NULL;				/* c1179 */
             if (areacontrolflag == GPF_POLYLINE)
               {
                if (attrcontrolflag == GSPECIFIED)
                  if (pdrintnum(datarec) != 8 || pdrrealnum(datarec) != 1)
                   {
                     gerrorhand(2003,errginitloc,(errfp));	/* d1 */
                     return;
                   }
                 glocrec.pet5.attr.ln.type   = pdrintindex(datarec,3);		/* c1179 */
                 glocrec.pet5.attr.ln.width  = pdrintindex(datarec,4);		/* c1179 */
                 glocrec.pet5.attr.ln.colour = pdrintindex(datarec,5);		/* c1179 */
                 glocrec.pet5.attr.ln.line   = pdrintindex(datarec,6);		/* c1179 */
                 glocrec.pet5.attr.ln.bundl.type = pdrintindex(datarec,7);	/* c1179 */
                 glocrec.pet5.attr.ln.bundl.colour = pdrintindex(datarec,8);	/* c1179 */
                 glocrec.pet5.attr.ln.bundl.width  = pdrrealindex(datarec,1);	/* c1179 */
               }
             else /* Fill Area */
               {
                if (attrcontrolflag == GSPECIFIED)
                  if (pdrintnum(datarec) != 9 )
                   {
                     gerrorhand(2003,errginitloc,(errfp));	/* d1 */
                     return;
                   }
                 glocrec.pet5.attr.fl.inter   = pdrintindex(datarec,3);		/* c1179 */
                 glocrec.pet5.attr.fl.style  = pdrintindex(datarec,4);		/* c1179 */
                 glocrec.pet5.attr.fl.colour = pdrintindex(datarec,5);		/* c1179 */
                 glocrec.pet5.attr.fl.fill   = pdrintindex(datarec,6);		/* c1179 */
                 glocrec.pet5.attr.fl.bundl.inter = pdrintindex(datarec,7);	/* c1179 */
                 glocrec.pet5.attr.fl.bundl.style = pdrintindex(datarec,8);	/* c1179 */
                 glocrec.pet5.attr.fl.bundl.colour= pdrintindex(datarec,9);	/* c1179 */
               }
             }
	    break;

	default:   /* covers cases 1,2,3,6  */
            glocrec.pet1.data = NULL;		/* c1179 */
	    break;
        }

    ginitloc ((Gint) *wkid,(Gint) *lcdnr, &gloc,(Gint) *pet, &glimit, &glocrec);/* c1179 */

}

/*$F
 * ginsk - Initialize Stroke
 *
 * int *wkid;                                    workstation identifier  
 * int *skdnr;                                   stroke device number    
 * int *tnr;                                     initial tranformation # 
 * int *n;                                       # of points in init     
 * float *ipx,*ipy;                              points in initial stroke
 * int *pet;                                     prompt/echo type        
 * float *xmin, *xmax, *ymin, *ymax;             echo area in device coord 
 * int *buflen;                                  input buffer size       
 * int *ldr;                                     dimension of data record
 * char *datarec;                                data record             
 *
 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.74
 */
ginsk_ (wkid,skdnr,tnr,n,ipx,ipy,pet,xmin,xmax,ymin,ymax,buflen,ldr,datarec)
int *wkid;                  
int *skdnr;                
int *tnr;                 
int *n;                  
float *ipx,*ipy;        
int *pet;              
float *xmin, *xmax, *ymin, *ymax;    
int *buflen;                        
int *ldr;                         
char *datarec;                   
{
    Glimit glimit;		/* c1179 */
    Gstrokerec gstrokerec;	/* c1179 */
    Gstroke gstroke;		/* c1179 */
    Gint i;			/* c2054 */

    debug ( ("Initialize stroke wkid = %d dev = %d pet = %d\n", *wkid, *skdnr,*pet) );
    debug ( ("    echo area (%7.2f, %7.2f) (%7.2f, %7.2f) \n", *xmin, *ymin, *xmax, *ymax) );  

    /* check magic num all pets require packed datarecord */

    CHECKMAGICNUM(datarec,errginitstroke);
      
    gstroke.transform = *tnr;		/* c1179 */
    gstroke.n_points = *n; 		/* c1179 */
    glimit.xmin = *xmin;		/* c1179 */
    glimit.xmax = *xmax;		/* c1179 */
    glimit.ymin = *ymin;		/* c1179 */
    glimit.ymax = *ymax;		/* c1179 */
    if (!(gstroke.points = (Gpoint *)malloc (*n * sizeof(Gpoint)) ) ) {	/* c2054 c1179 */
	gerrorhand(300,errginitstroke,(errfp));		/* d1 *//* c2054 */
 	return;								/* c2054 */
	}
    for (i=0;i<*n;i++) {		/* c2054 */
      gstroke.points[i].x = ipx[i];	/* c2054 c1179 */
      gstroke.points[i].y = ipy[i];	/* c2054 c1179 */
      }
    
    switch (*pet)
        {

        case 1: 
             debug(("Case 1 Entered \n"));
             gstrokerec.pet1.bufsiz = *buflen;					/* c1179 */
             if ((pdrintnum(datarec) != 2) || (pdrrealnum(datarec) != 3))	/* c2054 */
 	       { 
 		gerrorhand(2003,errginitstroke,(errfp));	/* d1 */
 		return;
 	       }

	     /* Note: The first integer in the datarec is the bufsize */
	     /*       this is not used because it is a parameter to   */
	     /*       this function.				      */

	     gstrokerec.pet1.editpos = pdrintindex(datarec,2);		/* c1179 */
             gstrokerec.pet1.interval.x  = pdrrealindex(datarec,1);	/* c1179 */
             gstrokerec.pet1.interval.y  = pdrrealindex(datarec,2);	/* c1179 */
             gstrokerec.pet1.time = pdrrealindex(datarec,3);		/* c1179 */
             gstrokerec.pet1.data = NULL;				/* c1179 */
             debug(("Flag : time = %7.2f \n",gstrokerec.pet1.time));
             debug(("Flag : edit interval = %7.2f, %7.2f \n",gstrokerec.pet1.interval.x, gstrokerec.pet1.interval.y));		/* c1179 */
             debug(("Flag : bufsiz = %d \n",gstrokerec.pet1.bufsiz));		/* c1179 */
             debug(("Flag : editpos = %d \n",gstrokerec.pet1.editpos));		/* c1179 */
             break;

	case 2:
             debug(("Case 2 Entered \n"));
             gstrokerec.pet2.bufsiz = *buflen;					/* c1179 */
             if ((pdrintnum(datarec) != 2) || (pdrrealnum(datarec) != 3))	/* c2054 */
 	       { 
 		gerrorhand(2003,errginitstroke,(errfp)); 	/* d1 */
 		return;
 	       }

	     /* Note: The first integer in the datarec is the bufsize */
	     /*       this is not used because it is a paremaeter to  */
	     /*       this function.				      */

	     gstrokerec.pet2.editpos = pdrintindex(datarec,2);		/* c1179 */
             gstrokerec.pet2.interval.x  = pdrrealindex(datarec,1);	/* c1179 */
             gstrokerec.pet2.interval.y  = pdrrealindex(datarec,2);	/* c1179 */
             gstrokerec.pet2.time = pdrrealindex(datarec,3);		/* c1179 */
             gstrokerec.pet2.data = NULL;				/* c1179 */
	     break; 							/* c2049 */

        case 3:
            {
             Gacf attrcontrolflag;
             if ((pdrintnum(datarec) < 3) || (pdrrealnum(datarec) < 3))	/* c2054 */
 	       { 
 		gerrorhand(2003,errginitstroke,(errfp)); 	/* d1 */
 		return;
 	       }
	     /* bufsiz is not removed from the datarecord  */
	     /* the parameter buflen is used */
             gstrokerec.pet3.editpos = pdrintindex(datarec,2);		/* c1179 */
             attrcontrolflag = (Gacf) pdrintindex(datarec,3);
             gstrokerec.pet3.acf = attrcontrolflag;			/* c1179 */
             gstrokerec.pet3.bufsiz = *buflen;				/* c1179 */
             gstrokerec.pet3.data = NULL;				/* c1179 */
             gstrokerec.pet3.interval.x  = pdrrealindex(datarec,1);	/* c1179 */
             gstrokerec.pet3.interval.y  = pdrrealindex(datarec,2);	/* c1179 */
             gstrokerec.pet3.time = pdrrealindex(datarec,3);		/* c1179 */
             if (attrcontrolflag == GSPECIFIED) 
               {
                 if ((pdrintnum(datarec) != 9) || (pdrrealnum(datarec) != 4))
                   {
                     gerrorhand(2003,errginitstroke,(errfp)); 	/* d1 */
                     return;
                   }
                gstrokerec.pet3.mk.type = pdrintindex(datarec,4);	/* c2074 c1179 */
                gstrokerec.pet3.mk.size = pdrintindex(datarec,5);	/* c2074 c1179 */
                gstrokerec.pet3.mk.color = pdrintindex(datarec,6);	/* c2074 c1179 */
                gstrokerec.pet3.mk.mark =  pdrintindex(datarec,7);		/* c1179 */
                gstrokerec.pet3.mk.bundl.type =  pdrintindex(datarec,8);	/* c1179 */
                gstrokerec.pet3.mk.bundl.size =  pdrrealindex(datarec,4);	/* c1179 */
                gstrokerec.pet3.mk.bundl.color =  pdrintindex(datarec,9);	/* c1179 */
               }
            }
            break;
                 
                

	case 4:

	    {
             Gacf attrcontrolflag;
             if ((pdrintnum(datarec) < 3) || (pdrrealnum(datarec) < 3))	/* c2054 c2074 */
 	       { 
 		gerrorhand(2003,errginitstroke,(errfp)); 	/* d1 */
 		return;
 	       }
	     /* bufsiz is not removed from the datarecord  */
	     /* the parameter buflen is used */
             gstrokerec.pet4.bufsiz = *buflen;				/* c1179 */
             gstrokerec.pet4.editpos = pdrintindex(datarec,2);		/* c1179 */
             attrcontrolflag = (Gacf) pdrintindex(datarec,3);
	     gstrokerec.pet4.acf  = attrcontrolflag;	/* c2054 */	/* c1179 */
             gstrokerec.pet4.interval.x  = pdrrealindex(datarec,1);	/* c1179 */
             gstrokerec.pet4.interval.y  = pdrrealindex(datarec,2);	/* c1179 */
             gstrokerec.pet4.time = pdrrealindex(datarec,3);		/* c1179 */
             gstrokerec.pet4.data = NULL;				/* c1179 */
             gstrokerec.pet4.acf = attrcontrolflag;			/* c1179 */
             if (attrcontrolflag == GSPECIFIED) 
               {
                 if ((pdrintnum(datarec) != 9) || (pdrrealnum(datarec) != 4))
                   {
                     gerrorhand(2003,errginitstroke,(errfp)); 	/* d1 */
                     return;
                   }
                 gstrokerec.pet4.ln.type   = pdrintindex(datarec,4);		/* c1179 */
                 gstrokerec.pet4.ln.width  = pdrintindex(datarec,5);		/* c1179 */
                 gstrokerec.pet4.ln.colour = pdrintindex(datarec,6);		/* c1179 */
                 gstrokerec.pet4.ln.line   = pdrintindex(datarec,7);		/* c1179 */
                 gstrokerec.pet4.ln.bundl.type = pdrintindex(datarec,8);	/* c1179 */
                 gstrokerec.pet4.ln.bundl.color = pdrintindex(datarec,9);	/* c1179 */
                 gstrokerec.pet4.ln.bundl.width  = pdrrealindex(datarec,4);	/* c1179 */
                }
	     }
             break;

	default:  /* covers cases 5,6   */
	    debug ( ("    WARNING! unknown pet (%d) specified \n", *pet) );
	    /* return statement removed - c1175 */
        }

    ginitstroke ((Gint) *wkid,(Gint) *skdnr,(Gstroke *) &gstroke,(Gint) *pet,(Glimit *) &glimit,(Gstrokerec *) &gstrokerec);		/* c1179 */

debug ( ("End ginsk \n"));
}

/*$F
 * ginvl - Initialize Valuator
 *
 * int *wkid;                                  workstation identifier 
 * int *vldnr;                                 valuator device #     
 * float *ival;                                initial value        
 * int *pet;                                   prompt/echo type    
 * float *xmin,*xmax,*ymin,*ymax;              echo area in device
 * float *loval,*hival;                        minimal and maximal
 * int *ldr;                                   dim of data record
 * int *datarec;                               data record      
 *
 * Returns: 
 *
 * See also: ANSI standard p.74
 */
ginvl_ (wkid, vldnr,ival,pet,xmin,xmax,ymin,ymax,loval,hival,ldr,datarec)
int *wkid;                               
int *vldnr;                             
float *ival;                           
int *pet;                             
float *xmin,*xmax,*ymin,*ymax;       
float *loval,*hival;                
int *ldr;                          
int *datarec;                     

{
Glimit area;
Gvalrec record;

area.xmin = (Gfloat) *xmin;
area.xmax = (Gfloat) *xmax;
area.ymin = (Gfloat) *ymin;
area.ymax = (Gfloat) *ymax;
switch (*pet)
{
  case 1: 
    record.pet1.low  = (Gfloat) *loval;
    record.pet1.high = (Gfloat) *hival;
    record.pet1.data = NULL;
    break;
  case 2: 
    record.pet2.low  = (Gfloat) *loval;
    record.pet2.high = (Gfloat) *hival;
    record.pet2.data = NULL;
    break;    				/* c2049 */
  case 3: 
    record.pet3.low  = (Gfloat) *loval;
    record.pet3.high = (Gfloat) *hival;
    record.pet3.data = NULL;
    break;
}

debug ( ("Initialize Valuator %d %d \n", *wkid, *vldnr) );
debug ( ("Echo area  xmin = %7.2f xmax =  %7.2f  ymin = %7.2f ymax = %7.2f \n", area.xmin,area.xmax,area.ymin,area.ymax) );
debug ( ("High val = %7.2f Low val =  %7.2f Init val =  %7.2f \n", *hival, *loval,*ival) );

ginitval ((Gint) *wkid,(Gint) *vldnr,(Gfloat) *ival,(Gint) *pet, &area, &record);  /* c2029 */

}


/*$F
 * ginch -  Initialize Choice
 *
 * int *wkid;                                   initialize choice     
 * int *chdnr;                                  choice device number 
 * int *istat;                                  initial status      
 * int *ichnr;                                  initial choice number
 * int *pet;                                    prompt/echo type    
 * float *xmin, *xmax, *ymin, *ymax;            echo area in device
 * int *ldr;                                    dimension of datarec
 * char *datarec;                               data record         
 *
 * Returns: 2000 and 2003, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.74
 */
ginch_ (wkid, chdnr, istat, ichnr, pet, xmin, xmax, ymin, ymax, ldr, datarec)
int *wkid;                                  
int *chdnr;                                
int *istat;                               
int *ichnr;                              
int *pet;                               
float *xmin, *xmax, *ymin, *ymax;      
int *ldr;                             
char *datarec;                       
{
    Gchoice gchoice;		/* c1179 */
    Glimit glimit;		/* c1179 */
    Gchoicerec gchoicerec;	/* c1179 */
    
    debug ( ("Initialize Choice %d %d %d %d %d\n", *wkid, *chdnr, *istat, *ichnr, *pet) );
    debug ( ("    echo area (%7.2f, %7.2f) (%7.2f, %7.2f) \n", *xmin, *ymin, *xmax, *ymax) );        

    INPUTDEVICESTATUS(*istat,errginitchoice);

    switch (*istat)   /* 2040 */
    { 
        case FORT_GNONE: gchoice.status = GC_NONE;		/* c1179 */
         break;
       case FORT_GNCHOI: gchoice.status = GC_NOCHOICE;		/* c1179 */
         break;
       case FORT_GOK: gchoice.status = GC_OK;			/* c1179 */
         break; 
    } 

    gchoice.choice = *ichnr;		/* c1179 */

    glimit.xmin = *xmin;		/* c1179 */
    glimit.xmax = *xmax;		/* c1179 */
    glimit.ymin = *ymin;		/* c1179 */
    glimit.ymax = *ymax;		/* c1179 */

    switch (*pet)
        {
	case 1:  				/* c2049 */
	      gchoicerec.pet1.data = NULL;      /* c2049 c1179 */
	      break;				/* c2054 */
	case 2:
	      CHECKMAGICNUM(datarec,errginitchoice);   		/* c2049 */
	      gchoicerec.pet2.number = pdrintnum(datarec);	/* c1179 */

	      /* point to the array of integers */
	      /* take advantage of C enumerated types */
              if (gchoicerec.pet2.number > 0) 					/* c1179 */
  	        gchoicerec.pet2.enable = (Gprflag *) pdrintaddr(datarec);	/* c1179 */
              else
	        gchoicerec.pet2.enable = NULL;		/* c1179 */
	      gchoicerec.pet2.data = NULL;		/* c1179 */
	      break;

	case 3:
	      CHECKMAGICNUM(datarec,errginitchoice);   /* c2049 */
	      /* number of strings */
	      gchoicerec.pet3.number = pdrstringnum(datarec);			/* c1179 */
	      gchoicerec.pet3.strings = (Gchar **) pdrstringtabptr(datarec);	/* c1179 */
	      gchoicerec.pet3.data = NULL;					/* c1179 */
              break;

        case 4:
	      CHECKMAGICNUM(datarec,errginitchoice);   /* c2049 */
	      /* number of strings */
	      gchoicerec.pet4.number = pdrstringnum(datarec);			/* c1179 */
	      gchoicerec.pet4.strings = (Gchar **) pdrstringtabptr(datarec);	/* c1179 */
	      gchoicerec.pet4.data = NULL;					/* c1179 */
              break;
	case 5:
	      CHECKMAGICNUM(datarec,errginitchoice);   /* c2049 */
	      /* number of strings */
	      gchoicerec.pet5.seg = pdrintindex(datarec,1);		/* c1179 */
	      gchoicerec.pet5.number = pdrintindex(datarec,2);		/* c1179 */
	      gchoicerec.pet5.pickids = pdrintaddr(datarec);		/* c1179 */
	      gchoicerec.pet5.data = NULL;				/* c1179 */
              break;

	default: /* covers cases 1 > pet || pet > 5 */ /* c2049 */
	    debug ( ("    WARNING! unknown pet (%d) specified \n", *pet) );
	    return;
	     }

ginitchoice ((Gint) *wkid,(Gint) *chdnr, &gchoice,(Gint) *pet, &glimit, &gchoicerec);	/* c1179 */


}

/*$F
 * ginpk  -  Initialize Pick
 *
 * int *wkid;                                workstation identifier  
 * int *pkdnr;                               pick device number      
 * int *istat;                               initial status          
 * int *isgna;                               initial segment name    
 * int *ipkid;                               initial pick identifier 
 * int *pet;                                 prompt/echo type       
 * float *xmin,*xmax,*ymin,*ymax;            echo area              
 * int *ldr;                                 dimension of datarec   
 * char *datarec;                            data record           
 *
 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.74
 */
ginpk_ (wkid, pkdnr,istat,isgna,ipkid,pet,xmin,xmax,ymin,ymax,ldr,datarec)
int *wkid;                              
int *pkdnr;                            
int *istat;                           
int *isgna;                          
int *ipkid;                         
int *pet;                          
float *xmin,*xmax,*ymin,*ymax;    
int *ldr;                        
char *datarec;                  

{
Gpick gpick;		/* c1179 */
Glimit glimit;		/* c1179 */
Gpickrec gpickrec;	/* c1179 */

switch (*istat)		/* 2040 */
{
    case FORT_GNONE: gpick.status = GP_NONE;		/* c1179 */
    break;
    case FORT_GNPICK: gpick.status = GP_NOPICK;		/* c1179 */
    break;
    case FORT_GOK: gpick.status = GP_OK;		/* c1179 */
    break;
}
gpick.seg = *isgna;		/* c1179 */
gpick.pickid = *ipkid;		/* c1179 */
glimit.xmin = *xmin;		/* c1179 */
glimit.xmax = *xmax;		/* c1179 */
glimit.ymin = *ymin;		/* c1179 */
glimit.ymax = *ymax;		/* c1179 */
switch(*pet)
 {
  case 1: gpickrec.pet1.data = NULL;		/* c1179 */
          break;
  case 3: gpickrec.pet3.data = NULL;		/* c1179 */
 }

debug ( ("Initialize Pick %d %d \n", *wkid, *pkdnr) );

ginitpick ((Gint) *wkid,(Gint) *pkdnr, &gpick,(Gint) *pet, &glimit, &gpickrec); 	/* c1179 */

}
/*$F
 * ginst  -  Initialize String 
 *
 * int *wkid;                                workstation identifier 
 * int *stdnr;                               string device number    
 * int *lstr;                                length of the init string
 * char *istr;                               initial string      
 * int *pet;                                 prompt/echo type     
 * float *xmin,*xmax,*ymin,*ymax;            echo area             
 * int *buflen;                              buffer length of string
 * int *inipos;                              initial cursor position
 * int *ldr;                                 dimension of datarec   
 * char *datarec;                            data record             
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard
 */
ginst_ (wkid,stdnr,lstr,istr,pet,xmin,xmax,ymin,ymax,buflen,inipos,ldr,datarec)
int *wkid;                           
int *stdnr;                         
int *lstr;                         
char *istr;                       
int *pet;                        
float *xmin,*xmax,*ymin,*ymax;  
int *buflen;                   
int *inipos;                  
int *ldr;                    
char *datarec;              
{ 
Glimit area;
Gstringrec record;

debug ( ("Initialize String  \n") );
    
area.xmin = *xmin;
area.xmax = *xmax;
area.ymin = *ymin;
area.ymax = *ymax;

record.pet1.bufsiz = *buflen;
record.pet1.position = *inipos;
record.pet1.data = NULL;

if (*lstr + 1 > currforttext)       /* Check amount of memory needed */
  {
   realloc(forttext,*lstr + 1); /* realloc more memory */
   if (forttext == NULL)     /* If memory allocation unsuccessful call error routine */
     {
      gerrorhand(300,errgreqstring,(errfp)); 	/* d1 */
      return;
     }
   else
      currforttext = *lstr + 1;
  }

strncpy(forttext,istr,*lstr);
forttext[*lstr] = '\0';

ginitstring ((Gint) *wkid,(Gint) *stdnr,forttext,(Gint) *pet, &area, &record); 
  
}
/*$F
 * ginsts  -  Initialize String  (Subset Version)
 * int *wkid;                                workstation identifier 
 * int *stdnr;                               string device number   
 * int *lstr;                                length of the init string
 * char *istr;                               initial string          
 * int *pet;                                 prompt/echo type       
 * float *xmin,*xmax,*ymin,*ymax;            echo area             
 * int *buflen;                              buffer length of string
 * int *inipos;                              initial cursor position
 * int *ldr;                                 dimension of datarec   
 * char *datarec;                            data record            
 *
 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard
 */
ginsts_ (wkid,stdnr,lstr,istr,pet,xmin,xmax,ymin,ymax,buflen,inipos,ldr,datarec)
int *wkid;                               
int *stdnr;                             
int *lstr;                             
char *istr;                           
int *pet;                            
float *xmin,*xmax,*ymin,*ymax;      
int *buflen;                       
int *inipos;                      
int *ldr;                        
char *datarec;                  
{ 
Glimit area;
Gstringrec record;

debug ( ("Initialize String (Subset)  \n") );
    
area.xmin = *xmin;
area.xmax = *xmax;
area.ymin = *ymin;
area.ymax = *ymax;

record.pet1.bufsiz = *buflen;
record.pet1.position = *inipos;
record.pet1.data = NULL;

if (*lstr + 1 > currforttext)       /* Check amount of memory needed */
  {
   realloc(forttext,*lstr + 1); /* realloc more memory */
   if (forttext == NULL)     /* If memory allocation unsuccessful call error routine */
     {
      gerrorhand(300,errgreqstring,(errfp)); 	/* d1 */
      return;
     }
   else
      currforttext = *lstr + 1;
  }
strncpy(forttext,istr,*lstr);
forttext[*lstr] = '\0';

ginitstring ((Gint) *wkid,(Gint) *stdnr,forttext,(Gint) *pet, &area, &record); 
  
}
