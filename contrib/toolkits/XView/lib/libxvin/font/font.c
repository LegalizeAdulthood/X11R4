#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)font.c 20.52 89/11/24";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/xv_debug.h>
#include <sys/types.h>
#include <X11/Xlib.h>
#include <pixrect/pixrect.h>
#include <pixrect/pixfont.h>
#include <xview/generic.h>
#include <xview/notify.h>
#include <xview/server.h>
#include <xview/window.h>
#include <xview_private/font_impl.h>

/*
 * Public
 */

extern Pixfont *xv_pf_sys;
extern char    *defaults_get_string();
extern char    *strcpy();
extern Xv_opaque xv_default_server;
extern Xv_opaque xv_default_display;

extern char *index();

/*
 * Private
 */

/*
 * Definitions for decrypting xlfd names
 */
#define		DASH		'-'
#define		NUMXLFDFIELDS	14
#define		FOUNDRYPOS	1
#define		FAMILYPOS	2
#define		WEIGHTPOS	3
#define		SLANTPOS	4
#define		SETWIDTHNAMEPOS	5
#define		ADDSTYLENAMEPOS	6
#define		PIXSIZEPOS	7
#define		PTSIZEPOS	8

#define DEFAULT_SMALL_FONT		"-b&h-lucida-medium-r-normal-sans-10-*-*-*-*-*-*-*"
#define DEFAULT_SMALL_FONT_SIZE		10
#define DEFAULT_MEDIUM_FONT		"-b&h-lucida-medium-r-normal-sans-12-*-*-*-*-*-*-*"
#define DEFAULT_MEDIUM_FONT_SIZE	12
#define DEFAULT_LARGE_FONT		"-b&h-lucida-medium-r-normal-sans-14-*-*-*-*-*-*-*"
#define DEFAULT_LARGE_FONT_SIZE		14
#define DEFAULT_XLARGE_FONT		"-b&h-lucida-medium-r-normal-sans-19-*-*-*-*-*-*-*"
#define DEFAULT_XLARGE_FONT_SIZE	19

#ifndef _XVIEW_DEFAULT_FONT
#define DEFAULT_DEFAULT_FONT_NAME	"fixed"
#else
#define DEFAULT_DEFAULT_FONT_NAME	"xview-default-font"
#endif				/* _XVIEW_DEFAULT_FONT */

#define DEFAULT_FONT_NAME		"-b&h-lucida-medium-r-normal-sans-12-*-*-*-*-*-*-*"
							/* default:FAMILY-STYLE-S
							 * IZE */

#define DEFAULT_FONT_FAMILY		"lucida"
#define DEFAULT_FONT_FIXEDWIDTH_FAMILY	"lucidatypewriter"
#define DEFAULT_FONT_STYLE		""

/*
 * Default font weight, slant
 */
#define DEFAULT_FONT_WEIGHT		"medium"
#define DEFAULT_FONT_SLANT		"r"
#define DEFAULT_FONT_SETWIDTHNAME	"normal"
#define DEFAULT_FONT_ADDSTYLENAME	"sans"
#define DEFAULT_FONT_SCALE		(int) WIN_SCALE_MEDIUM

static int      font_default_scale = DEFAULT_FONT_SCALE;
static int      font_default_size = 12;
static char    *font_default_family = DEFAULT_FONT_FAMILY;
static char    *font_default_fixedwidth_family = DEFAULT_FONT_FIXEDWIDTH_FAMILY;
static char    *font_default_style = DEFAULT_FONT_STYLE;

static char    *font_default_weight = DEFAULT_FONT_WEIGHT;
static char    *font_default_slant = DEFAULT_FONT_SLANT;
static char    *font_default_setwidthname = DEFAULT_FONT_SETWIDTHNAME;
static char    *font_default_addstylename = DEFAULT_FONT_ADDSTYLENAME;

static char    *sunview1_prefix = "/usr/lib/fonts/fixedwidthfonts/";

Xv_private Xv_Font xv_font_with_name();
static void     font_default_font();
static char    *font_default_font_from_scale();
static char    *font_determine_font_name();
static char    *font_rescale_from_font();
static int      font_names_ok();
static void     font_decrypt_name();
static void     font_decrypt_family_style();
static void     font_decrypt_rescale_info();
static int      font_decrypt_sv1_name();
static int      font_decrypt_xv_name();
static int      font_read_attrs();
static int      font_string_compare();
static int      font_string_compare_nchars();

/* font naming compatibility */

typedef struct font_translation {
    char           *sv1_name;
    char           *sv1_foundry;
    char           *sv1_family;
    char           *sv1_style;
    char           *sv1_weight;
    char           *sv1_slant;
    char           *sv1_setwidthname;
    char           *sv1_addstylename;

    int             size;
    int             scale;	/* medium resolution */
};

typedef struct xv_to_x_font_translation {
    char           *xv_family;
    char           *xv_style;
    char           *x_font_foundry;
    char           *x_font_family;
    char           *x_font_style;
    char           *x_font_weight;
    char           *x_font_slant;
    char           *x_font_setwidthname;
    char           *x_font_addstylename;
};

typedef struct font_definitions {
    char           *xv_family;
    char           *xv_style;
    char           *foundry;
    char           *family;
    char           *weight;
    char           *slant;
    char           *setwidthname;
    char           *addstylename;

    int             small_size;	/* medium resolution */
    int             medium_size;
    int             large_size;
    int             extra_large_size;
};

typedef struct font_return_attrs {
    char           *name;
    char           *family;
    char           *style;
    char           *foundry;
    char           *weight;
    char           *slant;
    char           *setwidthname;
    char           *addstylename;
    int             size;
    int             small_size;
    int             medium_size;
    int             large_size;
    int             extra_large_size;
    int             scale;
    Font_info      *resize_from_font;
    int             rescale_factor;
    int             free_name, free_family, free_style, 
		    free_weight, free_slant, free_foundry, 
		    free_setwidthname, free_addstylename;
};
typedef struct font_return_attrs *Font_return_attrs;

typedef struct style_definitions  {
    char	*style;
    char	*weight;
    char	*slant;
}Style_defs;

struct _unknown {
    struct _unknown *next;
    char           *name;
};
typedef struct _unknown *Unknown;

/*
 * Style defs - conversion table from style -> (weight, slant)
 * The 'real' style entry must precede the semantic entry.
 * i.e. the entry for 'bold' precedes 'FONT_STYLE_BOLD'
 * This is necessary so that when a (weight, slant) -> style
 * conversion is being done, the 'real' entry is matched first.
 */
static struct style_definitions	known_styles[] = {
	/*
	 * 'real' entries
	 */
	"normal", "medium", "r",
	"bold", "bold", "r",
	"italic", "medium", "i",
	"oblique", "medium", "o",
	"bolditalic", "bold", "i",
	"boldoblique", "bold", "o",

	/*
	 * semantic entries
	 */
	"FONT_STYLE_NORMAL", "medium", "r",
	"FONT_STYLE_BOLD", "bold", "r",
	"FONT_STYLE_ITALIC", "medium", "i",
	"FONT_STYLE_OBLIQUE", "medium", "o",
	"FONT_STYLE_BOLD_ITALIC", "bold", "i",
	"FONT_STYLE_BOLD_OBLIQUE", "bold", "o",
	0, 0, 0
    };

/*
 * Isa
 * Added foundry, weight, slant fields
 */
static struct font_translation known_sv1fonts[] = {
    "cmr.b.8", "adobe", "times", "bold", "bold", "r", "normal", "", 8, (int) WIN_SCALE_SMALL,
    "cmr.b.14", "adobe", "times", "bold", "bold", "r", "normal", "", 14, (int) WIN_SCALE_LARGE,
    "cmr.r.8", "adobe", "times", 0, "medium", "r", "normal", "", 8, (int) WIN_SCALE_SMALL,
    "cmr.r.14", "adobe", "times", 0, "medium", "r", "normal", "", 14, (int) WIN_SCALE_LARGE,
    "cour.b.10", "adobe", "courier", "bold", "bold", "r", "normal", "", 10, (int) WIN_SCALE_SMALL,
    "cour.b.12", "adobe", "courier", "bold", "bold", "r", "normal", "", 12, (int) WIN_SCALE_SMALL,
    "cour.b.14", "adobe", "courier", "bold", "bold", "r", "normal", "", 14, (int) WIN_SCALE_MEDIUM,
    "cour.b.16", "adobe", "courier", "bold", "bold", "r", "normal", "", 16, (int) WIN_SCALE_MEDIUM,
    "cour.b.18", "adobe", "courier", "bold", "bold", "r", "normal", "", 18, (int) WIN_SCALE_LARGE,
    "cour.b.24", "adobe", "courier", "bold", "bold", "r", "normal", "", 24, (int) WIN_SCALE_EXTRALARGE,
    "cour.r.10", "adobe", "courier", 0, "medium", "r", "normal", "", 10, (int) WIN_SCALE_SMALL,
    "cour.r.12", "adobe", "courier", 0, "medium", "r", "normal", "", 12, (int) WIN_SCALE_SMALL,
    "cour.r.14", "adobe", "courier", 0, "medium", "r", "normal", "", 14, (int) WIN_SCALE_MEDIUM,
    "cour.r.16", "adobe", "courier", 0, "medium", "r", "normal", "", 16, (int) WIN_SCALE_MEDIUM,
    "cour.r.18", "adobe", "courier", 0, "medium", "r", "normal", "", 18, (int) WIN_SCALE_LARGE,
    "cour.r.24", "adobe", "courier", 0, "medium", "r", "normal", "", 24, (int) WIN_SCALE_EXTRALARGE,
    "gallant.r.10", "adobe", "times", 0, "medium", "r", "normal", "", 10, (int) WIN_SCALE_SMALL,
    "gallant.r.19", "adobe", "times", 0, "medium", "r", "normal", "", 19, (int) WIN_SCALE_LARGE,
    "serif.r.10", "adobe", "times", 0, "medium", "r", "normal", "", 10, (int) WIN_SCALE_SMALL,
    "serif.r.11", "adobe", "times", 0, "medium", "r", "normal", "", 11, (int) WIN_SCALE_SMALL,
    "serif.r.12", "adobe", "times", 0, "medium", "r", "normal", "", 12, (int) WIN_SCALE_MEDIUM,
    "serif.r.14", "adobe", "times", 0, "medium", "r", "normal", "", 14, (int) WIN_SCALE_LARGE,
    "serif.r.16", "adobe", "times", 0, "medium", "r", "normal", "", 16, (int) WIN_SCALE_EXTRALARGE,
    0, 0, 0, 0, 0, 0, 0, 0, 0, (int) WIN_SCALE_MEDIUM
};

