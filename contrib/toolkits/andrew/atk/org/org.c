/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/org/RCS/org.c,v 1.6 89/08/31 17:12:55 tom Exp $";
#endif

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Org Data-Class

MODULE	org.c

VERSION	1.0

AUTHOR	TC Peters
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Org Data-Class.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  01/19/89	Created (TCP)
  08/23/89	Remove Create method (TCP)
  08/24/89	Upgrade to Version 1.0 (TCP)
  08/31/89	Change OfData to OfDatum (TCP)

END-SPECIFICATION  ************************************************************/

#include <andrewos.h>
#include <dataobj.ih>
#include <org.eh>
#include <tree.ih>
#include <ctype.h>
#include <apt.h>

#define  Tree			    (self->tree_data_object)
#define  OrgFileName		    (self->tree_file_name)

tree_Specification		    specification[] =
  {
  tree_Order( tree_PreOrder ),
  NULL
  };


char *
org__ViewName( self )
  register struct org		     *self;
  {
  return ( "orgv" );
  }

boolean 
org__InitializeObject( classID, self )
  register struct classheader	     *classID;
  register struct org		     *self;
  {
  register boolean		      status = true;

  IN(org_InitializeObject);
  DEBUGst(RCSID,rcsid);
  if ( (Tree = tree_Create( specification, self )) == NULL )
    {
    printf( "ORGVIEW: Unable to Create AptTree Object\n" );
    status = false;
    }
    else  tree_SetDebug( Tree, debug );
  OUT(org_InitializeObject);
  return  status;
  }

static
Free_Elements( self, tree, node, datum )
  register struct org		     *self;
  register struct tree		     *tree;
  register tree_type_node	      node;
  {
  if ( tree_NodeDatum( tree, node ) )
    free( tree_NodeDatum( tree, node ) );
  return  NULL;
  }

void
org__FinalizeObject( classID, self )
  register struct classheader	    *classID;
  register struct org		    *self;
  {
  IN(org_FinalizeObject );
  if ( OrgFileName )  free( OrgFileName );
  if ( Tree )
    {
    tree_Apply( Tree, tree_RootNode( Tree ), Free_Elements, self, NULL );
    tree_Destroy( Tree );
    }
  OUT(org_FinalizeObject );
  }

boolean
org__SetFileName( self, name )
  register struct org		     *self;
  register char			     *name;
  {
  register long			      status = ok;

  IN(org_SetFileName);
  if ( OrgFileName )  free( OrgFileName );
  OrgFileName = NULL;
  if ( name  &&  *name )
    if ( OrgFileName = (char *) malloc( strlen( name ) + 1 ))
      strcpy( OrgFileName, name );
      else  status = failure;
  OUT(org_SetFileName);
  return  status;
  }

long
org__Read( self, file, id )
  register struct org		     *self;
  register FILE			     *file;
  register long			      id;
  {
  register long			      status;

  IN(org_Read);
  status = Read_Body( self, file );
  OUT(org_Read);
  return status;
  }

static
Read_Body( self, file )
  register struct org		     *self;
  register FILE			     *file;
  {
  register boolean		      done = false;
  register long			      c, count, braces = 0, brackets = 0, status = ok,
				      description_size, description_length,
				      description_increment = 32;
  char				      string[4096], *ptr, *end_ptr,
				     *count_ptr, counter[32];
  register char			     *description_ptr = NULL, *description;
  register tree_type_node	      parent = NULL, child = NULL, node;

  IN(Read_Body);
  ptr = string;
  end_ptr = ptr + sizeof(string) - 2;
  while ( !done  &&  (c = getc( file )) )
    {
    switch ( c )
      {
      case '\n':    DEBUG(NewLine);
	if ( ptr > string )
	  {
	  Strip( ptr = string );
	  if ( parent )
	    {
	    if ( (child = node = tree_CreateChildNode( Tree, string, NULL, parent )) == NULL )
	      { DEBUG(ERROR Creating Node);/*===*/      }
	    }
	    else
	    {
	    if ( (parent = node = tree_CreateRootNode( Tree, string, NULL )) == NULL )
	      { DEBUG(ERROR Creating RootNode);/*===*/  }
	    }
	  }
        break;
      case '{':	    DEBUG(LeftBrace);
	braces++;
	if ( child )  parent = child;
	DEBUGst(Parent,tree_NodeName(Tree,parent));
        break;
      case '}':	    DEBUG(RightBrace);
	braces--;
	if ( ptr > string )
	  {
	  Strip( ptr = string );
	  if ( parent )
	    {
	    if ( (child = node = tree_CreateChildNode( Tree, string, NULL, parent )) == NULL )
	      { DEBUG(ERROR Creating Node);/*===*/      }
	    }
	    else {/*===*/}
	  }
	  else
	  {
	  child = parent;
	  parent = tree_ParentNode( Tree, parent );
	  }
	DEBUGst(Parent,tree_NodeName(Tree,parent));
        break;
      case '[':	    DEBUG(LeftBracket);
	DEBUGst(Node,tree_NodeName(Tree,node));
	brackets++;
	count_ptr = counter;
	while ( (c = getc( file ))  &&  c != EOF  && c != '\n' )
	  *count_ptr++ = c;
	*count_ptr = 0;
	count = atoi( counter );
	DEBUGdt(Count,count);
	description_size = 32;
	description_length = 0;
	description_ptr = description = (char *) malloc( description_size );
	tree_SetNodeDatum( Tree, node, (long)description_ptr );
	while ( (c = getc( file ))  &&  --count  &&  c != EOF )
	  {
	  *description_ptr++ = c;
	  description_length++;
	  if ( description_length == (description_size-1) )
	    {
	    *description_ptr = 0;
	    DEBUGst(Description,tree_NodeDatum( Tree, node ));
	    description_size += description_increment;
	    description_ptr = description = (char *) realloc( description, description_size );
	    tree_SetNodeDatum( Tree, node, (long)description );
	    description_ptr += description_length;
	    }
	  }
	*description_ptr = 0;
	DEBUGst(Description,tree_NodeDatum( Tree, node ));
	break;
      case ']':	    DEBUG(RightBracket);
	brackets--;
	break;
      case '\\':    DEBUG(EndData);
	while ( (c = getc( file )) != '\n'  &&  c != EOF ) ;
	done = true;
	break;
      case EOF:	    DEBUG(EOF);
	done = true;
        break;
      default:
	if ( ptr < end_ptr )
	  {
	  if ( (ptr > string)  ||  (c != ' '  &&  c != '\t') )
	    {  *ptr++ = c;  *ptr = 0;  }
	  }
	  else
	  { DEBUG(ERROR: exceeded string);/*===*/ }
        break;
      }
    }
  if ( braces )
    {
    status = failure;
/*===*/printf("ORG: ERROR  %d Unbalanced Braces\n", braces);
    }
  if ( brackets )
    {
    status = failure;
/*===*/printf("ORG: ERROR  %d Unbalanced Brackets\n", brackets);
    }
/*===*/
  OUT(Read_Body);
  return  status;
  }

