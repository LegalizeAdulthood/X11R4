/* xtparse.y  */

/*
 * Copyright 1989 Jon Bennett, Mike Bolotski, David Gagne, Dan Lovinger
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holders not be used in 
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The copyright holders make no 
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO 
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

%{

#include <X11/Xlib.h>
#include "defs.h"
#include "data.h"

/* Variables global to parser. */

#define NUL  '\0'


       char          *xtrekFile;
static struct planet *pp;
extern struct planet *findplanet();

static aShip  *cship = 0;
static int curemp = 0;
  
%}

%union 
  {
    int          Int;
    char         Char;
    char        *Str;
    double       Real;
  }

/* Lex tokens */

%start xtrekFile

%token   xEND 0

%token   xDEFAULT xEMPIRE xSHIP xPLANET xUNKNOWN

%token  xTORP_DAM       xPHAS_DAM     xPHAS_RANGE  xPHAS_PULSES
%token  xTORP_SPEED     xMAX_SPEED    xSHIELD_REP  xTURNS
%token  xHULL_REP       xMAX_FUEL     xTORP_COST   xPHAS_COST
%token  xMINE_COST      xMINE_DAM     xMINE_DAM_RANGE
%token  xDET_COST       xWARP_COST    xCLOAK_COST  xRECHARGE
%token  xACCEL          xDECEL        xMAX_ARMIES  xWEAP_COOL
%token  xENGINE_COOL    xMAX_DAMAGE   xSHIELD_MAX  xPHAS_FAIL
%token  xTORP_FAIL      xTRANS_FAIL   xSHIELD_FAIL xCLOAK_FAIL
%token  xLR_SENS_FAIL   xSR_SENS_FAIL xLOCK_FAIL   xCOOL_FAIL
%token  xRELOAD         xBURST        xCRUISE_SPD  xHSCRUISE
%token  xFLEE_SPD     xCLOAK_SPD   xBATTLE_SPD
%token  xENGAGE		xDISENGAGE    xSHOTDAMAGE  xCIRCLEDIST
%token	xSNEAKY		xMINCLOAK	xMAXCLOAK
%token  xTORP_DAM_RANGE xREFRESH      xTEL_COST    xTEL_RANGE xTEL_HEAT
%token  xICON           xROBOT_NAME   xTURBO_SPEED xTURBO_TIME xMINE_DETONATE

%token  xDEATHTIME xPFIRETIME xTFIREMIN xTFIREVAR xMFIREMIN
%token             xMFIREVAR xPEXPTIME xPWEAPLOCKMIN xPWEAPLOCKVAR
%token       xPENGLOCKMIN xPENGLOCKVAR xPSELFDESTTIME xRGIVEUPTIME xORBSPEED

%token                   xDETDIST xORBDIST xPFIREDIST xPHITANG
%token          xPHITANGL xAUTOQUIT xCOOL_BONUS xFAST_DESTRUCT

%token   xHOME xFUEL xREPAIR xX xY xARMIES xGLOB

%token   <Int> xINT  xBOOLEAN
%token   <Real> xREAL 
%token   <Str> xSTRING 

%token   xEQ xSEMI xCOMMA


%%

xtrekFile : commands endCommand
          ;

commands : command 
         | commands command
         ;

command     : empire
            | ship
            | planet
            | error
            | unknown
	    | global
            ;

unknown : xUNKNOWN
          {
            printf ("Unknown keyword: %s\n", yytext);
            yyClearError();
          }
        ;

/* 
 *  Empire commands
 */

empire : empireName empireOpts xSEMI
	| empireName xSEMI
       ;

empireName	: xEMPIRE xSTRING 
         	{
                  curemp = addempire($2, NULL);
         	}
       		| xEMPIRE xSTRING xSTRING 
         	{
                  curemp = addempire($2, $3);
         	}
		;

empireOpts 	: empireOpt
		| empireOpts empireOpt
		;	


empireOpt	: xICON xEQ xSTRING 
		{ 
                  strcpy(empires[curemp].iconname, $3); 
		} 
		| xROBOT_NAME xEQ xSTRING
		{
		  strcpy(empires[curemp].robotname, $3);
		}
	        ;


/*
 *  Global commands
 */

global : xGLOB globOpts xSEMI
       ;
       
/*
 *   Ship commands.
 */

ship : shipName shipOpts xSEMI
     ;

shipName : xSHIP xSTRING 
           {
	     cship = findship($2);
             if (!cship) {
                 fprintf (stderr, "empire %s not found\n", $2);
                 exit(1);
               }
           }
	 | xDEFAULT xSHIP
           {
	     cship = findship(NULL);
             if (!cship) {
                 fprintf (stderr, "default ship sanity check failed\n");
                 exit(1);
               }
           }
         ;