/*
 * Isa
 * Added weight, slant fields
 */
static struct xv_to_x_font_translation known_openwin_fonts[] = {
    FONT_FAMILY_OLGLYPH, "", "sun", "open look glyph", "", "", "", "", "",
    FONT_FAMILY_OLGLYPH, FONT_STYLE_NORMAL, "sun", "open look glyph", "", "", "", "", "",

    FONT_FAMILY_OLCURSOR, "", "sun", "open look cursor", "", "", "", "", "",
    FONT_FAMILY_OLCURSOR, FONT_STYLE_NORMAL, "sun", "open look cursor", "", "", "", "", "",

    FONT_FAMILY_LUCIDA, "", "b&h", "lucida", "", "medium", "r", "normal", "sans",
    FONT_FAMILY_LUCIDA, FONT_STYLE_NORMAL, "b&h", "lucida", "", "medium", "r", "normal", "sans",
    FONT_FAMILY_LUCIDA, FONT_STYLE_BOLD, "b&h", "lucida", "bold", "bold", "r", "normal", "sans",
    FONT_FAMILY_LUCIDA, FONT_STYLE_ITALIC, "b&h", "lucida", "italic", "medium", "i", "normal", "sans",
    FONT_FAMILY_LUCIDA, FONT_STYLE_BOLD_ITALIC, "b&h", "lucida", "bolditalic", "bold", "i", "normal", "sans",

    FONT_FAMILY_LUCIDA_FIXEDWIDTH, "", "b&h", "lucidatypewriter", "", "medium", "r", "normal", "sans",
    FONT_FAMILY_LUCIDA_FIXEDWIDTH, FONT_STYLE_NORMAL, "b&h", "lucidatypewriter", "", "medium", "r", "normal", "sans",
    FONT_FAMILY_LUCIDA_FIXEDWIDTH, FONT_STYLE_BOLD, "b&h", "lucidatypewriter", "bold", "bold", "r", "normal", "sans",

    /*
     * Isa
     * Added for each entry in table that had FONT_STYLE_NORMAL, an equivalent entry with
     * style = "". This is so that the matches of FONT_FAMILY = "whatever", FONT_STYLE =
     * "" are sucessful.
     */
    FONT_FAMILY_ROMAN, "", "adobe", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_ROMAN, FONT_STYLE_NORMAL, "adobe", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_ROMAN, FONT_STYLE_BOLD, "adobe", "times", "bold", "bold", "r", "normal", "",
    FONT_FAMILY_ROMAN, FONT_STYLE_ITALIC, "adobe", "times", "italic", "medium", "i", "normal", "",
    FONT_FAMILY_ROMAN, FONT_STYLE_BOLD_ITALIC, "adobe", "times", "bolditalic", "bold", "i", "normal", "",

    FONT_FAMILY_SERIF, "", "adobe", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_SERIF, FONT_STYLE_NORMAL, "adobe", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_SERIF, FONT_STYLE_BOLD, "adobe", "times", "bold", "bold", "r", "normal", "",
    FONT_FAMILY_SERIF, FONT_STYLE_ITALIC, "adobe", "times", "italic", "medium", "i", "normal", "",
    FONT_FAMILY_SERIF, FONT_STYLE_BOLD_ITALIC, "adobe", "times", "bolditalic", "bold", "i", "normal", "",

    FONT_FAMILY_CMR, "", "adobe", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_CMR, FONT_STYLE_NORMAL, "adobe", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_CMR, FONT_STYLE_BOLD, "adobe", "times", "bold", "bold", "r", "normal", "",
    FONT_FAMILY_CMR, FONT_STYLE_ITALIC, "adobe", "times", "italic", "medium", "i", "normal", "",
    FONT_FAMILY_CMR, FONT_STYLE_BOLD_ITALIC, "adobe", "times", "bolditalic", "bold", "i", "normal", "",

    FONT_FAMILY_GALLENT, "", "adobe", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_GALLENT, FONT_STYLE_NORMAL, "adobe", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_GALLENT, FONT_STYLE_BOLD, "adobe", "times", "bold", "bold", "r", "normal", "",
    FONT_FAMILY_GALLENT, FONT_STYLE_ITALIC, "adobe", "times", "italic", "medium", "i", "normal", "",
    FONT_FAMILY_GALLENT, FONT_STYLE_BOLD_ITALIC, "adobe", "times", "bolditalic", "bold", "i", "normal", "",

    FONT_FAMILY_COUR, "", "adobe", "courier", "", "medium", "r", "normal", "",
    FONT_FAMILY_COUR, FONT_STYLE_NORMAL, "adobe", "courier", "", "medium", "r", "normal", "",
    FONT_FAMILY_COUR, FONT_STYLE_BOLD, "adobe", "courier", "bold", "bold", "r", "normal", "",
    FONT_FAMILY_COUR, FONT_STYLE_ITALIC, "adobe", "courier", "italic", "medium", "i", "normal", "",
    FONT_FAMILY_COUR, FONT_STYLE_OBLIQUE, "adobe", "courier", "oblique", "medium", "o", "normal", "",
    FONT_FAMILY_COUR, FONT_STYLE_BOLD_ITALIC, "adobe", "courier", "bolditalic", "bold", "i", "normal", "",
    FONT_FAMILY_COUR, FONT_STYLE_BOLD_OBLIQUE, "adobe", "courier", "boldoblique", "bold", "o", "normal", "",

