%Start  CODE SHIP COMM EMP GLOB PLA
%p 5500
%e 10000
%n 1000

le     [a-zA-Z]
as     [a-zA-Z0-9]
di     [0-9]
nl     \n
ex     [eE]

%%
	/* Version 2.0 : new and improved */

    static int savestate;
    static char first = 1;
    if (first)
      {
        BEGIN CODE;
        first = 0;
      }

<CODE>default   {             return(xDEFAULT); }
<CODE>empire    { BEGIN EMP; return(xEMPIRE);  }
<CODE>ship      { BEGIN SHIP; return(xSHIP);    }
<CODE>planet    { BEGIN PLA; return(xPLANET);  }
<CODE>global    { BEGIN GLOB; return(xGLOB);  }

<CODE,SHIP,EMP,GLOB,PLA>"//".*    { ; }
<CODE>"/*"      { savestate = CODE; BEGIN COMM; }
<SHIP>"/*"      { savestate = SHIP; BEGIN COMM; }
<EMP>"/*"      { savestate = EMP; BEGIN COMM; }
<GLOB>"/*"      { savestate = GLOB; BEGIN COMM; }
<PLA>"/*"      { savestate = PLA; BEGIN COMM; }

<COMM>"*/"      { BEGIN savestate; }
<COMM>.        { ; }
<COMM>\n        { ; }

<CODE,PLA,EMP,SHIP,GLOB>;            		{ BEGIN CODE; return(xSEMI);  }
<CODE,PLA,EMP,SHIP,GLOB>\,            		{ return xCOMMA;  }

<CODE,PLA,EMP,SHIP,GLOB>=            		{ return(xEQ);  }

<SHIP>turns                       { return(xTURNS);          }
<SHIP>torp-damage                 { return(xTORP_DAM);       }
<SHIP>torp-blast-range           { return(xTORP_DAM_RANGE); }
<SHIP>mine-damage                 { return(xMINE_DAM);       }
<SHIP>mine-blast-range           { return(xMINE_DAM_RANGE); }
<SHIP>phaser-damage               { return(xPHAS_DAM);       }
<SHIP>phaser-range                { return(xPHAS_RANGE);     }
<SHIP>ph-pulses                   { return(xPHAS_PULSES);    }
<SHIP>torp-speed                  { return(xTORP_SPEED);     }
<SHIP>max-speed                   { return(xMAX_SPEED);      }
<SHIP>shield-repair               { return(xSHIELD_REP);     }
<SHIP>hull-repair                 { return(xHULL_REP);       }
<SHIP>max-fuel                    { return(xMAX_FUEL);       }
<SHIP>torp-cost                   { return(xTORP_COST);      }
<SHIP>mine-cost                   { return(xMINE_COST);      }
<SHIP>phaser-cost                 { return(xPHAS_COST);      }
<SHIP>det-cost                    { return(xDET_COST);       }
<SHIP>warp-cost                   { return(xWARP_COST);      }
<SHIP>cloak-cost                  { return(xCLOAK_COST);     }
<SHIP>recharge                    { return(xRECHARGE);       }
<SHIP>accint                      { return(xACCEL);          }
<SHIP>decint                      { return(xDECEL);          }
<SHIP>max-armies                  { return(xMAX_ARMIES);     }
<SHIP>weapon-cool                 { return(xWEAP_COOL);      }
<SHIP>engine-cool                 { return(xENGINE_COOL);    }
<SHIP>max-damage                  { return(xMAX_DAMAGE);     }
<SHIP>shield-max                  { return(xSHIELD_MAX);     }
<SHIP>teleport-heat               { return(xTEL_HEAT);       }
<SHIP>teleport-cost               { return(xTEL_COST);       }
<SHIP>teleport-range              { return(xTEL_RANGE);      }
<SHIP>turbo-speed                 { return(xTURBO_SPEED);    }
<SHIP>turbo-time                  { return(xTURBO_TIME);     }
<SHIP>phaser-fail                 { return(xPHAS_FAIL);      }
<SHIP>torp-fail                   { return(xTORP_FAIL);      }
<SHIP>trans-fail                  { return(xTRANS_FAIL);     }
<SHIP>shield-fail                 { return(xSHIELD_FAIL);    }
<SHIP>cloak-fail                  { return(xCLOAK_FAIL);     }
<SHIP>lrsensor-fail               { return(xLR_SENS_FAIL);   }
<SHIP>srsensor-fail               { return(xSR_SENS_FAIL);   }
<SHIP>lock-fail                   { return(xLOCK_FAIL);      }
<SHIP>cooling-fail                { return(xCOOL_FAIL);      }
<SHIP>reload                      { return(xRELOAD);         }
<SHIP>burst                       { return(xBURST);          }
<SHIP>hscruise                    { return(xHSCRUISE);       }
<SHIP>cruise                      { return(xCRUISE_SPD);     }
<SHIP>battle                      { return(xBATTLE_SPD);     }
<SHIP>engage                      { return(xENGAGE);         }
<SHIP>disengage                   { return(xDISENGAGE);      }
<SHIP>shotdamage                  { return(xSHOTDAMAGE);     }
<SHIP>circledist                  { return(xCIRCLEDIST);     }
<SHIP>sneaky                      { return(xSNEAKY);         }
<SHIP>max-cloak                   { return(xMAXCLOAK);       }
<SHIP>min-cloak                   { return(xMINCLOAK);       }
<SHIP>flee                        { return(xFLEE_SPD);       }
<SHIP>cloaked                     { return(xCLOAK_SPD);      }
<SHIP>refresh                     { return(xREFRESH);        }

