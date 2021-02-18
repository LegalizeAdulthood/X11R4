
/*

$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/font1.c,v 1.2 89/10/26 14:09:44 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/font1.c,v $

		(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	font1.c,v $
 * Revision 1.2  89/10/26  14:09:44  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.1  89/09/21  06:30:31  mss
 * Initial revision
 * 
 * Revision 1.10  89/01/24  12:16:23  mss
 * Made forward decls consistent with actual declations (w.r.t. static)
 * 
 * Revision 1.9  88/11/02  12:43:44  jr
 * Fix bug in Ints2Dsclass found
 * by Ann.
 * 
 * Revision 1.8  88/10/27  15:47:35  mz0k
 * Oops..  Put the iso_0's into the UnspecifyModalfont
 * 
 * Revision 1.7  88/10/27  15:08:57  mz0k
 * Added UnfpecifyWritingModes
 * 
 * Revision 1.6  88/10/25  17:39:47  jr
 * Unspecify AVGWTESC field
 * in UnspecifyFont
 * 
 * Revision 1.5  88/09/20  15:58:13  jr
 * Shorten long names.
 * 
 * Revision 1.4  88/09/19  13:13:00  annm
 * fixed tkerr.h include problem (no GENINCLDIR
 * 
 * Revision 1.3  88/09/16  10:16:35  mss
 * added missing tk_tki_tkerr flag
 * 
 * Revision 1.2  88/09/15  09:19:55  jr
 * UnspecifyAssignStructName -> UnspecifytructName
 * 
 * Revision 1.1  88/09/12  17:42:12  jr
 * Initial revision
 * 
*/

#define TK_IMPL_FONT
#define TK_GVARS_FONT1
#define TK_TKI_MM
#define TK_TKI_TKERR

#include <font.h>
#include <seq.h>
#include <mm.h>
#include <tkerr.h>

#undef TK_TKI_MM
#undef TK_GVARS_FONT1
#undef TK_IMPL_FONT

/* Forward declarations */
static INT_type		UnspecifyModalFont();
static INT_type		UnspecifyScore();
static INT_type		UnspecifyVarScript();
static INT_type		UnspecifyStructName();
static STRUCTURED_NAME_type	*MakeStructName();

/* Table for ISO/DSCLASS Values */

