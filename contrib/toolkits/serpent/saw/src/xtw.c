	static char rcsid [] = "$Header: /u/04c/mnt/integ/saw/src/RCS/xtw.c,v 1.6 89/09/03 15:47:18 rnk Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:
|	xtw.c
|
|  Description:
|	This is the implementation of the various routines needed to interface
|	with the text widget.
|
|  Component of:
|	SerpentX
|
|  Related files:
|	xtw.h
|
|  Project:
|	SERPENT System
|	User Interface Prototyping
|	Software Engineering Institute
|	Carnegie Mellon University
|	Pittsburgh, PA  15213
|
|  Version:
|	1.0
|
|  Owner:  
|	Len Bass
|	ljb@sei.cmu.edu
|
|  Authors:
|	Kurt W. Hoyt
|	kwh@sei.cmu.edu
|
|	Erik J. Hardy
|	erik@sei.cmu.edu
|
|  Change History:
$Log:	xtw.c,v $
 * Revision 1.6  89/09/03  15:47:18  rnk
 * changed boolean attributes to be true booleans (they were int before)
 * 
 * Revision 1.5  89/06/29  15:12:36  rnk
 * modified translation table to disable mouse events
 * 
 * Revision 1.4  89/06/27  22:24:16  rnk
 * removed code to blank out text buffer and added a call to XtTextSetLength
 * to change the length of the string displayed.  Also added code to create
 * a text buffer in any created text widget (if one has not already been
 * specified by the user).
 * 
 * Revision 1.3  89/06/13  12:49:00  rnk
 * fixed rewriting of text strings.  we now use XtTextReplace to modify
 * the XtNstring field
 * 
 * Revision 1.2  89/06/07  17:43:22  rnk
 * set XtNtop, XtNleft, XtNbottom and XtNright to override geometry management
 * 
 * Revision 1.1  89/04/26  10:17:33  rnk
 * Initial revision
 * 
 * Revision 1.4  89/02/23  13:22:45  ejh
 * font changing in the text widget works only on vaxes
 * 
 * Revision 1.3  89/02/17  03:43:33  ejh
 * added copyright notice; fixed x includes
 * 
 * Revision 1.2  89/01/26  16:40:03  ejh
 * fixed handling of text buffers to match semantics of buffers
 * 
 * Revision 1.1  88/11/14  16:13:10  ejh
 * Initial revision
 * 
|	 2 Nov 88	project standards imposed	erik
|	29 Sep 88	fixed the null parent problem	erik
|	 2 Jun 88	created		kwh
|
\*--------------------------------------------------------------------------*\
|
|   Distribution: Approved for public release; distribution is unlimited.
|
|   Copyright (c) 1989  by  Carnegie  Mellon  University, Pittsburgh, PA.  The
|  Software Engineering  Institute  (SEI) is a federally  funded  research and
|  development center established  and  operated by Carnegie Mellon University
|  (CMU).  Sponsored  by  the  U.S.  Department  of   Defense  under  contract
|  F19628-85-C-0003,  the  SEI  is  supported  by  the  services  and  defense
|  agencies, with the U.S. Air Force as the executive contracting agent.
|
|    Permission to use,  copy,  modify, or  distribute  this  software and its
|  documentation for  any purpose and without fee  is hereby granted, provided
|  that  the above copyright notice appear  in  all copies and that both  that
|  copyright  notice  and   this  permission  notice  appear   in   supporting
|  documentation.   Further,  the  names  Software  Engineering  Institute  or
|  Carnegie  Mellon  University  may  not be used in  advertising or publicity
|  pertaining to distribution of the software without specific, written  prior
|  permission.  CMU makes no claims  or representations  about the suitability
|  of this software for any purpose.  This software is provided "as is" and no
|  warranty,  express  or  implied,  is  made  by  the  SEI  or CMU, as to the
|  accuracy  and  functioning of the program and related program material, nor
|  shall   the  fact of  distribution   constitute  any   such  warranty.   No
|  responsibility is assumed by the SEI or CMU in connection herewith.
|
\*--------------------------------------------------------------------------*/

