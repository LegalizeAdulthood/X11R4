/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/filetype.c,v 2.8 89/11/20 11:11:12 ajp Exp $ */
/* $ACIS:filetype.c 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/filetype.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/filetype.c,v 2.8 89/11/20 11:11:12 ajp Exp $";
#endif /* lint */

#include <stdio.h>
#include <class.h>
#include <im.ih>
#include <filetype.eh>
#include <attribs.h>
#include <environ.ih>

#include <andrewos.h> /* sys/types.h sys/file.h */
#include <sys/param.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include <pwd.h>

#define DEFAULTTYPE "text"

static struct mapEntry {
    struct mapEntry *next;
    char *fileExtension;
    char *dataName;
    struct attributes *attributes;
} *allEntries = NULL, defaultMapping = {NULL, NULL, NULL, NULL};

extern int errno;
extern char *index (), *rindex();

/* These next statics are fairly bogus. */
static char lastFilename[MAXPATHLEN];
static char lastExtension[32]; /* Extensions > 32 char's are trunc'ed. */

/* Some static space for holding file dependent attributes (as opposed to
 * extension specific ones).
 */
static struct attributes hardWiredAttributes[4];

#define SIZEATTRINDEX 0 /* Where in above array to put file's size. */
#define NAMEATTRINDEX 1 /* Where to put filename attribute. */
#define EXTATTRINDEX  2 /* Where to put file's extension. */
#define READONLYINDEX 3 /* Where to put the readonly flag. */

/* FullName (path, basefile) computes the full path name of path and 
	returns a pointer to static storage containing the path.  If path  
	begins with /, the name is justed folded to remove extraneous  
	dots.  If the path begins with ~, the appropriate home directory  
	is used.  If the name begins otherwaise, it is made relative to  
	the directory containing basefile.    
	Basefile should be a full pathname.  If empty, or begins without a slash,
	the pathname is made relative to the current working directory. 
*/

static char *FullName (path, basefile) register char *path, *basefile; {
    static char buf[MAXPATHLEN];

    if (*path == '/')
	strcpy (buf, path);
    else if (*path == '~') {
	register char *tail;
	register char *p1 = path+1;
	char *home;

	if (*p1=='\0' || *p1=='/') {

            
            home = environ_GetHome(NULL);

            if (*p1 == '\0')
	        tail = p1;
            else
	        tail = path+2;
	}
	else {  /* ~name/...  */
	    tail = index(path, '/');
	    if (tail==0) {
		home = environ_GetHome(p1);
		tail = path + strlen(path);
	    }
	    else {
		*tail = '\0';
		home = environ_GetHome(p1);
		*tail++ = '/';
	    }
	}
        if (home != NULL)
            strcpy(buf, home);
	strcat(buf, "/");
	strcat(buf, tail);
    }
    else {   /* find name relative to directory for basefile */
	register char *slash;
	if (basefile==NULL) basefile = "";
	if (*basefile != '/' && im_GetDirectory(buf) != NULL)
	    strcat (buf, "/");
	else
	    *buf = '\0';    /* ??? discard error message if getwd==NULL */
	strcat (buf, basefile);
	slash = rindex(buf, '/');
	if (slash==0)
	    slash = buf+strlen(buf), *slash = '/';
	*++slash = '\0';
	strcat(buf, path);
    }
    FoldName(buf);
    return buf;
}