long
org__Write( self, file, writeID, level )
  register struct org		     *self;
  register FILE			     *file;
  register long			      writeID;
  register long			      level;
  {
  register long			      status, id;

  IN(org_Write);
  DEBUGdt(Headerwriteid,self->header.dataobject.writeID);
  DEBUGdt(Given Id,writeID);
  DEBUGdt(Given Level,level);
  id = org_UniqueID( self );
  DEBUGdt(Local-ID,id);
  if ( self->header.dataobject.writeID != writeID )
    {
    self->header.dataobject.writeID = writeID;
    if ( level )
      { DEBUG(Not Parent -- Write To Datastream);
      fprintf( file, "\\begindata{%s,%d}\n", class_GetTypeName( self ), id );
      status = Write_Body( self, file );
      fprintf( file, "\n\\enddata{%s,%d}\n", class_GetTypeName( self ), id );
      }
      else
      { DEBUG(Parent -- Write To Raw File);
      status = Write_Body( self, file );
      DEBUGdt(Enparse_Stream Status,status);
      }
    }
  DEBUGdt(Status,status);
  OUT(org_Write);
  return  self->header.dataobject.id;
  }

static
Write_Body( self, file )
  register struct org		     *self;
  register FILE			     *file;
  {
  register long			      status = ok;
  register tree_type_node	      node = tree_RootNode( Tree );
  register long			      level, current_level = 1;

  IN(Write_Body);
  while ( node )
    {
    if ( (level = tree_NodeLevel( Tree, node )) > current_level )
      fprintf( file, "%*s{\n", 2 * level, "" );
      else if ( level < current_level )
	for ( ; current_level > level; current_level-- )
          fprintf( file, "%*s}\n", 2 * current_level, "" );
    current_level = level;
    fprintf( file, "%*s%s\n", 2 * level, "", tree_NodeName( Tree, node ) );
    if ( tree_NodeDatum( Tree, node ) )
      {
      DEBUGst(Description,tree_NodeDatum( Tree, node ));
      fprintf( file, "%*s[%d\n%s]\n", 2 * level, "",
		strlen(tree_NodeDatum( Tree, node )),
		tree_NodeDatum( Tree, node ) );
      }
    node = tree_NextNode( Tree, node );
    }
  for ( ; current_level > 1; current_level-- )
    fprintf( file, "%*s}\n", 2 * current_level, "" );
  OUT(Write_Body);
  return  status;
  }

char *
org__NodeName( self, node )
  register struct org		      *self;
  register struct tree_node	      *node;
  {
  register char			      *name = NULL;

  IN(org_NodeName);
  if ( node )
    name = tree_NodeName( Tree, node );
  OUT(org_NodeName);
  return  name;
  }

void
org__SetDebug( self, state )
  register struct org		      *self;
  register char			       state;
  {
  IN(org_SetDebug);
  debug = state;
  tree_SetDebug( Tree, debug );
  OUT(org_SetDebug);
  }

static
Strip( string )
  register char			     *string;
  {
  register char			     *ptr = string;

  while ( *ptr == ' '  ||  *ptr == '\t' )  ptr++;
  strcpy( string, ptr );
  while ( *ptr )  ptr++;
  ptr--;
  while ( *ptr == ' '  ||  *ptr == '\t' ) *ptr-- = 0;
  DEBUGst(String,string);
  }