    0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * Isa
 * Added weight, slant, setwidthname, addstylename fields
 * changed lucidasans to lucida
 * added ol* fonts
    "olglyph", 0, "sun", "open look glyph", 0, 0, 0, 0, 10, 12, 14, 19,
    "olcursor", 0, "sun", "open look cursor", 0, 0, 0, 0, 10, 12, 14, 19,
 */
static struct font_definitions known_xvfonts[] = {
    "lucida", 0, "b&h", "lucida", "medium", "r", "normal", "sans", 10, 12, 14, 19,
    "lucida", "bold", "b&h", "lucida", "bold", "r", "normal", "sans", 10, 12, 14, 19,
    "lucida", "bolditalic", "b&h", "lucida", "bold", "i", "normal", "sans", 10, 12, 14, 19,
    "lucida", "italic", "b&h", "lucida", "medium", "i", "normal", "sans", 10, 12, 14, 19,
    "lucidatypewriter", "", "b&h", "lucidatypewriter", "medium", "r", "normal", "sans", 10, 12, 14, 19,
    "lucidatypewriter", "bold", "b&h", "lucidatypewriter", "bold", "r", "normal", "sans", 10, 12, 14, 19,
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

static char    *
normalize_font_name(name)
    register char  *name;
{
    if (name == NULL) {
	char           *default_scale;
	/*
	 * NULL for name => use default font name. Warning: Database may have
	 * "" rather than NULL.
	 */
	char           *use_defaults_font = (char *) getenv("XVIEW_FONT_DEBUG");
	if (!use_defaults_font) {
	    default_scale = (char *) defaults_get_string("window.scale",
						  "Window.Scale", "Medium");
	    name = font_default_font_from_scale(default_scale);
	} else {
	    name = defaults_get_string("font.name", "Font.Name", NULL);
	    default_scale = (char *) defaults_get_string("window.scale",
						  "Window.Scale", "Medium");
	    if (name == NULL || (strlen(name) == 0)) {
		name = font_default_font_from_scale(default_scale);
	    }
	}
    }
    if (font_string_compare_nchars(name, sunview1_prefix,
				   strlen(sunview1_prefix)) == 0) {
	/* Map SunView1.X font name into server name. */
	name += strlen(sunview1_prefix);	/* index into str */
    }
    return (name);
}

static char    *
font_default_font_from_scale(scale)
    register char  *scale;
{
    if (!scale)
	return DEFAULT_MEDIUM_FONT;

    if ((font_string_compare(scale, "small") == 0) ||
	(font_string_compare(scale, "Small") == 0)) {
	return DEFAULT_SMALL_FONT;
    } else if ((font_string_compare(scale, "medium") == 0) ||
	       (font_string_compare(scale, "Medium") == 0)) {
	return DEFAULT_MEDIUM_FONT;
    } else if ((font_string_compare(scale, "large") == 0) ||
	       (font_string_compare(scale, "Large") == 0)) {
	return DEFAULT_LARGE_FONT;
    } else if ((font_string_compare(scale, "Extra_large") == 0) ||
	       (font_string_compare(scale, "Extra_Large") == 0) ||
	       (font_string_compare(scale, "extra_Large") == 0) ||
	       (font_string_compare(scale, "extra_large") == 0)) {
	return DEFAULT_XLARGE_FONT;
    } else
	return DEFAULT_MEDIUM_FONT;
}

/* Called to free the font list when server is being destroyed. */
static void
font_list_free(server, key, data)
    Xv_object       server;
    Font_attribute  key;
    Xv_opaque       data;
{
    register Font_info *font, *next;
    register int    ref_count;

    ASSERT(key == FONT_HEAD, _svdebug_always_on);
    for (font = (Font_info *) data; font; font = next) {
	next = font->next;	/* Paranoia in case xv_destroy is immediate */
	if ((ref_count = (int) xv_get(FONT_PUBLIC(font), XV_REF_COUNT)) == 0) {
	    xv_destroy(FONT_PUBLIC(font));
#ifdef _XV_DEBUG
	} else {
	    fprintf(stderr,
		    "Font %s has %d refs but server being destroyed.\n",
		    font->name, ref_count);
#endif
	}
    }
}

/* Called to free the unknown list when server is being destroyed. */
static void
font_unknown_list_free(server, key, data)
    Xv_object       server;
    Font_attribute  key;
    Xv_opaque       data;
{
    register Unknown unknown, next;

    ASSERT(key == FONT_UNKNOWN_HEAD, _svdebug_always_on);
    for (unknown = (Unknown) data; unknown; unknown = next) {
	next = unknown->next;
	free(unknown->name);
	free((char *) unknown);
    }
}

/*
 * Initialize font. This routine creates a new font (because xv_create always
 * allocates a new instance) and can not be used to get a handle to an
 * existing font.
 */
int
font_init(parent_public, font_public, avlist, offset_ptr)
    Xv_opaque       parent_public;
    Xv_font_struct *font_public;
    Attr_avlist     avlist;
    int            *offset_ptr;
{
    register Font_info *font;
    register struct pixchar *pfc;
    int             font_attrs_exist;
    Xv_opaque       server;
    Xv_opaque       display, x_font_info;
    long unsigned   xid;
    int             i, default_x, default_y, max_char, min_char;
    int             neg_left_bearing;
#ifdef CHECK_VARIABLE_HEIGHT
    int             variable_height_font, last_char_height;
#endif				/* CHECK_VARIABLE_HEIGHT */
    Font_info      *font_head;
    int             need_to_free_font_name;	/* font_name if malloc'd */
    char           *font_name = NULL;
    Pixfont        *zap_font_public =
    (Pixfont *) (font_public->pixfont);
    struct font_return_attrs my_attrs;
    /*
     * initialization
     */
    my_attrs.free_name = my_attrs.free_foundry = my_attrs.free_family = my_attrs.free_style 
		= my_attrs.free_weight = my_attrs.free_slant 
		= my_attrs.free_setwidthname 
		= my_attrs.free_addstylename = (int) 0;

    my_attrs.name = my_attrs.foundry = my_attrs.family = my_attrs.style 
			= my_attrs.weight = my_attrs.slant 
			= my_attrs.setwidthname = my_attrs.addstylename = (char *)0;
    my_attrs.size = (int) FONT_NO_SIZE;
    my_attrs.small_size = my_attrs.medium_size = my_attrs.large_size =
	my_attrs.extra_large_size = (int) FONT_NO_SIZE;
    my_attrs.scale = FONT_NO_SCALE;
    my_attrs.rescale_factor = (int) 0;
    my_attrs.resize_from_font = (Font_info *) 0;

    /*
     * Get the optional creation arguments
     */
    font_attrs_exist = font_read_attrs(&my_attrs, TRUE, avlist);
    if (!font_attrs_exist)
	(void) font_default_font(&my_attrs);

    if (!parent_public) {
	/* xv_create ensures that xv_default_server is valid. */
	parent_public = (Xv_opaque) xv_default_server;
	display = xv_get(parent_public, XV_DISPLAY);
	server = (Xv_opaque) xv_default_server;
    } else {
	Xv_pkg         *pkgtype = (Xv_pkg *) xv_get(parent_public, XV_TYPE);
	display = xv_get(parent_public, XV_DISPLAY);
	if (!display) {
	    if ((Xv_pkg *) pkgtype == (Xv_pkg *) & xv_font_pkg) {
		Xv_Font         real_parent_public = 0;
		Font_info      *real_parent_private = 0;

		XV_OBJECT_TO_STANDARD(parent_public, "font_init", real_parent_public);
		real_parent_private = FONT_PRIVATE(real_parent_public);
		parent_public = real_parent_private->parent;
		display = real_parent_private->display;
	    } else
		display = xv_default_display;
	}
	if ((Xv_pkg *) pkgtype == (Xv_pkg *) & xv_server_pkg) {
	    server = (Xv_opaque) parent_public;
	} else {
	    server = (Xv_opaque) XV_SERVER_FROM_WINDOW(parent_public);
	}
    }

    if (my_attrs.name) {
	char *name = my_attrs.name = normalize_font_name(my_attrs.name);
	if (font_decrypt_sv1_name(&my_attrs) == 0) {
	    font_name = font_determine_font_name(&my_attrs);
	    if ((font_name == NULL) || (strlen(font_name) == 0))
		my_attrs.name = name; /* reset */
	} else  {
	    /* not sv1 name */
	    /*
	     * If successfully decrypted xv name, try to construct
	     * xlfd name.
	     */
	    if (font_decrypt_xv_name(&my_attrs) == 0)  {
	        font_name = font_determine_font_name(&my_attrs);
	        if ((font_name == NULL) || (strlen(font_name) == 0))
		    my_attrs.name = name; /* reset */
	    }
	    else  {
		/*
		 * If not sv1 name, not xv name, check if xlfd name
		 * Do not have to call font_determine_font_name here 
		 * because the name is already in xlfd format.
		 */
		(void)font_decrypt_xlfd_name(&my_attrs);
	    }
	}
    } else if (my_attrs.resize_from_font) {
	font_name = font_rescale_from_font(
					(Xv_Font) my_attrs.resize_from_font,
					   (int) my_attrs.rescale_factor,
					   &my_attrs);
	if ((font_name == NULL) || (strlen(font_name) == 0))
	    return XV_ERROR;
    } else {
	font_name = font_determine_font_name(&my_attrs);
	if ((font_name == NULL) || (strlen(font_name) == 0)) {
	    char            dummy[128];

	    (void) sprintf(dummy, "Cannot load font '%s'",
			   (my_attrs.name) ? my_attrs.name : font_name);
	    xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		     XV_ERROR_STRING, dummy, XV_ERROR_PKG, ATTR_PKG_FONT, 0);
	    return XV_ERROR;
	} else
	    need_to_free_font_name = 1;	/* free name later */
    }

    xid = xv_load_x_font(display,
			 (my_attrs.name) ? my_attrs.name : font_name,
			 TRUE,
			 &x_font_info,
			 &default_x, &default_y,
			 &max_char, &min_char);
    if (xid == 0) {
	char            dummy[128];

	(void) sprintf(dummy, "Cannot load font '%s'",
		       (my_attrs.name) ? my_attrs.name : font_name);
	xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING, dummy, XV_ERROR_PKG, ATTR_PKG_FONT, 0);
	if (need_to_free_font_name && (font_name != (char *) 0))
	    free(font_name);
	return XV_ERROR;
    }
    zap_font_public->pf_defaultsize.x = default_x;
    zap_font_public->pf_defaultsize.y = default_y;
    neg_left_bearing = FALSE;
#ifdef CHECK_VARIABLE_HEIGHT
    last_char_height = 0;
    variable_height_font = FALSE;
#endif				/* CHECK_VARIABLE_HEIGHT */
    for (i = min_char, pfc = &(zap_font_public->pf_char[i]);
	 i <= MIN(255, max_char);	/* "i" cannot ever be >255 - pixfont
					 * compat */
	 i++, pfc++) {
	xv_x_char_info(x_font_info, i - min_char,
		       &pfc->pc_home.x, &pfc->pc_home.y,
		       &pfc->pc_adv.x, &pfc->pc_adv.y,
		       &pfc->pc_pr);
#ifdef CHECK_VARIABLE_HEIGHT
	if (pfc->pc_pr->pr_height != last_char_height) {
	    if (last_char_height)
		variable_height_font = TRUE;
	    else
		last_char_height = pfc->pc_pr->pr_height;
	}
#endif				/* CHECK_VARIABLE_HEIGHT */
	if (pfc->pc_home.x < 0)
	    neg_left_bearing = TRUE;
    }

    font = (Font_info *) xv_alloc(Font_info);

    /*
     * set forward and back pointers
     */
    font_public->private_data = (Xv_opaque) font;
    font->public_self = (Xv_opaque) zap_font_public;
    font->parent = parent_public;
    *offset_ptr = xv_set_embedding_data(zap_font_public, font_public);

    font->display = display;
    font->server = server;
    font->pkg = ATTR_PKG_FONT;
    font->xid = xid;
    font->x_font_info = x_font_info;
    (void) xv_set(font_public, XV_RESET_REF_COUNT, 0);
    font->type = FONT_TYPE_TEXT;

#ifdef CHECK_OVERLAPPING_CHARS
    if (neg_left_bearing
#ifdef CHECK_VARIABLE_HEIGHT
	|| variable_height_font
#endif				/* CHECK_VARIABLE_HEIGHT */
	)
	font->overlapping_chars = TRUE;
#endif				/* CHECK_OVERLAPPING_CHARS */

