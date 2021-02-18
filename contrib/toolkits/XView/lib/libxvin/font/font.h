/*      @(#)font.h 20.21 89/11/24 SMI      */

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef xview_font_DEFINED
#define xview_font_DEFINED

/*
 ***********************************************************************
 *			Include files
 ***********************************************************************
 */

#include <xview/generic.h>

/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

/*
 * Public #defines 
 */
#ifndef XV_ATTRIBUTES_ONLY

#define	FONT			&xv_font_pkg

/*
 * font family/style values available 
 */
#define FONT_FAMILY_DEFAULT		"FONT_FAMILY_DEFAULT"
#define FONT_FAMILY_DEFAULT_FIXEDWIDTH	"FONT_FAMILY_DEFAULT_FIXEDWIDTH"

#define FONT_FAMILY_LUCIDA		"FONT_FAMILY_LUCIDA"
#define FONT_FAMILY_LUCIDA_FIXEDWIDTH	"FONT_FAMILY_LUCIDA_FIXEDWIDTH"

#define FONT_FAMILY_ROMAN	"FONT_FAMILY_ROMAN"
#define FONT_FAMILY_SERIF	"FONT_FAMILY_SERIF"
#define FONT_FAMILY_COUR	"FONT_FAMILY_COUR"
#define FONT_FAMILY_CMR		"FONT_FAMILY_CMR"
#define FONT_FAMILY_GALLENT	"FONT_FAMILY_GALLENT"
#define FONT_FAMILY_OLGLYPH	"FONT_FAMILY_OLGLYPH"
#define FONT_FAMILY_OLCURSOR	"FONT_FAMILY_OLCURSOR"

#define FONT_STYLE_DEFAULT	"FONT_STYLE_DEFAULT"
#define FONT_STYLE_NORMAL	"FONT_STYLE_NORMAL"
#define FONT_STYLE_BOLD		"FONT_STYLE_BOLD"
#define FONT_STYLE_ITALIC	"FONT_STYLE_ITALIC"
#define FONT_STYLE_OBLIQUE	"FONT_STYLE_OBLIQUE"
#define FONT_STYLE_BOLD_ITALIC	"FONT_STYLE_BOLD_ITALIC"
#define FONT_STYLE_BOLD_OBLIQUE	"FONT_STYLE_BOLD_OBLIQUE"

#define FONT_SIZE_DEFAULT	-99
#define FONT_NO_SIZE		-66
#define FONT_NO_SCALE		-55
#define FONT_SCALE_DEFAULT	-33

#endif	~XV_ATTRIBUTES_ONLY

/*
 * OPEN LOOK Glyph font character definitions
 */
/* Note: Character 0 is used as a flag to indicate "no character" */
#define OLG_VSB_ELEVATOR	1
#define OLG_VSB_ELEVATOR_LINE_BACKWARD	2
#define OLG_VSB_ELEVATOR_ABSOLUTE		3
#define OLG_VSB_ELEVATOR_LINE_FORWARD	4
#define OLG_VSB_REDUCED_ELEVATOR	5
#define OLG_VSB_REDUCED_ELEVATOR_LINE_BACKWARD	6
#define OLG_VSB_REDUCED_ELEVATOR_LINE_FORWARD	7
#define OLG_VSB_ANCHOR	8
#define OLG_VSB_ANCHOR_INVERTED	9
#define OLG_HSB_ELEVATOR	10
#define OLG_HSB_ELEVATOR_LINE_BACKWARD	11
#define OLG_HSB_ELEVATOR_ABSOLUTE		12
#define OLG_HSB_ELEVATOR_LINE_FORWARD	13
#define OLG_HSB_REDUCED_ELEVATOR	14
#define OLG_HSB_REDUCED_ELEVATOR_LINE_BACKWARD	15
#define OLG_HSB_REDUCED_ELEVATOR_LINE_FORWARD	16
#define OLG_HSB_ANCHOR	17
#define OLG_HSB_ANCHOR_INVERTED	18
#define OLG_MENU_PIN_OUT	19
#define OLG_MENU_PIN_IN		20
#define OLG_MENU_DEFAULT_PIN_OUT	21
#define OLG_ABBREV_MENU_BUTTON	22
#define OLG_ABBREV_MENU_BUTTON_INVERTED	23

