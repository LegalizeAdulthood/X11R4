
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr14.h,v 1.1 89/09/21 06:17:48 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr14.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr14.h,v $
 * Revision 1.1  89/09/21  06:17:48  mss
 * Initial revision
 * 
 * Revision 1.6  89/09/11  14:33:46  mss
 * removed leading #
 * 
 * Revision 1.5  89/01/03  16:17:18  mss
 * Misspelled ATTR_Copy_Fields in long name version of file
 * 
 * Revision 1.4  88/12/21  14:36:51  annm
 * cosmetic change to improve readability
 * 
 * Revision 1.3  88/11/01  16:02:16  annm
 * fix of font monster alloc/copy bug for null sequence value in the sequence of ints field
 * 
 * Revision 1.2  88/10/19  12:07:44  mss
 * Made structures external
 * 
 * Revision 1.1  88/10/14  16:10:10  annm
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_ATTR14
#define TK_INCLUDED_ATTR14

#undef EXTERN

#ifdef TK_GVARS_ATTR14
#define EXTERN
#else
#define EXTERN extern
#endif


#ifdef TK_IMPL_ATTR

/* real names for most routines in attr14.c come from rnames.h */

#ifndef USE_LONG_NAMES

#define ATTR_FontPairCopy	gatfpc
#define ATTR_Copy_Fields	gatcpf

#endif

/* declarations for routines in attr14.c */

EXTERN INT_type ATTR_FontPairCopy();

EXTERN INT_type ATTR_Copy_Fields();

/* type indexing */

#define DFNPRE_FIELD		((INT_type) 1)
#define STRUCTURED_NAME_TYPE	((INT_type) 2)
#define INT_TYPE		((INT_type) 3)
#define INT_PAIR_TYPE		((INT_type) 4)
#define WRITING_MODES_TYPE	((INT_type) 5)
#define SEQUENCE_INT_TYPE	((INT_type) 6)
#define MODAL_FONT_ATTR_TYPE	((INT_type) 7)
#define TABESC			((INT_type) 8)
#define SCORES_TYPE		((INT_type) 9)
#define VARIANT_SCRIPTS_TYPE	((INT_type) 10)
#define SCORE_TYPE		((INT_type) 11)
#define VARIANT_SCRIPT_TYPE	((INT_type) 12)


#ifndef USE_LONG_NAMES


#define ATTR_Copy_Dfnpre		gfrcdf
#define ATTR_Copy_Structured_Name	gfrcsn
#define ATTR_Copy_ISO9541_Id_Type	gfrcis
#define ATTR_Copy_Int_Type		gfrcit
#define ATTR_Copy_Int_Pair_Type		gfrcip
#define ATTR_Copy_Sequence_Int_Type	gfrcsi
#define ATTR_Copy_Writing_Modes_Type	gfrcwm
#define ATTR_Copy_Modal_Font_Attr_Type	gftcmf
#define ATTR_Copy_Tabesc		gfrctb
#define ATTR_Copy_Scores_Type		gfrcst
#define ATTR_Copy_Variant_Scripts_Type	gfrcvs
#define ATTR_Copy_Score_Type		gfrcsc
#define ATTR_Copy_Variant_Script_Type	gfrcvt

#define ATTR_GetFontReferenceBase	gagfrb
#define ATTR_GetWritingModesBase	gagwmb
#define ATTR_GetModalFontAttrBase	ggmfab
#define ATTR_GetTabescBase		gagteb
#define ATTR_GetScoresBase		ggscsb
#define ATTR_GetScoreBase		gagscb
#define ATTR_GetVariantScriptsBase	ggvssb
#define ATTR_GetVariantScriptBase	gagvsb

/* data structure real names */

#define FontReferenceFields	gatfrf
#define FontReferenceInfo	gatfri
#define WritingModesFields	gatwmf
#define WritingModesInfo	gatwmi
#define ModalFontAttrFields	gamfaf
#define ModalFontAttrInfo	gamfai
#define TabescFields		gattef
#define TabescInfo		gattei
#define ScoresFields		gascsf
#define ScoresInfo		gascsi
#define ScoreFields		gatscf
#define ScoreInfo		gatsci
#define VariantScriptsFields	gavssf
#define VariantScriptsInfo	gavssi
#define VariantScriptFields	gatvsf
#define VariantScriptInfo	gatvsi

#endif 

/* declarations for type specific copy  canonical procedures */

