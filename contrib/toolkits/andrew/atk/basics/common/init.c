/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/init.c,v 2.9 89/10/04 18:51:16 cfe Exp $ */
/* $ACIS:init.c 1.5$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/init.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/init.c,v 2.9 89/10/04 18:51:16 cfe Exp $";
#endif /* lint */

/* init.c
 * Read a user's init file to bind keys and menus.
  */

#include <sys/param.h>
#include <ctype.h>
#include <sys/stat.h>
#include <andrewos.h> /* sys/file.h */

#include <class.h>
#include <init.eh>
#include <environ.ih>
#include <filetype.ih>
#include <proctbl.ih>
#include <keymap.ih>
#include <keystate.ih>
#include <menulist.ih>

extern char *index();
char *GetToken();
void ErrorMsg();

/* Format of a .bxinit file:
 * Blank lines and lines beginning with a # are ignored.
 * Other lines have a command and maybe some arguments.  The commands are:
 *
 * addkey function keysequence [class] [loadclass] [inheritp] -- Bind a key to a function.The key
 *     sequence is only honored if an object of type class is present. If
 *     class is omitted, the first part of the function name (upto a hyphen) is
 *     used. Loadclass is used to determine which module must be loaded to find
 *     function. If loadclass is omitted, the first part ot the function name
 *     (upto a hyphen) is used. The class parameter must be present for the
 *     loadclass parameter to be used. The inheritp parameter determines whether
 *     the class test is an inclusion test or an equality test.
 * addmenu function menustring [class] [loadclass] [inheritp] -- Add an extra menu item which invokes
 *     function. See description of addkey for information about class and loadclass.
 *     if function is "", this constitutes a menu deletion and will prevent the 
 *     named menu item from appearing.
 * addfiletype extension classname [attributes] -- Use indicated class as the
 *     data object for files with given extension. the optional attributes
 *     string is used to pass through class specific data.
 * load classname -- Loads the indicated class.
 * call function args -- call named function with given args.
 * include filename -- includes the given file.
 */

struct keys {
    struct keys *next;
    char *class;
    boolean inherit;
    struct keymap *keymap;
};

struct menus {
    struct menus *next;
    char *class;
    boolean inherit;
    struct menulist *menulist;
};

struct keystateList {
    struct keystateList *next;
    struct keystate *keystate;
};

struct mlList {
    struct mlList *next;
    struct menulist *menulist;
};

static struct keystateList *freeKeystates = NULL;
static struct mlList *freeMenulists = NULL;

/* Next three used for error handling. */
static char *currentFile;
static int currentLine = 0;
static void (*currentErrorProc)() = NULL;
static pointer currentErrorRock;

boolean init__InitializeObject(classID, init)
    struct classheader *classID;
    struct init *init;
{
    init->keys = NULL;
    init->menus = NULL;
    init->usedKeystates = NULL;
    init->usedMenus = NULL;
    return TRUE;
}

void init__FinalizeObject(self)
    struct init *self;
{
    {
        struct keystateList *freeItem, *next;

        for (freeItem = self->usedKeystates; freeItem != NULL; freeItem = next) {
            next = freeItem->next;
            freeItem->next = freeKeystates;
            freeKeystates = freeItem;
        }
    }

    {
        struct mlList *freeItem, *next;

        for (freeItem = self->usedMenus; freeItem != NULL; freeItem = next) {
            next = freeItem->next;
            freeItem->next = freeMenulists;
            freeMenulists = freeItem;
        }
    }
}

static struct keymap *GetKeymap(init, className, inheritFlag)
    struct init *init;
    char *className;
    boolean inheritFlag;
{

    struct keys *keys;

    for (keys = init->keys; keys != NULL && (strcmp(className, keys->class) != 0); keys = keys->next)
        ;
    if (keys == NULL) {
        keys = (struct keys *) malloc(sizeof(struct keys));
        keys->class = (char *) malloc(strlen(className) + 1);
        strcpy(keys->class, className);
        keys->keymap = keymap_New();
        keys->inherit = inheritFlag;
        keys->next = init->keys;
        init->keys = keys;
    }
    return keys->keymap;
}