static struct {
    VALUE_type	name;
    BYTE_type	seq[3];
} dsclasses[] = {

    { DSCLASS_UNCIALS,			{ 1, 0, 0 } },
    { DSCLASS_UNCIALS_1,		{ 1, 1, 0 } },
    { DSCLASS_UNCIALS_1_sans,		{ 1, 1, 1 } },
    { DSCLASS_UNCIALS_1_serif,		{ 1, 1, 2 } },
    { DSCLASS_UNCIALS_2,		{ 1, 2, 0 } },
    { DSCLASS_UNCIALS_2_sans,		{ 1, 2, 1 } },
    { DSCLASS_UNCIALS_2_serif,		{ 1, 2, 2 } },

    { DSCLASS_INSCR,			{ 2, 0, 0 } },
    { DSCLASS_INSCR_solids,		{ 2, 1, 0 } },
    { DSCLASS_INSCR_solids_sans,	{ 2, 1, 1 } },
    { DSCLASS_INSCR_solids_serif,	{ 2, 1, 2 } },
    { DSCLASS_INSCR_inlines,		{ 2, 2, 0 } },
    { DSCLASS_INSCR_inlines_sans,	{ 2, 2, 1 } },
    { DSCLASS_INSCR_inlines_serif,	{ 2, 2, 2 } },
    { DSCLASS_INSCR_outlines,		{ 2, 3, 0 } },
    { DSCLASS_INSCR_outlines_sans,	{ 2, 3, 1 } },
    { DSCLASS_INSCR_outlines_serif,	{ 2, 3, 2 } },

    { DSCLASS_BLACK,			{ 3, 0, 0 } },
    { DSCLASS_BLACK_formal,		{ 3, 1, 0 } },
    { DSCLASS_BLACK_formal_sans,	{ 3, 1, 1 } },
    { DSCLASS_BLACK_formal_serif,	{ 3, 1, 2 } },
    { DSCLASS_BLACK_formal_sans_eng,	{ 3, 1, 3 } },
    { DSCLASS_BLACK_formal_serif_eng,	{ 3, 1, 4 } },
    { DSCLASS_BLACK_round,		{ 3, 2, 0 } },
    { DSCLASS_BLACK_round_sans,		{ 3, 2, 1 } },
    { DSCLASS_BLACK_round_serif,	{ 3, 2, 2 } },
    { DSCLASS_BLACK_round_sans_eng,	{ 3, 2, 3 } },
    { DSCLASS_BLACK_round_serif_eng,	{ 3, 2, 4 } },
    { DSCLASS_BLACK_hybrid,		{ 3, 3, 0 } },
    { DSCLASS_BLACK_hybrid_sans,	{ 3, 3, 1 } },
    { DSCLASS_BLACK_hybrid_serif,	{ 3, 3, 2 } },
    { DSCLASS_BLACK_hybrid_sans_eng,	{ 3, 3, 3 } },
    { DSCLASS_BLACK_hybrid_serif_eng,	{ 3, 3, 4 } },
    { DSCLASS_BLACK_inform,		{ 3, 4, 0 } },
    { DSCLASS_BLACK_inform_sans,	{ 3, 4, 1 } },
    { DSCLASS_BLACK_inform_serif,	{ 3, 4, 2 } },
    { DSCLASS_BLACK_inform_sans_eng,	{ 3, 4, 3 } },
    { DSCLASS_BLACK_inform_serif_eng,	{ 3, 4, 4 } },

    { DSCLASS_SERIFS,			{ 4, 0, 0 } },
    { DSCLASS_SERIFS_oldstyle,		{ 4, 1, 0 } },
    { DSCLASS_SERIFS_oldstyle_vene,	{ 4, 1, 1 } },
    { DSCLASS_SERIFS_oldstyle_garalde,	{ 4, 1, 2 } },
    { DSCLASS_SERIFS_oldstyle_dutch,	{ 4, 1, 3 } },
    { DSCLASS_SERIFS_trans,		{ 4, 2, 0 } },
    { DSCLASS_SERIFS_trans_direct,	{ 4, 2, 1 } },
    { DSCLASS_SERIFS_trans_modif,	{ 4, 2, 2 } },
    { DSCLASS_SERIFS_modern,		{ 4, 3, 0 } },
    { DSCLASS_SERIFS_modern_italian,	{ 4, 3, 1 } },
    { DSCLASS_SERIFS_modern_fat,	{ 4, 3, 2 } },
    { DSCLASS_SERIFS_cont,		{ 4, 4, 0 } },
    { DSCLASS_SERIFS_cont_eclectic,	{ 4, 4, 1 } },
    { DSCLASS_SERIFS_cont_fine,		{ 4, 4, 2 } },
    { DSCLASS_SERIFS_cont_lettering,	{ 4, 4, 3 } },
    { DSCLASS_SERIFS_legi,		{ 4, 5, 0 } },
    { DSCLASS_SERIFS_legi_rounded,	{ 4, 5, 1 } },
    { DSCLASS_SERIFS_legi_square,	{ 4, 5, 2 } },
    { DSCLASS_SERIFS_square,		{ 4, 6, 0 } },
    { DSCLASS_SERIFS_square_monotone,	{ 4, 6, 1 } },
    { DSCLASS_SERIFS_square_clarendon,	{ 4, 6, 2 } },
    { DSCLASS_SERIFS_square_french,	{ 4, 6, 3 } },
    { DSCLASS_SERIFS_square_short,	{ 4, 6, 4 } },
    { DSCLASS_SERIFS_square_type,	{ 4, 6, 5 } },
    { DSCLASS_SERIFS_square_dot,	{ 4, 6, 6 } },
    { DSCLASS_SERIFS_latin,		{ 4, 7, 0 } },
    { DSCLASS_SERIFS_latin_solid,	{ 4, 7, 1 } },
    { DSCLASS_SERIFS_latin_incised,	{ 4, 7, 2 } },
    { DSCLASS_SERIFS_engraving,		{ 4, 8, 0 } },
    { DSCLASS_SERIFS_engraving_barbed,	{ 4, 8, 1 } },
    { DSCLASS_SERIFS_engraving_strait,	{ 4, 8, 2 } },
    { DSCLASS_SERIFS_art,		{ 4, 9, 0 } },
    { DSCLASS_SERIFS_art_french,	{ 4, 9, 1 } },
    { DSCLASS_SERIFS_computer,		{ 4, 10, 0 } },
    { DSCLASS_SERIFS_computer_ocr,	{ 4, 10, 1 } },
    { DSCLASS_SERIFS_computer_dig,	{ 4, 10, 2 } },
    { DSCLASS_SERIFS_misc,		{ 4, 11, 0 } },

    { DSCLASS_SANS,			{ 5, 0, 0 } },
    { DSCLASS_SANS_gothic,		{ 5, 1, 0 } },
    { DSCLASS_SANS_gothic_grot,		{ 5, 1, 1 } },
    { DSCLASS_SANS_gothic_neo,		{ 5, 1, 2 } },
    { DSCLASS_SANS_gothic_type,		{ 5, 1, 3 } },

    { DSCLASS_SCRIPTS,			{ 6, 0, 0 } },

    { DSCLASS_ORNAM,			{ 7, 0, 0 } }
};

