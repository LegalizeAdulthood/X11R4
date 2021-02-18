/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1988 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	March 1988.
 *
 *	%W%	%G%
*/
#include "fig.h"
#include "resources.h"
#include "func.h"
#include "object.h"

#define			PROMPT		1
#define			NO_PROMPT	0

extern int		figure_modified;
extern char		current_file[];
extern char		directory[];
extern int		num_object;

edit_file(file)
char	*file;
{
	extern F_compound	objects, saved_objects;
	int		s;
	F_compound	c;

	if (*file == 0) {
	    put_msg("Empty name");
	    return;
	    }
	c.arcs = NULL;
	c.compounds = NULL;
	c.ellipses = NULL;
	c.lines = NULL;
	c.splines = NULL;
	c.texts = NULL;
	c.next = NULL;
	set_temp_cursor(&wait_cursor);
	s = read_fig(file, &c);
	if (s == 0) {		/* Successful read */
	    clean_up();
	    (void)strcpy(current_file, file);
	    saved_objects = objects;
	    objects = c;
	    redisplay_canvas();
	    put_msg("Edit \"%s\" %d objects", file, num_object);
	    set_action(F_EDIT);
	    }
	else if (s == ENOENT) {
	    clean_up();
	    saved_objects = objects;
	    objects = c;
	    redisplay_canvas();
	    put_msg("\"%s\" new file", file);
	    (void)strcpy(current_file, file);
	    set_action(F_EDIT);
	    }
	else if (s > 0)
	    read_fail_message(file, s);
	reset_cursor();
	}

read_file(file)
char	*file;
{
	extern F_compound	objects, saved_objects, object_tails;
	int		s;
	F_compound	c;

	if (*file == 0) {
	    put_msg("Empty name");
	    return;
	    }

	c.arcs = NULL;
	c.compounds = NULL;
	c.ellipses = NULL;
	c.lines = NULL;
	c.splines = NULL;
	c.texts = NULL;
	c.next = NULL;
	set_temp_cursor(&wait_cursor);
	s = read_fig(file, &c);
	if (s == 0) {		/* Successful read */
	    clean_up();
	    saved_objects = c;
	    tail(&objects, &object_tails);
	    append_objects(&objects, &saved_objects, &object_tails);
	    pw_batch_on(canvas_pixwin);
	    redisplay_canvas();
	    pw_batch_off(canvas_pixwin);
	    put_msg("File \"%s\" %d objects", file, num_object);
	    set_action_object(F_CREATE, O_ALL_OBJECT);
	    }
	else if (s > 0)
	    read_fail_message(file, s);
	reset_cursor();
	}

save_and_exit(file)
char	*file;
{
	if (0 == write_file(file, PROMPT)) quit();
	}

save_current_file()
{
	return(write_file(current_file, NO_PROMPT));
	}

save_file(file)
char	*file;
{
	return(write_file(file, PROMPT));
	}

status()
{
	extern char	*getwd();

	if (*directory == NULL) {
	    if (NULL == getwd(directory)) {
		put_msg("%s", directory);	/* err msg is in directory */
		*directory = '\0';
		return;
		}
	    }

	if (*current_file == '\0')
	    put_msg("No file; directory \"%s\"", directory);
	else
	    put_msg("file \"%s\" %s; directory \"%s\"", current_file,
			(figure_modified ? "[modified]" : ""), directory);
	}