    if (my_attrs.foundry) {
	if (my_attrs.free_foundry) {
	    font->foundry = my_attrs.foundry;	/* take malloc'd ptr */
	} else
	    font->foundry = xv_strsave(my_attrs.foundry);
    }
    if (my_attrs.family) {
	if (my_attrs.free_family) {
	    font->family = my_attrs.family;	/* take malloc'd ptr */
	} else
	    font->family = xv_strsave(my_attrs.family);
    }
    if (my_attrs.style) {
	if (my_attrs.free_style) {
	    font->family = my_attrs.style;	/* take malloc'd ptr */
	} else
	    font->style = xv_strsave(my_attrs.style);
    }
    if (my_attrs.weight) {
	if (my_attrs.free_weight) {
	    font->weight = my_attrs.weight;	/* take malloc'd ptr */
	} else
	    font->weight = xv_strsave(my_attrs.weight);
    }
    if (my_attrs.slant) {
	if (my_attrs.free_slant) {
	    font->slant = my_attrs.slant;	/* take malloc'd ptr */
	} else
	    font->slant = xv_strsave(my_attrs.slant);
    }
    if (my_attrs.setwidthname) {
	if (my_attrs.free_setwidthname) {
	    font->setwidthname = my_attrs.setwidthname;	/* take malloc'd ptr */
	} else
	    font->setwidthname = xv_strsave(my_attrs.setwidthname);
    }
    if (my_attrs.addstylename) {
	if (my_attrs.free_addstylename) {
	    font->addstylename = my_attrs.addstylename;	/* take malloc'd ptr */
	} else
	    font->addstylename = xv_strsave(my_attrs.addstylename);
    }
    if (my_attrs.name) {
	if (my_attrs.free_name) {
	    font->name = my_attrs.name;
	} else
	    font->name = xv_strsave(my_attrs.name);
    } else if (font_name) {
	font->name = xv_strsave(font_name);
    }
    font->size = (int) my_attrs.size;
    font->scale = (int) my_attrs.scale;
    font->small_size = (int) my_attrs.small_size;
    font->medium_size = (int) my_attrs.medium_size;
    font->large_size = (int) my_attrs.large_size;
    font->extra_large_size = (int) my_attrs.extra_large_size;

    /*
     * Add new font to server's list
     */
    if (font_head = (Font_info *) xv_get(server, XV_KEY_DATA, FONT_HEAD)) {
	font->next = font_head->next;
	font_head->next = font;
    } else {
	font->next = (Font_info *) 0;
	(void) xv_set(server,
		      XV_KEY_DATA, FONT_HEAD, font,
		      XV_KEY_DATA_REMOVE_PROC, FONT_HEAD, font_list_free,
		      0);
    }

    /*
     * SunView1.X compatibility: set this font as default if appropriate.
     */
    if ((xv_pf_sys == (Pixfont *) 0) &&
	(parent_public == (Xv_opaque) xv_default_server)) {
	if ((font_string_compare(my_attrs.name,
				 normalize_font_name((char *)NULL)) == 0)
	/*
	 * || (font_string_compare(my_attrs.name, DEFAULT_DEFAULT_FONT_NAME)
	 * == 0)
	     */ ) {
	    xv_pf_sys = zap_font_public;
	    (void) xv_set(font_public, XV_INCREMENT_REF_COUNT, 0);
	}
    }
    return XV_OK;
}

int
font_destroy_struct(font_public, status)
    Xv_font_struct *font_public;
    Destroy_status  status;
{
    register Font_info *font = FONT_PRIVATE(font_public);
    register Font_info *prev;
    register int    i;
    register struct pixchar
                   *pfc;
    Font_info      *font_head;
    Xv_opaque       display, server = font->server;
    Pixfont        *zap_font_public = (Pixfont *) (font_public->pixfont);

    if (status == DESTROY_CLEANUP) {
	/* PERFORMANCE ALERT: revive list package to consolidate code. */
	/* Remove the font from SunView's server list. */
	font_head = (Font_info *) xv_get(server, XV_KEY_DATA, FONT_HEAD);
	if (!font_head) {
	    server = (Xv_opaque) xv_default_server;
	    font_head = (Font_info *) xv_get(server, XV_KEY_DATA, FONT_HEAD);
	}
	if (((Xv_Font) FONT_PUBLIC(font_head)) == (Xv_Font) font_public) {
	    /* at head of list */
	    (void) xv_set(server, XV_KEY_DATA, FONT_HEAD, font->next, 0);
	} else {
	    for (prev = font_head; prev; prev = prev->next) {
		if (prev->next = font) {
		    prev->next = font->next;
		    break;
		}
	    }
#ifdef _XV_DEBUG
	    if (prev == 0)
		abort();
#endif
	}
	/* Free the storage allocated for glyphs. */
	for (i = 0, pfc = &(zap_font_public->pf_char[i]);
	     i <= 255; i++, pfc++) {
	    if (pfc->pc_pr) {
		xv_mem_destroy(pfc->pc_pr);
	    }
	}
	/* free string storage */
	if (font->name)
	    free(font->name);
	if (font->foundry)
	    free(font->foundry);
	if (font->family)
	    free(font->family);
	if (font->style)
	    free(font->style);
	if (font->weight)
	    free(font->weight);
	if (font->slant)
	    free(font->slant);
	if (font->setwidthname)
	    free(font->setwidthname);
	if (font->addstylename)
	    free(font->addstylename);
	/* Remove the font from X server list, and free our private data. */
	display = font->display;
	if (!display)
	    display = (Xv_opaque) xv_get(xv_default_server, XV_DISPLAY);
	xv_unload_x_font(display, font->x_font_info);
	free(font);
    }
    return XV_OK;
}

Xv_private      Xv_Font
xv_font_with_name(server, name)
    Xv_opaque       server;
    char           *name;
{
    register Font_info *font, *font_list;
    Xv_Font         font_public;

    name = normalize_font_name(name);
    /*
     * Get "open fonts" list for specified server (may not be one yet), and
     * look to see if specified font has already been opened.
     */
    font_list = (Font_info *) xv_get(server, XV_KEY_DATA, FONT_HEAD);
    font_public = (font_list) ? FONT_PUBLIC(font_list) : (Xv_Font) 0;
    for (font = font_list; font; font = font->next) {
	if (font_string_compare(name, font->name) == 0)
	    break;
    }

    if (font) {
	font_public = FONT_PUBLIC(font);
    } else {
	/*
	 * Get "unknown fonts" list (may not be one yet) and see if we have
	 * already tried to open specified font previously.
	 */
	Unknown         unknown_list = (Unknown) xv_get(server,
					    XV_KEY_DATA, FONT_UNKNOWN_HEAD);
	Unknown         unknown;
	for (unknown = unknown_list; unknown; unknown = unknown->next) {
	    if (font_string_compare(name, unknown->name) == 0)
		break;
	}
	if (unknown == 0) {
	    /*
	     * Try to open the font, since it is not yet available
	     */
	    font_public = xv_create(server, FONT,
				    FONT_NAME, name,
				    0);
	    if (!font_public) {
		/* Add this name to list of unknown fonts. */
		unknown = xv_alloc(struct _unknown);
		unknown->name = xv_strsave(name);
		if (unknown_list) {
		    unknown->next = unknown_list->next;
		    unknown_list->next = unknown;
		} else {
		    unknown->next = (Unknown) 0;
		    (void) xv_set(server,
				  XV_KEY_DATA, FONT_UNKNOWN_HEAD, unknown,
				  XV_KEY_DATA_REMOVE_PROC, FONT_UNKNOWN_HEAD,
				  font_unknown_list_free,
				  0);
		}
		if (font_string_compare(name, DEFAULT_FONT_NAME) != 0) {
		    char            dummy[128];

		    (void) sprintf(dummy, "Attempting to load font '%s%' instead",
				   DEFAULT_FONT_NAME);
		    xv_error(NULL, XV_ERROR_RECOVERABLE,
			     XV_ERROR_INTERNAL,
			     XV_ERROR_STRING, dummy,
			     XV_ERROR_PKG, ATTR_PKG_FONT, 0);
		    return (xv_font_with_name(server, DEFAULT_FONT_NAME));
		}
		if (font_string_compare(name, DEFAULT_DEFAULT_FONT_NAME) != 0) {
		    char            dummy[128];

		    (void) sprintf(dummy, "Loading default font '%s%' instead",
				   DEFAULT_DEFAULT_FONT_NAME);
		    xv_error(NULL, XV_ERROR_RECOVERABLE,
			     XV_ERROR_INTERNAL,
			     XV_ERROR_STRING, dummy,
			     XV_ERROR_PKG, ATTR_PKG_FONT, 0);
		    return (xv_font_with_name(server, DEFAULT_DEFAULT_FONT_NAME));
		}
	    }
	} else {
	    /*
	     * Font known not to exist: return default.
	     */
	    if (font_string_compare(name, DEFAULT_FONT_NAME) == 0) {
		/* look for "fixed" font */
		return (xv_font_with_name(server, DEFAULT_DEFAULT_FONT_NAME));
	    } else
		return (xv_font_with_name(server, DEFAULT_FONT_NAME));
	}
    }
    return (font_public);
}

