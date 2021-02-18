
/*

$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/font.h,v 1.3 89/10/26 14:09:31 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/font.h,v $

		(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	font.h,v $
 * Revision 1.3  89/10/26  14:09:31  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.2  89/09/29  11:40:19  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:30:21  mss
 * Initial revision
 * 
 * Revision 1.3  88/09/20  15:58:48  jr
 * Shorten long names.
 * 
 * Revision 1.2  88/09/19  13:12:38  annm
 * fixed tkerr.h include problem (no GENINCLDIR
 * 
 * Revision 1.1  88/09/12  17:42:02  jr
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_FONT
#define	TK_INCLUDED_FONT

#ifdef TK_TKI_LABEL
#define TK_FONT
#else
#ifdef TK_IMPL_FONT
#define	TK_FONT
#else
#define AP_FONT
#endif
#endif

#include <lcsite.h>
#include <errors.h>
#include <apirt.h>
#include <apityp.h>

/* Everyone sees these definitions */

    /* Sub-parameter definitions */
#define ISO_PROPDATA_unspec		((INT_type)-1)
#define ISO_PROPDATA_no			((INT_type)2)
#define ISO_PROPDATA_yes		((INT_type)3)

#define ISO_POSTURE_unspec		((INT_type)-1)
#define ISO_POSTURE_upright		((INT_type)1)
#define ISO_POSTURE_clockwise		((INT_type)2)
#define ISO_POSTURE_counter		((INT_type)3)
#define ISO_POSTURE_italic		((INT_type)4)
#define ISO_POSTURE_other		((INT_type)5)

#define ISO_WEIGHT_unspec		((INT_type)-1)
#define ISO_WEIGHT_ultra_light		((INT_type)1)
#define ISO_WEIGHT_extra_light		((INT_type)2)
#define ISO_WEIGHT_light		((INT_type)3)
#define ISO_WEIGHT_semi_light		((INT_type)4)
#define ISO_WEIGHT_medium		((INT_type)5)
#define ISO_WEIGHT_semi_bold		((INT_type)6)
#define ISO_WEIGHT_bold			((INT_type)7)
#define ISO_WEIGHT_extra_bold		((INT_type)8)
#define ISO_WEIGHT_ultra_bold		((INT_type)9)

#define ISO_PROPWDTH_unspec		((INT_type)-1)
#define ISO_PROPWDTH_ultra_condensed	((INT_type)1)
#define ISO_PROPWDTH_extra_condensed	((INT_type)2)
#define ISO_PROPWDTH_condensed		((INT_type)3)
#define ISO_PROPWDTH_semi_condensed	((INT_type)4)
#define ISO_PROPWDTH_medium		((INT_type)5)
#define ISO_PROPWDTH_semi_expanded	((INT_type)6)
#define ISO_PROPWDTH_expanded		((INT_type)7)
#define ISO_PROPWDTH_extra_expanded	((INT_type)8)
#define ISO_PROPWDTH_ultra_expanded	((INT_type)9)

#define ISO_STRUCTUR_unspec		((INT_type)-1)
#define ISO_STRUCTUR_solid		((INT_type)1)
#define ISO_STRUCTUR_outline		((INT_type)2)
#define ISO_STRUCTUR_inline		((INT_type)3)
#define ISO_STRUCTUR_shadow		((INT_type)4)
#define ISO_STRUCTUR_patterned		((INT_type)5)

#define ISO_ESCLASS_unspec		((INT_type)-1)
#define ISO_ESCLASS_fixed		((INT_type)1)
#define ISO_ESCLASS_variable		((INT_type)2)

    /* ISO/DSCLASS values */