static struct menulist *GetMenulist(init, className, inheritFlag)
    struct init *init;
    char *className;
    boolean inheritFlag;
{

    struct menus *menus;

    for (menus = init->menus; menus != NULL && (strcmp(className, menus->class) != 0); menus = menus->next)
        ;
    if (menus == NULL) {
        menus = (struct menus *) malloc(sizeof(struct menus));
        menus->class = (char *) malloc(strlen(className) + 1);
        strcpy(menus->class, className);
        menus->menulist = menulist_New();
        menus->inherit = inheritFlag;
        menus->next = init->menus;
        init->menus = menus;
    }
    return menus->menulist;
}

enum init_bindingtype {init_KEY, init_MENU};

static void BindFunction(init, args, forceLoad, type, commandName)
    struct init *init;
    char **args;
    boolean forceLoad;
    enum init_bindingtype type;
    char *commandName;
{

    char *function, *tempString, *binding, *class, *loadClass, *inherit, *parameterString;
    char className[100], loadClassName[100], translatedKeys[32];
    struct proctable_Entry *proc;
    boolean inheritFlag = TRUE;

    function = GetToken(args);
    if (function == NULL) {
        ErrorMsg("Missing function name in %s command.\n", commandName);
        return;
    }
    binding = GetToken(args);
    if (binding == NULL) {
        if (type == init_KEY)
            ErrorMsg("Missing key sequence in addkey command.\n");
        else
            ErrorMsg("Missing menu string in addmenu command.\n");
        return;
    }
    class = GetToken(args);
    if (class == NULL) {

        char *hyphen;

        hyphen = index(function, '-');
        if (hyphen == NULL) {
            ErrorMsg("Missing class name in %s command.\n", commandName);
            return;
        }
        strncpy(className, function, hyphen - function);
        className[hyphen - function] = '\0';
        class = loadClass = className;
    }
    else {
        loadClass = GetToken(args);
        if (loadClass == NULL && *function != '\0') {

            char *hyphen;

            hyphen = index(function, '-');
            if (hyphen == NULL) {
                ErrorMsg("Missing loadclass name in %s command.\n", commandName);
                return;
            }
            strncpy(loadClassName, function, hyphen - function);
            loadClassName[hyphen - function] = '\0';
            loadClass = loadClassName;
        }
    }

    inherit = GetToken(args);
    if (inherit != NULL)
        if (strcmp(inherit, "noinherit") == 0)
            inheritFlag = FALSE;
        else if (strcmp(inherit, "inherit") != 0) {
            ErrorMsg("Bad inherit flag value %s; must be either \"inherit\" or \"noinherit\".\n", inherit);
            return;
        }            

    parameterString = GetToken(args);

    if (type == init_KEY)
        if (TranslateKeySequence(binding, translatedKeys) < 0) {
            ErrorMsg("Bad key sequence %s.\n", binding);
            return;
        }

/* This next if statement handles the special case of a meu item deletion. */
    if (*function != '\0') {
/* proctable_DefineProc does not allocate storage for its arguments... */
        tempString = (char *) malloc(strlen(function) + 1);
        strcpy(tempString, function);
        function = tempString;
        tempString = (char *) malloc(strlen(loadClass) + 1);
        strcpy(tempString, loadClass);
        loadClass = tempString;
        proc = proctable_DefineProc(function, NULL, NULL, loadClass, NULL);

        if (forceLoad && !proctable_Defined(proc)) {
            proctable_ForceLoaded(proc);
            if (!proctable_Defined(proc)) { /* Should be better than this... */
                ErrorMsg("Either couldn't load class %s, or class didn't define function %s\n", loadClass, function);
                return;
            }
        }
    }
    else
        proc = NULL;

    if (parameterString) {

        char *tempString;

        tempString = (char *) malloc(strlen(parameterString) + 1);
        strcpy(tempString, parameterString);
        parameterString = tempString;
    }

    if (type == init_KEY) {

        struct keymap *keymap;

        keymap = GetKeymap(init, class, inheritFlag);
        keymap_BindToKey(keymap, translatedKeys, proc,
                         (parameterString != NULL) ? (long) parameterString :
                         (long) translatedKeys[strlen(translatedKeys) - 1] /* a useful rock */);
    }
    else {

        struct menulist *menulist;

        menulist = GetMenulist(init, class, inheritFlag);
        menulist_AddToML(menulist, binding, proc, parameterString, 0);
    }
}

