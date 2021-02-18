/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 */ 
#include <stdio.h>
#include <string.h>

typedef struct FontStruct Font;

struct FontStruct {
  char *name;
  unsigned int scale;
  unsigned int extension;
  Font *next;
};

unsigned int Dpi = 85;
unsigned int AndMag = 1440;
char *Mode = "sun";

int	DviBlackness = -1;
int	DviMag = 1000;
int	DviDsz = 1000;
char	*ProgName;
int	ShowSnf = 0;
int	UserMag = 1000;

long Numerator = 25400000;
long Denomonator = 473628272;

#define TWOTO16	0x10000
#define TWOTO24 16777216
#define PTPI 72.27

Font *FontList = 0;

static
int suffix(scale, usermag)
int scale;
int usermag;
{
  double foo;
  double dsuffix;
#define	ROUND(f) ((int) ((f) < 0.0 ? (f) - 0.5 : (f) + 0.5))
  int isuffix;

/*    fprintf(stdout,  "IN: mag = %d, scale = %d, Dpi = %d\n",
	    usermag, scale, Dpi); */

    foo = scale * usermag;
    dsuffix = Dpi * ROUND(foo * 65536);
    dsuffix /= 65536;

    isuffix = ROUND(dsuffix);

/*    fprintf(stdout,  "OUT: mag = %d, scale = %d, dsuffix = %f, isuffix = %d\n",
	    mag2, scale2, dsuffix,isuffix); */

    return(isuffix);
}

void
AddFont(name, scale, usermag)
char *name;
int scale;
int usermag;
{
  if ( name ) {
    Font *font = (Font *) malloc( sizeof(Font) );
    int len = strlen(name)+1;

    font -> name = (char *) malloc( len+1 );
    strncpy(font -> name, name, len);
    font -> scale = scale;
    font -> extension = suffix(scale, usermag);
    font -> next = FontList;
    FontList = font;
  }
}

int
InFontList(name,scale,mag)
char *name;
int scale;
int mag;
{
  Font *p = FontList;
  int extension = suffix(scale,mag);
  while ( p ) {
    if ( p -> extension == extension && strcmp(p -> name, name) == 0 ) {
      return(1);
    }
    p = p -> next;
  }
  return(0);
}

main(argc, argv)
int argc;
char **argv;
{
  int arg;
  for (arg = 1; arg < argc; arg++ ) {
    if ( strcmp(argv[arg],"-dpi") == 0 ) {
      arg++;
      Dpi = atoi(argv[arg]);
    }
    else if (strcmp(argv[arg],"-andMag") == 0 ) {
      arg++;
      AndMag = atoi(argv[arg]);
    }
  }
  
  /* build the list of fonts */
  
  while (! feof(stdin) ) {
    static char fontName[128];
    static char trash[128];
    int scale;
    int number;

    bzero(fontName,128);
    bzero(trash,128);
    number = fscanf(stdin, " %s %s %d\n",
			fontName, trash, &scale);

    if ( strcmp(trash,"scaled") != 0 ) {
      fprintf(stderr,"Bogus line: got %s %s %d\n", fontName, trash, scale);
    }

    if ( number == 3 ) {
      if ( ! InFontList(fontName, scale, 1000) ) {
	AddFont(fontName, scale, 1000);
      }
      if ( ! InFontList(fontName, scale, AndMag) ) {
	AddFont(fontName, scale, AndMag);
      }
    }
    else {
      fprintf(stderr,"Wrong number(%d): got %s %s %d\n",
	      number, fontName, trash, scale);
    }
  }
  
  fprintf(stdout,".SUFFIXES: .snf .bdf .pk .gf .mf\n\n");
  fprintf(stdout,"goals: snf-fonts\n\techo Done\n");
  
  /* generate the rules to build PK files in two different sizes */
  {
    Font *p = FontList;
    while ( p ) {
      char* name = p -> name;
      int scale = p -> scale;
      int extension = p -> extension;
      
      fprintf(stdout, "#\n# NAME=%s\n# SCALE = %d\n# EXTENSION = %d\n#\n",
	      name, scale, extension);
      
      /* MF -> PK */
      
      fprintf(stdout,"%s.%dpk:\n", name, extension, name);
      fprintf(stdout,"	-mf '\\mode:=%s;mag=%d/1000;\input %s'\n",
	      Mode, scale, name);
      fprintf(stdout,"	-gftopk %s.%dgf && rm -f %s.%dgf\n",
	      name, extension, name, extension);
      
      /* PK -> SNF */
      
      fprintf(stdout,"%s.%d.snf:	%s.%dpk\n",
	      name, extension, name, extension);
      fprintf(stdout,"	-mftobdf -dpi %d %s.%dpk\n",
	      Dpi, name, extension);
      fprintf(stdout,"	bdftosnf < %s.%d.bdf > %s.%d.snf\n",
	      name, extension, name, extension);
      
      p = p -> next;
    }
  }
  /* generate the goal line for PK files */
  {
    Font *p = FontList;
    fprintf(stdout,"pk-fonts:\\\n");
    while ( p ) {
      char* name = p -> name;
      int extension = p -> extension;
      
      fprintf(stdout, "\t%s.%dpk\\\n", name, extension);
      p = p -> next;
    }
    fprintf(stdout,"#	end of pk-fonts\n");	
  }
  
  /* generate the goal line for SNF files */
  {
    Font *p = FontList;
    fprintf(stdout,"snf-fonts:\\\n");
    while ( p ) {
      char* name = p -> name;
      int extension = p -> extension;
      
      fprintf(stdout, "\t%s.%d.snf\\\n", name, extension);
      p = p -> next;
    }
    fprintf(stdout,"#	end of snf-fonts\n");	
  }
}
