/*	"@(#)defaults.h 20.11 89/08/17 SMI	*/

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef	xview_defaults_DEFINED
#define	xview_defaults_DEFINED

/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

#define DEFAULTS_MAX_VALUE_SIZE	128	/* move this to defaults.h */

#ifndef Bool
#define Bool int
#endif


/*
 ***********************************************************************
 *		Typedefs, enumerations, and structs
 ***********************************************************************
 */

typedef struct _default_pairs {
	char	*name;					/* Name of pair */
	int	value;					/* Value of pair */
} Defaults_pairs;


/*
 ***********************************************************************
 *			Globals
 ***********************************************************************
 */

/*
 * Public Functions
 *
 *
 * NOTE: Any returned string pointers should be considered temporary at best.
 * If you want to hang onto the data, make your own private copy of the string!
 */

/*
 * defaults_exists(path_name, status) will return TRUE if path_name exists in
 * the database.
 */
Bool defaults_exists();

/*
 * defaults_get_boolean(name, class, default) will lookup name and class in
 * the defaults database and return TRUE if the value is "True", "Yes", "On",
 * "Enabled", "Set", "Activated", or "1".  FALSE will be returned if the
 * value is "False", "No", "Off", "Disabled", "Reset", "Cleared",
 * "Deactivated", or "0".  If the value is none of the above, a warning
 * message will be displayed and Default will be returned.
 */
Bool defaults_get_boolean();

/*
 * defaults_get_character(name, class, default) will lookup name and class in
 * the defaults database and return the resulting character value.  Default
 * will be returned if any error occurs.
 */
char defaults_get_character();

/*
 * defaults_get_enum(name, class, pairs) will lookup the value associated
 * with name and class, scan the Pairs table and return the associated value.
 * If no match is found, an error is generated and the value associated with
 * last entry (i.e. the NULL entry) is returned.
 */
int defaults_get_enum();

/*
 * defaults_get_integer(name, class, default) will lookup name and class in
 * the defaults database and return the resulting integer value. Default will
 * be returned if any error occurs.
 */
int defaults_get_integer();

/*
 * defaults_get_integer_check(name, class, default, mininum, maximum) will
 * lookup name and class in the defaults database and return the resulting
 * integer value. If the value in the database is not between Minimum and
 * Maximum (inclusive), an error message will be printed.  Default will be
 * returned if any error occurs.
 */
int defaults_get_integer_check();

/*
 * defaults_get_string(name, class, default) will lookup and return the
 * null-terminated string value assocatied with name and class in the
 * defaults database.  Default will be returned if any error occurs.
 */
char *defaults_get_string();

/*
 * defaults_init_db initializes the X11 Resource Manager.
 */
void defaults_init_db();

/*
 * defaults_load_db(filename) will load the server database if filename is
 * NULL, or the database residing in the specified filename.
 */
void defaults_load_db();

/*
 * defaults_store_db(filename) will write the defaults database to the
 * specified file, and update the server Resource Manager property.
 */
void defaults_store_db();

/*
 * defaults_lookup(name, pairs) will linearly scan the Pairs data structure
 * looking for Name.  The value associated with Name will be returned.
 * If Name can not be found in Pairs, the value assoicated with NULL will
 * be returned.  (The Pairs data structure must be terminated with NULL.)
 */
int defaults_lookup();

/*
 * defaults_set_character(resource, value) will set the resource to
 * value.  value is an character. resource is a string.
 */
void defaults_set_character();

/*
 * defaults_set_character(resource, value) will set the resource to
 * value.  value is a integer. resource is a string.
 */
void defaults_set_integer();

/*
 * defaults_set_character(resource, value) will set the resource to
 * value.  value is a Boolean. resource is a string.
 */
void defaults_set_boolean();

/*
 * defaults_set_character(resource, value) will set the resource to
 * value.  value is a string. resource is a string.
 */
void defaults_set_string();

#endif ~xview_defaults_DEFINED
