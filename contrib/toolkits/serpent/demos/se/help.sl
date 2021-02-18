/*--------------------------------------------------------------------------*\
|   
|  Name:  Help Information (help)
|
|  Description:  
|     This file contains all the help information used throught the 
|     Serpent editor.  The VC's which control the display of this 
|     information are based on globally defined variables.  This
|     allows access to the same help information from different 
|     parts of the dialogue as well as allowing multiple help
|     messages to be displayed simultaneously.
|
|  Component of:
|     Serpent editor dialogue layer.
|
|  Related files:
|     ve.slang - contains the source for the visulization editor.
|
|  Project:
|     Serpent
|     User Interface Project
|     Software Engineering Institute
|     Carnegie Mellon University
|     Pittsburgh, PA  15213
|
|  Version: 1.0
|
|  Project Leader:  
|     Len Bass
|     ljb@sei.cmu.edu
|
|  Author:  
|     Robert C. Seacord
|     rcs@sei.cmu.edu  
|
|  Change History:
|     March 9, 1989 : Module created.
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

/*-------------------- Associate Command Help ------------------------------*/

  help_template(associate_cmd_help,
    "Associate Command Help",
    "The associate command is used to associate one or more unassociated\ninteraction object instance with the current object template.\n\nPerform one of the following actions:\n\n\t1. To associate one or more unassociated interaction object\n\t   instance with the current object template, select the\n\t   'Associate' display button on the 'Associate Prototyping\n\t   Command' form.  A new form will appear containing a\n\t   list of all the unassociated interaction objects.\n\t   Select any number of these interaction objects and\n\t   then select the 'OK' display button on this form.\n\n\t2. To associate one or more unassociated interaction object\n\t   instance with a different object template, either select\n\t   the object template or enter the name of the template in\n\t   the space provided on the 'Associate Prototyping Command'\n\t   form.  Select the 'Associate' display button.  Select any\n\t   number of these interaction objects and then select the\n\t   'OK' display button.\n\n\t3. To abort this operation, press the 'Cancel' display button.",
    300
  );

/*-------------------- Instantiate Command Help ----------------------------*/

  help_template(instantiate_cmd_help,
    "Instantiate Command Help",
    "Perform one of the following actions:\n\n\t1.  To instantiate the current object, select the 'Instantiate'\n\t    display button on the Instantiate Prototyping Command form.\n\n\t2.  To instantiate a different object, identify the current\n\t    object and select the 'Instantiate' display button.\n\n\t	3.  To abort this operation, press the 'Cancel' display button\n\t    Instantiate Prototyping Command form.",
    120
  );

/*-------------------- Command Widget Help ---------------------------------*/

  help_template(command_widget_help,
    "Command Widget Help",
    "The command button widget is a rectangle that contains a text\nlabel.  When the pointer cursor is on the button, its border\nis highlighted to indicate that the button is available for\nselection.  When a pointer button is clicked, the button is\nselected, and the corresponding 'notify' method is executed\nin the dialogue.",
    100
  );

/*--------------------------------------------------------------------------*\
|   Slang language help.
\*--------------------------------------------------------------------------*/

/*-------------------- Comments Help ---------------------------------------*/

  help_template(comments_help,
    "Comments Help",
"Description:\n\nComments are nice.",
    180
  );

/*-------------------- Prologue Help ---------------------------------------*/

  help_template(prologue_help,
    "Prologue Help",
"Description:\n\nPrologues are nice.",
    180
  );

/*-------------------- Variables Help --------------------------------------*/

  help_template(variables_help,
    "Variables Help",
"Description:\n\nVariables are nice.",
    180
  );

/*-------------------- Initialization Help ---------------------------------*/

  help_template(initialization_help,
    "Initialization Help",
"Description:\n\nInitialization are nice.",
    180
  );

/*-------------------- Termination Help ---------------------------------*/

  help_template(termination_help,
    "Termination Help",
"Description:\n\nTermination are nice.",
    180
  );

/*-------------------- Parent Attribute Help -------------------------------*/

  help_template(parent_attribute_help,
    "Parent Attribute Help",
"Description:\n\nThis attribute defines the widget which will act as parent for the\ninteraction object in which this attribute is specified.  If a\nparent is provided the widget will appear at the positions\ndesignated by 'horiz_distance' and 'vert_distance' from the upper\nleft hand corner of the parent widget.  If no parent is provided,\nthe end user will be prompted for the location of the interaction\nobject.\n\nDefault:  NULL",
    180
  );
