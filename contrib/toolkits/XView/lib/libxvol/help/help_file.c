/* @(#)help_file.c 1.12 89/10/17 SMI      */

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <xview/pkg.h>

#define DEFAULT_HELP_DIRECTORY "/usr/lib/help"

static FILE    *help_file;
static char     help_buffer[128];

static char    *
help_search_file(key)
    char           *key;
{
    char           *entry, *arg;
    static char     last_arg[64];

    fseek(help_file, 0, 0);

    while (entry = fgets(help_buffer, sizeof(help_buffer), help_file))
	if (*entry++ == ':') {
	    strtok(entry, ":\n");
	    arg = strtok(NULL, "\n");
	    entry = strtok(entry, " \t");

	    while (entry && strcmp(entry, key))
		entry = strtok(NULL, " \t");

	    if (entry) {
		strncpy(last_arg, (arg ? arg : ""), sizeof(last_arg) - 1);
		return (last_arg);
	    }
	}
    return (NULL);
}

Pkg_private char *
xv_help_get_arg(data)
    char           *data;
{
    char           *client, *helpdir, *key, copy_data[64];
    static char     last_client[64];

    if (data == NULL)
	return (NULL);
    strncpy(copy_data, data, sizeof(copy_data));
    copy_data[sizeof(copy_data) - 1] = '\0';
    if (!(client = strtok(copy_data, ":")) || !(key = strtok(NULL, "")))
	return (NULL);
    helpdir = (char *) getenv("HELPDIR");
    if (!helpdir)
	helpdir = DEFAULT_HELP_DIRECTORY;
    if (strcmp(last_client, client)) {
	sprintf(help_buffer, "%s/%s.info", helpdir, client);
	if (help_file) {
	    fclose(help_file);
	    last_client[0] = '\0';
	}
	if ((help_file = fopen(help_buffer, "r")) == NULL)
	    return (NULL);
	strcpy(last_client, client);
    }
    return (help_search_file(key));
}

Pkg_private char *
xv_help_get_text()
{
    char           *ptr = fgets(help_buffer, sizeof(help_buffer), help_file);

    return (ptr && *ptr != ':' && *ptr != '#' ? ptr : NULL);
}