/* Establish a default dataobject for files with a given extension. */
static void AddFileType(args, forceLoad)
    char **args;
    boolean forceLoad;
{

    char *extension, *type, *attributes;

    extension = GetToken(args);
    if (extension == NULL) {
	ErrorMsg("Missing extension in addfiletype command.\n");
	return;
    }
    type = GetToken(args);
    if (type == NULL) {
	ErrorMsg("Missing class name in addfiletype command.\n");
	return;
    }
    attributes = GetToken(args);

    if (forceLoad && (class_Load(type) == NULL))
    	ErrorMsg("Could not load class %s in addfiletype command for extension %s.\n", type, extension);
    else
        filetype_AddEntry(extension, type, attributes);
}

/* Call the function associated with a proctable command with add-hoc
 * arguments. Not very useful since first argument is supposed to be a view.
 */
#define NARGS	6
static void Call(args)
    char **args;
{

    char *functionName;
    char *functionArgs[NARGS];
    struct proctable_Entry *entry;
    int (*function)() = NULL;
    int i;
    char *argument;

    functionName = GetToken(args);
    if (functionName == NULL) {
	ErrorMsg("Missing function name in call command.\n");
	return;
    }
    for (i = 0; i < NARGS && ((argument = GetToken(args)) != NULL); ++i) {
	if (isdigit(*argument))
	    functionArgs[i] = (char *) atoi(argument);
	else
	    functionArgs[i] = argument;
    }
    entry = proctable_Lookup(functionName);
    if (entry != NULL) {
        proctable_ForceLoaded(entry);
        function = entry->proc;
    }
    if (function == NULL) {
	ErrorMsg("Undefined function in call command: %s\n", functionName);
	return;
    }
    else
	(*function)(functionArgs[0], functionArgs[1], functionArgs[2], functionArgs[3], functionArgs[4], functionArgs[5]);
}

/* Load the named class. Useful for debugging and in certain applications where
 * key bindings (or menus) are inappropriate.
 */
static void Load(args)
    char **args;
{

    char *class = GetToken(args);

    if (class == NULL)
        ErrorMsg("Missing class name in load command.");
    else {
        if (class_Load(class) == NULL)
            ErrorMsg("Could not load class %s in load command.", class);
    }
}

/* Include the named file as if its contents were inline within the current
 * file.
 */
Include(init, args, forceLoad)
    struct init *init;
    char **args;
    boolean forceLoad;
{

    char *file = GetToken(args);

    if (file == NULL)
        ErrorMsg("Missing filename in include command.");
    else {
        if (ReadFile(init, file, forceLoad) < 0)
            ErrorMsg("Could not read included file %s.", file);
    }
}

/* Find the first token on this line and update the pointer to the buffer to
 * point past it.  Also smashes the buffer with a null character.
 */
static char *GetToken(pp)
    char **pp;
{
    char *from = *pp, *to = from;
    int quote = FALSE;
    char *s;

    while (*from != '\n' && isspace(*from)) /* Skip whitespace .*/
	++from;

    if (*from == '#') /* Skip comments and fall through to end-of-buffer code immediately below. */
        do {
	    ++from;
	} while (*from != '\n' && *from != '\0');

    if (*from == '\n' || *from == '\0') {
        *pp = from;
        return NULL;
    }

    if (*from == '"') {
        quote = TRUE;
        to = s = ++from;
        if (*from == '"') {
            *pp = from + 1;
            return "";
        }
    }
    else
        to = s = from;
    do {
        if (*from == '\\')
            *to++ = parseBackslashed(&from);
        else
            *to++ = *from++;
    } while (*from != 0 && (quote ? *from != '"' : !isspace(*from)));

    if (*from == '\n')
        *pp = from;
    else
        *pp = ++from;

    *to++ = '\0';
    return s;
}