/*
 * Private #defines 
 */
#define	FONT_ATTR(type, ordinal)	ATTR(ATTR_PKG_FONT, type, ordinal)
#define FONT_QUAD_ATTR			ATTR_TYPE(ATTR_BASE_INT, 4)

/*
 ***********************************************************************
 *		Typedefs, enumerations, and structs
 ***********************************************************************
 */

/*
 * Public types 
 */
#ifndef XV_ATTRIBUTES_ONLY

typedef	Xv_opaque 	Xv_Font;
typedef	Xv_opaque 	Xv_font;

#endif 	~XV_ATTRIBUTES_ONLY

typedef enum {
    /*
     * Public attributes. 
     */
    FONT_CHAR_WIDTH	= FONT_ATTR(ATTR_CHAR,		 1),	/* G 	*/
    FONT_CHAR_HEIGHT	= FONT_ATTR(ATTR_CHAR,		 5),	/* G 	*/
    FONT_DEFAULT_CHAR_HEIGHT
    			= FONT_ATTR(ATTR_NO_VALUE,	10),	/* G 	*/
    FONT_DEFAULT_CHAR_WIDTH
    			= FONT_ATTR(ATTR_NO_VALUE,	15),	/* G 	*/
    FONT_FAMILY		= FONT_ATTR(ATTR_STRING,	20),	/* C-G 	*/
    FONT_NAME		= FONT_ATTR(ATTR_STRING,	25),	/* C-G 	*/
    FONT_RESCALE_OF	= FONT_ATTR(ATTR_OPAQUE_PAIR,	30),	/* F-C 	*/
    FONT_SCALE		= FONT_ATTR(ATTR_INT,		40),	/* C-G 	*/
    FONT_SIZE		= FONT_ATTR(ATTR_INT,		45),	/* C-G 	*/
    FONT_SIZES_FOR_SCALE= ATTR(ATTR_PKG_FONT,
    					FONT_QUAD_ATTR, 50),	/* C-S-G*/
    FONT_STRING_DIMS	= FONT_ATTR(ATTR_OPAQUE_PAIR,	55),	/* G 	*/
    FONT_STYLE		= FONT_ATTR(ATTR_STRING,	60),	/* C-G 	*/
	FONT_TYPE		= FONT_ATTR(ATTR_ENUM,		65),	/* C-S-G */
    /*
     * Private attributes. 
     */
    FONT_HEAD		= FONT_ATTR(ATTR_INT,		70),	/* Key 	*/
    FONT_UNKNOWN_HEAD	= FONT_ATTR(ATTR_INT,		75),	/* Key 	*/
    FONT_INFO		= FONT_ATTR(ATTR_OPAQUE,	80),	/* G    */

} Font_attribute;

typedef enum {
	FONT_TYPE_TEXT = 0,
	FONT_TYPE_CURSOR = 1,
	FONT_TYPE_GLYPH = 2
} Font_type;

#ifndef XV_ATTRIBUTES_ONLY

typedef struct {
    Xv_generic_struct 	parent_data;
    Xv_opaque	 	private_data;
    Xv_embedding	embedding_data;
    char		*pixfont[2+(5*256)];
} Xv_font_struct;

typedef struct {
    int     		width, height;
} Font_string_dims;

#endif ~XV_ATTRIBUTES_ONLY

/*
 ***********************************************************************
 *			Globals
 ***********************************************************************
 */

#ifndef XV_ATTRIBUTES_ONLY
extern Xv_pkg		xv_font_pkg;
#endif ~XV_ATTRIBUTES_ONLY

#endif ~xview_font_DEFINED
