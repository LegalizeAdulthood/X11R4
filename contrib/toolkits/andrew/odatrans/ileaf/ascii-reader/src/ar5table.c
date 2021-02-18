
/*
 * Copyright 1989 by Interleaf, Inc, 10 Canal Park,  Cambridge, MA. 02141
 *
 *		All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Interleaf not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Interleaf, Inc. makes no representations 
 * about the suitability of this software for any purpose. 
 * It is provided "AS IS" without express or implied warranty.
 *
 * INTERLEAF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL INTERLEAF BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * 
 */

/*************************************************/
/*	ar5table.c                               */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  12/88    BT	Original AR version, derived from ld sources
 */

#ifdef INTERLEAF_INCLUDE_FILES
#include "cdefs.h"
#include "sysstdio.h"
#else
#ifndef AR_DEFINES
#define AR_DEFINES
typedef short bool;
typedef short rsu;
typedef long Rsu;
typedef short shar;
typedef unsigned short ushar;
#endif
#endif
#include "ar5defs.h"
#include "ar5private.h"
/**--------------------------------------------------------------------------**/
extern bool ArCellFree();
extern bool ArRowFree();
/**--------------------------------------------------------------------------**/
/**	Table Build functions.						     **/
/**--------------------------------------------------------------------------**/
/* these keywords must be preceeded by "Column n" */
#define LD_TBL_WIDTH	1
#define LD_TBL_COL_RULE_COLOR	2
#define LD_TBL_COL_RULE_PATTERN	3
#define LD_TBL_COL_RULE_WIDTH	4
#define LD_TBL_COL_RULE_VIS	5

#define LD_TBL_ALT_TINT 6
#define LD_TBL_BRKAFTER 7
#define LD_TBL_BRKWITHIN 8
#define LD_TBL_MARGIN	 9
#define LD_TBL_BORDER	10	/* old, to be removed */
#define LD_TBL_NEWPAGE	11
#define LD_TBL_ORPH_CNTL 12
#define LD_TBL_NEWCOLUMN 13
#define LD_TBL_STRADDLE	14
#define LD_TBL_WID_CNTL	16
#define LD_TBL_ROWNAME	17
#define LD_TBL_COLUMNS	18
#define LD_TBL_BORDER_COLOR	19
#define LD_TBL_BORDER_PATTERN	20
#define LD_TBL_BORDER_WIDTH	21
#define LD_TBL_BORDER_VIS	22
/* #define LD_TBL_NAME		23 unused */
#define LD_TBL_MILRUL		24
#define LD_TBL_APAGE		25

struct keytab al_table_tokens[] = {  /* this list must remain sorted */
    /* NOTE: any entries that start out "Left/Right/Top/Bottom/Header/Footer"
     * are handled specially. */
    {"A-Page",LD_TBL_APAGE},
    {"Allow Page Break After", LD_TBL_BRKAFTER},
    {"Allow Page Break Within", LD_TBL_BRKWITHIN},
    {"Alt Tint", LD_TBL_ALT_TINT},	/* TEMP HACK */
    {"Begin New Column",LD_TBL_NEWCOLUMN},
    {"Begin New Page",LD_TBL_NEWPAGE},
    {"Border", LD_TBL_BORDER},
    {"Border Color", LD_TBL_BORDER_COLOR},
    {"Border Pattern", LD_TBL_BORDER_PATTERN},
    {"Border Visible", LD_TBL_BORDER_VIS}, 
    {"Border Weight", LD_TBL_BORDER_WIDTH},
    {"Border Width", LD_TBL_BORDER_WIDTH},
    {"Columns", LD_TBL_COLUMNS},
    {"Margin",LD_TBL_MARGIN},			/* has top, left etc prefix */
    {"New Page",LD_TBL_NEWPAGE},
    {"Orphan Control",LD_TBL_ORPH_CNTL},
    {"Page Break Rulings",LD_TBL_MILRUL},
    {"Row",LD_TBL_ROWNAME},
    {"Ruling Color", LD_TBL_COL_RULE_COLOR},
    {"Ruling Pattern", LD_TBL_COL_RULE_PATTERN},
    {"Ruling Visible", LD_TBL_COL_RULE_VIS},
    {"Ruling Weight", LD_TBL_COL_RULE_WIDTH},
    {"Ruling Width", LD_TBL_COL_RULE_WIDTH},
    {"Straddle",LD_TBL_STRADDLE},
    {"Widow Control",LD_TBL_WID_CNTL},
    {"Width", LD_TBL_WIDTH}		/* column width */
};

/* Ruling syntax for table borders:
 *	Left    Border  Width	= n <double>
 *	Right		Pattern = n
 *	Top		Color	= n <or transparent (OBSOLETE)>
 *	Bottom		Visible = yes/no
 *	Header
 *	Footer
 *  
 * NOTE: default table borders come from the master table.
 *
 * Ruling syntax for table columns:
 *	Column N Left   Ruling Width   = n <double>
 *		 Top	       Pattern = n
 *			       Color   = n <or transparent (OBSOLETE)>
 *			       Visible = yes/no
 *
 * NOTE: default column rulings come from the master table.
 * But if the instance has more columns than the master, default
 * values (from al_table_default_rulings) are used.
 */

#define	BMARGIN		(0)
#define	FONT		(1+BMARGIN)
#define	NEWPAGE		(1+FONT)
#define	NEWCOLUMN	(1+NEWPAGE)
#define	TMARGIN		(1+NEWCOLUMN)
#define	BRKAFTER	(1+TMARGIN)
#define	TBL_FOOTER	(1+BRKAFTER)
#define	TBL_HEADER	(1+TBL_FOOTER)
#define	ROW_APAGE	(1+TBL_HEADER)
#define	ROW_HIDDEN	(1+ROW_APAGE)

struct keytab al_row_tokens[] = {  /* this list must remain sorted */
    {"A-Page", ROW_APAGE},
    {"Allow Break After", BRKAFTER},
    {"Allow Page Break After", BRKAFTER},
    {"Begin New Column", NEWCOLUMN},
    {"Begin New Page",NEWPAGE},
    {"Bottom Margin",BMARGIN},
    {"Font",FONT},
    {"Footer",TBL_FOOTER},		/* table row only */
    {"Header",TBL_HEADER},		/* table row only */
    {"Hidden", ROW_HIDDEN},
    {"New Column", NEWCOLUMN},
    {"New Page",NEWPAGE},
    {"Top Margin",TMARGIN},
};

