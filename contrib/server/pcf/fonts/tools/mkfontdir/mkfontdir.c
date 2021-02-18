/***********************************************************
Copyright 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $XConsortium: mkfontdir.c,v 1.10 88/10/11 08:07:40 rws Exp $ */

#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>

#include <X11/X.h>
#include "fontstr.h"
#include "fonttype.h"
#include "fontfilestr.h"
#include "fontdirstr.h"
#include "psfontdir.h"

#define  XK_LATIN1
#include <X11/keysymdef.h>

/***====================================================================***/

/*
 * Only works when table is freshly built. I.e. it cannot contain any aliases.
 */

static void 
WriteFontDir(table)
FontTablePtr	table;
{
char	 name[MAXPATHLEN];
FILE	*file;
int	 i;
int	 width = 0;

    /* XXX unix dependent file-name mangling */
    strcpy(name, (char *)fdOSContext(table));
    strcat(name, FontDirFile);
    file = fopen(name, "w");
    if (file == NULL)
	return;
    for (i = 0; i < table->used; i++) {
	char	*tmp=	tffName(table->fonts[i].pTFF);
	int 	len;
	
	len=	strlen(tmp);
	if (len > width)
	    width = len;
    }
    width += 4;
    fprintf(file, "%d\n", table->used);
    for (i = 0; i < table->used; i++) {
	char	*tmp=	tffName(table->fonts[i].pTFF);

	if (!strncmp(tmp,"./",2))	tmp+=	2;
	fprintf(file, "%-*s %s\n", width,  tmp, table->fonts[i].fontName);
    }
    if (fdPrivate(table))
	(void)WritePSFontDir(table);
    (void)fclose(file);
}

/***====================================================================***/

void 
GetFontName( pTFFile, font_name, ps_font_name, x_info )
TypedFontFilePtr	 pTFFile;
char			*font_name;
char			*ps_font_name;
PSXInfo			*x_info;
{
EncodedFontRec	font,*pFont;
CharSetRec	cs,*pCS;
int		i;

    Mask rtrn;
    char *name;

    font_name[0]=	'\0';
    ps_font_name[0]=	'\0';

    pFont=	&font;
    pCS=	&cs;
    font.pCS=	pCS;
    cs.tables=	0;

    rtrn= tffLoadFontTables(pTFFile,&pFont,FONT_PROPERTIES,fosNaturalParams);

    if (rtrn&FONT_PROPERTIES)
	return;

    for (i=0;i<pCS->nProps;i++) {
	name= (char *)NameForAtomOrNone(pCS->props[i].name);
	if ((strcmp(name,"FONT")==0)&&(pCS->isStringProp[i])) {
	    strcpy(font_name,NameForAtomOrNone(pCS->props[i].value));
	}
	else if ((strcmp(name,"_DEC_DEVICE_FONTNAMES")==0)&&
					(pCS->isStringProp[i])) {
	    register char *str=(char *)NameForAtomOrNone(pCS->props[i].value);

	    while (*str!='\0') {
		/* 5/14/89 (ef) -- I'm not proud of this code... */
		if (str[0]=='P'&&str[1]=='S'&&str[2]=='=') {
		    str+= 3;
		    while ((*str!='\0')&&(*str!=',')) {
			*ps_font_name= *str;
			ps_font_name++; str++;
		    }
		    *ps_font_name= '\0';
		    break;
		}
		else {
		    while (*str!='\0' && *str!= ',')
			str++;
		    if (*str==',')
			str++;
		}
	    }
	}
	else if (strcmp(name,"RESOLUTION_X")==0) {
	    x_info->xResolution= pCS->props[i].value;
	}
	else if (strcmp(name,"RESOLUTION_Y")==0) {
	    x_info->yResolution= pCS->props[i].value;
	}
	else if (strcmp(name,"POINT_SIZE")==0) {
	    x_info->pointSize= pCS->props[i].value;
	}
    }
    if (pFont)
	fontUnload(pFont,FONT_EVERYTHING);
    return;
}

/***====================================================================***/

static Bool
ProcessFile (dirName, fileName, table)
char		*dirName;
char		*fileName;
FontTablePtr	 table;
{
int			i, j;
TypedFontFilePtr	pTFF,pOldTFF;
char			font_name[MAXPATHLEN];
char			ps_font_name[MAXPATHLEN];
PSXInfo			ps_x_info;
int			pid, child;

    pTFF= tffOpenFile(fileName,dirName,"r");

    if (pTFF== NullTypedFontFile) {
	fosError("Couldn't open %s\n",fileName);
	return FALSE;
    }
    font_name[0]=	ps_font_name[0]=	'\0';
    GetFontName(pTFF, font_name, ps_font_name, &ps_x_info);
    tffCloseFile(pTFF,FALSE);

    if (strlen(font_name) == 0) {
	fosError("%s: invalid font, skipping\n", fileName);
	return FALSE;
    }
    fosCopyISOLatin1Lowered (font_name, font_name, strlen(font_name));
    pOldTFF = fdFindFontFile (table, font_name);
    if (pOldTFF!=NullTypedFontFile) {
	fosError("duplicate FONT property %s in file %s and %s\n",
	    font_name, fileName, tffName(pOldTFF));
	fosInformation("%s ignored\n",fileName);
	tffCloseFile(pTFF,TRUE);
	return FALSE;
    }
    (void) fdAddFont(table, font_name, pTFF);
    if (ps_font_name[0]!='\0') {
	(void) AddPSFontXName(table,ps_font_name,font_name,
				ps_x_info.pointSize,
				ps_x_info.xResolution,ps_x_info.yResolution);
    }
    return TRUE;
}

/***====================================================================***/

static Bool
Ematch(ext, name)
char	*ext;
char	*name;
{
int	i,j;

    i = strlen(name);
    j = strlen(ext);
    return ((i > j) && (strcmp(ext, name + i - j) == 0));
}

/***====================================================================***/

static int EnumFiles(directory, extension, data)
char		*directory;
char		*extension;
FontTablePtr	 data;		/* should be an opaque type */
{
DIR		*dirp;
struct direct	*file;
int 		 count = 0;
char		 filepath[MAXPATHLEN];

    if ((dirp = opendir (directory)) == NULL) {
	return 0;
    }

    while ((file = readdir (dirp)) != NULL) {
	if (Ematch (extension, file->d_name)) {
	    if (ProcessFile(directory, file->d_name, data))
		count++;
	}
    }
    closedir (dirp);
    return count;
}

/***====================================================================***/

static void 
DoDirectory(dir)
char	*dir;
{
FontTablePtr	table;
char		dirName[MAXPATHLEN];
int		i;

    /* XXX unix dependent file-name mangling */
    strcpy(dirName, dir);
    if (dirName[strlen(dirName) - 1] != '/')
	strcat(dirName, "/");
    table = fdCreateFontTable((pointer)dirName, 5);
    for (i= 0; fontFileReaders[i].extension; i++)
	(void)EnumFiles(dir, fontFileReaders[i].extension, table);
    if (table->used > 0)
	(void)WriteFontDir(table);
    (void)fdFreeFontTable(table);
}

/***====================================================================***/

main (argc, argv)
    int argc;
    char **argv;
{
    int i;

    InitAtoms();
    if (argc == 1)
	DoDirectory(".");
    else
	for (i = 1; i < argc; i++) {
	    DoDirectory(argv[i]);
 	}
    exit(0);	
}