EXTERN INT_type ATTR_Copy_Dfnpre();
EXTERN INT_type ATTR_Copy_Structured_Name();
EXTERN INT_type ATTR_Copy_ISO9541_Id_Type();
EXTERN INT_type ATTR_Copy_Int_Type();
EXTERN INT_type ATTR_Copy_Int_Pair_Type();
EXTERN INT_type ATTR_Copy_Sequence_Int_Type();
EXTERN INT_type ATTR_Copy_Writing_Modes_Type();
EXTERN INT_type ATTR_Copy_Modal_Font_Attr_Type();
EXTERN INT_type ATTR_Copy_Tabesc();
EXTERN INT_type ATTR_Copy_Scores_Type();
EXTERN INT_type ATTR_Copy_Variant_Scripts_Type();
EXTERN INT_type ATTR_Copy_Score_Type();
EXTERN INT_type ATTR_Copy_Variant_Script_Type();

EXTERN POINTER_type ATTR_GetFontReferenceBase();
EXTERN POINTER_type ATTR_GetWritingModesBase();
EXTERN POINTER_type ATTR_GetModalFontAttrBase();
EXTERN POINTER_type ATTR_GetTabescBase();
EXTERN POINTER_type ATTR_GetScoresBase();
EXTERN POINTER_type ATTR_GetScoreBase();
EXTERN POINTER_type ATTR_GetVariantScriptsBase();
EXTERN POINTER_type ATTR_GetVariantScriptBase();

typedef struct font_branch_info_struct {
	CHAR_type type_name[50];
	INT_type table_size;
	POINTER_type (*GetBase)();
} FONT_BRANCH_INFO_type;

typedef struct fields_struct {
	    CHAR_type field_name[50];
	    CHAR_type field_type[50];
	    INT_type field_index;
		/* index within a given FIELDS_type */
	    INT_type universal_field_index;
		/* over all possible fields */
	    INT_type type_index;
	    INT_type (*Copy)();
	    INT_type (*CkVal)();
} FIELDS_type;

/* 'universal' indexing of fields */

#define DFNPRE_UNIV	    ((INT_type) 1)
#define FONTNAME_UNIV	    ((INT_type) 2)
#define DIFNTNAM_UNIV	    ((INT_type) 3)
#define PROPDATA_UNIV	    ((INT_type) 4)
#define TYPEFACE_UNIV	    ((INT_type) 5)
#define FAMILY_UNIV	    ((INT_type) 6)
#define POSTURE_UNIV	    ((INT_type) 7)
#define WEIGHT_UNIV	    ((INT_type) 8)
#define PROPWDTH_UNIV	    ((INT_type) 9)
#define NUMCHARS_UNIV	    ((INT_type) 10)
#define DSNSIZE_UNIV	    ((INT_type) 11)
#define MINSIZE_UNIV	    ((INT_type) 12)
#define MAXSIZE_UNIV	    ((INT_type) 13)
#define DSCLASS_UNIV	    ((INT_type) 14)
#define STRUCTUR_UNIV	    ((INT_type) 15)
#define MNFEATSZ_UNIV	    ((INT_type) 16)
#define MODES_UNIV	    ((INT_type) 17)
#define MODE_UNIV	    ((INT_type) 19)
#define ISO_0_UNIV	    ((INT_type) 20)
#define ISO_180_UNIV	    ((INT_type) 21)
#define ISO_270_UNIV	    ((INT_type) 22)
#define ESCAPDIR_UNIV	    ((INT_type) 23)
#define ESCLASS_UNIV	    ((INT_type) 24)
#define AVGESC_UNIV	    ((INT_type) 25)
#define AVGLCESC_UNIV	    ((INT_type) 26)
#define AVGCPESC_UNIV	    ((INT_type) 27)
#define AVGWTESC_UNIV	    ((INT_type) 28)
#define TABESC_FIELD_UNIV   ((INT_type) 29)
#define MFX_UNIV	    ((INT_type) 30)
#define MLX_UNIV	    ((INT_type) 31)
#define MBX_UNIV	    ((INT_type) 32)
#define MRX_UNIV	    ((INT_type) 33)
#define MINAMPL_UNIV	    ((INT_type) 34)
#define MAXAMPL_UNIV	    ((INT_type) 35)
#define SCORES_FIELD_UNIV   ((INT_type) 36)
#define VARSCRPT_FIELD_UNIV ((INT_type) 37)
#define MINL2LSP_UNIV	    ((INT_type) 38)
#define MINANASC_UNIV	    ((INT_type) 39)
#define MAXANASC_UNIV	    ((INT_type) 40)
#define TX_UNIV		    ((INT_type) 41)
#define TY_UNIV		    ((INT_type) 42)
#define RSCORE_UNIV	    ((INT_type) 43)
#define LSCORE_UNIV	    ((INT_type) 44)
#define TSCORE_UNIV	    ((INT_type) 45)
#define SCORPOS_UNIV	    ((INT_type) 46)
#define SCORTHIK_UNIV	    ((INT_type) 47)
#define LSCRIPT_UNIV	    ((INT_type) 48)
#define RSCRIPT_UNIV	    ((INT_type) 49)
#define VXOFFSET_UNIV	    ((INT_type) 50)
#define VYOFFSET_UNIV	    ((INT_type) 51)
#define VXSCALE_UNIV	    ((INT_type) 52)
#define VYSCALE_UNIV	    ((INT_type) 53)