/**--------------------------------------------------------------------------**/
static void tableColsSetup(tblP, current_col)
  Table_t *tblP;
  int	   current_col;
{
  register Table_column_t *tcolP  = NULL;
  register Table_column_t *trailP = NULL;
  register int 		   n	  = 1;

    if (tblP == NULL || current_col <= tblP->num_columns) {
	return;
    }
    else if (current_col == 1 && tblP->column_infoP == NULL) {
	/** starting a table from scratch. **/
	/** ArFprintf(stderr, "tCS(%d) starting a table from scratch.\n", BT **/
			/** Identify(tblP)); BT **/
	tblP->column_infoP = 
		(Table_column_t *)ArSafeMalloc(sizeof(Table_column_t));
	*tblP->column_infoP = default_table_column;
	tblP->column_infoP->num_column = 1;
	tblP->num_columns = 1;
    }
    else {
	for (tcolP = tblP->column_infoP; 
	    tcolP != NULL; 
	    tcolP = tcolP->nextP) {
		n = tcolP->num_column;
		trailP = tcolP;
		if (n == current_col) {
		    /** assert: things are messed up.  **/
		    ArFprintf(stderr,
			"tableColsSetup(): internal error at %d\n", n);
		    return;
		}
	}
	/** assert: 
	    tcolP == NULL && trailP == last real struct && n == last real column
	**/
	for ( ; ++n <= current_col; ) {
	    tcolP = (Table_column_t *)ArSafeMalloc(sizeof(Table_column_t));
	    *tcolP = default_table_column;
	    tcolP->num_column = n;
	    trailP->nextP = tcolP;
	    trailP = trailP->nextP;
	}
	tblP->num_columns = current_col;

    }
}
/**--------------------------------------------------------------------------**/
Table_column_t *tableColumnN(tblP, col)
  Table_t *tblP;
  int      col;
{
  Table_column_t *tcolP = NULL;
  int i;

    if (col >= 1 && col <= tblP->num_columns) {
	for (i=1,tcolP = tblP->column_infoP; 
	    tcolP != NULL; 
	    ++i,tcolP = tcolP->nextP) {
		if (tcolP->num_column == col) {
		    break;
		}
	}
    }
    if (tcolP == NULL) {
	ArFprintf(stderr, "tableColumnN() internal error: tcolP == NULL!\n");
    }
    return( tcolP );
}

/**--------------------------------------------------------------------------**/
    /* If the markup says "Weight = ", "Color = " or "Pattern = "
     * then we assume the thing is visible unless we see "Visible = no"
     */

ArGetRulingPattern(fiP, ruleP)
  File_info_t *fiP;
  Table_rule_t *ruleP;
{
    int temp;

    if(ArGetInt(fiP, &temp, NO, NO) && temp >= 0 && temp <= 255) 
	ruleP->pattern = temp;
    /** ruleP->flags &= ~RULFTRANS; 					    **/
    /** I assume visible is true by default.				    **/
    /** and the boolean sets "visible", the normal case, not "transparent", **/
    /** the abnormal case.						    **/
}

/**--------------------------------------------------------------------------**/
ArGetRulingColor(fiP, ruleP)
  File_info_t *fiP;
  Table_rule_t *ruleP;
{
    int temp;

    if (ArGetInt(fiP, &temp, NO, NO) && temp >= 0 && temp <= 255) {
	ruleP->color = temp;
    }
}

/**--------------------------------------------------------------------------**/
ArGetRulingWidth(fiP, ruleP)
  File_info_t *fiP;
  Table_rule_t *ruleP;
{
    int temp;

    /* ENHANCE limit check and conversion? */
    if(ArGetInt(fiP, &temp, NO, NO) && temp >= 0 && temp <= 6)  {
	ruleP->width = temp;
    }
    if ((temp = ArGetchNW(fiP)) != COMMA) {
	ArGetWordShrC(fiP, temp, al_token_shr, AR_MAX_WORD, COMMA);
	if (ArStrncmpShr(al_token_shr, "Double", 6, LOOSE) == 0)
	    ruleP->double_rule = YES;
    }
}

/**--------------------------------------------------------------------------**/
ArGetRulingVis(fiP, ruleP)
  File_info_t *fiP;
  Table_rule_t *ruleP;
{
    short tempshort = TRUE;		/* if ERR_YESNO assume true */
    
    ArGetYesNoShort(fiP, &tempshort);
    ruleP->visible = tempshort;
}

/**--------------------------------------------------------------------------**/
/**	NOTE	ArParseCell(fiP, cellP) is in ar5frameread.c, since a cell   **/
/**		is very much like a frame.				     **/
/**--------------------------------------------------------------------------**/
/**--------------------------------------------------------------------------**/
/**  Build a Cell.  **/