#define memoryPack

#include	<X11/Intrinsic.h>
#include	<X11/StringDefs.h>
#include	<X11/Cardinals.h>
#include	<X11/Shell.h>
#include        <X11/Form.h>
#include	<X11/AsciiText.h>
#include	<X11/Text.h>

#include	"x22.h"
#include	"xcp.h"
#include	"xfw.h"
#include	"xtw.h"
#include	"hashPack.h"
#include	"serpent.h"

#include	"xdb.h"

#define XtNmaximumLength	"maximumLength"

extern	XrmQuark	ParentQ;
extern	XrmQuark	ForegroundColorQ;
extern	XrmQuark	EditTypeQ;
extern	XrmQuark	FontQ;
extern	XrmQuark	EditableQ;
extern	XrmQuark	ResizeHeightQ;
extern	XrmQuark	ResizeWidthQ;
extern	XrmQuark	ScrollHorizontalQ;
extern	XrmQuark	ScrollOnOverflowQ;
extern	XrmQuark	ScrollVerticalQ;
extern	XrmQuark	WordBreakQ;
extern	XrmQuark	TextBufferQ;
extern	XrmQuark	MaxLengthQ;
extern	XrmQuark	SendBufferQ;
extern	XrmQuark	CallbackQ;

static	char	widget_name [] = "text_widget";

isd_change_type	trans_type ;  /* Kluge for XtNstring     */
Widget		text_widget;  /* Kluge for XtNtextBuffer */

#define	DEFAULT_MAX_LENGTH	1028

extern	Display	*display;

Boolean	send_buffer,
	text_has_changed,
	text_options_set;
int	text_options,
	xtw_max_length;

static String textTranslations =
"\
Ctrl<Key>F:	forward-character() \n\
Ctrl<Key>B:	backward-character() \n\
Ctrl<Key>D:	delete-next-character() \n\
Ctrl<Key>A:	beginning-of-line() \n\
Ctrl<Key>E:	end-of-line() \n\
Ctrl<Key>H:	delete-previous-character() \n\
Ctrl<Key>J:	newline-and-indent() \n\
Ctrl<Key>K:	kill-to-end-of-line() \n\
Ctrl<Key>L:	redraw-display() \n\
Ctrl<Key>M:	newline() \n\
Ctrl<Key>N:	next-line() \n\
Ctrl<Key>O:	newline-and-backup() \n\
Ctrl<Key>P:	previous-line() \n\
Ctrl<Key>V:	next-page() \n\
Ctrl<Key>W:	kill-selection() \n\
Ctrl<Key>Y:	unkill() \n\
Ctrl<Key>Z:	scroll-one-line-up() \n\
Meta<Key>F:	forward-word() \n\
Meta<Key>B:	backward-word() \n\
Meta<Key>I:	insert-file() \n\
Meta<Key>K:	kill-to-end-of-paragraph() \n\
Meta<Key>V:	previous-page() \n\
Meta<Key>Y:	stuff() \n\
Meta<Key>Z:	scroll-one-line-down() \n\
:Meta<Key>d:	delete-next-word() \n\
:Meta<Key>D:	kill-word() \n\
:Meta<Key>h:	delete-previous-word() \n\
:Meta<Key>H:	backward-kill-word() \n\
:Meta<Key>\\<:	beginning-of-file() \n\
:Meta<Key>\\>:	end-of-file() \n\
:Meta<Key>]:	forward-paragraph() \n\
:Meta<Key>[:	backward-paragraph() \n\
~Shift Meta<Key>Delete:		delete-previous-word() \n\
 Shift Meta<Key>Delete:		backward-kill-word() \n\
~Shift Meta<Key>Backspace:	delete-previous-word() \n\
 Shift Meta<Key>Backspace:	backward-kill-word() \n\
<Key>Right:	forward-character() \n\
<Key>Left:	backward-character() \n\
<Key>Down:	next-line() \n\
<Key>Up:	previous-line() \n\
<Key>Delete:	delete-previous-character() \n\
<Key>BackSpace:	delete-previous-character() \n\
<Key>Linefeed:	newline-and-indent() \n\
<Key>Return:	newline() \n\
<Key>:		insert-char() \n\
<FocusIn>:	focus-in() \n\
<FocusOut>:	focus-out()" ;