/* FoldName -- remove all "." and ".." components from a file name 
     modifies the buffer passed to it 
	Rules:
	    buffer is	=> use this string
		/	=>  /
		.	=> .
		..	=> ..
		empty	=> .
		x	=> x
	    buffer starts    use this start
		/..	=> /
		x/..	=> .
		./..	=> ..
		..	=> ..
		../..	=> ../..   (any number of ../)
		x/y	=> x/y
	    buffer continues    replace with
		/./	=> /
		//	=> /
		/x/../	=> /
    Does not know about ~ 
*/
static FoldName (path)
    register char *path;			/* path to fold */
{
    char   *pStart,		/* points to first char of a component */
	   *pEnd;		/* points to first char following
				   component */
    register char *dest;
    int     len;

    if (path == NULL)
	return;
    dest = path;
    pEnd = (*path == '/' ? path + 1 : path);
    for (;;) {
	pStart = pEnd;
	pEnd = index (pStart, '/');
	if (pEnd == NULL)
	    pEnd = pStart + strlen (pStart);
	len = pEnd - pStart;
	if (len == 0) { /* ignore empty components, but preserve trailing /'s. */
            if (*pEnd == '\0')
                *dest++ = '/';
        }
	else if (len == 1 && *pStart == '.' && pStart[1] == '/')
	    {}   /* ignore single dots */
	else if (len == 2 && pStart[0] == '.' && pStart[1] == '.' && pStart[2] == '/') {
	    switch (dest - path) {
	    case 0: /*  /.. => /   and   .. => .. */
		    if (*path != '/')  
			*dest++ = '.', *dest++ = '.';
		    break;
	    case 1: /*  ./.. => ..   and   x/.. => . */
		    if (*path == '.') 
			*dest++ = '.';
		    else *path = '.';
		    break;
	    case 2: /*  ../.. => ../..   and   x/.. => .  */
		    if (strncmp(path, "..", 2)==0)
			*dest++ = '/', *dest++ = '.', *dest++ = '.';
		    else *path = '.', dest = path+1;
		    break;
	    default: /*  y/x/..=>y   x/..=>.  /x/..=>/   ../../..=>../../..  */
		    if (strncmp(dest-3, "/..", 3)==0)
			*dest++ = '/', *dest++ = '.', *dest++ = '.';
		    else {  /* this is the principal case for .. */
			while (dest > path && *--dest != '/') {}
			if (dest == path && *path != '/')
			    *dest++ = '.';
		    }
		    break;
	    }
	}
	else {
	    if (dest>path || *path == '/') 
		*dest++ = '/';
	    strncpy (dest, pStart, len);
	    dest += len;
	}
	if (*pEnd++ == 0)
	    break;
    }
    if (dest==path)   /* inital path was  /  .  or empty */
	*dest++ = (*path == '/' ? '/' : '.');
    *dest++ = 0;
}

static void FreeAttributes(entry)
    struct mapEntry *entry;
{

    struct attributes *thisAttr, *nextAttr;

    for (thisAttr = entry->attributes; thisAttr != NULL; thisAttr = nextAttr) {
        free(thisAttr->key);
        if (thisAttr->value.string != NULL) /* Attributes on this list are guaranteed to be string attributes... */
            free(thisAttr->value.string);
        nextAttr = thisAttr->next;
        free(thisAttr);
    }
}

void filetype__AddEntry(classID, extension, dataName, attributes)
    struct classheader *classID;
    char *extension, *dataName;
    char *attributes;
{

    register struct mapEntry *thisEntry;

    if (strcmp(extension, "*") == 0) {
        thisEntry = &defaultMapping;
        defaultMapping.fileExtension = "*";
    }
    else
        for (thisEntry = allEntries; thisEntry != NULL && strcmp(thisEntry->fileExtension, extension); thisEntry = thisEntry->next)
            ;

    if (thisEntry == NULL) { /* Allocate an entry if we didn't find one. */
        thisEntry = (struct mapEntry *) malloc(sizeof(struct mapEntry));
        thisEntry->fileExtension = (char *) malloc(strlen(extension) + 1);
        strcpy(thisEntry->fileExtension, extension);
        thisEntry->next = allEntries;
        allEntries = thisEntry;
    }
    else { /* Otherwise free the old values associated with this entry. */
        if (thisEntry->dataName != NULL)
            free(thisEntry->dataName);
        FreeAttributes(thisEntry);
    }

/* Fill in name. */
    thisEntry->dataName = (char *) malloc(strlen(dataName) + 1);
    strcpy(thisEntry->dataName, dataName);

    thisEntry->attributes = NULL; /* Initialize This list to empty. */

/* If necessary, parse the colon seperated string of assignments to generate the
 * values list. The string looks like "key1=value1;key2=value2;...". All values
 * that result from this parsing are string values, integer values are never
 * generated. The only way integer values show up at present is in the hardwired
 * attributes from filetype_Lookup below.
 *
 * This function does no whitespace parsing in order to be "literal" about the
 * values it generates.
 */
    if (attributes != NULL) {

        char *thisKey = attributes, *thisVal;
        struct attributes *thisAttr;

        while (*thisKey != '\0') {
            for (thisVal = thisKey; *thisVal != '=' && *thisVal != '\0'; ++thisVal)
                ; /* Skip to find value */
            if (thisVal != thisKey) { /* If we found something. */
                thisAttr = (struct attributes *) malloc(sizeof(struct attributes));
                thisAttr->key = (char *) malloc(thisVal - thisKey + 1);
                strncpy(thisAttr->key, thisKey, thisVal - thisKey);
                thisAttr->key[thisVal - thisKey] = '\0';
                if (*thisVal != '\0') /* Guaranteed to be either '=' or '\0' */
                    thisVal++;
                for (thisKey = thisVal; *thisKey != ';' && *thisKey != '\0'; ++thisKey)
                    ;
                if (thisKey != thisVal) {
                    thisAttr->value.string = (char *) malloc(thisKey - thisVal + 1);
                    strncpy(thisAttr->value.string, thisVal, thisKey - thisVal);
                    thisAttr->value.string[thisKey - thisVal] = '\0';
                }
                else
                    thisAttr->value.string = NULL;
                thisAttr->next = thisEntry->attributes;
                thisEntry->attributes = thisAttr;
                if (*thisKey == ';')
                    ++thisKey;
            }
	    else /* Else, guarantee the loop will terminate. */
		if (*thisKey != '\0')
		    ++thisKey;
        }
    }
}