Cell_t *ArBuildCell(fiP, ilP)
  File_info_t *fiP;
  Il_state_t *ilP;
{
  shar       c       = 0;
  Cell_t    *cellP   = NULL;
  NullObj_t *thingP  = NULL;
  bool	     done    = NO;
  int	     tempint = 0;

    if (fiP == NULL) {
	return( NULL );
    }

    cellP = (Cell_t *)ArSafeMalloc(sizeof(Cell_t));
    *cellP = default_table_cell;
    SET_NullObjFields(cellP, ID_Cell);

    if (ArParseCell(fiP, cellP) == NO) {
	ArCellFree(cellP);
	ArFprintf(stderr, "ArBuildCell(): error parsing Cell.\n");
	return( NULL );
    }
    /**----------------------------------------------------------------------**/
    /**	now get the contents, if any.					     **/
    /**----------------------------------------------------------------------**/
    for (done = NO; done == NO && (c = ArGetchNW(fiP)) != EOF; ) {
	if (c == AR_CMD_CHAR) {
	    if ((c = ArGetch(fiP)) == AR_CMD_CHAR) {
		/** it is '<<' and doesn't belong here. **/
		ArFprintf(stderr, 
		   "ArBuildCell(): unexpected '<<' in text. Ignoring it.\n",
		       c);
	    }
	    else if (c == AR_DECL_CHAR) {
		/** it had better be a comment. **/
		ArUnGetch(AR_DECL_CHAR, fiP);
		ArUnGetch(AR_CMD_CHAR, fiP);
		thingP = ArNextObject(fiP, ilP);
		if (Identify(thingP) != ID_Comment) {
		    ArFprintf(stderr, 
       "ArBuildCell(): unexpected declaration %d in table text. Ignoring it.\n",
			   Identify(thingP));
		    ArObjectFree(thingP);
		}
		else {
		    ListAddatEnd(&cellP->cellbodyP, thingP);
		}
		continue;
	    }
	    else if (c == AR_HEX_CHAR) {
		/** it is '<#xxxx>' and doesn't belong here. **/
		ArFprintf(stderr, 
		   "ArBuildCell(): unexpected <#xxxx> command. Ignoring it.\n");
		ArSkipPastTerm(fiP);
		continue;
	    }
	    else {
		/** it is a Header or Footer, a cmpn instance, (quoted or not)
		or a markup command. We really hope it is a component instance,
		being one of possibly several components in a cell.
		If we see a Cell, a Frame, a MasterRow, a Row, or an 
		EndTable command, it terminates this cell and gets pushed back.
		BECAUSE THIS IS 5.0 BASED, each component instance will have
		an explicit tag.  Any other command is an error.  double-
		newline does NOT imply another component identical to the
		previous one.
		**/
		if (ArGetWordShrC(fiP,c,al_token_shr,AR_MAX_WORD-1,COMMA) ==0
		    || (tempint = ArIsCommandName(fiP,al_token_shr,LOOSE))==-1){
		    /** it is not a recognized command keyword.  It must     **/
		    /** be a component instance.			     **/
		    thingP = ArBuildCmpnName(fiP, al_token_shr, NO, NULL, ilP);
		    if (Identify(thingP) == ID_Cmpn) {
			ListAddatEnd(&cellP->cellbodyP, thingP);
		    }
		    else {
			ArObjectFree(thingP);
		    }
		    for ( thingP = arObjectPreFetch();  /* Boy is this ugly, */
		          thingP != NULL;		/* and maybe even    */
		          thingP = arObjectPreFetch() ){/* wrong.  MS        */
		       if (Identify(thingP) == ID_Cmpn) {
		          ListAddatEnd(&cellP->cellbodyP, thingP);
		       }else{
			  ArObjectFree(thingP);
		       }
		    }
		    continue;
		}
		/** so now it is at least a command. Is it one we want? **/
		switch (tempint) {
		case AL_CELL:
		case LD_FRAME:
		case AL_MASTER_ROW:
		case AL_ROW:
		case END_TABLE:
		    /** push back the command for ArBuildTable to find. **/
		    ArUnGetch(COMMA, fiP);
		    ArUnGetshr(al_token_shr, fiP);
		    ArUnGetch(AR_CMD_CHAR, fiP);
		    done = YES;	/** this will break us out of the loop. **/
		    break;
		default:
		    ArFprintf(stderr, 
			    "ArBuildCell(): command %Q not valid in table.\n", 
			    al_token_shr);
		    ArSkipPastTerm(fiP);
		    continue;
		    break;
		}
	    }
	}
	else {
	    /** fucked up.  text chars must follow a command. **/
	    ArFprintf(stderr, 
		   "ArBuildCell(): unexpected plain text %#C.  Ignoring it.\n",
		   c);
	}
    }
    return( cellP );
}
/**--------------------------------------------------------------------------**/
bool
ArParseRow(fiP, rowP)
  File_info_t *fiP;
  register Row_t* rowP;
{
    register shar c;

    while ((c = ArGetchNW(fiP)) != AR_CMD_TERM && c != EOF) {
	if (c == '@') {		/* indicates a component attribute */
	    arParseUserAttr(fiP, &rowP->attrvalsP);
	    continue;
	}
	else {
	    ArUnGetch(c, fiP);
	}
        switch(ArGetTokenEq(fiP, al_row_tokens, NUM_OF(al_row_tokens))) {
	case NO_EQUALS:
	    break;
	case BMARGIN:
	    ArGetPZRsu(fiP, &rowP->bottom_margin);
	    break;
        case TMARGIN :
	    ArGetPZRsu(fiP, &rowP->top_margin);
	    break;
	case FONT:
	    { Font_value_t my_val;

		if (ArFontGetValue(fiP, &my_val) == YES) {
		    rowP->font = my_val; 
		}
		ArSkipPastComma(fiP);	/** always do this. **/
	    }
	    break;
	case ROW_HIDDEN:
	    { short temp;
		if (ArGetYesNoShort(fiP, &temp) == YES) {
		    rowP->flags.hidden = temp;
		}
	    }
	    break;
	case NEWPAGE:
	    { short temp;
		if (ArGetYesNoShort(fiP, &temp) == YES) {
		    rowP->flags.new_page = temp;
		}
	    }
	    break;
	case NEWCOLUMN:
	    { short temp;
		if (ArGetYesNoShort(fiP, &temp) == YES) {
		    rowP->flags.new_column = temp;
		}
	    }
	    break;
	case BRKAFTER:
	    { short temp;
		if (ArGetYesNoShort(fiP, &temp) == YES) {
		    rowP->flags.pbreak_after = temp;
		}
	    }
	    break;
	case TBL_HEADER:
	    { short temp;
		if (ArGetYesNoShort(fiP, &temp) == YES) {
		    rowP->flags.is_header = temp;
		}
	    }
	    break;
	case TBL_FOOTER:
	    { short temp;
		if (ArGetYesNoShort(fiP, &temp) == YES) {
		    rowP->flags.is_footer = temp;
		}
	    }
	    break;
	case ROW_APAGE:
	    { short temp;
		if (ArGetYesNoShort(fiP, &temp) == YES) {
		    rowP->flags.A_page = temp;
		}
	    }
	    break;
        default      :
	    rowP->unkP = 
	       (Unknown_t *)ArBuildUnknownPropEq(fiP, al_token_shr, rowP->unkP);
	    ArSkipPastComma(fiP);
	    break;
	}
    }
    return( YES );
}
/**--------------------------------------------------------------------------**/
/**  Build a MasterRow or a Row.  tblP == NULL implies this is a MasterRow.  **/
/**  A MasterRow gets its defaults from default_table_row, regardless of     **/
/**  whether or not this MasterRow is in a MasterTable or in a table	     **/
/**  instance.  A plain Row, on the other hand, gets its defaults from the   **/
/**  like-named MasterRow in its own table.  This is likewise true whether   **/
/**  or not the Row is in a MasterTable or an instance table.		     **/

Row_t *ArBuildRow(fiP, tblP, ilP)
  File_info_t *fiP;
  Table_t *tblP;
  Il_state_t *ilP;
{
  Row_t   *rowP    = NULL;
  Row_t   *masterP = NULL;
  shar    *nameP   = NULL;
  Cell_t  *cellP   = NULL;
  Frame_t *frameP  = NULL;
  shar     c       = 0;
  int      tempint = 0;
  bool     done    = NO;

    if (fiP == NULL) {
	return( NULL );
    }

    /** get the row name. **/
    if (ArGetWordShrC(fiP,NOCHAR,al_token_shr,AR_MAX_WORD-1,COMMA)==0){
	ArFprintf(stderr, "ArBuildRow(): error reading row name.\n");
	return( NULL );
    }
    /** save it until after the table is initialized. **/
    nameP = (shar *)ArSafeMalloc(sizeof(shar) * ( 1 + ArShrlen(al_token_shr)) );
    ArShrcpy(nameP, al_token_shr);

    rowP = (Row_t *)ArSafeMalloc(sizeof(Row_t));
    if (tblP == NULL) {
	masterP = NULL;
    }
    else {
	/** look up the master, **/
	for (masterP = tblP->master_rowsP;
	     masterP != NULL; 
	     masterP = (Row_t *)masterP->nextP) {
		 if (ArShrcmp(nameP, masterP->row_nameP) == 0) {
		     /** found it. **/
		     break;
		 }
	}
    }
    if (masterP == NULL) {
	*rowP = default_table_row;		/** set basic defaults  **/
    }
    else {
	/** copy row props, but not "content". **/
	 *rowP = *masterP;
	 rowP->nextP = NULL;
	 rowP->unkP = NULL;
	 rowP->contentsP = NULL;
	 rowP->attrvalsP = NULL;
	 rowP->text_content = SsInitEmpty();
    }

    SET_NullObjFields(rowP, (tblP==NULL? ID_MasterRow : ID_Row) );
    rowP->row_nameP = nameP;		        /** set this name.	**/

    if (ArParseRow(fiP, rowP) == NO) {
	ArRowFree(rowP);
	ArFprintf(stderr, "ArBuildRow(): error parsing row.\n");
	return( NULL );
    }
    /**----------------------------------------------------------------------**/
    /**	now get the contents, if any.					     **/
    /**----------------------------------------------------------------------**/
    for (done = NO; done == NO && (c = ArGetchNW(fiP)) != EOF; ) {
	if (c == AR_CMD_CHAR) {
	    if ((c = ArGetch(fiP)) == AR_CMD_CHAR) {
		/** it is '<<' and doesn't belong here. **/
		ArFprintf(stderr, 
		   "ArBuildRow(): unexpected '<<' in text. Ignoring it.\n",
		       c);
	    }
	    else if (c == AR_DECL_CHAR) {
		/** it had better be a comment. **/
		ArFprintf(stderr, 
	       "ArBuildRow(): unexpected declaration in text. Ignoring it.\n",
		       c);
		ArSkipPastTerm(fiP);
		continue;
	    }
	    else if (c == AR_HEX_CHAR) {
		/** it is '<#xxxx>' and doesn't belong here. **/
		ArFprintf(stderr, 
		   "ArBuildRow(): unexpected <#xxxx> command. Ignoring it.\n",
		       c);
		ArSkipPastTerm(fiP);
		continue;
	    }
	    else {
		/** it is a Header or Footer, a cmpn instance, (quoted or not)
		or a markup command. We really hope it is a Cell, a Frame, 
		a MasterRow, a Row, or an EndTable command, they being 
		the only ones we're prepared to accept here, or as a 
		special case in a non-master row, a page-break token.
		**/
		if (ArGetWordShrC(fiP,c,al_token_shr,AR_MAX_WORD-1,COMMA) ==0
		    || (tempint = ArIsCommandName(fiP,al_token_shr,LOOSE))==-1){
		    /** it is not a recognized command keyword.  error! **/
		    ArFprintf(stderr, 
				"ArBuildRow(): unrecognized command %Q\n", 
				al_token_shr);
		    ArSkipPastTerm(fiP);
		    continue;
		}
		/** so now it is at least a command. Is it one we want? **/
		switch (tempint) {
		case LD_PAGEBR:
		    ArUnGetch(COMMA, fiP); /** read too far ahead. **/
		    if (arBufGetPageBreak(fiP, &rowP->text_content) == NO) {
			ArFprintf(stderr, 
			    "ArBuildRow(): error reading page-break token.\n");
			SsFree(&rowP->text_content);
			/** expect other cascade errors, but what to skip to?**/
		    }
		    break;
		case AL_CELL:
		    if ((cellP = ArBuildCell(fiP, ilP)) != NULL) {
			ListAddatEnd(&rowP->contentsP, cellP);
		    }
		    break;
		case LD_FRAME:
		    if ((frameP = ArBuildFrame(fiP, NO, YES)) != NULL) {
			ListAddatEnd(&rowP->contentsP, frameP);
		    }
		    break;
		case AL_MASTER_ROW:
		case AL_ROW:
		case END_TABLE:
		    /** push back the command for ArBuildTable to find. **/
		    ArUnGetch(COMMA, fiP);
		    ArUnGetshr(al_token_shr, fiP);
		    ArUnGetch(AR_CMD_CHAR, fiP);
		    done = YES;	/** this will break us out of the loop. **/
		    break;
		default:
		    ArFprintf(stderr, 
			    "ArBuildRow(): command %Q not valid in table.\n", 
			    al_token_shr);
		    ArSkipPastTerm(fiP);
		    continue;
		    break;
		}
	    }
	}
	else {
	    /** fucked up.  text chars must follow a command. **/
	    ArFprintf(stderr, 
		   "ArBuildRow(): unexpected plain text %#C.  Ignoring it.\n",
		   c);
	}
    }
    return( rowP );
}