/* standard indexing for common font reference fields */
#define DFNPRE	    ((INT_type) 0)

/* indexing for font reference fields */
#define FONTNAME    ((INT_type) 1)
#define DIFNTNAM    ((INT_type) 2)
#define PROPDATA    ((INT_type) 3)
#define TYPEFACE    ((INT_type) 4)
#define FAMILY	    ((INT_type) 5)
#define POSTURE	    ((INT_type) 6)
#define WEIGHT	    ((INT_type) 7)
#define PROPWDTH    ((INT_type) 8)
#define NUMCHARS    ((INT_type) 9)
#define DSNSIZE	    ((INT_type) 10)
#define MINSIZE	    ((INT_type) 11)
#define MAXSIZE	    ((INT_type) 12)
#define DSCLASS	    ((INT_type) 13)
#define STRUCTUR    ((INT_type) 14)
#define MNFEATSZ    ((INT_type) 15)
#define MODES	    ((INT_type) 16)
#define MODE	    ((INT_type) 17)

/* fields in the font reference structure */

EXTERN FIELDS_type FontReferenceFields[] 

#ifdef TK_GVARS_ATTR14
= {

{
    "dfnpre",
    "default name prefix",
    DFNPRE,
    DFNPRE_UNIV,
    DFNPRE_FIELD,
    ATTR_Copy_Dfnpre,
    ATTR_CkVal_Dfnpre,
},
{
    "fontname",
    "font name type",
    FONTNAME,
    FONTNAME_UNIV,
    STRUCTURED_NAME_TYPE,
    ATTR_Copy_Structured_Name,
    ATTR_CkVal_Structured_Name
},
{
    "difntnam",
    "structured name type",
    DIFNTNAM,
    DIFNTNAM_UNIV,
    STRUCTURED_NAME_TYPE,
    ATTR_Copy_Structured_Name,
    ATTR_CkVal_Structured_Name
},
{
    "propdata",
    "INT_type",
    PROPDATA,
    PROPDATA_UNIV,
    INT_TYPE,
    ATTR_Copy_Int_Type,
    ATTR_CkVal_Int_Type
},
{
    "typeface",
    "structured name type",
    TYPEFACE,
    TYPEFACE_UNIV,
    STRUCTURED_NAME_TYPE,
    ATTR_Copy_Structured_Name,
    ATTR_CkVal_Structured_Name
},
{
    "family",
    "structured name type",
    FAMILY,
    FAMILY_UNIV,
    STRUCTURED_NAME_TYPE,
    ATTR_Copy_Structured_Name,
    ATTR_CkVal_Structured_Name
},
{
    "posture", 
    "INT_type",
    POSTURE,
    POSTURE_UNIV,
    INT_TYPE,
    ATTR_Copy_Int_Type,
    ATTR_CkVal_Int_Type    
},
{
    "weight", 
    "INT_type",
    WEIGHT,
    WEIGHT_UNIV,
    INT_TYPE,
    ATTR_Copy_Int_Type,
    ATTR_CkVal_Int_Type    
},
{
    "propwdth", 
    "INT_type",
    PROPWDTH,
    PROPWDTH_UNIV,
    INT_TYPE,
    ATTR_Copy_Int_Type,
    ATTR_CkVal_Int_Type    
},
{
    "numchars", 
    "INT_type",
    NUMCHARS,
    NUMCHARS_UNIV,
    INT_TYPE,
    ATTR_Copy_Int_Type,
    ATTR_CkVal_Int_Type    
},
{
    "dsnsize", 
    "INT_PAIR_type",
    DSNSIZE,
    DSNSIZE_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type
},
{
    "minsize", 
    "INT_PAIR_type",
    MINSIZE,
    MINSIZE_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type    
},
{
    "maxsize", 
    "INT_PAIR_type",
    MAXSIZE,
    MAXSIZE_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type    
},
{
    "dsclass", 
    "SEQUENCE_INT_type",
    DSCLASS,
    DSCLASS_UNIV,
    SEQUENCE_INT_TYPE,
    ATTR_Copy_Sequence_Int_Type,
    ATTR_CkVal_Sequence_Int_Type
},
{
    "structur", 
    "INT_type",
    STRUCTUR,
    STRUCTUR_UNIV,
    INT_TYPE,
    ATTR_Copy_Int_Type,
    ATTR_CkVal_Int_Type    
},
{
    "mnfeatsz", 
    "INT_PAIR_type",
    MNFEATSZ,
    MNFEATSZ_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type
},
{
    "modes", 
    "WRITING_MODES_type",
    MODES,
    MODES_UNIV,
    WRITING_MODES_TYPE,
    ATTR_Copy_Writing_Modes_Type,
    ATTR_CkVal_Writing_Modes_Type
},
{
    "mode", 
    "structured name type",
    MODE,
    MODE_UNIV,
    STRUCTURED_NAME_TYPE,
    ATTR_Copy_Structured_Name,
    ATTR_CkVal_Structured_Name
}

}
#endif
;