/* Translate a key sequence that has ^A, \ddd, and \c conventions. */
static int TranslateKeySequence(from, to)
    char *from;
    char *to;
{
    while (*from != '\0') {
        if (*from == '\\') {

            int temp = parseBackslashed(&from);

            if (temp == -1)
                return -1;
            else
                *to++ = temp;
        }
        else if (*from == '^') {
            ++from;
            if (*from == 0)
                return -1;
            *to++ = (*from++) & 0x1f;
        }
        else
            *to++ = *from++;
    }
    *to++ = 0;
    return 0;
}

static int parseBackslashed(fromChars)
    char **fromChars;
{

    int returnChar;
    char *from = *fromChars;
    static char *bsSource = "ebrnt";
    static char *bsDest = "\033\b\r\n\t";

    if (*from == '\\') {
        ++from;
        if (*from == 0)
            return -1;
        if (isdigit(*from)) {

            int sum = 0, i;

            for (i = 0; i < 3; ++i) {
                if (!isdigit(*from))
                    break;
                if (*from == '8' || *from == '9')
                    return -1;
                sum = sum * 8 + *from - '0';
                ++from;
            }
            returnChar = sum;
        }
        else {

            char *p;

            p = index(bsSource, *from);
            if (p != NULL)
                returnChar = bsDest[p-bsSource];
            else
                returnChar = *from;
            ++from;
        }
    }
    else
        return -1;
    *fromChars = from;
    return returnChar;
}

struct keystate *init__ModifyKeystate(self, keystate)
    struct init *self;
    struct keystate *keystate;
{

    struct keys *keys;
    struct keystate *traverse, **previous;
    struct keystateList *freeItem, *next;

    for (freeItem = self->usedKeystates; freeItem != NULL; freeItem = next) {
        next = freeItem->next;
        freeItem->next = freeKeystates;
        freeKeystates = freeItem;
    }
    self->usedKeystates = NULL;
    for (keys = self->keys; keys != NULL; keys = keys->next) {
        previous = &keystate;
        for (traverse = keystate; traverse != NULL; traverse = traverse->next) {
            if (keys->inherit ?
                    class_IsTypeByName(class_GetTypeName(traverse->object), keys->class) :
                    (strcmp(class_GetTypeName(traverse->object), keys->class) == 0)) {
                if (freeKeystates == NULL) {
                    freeItem = (struct keystateList *) malloc(sizeof(struct keystateList));
                    freeItem->keystate = keystate_Create(traverse->object, keys->keymap);
                }
                else {
                    freeItem = freeKeystates;
                    freeKeystates = freeItem->next;
                    freeItem->keystate->object = traverse->object;
                    freeItem->keystate->orgMap = keys->keymap;
                    freeItem->keystate->curMap = keys->keymap;
                    keystate_Reset(freeItem->keystate);
                }
                freeItem->next = self->usedKeystates;
                self->usedKeystates = freeItem;
                *previous = freeItem->keystate;
                freeItem->keystate->next = traverse;
            }
            previous = &traverse->next;
        }
    }
    return keystate;
}

static struct basicobject *CheckML(menulist, class, inherit)
    struct menulist *menulist;
    char *class;
    boolean inherit;
{

    struct menulist *thisML;
    struct basicobject *thisObject;

    if (inherit ?
            class_IsTypeByName(class_GetTypeName(menulist->object), class) :
            (strcmp(class_GetTypeName(menulist->object), class) == 0))
        return menulist->object;
    menulist_RewindBeforeMC(menulist);
    while ((thisML = menulist_NextBeforeMC(menulist)) != NULL)
        if ((thisObject = CheckML(thisML, class, inherit)) != NULL)
            return thisObject;
    menulist_RewindAfterMC(menulist);
    while ((thisML = menulist_NextAfterMC(menulist)) != NULL)
        if ((thisObject = CheckML(thisML, class, inherit)) != NULL)
            return thisObject;
    return NULL;
}