/**--------------------------------------------------------------------------**/
bool
ArParseTable(fiP, tblP)
  File_info_t *fiP;
  register Table_t* tblP;
{
    register shar c;
    register shar *sP;
    int tempint;
    int column = 1;
    int current_col = -1;
    Table_column_t *tcolP = NULL;
    Row_name_t *last_rownameP = NULL;
    bool columns_specified = FALSE;
    int lr_prefix;
    Table_rule_t *cur_border_ruleP = NULL;
    Table_rule_t *cur_col_ruleP = NULL;

    /* If the table does not have "Columns = N" then the number of
     * columns is taken from the master (1 if none) and then upped
     * to the largest "Column n foo = ". 
     */

    while((c = ArGetchNW(fiP)) != AR_CMD_TERM && c != EOF) {
	ArGetWordShrC(fiP, c, al_token_shr, AR_MAX_WORD, 0);
	if(ArGetchNW(fiP) != '=') {
	    ArUnGetch('=', fiP);
	    ArTellError(ERR_MISS_EQ);
	    ArSkipPastComma(fiP);
	    continue;
	}
	if(ArStrncmpShr(al_token_shr, "Column", 6, LOOSE) == 0 &&
	   ArForceCap(al_token_shr[6]) != 'S') {
	    sP = al_token_shr + 6;
	    while((c = *sP) && (c > '9' || c < '0')) ++sP;
	    if(c && (tempint = ArAtoiS(sP)) > 0 &&
	       (tempint <= tblP->num_columns || !columns_specified)) {
		column = tempint;
		while((c = *sP) && ((c >= '0' && c <= '9') || c == ' ')) ++sP;
		ArShrcpy(al_token_shr, sP);
	    } else {
		ArTellError(ERR_TBL_COLS);
		ArSkipPastComma(fiP);
		column = 1;
		continue;
	    }
	}
	if(column != current_col) {
	    current_col = column;
	    if(current_col > tblP->num_columns)
		tableColsSetup(tblP, current_col);
	    tcolP = tableColumnN(tblP, current_col);
	    cur_col_ruleP = &tcolP->left;
	}
	/** assert: tcolP != NULL **/

	switch(lr_prefix = ArLRTBPrefix(fiP)) {
	default:
	case LD_TOP:
	    cur_border_ruleP = &tblP->border.top;
	    cur_col_ruleP = &tcolP->top;
	    break;
	case LD_BOTTOM:
	    cur_border_ruleP = &tblP->border.bottom;
	    cur_col_ruleP = &tcolP->top;
	    break;
	case LD_LEFT:
	    cur_border_ruleP = &tblP->border.left;
	    cur_col_ruleP = &tcolP->left;
	    break;
	case LD_RIGHT:
	    cur_border_ruleP = &tblP->border.right;
#ifdef OUT_FOR_NOW
/* NOTE: this doesn't work, because the "Column N+1 Width =" will
 * reset all the ruling settings. */
	    if(col->next)
		cur_col_ruleP = &tcolP->next->left;
#endif
	    break;
	case LD_HEADER:
	    cur_border_ruleP = &tblP->header_rule;
	    break;
	case LD_FOOTER:
	    cur_border_ruleP = &tblP->footer_rule; /* corrected typo kint 8/22/89 */
	    break;
	}

        switch(ArTokenShr(fiP, al_token_shr, al_table_tokens,
			  NUM_OF(al_table_tokens), LOOSE)) {
	case LD_TBL_COLUMNS:
	    if (ArGetInt(fiP, &tempint, YES, NO) == NO || 
	        columns_specified                      || 
	        tempint <= 0) {
		    ArTellError(ERR_TBL_COLS);
	    } else {
		    tableColsSetup(tblP, tempint);
		    columns_specified = TRUE;
	    }
	    break;
	case LD_TBL_WIDTH:
	    /* NOTE! Width must come before column rulings */
	    ArGetPRsu(fiP, &tcolP->width);
	    break;
	case LD_TBL_BORDER:
	    /* this should be removed after all old tables using it are gone */
	    {
		/** int temp = TBL_BORDER_DEF; BT **/
		/** ArGetInt(&temp); BT **/
		/** if(temp >= 0 && temp <= TBL_BORDER_MAX) BT **/
		/**     EcTblRulSetBorderWidth(table, temp); BT **/
		ArFprintf(stderr, 
			"ArParseTable(): obsolete 'Border' prop ignored: \n");
		ArSkipPastComma(fiP);
	    }
	    break;
	case LD_TBL_ALT_TINT:	/* TEMP HACK */
	    {
		/** commented out in ld original.  BT **/
		/** short tempshort = TRUE;/* if ERR_YESNO assume true  BT **/
		/** ArGetYesno(&tempshort, TRUE); BT **/
		/** if(tempshort) { BT **/
		/**     tblP->tbl_rulings |= TBL_ALT_TINT;  BT **/
		/** } BT **/
	    }
	    break;
	case LD_TBL_BRKAFTER:
	    { short temp_short;
		if (ArGetYesNoShort(fiP, &temp_short) == YES) {
		    tblP->flags.pbreak_after = temp_short;
		}
	    }
	    break;
	case LD_TBL_BRKWITHIN:
	    { short temp_short;
		if (ArGetYesNoShort(fiP, &temp_short) == YES) {
		    tblP->flags.pbreak_within = temp_short;
		}
	    }
	    break;
	case LD_TBL_MILRUL:
	    { short temp_short;
		if (ArGetYesNoShort(fiP, &temp_short) == YES) {
		    tblP->flags.pbreak_rulings = temp_short;
		}
	    }
	    break;
	case LD_TBL_APAGE:
	    { short temp_short;
		if (ArGetYesNoShort(fiP, &temp_short) == YES) {
		    tblP->flags.A_page = temp_short;
		}
	    }
	    break;
	case LD_TBL_MARGIN:
	    switch(lr_prefix) {
	    case LD_BOTTOM:
		ArGetPZRsu(fiP, &tblP->bottom_margin);
		break;
	    case LD_LEFT:
		ArGetRsu(fiP, &tblP->left_margin);
		break;
	    case LD_RIGHT:
		ArGetPZRsu(fiP, &tblP->right_margin);
		break;
	    case LD_TOP:
		ArGetPZRsu(fiP, &tblP->top_margin);
		break;
	    }
	    break;
	case LD_TBL_NEWPAGE:
	    { short temp_short;
		if (ArGetYesNoShort(fiP, &temp_short) == YES) {
		    tblP->flags.new_page = temp_short;
		}
	    }
	    break;
	case LD_TBL_NEWCOLUMN:
	    { short temp_short;
		if (ArGetYesNoShort(fiP, &temp_short) == YES) {
		    tblP->flags.new_column = temp_short;
		}
	    }
	    break;
	case LD_TBL_ORPH_CNTL:
	    if (ArGetInt(fiP, &tempint, YES, NO) == NO || 
		tempint < 0 || 
		tempint > AR_MAX_WIDORPH) {
		    ArTellError(ERR_ORPH_VAL);
		    break;
	    }
	    if(tempint == 0) tempint = 1;
	    tblP->orphan_control = tempint;
	    break;
	case LD_TBL_WID_CNTL:
	    if (ArGetInt(fiP, &tempint, YES, NO) == NO || 
		tempint < 0 || 
		tempint > AR_MAX_WIDORPH) {
		    ArTellError(ERR_WID_VAL);
		    break;
	    }
	    if(tempint == 0) tempint = 1;
	    tblP->widow_control = tempint;
	    break;
	case LD_TBL_STRADDLE:
	    { short temp_short;
		if (ArGetYesNoShort(fiP, &temp_short) == YES) {
		    tblP->flags.straddle = temp_short;
		}
	    }
	    break;
	case LD_TBL_ROWNAME:
	    {
	      Row_name_t *my_rownameP;
	      int length;

		ArGetWordShrC(fiP, NOCHAR, al_token_shr, AR_MAX_NAME, COMMA);
		my_rownameP = NEW(Row_name_t);
		my_rownameP->nextP = NULL;
		length = ArShrlen(al_token_shr);
		my_rownameP->row_nameP = 
			(shar *)ArSafeMalloc(sizeof(shar)*(length+1));
		ArShrcpy(my_rownameP->row_nameP, al_token_shr);
		if (last_rownameP == NULL) {
		    tblP->create_rowsP = last_rownameP = my_rownameP;
		}
		else {
		    last_rownameP->nextP = my_rownameP;
		    last_rownameP = my_rownameP;
		}
	    }
	    break;

	case LD_TBL_BORDER_COLOR:
	    ArGetRulingColor(fiP, cur_border_ruleP);
	    break;
	case LD_TBL_BORDER_PATTERN:
	    ArGetRulingPattern(fiP, cur_border_ruleP);
	    break;
	case LD_TBL_BORDER_WIDTH:
	    ArGetRulingWidth(fiP, cur_border_ruleP);
	    break;
	case LD_TBL_BORDER_VIS:
	    ArGetRulingVis(fiP, cur_border_ruleP);
	    break;

	case LD_TBL_COL_RULE_COLOR:
	    ArGetRulingColor(fiP, cur_col_ruleP);
	    break;
	case LD_TBL_COL_RULE_PATTERN:
	    ArGetRulingPattern(fiP, cur_col_ruleP);
	    break;
	case LD_TBL_COL_RULE_WIDTH:
	    ArGetRulingWidth(fiP, cur_col_ruleP);
	    break;
	case LD_TBL_COL_RULE_VIS:
	    ArGetRulingVis(fiP, cur_col_ruleP);
	    break;

	default:
	    /** ArTellError2(ERR_NO_SUCH_CVAL, al_token_shr, 0); BT **/
	    if (lr_prefix == NO_MATCH) {
		tblP->unkP = ArBuildUnknownPropEq(fiP, al_token_shr,tblP->unkP);
	    }
	    else {
		tblP->unkP = ArBuildUnknownPropPrefixEq(fiP,
						      lr_prefix,
						      al_token_shr,
						      tblP->unkP);
	    }
	    ArSkipPastComma(fiP);
	    break;
	}
    }	    /* end while loop */
#if 0	/** BT **/
    EcTableColSetX(tblP);
#endif	/** BT **/

    return( YES );
}