/*--------------------------------------------------------------------------*\
|  Name: xtw_fill_table
|
|  Description:
|	xtw_fill_table inserts into a hash table with the nodes necessary to
|	match a text part component name, which is a string, with the actual
|	data for the component.
|
|  Parameters:
|	node		The xht_record_node in which to insert the data.
|	widget_name	The name of the widget whose table is being filled.
|
|  Returns: Modifies node.
\*--------------------------------------------------------------------------*/

void	xtw_fill_table (node, widget_name)
xht_record_node	*node;
char		*widget_name;

    {
    xht_comp_node	*temp;

    if (xtw_debug > 1)
	{
	fprintf (stderr, "xtw_fill_table: Entering.\n");
	fprintf (stderr, "xtw_fill_table: table = %x\n", node->comp_table);
	fprintf (stderr, "xtw_fill_table: widget_name = %s\n", widget_name);
	}

/*	Add the record component names and component addresses to the table */

    temp = xht_make_comp_node (widget_name, "foreground_color",XtNforeground);
    ForegroundColorQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "edit_type", XtNeditType);
    EditTypeQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "font", XtNfont);
    FontQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "editable", XtNtextOptions);
    EditableQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "resize_height", XtNtextOptions);
    ResizeHeightQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "resize_width", XtNtextOptions);
    ResizeWidthQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "scroll_horizontal",
	XtNtextOptions);
    ScrollHorizontalQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "scroll_on_overflow",
	XtNtextOptions);
    ScrollOnOverflowQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "scroll_vertical",XtNtextOptions);
    ScrollVerticalQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "word_break", XtNtextOptions);
    WordBreakQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "text_buffer", XtNstring);
    TextBufferQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "max_length", XtNmaximumLength);
    MaxLengthQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "method", XtNcallback);
    CallbackQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    temp = xht_make_comp_node (widget_name, "send_buffer", XtNsendBuffer);
    SendBufferQ = temp->quark;
    add_to_hashtable (node->comp_table, temp, temp->name);

    if (xtw_debug > 1)
	fprintf (stderr, "xtw_fill_table: Leaving.\n");

    return;
    }

/*--------------------------------------------------------------------------*\
|  Name: xtw_make_table
|
|  Description:
|	xtw_make_table creates a hash table with the nodes necessary to match
|	a text widget component name, which is a string, with the actual data
|	for the component.
|
|  Parameters: None.
|
|  Returns:	A record suitable for insertion into the toplevel hash table
|		for all widgets and objects.
\*--------------------------------------------------------------------------*/

xht_record_node	*xtw_make_table ()

    {
    xht_record_node	*temp;
    HASH		record_hash;

    if (xtw_debug > 1)
	fprintf (stderr, "xtw_make_table: Entering.\n");

    record_hash = make_hashtable (HASH_SIZE, xht_hash_string,
	xht_match_component_name);

    if (xtw_debug > 2)
	{
	fprintf (stderr, "xtw_make_table: record_hash = %x.\n", record_hash);
	fprintf (stderr, "xtw_make_table: Adding record table data.\n");
	}

    temp = xht_make_record_node (widget_name, (char *) record_hash,
	(String) asciiStringWidgetClass);

    xcp_fill_table (temp, widget_name);
    xfc_fill_table (temp, widget_name);
    xtw_fill_table (temp, widget_name);

    if (xtw_debug > 1)
	{
	fprintf (stderr, "xtw_make_table: node = %x.\n", temp);
	fprintf (stderr, "xtw_make_table: Leaving.\n");
	}

    return (temp);
    }