static char    *
font_determine_font_name(my_attrs)
    Font_return_attrs my_attrs;
{
    char            name[512], this_name[512];
    int             size;

    /*
     * Return null if no family/style specified
     */
    if ( (my_attrs->family == (char *) NULL)
	&& (my_attrs->style == (char *) NULL) 
	&& (my_attrs->weight == (char *) NULL) 
	&& (my_attrs->slant == (char *) NULL) )
	return (char *) NULL;

    if (font_string_compare(my_attrs->family, FONT_FAMILY_DEFAULT) == 0) {
	if (my_attrs->free_family) {
	    free(my_attrs->family);
	    my_attrs->free_family = 0;
	}
	if (font_default_family) {
	    my_attrs->family = xv_strsave(font_default_family);
	    my_attrs->free_family = 1;
	} else {
	    my_attrs->family = (char *) 0;
	    my_attrs->free_family = 0;
	}
    }
    if (font_string_compare(my_attrs->family, FONT_FAMILY_DEFAULT_FIXEDWIDTH) == 0) {
	if (my_attrs->free_family) {
	    free(my_attrs->family);
	    my_attrs->free_family = 0;
	}
	if (font_default_fixedwidth_family) {
	    my_attrs->family = xv_strsave(font_default_fixedwidth_family);
	    my_attrs->free_family = 1;
	} else {
	    my_attrs->family = (char *) 0;
	    my_attrs->free_family = 0;
	}
    }
    if (font_string_compare(my_attrs->style, FONT_STYLE_DEFAULT) == 0) {
	if (my_attrs->free_style) {
	    free(my_attrs->style);
	    my_attrs->free_style = 0;
	}
	if (font_default_style) {
	    my_attrs->style = xv_strsave(font_default_style);
	    my_attrs->free_style = 1;
	} else {
	    my_attrs->style = (char *) 0;
	    my_attrs->free_style = 0;
	}
    }
    /*
     * Isa
     * Convert font style into weight and slant
     */
    if ((my_attrs->style) && (!(my_attrs->weight) || !(my_attrs->slant)))  {
        font_convert_style(my_attrs);
    }

    if (my_attrs->size == FONT_SIZE_DEFAULT) {
	my_attrs->size = font_size_from_scale_and_sizes(
							my_attrs->scale,
	     my_attrs->small_size = (my_attrs->small_size == FONT_NO_SIZE) ?
			     DEFAULT_SMALL_FONT_SIZE : my_attrs->small_size,
	   my_attrs->medium_size = (my_attrs->medium_size == FONT_NO_SIZE) ?
			   DEFAULT_MEDIUM_FONT_SIZE : my_attrs->medium_size,
	     my_attrs->large_size = (my_attrs->large_size == FONT_NO_SIZE) ?
			     DEFAULT_LARGE_FONT_SIZE : my_attrs->large_size,
							my_attrs->extra_large_size = (my_attrs->extra_large_size == FONT_NO_SIZE) ?
		     DEFAULT_XLARGE_FONT_SIZE : my_attrs->extra_large_size);
	if (my_attrs->size == FONT_NO_SIZE)
	    my_attrs->size = font_default_size;
    }

    /*
     * Isa
     * BUG!!??
     * This if() will cause the scale specification to precede over whatever is
     * specified in size and even in the font's name.
     * Perhaps it is better to use:
    if ((my_attrs->scale == FONT_SCALE_DEFAULT) && (my_attrs->size < 0))
     *
     * instead of just
     *
    if (my_attrs->scale == FONT_SCALE_DEFAULT)
     */
    if ((my_attrs->scale == FONT_SCALE_DEFAULT) && (my_attrs->size < 0)) {
	my_attrs->scale = DEFAULT_FONT_SCALE;
	my_attrs->size = font_size_from_scale_and_sizes(
							my_attrs->scale,
	     my_attrs->small_size = (my_attrs->small_size == FONT_NO_SIZE) ?
			     DEFAULT_SMALL_FONT_SIZE : my_attrs->small_size,
	   my_attrs->medium_size = (my_attrs->medium_size == FONT_NO_SIZE) ?
			   DEFAULT_MEDIUM_FONT_SIZE : my_attrs->medium_size,
	     my_attrs->large_size = (my_attrs->large_size == FONT_NO_SIZE) ?
			     DEFAULT_LARGE_FONT_SIZE : my_attrs->large_size,
							my_attrs->extra_large_size = (my_attrs->extra_large_size == FONT_NO_SIZE) ?
		     DEFAULT_XLARGE_FONT_SIZE : my_attrs->extra_large_size);
	if (my_attrs->size == FONT_NO_SIZE)
	    my_attrs->size = font_default_size;
    }

    size = ((my_attrs->size != FONT_NO_SIZE) || (my_attrs->size >= 0))
	? my_attrs->size : font_size_from_scale_and_sizes(
							  my_attrs->scale,
						       my_attrs->small_size,
						      my_attrs->medium_size,
						       my_attrs->large_size,
						my_attrs->extra_large_size);

    this_name[0] = '\0';
    name[0] = '\0';

    sprintf(name, "-%s-%s-%s-%s-%s-%s",
			(my_attrs->foundry ? my_attrs->foundry:"*"), 
			(my_attrs->family ? my_attrs->family:"*"), 
			(my_attrs->weight ? my_attrs->weight:"*"), 
			(my_attrs->slant ? my_attrs->slant:"*"), 
			(my_attrs->setwidthname ? my_attrs->setwidthname:"*"),
			(my_attrs->addstylename ? my_attrs->addstylename:"*") );

    /*
     * If at this time, we still don't have a usable size, use the default
     */
    if (size != FONT_NO_SIZE) {
	my_attrs->size = size;
    } else  {
	my_attrs->size = size = font_default_size;
    }

    sprintf(this_name, "%s-%i-*-*-*-*-*-*-*", name, size);
    
    /*
     * Isa
     * Check if no important attrs managed to be filled in
     * BUG!!?? What are important attributes. The list below
     * is probably too long a list. Perhaps family, style 
     * alone is sufficient
    if ( !(my_attrs->family) &&
	 !(my_attrs->weight) &&
	 !(my_attrs->slant) )  {

	return (char *) NULL;
    }
     */

    if (this_name[0]) {		/* any name constructed */
	my_attrs->name = xv_strsave(this_name);
	my_attrs->free_name = 1;
	if (font_decrypt_sv1_name(my_attrs) != 0) {
	    /* not sv1 name */
	    if (font_decrypt_xv_name(my_attrs) != 0)
		/* not xv name */
		font_decrypt_xlfd_name(my_attrs);
	};
	return (char *) my_attrs->name;
    } else
	return (char *) NULL;
}

static int
font_size_from_scale_and_sizes(scale, small, med, large, xlarge)
    int             scale, small, med, large, xlarge;
{
    switch (scale) {
      case WIN_SCALE_SMALL:
	if (small == FONT_NO_SIZE)
	    return DEFAULT_SMALL_FONT_SIZE;
	return small;
      case WIN_SCALE_MEDIUM:
	if (med == FONT_NO_SIZE)
	    return DEFAULT_MEDIUM_FONT_SIZE;
	return med;
      case WIN_SCALE_LARGE:
	if (large == FONT_NO_SIZE)
	    return DEFAULT_LARGE_FONT_SIZE;
	return large;
      case WIN_SCALE_EXTRALARGE:
	if (xlarge == FONT_NO_SIZE)
	    return DEFAULT_XLARGE_FONT_SIZE;
	return xlarge;
      default:
	return FONT_NO_SIZE;
    }
}

static int
font_size_from_name(name, size)
    char           *name;
    int            *size;
{
    char            family_style[512];
    int             my_size, n;

    family_style[0] = '\0';
    n = sscanf(name, "%[-.A-Z_a-z]%d", family_style, &my_size);
    if (n == 2) {
	*size = my_size;
	return (0);
    } else
	return (-1);
}

static int
font_familystyle_from_name(name, familystyle)
    char           *name;
    char           *familystyle;
{
    char            my_familystyle[512];

    my_familystyle[0] = '\0';
    if (my_familystyle[0]) {
	familystyle[0] = '\0';
	strncat(familystyle, my_familystyle, strlen(my_familystyle));
	return (0);
    } else
	return (-1);
}

static char    *
font_rescale_from_font(font, scale, attrs)
    Font_info      *font;
    int             scale;
    struct font_return_attrs *attrs;
{
    char           *font_name = NULL;
    char            new_name[256], name[512];
    int             desired_scale;

    if (!font)			/* if possibly not set? */
	return (char *) font_name;
    name[0] = '\0';
    if ((scale < (int) WIN_SCALE_SMALL) ||
	(scale > (int) WIN_SCALE_EXTRALARGE) ||
	(scale == FONT_NO_SCALE))
	return (char *) font_name;	/* no scaling */
    if ((font->family == 0) && (font->style == 0) && 
		(font->weight == 0) && (font->slant == 0)) {
	if (font->name == 0)
	    return (char *) font_name;	/* you're really out of luck! */
	/*
	 * Try to decrypt the attributes from the name
	 */
	if (font_decrypt_xlfd_name(attrs) == -1)  {
	    return (char *) font_name;	/* you're REALLY out of luck! */
	}
    }
    