struct menulist *init__ModifyMenulist(self, menulist)
    struct init *self;
    struct menulist *menulist;
{

    struct menus *menus;
    struct mlList *freeItem, *next;
    struct basicobject *thisObject;
    struct menulist *topMenulist = menulist;

    for (freeItem = self->usedMenus; freeItem != NULL; freeItem = next) {
        next = freeItem->next;
        freeItem->next = freeMenulists;
        freeMenulists = freeItem;
    }
    self->usedMenus = NULL;
    for (menus = self->menus; menus != NULL; menus = menus->next)
        if ((thisObject = CheckML(menulist, menus->class, menus->inherit)) != NULL) {
            if (freeMenulists == NULL) {
                freeItem = (struct mlList *) malloc(sizeof(struct mlList));
                freeItem->menulist = menulist_DuplicateML(menus->menulist, thisObject);
            }
            else {
                freeItem = freeMenulists;
                freeMenulists = freeMenulists->next;
                freeItem->menulist->object = thisObject;
                freeItem->menulist->menus = menus->menulist->menus;
                menulist_ClearChain(freeItem->menulist);
            }
            freeItem->next = self->usedMenus;
            self->usedMenus = freeItem;
            menulist_ChainAfterML(freeItem->menulist, topMenulist, topMenulist);
            topMenulist = freeItem->menulist;
        }
    return topMenulist;
}

struct init *init__Duplicate(init)
    struct init *init;
{

    struct init *newInit;

    newInit = init_New();
    newInit->keys = init->keys;
    newInit->menus = init->menus;
    return newInit;
}

#define INITIALSIZE 512

/* Hacked routine to rea a "whole file" into memory. */
static char *MapFile(filename, fileLength)
    char *filename;
    long *fileLength; /* OUT */
{

    int fd;
    char *buffer;
    long length = 0;

    if ((fd = open(filename, O_RDONLY, 0)) >= 0) {

        struct stat statBuf;

        if (fstat(fd, &statBuf) >= 0) {

            long bufferSize; /* Current size of malloced block. */
            long bytesRead = 0;

            /* Find the size. In the case of special files, use a suitable default. */
            if ((statBuf.st_mode & S_IFMT) == S_IFREG)
                bufferSize = statBuf.st_size ;
            else
                bufferSize = INITIALSIZE;

            buffer = (char *) malloc(bufferSize + 1); /* +1 for NUL at end. */

            while (buffer != NULL && (bytesRead = read(fd, buffer + length, bufferSize - length )) > 0) {
                length += bytesRead;
                if (length >= bufferSize) {
                    bufferSize *= 2;
                    buffer = (char *) realloc(buffer, bufferSize + 1); /* +1 for NUL at end. */
                }
            }
            if (bytesRead < 0) {
                free(buffer);
                buffer = NULL;
            }
            else
                buffer[length] = '\0';
        }
        else
            buffer = NULL;

        close(fd);
    }
    else
        buffer = NULL;

    if (fileLength != NULL)
        *fileLength = length;
    return buffer;
}

#define UnmapFile(mappedMemory) free(mappedMemory)