/*--------------------------------------------------------------------------*\
|  Name: xtw_process
|
|  Description:
|	xtw_process generates the argument list values for the text widget.
|
|  Parameters:
|	node		the node retrived for the SD component.
|	data		the element values from the SD.
|	args		the X Toolkit argument list.
|	num_args	the length of args.
|
|  Returns:
|	args is modified to reflect the new value inserted.
|	num_args is incremented.
\*--------------------------------------------------------------------------*/

Boolean	xtw_process (node, data, args, num_args)
xht_comp_node	*node;
caddr_t		data;
ArgList		args;     /* MODIFIED */
Cardinal	*num_args; /* MODIFIED */

{
    char		*data_ptr ;
    Pixel		color ;
    XFontStruct		*font ;
    caddr_t		temp_body ;
    XtTextBlock 	text_block ;
    char		*text_address ;
    Arg 		local_args[5] ;
    int			n, i, length ;

    if (xtw_debug > 1)
    {
	fprintf (stderr, "xtw_process: Entering.\n");
	fprintf (stderr, "xtw_process: node = %x.\n", node);
	fprintf (stderr, "xtw_process: args = %x.\n", args);
	fprintf (stderr, "xtw_process: num_args = %d.\n", *num_args);
    }

    data_ptr = data + node->offset;

    if (xtw_debug > 2)
    {
	fprintf (stderr, "xtw_process: Address of data = %x.\n", data_ptr);
	fprintf (stderr, "xtw_process: Attribute name = %s.\n",
	    node->xt_arg_value);
    }

    if (node->quark == ForegroundColorQ)
    {
	if (xtw_debug > 2)
	    fprintf (stderr, "xtw_process: string = %s\n", data_ptr);

	color = xct_get_pixel (data_ptr);

	XtSetArg (args [*num_args], node->xt_arg_value, color);
	(*num_args)++;

	if (xtw_debug > 1)
	   fprintf (stderr, "xtw_process: num_args = %d.\n", *num_args);

	return (True);
    }

    if (node->quark == FontQ)
    {
	if (xtw_debug > 2)
	    fprintf (stderr, "xtw_process: string = %s\n", data_ptr);

	if ((font = XLoadQueryFont (display, data_ptr)) != NULL) 
	{
	    XtSetArg (args [*num_args], node->xt_arg_value, font);
	    (*num_args)++;
	} else 
	    fprintf(stderr, "xtw_process: font not found--using default\n") ;

	return (True);
    }

    if (node->quark == MaxLengthQ)
    {
	if (xtw_debug > 2)
	    fprintf (stderr, "xtw_process: max length = %d\n",
		*((int *) data_ptr));

	xtw_max_length = *((int *) data_ptr);

	if (xtw_max_length > DEFAULT_MAX_LENGTH)
	    fprintf (stderr, "xtw_process: max length %d too big\n") ;

	return (True);
    }

    if (node->quark == TextBufferQ)
    {
	if (xtw_debug > 2)
	    fprintf (stderr, "xtw_process: text string=%s\n", data_ptr);

	/* Don't do a setarg unless this is a create */
	if (trans_type == isd_create)
	{   
	    temp_body = (caddr_t) make_node (xtw_max_length) ;
	    strcpy ((char *) temp_body, data_ptr) ;

	    XtSetArg(args[*num_args], XtNlength, xtw_max_length); 
	    (*num_args)++;
	    XtSetArg (args[*num_args], XtNstring, temp_body);
	    (*num_args)++; 

	} else if (trans_type == isd_modify)
	{   
	    /* Insert the new string */
	    length = strlen(data_ptr) ;
	    text_block.firstPos = 0 ;
	    text_block.length = length ;
	    text_block.ptr = data_ptr ;
	    text_block.format = FMT8BIT ;
	    XtTextReplace(text_widget, 0, length, &text_block) ;
	    XtTextSetLastPos(text_widget, length) ;

	    text_has_changed = TRUE ;
	} 
	else fprintf(stderr,"xtw_process:  invalid transaction type\n") ;

	if (xtw_debug > 1)
	    fprintf (stderr, "xtw_process: num_args = %d.\n", *num_args);

	return (True);
    }

    if (node->quark == EditTypeQ)
    {
	if (xtw_debug > 2)
	    fprintf (stderr, "xtw_process: integer = %d\n",
		*((int *) data_ptr));

	XtSetArg (args [*num_args], node->xt_arg_value, *((int *) data_ptr));
	(*num_args)++;

	if (xtw_debug > 1)
	    fprintf (stderr, "xtw_process: num_args = %d.\n", *num_args);

	return (True);
    }

    if (node->quark == EditableQ)
    {
	if (xtw_debug > 2)
	    fprintf (stderr, "xtw_process: editable\n");

	if (*((boolean *) data_ptr)) text_options |= editable;
	text_options_set = True;

	return (True);
    }

    if (node->quark == ResizeHeightQ)
    {
	if (xtw_debug > 2)
	    fprintf (stderr, "xtw_process: resizeHeight\n");

	if (*((boolean *) data_ptr)) text_options |= resizeHeight;
	text_options_set = True;

	return (True);
    }

    if (node->quark == ResizeWidthQ)
    {
	if (xtw_debug > 2)
	    fprintf (stderr, "xtw_process: resizeWidth\n");

	if (*((boolean *) data_ptr)) text_options |= resizeWidth;
	text_options_set = True;

	return (True);
    }

    if (node->quark == ScrollHorizontalQ)
    {
	if (xtw_debug > 2)
	    fprintf (stderr, "xtw_process: scrollHorizontal\n");

	if (*((boolean *) data_ptr)) text_options |= scrollHorizontal;
	text_options_set = True;

	return (True);
    }

    if (node->quark == ScrollOnOverflowQ)
    {
	if (xtw_debug > 2)
	    fprintf (stderr, "xtw_process: scrollOnOverflow\n");

	if (*((boolean *) data_ptr)) text_options |= scrollOnOverflow;
	text_options_set = True;

	return (True);
    }

    if (node->quark == ScrollVerticalQ)
    {
	if (xtw_debug > 2)
	    fprintf (stderr, "xtw_process: scrollVertical\n");

	if (*((boolean *) data_ptr))
	    text_options |= scrollVertical;
	text_options_set = True;

	return (True);
    }

    if (node->quark == WordBreakQ)
    {
	if (xtw_debug > 2)
	    fprintf (stderr, "xtw_process: wordBreak\n");

	if (*((boolean *) data_ptr)) text_options &= wordBreak;
	text_options_set = True;

	return (True);
    }

    if (node->quark == SendBufferQ)
    {
	if (xtw_debug > 2)
	    fprintf (stderr, "xtw_process: send_string\n");

	send_buffer = *((boolean *) data_ptr) ;

	return (True);
    }

    return (False);
}