	/* munch everything together */
        sprintf(name, "-%s-%s-%s-%s-%s-%s",
			(font->foundry ? font->foundry:"*"), 
			(font->family ? font->family:"*"), 
			(font->weight ? font->weight:"*"), 
			(font->slant ? font->slant:"*"), 
			(font->setwidthname ? font->setwidthname:"*"),
			(font->addstylename ? font->addstylename:"*") );


	if (font->foundry) {
		attrs->foundry = xv_strsave(font->foundry);
		attrs->free_foundry = 1;
	}
	if (font->family) {
		attrs->family = xv_strsave(font->family);
		attrs->free_family = 1;
	}
	if (font->style) {
		attrs->style = xv_strsave(font->style);
		attrs->free_style = 1;
	}
	if (font->weight) {
		attrs->weight = xv_strsave(font->weight);
		attrs->free_weight = 1;
	}
	if (font->slant) {
		attrs->slant = xv_strsave(font->slant);
		attrs->free_slant = 1;
	}
	if (font->setwidthname) {
		attrs->setwidthname = xv_strsave(font->setwidthname);
		attrs->free_setwidthname = 1;
	}
	if (font->addstylename) {
		attrs->addstylename = xv_strsave(font->addstylename);
		attrs->free_addstylename = 1;
	}

    switch (scale) {
      case WIN_SCALE_SMALL:
	desired_scale = font->small_size;
	break;
      case WIN_SCALE_MEDIUM:
	desired_scale = font->medium_size;
	break;
      case WIN_SCALE_LARGE:
	desired_scale = font->large_size;
	break;
      case WIN_SCALE_EXTRALARGE:
	desired_scale = font->extra_large_size;
	break;
      default:
	desired_scale = -1;
    }
    if (desired_scale == -1)
	return (char *) font_name;	/* no font that scale */
    new_name[0] = '\0';

    /*
     * If cannot get a size, give the default
     */
    if (desired_scale == FONT_NO_SIZE) {
	desired_scale = font_default_size;
    }
    
    sprintf(new_name, "%s-%i-*-*-*-*-*-*-*", name, desired_scale);

    attrs->name = xv_strsave(new_name);
    attrs->free_name = 1;
    attrs->size = desired_scale;
    attrs->scale = scale;
    attrs->small_size = font->small_size;
    attrs->medium_size = font->medium_size;
    attrs->large_size = font->large_size;
    attrs->extra_large_size = font->extra_large_size;

    return (attrs->name);
}

Xv_object
font_find_font(parent_public, pkg, avlist)
    Xv_opaque       parent_public;
    Xv_pkg         *pkg;
    Attr_avlist     avlist;
{
    int             font_attrs_exist;
    struct font_info *font_list;
    struct font_return_attrs my_attrs;
    char           *font_name = (char *) NULL;
    char           *this_name;
    int             need_to_free_font_name = 0;
    Xv_opaque       server;

    /* initialization */
    my_attrs.free_name = my_attrs.free_foundry = my_attrs.free_family = my_attrs.free_style 
		= my_attrs.free_weight = my_attrs.free_slant 
		= my_attrs.free_setwidthname 
		= my_attrs.free_addstylename = (int) 0;
    my_attrs.name = my_attrs.foundry = my_attrs.family 
			= my_attrs.style = my_attrs.weight 
			= my_attrs.slant = my_attrs.setwidthname 
			= my_attrs.addstylename = (char *)0;
    my_attrs.size = (int) FONT_NO_SIZE;
    my_attrs.small_size = my_attrs.medium_size = my_attrs.large_size =
	my_attrs.extra_large_size = (int) FONT_NO_SIZE;
    my_attrs.scale = FONT_NO_SCALE;
    my_attrs.rescale_factor = (int) 0;
    my_attrs.resize_from_font = (Font_info *) 0;

    font_attrs_exist = font_read_attrs(&my_attrs, FALSE, avlist);	/* get attrs */
    if (!font_attrs_exist)
	(void) font_default_font(&my_attrs);

    if (!parent_public) {
	/* xv_create/xv_find ensures that xv_default_server is valid. */
	server = (Xv_opaque) xv_default_server;
    } else {
	Xv_pkg         *pkgtype = (Xv_pkg *) xv_get(parent_public, XV_TYPE);
	if ((Xv_pkg *) pkgtype == (Xv_pkg *) & xv_server_pkg) {
	    server = parent_public;
	} else {
	    server = (Xv_opaque) XV_SERVER_FROM_WINDOW(parent_public);
	}
    }

    if (my_attrs.name) {
	my_attrs.name = normalize_font_name(my_attrs.name);
	if (font_decrypt_sv1_name(&my_attrs) != 0) {
	    /* not sv1 name */
	    if (font_decrypt_xv_name(&my_attrs) != 0) {
		/* not xv name */
		font_decrypt_xlfd_name(&my_attrs);
	    }
	}
    } else if (my_attrs.resize_from_font) {
	font_name = font_rescale_from_font(
					(Xv_Font) my_attrs.resize_from_font,
					   (int) my_attrs.rescale_factor,
					   &my_attrs);
	if ((font_name == NULL) || (strlen(font_name) == 0))
	    return ((Xv_object) 0);
    } else {
	font_name = font_determine_font_name(&my_attrs);
	if ((font_name == NULL) || (strlen(font_name) == 0)) {
	    return ((Xv_object) 0);
	} else {
	    font_name = xv_strsave(font_name);
	    need_to_free_font_name = 1;	/* free name later */
	}
    }

    this_name = (font_name) ? font_name : my_attrs.name;
    if (font_list = (Font_info *) xv_get(server, XV_KEY_DATA, FONT_HEAD)) {
	while (font_list) {
	    if (((font_string_compare(this_name, font_list->name) == 0)
		 && (this_name != (char *) 0)
		 && (font_list->name != (char *) 0))
	    /*
	     * first above for name, else below for family/style/size/scale
	     * match
	     */
		|| (((font_string_compare(my_attrs.family, font_list->family) == 0)
		     && (my_attrs.family != (char *) 0)
		     && (font_list->family != (char *) 0))
	     && (font_string_compare(my_attrs.style, font_list->style) == 0)
		    && ((font_list->size == my_attrs.size) ||
			(font_list->scale == my_attrs.scale)))) {
		if (need_to_free_font_name) {
		    free(font_name);
		}
		font_free_font_return_attr_strings(&my_attrs);
		(void) xv_set(FONT_PUBLIC(font_list), XV_INCREMENT_REF_COUNT, 0);
		return (FONT_PUBLIC(font_list));
	    }
	    font_list = font_list->next;
	}
    }
    return ((Xv_object) 0);
}

font_free_font_return_attr_strings(attrs)
    struct font_return_attrs *attrs;
{
    if (attrs->free_name) {
	free(attrs->name);
	attrs->free_name = 0;
    }
    if (attrs->free_foundry) {
	free(attrs->foundry);
	attrs->free_foundry = 0;
    }
    if (attrs->free_family) {
	free(attrs->family);
	attrs->free_family = 0;
    }
    if (attrs->free_style) {
	free(attrs->style);
	attrs->free_style = 0;
    }
    if (attrs->free_weight) {
	free(attrs->weight);
	attrs->free_weight = 0;
    }
    if (attrs->free_slant) {
	free(attrs->slant);
	attrs->free_slant = 0;
    }
    if (attrs->free_setwidthname) {
	free(attrs->setwidthname);
	attrs->free_setwidthname = 0;
    }
    if (attrs->free_addstylename) {
	free(attrs->addstylename);
	attrs->free_addstylename = 0;
    }
}

/*
 * the following proc is a wrapper for strcmp() strncmp() such that it will
 * return =0 if both strings are NULL and !=0 if one or other is NULL, and
 * standard strcmp otherwise. BUG: strcmp will seg fault if either str is
 * NULL.
 */
static int
font_string_compare(str1, str2)
    char           *str1, *str2;
{
    if ((str1 == NULL) && (str2 == NULL)) {
	return (int) 0;		/* they're the same (ie. nothing */
    } else if ((str1 == NULL) || (str2 == NULL)) {
	return (int) -1;	/* avoid seg fault */
    } else
	return (int) strcmp(str1, str2);
}

static int
font_string_compare_nchars(str1, str2, n_chars)
    char           *str1, *str2;
    int             n_chars;
{
    int             result;
    int             len1 = (str1) ? strlen(str1) : 0;
    int             len2 = (str2) ? strlen(str2) : 0;
    if ((len1 == 0) && (len2 == NULL)) {
	return (int) 0;		/* they're the same (ie. nothing */
    } else if ((len1 && !len2) || (!len1 && len2)) {
	return (int) -1;	/* They're different strings */
    } else if ((!len1) || (!len2)) {
	return (int) -1;	/* avoid seg fault */
    } else
	result = strncmp(str1, str2, n_chars);
    return (int) result;
}

static int
font_decrypt_sv1_name(my_attrs)
    Font_return_attrs my_attrs;
{
    struct font_translation current_entry;
    int             i;

    if (my_attrs->name == NULL)
	return;
    for (i = 0, current_entry = known_sv1fonts[i];
	 current_entry.sv1_name;
	 current_entry = known_sv1fonts[i]) {
	if (font_string_compare_nchars(current_entry.sv1_name, my_attrs->name,
				     strlen(current_entry.sv1_name)) == 0) {

	    my_attrs->foundry = current_entry.sv1_foundry;
	    my_attrs->family = current_entry.sv1_family;
	    my_attrs->style = current_entry.sv1_style;
	    my_attrs->weight = current_entry.sv1_weight;
	    my_attrs->slant = current_entry.sv1_slant;
	    my_attrs->setwidthname = current_entry.sv1_setwidthname;
	    my_attrs->addstylename = current_entry.sv1_addstylename;
	    my_attrs->size = current_entry.size;
	    my_attrs->scale = current_entry.scale;

	    return (0);
	}
	i++;
    }
    return (-1);
}