EXTERN FONT_BRANCH_INFO_type FontReferenceInfo 

#ifdef TK_GVARS_ATTR14
= {
    /* type name */ "Font Reference",
    /* field table size */ (sizeof(FontReferenceFields)/sizeof(FIELDS_type)),
    /* get base routine */ ATTR_GetFontReferenceBase    
}
#endif
;

/* indexing for writing mode fields */

#define ISO_0	    ((INT_type) 1)
#define ISO_180	    ((INT_type) 2)
#define ISO_270	    ((INT_type) 3)

EXTERN FIELDS_type WritingModesFields[]

#ifdef TK_GVARS_ATTR14
= {

{
    "dfnpre",
    "default name prefix",
    DFNPRE,
    DFNPRE_UNIV,
    DFNPRE_FIELD,
    ATTR_Copy_Dfnpre,
    ATTR_CkVal_Dfnpre,
},
{
    "iso_0",
    "modal font attr type",
    ISO_0,
    ISO_0_UNIV,
    MODAL_FONT_ATTR_TYPE,
    ATTR_Copy_Modal_Font_Attr_Type,
    ATTR_CkVal_Modal_Font_Attr_Type,
},
{
    "iso_180",
    "modal font attr type",
    ISO_180,
    ISO_180_UNIV,
    MODAL_FONT_ATTR_TYPE,
    ATTR_Copy_Modal_Font_Attr_Type,
    ATTR_CkVal_Modal_Font_Attr_Type,
},
{
    "iso_270",
    "modal font attr type",
    ISO_270,
    ISO_270_UNIV,
    MODAL_FONT_ATTR_TYPE,
    ATTR_Copy_Modal_Font_Attr_Type,
    ATTR_CkVal_Modal_Font_Attr_Type,
}

}

#endif
;

EXTERN FONT_BRANCH_INFO_type WritingModesInfo

#ifdef TK_GVARS_ATTR14
= {
    "writing modes type",
    (sizeof(WritingModesFields)/sizeof(FIELDS_type)),
    ATTR_GetWritingModesBase
}

#endif
;

/* indexing of modal font attr fields */

#define ESCAPDIR	    ((INT_type) 1)
#define ESCLASS		    ((INT_type) 2)
#define AVGESC		    ((INT_type) 3)
#define AVGLCESC	    ((INT_type) 4)
#define AVGCPESC	    ((INT_type) 5)
#define AVGWTESC	    ((INT_type) 6)
#define TABESC_FIELD	    ((INT_type) 7)
#define MFX		    ((INT_type) 8)
#define MLX		    ((INT_type) 9)
#define MBX		    ((INT_type) 10)
#define MRX		    ((INT_type) 11)
#define MINAMPL		    ((INT_type) 12)
#define MAXAMPL		    ((INT_type) 13)
#define SCORES_FIELD	    ((INT_type) 14)
#define VARSCRPT_FIELD	    ((INT_type) 15)
#define MINL2LSP	    ((INT_type) 16)
#define MINANASC	    ((INT_type) 17)
#define MAXANASC	    ((INT_type) 18)