/*--------------------------------------------------------------------------*\
|  Name: xtw_send_buffer
|
|  Description:
|	xtw_send_buffer sends a buffer back to the dialogue manager.
|
|  Parameters:
|	widget	The widget from which to get the string.
|
|  Returns: Nothing.
\*--------------------------------------------------------------------------*/

void	xtw_send_buffer (widget)
Widget	widget;

{
    iid_id_type		widget_id;
    caddr_t		text_buffer;
    char		*text_address;
    static	Arg	buf_args [1];
    int			*nbytes, text_length ;

    if (xtw_debug > 1)
    {
	fprintf (stderr, "xtw_send_buffer: Entering.\n");
	fprintf (stderr, "xtw_send_buffer: widget = %x.\n", widget);
    }
    text_buffer = make_node(DEFAULT_MAX_LENGTH) ;
    XtSetArg (buf_args[0], XtNstring, &text_address);
    XtGetValues (widget, buf_args, ONE);

    strcpy(text_buffer, text_address);
    text_length = strlen(text_buffer) ;

    if (xtw_debug > 2)
	fprintf (stderr, "xtw_send_buffer: length = %d, body = %s.\n",
	    text_length, text_buffer);

    widget_id = xht_get_dm_id (widget);

    if (xtw_debug > 2)
    {
	fprintf (stderr, "xtw_send_buffer: widget_id = %d.\n", widget_id);
	fprintf (stderr, "xtw_send_buffer: Sending to dm.\n");
    }

    if (!x22_transaction_started)
    {
	x22_trans = isd_start_transaction();
	x22_transaction_started = TRUE;
    }

    isd_put_shared_data (x22_trans, widget_id, "text_widget", "method",
	"send");
    isd_put_shared_data (x22_trans, widget_id, "text_widget", "text_buffer",
	text_buffer);

    if (xtw_debug > 2)
	fprintf (stderr, "xtw_send_buffer: buffer into trans.\n");

    if (xtw_debug > 1)
	fprintf (stderr, "xtw_send_buffer: Leaving.\n");
}