shipOpts : shipOpt
         | shipOpts shipOpt
         ;

shipOpt : 	  xTURNS          xEQ xINT { cship->turns         = $3; }   
		| xTORP_DAM       xEQ xINT { cship->torpdamage    = $3; }
		| xTORP_DAM_RANGE xEQ xINT { cship->torpdrange    = $3; }
		| xMINE_DAM       xEQ xINT { cship->minedamage    = $3; }
		| xMINE_DAM_RANGE xEQ xINT { cship->minedrange    = $3; }
		| xPHAS_DAM       xEQ xINT { cship->phaserdamage  = $3; }
		| xPHAS_RANGE     xEQ xINT { cship->phaserdist    = $3; }
		| xPHAS_PULSES    xEQ xINT { cship->ph_pulses     = $3; }
		| xTORP_SPEED     xEQ xINT { cship->torpspeed     = $3; }
		| xMAX_SPEED      xEQ xINT { cship->maxspeed      = $3; }
		| xSHIELD_REP     xEQ xINT { cship->repair        = $3; }
		| xHULL_REP       xEQ xINT { cship->repair        = $3; }
		| xMAX_FUEL       xEQ xINT { cship->maxfuel       = $3; }
		| xWARP_COST      xEQ xINT { cship->warpcost      = $3; }
		| xTORP_COST      xEQ xINT { cship->cost[(int)Torp]    = $3; }
		| xMINE_COST      xEQ xINT { cship->cost[(int)Mine]    = $3; }
		| xPHAS_COST      xEQ xINT { cship->cost[(int)Phaser]  = $3; }
		| xDET_COST       xEQ xINT { cship->cost[(int)Detonate]= $3; }
		| xCLOAK_COST     xEQ xINT { cship->cost[(int)Cloak]   = $3; }
		| xTEL_COST       xEQ xINT { cship->cost[(int)Teleport]= $3; }
		| xTEL_HEAT       xEQ xINT { cship->telheat            = $3; }
		| xTURBO_SPEED    xEQ xINT { cship->turbospeed         = $3; }
		| xTURBO_TIME     xEQ xINT { cship->turbotime          = $3; }
		| xRECHARGE       xEQ xINT { cship->recharge           = $3; }
		| xACCEL          xEQ xINT { cship->accint             = $3; }
		| xDECEL          xEQ xINT { cship->decint             = $3; }
		| xMAX_ARMIES     xEQ xINT { cship->maxarmies          = $3; }
		| xWEAP_COOL      xEQ xINT { cship->wcool              = $3; }
		| xENGINE_COOL    xEQ xINT { cship->ecool              = $3; }
		| xMAX_DAMAGE     xEQ xINT { cship->maxdamage          = $3; }
		| xSHIELD_MAX     xEQ xINT { cship->maxshields         = $3; }
		| xTEL_RANGE      xEQ xINT { cship->telrange           = $3; }
		| xPHAS_FAIL      xEQ xINT { cship->fail[(int)Phaser]  = $3; }
		| xTORP_FAIL      xEQ xINT { cship->fail[(int)Torp]    = $3; }
		| xTRANS_FAIL     xEQ xINT { cship->fail[(int)Trans]   = $3; }
		| xSHIELD_FAIL    xEQ xINT { cship->fail[(int)Shield]  = $3; }
		| xCLOAK_FAIL     xEQ xINT { cship->fail[(int)Cloak]   = $3; }
		| xLR_SENS_FAIL   xEQ xINT { cship->fail[(int)LRS]     = $3; }
		| xSR_SENS_FAIL   xEQ xINT { cship->fail[(int)SRS]     = $3; }
		| xLOCK_FAIL      xEQ xINT { cship->fail[(int)Lock]    = $3; }
		| xCOOL_FAIL      xEQ xINT { cship->fail[(int)Cooling] = $3; }
		| xRELOAD         xEQ xINT { cship->reload        = $3; }
		| xBURST          xEQ xINT { cship->burst         = $3; }
		| xCRUISE_SPD     xEQ xINT { cship->cruise        = $3; }
		| xHSCRUISE       xEQ xINT { cship->hscruise      = $3; }
		| xBATTLE_SPD     xEQ xINT { cship->battle        = $3; }
		| xFLEE_SPD       xEQ xINT { cship->flee          = $3; }
		| xCLOAK_SPD      xEQ xINT { cship->cloaked       = $3; }
		| xENGAGE	  xEQ xINT { cship->engage        = $3; }
		| xDISENGAGE      xEQ xINT { cship->disengage     = $3; }
		| xSHOTDAMAGE     xEQ xINT { cship->shotdamage    = $3; }
                | xSNEAKY	  xEQ xINT { cship->sneaky	  = $3; }
                | xMINCLOAK	  xEQ xINT { cship->mincloak	  = $3; }
                | xMAXCLOAK	  xEQ xINT { cship->maxcloak	  = $3; }
		| xCIRCLEDIST     xEQ xINT { cship->circledist    = $3; }
		| xREFRESH        xEQ xINT { cship->refresh       = $3; }
		;