/**--------------------------------------------------------------------------**/
NullObj_t *ArBuildTable(fiP, ilP, isMaster)
  File_info_t *fiP;
  Il_state_t *ilP;
  bool isMaster;
{ 
  Table_t *tblP    = NULL;
  Table_t *masterP = NULL;
  shar    *nameP   = NULL;
  Row_t   *rowP    = NULL;
  shar     c       = 0;
  int      tempint = 0;

    if (fiP == NULL) {
	return( NULL );
    }

    tblP= (Table_t *)ArSafeMalloc(sizeof(Table_t));

    /** get the table name. **/
    if (ArGetWordShrC(fiP,NOCHAR,al_token_shr,AR_MAX_WORD-1,COMMA)==0){
	FREE(tblP);
	ArFprintf(stderr, "ArBuildTable(): error reading table name.\n");
	return( NULL );
    }
    /** save it until after the table is initialized. **/
    nameP = (shar *)ArSafeMalloc(sizeof(shar) * (1 + ArShrlen(al_token_shr)) );
    ArShrcpy(nameP, al_token_shr);

    if (isMaster == YES || ilP == NULL) {
	masterP = NULL;
    }
    else {
	/** look up the master, copy ONLY the table props, not row props. **/
	for (masterP = ilP->tableP; 
	     masterP != NULL; 
	     masterP = (Table_t *)masterP->nextP) {
		 if (ArShrcmp(nameP, masterP->table_nameP) == 0) {
		     /** found it. **/
		     break;
		 }
	}
    }
    if (masterP == NULL) {
	*tblP = default_table;		/** set basic defaults **/
#if 0	/** BT **/
this is done in tableColsSetup()
	tblP->num_columns = 1;
	tblP->column_infoP = 
		(Table_column_t *)ArSafeMalloc(sizeof(Table_column_t));
	*tblP->column_infoP = default_table_column;
#endif	/** BT **/
    }
    else {
	 /** copy properties, but not "contents", from the master.  **/
	 *tblP = *masterP;
	 tblP->nextP         = NULL;
	 tblP->unkP          = NULL;
	 tblP->num_columns   = 0;
	 tblP->column_infoP  = NULL;
	 tblP->create_rowsP  = NULL;
	 tblP->master_rowsP  = NULL;
	 tblP->content_rowsP = NULL;
    }
    SET_NullObjFields(tblP, (isMaster? ID_MasterTable : ID_Table) );
    tblP->table_nameP = nameP;

    if (ArParseTable(fiP, tblP) == NO) {
	ArTableFree(tblP);
	ArFprintf(stderr, "ArBuildTable(): error parsing table.\n");
	return( NULL );
    }
    /**----------------------------------------------------------------------**/
    /**	now get the master rows, and the contents, if any.		     **/
    /**----------------------------------------------------------------------**/
    while ((c = ArGetchNW(fiP)) != EOF) {
	if (c == AR_CMD_CHAR) {
	    if ((c = ArGetch(fiP)) == AR_CMD_CHAR) {
		/** it is '<<' and doesn't belong here. **/
		ArFprintf(stderr, 
		   "ArBuildTable(): unexpected '<<' in text. Ignoring it.\n",
		       c);
	    }
	    else if (c == AR_DECL_CHAR) {
		/** it had better be a comment. **/
		ArFprintf(stderr, 
	       "ArBuildTable(): unexpected declaration in text. Ignoring it.\n",
		       c);
		ArSkipPastTerm(fiP);
		continue;
	    }
	    else if (c == AR_HEX_CHAR) {
		/** it is '<#xxxx>' and doesn't belong here. **/
		ArFprintf(stderr, 
		   "ArBuildTable(): unexpected <#xxxx> command. Ignoring it.\n",
		       c);
		ArSkipPastTerm(fiP);
		continue;
	    }
	    else {
	    /** it is a Header or Footer, a cmpn instance, (quoted or not)
		or a markup command. We really hope it is a MasterRow
		a Row, or an EndTable command, they being the only ones 
		we're prepared to accept here.				     **/
		if (ArGetWordShrC(fiP,c,al_token_shr,AR_MAX_WORD-1,COMMA) ==0
		    || (tempint = ArIsCommandName(fiP,al_token_shr,LOOSE))==-1){
		    /** it is not a recognized command keyword.  error! **/
		    ArFprintf(stderr, 
				"ArBuildTable(): unrecognized command %Q\n", 
				al_token_shr);
		    ArSkipPastTerm(fiP);
		    continue;
		}
		/** so now it is at least a command. Is it one we want? **/
		switch (tempint) {
		case END_TABLE:
		    ArSkipPastTerm(fiP);
		    /** we will leave the while loop at the statement below. **/
		    break;
		case AL_MASTER_ROW:
		    if ((rowP = ArBuildRow(fiP, NULL, ilP)) != NULL) {
			ListAddatEnd(&tblP->master_rowsP, rowP);
		    }
		    break;
		case AL_ROW:
		    if ((rowP = ArBuildRow(fiP, tblP, ilP)) != NULL) {
			ListAddatEnd(&tblP->content_rowsP, rowP);
		    }
		    break;
		default:
		    ArFprintf(stderr, 
			    "ArBuildTable(): command %Q not valid in table.\n", 
			    al_token_shr);
		    ArSkipPastTerm(fiP);
		    continue;
		    break;
		}
		if (tempint == END_TABLE) { /** done with this whole table.  **/
		    break;		    /** leave the while loop and end.**/
		}
	    }
	}
	else {
	    /** fucked up.  text chars must be within a command,
		either MasterRow, Row, or Cell
	    **/
	    ArFprintf(stderr, 
		   "ArBuildTable(): unexpected plain text %#C.  Ignoring it.\n",
		   c);
	}
    }

    return( (NullObj_t *)tblP );
}
/**--------------------------------------------------------------------------**/
/**	Table Output functions.						     **/
/**--------------------------------------------------------------------------**/
void ArRulingWrite(ruleP, masterP, labelP, ofile)
  Table_rule_t *ruleP;
  Table_rule_t *masterP;
  char *labelP;
  FILE *ofile;
{ 
  bool implied_visible = NO;
  register int temp;

    if (ruleP == NULL || labelP == NULL || ofile == NULL) {
	return;
    }
    if (masterP == NULL) {
	masterP = &default_table_rule;
    }
    if ((temp = ruleP->width) != masterP->width || ruleP->double_rule == YES) {
	ArFprintf(ofile, PROP_PREFIX_SUFFIX_DEC_FMT,
		labelP, "Weight", temp, 
		(ruleP->double_rule == YES? " double" : " ") );
	implied_visible = YES;
    }
    if ((temp = ruleP->color) != masterP->color) {
	ArFprintf(ofile, PROP_PREFIX_DECIMAL_FMT, labelP, "Color", temp);
	implied_visible = YES;
    }
    if ((temp = ruleP->pattern) != masterP->pattern) {
	ArFprintf(ofile, PROP_PREFIX_DECIMAL_FMT, labelP, "Pattern", temp);
	implied_visible = YES;
    }
    if (ruleP->visible != masterP->visible ||
	(implied_visible == YES && ruleP->visible == NO) ) {
	    ArFprintf(ofile, PROP_PREFIX_YESNO_FMT,
			labelP, "Visible", YesNo(ruleP->visible) );
    }
}
/**--------------------------------------------------------------------------**/
void ArCellWrite(cellP, ilP, ofile)
  Cell_t *cellP;
  Il_state_t *ilP;
  FILE *ofile;
{ 
  Cell_t       *defaultP = &default_table_cell;
  register int  temp_int = 0;
  NullObj_t    *thingP   = NULL;

    if (Identify(cellP) == ID_Cell) {
	ArFprintf(ofile, CMD_START_LINE_FMT, "Cell");
	ArUnknownWrite(cellP->unkP, ofile);

	if (cellP->autosizeP != NULL) {
	    ArAutosizeWrite(cellP->autosizeP, cellP->width, cellP->height, ofile);
	}
	else {
	    if ((temp_int = cellP->width) != defaultP->width) {
		FPRINTF(ofile, PROP_INCH_FMT, "Width",  R2I(temp_int)  );
	    }
	    if ((temp_int = cellP->height) != defaultP->height) {
		FPRINTF(ofile, PROP_INCH_FMT, "Height",  R2I(temp_int)  );
	    }
	}

	if (cellP->auto_edit != defaultP->auto_edit) {
	    ArFprintf(ofile, PROP_YESNO_FMT,"AutoEdit",YesNo(cellP->auto_edit));
	}
	if (cellP->straddle != defaultP->straddle) {
	    ArFprintf(ofile, PROP_DECIMAL_FMT, "Straddle", cellP->straddle);
	}
	if ((temp_int = cellP->v_align) != defaultP->v_align) {
	    ArFprintf(ofile, PROP_STRING_FMT, "Vertical Alignment", 
			FrameVAlign_names[cellP->v_align]);
	}

	ArRulingWrite(&cellP->rulings.top,   NULL, "TopRuling",  ofile);
	ArRulingWrite(&cellP->rulings.left,  NULL, "LeftRuling", ofile);

	ArFprintf(ofile, CMD_END_FMT);
	for (thingP = cellP->cellbodyP; 
	    thingP != NULL;
	    thingP = thingP->nextP) {
		ArObjectWrite(thingP, ilP, ofile);
	}
    }
}
/**--------------------------------------------------------------------------**/
void ArRowWrite(rowP, tblP, ilP, ofile)
  Row_t *rowP;
  Table_t *tblP;
  Il_state_t *ilP;
  FILE *ofile;
{ 
  Row_t *masterP       = NULL;
  bool   isMaster      = NO;
  register int temp    = 0;
  NullObj_t *contentsP = NULL;

    if (rowP == NULL || ofile == NULL) {
	return;
    }

    switch(Identify(rowP)) {
    case ID_MasterRow:
	ArFprintf(ofile, CMD_START_LINE_FMT, "MasterRow");
	isMaster = YES;
	break;
    case ID_Row:
	ArFprintf(ofile, CMD_START_LINE_FMT, "Row");
	break;
    default:
	ArFprintf(stderr, 
		  "ArRowWrite(): given non-row, id == %d. Ignoring it.\n",
		  Identify(rowP));
	return;
	break;
    }
    ArFprintf(ofile, OBJ_NAME_FMT, rowP->row_nameP);

    if (isMaster == NO && tblP != NULL) {
	for (masterP = tblP->master_rowsP; 
	    masterP != NULL; 
	    masterP = (Row_t *)masterP->nextP) {
		if (ArShrcmp(rowP->row_nameP, masterP->row_nameP) == 0) {
		    break;
		}
	}
    }
    if (masterP == NULL) {
	masterP = &default_table_row;
    }
    if (rowP->attrvalsP) {
	ArAttrValsWrite( rowP->attrvalsP, ofile );
    }
    /** ArFprintf(ofile, "\n-- checking row props. --"); BT **/
    if ((temp = rowP->top_margin) != masterP->top_margin) {
	FPRINTF(ofile, PROP_INCH_FMT, "TopMargin", R2I(temp) );
    }
    if ((temp = rowP->bottom_margin) != masterP->bottom_margin) {
	    FPRINTF(ofile, PROP_INCH_FMT, "BottomMargin", R2I(temp) );
    }
    if (ArFontValueEqual(&rowP->font, &masterP->font) == NO) {
	ArFprintf(ofile, PROP_KEYWORD_EQ_FMT, "Font");
	ArFontValueWrite(&rowP->font, NULL, ofile);
    }
    if ((temp = rowP->flags.hidden) != masterP->flags.hidden) {
	ArFprintf(ofile, PROP_YESNO_FMT, "Hidden", YesNo(temp) );
    }
    if ((temp = rowP->flags.new_page) != masterP->flags.new_page) {
	ArFprintf(ofile, PROP_YESNO_FMT, "BeginNewPage", YesNo(temp) );
    }
    if ((temp = rowP->flags.new_column) != masterP->flags.new_column) {
	ArFprintf(ofile, PROP_YESNO_FMT, "BeginNewColumn", YesNo(temp) );
    }
    if ((temp = rowP->flags.pbreak_after) != masterP->flags.pbreak_after) {
	ArFprintf(ofile, PROP_YESNO_FMT, "AllowPageBreakAfter", YesNo(temp) );
    }
    if ((temp = rowP->flags.is_header) != masterP->flags.is_header) {
	ArFprintf(ofile, PROP_YESNO_FMT, "Header", YesNo(temp) );
    }
    if ((temp = rowP->flags.is_footer) != masterP->flags.is_footer) {
	ArFprintf(ofile, PROP_YESNO_FMT, "Footer", YesNo(temp) );
    }
    if ((temp = rowP->flags.A_page) != masterP->flags.A_page) {
	ArFprintf(ofile, PROP_YESNO_FMT, "A-page", YesNo(temp) );
    }
    ArUnknownWrite(rowP->unkP, ofile);
    ArFprintf(ofile, DECL_END_FMT);
    /** ArFprintf(ofile, "\n-- writing text_content. --"); BT **/
    ArTextBodyWrite(&rowP->text_content, ilP, ofile);
    /** ArFprintf(ofile, "\n-- writing row contents. --"); BT **/
    for (contentsP = rowP->contentsP; 
	contentsP != NULL;
	contentsP = contentsP->nextP) {
	    ArObjectWrite(contentsP, ilP, ofile);
    }
}
/**--------------------------------------------------------------------------**/
void ArTableWrite(tblP, ilP, ofile)
  register Table_t *tblP;
  Il_state_t *ilP;
  register FILE *ofile;
{ 
  Table_t        *masterP 	= NULL;
  Table_column_t *tcolP   	= NULL;
  Row_t          *rowP    	= NULL;
  Row_name_t     *create_rowP   = NULL;
  register Rsu    temp    	= 0;
  bool		  isMaster	= NO;
  static char contents_err_msg[] = 
      "ArBuildTable(): error: Master Table has contents.  Get help.";

    if (tblP == NULL || ofile == NULL) {
	return;
    }
    switch(Identify(tblP)) {
    case ID_MasterTable:
	ArFprintf(ofile, DECL_START_FMT, "MasterTable");
	isMaster = YES;
	break;
    case ID_Table:
	ArFprintf(ofile, CMD_START_LINE_FMT, "Table");
	break;
    default:
	ArFprintf(stderr, 
		  "ArTableWrite(): given non-table, id == %d. Ignoring it.\n",
		  Identify(tblP));
	return;
	break;
    }
    ArFprintf(ofile, OBJ_NAME_FMT, tblP->table_nameP);
    ArFprintf(ofile, PROP_DECIMAL_FMT, "Columns", tblP->num_columns);

    if (isMaster == NO) {
	for (masterP = ilP->tableP; 
	    masterP != NULL; 
	    masterP = (Table_t *)masterP->nextP) {
		if (ArShrcmp(tblP->table_nameP, masterP->table_nameP) == 0) {
		    break;
		}
	}
    }
    if (masterP == NULL) {
	masterP = &default_table;
    }

    for (tcolP = tblP->column_infoP, temp = 1; 
	 tcolP != NULL && temp <= tblP->num_columns;
	 ++temp, tcolP = tcolP->nextP) 
    { 
      char buf[AR_MAX_WORD];
	if (temp != tcolP->num_column) {
	     /** redundant check **/
	     ArFprintf(stderr,"ArTableWrite():column counts inconsistent:");
	     ArFprintf(stderr, 
			 " list entry %d claims to be %d.  Get help.\n",
			 temp, tcolP->num_column);
	}
	sprintf(buf, "Column %d ", tcolP->num_column);
	ArRulingWrite(&tcolP->top,  NULL, buf, ofile);
	ArRulingWrite(&tcolP->left, NULL, buf, ofile);
	FPRINTF(ofile, PROP_PREFIX_INCH_FMT, buf, "Width", R2I(tcolP->width));
    }

    /** ArFprintf(ofile, "\n-- checking table props. --"); BT **/
    if ((temp = tblP->top_margin) != masterP->top_margin) {
	FPRINTF(ofile, PROP_INCH_FMT, "TopMargin", R2I(temp) );
    }
    if ((temp = tblP->bottom_margin) != masterP->bottom_margin) {
	FPRINTF(ofile, PROP_INCH_FMT, "BottomMargin", R2I(temp) );
    }
    if ((temp = tblP->left_margin) != masterP->left_margin) {
	FPRINTF(ofile, PROP_INCH_FMT, "LeftMargin", R2I(temp) );
    }
    if ((temp = tblP->right_margin) != masterP->right_margin) {
	FPRINTF(ofile, PROP_INCH_FMT, "RightMargin", R2I(temp) );
    }
    if ((temp = tblP->flags.new_page) != masterP->flags.new_page) {
	ArFprintf(ofile, PROP_YESNO_FMT, "BeginNewPage", YesNo(temp) );
    }
    if ((temp = tblP->flags.new_column) != masterP->flags.new_column) {
	ArFprintf(ofile, PROP_YESNO_FMT, "BeginNewColumn", YesNo(temp) );
    }
    if ((temp = tblP->flags.pbreak_within) != masterP->flags.pbreak_within) {
	ArFprintf(ofile, PROP_YESNO_FMT, "AllowPageBreakWithin", YesNo(temp) );
    }
    if ((temp = tblP->flags.pbreak_after) != masterP->flags.pbreak_after) {
	ArFprintf(ofile, PROP_YESNO_FMT, "AllowPageBreakAfter", YesNo(temp) );
    }
    if ((temp = tblP->flags.straddle) != masterP->flags.straddle) {
	ArFprintf(ofile, PROP_YESNO_FMT, "Straddle", YesNo(temp) );
    }
    if ((temp = tblP->flags.pbreak_rulings) != masterP->flags.pbreak_rulings) {
	ArFprintf(ofile, PROP_YESNO_FMT, "PageBreakRulings", YesNo(temp) );
    }
    if ((temp = tblP->flags.A_page) != masterP->flags.A_page) {
	ArFprintf(ofile, PROP_YESNO_FMT, "A-page", YesNo(temp) );
    }
    if ((temp = tblP->widow_control) != masterP->widow_control) {
	ArFprintf(ofile, PROP_DECIMAL_FMT, "WidowControl", temp);
    }
    if ((temp = tblP->orphan_control) != masterP->orphan_control) {
	ArFprintf(ofile, PROP_DECIMAL_FMT, "OrphanControl", temp);
    }
    ArRulingWrite(&tblP->border.top,   &masterP->border.top,   
			"TopBorder", ofile);
    ArRulingWrite(&tblP->border.bottom,&masterP->border.bottom,
			"BottomBorder", ofile);
    ArRulingWrite(&tblP->border.left,  &masterP->border.left,  
			"LeftBorder", ofile);
    ArRulingWrite(&tblP->border.right, &masterP->border.right, 
			"RightBorder", ofile);
    ArRulingWrite(&tblP->header_rule,  &masterP->header_rule,  
			"HeaderBorder", ofile);
    ArRulingWrite(&tblP->footer_rule,  &masterP->footer_rule,  
			"FooterBorder", ofile);

    if (isMaster) {
	/** ArFprintf(ofile, "\n-- dumping create row masters. --"); BT **/
	for (create_rowP = tblP->create_rowsP; 
	     create_rowP != NULL; 
	     create_rowP = create_rowP->nextP) {
		ArFprintf(ofile, PROP_NAME_FMT, "Row", create_rowP->row_nameP );
	}
    }
    ArUnknownWrite(tblP->unkP, ofile);

    ArFprintf(ofile, (isMaster==YES ? DECL_END_FMT : CMD_END_FMT) );

    /** always write the definition of the rows. **/
    /** ArFprintf(ofile, "\n-- dumping master_rows. --"); BT **/
    for(rowP = tblP->master_rowsP; rowP != NULL; rowP = (Row_t *)rowP->nextP) {
	ArRowWrite(rowP, tblP, ilP, ofile);
    }
    if (tblP->content_rowsP != NULL) {
	if (isMaster == YES) {
	    ArFprintf(stderr, "%s\n", contents_err_msg);
	    ArFprintf(ofile, "-- %s --\n", contents_err_msg);
	}
	else {
	    /** write the contents of the rows. **/
	    /** ArFprintf(ofile, "\n-- dumping content_rows. --"); BT **/
	    for(rowP = tblP->content_rowsP; 
		rowP != NULL; 
		rowP = (Row_t *)rowP->nextP) {
		    ArRowWrite(rowP, tblP, ilP, ofile);
	    }
	}
    }
    ArFprintf(ofile, CMD_START_LINE_FMT, "EndTable");
    ArFprintf(ofile, CMD_END_FMT);
}
/**--------------------------------------------------------------------------**/
void ArTableMastersWrite(tblP, ofile)
  register Table_t *tblP;
  register FILE *ofile;
{ 
    for ( ; tblP != NULL; tblP = (Table_t *)tblP->nextP) {
	ArTableWrite(tblP, NULL, ofile);
    }
}
/**--------------------------------------------------------------------------**/
/**	Table Free-ing functions.					     **/
/**--------------------------------------------------------------------------**/
bool ArCellFree(cellP) 
  Cell_t *cellP;
{ 
    if (cellP == NULL) {
	return( YES );
    }
    /** extra security check. **/
    switch (Identify(cellP)) {
	case ID_Cell:
	    /** it's cool. **/
	    break;
	default:
	    return( NO );
	    break;
    }
    if (ArObjectFree(cellP->nextP) == YES) {
	ArUnknownFree(cellP->unkP);
	if (cellP->autosizeP != NULL) {
	    FREE((char *)cellP->autosizeP);
	}
	if (ArObjectFree(cellP->cellbodyP) == YES) {
	    FREE((char *)cellP);
	    return( YES );
	}
    }
    return( NO );
}
/**--------------------------------------------------------------------------**/
bool ArRowFree(rowP) 
  Row_t *rowP;
{ 
    if (rowP == NULL) {
	return( YES );
    }
    /** extra security check. **/
    switch (Identify(rowP)) {
	case ID_MasterRow:
	case ID_Row:
	    /** it's cool. **/
	    break;
	default:
	    return( NO );
	    break;
    }
    if (ArRowFree(rowP->nextP) == YES) {
	ArUnknownFree(rowP->unkP);
	ArAttrFree(rowP->attrvalsP);
	ArBodyFree(&rowP->text_content);
	if (ArObjectFree(rowP->contentsP) == YES) {
	    FREE((char *)rowP->row_nameP);
	    FREE((char *)rowP);
	    return( YES );
	}
    }
    return( NO );
}
/**--------------------------------------------------------------------------**/
bool ArTableColumnFree(tcolP)
  Table_column_t *tcolP;
{ 
  Table_column_t *nextP;

    for ( ; tcolP != NULL; ) {
	nextP = tcolP->nextP;
	FREE((char *)tcolP);
	tcolP = nextP;
    }
}
/**--------------------------------------------------------------------------**/
bool ArTableFree(tblP)
  Table_t *tblP;
{ 
    if (tblP == NULL) {
	return( YES );
    }
    /** extra security check. **/
    switch (Identify(tblP)) {
	case ID_MasterTable:
	case ID_Table:
	    /** it's cool. **/
	    break;
	default:
	    return( NO );
	    break;
    }
    if (ArObjectFree(tblP->nextP) == YES) {
	ArUnknownFree(tblP->unkP);
	ArTableColumnFree(tblP->column_infoP);
	if (ArRowFree(tblP->content_rowsP) == YES &&
	    ArRowFree(tblP->master_rowsP)  == YES) {
		FREE((char *)tblP->table_nameP);
		FREE((char *)tblP);
		return( YES );
	}
    }
    return( NO );
}
/**--------------------------------------------------------------------------**/