<EMP>icon                        { return(xICON);           }
<EMP>robotname                   { return(xROBOT_NAME);     }

<PLA>home                        { return(xHOME);           }
<PLA>fuel                        { return(xFUEL);           }
<PLA>repair                      { return(xREPAIR);         }
<PLA>x                           { return(xX);              }
<PLA>y                           { return(xY);              }
<PLA>armies                      { return(xARMIES);         }


<GLOB>death-time                { return xDEATHTIME;   }
<GLOB>phaser-fire-time            { return xPFIRETIME;   }
<GLOB>torp-life-min            { return xTFIREMIN;   }
<GLOB>torp-life-var            { return xTFIREVAR;   }
<GLOB>mine-life-min            { return xMFIREMIN;   }
<GLOB>mine-life-var            { return xMFIREVAR;   }
<GLOB>player-explode-time      { return xPEXPTIME;   }
<GLOB>weapon-lock-min      { return xPWEAPLOCKMIN;   }
<GLOB>weapon-lock-var      { return xPWEAPLOCKVAR;   }
<GLOB>engine-lock-min      { return xPENGLOCKMIN;   }
<GLOB>engine-lock-var      { return xPENGLOCKVAR;   }
<GLOB>self-destruct-time      { return xPSELFDESTTIME;   }
<GLOB>robot-giveup-time      { return xRGIVEUPTIME;   }
<GLOB>orbit-speed		{ return xORBSPEED;   }

<GLOB>det-dist			{ return xDETDIST;   }
<GLOB>orbit-dist		{ return xORBDIST;   }
<GLOB>planet-fire-dist		{ return xPFIREDIST;   }
<GLOB>phaser-hit-angle		{ return xPHITANG;   }
<GLOB>phaser-hit-langle		{ return xPHITANGL;   }
<GLOB>auto-quit			{ return xAUTOQUIT;   }

<GLOB>cool-penalty                { return xCOOL_BONUS;   }
<GLOB>fast-destruct             { return xFAST_DESTRUCT;  }
<GLOB>mine-detonate          { return xMINE_DETONATE;  }

<CODE,PLA,EMP,SHIP,GLOB>true		{ yylval.Int = 1; return xBOOLEAN; }
<CODE,PLA,EMP,SHIP,GLOB>false		{ yylval.Int = 0; return xBOOLEAN; }


<CODE,PLA,EMP,SHIP,GLOB>[-+]?{di}+     { sscanf(yytext, "%d", &yylval.Int);
				  return(xINT);        }
<CODE,PLA,EMP,SHIP,GLOB>[-+]?{di}+"."{di}*({ex})? |
<CODE,PLA,EMP,SHIP,GLOB>[-+]?{di}*"."{di}+({ex})? |
<CODE,PLA,EMP,SHIP,GLOB>[-+]?{di}+{ex}             {   sscanf (yytext, "%lf", &yylval.Real);
                                    return(xREAL);} 

<CODE,PLA,EMP,SHIP,GLOB>{le}({le}|{di})*		{ yylval.Str = malloc(strlen(yytext)+1);
                                  strcpy(yylval.Str, yytext);
                                  return(xSTRING);         
                                 }
<CODE,PLA,EMP,SHIP,GLOB>\"[^\"]+\"   { int len = strlen(yytext);
                                       yylval.Str = malloc(len);
                                       strcpy(yylval.Str, yytext+1);
                                       yylval.Str[len-2] = 0;
                                       return xSTRING;}

<CODE,PLA,EMP,SHIP,GLOB>[\n\t ]			;


%%