EXTERN FIELDS_type ModalFontAttrFields[]

#ifdef TK_GVARS_ATTR14
= {

{
    "dfnpre",
    "default name prefix",
    DFNPRE,
    DFNPRE_UNIV,
    DFNPRE_FIELD,
    ATTR_Copy_Dfnpre,
    ATTR_CkVal_Dfnpre,
},
{
    "escapdir",
    "int pair type",
    ESCAPDIR,
    ESCAPDIR_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type
},
{
    "esclass",
    "int type",
    ESCLASS,
    ESCLASS_UNIV,
    INT_TYPE,
    ATTR_Copy_Int_Type,
    ATTR_CkVal_Int_Type    
},
{
    "avgesc",
    "int pair type",
    AVGESC,
    AVGESC_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type  
},
{
    "avglcesc",
    "int pair type",
    AVGLCESC,
    AVGLCESC_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type
},
{
    "avgcpesc",
    "int pair type",
    AVGCPESC,
    AVGCPESC_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type    
},
{
    "avgwtesc",
    "int pair type",
    AVGWTESC,
    AVGWTESC_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type
},
{
    "tabesc",
    "tabesc structure",
    TABESC_FIELD,
    TABESC_FIELD_UNIV,
    TABESC,
    ATTR_Copy_Tabesc,
    ATTR_CkVal_Tabesc
},
{
    "mfx",
    "int pair type",
    MFX,
    MFX_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type
},
{
    "mlx",
    "int pair type",
    MLX,
    MLX_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type
},
{
    "mbx",
    "int pair type",
    MBX,
    MBX_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type
},
{
    "mrx",
    "int pair type",
    MRX,
    MRX_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type
},
{
    "minampl",
    "int pair type",
    MINAMPL,
    MINAMPL_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type
},
{
    "maxampl",
    "int pair type",
    MAXAMPL,
    MAXAMPL_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type
},
{
    "scores",
    "SCORES type",
    SCORES_FIELD,
    SCORES_FIELD_UNIV,
    SCORES_TYPE,
    ATTR_Copy_Scores_Type,
    ATTR_CkVal_Scores_Type    
},
{
    "varscrpt",
    "variant scripts type",
    VARSCRPT_FIELD,
    VARSCRPT_FIELD_UNIV,
    VARIANT_SCRIPTS_TYPE,
    ATTR_Copy_Variant_Scripts_Type,
    ATTR_CkVal_Variant_Scripts_Type
},
{
    "minl2lsp",
    "int pair type",
    MINL2LSP,
    MINL2LSP_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type
},
{
    "minanasc",
    "int pair type",
    MINANASC,
    MINANASC_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type
},
{
    "maxanasc",
    "int pair type",
    MAXANASC,
    MAXANASC_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type
}

}

#endif
;

EXTERN FONT_BRANCH_INFO_type ModalFontAttrInfo

#ifdef TK_GVARS_ATTR14
= {
    "modal font attr type",
    (sizeof(ModalFontAttrFields)/sizeof(FIELDS_type)),
    ATTR_GetModalFontAttrBase
}
#endif
;

/* indexing for tabesc fields */
#define TX	    ((INT_type) 0)
#define TY	    ((INT_type) 1)

EXTERN FIELDS_type TabescFields[]

#ifdef TK_GVARS_ATTR14
= {

{
    "tx",
    "int pair type",
    TX,
    TX_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type    
},
{
    "ty",
    "int pair type",
    TY,
    TY_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type    
}

}

#endif

;

/* info for the tabesc type */

EXTERN FONT_BRANCH_INFO_type TabescInfo

#ifdef TK_GVARS_ATTR14
= {
    "tabesc ",
    (sizeof(TabescFields)/sizeof(FIELDS_type)),
    ATTR_GetTabescBase
    
}

#endif
;


/* field indexing for scores type */
#define RSCORE	((INT_type) 1)
#define LSCORE	((INT_type) 2)
#define TSCORE	((INT_type) 3)

EXTERN FIELDS_type ScoresFields []