int filetype__DeleteEntry(classID, extension)
    struct classheader *classID;
    register char *extension;
{

    register struct mapEntry *traverse, **previous;

    if (strcmp(extension, "*")) {
        if (defaultMapping.dataName)
            free(defaultMapping.dataName);
        if (defaultMapping.attributes) {
            FreeAttributes(&defaultMapping);
            defaultMapping.attributes = NULL;
        }
        defaultMapping.fileExtension = NULL;
        return 1;
    }
    previous = &allEntries;
    for (traverse = allEntries; traverse != NULL; traverse = traverse->next) {
        if (strcmp(traverse->fileExtension, extension) == 0) {
            free(traverse->fileExtension);
            free(traverse->dataName);
            FreeAttributes(traverse);
            *previous = traverse->next;
            free(traverse);
            return 1;
        }
        previous = &traverse->next;
    }
    return 0;
}

/* NOTE: filetype_Lookup.
 *    The value returned through the attributes parameter to this function
 *    contains pointers to static storage. Its value should be used before
 *    the next call to this routine. If this can't be guaranteed, the
 *    programmer must copy the attributes list.
 */
char *filetype__Lookup(classID, file, filename, objectID, attributes)
    struct classheader *classID;
    FILE *file;
    register char *filename;
    long *objectID;
    struct attributes **attributes;
{

    register struct mapEntry *thisEntry;
    register char *extension;
    static char objectName[100]; /* The place to put the name of the class that should be used to read this file. */
    char *targetObject = NULL; /* Holds potential value for objectName. */
    struct attributes *defaultAttributes = NULL; /* Only used if the file is in non-datastream format. */