#define NDSCLASSES  ((INT_type)(sizeof(dsclasses) / sizeof(dsclasses[0])))

INT_type FONT_Final()
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

INT_type FONT_Init()
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

INT_type UnspecifyFont(font)
    IN OUT FONT_REFERENCE_type  *font;
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = UnspecifyStructName(&font->fontname);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    rval = UnspecifyStructName(&font->difntnam);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    font -> propdata = ISO_PROPDATA_unspec;
    rval = UnspecifyStructName(&font->typeface);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    rval = UnspecifyStructName(&font->family);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    font -> posture = ISO_POSTURE_unspec;
    font -> weight = ISO_WEIGHT_unspec;
    font -> propwdth = ISO_PROPWDTH_unspec;
    font -> numchars = -1;
    font -> dsnsize.second = 0;
    font -> minsize.second = 0;
    font -> maxsize.second = 0;
    font -> dsclass = NULL_SEQUENCE;
    font -> structur = ISO_STRUCTUR_unspec;
    font -> mnfeatsz.second = 0;
    font ->modes.specified = BOOL_false;


	if (UnspecifyModalFont(&font->modes.iso_0) == ERROR_INT ||
	    UnspecifyModalFont(&font->modes.iso_180) == ERROR_INT ||
	    UnspecifyModalFont(&font->modes.iso_270) == ERROR_INT) {
		rval = ERROR_INT;
		goto LEAVE;
	}

    rval = UnspecifyStructName(&font->mode);
    if (rval == ERROR_INT) {
	goto LEAVE;
    }

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}


static INT_type UnspecifyModalFont(modal)
    IN OUT MODAL_FONT_ATTR_type	*modal;
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    
    modal -> specified = BOOL_false;

    modal -> escapdir.second = 0;
    modal -> esclass = ISO_ESCLASS_unspec;
    modal -> avgesc.second = 0;
    modal -> avglcesc.second = 0;
    modal -> avgcpesc.second = 0;
    modal -> avgwtesc.second = 0;
    modal -> tabesc.tx.second = 0;
    modal -> tabesc.ty.second = 0;
    modal -> mfx.second = 0;
    modal -> mlx.second = 0;
    modal -> mbx.second = 0;
    modal -> mrx.second = 0;
    modal -> minampl.second = 0;
    modal -> maxampl.second = 0;
	modal -> scores.specified = BOOL_false;
	if (UnspecifyScore(&modal->scores.rscore) == ERROR_INT ||
	    UnspecifyScore(&modal->scores.lscore) == ERROR_INT ||
	    UnspecifyScore(&modal->scores.tscore) == ERROR_INT) {
		rval = ERROR_INT;
		goto LEAVE;
	}
	modal -> varscrpt.specified = BOOL_false;
	if (UnspecifyVarScript(&modal->varscrpt.lscript) == ERROR_INT ||
	    UnspecifyVarScript(&modal->varscrpt.rscript) == ERROR_INT) {
		rval = ERROR_INT;
		goto LEAVE;
	}
    modal -> minl2lsp.second = 0;
    modal -> minanasc.second = 0;
    modal -> maxanasc.second = 0;

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type UnspecifyScore(score)
    IN OUT SCORE_type	*score;
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    score -> specified = BOOL_false;
    score -> scorpos.second = 0;
    score -> scorthik.second = 0;

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type UnspecifyVarScript(script)
    IN OUT VARIANT_SCRIPT_type	*script;
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    script -> specified = BOOL_false;
    script -> vxoffset.second = 0;
    script -> vyoffset.second = 0;
    script -> vxscale.second = 0;
    script -> vyscale.second = 0;

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type UnspecifyStructName(where)
    OUT STRUCTURED_NAME_type	*where;