static int
font_decrypt_xv_name(my_attrs)
    Font_return_attrs my_attrs;
{
    struct font_definitions current_entry;
    int             i;

    char            family_style[128], nosize_entry[128], small_entry[128], 
		    med_entry[128], large_entry[128], xlarge_entry[128];
    int             len_of_name, match = 0;

    if (my_attrs->name == NULL)
	return;
    len_of_name = strlen(my_attrs->name);
    for (i = 0, current_entry = known_xvfonts[i];
	 current_entry.xv_family;
	 current_entry = known_xvfonts[i]) {
	family_style[0] = '\0';
	nosize_entry[0] = '\0';
	small_entry[0] = '\0';
	med_entry[0] = '\0';
	large_entry[0] = '\0';
	xlarge_entry[0] = '\0';

	if (current_entry.xv_style != (char *) 0) {
	    (void) sprintf(family_style, "%s-%s",
			current_entry.xv_family,
			current_entry.xv_style);
	}
	else  {
	    (void) sprintf(family_style, "%s",
			current_entry.xv_family);
			
	}

	sprintf(nosize_entry, "%s", family_style);
	sprintf(small_entry, "%s-%i", family_style, current_entry.small_size);
	sprintf(med_entry, "%s-%i", family_style, current_entry.medium_size);
	sprintf(large_entry, "%s-%i", family_style, current_entry.large_size);
	sprintf(xlarge_entry, "%s-%i", family_style, current_entry.extra_large_size);
	if (font_string_compare_nchars(small_entry, my_attrs->name,
			      MAX(strlen(small_entry), len_of_name)) == 0) {
	    my_attrs->foundry = current_entry.foundry;
	    my_attrs->family = current_entry.family;
	    my_attrs->weight = current_entry.weight;
	    my_attrs->slant = current_entry.slant;
	    my_attrs->setwidthname = current_entry.setwidthname;
	    my_attrs->addstylename = current_entry.addstylename;

	    my_attrs->style = current_entry.xv_style;
	    my_attrs->size = current_entry.small_size;
	    my_attrs->scale = (int) WIN_SCALE_SMALL;
	    my_attrs->small_size = current_entry.small_size;
	    my_attrs->medium_size = current_entry.medium_size;
	    my_attrs->large_size = current_entry.large_size;
	    my_attrs->extra_large_size = current_entry.extra_large_size;
	    match = 1;
	} else if (font_string_compare_nchars(med_entry, my_attrs->name,
				MAX(strlen(med_entry), len_of_name)) == 0) {
	    my_attrs->foundry = current_entry.foundry;
	    my_attrs->family = current_entry.family;
	    my_attrs->weight = current_entry.weight;
	    my_attrs->slant = current_entry.slant;
	    my_attrs->setwidthname = current_entry.setwidthname;
	    my_attrs->addstylename = current_entry.addstylename;

	    my_attrs->style = current_entry.xv_style;
	    my_attrs->size = current_entry.medium_size;
	    my_attrs->scale = (int) WIN_SCALE_MEDIUM;
	    my_attrs->small_size = current_entry.small_size;
	    my_attrs->medium_size = current_entry.medium_size;
	    my_attrs->large_size = current_entry.large_size;
	    my_attrs->extra_large_size = current_entry.extra_large_size;
	    match = 1;
	} else if (font_string_compare_nchars(large_entry, my_attrs->name,
			      MAX(strlen(large_entry), len_of_name)) == 0) {
	    my_attrs->foundry = current_entry.foundry;
	    my_attrs->family = current_entry.family;
	    my_attrs->weight = current_entry.weight;
	    my_attrs->slant = current_entry.slant;
	    my_attrs->setwidthname = current_entry.setwidthname;
	    my_attrs->addstylename = current_entry.addstylename;

	    my_attrs->style = current_entry.xv_style;
	    my_attrs->size = current_entry.large_size;
	    my_attrs->scale = (int) WIN_SCALE_LARGE;
	    my_attrs->small_size = current_entry.small_size;
	    my_attrs->medium_size = current_entry.medium_size;
	    my_attrs->large_size = current_entry.large_size;
	    my_attrs->extra_large_size = current_entry.extra_large_size;
	    match = 1;
	} else if (font_string_compare_nchars(xlarge_entry, my_attrs->name,
			     MAX(strlen(xlarge_entry), len_of_name)) == 0) {
	    my_attrs->foundry = current_entry.foundry;
	    my_attrs->family = current_entry.family;
	    my_attrs->weight = current_entry.weight;
	    my_attrs->slant = current_entry.slant;
	    my_attrs->setwidthname = current_entry.setwidthname;
	    my_attrs->addstylename = current_entry.addstylename;

	    my_attrs->style = current_entry.xv_style;
	    my_attrs->size = current_entry.extra_large_size;
	    my_attrs->scale = (int) WIN_SCALE_EXTRALARGE;
	    my_attrs->small_size = current_entry.small_size;
	    my_attrs->medium_size = current_entry.medium_size;
	    my_attrs->large_size = current_entry.large_size;
	    my_attrs->extra_large_size = current_entry.extra_large_size;
	    match = 1;
	} else if (font_string_compare_nchars(nosize_entry, my_attrs->name,
			     MAX(strlen(nosize_entry), len_of_name)) == 0) {
	    my_attrs->foundry = current_entry.foundry;
	    my_attrs->family = current_entry.family;
	    my_attrs->weight = current_entry.weight;
	    my_attrs->slant = current_entry.slant;
	    my_attrs->setwidthname = current_entry.setwidthname;
	    my_attrs->addstylename = current_entry.addstylename;

	    my_attrs->style = current_entry.xv_style;
	    my_attrs->size = font_default_size;
	    my_attrs->scale = font_default_scale;
	    my_attrs->small_size = current_entry.small_size;
	    my_attrs->medium_size = current_entry.medium_size;
	    my_attrs->large_size = current_entry.large_size;
	    my_attrs->extra_large_size = current_entry.extra_large_size;
	    match = 1;
	}

	if (match)  {
	    return(0);
	}

	i++;
    }
    return (-1);
}

static void
font_decrypt_family_style(return_attrs)
    Font_return_attrs return_attrs;
{
    struct xv_to_x_font_translation current_entry;
    register int    i;
    char           *requested_family = return_attrs->family;
    char           *requested_style = return_attrs->style;

    /*
     * If family/style is not specified, give the default
     */
    if (!(requested_family) && !(return_attrs->name))  {
	requested_family = return_attrs->family = FONT_FAMILY_DEFAULT;
    }

    if (!(requested_style) && !(return_attrs->name))  {
	requested_style = return_attrs->style = FONT_STYLE_DEFAULT;
    }

    if (font_string_compare(requested_family,
			    FONT_FAMILY_DEFAULT) == 0)
	requested_family = return_attrs->family
	    = font_default_family;
    if (font_string_compare(requested_family,
			    FONT_FAMILY_DEFAULT_FIXEDWIDTH) == 0)
	requested_family = return_attrs->family
	    = font_default_fixedwidth_family;
    if (!requested_family)
	requested_family = "";	/* avoid segv in strlen */
    if (font_string_compare(requested_style,
			    FONT_STYLE_DEFAULT) == 0)
	requested_style = return_attrs->style
	    = font_default_style;
    if (!requested_style)
	requested_style = "";	/* avoid segv in strlen */
    /* otherwise, leave family/style alone */

    if (requested_family && strlen(requested_family)) {
	for (i = 0, current_entry = known_openwin_fonts[i];
	     current_entry.xv_family;
	     current_entry = known_openwin_fonts[i]) {

	    if (((font_string_compare_nchars(current_entry.xv_family,
			   requested_family, strlen(requested_family)) == 0)
		 || (font_string_compare_nchars(current_entry.x_font_family,
			  requested_family, strlen(requested_family)) == 0))
		&& ((font_string_compare_nchars(current_entry.xv_style,
			     requested_style, strlen(requested_style)) == 0)
		  || (font_string_compare_nchars(current_entry.x_font_style,
			 requested_style, strlen(requested_style)) == 0))) {
		/* assign x font names */
		return_attrs->foundry = current_entry.x_font_foundry;
		return_attrs->family = current_entry.x_font_family;
		return_attrs->style = current_entry.x_font_style;
		return_attrs->weight = current_entry.x_font_weight;
		return_attrs->slant = current_entry.x_font_slant;
		return_attrs->setwidthname = current_entry.x_font_setwidthname;
		return_attrs->addstylename = current_entry.x_font_addstylename;
		return;
	    } else
		i++;
	}
    }
}

static void
font_decrypt_rescale_info(return_attrs)
    Font_return_attrs return_attrs;
{
    /* determine family-style of font wished to rescaled against */
    if (return_attrs->resize_from_font) {
	if (!return_attrs->family && return_attrs->resize_from_font->family)
	    return_attrs->family = return_attrs->resize_from_font->family;
	if (!return_attrs->style && return_attrs->resize_from_font->style)
	    return_attrs->style = return_attrs->resize_from_font->style;
	return_attrs->scale = return_attrs->rescale_factor;
	(void) font_decrypt_family_style(return_attrs);
    }
}