/*
 *  Planet stuff.
 */

planet : planetName planetOpts xSEMI
       ;

planetName : xPLANET xSTRING xSTRING
	     {
               addplanet($2, $3);
               pp = findplanet($2);
               if (pp == NULL)
                 {
                   fprintf (stderr, "planet %s not found\n", $2);
                   exit(1);
                 }
	     }
           | xDEFAULT xPLANET
             {
                pp = findplanet(NULL);
                if (pp == NULL)
                  {
                    fprintf (stderr, "default planet not found\n");
                    exit(1);
                  }
	      }
	    ;

planetOpts : planetOpt
           | planetOpts planetOpt
           ;

        
planetOpt :	   xHOME                { pp->pl_flags |= PLHOME; } 
		|  xREPAIR              { pp->pl_flags |= PLREPAIR; } 
		|  xFUEL                { pp->pl_flags |= PLFUEL; } 
		|  xX      xEQ  xINT { pp->pl_x = $3;       } 
		|  xY      xEQ  xINT { pp->pl_y = $3;       } 
		|  xARMIES xEQ  xINT { pp->pl_armies = $3;  } 
		;

globOpts : globOpt
         | globOpts globOpt
         ;

globOpt   	: xDEATHTIME xEQ xINT  { DEATHTIME = $3; }
		| xPFIRETIME xEQ xINT  { PFIRETIME = $3; }
		| xTFIREMIN xEQ xINT  { TFIREMIN = $3; }
		| xTFIREVAR xEQ xINT  { TFIREVAR = $3; }
		| xMFIREMIN xEQ xINT  { MFIREMIN = $3; }
		| xMFIREVAR xEQ xINT  { MFIREVAR = $3; }
		| xPEXPTIME xEQ xINT  { PEXPTIME = $3; }
		| xPWEAPLOCKMIN xEQ xINT  { PWEAPLOCKMIN = $3; }
		| xPWEAPLOCKVAR xEQ xINT  { PWEAPLOCKVAR = $3; }
		| xPENGLOCKMIN xEQ xINT  { PENGLOCKMIN = $3; }
		| xPENGLOCKVAR xEQ xINT  { PENGLOCKVAR = $3; }
		| xPSELFDESTTIME xEQ xINT  { PSELFDESTTIME = $3; }
		| xRGIVEUPTIME xEQ xINT  { RGIVEUPTIME = $3; }
		| xORBSPEED xEQ xINT  { ORBSPEED = $3; }
		| xDETDIST xEQ xINT  { DETDIST = $3; }
		| xORBDIST xEQ xINT  { ORBDIST = $3; }
		| xPFIREDIST xEQ xINT  { PFIREDIST = $3; }
		| xPHITANG xEQ xINT  { PHITANG = $3; }
		| xPHITANGL xEQ xINT  { PHITANGL = $3; }
		| xAUTOQUIT xEQ xINT  { AUTOQUIT = $3; }
		| xCOOL_BONUS xEQ xREAL  { g_coolbonus = $3; }
		| xFAST_DESTRUCT { g_fastdestruct = 1; }
		| xMINE_DETONATE { g_minedetonate = 1; }
		;


/*
 *  END
 */

endCommand : xEND
             {
                fprintf (stderr, "Read configuration\n"); 
             }
           ;


%%

#include  "xttoken.c"

/**************************************************************************** 
*
* yyerror()
*
****************************************************************************/

yyerror(str)

  char  *str;

  {

    fflush (stdout);
    fprintf (stderr, "%s: line #%d: %s near %s\n", 
             xtrekFile, yylineno, str, yytext);
    fflush (stderr);

    yyClearError();
  }

/**************************************************************************** 
 *
 * yyClearError()
 *
 ****************************************************************************/

yyClearError()

  {
    char  ch;

    ch = input();
    while ( (ch != NUL) && (ch != ';') )
        ch = input();

    if (ch == ';')
      {
        fflush(stdout);
        fprintf (stderr, "  -- ignoring command.\n");
        fflush(stderr);
      }
    else
      {
        fflush(stdout);
        fprintf (stderr, "  AARRGHH -- Could not recover from errors.\n");
        fflush(stderr);
        exit(1);
      }
    yyclearin;
    yyerrok;
  }


/**************************************************************************** 
 *
 * yywrap()
 *
 ****************************************************************************/

yywrap()
  {
    return(1);
  }