#ifdef TK_GVARS_ATTR14
= {

{
    "dfnpre",
    "default name prefix",
    DFNPRE,
    DFNPRE_UNIV,
    DFNPRE_FIELD,
    ATTR_Copy_Dfnpre,
    ATTR_CkVal_Dfnpre,
},
{
    "rscore",
    "score type",
    RSCORE,
    RSCORE_UNIV,
    SCORE_TYPE,
    ATTR_Copy_Score_Type,
    ATTR_CkVal_Score_Type
},
{
    "lscore",
    "score type",
    LSCORE,
    LSCORE_UNIV,
    SCORE_TYPE,
    ATTR_Copy_Score_Type,
    ATTR_CkVal_Score_Type
},
{
    "tscore",
    "score type",
    TSCORE,
    TSCORE_UNIV,
    SCORE_TYPE,
    ATTR_Copy_Score_Type,
    ATTR_CkVal_Score_Type
}
}
#endif
;

EXTERN FONT_BRANCH_INFO_type ScoresInfo 

#ifdef TK_GVARS_ATTR14

= {
    "scores type",
    (sizeof(ScoresFields)/sizeof(FIELDS_type)),
    ATTR_GetScoresBase
}

#endif
;

/* score specific field indexing */

#define SCORPOS	    ((INT_type) 0)
#define SCORTHIK    ((INT_type) 1)

EXTERN FIELDS_type ScoreFields[]

#ifdef TK_GVARS_ATTR14
= {

{
    "scorpos",
    "int pair type",
    SCORPOS,
    SCORPOS_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type
},
{
    "scorthik",
    "int pair type",
    SCORTHIK,
    SCORTHIK_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type
}

}

#endif
;

/* info for score type */

EXTERN FONT_BRANCH_INFO_type ScoreInfo

#ifdef TK_GVARS_ATTR14
= {
    "score type",
    (sizeof(ScoreFields)/sizeof(FIELDS_type) ),
    ATTR_GetScoreBase
}

#endif
;

/* indexing for variant scripts fields */
#define LSCRIPT	    ((INT_type) 1)
#define RSCRIPT	    ((INT_type) 2)

EXTERN FIELDS_type VariantScriptsFields[]

#ifdef TK_GVARS_ATTR14
= {

{
    "dfnpre",
    "default name prefix",
    DFNPRE,
    DFNPRE_UNIV,
    DFNPRE_FIELD,
    ATTR_Copy_Dfnpre,
    ATTR_CkVal_Dfnpre,
},
{
    "lscript",
    "variant script type",
    LSCRIPT,
    LSCRIPT_UNIV,
    VARIANT_SCRIPT_TYPE,
    ATTR_Copy_Variant_Script_Type,
    ATTR_CkVal_Variant_Script_Type
},
{
    "rscript",
    "variant script type",
    RSCRIPT,
    RSCRIPT_UNIV,
    VARIANT_SCRIPT_TYPE,
    ATTR_Copy_Variant_Script_Type,
    ATTR_CkVal_Variant_Script_Type
}


}

#endif
;

/* info for the variant scripts type */

EXTERN FONT_BRANCH_INFO_type VariantScriptsInfo

#ifdef TK_GVARS_ATTR14
= {
    "variant scripts type",
    (sizeof(VariantScriptsFields)/sizeof(FIELDS_type) ),
    ATTR_GetVariantScriptsBase
}

#endif
;

/* indexing for variant script fields */
#define VXOFFSET    ((INT_type) 0)
#define VYOFFSET    ((INT_type) 1)
#define VXSCALE	    ((INT_type) 2)
#define VYSCALE	    ((INT_type) 3)

EXTERN FIELDS_type VariantScriptFields[]

#ifdef TK_GVARS_ATTR14
= {

{
    "vxoffset",
    "int pair type",
    VXOFFSET,
    VXOFFSET_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type    
},
{
    "vyoffset",
    "int pair type",
    VYOFFSET,
    VYOFFSET_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type    
},
{
    "vxscale",
    "int pair type",
    VXSCALE,
    VXSCALE_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type    
},
{
    "vyscale",
    "int pair type",
    VYSCALE,
    VYSCALE_UNIV,
    INT_PAIR_TYPE,
    ATTR_Copy_Int_Pair_Type,
    ATTR_CkVal_Int_Pair_Type    
}

}

#endif
;

/* info for the variant script type */

EXTERN FONT_BRANCH_INFO_type VariantScriptInfo

#ifdef TK_GVARS_ATTR14
= {
    "variant script type",
    (sizeof(VariantScriptFields)/sizeof(FIELDS_type)),
    ATTR_GetVariantScriptBase
    
}

#endif
;

#endif

#endif