{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    where -> structured_name_tag = STRUCTURED_NAME_UNSPEC_tag;
    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

SEQUENCE_INT_type Dsclass2Ints(value)
    IN VALUE_type   value;
{
    REGISTER SEQUENCE_INT_type	rval;
    REGISTER INT_type		i;
    REGISTER INT_type		dummy;
    REGISTER INT_type		length;
    REGISTER INT_type		j;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Search table for matching value */
    for (i=0; i<NDSCLASSES; i++) {
	if (dsclasses[i].name == value) {
	    if (dsclasses[i].seq[1] == 0) {
		length = 1;
	    } else if (dsclasses[i].seq[2] == 0) {
		length = 2;
	    } else {
		length = 3;
	    }
	    rval = MakeSequence(SEQUENCE_INT_tag, length);
	    if (rval == ERROR_SEQUENCE) {
		goto LEAVE;
	    }
	    for (j=0; j<length; j++) {
		rval -> sequence_value.ints[j] = dsclasses[i].seq[j];
	    }
	    goto LEAVE;
	}
    }

    /* Didn't find a match */
    rval = ERROR_SEQUENCE;
    TKError = BADPARM_err;
    dummy = TKERR_StartErrMsg();
    dummy = TKERR_FormatInt("[Dsclass2Ints]: Bad parm `value': %d", (INT_type) value);
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

VALUE_type Ints2Dsclass(seq)
    IN SEQUENCE_INT_type   seq;
{
    REGISTER VALUE_type	rval;
    REGISTER INT_type	i;
    REGISTER INT_type	length;
    REGISTER INT_type	*ints;
    REGISTER INT_type	dummy;
    REGISTER INT_type	first;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Get length & check it */
    length = seq -> length;
    if (length > 3) {
	rval = DSCLASS_NOMATCH;
	goto LEAVE;
    }
    if (length <= 0) {
	rval = DSCLASS_ERROR;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt("[Ints2Dsclass]: Invalid length for sequence: %d",
				length);
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Point to integers */
    ints = seq -> sequence_value.ints;

    /* Pull off first digit */
    first = ints[0];

    /* Search table for matching sequence */
    for (i=0; i<NDSCLASSES; i++) {
	if (first < dsclasses[i].seq[0]) {
	    rval = DSCLASS_NOMATCH;
	    goto LEAVE;
	}
	if (first == dsclasses[i].seq[0] &&
	    ints[1] == dsclasses[i].seq[1] &&
	    ints[2] == dsclasses[i].seq[2]) {
		rval = dsclasses[i].name;
		goto LEAVE;
	}
    }

    rval = DSCLASS_NOMATCH;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

#ifdef NOTDEF

static STRUCTURED_NAME_type *MakeStructName()
{
    STRUCTURED_NAME_type    *rval;
    POINTER_type	    p;
    INT_type		    dummy;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    p = MM_Malloc(sizeof(STRUCTURED_NAME_type));
    if (p == ERROR_POINTER) {
	rval = (STRUCTURED_NAME_type *) 0;
	goto LEAVE;
    }
    rval = (STRUCTURED_NAME_type *) p;
    if (UnspecifyStructName(rval) == ERROR_INT) {
	dummy = MM_Free(p);
	rval = (STRUCTURED_NAME_type *) 0;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

#endif
    /* NOTDEF */