static void
font_decrypt_size(return_attrs)
    Font_return_attrs return_attrs;
{
    /* determine size */
    if (return_attrs->size == FONT_SIZE_DEFAULT) {
	return_attrs->size = (int) font_default_size;
    }
}

static void
font_decrypt_scale(return_attrs)
    Font_return_attrs return_attrs;
{
    /* determine scale */
    if (return_attrs->scale == FONT_SCALE_DEFAULT) {
	return_attrs->scale = (int) font_default_scale;
    }
}

static int
font_read_attrs(return_attrs, consume_attrs, avlist)
    Font_return_attrs return_attrs;
    int             consume_attrs;
    Attr_avlist     avlist;
{
    register Attr_avlist attrs;
    int             font_attrs_exist = 0;

    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch ((int) attrs[0]) {
	  case FONT_NAME:
	    return_attrs->name = (char *) attrs[1];
	    font_attrs_exist = 1;
	    if (consume_attrs)
		ATTR_CONSUME(attrs[0]);
	    break;
	  case FONT_FAMILY:
	    font_attrs_exist = 1;
	    return_attrs->family = (char *) attrs[1];
	    if (consume_attrs)
		ATTR_CONSUME(attrs[0]);
	    break;
	  case FONT_STYLE:
	    font_attrs_exist = 1;
	    return_attrs->style = (char *) attrs[1];
	    if (consume_attrs)
		ATTR_CONSUME(attrs[0]);
	    break;
	  case FONT_SIZE:
	    font_attrs_exist = 1;
	    return_attrs->size = (int) attrs[1];
	    if (consume_attrs)
		ATTR_CONSUME(attrs[0]);
	    break;
	  case FONT_SCALE:
	    font_attrs_exist = 1;
	    return_attrs->scale = (int) attrs[1];
	    if (consume_attrs)
		ATTR_CONSUME(attrs[0]);
	    break;
	  case FONT_SIZES_FOR_SCALE:{
		font_attrs_exist = 1;
		return_attrs->small_size = (int) attrs[1];
		return_attrs->medium_size = (int) attrs[2];
		return_attrs->large_size = (int) attrs[3];
		return_attrs->extra_large_size = (int) attrs[4];
		if (consume_attrs)
		    ATTR_CONSUME(attrs[0]);
		break;
	    }
	  case FONT_RESCALE_OF:{
		Xv_opaque       pf = (Xv_opaque) attrs[1];
		Xv_Font         font = 0;

		XV_OBJECT_TO_STANDARD(pf, "font_read_attrs", font);
		font_attrs_exist = 1;
		return_attrs->resize_from_font = (Font_info *) FONT_PRIVATE(
								      font);
		return_attrs->rescale_factor = (int) attrs[2];
		if (consume_attrs)
		    ATTR_CONSUME(attrs[0]);
		break;
	    }
	  default:
	    break;
	}
    }
    if (!font_attrs_exist) {
	return (font_attrs_exist);
    } else {
	font_decrypt_family_style(return_attrs);
	font_decrypt_size(return_attrs);
	font_decrypt_scale(return_attrs);
	font_decrypt_rescale_info(return_attrs);
	return (font_attrs_exist);
    }
}

static void
font_default_font(return_attrs)
    Font_return_attrs return_attrs;
{
    return_attrs->family = font_default_family;
    return_attrs->style = font_default_style;
    return_attrs->weight = font_default_weight;
    return_attrs->slant = font_default_slant;
    return_attrs->setwidthname = font_default_setwidthname;
    return_attrs->addstylename = font_default_addstylename;
    return_attrs->size = font_default_size;
    return_attrs->scale = (int) font_default_scale;
}

font_convert_style(return_attrs)
Font_return_attrs return_attrs;
{
    Style_defs	current_entry;
    int		i, len_of_style;

    len_of_style = (return_attrs->style) ? strlen(return_attrs->style):0;

    for (i=0, current_entry = known_styles[i]; current_entry.weight; 
		current_entry = known_styles[++i])  {
	if (font_string_compare_nchars(current_entry.style, return_attrs->style,
			      MAX(strlen(current_entry.style), len_of_style)) == 0) {

	    return_attrs->weight = current_entry.weight;
	    return_attrs->slant = current_entry.slant;

	    return(0);
	}
    }

    return(-1);
}

font_convert_weightslant(return_attrs)
Font_return_attrs return_attrs;
{
    Style_defs	current_entry;
    int		i, len_of_weight, len_of_slant;

    len_of_weight = (return_attrs->weight) ? strlen(return_attrs->weight):0;
    len_of_slant = (return_attrs->slant) ? strlen(return_attrs->slant):0;

    for (i=0, current_entry = known_styles[i]; current_entry.weight; 
		current_entry = known_styles[++i])  {
	if ( (font_string_compare_nchars(current_entry.weight, return_attrs->weight,
			      MAX(strlen(current_entry.weight), len_of_weight)) == 0) &&
	     (font_string_compare_nchars(current_entry.slant, return_attrs->slant,
			      MAX(strlen(current_entry.slant), len_of_slant)) == 0) ) {

	    return_attrs->style = current_entry.style;

	    return(0);
	}
    }

    return(-1);
}


int
font_dashcount(str)
char	*str;
{
    register char	*p1, *p2 = str;
    int		count = 0;

    if (str == NULL)  {
	return(0);
    }

    p1 = index(str, DASH);

    if (p1 == NULL)  {
	return(0);
    }

    ++count;

    while(p2 != NULL)  {
	p2 = index(p1+1, DASH);

	if (p2 != NULL)  {
	    ++count;
	    p1 = p2;
	}
    }

    return(count);
}

char *
strip_xlfd(str, pos)
char	*str;
int	pos;
{
    char	*p1 = str, *p2;

    if ((str == NULL) || (pos < 0))  {
	return((char *)NULL);
    }
    
    while(pos)  {
	p2 = index(p1, DASH);
	if (p2 == NULL)  {
	    return((char *)NULL);
	}
	--pos;
	p1 = p2 + 1;
    }

    /*
     * the string at position `pos` spans p1 to 
     * strip_xlfd(str, pos+1)-1
     */
    return(p1);
}

static int
font_decrypt_xlfd_name(my_attrs)
Font_return_attrs my_attrs;
{
    char		tempName[255];
    char		*foundry = NULL;
    char		*family = NULL;
    char		*weight = NULL;
    char		*slant = NULL;
    char		*setwidthname = NULL;
    char		*addstylename = NULL;
    char		*pixsize = NULL;
    char		*ptsize = NULL;

    if (my_attrs->name == NULL)  {
	return(-1);
    }

    if (font_dashcount(my_attrs->name) != NUMXLFDFIELDS)  {
	return(-1);
    }

    sprintf(tempName, "%s", my_attrs->name);

    foundry = strip_xlfd(tempName, FOUNDRYPOS);
    family = strip_xlfd(tempName, FAMILYPOS);
    weight = strip_xlfd(tempName, WEIGHTPOS);
    slant = strip_xlfd(tempName, SLANTPOS);
    setwidthname = strip_xlfd(tempName, SETWIDTHNAMEPOS);
    addstylename = strip_xlfd(tempName, ADDSTYLENAMEPOS);
    pixsize = strip_xlfd(tempName, PIXSIZEPOS);
    ptsize = strip_xlfd(tempName, PTSIZEPOS);

    *(family-1) = '\0';
    *(weight-1) = '\0';
    *(slant-1) = '\0';
    *(setwidthname-1) = '\0';
    *(addstylename-1) = '\0';
    *(pixsize-1) = '\0';
    *(ptsize-1) = '\0';

    if ((*foundry) && (*foundry != DASH))  {
	my_attrs->foundry = xv_strsave(foundry);
	my_attrs->free_foundry = 1;
    }

    if ((*foundry) && (*family != DASH))  {
	my_attrs->family = xv_strsave(family);
	my_attrs->free_family = 1;
    }

    if ((*weight) && (*weight != DASH))  {
	my_attrs->weight = xv_strsave(weight);
	my_attrs->free_weight = 1;
    }

    if ((*slant) && (*slant != DASH))  {
	my_attrs->slant = xv_strsave(slant);
	my_attrs->free_slant = 1;
    }

    if ( (*setwidthname) && (*setwidthname != DASH))  {
	my_attrs->setwidthname = xv_strsave(setwidthname);
	my_attrs->free_setwidthname = 1;
    }

    if ( (*addstylename) && (*addstylename != DASH))  {
	my_attrs->addstylename = xv_strsave(addstylename);
	my_attrs->free_addstylename = 1;
    }

    if ((*pixsize) && (*pixsize != DASH))  {
	my_attrs->size = atoi(pixsize);

	if (my_attrs->size < DEFAULT_MEDIUM_FONT_SIZE)  {
	    my_attrs->scale = (int)WIN_SCALE_SMALL;
	}
	else  {
	    if (my_attrs->size < DEFAULT_LARGE_FONT_SIZE)  {
	        my_attrs->scale = (int)WIN_SCALE_MEDIUM;
	    }
	    else  {
		if (my_attrs->size < DEFAULT_XLARGE_FONT_SIZE)  {
	            my_attrs->scale = (int)WIN_SCALE_LARGE;
		}
		else  {
	            my_attrs->scale = (int)WIN_SCALE_EXTRALARGE;
		}
	    }
	}
    }

    if ((my_attrs->weight) && (my_attrs->slant))  {
	font_convert_weightslant(my_attrs);
    }

    return (0);
}