    if (attributes)
        *attributes = NULL; /* In case we don't set it otherwise. */
/* First, if possible, do the lookup on the filename extension
 * to get the attributes and possibly the object (class) type.
 * This information may be overrriden by that in the file.
 */
    if (filename != NULL && filename[0] != '\0') {
        register char *s = rindex(filename, '/');
        extension = rindex(filename, '.');
        if (extension == NULL || (s != NULL && s > extension))
            extension = "";

	for(thisEntry = allEntries; thisEntry != NULL; thisEntry = thisEntry->next)
	    if (!strcmp(thisEntry->fileExtension, extension))  {
		targetObject = thisEntry->dataName;
                defaultAttributes = thisEntry->attributes;
                break;
            }
        if (thisEntry == NULL && defaultMapping.fileExtension != NULL) {
            targetObject = defaultMapping.dataName;
            defaultAttributes = defaultMapping.attributes;
        }
/* Setup hardwired attributes in static storage.
 * Currently, we support the following hardwired attributes:
 *    "filename" => The full pathname of the file as given to this routine.
 *    "extension" => The file's extension, either of the form ".x"
 *                    (where x is a string) or "".
 *    "filesize" => An integer attribute giving the size (in bytes) of the file.
 *                  This is filled in below in the file part of this routine.
 */
        if (attributes != NULL) {
            strcpy(lastFilename, filename);
            /* Why can't the string library do the right thing? */
            strncpy(lastExtension, extension, sizeof(lastExtension));
            lastExtension[sizeof(lastExtension) - 1] = '\0';

            hardWiredAttributes[EXTATTRINDEX].key = "extension";
            hardWiredAttributes[EXTATTRINDEX].value.string = lastExtension;
            hardWiredAttributes[EXTATTRINDEX].next = NULL;

            hardWiredAttributes[NAMEATTRINDEX].key = "filename";
            hardWiredAttributes[NAMEATTRINDEX].value.string = lastFilename;
            hardWiredAttributes[NAMEATTRINDEX].next = &hardWiredAttributes[EXTATTRINDEX];

            if ((access(filename, W_OK) < 0) && (errno == EACCES || errno == EROFS)) {
                hardWiredAttributes[READONLYINDEX].key = "readonly";
                hardWiredAttributes[READONLYINDEX].value.integer = TRUE;
                hardWiredAttributes[READONLYINDEX]. next = &hardWiredAttributes[NAMEATTRINDEX];
                *attributes = &hardWiredAttributes[READONLYINDEX];
            }
            else
                *attributes = &hardWiredAttributes[NAMEATTRINDEX];
        }
    }

/* Now try and get any info we need out of the file. */
    if (file != NULL) {
        long origPos;
        int c;
        char *s = "\\begindata{",
             objectIDBuffer[20], *readID = objectIDBuffer; /* This size should hold a number alright. */
        struct stat statBuf;

        origPos = ftell(file);

        if ((attributes != NULL) && (fstat(fileno(file), &statBuf) >= 0)) { /* This may not work for pipes... */
            hardWiredAttributes[SIZEATTRINDEX].key = "filesize";
            hardWiredAttributes[SIZEATTRINDEX].value.integer = statBuf.st_size;
            hardWiredAttributes[SIZEATTRINDEX].next = *attributes;
            *attributes = &hardWiredAttributes[SIZEATTRINDEX];

        }

        while (getc(file) == *s && *++s != '\0')
            ;
        if (*s == '\0') {
            s = objectName;
            while ((c = getc(file)) != EOF && c != ',')
                *s++ = c;
            if (c == ',') {
                *s = '\0';
                while ((c = getc(file)) != EOF && c != '}' && (readID < (objectIDBuffer + (sizeof(objectIDBuffer) - 1))))
                    *readID++ = c;
                if (c == '}') {
                    if ((c = getc(file)) != '\n')
                        ungetc(c, file);
                    if (objectID != NULL) {
                        *readID = '\0';
                        *objectID = atoi(objectIDBuffer);
                    }

                    return objectName;
                }
            }
        }

        fseek(file, origPos > 0 ? origPos : 0, 0L);
    }

    /* Link in default attributes. */
    if (attributes && *attributes) {

        struct attributes *thisAttribute;

        for (thisAttribute = *attributes; thisAttribute != NULL; thisAttribute = thisAttribute->next)
            if (thisAttribute->next == NULL) {
                thisAttribute->next = defaultAttributes;
                break;
            }
    }

    if (objectID != NULL)
        *objectID = 0;

    if (targetObject != NULL) {
        strcpy(objectName, targetObject);
        return objectName;
    }
    else
        return NULL;
}

void filetype__CanonicalizeFilename(classID, canonicalName, name, maxSize)
    struct classheader *classID;
    char *canonicalName, *name;
    int maxSize;
{

    char fullName[MAXPATHLEN];
    int firstChar;

    while (*name == ' ')
        name++;
    if ((firstChar = *name) != '/' && firstChar != '~') {
        im_GetDirectory(fullName);
        strcat(fullName, "/");
        strcat(fullName, name);
    }
    else
        strncpy(fullName, name, sizeof(fullName));
    strncpy(canonicalName, FullName(fullName, 0), maxSize);
}