#define DSCLASS_ERROR			    ((VALUE_type) 1)
#define DSCLASS_NOMATCH			    ((VALUE_type) 2)
#define DSCLASS_UNCIALS			    ((VALUE_type) 3)
#define DSCLASS_UNCIALS_1		    ((VALUE_type) 4)
#define DSCLASS_UNCIALS_1_sans		    ((VALUE_type) 5)
#define DSCLASS_UNCIALS_1_serif		    ((VALUE_type) 6)
#define DSCLASS_UNCIALS_2		    ((VALUE_type) 7)
#define DSCLASS_UNCIALS_2_sans		    ((VALUE_type) 8)
#define DSCLASS_UNCIALS_2_serif		    ((VALUE_type) 9)
#define DSCLASS_INSCR			    ((VALUE_type) 10)
#define DSCLASS_INSCR_solids		    ((VALUE_type) 11)
#define DSCLASS_INSCR_solids_sans	    ((VALUE_type) 12)
#define DSCLASS_INSCR_solids_serif	    ((VALUE_type) 13)
#define DSCLASS_INSCR_inlines		    ((VALUE_type) 14)
#define DSCLASS_INSCR_inlines_sans	    ((VALUE_type) 15)
#define DSCLASS_INSCR_inlines_serif	    ((VALUE_type) 16)
#define DSCLASS_INSCR_outlines		    ((VALUE_type) 17)
#define DSCLASS_INSCR_outlines_sans	    ((VALUE_type) 18)
#define DSCLASS_INSCR_outlines_serif	    ((VALUE_type) 19)
#define DSCLASS_BLACK			    ((VALUE_type) 20)
#define DSCLASS_BLACK_formal		    ((VALUE_type) 21)
#define DSCLASS_BLACK_formal_sans	    ((VALUE_type) 22)
#define DSCLASS_BLACK_formal_serif	    ((VALUE_type) 23)
#define DSCLASS_BLACK_formal_sans_eng	    ((VALUE_type) 24)
#define DSCLASS_BLACK_formal_serif_eng	    ((VALUE_type) 25)
#define DSCLASS_BLACK_round		    ((VALUE_type) 26)
#define DSCLASS_BLACK_round_sans	    ((VALUE_type) 27)
#define DSCLASS_BLACK_round_serif	    ((VALUE_type) 28)
#define DSCLASS_BLACK_round_sans_eng	    ((VALUE_type) 29)
#define DSCLASS_BLACK_round_serif_eng	    ((VALUE_type) 30)
#define DSCLASS_BLACK_hybrid		    ((VALUE_type) 31)
#define DSCLASS_BLACK_hybrid_sans	    ((VALUE_type) 32)
#define DSCLASS_BLACK_hybrid_serif	    ((VALUE_type) 33)
#define DSCLASS_BLACK_hybrid_sans_eng	    ((VALUE_type) 34)
#define DSCLASS_BLACK_hybrid_serif_eng	    ((VALUE_type) 35)
#define DSCLASS_BLACK_inform		    ((VALUE_type) 36)
#define DSCLASS_BLACK_inform_sans	    ((VALUE_type) 37)
#define DSCLASS_BLACK_inform_serif	    ((VALUE_type) 38)
#define DSCLASS_BLACK_inform_sans_eng	    ((VALUE_type) 39)
#define DSCLASS_BLACK_inform_serif_eng	    ((VALUE_type) 40)
#define DSCLASS_SERIFS			    ((VALUE_type) 41)
#define DSCLASS_SERIFS_oldstyle		    ((VALUE_type) 42)
#define DSCLASS_SERIFS_oldstyle_vene	    ((VALUE_type) 43)
#define DSCLASS_SERIFS_oldstyle_garalde	    ((VALUE_type) 44)
#define DSCLASS_SERIFS_oldstyle_dutch	    ((VALUE_type) 45)
#define DSCLASS_SERIFS_trans		    ((VALUE_type) 46)
#define DSCLASS_SERIFS_trans_direct	    ((VALUE_type) 47)
#define DSCLASS_SERIFS_trans_modif	    ((VALUE_type) 48)
#define DSCLASS_SERIFS_modern		    ((VALUE_type) 49)
#define DSCLASS_SERIFS_modern_italian	    ((VALUE_type) 50)
#define DSCLASS_SERIFS_modern_fat	    ((VALUE_type) 51)
#define DSCLASS_SERIFS_cont		    ((VALUE_type) 52)
#define DSCLASS_SERIFS_cont_eclectic	    ((VALUE_type) 53)
#define DSCLASS_SERIFS_cont_fine	    ((VALUE_type) 54)
#define DSCLASS_SERIFS_cont_lettering	    ((VALUE_type) 55)
#define DSCLASS_SERIFS_legi		    ((VALUE_type) 56)
#define DSCLASS_SERIFS_legi_rounded	    ((VALUE_type) 57)
#define DSCLASS_SERIFS_legi_square	    ((VALUE_type) 58)
#define DSCLASS_SERIFS_square		    ((VALUE_type) 59)
#define DSCLASS_SERIFS_square_monotone	    ((VALUE_type) 60)
#define DSCLASS_SERIFS_square_clarendon	    ((VALUE_type) 61)
#define DSCLASS_SERIFS_square_french	    ((VALUE_type) 62)
#define DSCLASS_SERIFS_square_short	    ((VALUE_type) 63)
#define DSCLASS_SERIFS_square_type	    ((VALUE_type) 64)
#define DSCLASS_SERIFS_square_dot	    ((VALUE_type) 65)
#define DSCLASS_SERIFS_latin		    ((VALUE_type) 66)
#define DSCLASS_SERIFS_latin_solid	    ((VALUE_type) 67)
#define DSCLASS_SERIFS_latin_incised	    ((VALUE_type) 68)
#define DSCLASS_SERIFS_engraving	    ((VALUE_type) 69)
#define DSCLASS_SERIFS_engraving_barbed	    ((VALUE_type) 70)
#define DSCLASS_SERIFS_engraving_strait	    ((VALUE_type) 71)
#define DSCLASS_SERIFS_art		    ((VALUE_type) 72)
#define DSCLASS_SERIFS_art_french	    ((VALUE_type) 73)
#define DSCLASS_SERIFS_computer		    ((VALUE_type) 74)
#define DSCLASS_SERIFS_computer_ocr	    ((VALUE_type) 75)
#define DSCLASS_SERIFS_computer_dig	    ((VALUE_type) 76)
#define DSCLASS_SERIFS_misc		    ((VALUE_type) 77)
#define DSCLASS_SANS			    ((VALUE_type) 78)
#define DSCLASS_SANS_gothic		    ((VALUE_type) 79)
#define DSCLASS_SANS_gothic_grot	    ((VALUE_type) 80)
#define DSCLASS_SANS_gothic_neo		    ((VALUE_type) 81)
#define DSCLASS_SANS_gothic_type	    ((VALUE_type) 82)
#define DSCLASS_SCRIPTS			    ((VALUE_type) 83)
#define DSCLASS_ORNAM			    ((VALUE_type) 84)

#ifdef  TK_FONT

    /* Inside TK only */
#endif

#ifdef TK_IMPL_FONT

    /* Implementation module defn's */
#endif

#include <font1.h>

#endif