/*--------------------------------------------------------------------------*\
|  Name: xtw_process_changes
|
|  Description:
|	xtw_process_changes processes text_widget records from the dialogue
|	manager.
|
|  Parameters:
|	change		The change type from the dialogue manager.
|
|	transaction	The transaction that was sent.
|
|	id		The id of the record in shared data to process.
|
|  Returns: Nothing.
\*--------------------------------------------------------------------------*/

void	xtw_process_changes (change, transaction, id)
isd_change_type	change;
isd_trans	transaction;
iid_id_type	id;

{
    Widget	parent,
		widget;
    int		record_size,
		num_args;
    char	*data,
		*data_ptr,
		*value_string;
    string	element_name,
		component_name;
    LIST	change_list;
    Arg		args[50];
    caddr_t	temp_body ;
    xht_comp_node	*node;
    boolean	parent_is_default = FALSE;
    boolean	has_buffer = FALSE;

    if (xtw_debug > 1)
    {
	fprintf (stderr, "xtw_process_changes: Entering.\n");
	fprintf (stderr, "xtw_process_changes: change = %d\n", change);
	fprintf (stderr, "xtw_process_changes: transaction = %d\n",
	    transaction);
	fprintf (stderr, "xtw_process_changes: id = %d\n", id);
    }

    trans_type = change ; /* Kluge for XtNstring */
    switch (change)
    {

	case isd_create:

	    element_name = isd_get_element_name (transaction, id);
	    record_size = isd_get_length (element_name);
	    data = make_node (record_size);
	    xtw_max_length = DEFAULT_MAX_LENGTH;

	    isd_incorporate_changes (transaction, id, data);

	    if (xtw_debug > 2)
		fprintf (stderr,
		   "xtw_process_changes: Changes incorporated.\n");

	    change_list = isd_create_changed_component_list (transaction, id);

	    num_args = 0;
	    component_name = get_list_next (change_list, NULL);
	    text_options = 0;
	    text_options_set = False;

	    node = xht_get_node (element_name, component_name);

	    if (node->quark == ParentQ)
	    {
		data_ptr = data + node->offset;
		if (*((int *) data_ptr) == iid_null_id)
		    parent_is_default = TRUE;
		else
		    parent = (Widget) xht_get_item
			(*((iid_id_type *) data_ptr));
		component_name = get_list_next (change_list, component_name);
	    }
	    else
		parent_is_default = TRUE;

	    while (component_name != NULL)
	    {
		node = xht_get_node (element_name, component_name);
		if (node->quark == TextBufferQ) has_buffer = TRUE ;

		if (xcp_process (node, data, args, &num_args)) {}
		else if (xfc_process (node, data, args, &num_args)) {}
		else if (xtw_process (node, data, args, &num_args)) {}

		component_name = get_list_next (change_list, component_name);
	    }

	    if (text_options_set)
	    {
		XtSetArg (args[num_args], XtNtextOptions, text_options);
		num_args++;
	    }

            /* override geometry management */

            XtSetArg(args[num_args], XtNtop, XtChainTop); num_args++;
            XtSetArg(args[num_args], XtNleft, XtChainLeft); num_args++;
            XtSetArg(args[num_args], XtNbottom, XtChainTop); num_args++;
            XtSetArg(args[num_args], XtNright, XtChainLeft); num_args++;

            /* add translation tables.  */
            XtSetArg(args[num_args], XtNtranslations,
                XtParseTranslationTable(textTranslations)) ; num_args++;

	    /* if the widget has no text buffer, assign one */
	    if (!has_buffer) 
	    {
		temp_body = (caddr_t) make_node(xtw_max_length) ;
		XtSetArg(args[num_args], XtNlength, xtw_max_length); 
		num_args++;
		XtSetArg (args[num_args], XtNstring, temp_body);
		num_args++; 
	    }

	    if (xtw_debug > 2) fprintf (stderr, 
		"xtw_process_changes: num_args = %d\n", num_args);

	    if (parent_is_default) parent = x22_build_default_parent ();

	    widget = XtCreateManagedWidget (element_name,
		asciiStringWidgetClass, parent, args, num_args);
	    xht_item_id_mapping (id, widget);

	    if (parent_is_default) x22_add_parent (widget, parent);

	    isd_destroy_changed_component_list (change_list);
	    free_node (data);

	    break;

	case isd_modify:

	    element_name = isd_get_element_name (transaction, id);
	    record_size = isd_get_length (element_name);
	    data = make_node (record_size);

	    isd_incorporate_changes (transaction, id, data);

	    widget = (Widget) xht_get_item (id);
	    text_widget = widget ;

	    if (xtw_debug > 2)
		fprintf (stderr,
		    "xtw_process_changes: Changes incorporated.\n");

	    change_list = isd_create_changed_component_list (transaction, id);

	    send_buffer = False;
	    text_has_changed = False;
	    num_args = 0;
	    component_name = get_list_next (change_list, NULL);
	    text_options = 0;
	    text_options_set = False;

	    while (component_name != NULL)
	    {
		node = xht_get_node (element_name, component_name);

		if (xcp_process (node, data, args, &num_args)) {}
		else if (xfc_process (node, data, args, &num_args)) {}
		else if (xtw_process (node, data, args, &num_args)) {}

		component_name = get_list_next (change_list, component_name);
	    }

	    if (text_options_set)
	    {
		XtSetArg (args[num_args], XtNtextOptions, text_options);
		num_args++;
	    }

	    if (xtw_debug > 2)
		fprintf (stderr, "xtw_process_changes: num_args = %d\n",
		    num_args);

	    XtSetValues (widget, args, num_args);
 
	    if (send_buffer)
		xtw_send_buffer (widget);

	    /* redraw the widget if the text has been changed */
            if (text_has_changed)
		XClearArea (XtDisplay(widget), XtWindow(widget), 
		    0, 0, 0, 0, TRUE) ;

	    isd_destroy_changed_component_list (change_list);
	    free_node (data);

	    break;

	case isd_remove:

	    if (widget = (Widget) xht_get_item (id))
	    {
		xht_delete_item (id);
		x22_delete_parent (widget);
	    }

	    break;

	default:

	    break;
    }

    if (xtw_debug > 1)
	fprintf (stderr, "xtw_process_changes: Exiting.\n");
}