static int ReadFile(init, filename, executeImmediately)
struct init *init;
char *filename;
boolean executeImmediately;
{

    char *buffer;
    long length;

    if ((buffer = MapFile(filename, &length)) != NULL) {

        char *token, *p;
        char *prevFile = currentFile;
        int prevLine = currentLine;

        currentFile = filename;
        currentLine = 0;

        p = buffer;
        while (p < buffer + length) {
            ++currentLine;
            token = GetToken(&p);
            if (token != NULL)
                if (strcmp(token, "addkey") == 0)
                    BindFunction(init, &p, executeImmediately, init_KEY, "addkey");
                else if (strcmp(token, "addmenu") == 0)
                    BindFunction(init, &p, executeImmediately, init_MENU, "addmenu");
                else if (strcmp(token, "addfiletype") == 0)
                    AddFileType(&p, executeImmediately);
                else if (strcmp(token, "call") == 0)
                    Call(&p);
                else if (strcmp(token, "load") == 0)
                    Load(&p);
                else if (strcmp(token, "include") == 0)
                    Include(init, &p, executeImmediately);
                else
                    ErrorMsg("Undefined command - %s\n", token);

            /* Skip to the end of line. */
            while (*p != '\n' && *p != '\0')
                ++p;

            /* Get to the next line. */
            ++p;
        }

        UnmapFile(buffer);

        currentFile = prevFile;
        currentLine = prevLine;

        return 0;
    }
    else
        return -1;
}    

/* Read the user's init file. */
int init__Load(init, filename, errorProc, errorRock, executeImmediately)
    struct init *init;
    char *filename;
    void (*errorProc)();
    pointer errorRock;
    boolean executeImmediately; /* True if modules should be loaded now. Useful for debugging init files. */
{
    currentErrorProc = errorProc;
    currentErrorRock = errorRock;

    return ReadFile(init, filename, executeImmediately);
}

static void ErrorMsg(msg, a1, a2, a3, a4)
    char *msg;
{

    char buffer[300], *bufferEnd;

    if (currentErrorProc != NULL) {
        sprintf(buffer, "File: %s Line: %d: ", currentFile, currentLine);
        bufferEnd = buffer + strlen(buffer);
        sprintf(bufferEnd, msg, a1, a2, a3, a4);
        (*currentErrorProc)(currentErrorRock, buffer);
    }
}

	void
init__AddKeyBinding(self, class, inherit, keymap)
	struct init *self;
	char *class;
	boolean inherit;
	struct keymap *keymap;
{
	struct keys *keys;
	keys = (struct keys *)malloc(sizeof(struct keys));
	keys->class = (char *)malloc(strlen(class) + 1);
	strcpy(keys->class, class);
	keys->inherit = inherit;
	keys->keymap = keymap;
	keys->next = self->keys;
	self->keys = keys;
}

	void
init__DeleteKeyBinding(self, class, inherit, keymap)
	struct init *self;
	char *class;
	boolean inherit;
	struct keymap *keymap;
{
	struct keys *keys, **prev;
	for (keys = self->keys, prev = &self->keys; 
			keys != NULL 
			&& ((strcmp(class, keys->class) != 0)
			   || keys->keymap != keymap) ; 
		    prev = &keys->next, keys = keys->next)
		{}
	if (keys != NULL) {
		/* found the one to delete */
		*prev = keys->next;
		free (keys);
	}
}

	void
init__AddMenuBinding(self, class, inherit, menulist)
	struct init *self;
	char *class;
	boolean inherit;
	struct menulist *menulist;
{
	struct menus *menus;
	menus = (struct menus *)malloc(sizeof(struct menus));
	menus->class = (char *)malloc(strlen(class) + 1);
	strcpy(menus->class, class);
	menus->inherit = inherit;
	menus->menulist = menulist;
	menus->next = self->menus;
	self->menus = menus;
}

	void
init__DeleteMenuBinding(self, class, inherit, menulist)
	struct init *self;
	char *class;
	boolean inherit;
	struct menulist *menulist;
{
	struct menus *menus, **prev;
	for (menus = self->menus, prev = &self->menus; 
			menus != NULL 
			&& ((strcmp(class, menus->class) != 0)
			   || menus->menulist != menulist) ; 
		    prev = &menus->next, menus = menus->next)
		{}
	if (menus != NULL) {
		/* found the one to delete */
		*prev = menus->next;
		free (menus);
	}
}


