/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#include <class.h>

#include <cursor.ih>
#include <envrment.ih>
#include <fontdesc.ih>
#include <graphic.ih>
#include <keymap.ih>
#include <keystate.ih>
#include <menulist.ih>
#include <nstdmark.ih>
#include <proctbl.ih>
#include <style.ih>
#include <text.ih>
#include <textv.ih>
#include <view.ih>

#include <panel.eh>

static struct keymap *classKeymap;
static struct style *defaultHighlightStyle;
static struct style *defaultOverallStyle;
static struct fontdesc *defaultIconFont = NULL;
static char defaultIcon;

/*
 * Statics
 */

static void DestroyPanelList(pe)
register struct panel_Entry *pe;
{
    while (pe != NULL) {
        register struct panel_Entry *ne;
        ne = pe->next;
	free(pe->tag);
        free(pe);
        pe = ne;
    }
}

static void DestroyKeyList(ke)
register struct key_Entry *ke;
{
    while (ke != NULL) {
        register struct key_Entry *ne;
        ne = ke->next;
        free(ke);
        ke = ne;
    }
}

static void ClearHighlight(self)
register struct panel *self;
{
    register long pos, len;

    if (self->highlightEnv == NULL)
        return;

    pos = environment_Eval(self->highlightEnv);
    len = environment_GetLength(self->highlightEnv);

    environment_Delete(self->highlightEnv);
    text_RegionModified(self->text, pos, len);

    self->highlightEnv = NULL;
    self->highlightEntry = NULL;
}

static void SetupHighlight(self, entry)
register struct panel *self;
struct panel_Entry *entry;
{
    self->highlightEnv =
      text_AddStyle(self->text,
        entry->pos, entry->len,
          self->highlightStyle);

    self->highlightEntry = entry;
}

static void SelectAtPos(self, pos)
struct panel *self;
long pos;
{
    register struct panel_Entry *pe;

    for (pe = self->panelList; pe != NULL; pe = pe->next)
        if (pos >= pe->pos && pos <= pe->pos + pe->len)
            break;

    if (pe == NULL)
        return;

    ClearHighlight(self);
    SetupHighlight(self, pe);

    panel_SetDotPosition(self, pe->pos);
    panel_FrameDot(self, pe->pos);
    panel_SetDotLength(self, 0);
    panel_WantUpdate(self, self);
    panel_LoseInputFocus(self);

    (*self->handler)(self->globalTag, pe->tag, self);
}

static void KeyDispatch(self, rock)
struct panel *self;
long rock;
{
    struct key_Entry *k = self->keyList;
    char c = (char) rock;

    while (k != NULL) {
        if (k->key == c)
            break;
        k = k->next;
    }

    if (k == NULL)
        return;

    (*k->proc)(k->rock, self, c);
}

static void ProcNext(rock, self, c)
long rock;
struct panel *self;
char c;
{
    panel_SelectNext(self);
}

static void ProcPrev(rock, self, c)
long rock;
struct panel *self;
char c;
{
    panel_SelectPrevious(self);
}

/*
 * Class Procedures
 */

boolean panel__InitializeClass(classID)
struct classheader *classID;
{
    defaultHighlightStyle = style_New();
    style_AddNewFontFace(defaultHighlightStyle, fontdesc_Bold);

    defaultOverallStyle = style_New();
    style_SetJustification(defaultOverallStyle, style_LeftJustified);
    style_SetFontSize(defaultOverallStyle, style_ConstantFontSize, 10);
    style_SetNewLeftMargin(defaultOverallStyle, style_LeftMargin,             16384, style_Inches);
    style_SetNewIndentation(defaultOverallStyle, style_LeftEdge,             -16384, style_Inches);

    defaultIconFont = fontdesc_Create("icon", fontdesc_Plain, 12);
    defaultIcon = 'R';

    classKeymap = keymap_New();

    {
        struct proctable_Entry *pte;
        unsigned char s[2];

        pte = proctable_DefineProc("key-dispatch",
          KeyDispatch, &panel_classinfo, NULL, NULL);

        s[1] = '\0';
        for (s[0] = '\0'; s[0] < 128; s[0]++)
            keymap_BindToKey(classKeymap, s, pte, s[0]);
    }

    return TRUE;
}

boolean panel__InitializeObject(classID, self)
struct classheader *classID;
register struct panel *self;
{
    self->panelList = NULL;
    self->keyList = NULL;
    self->keystate = keystate_Create(self, classKeymap);
    self->text = text_New();
    self->handler = NULL;
    self->iconFont = defaultIconFont;
    self->icon = defaultIcon;
    self->cursor = cursor_Create(self);
    self->highlightStyle = defaultHighlightStyle;
    self->highlightEntry = NULL;
    self->highlightEnv = NULL;

    panel_SetBorder(self, 5, 5);
    panel_SetDefaultStyle(self, defaultOverallStyle);
    panel_SetDataObject(self, self->text);

    panel_AssignKey(self, 'P' - 64, ProcPrev, 0);
    panel_AssignKey(self, 'P', ProcPrev, 0);
    panel_AssignKey(self, 'p', ProcPrev, 0);
    panel_AssignKey(self, 'B', ProcPrev, 0);
    panel_AssignKey(self, 'b', ProcPrev, 0);
    panel_AssignKey(self, 'N' - 64, ProcNext, 0);
    panel_AssignKey(self, 'N', ProcNext, 0);
    panel_AssignKey(self, 'n', ProcNext, 0);
    panel_AssignKey(self, 'F', ProcNext, 0);
    panel_AssignKey(self, 'f', ProcNext, 0);

    return TRUE;
}

void panel__FinalizeObject(self)
register struct panel *self;
{
    ClearHighlight(self);   /* clears env */

    DestroyPanelList(self->panelList);
    DestroyKeyList(self->keyList);

    keystate_Destroy(self->keystate);
    text_Destroy(self->text);
    fontdesc_Destroy(self->iconFont);
    cursor_Destroy(self->cursor);
    style_Destroy(self->highlightStyle);
}

/*
 * Methods
 */

struct panel_Entry *panel__Add(self, item, tag, showNow)
register struct panel *self;
char *item;
char *tag;
int showNow;			/* make new selection visible now? */
{
    register struct panel_Entry *new;
    register long len;
    register long textlen;
    register struct text *text = self->text;
    char c = '\n';

    new = (struct panel_Entry *) malloc(sizeof (struct panel_Entry));
    if (!new)
	return (struct panel_Entry *)NULL;

    len = strlen(item);
    textlen = text_GetLength(text);

    new->pos = textlen;
    new->tag = tag;
    new->len = len;
    new->next = self->panelList;

    self->panelList = new;

    text_AlwaysInsertCharacters(text, textlen, item, len);
    text_AlwaysInsertCharacters(text, textlen + len, &c, 1);
    if (showNow)
	panel_FrameDot(self, textlen);
    text_NotifyObservers(text, 0);

    return new;
}

void panel__Remove(self, entry)
register struct panel *self;
register struct panel_Entry *entry;
{
    register long len;
    register struct panel_Entry *pe, **le;

    /* Find and unlink from list */

    le = &self->panelList;
    for (pe = *le; pe; le = &pe->next, pe = *le)
        if (pe == entry)
            break;

    if (pe == NULL)
        return;     /* Invalid entry */

    *le = entry->next;

    /* Remove from display and deallocate */

    len = entry->len + 1;
    text_AlwaysDeleteCharacters(self->text, entry->pos, len);
    panel_WantUpdate(self, self);

    for (pe = self->panelList; pe != NULL; pe = pe->next)
        if (pe->pos >= entry->pos)
            pe->pos -= len;

    free(entry->tag);
    free(entry);
}

void panel__RemoveAll(self)
struct panel *self;
{
    ClearHighlight(self);
    DestroyPanelList(self->panelList);
    self->panelList = NULL;
    text_Clear(self->text);
    panel_WantUpdate(self, self);
}

void panel__SelectNext(self)
struct panel *self;
{
    long pos;

    if (self->highlightEnv == NULL)
        return;

    pos = environment_Eval(self->highlightEnv) +
      environment_GetLength(self->highlightEnv) + 1;

    SelectAtPos(self, pos);     /* Handles end of doc okay. */
}

void panel__SelectPrevious(self)
struct panel *self;
{
    long pos;

    if (self->highlightEnv == NULL)
        return;

    pos = environment_Eval(self->highlightEnv) - 2;

    SelectAtPos(self, pos);     /* Handles beg. of doc okay. */
}

void panel__ClearSelection(self)
struct panel *self;
{
    ClearHighlight(self);
    panel_WantUpdate(self, self);
}

void panel__MakeSelection(self, entry)
register struct panel *self;
register struct panel_Entry *entry;
{
    ClearHighlight(self);
    SetupHighlight(self, entry);

    if (entry == (struct panel_Entry *)NULL)
	return;
    panel_SetDotPosition(self, entry->pos);
    panel_SetDotLength(self, 0);
    panel_LoseInputFocus(self);
    panel_WantUpdate(self, self);
}

void panel__AssignKey(self, c, proc, rock)
struct panel *self;
char c;
void (*proc)();
long rock;
{
    struct key_Entry *k;

    k = self->keyList;

    while (k != NULL) {
        if (k->key == c)
            break;
        k = k->next;
    }

    if (k == NULL) {
        k = (struct key_Entry *) malloc(sizeof (struct key_Entry));
        k->next = self->keyList;
        self->keyList = k;
    }

    k->key = c;
    k->proc = proc;
    k->rock = rock;
}

/*
 * Overrides
 */

void panel__FullUpdate(self, type, x, y, w, h)
register struct panel *self;
enum view_UpdateType type;
long x, y, w, h;
{
    register struct graphic *graphic;

    graphic = panel_GetDrawable(self);
    cursor_SetGlyph(self->cursor, self->iconFont, self->icon);
    panel_PostCursor(self, &graphic->visualBounds, self->cursor);

    super_FullUpdate(self, type, x, y, w, h);
}

void panel__PostMenus(self, ml)
struct panel *self;
struct menulist *ml;
{
    /* Discard child menu postings */
}

void panel__PostKeyState(self, ks)
struct panel *self;
struct keystate *ks;
{
    /* Post our own keystate, discarding keystate from child */

    self->keystate->next = NULL;
    view_PostKeyState(self->header.view.parent, self->keystate);
}

struct view *panel__Hit(self, action, x, y, numberOfClicks)
register struct panel *self;
enum view_MouseAction action;
long x, y, numberOfClicks;
{
    if (self->handler == NULL)
        return NULL;

    super_Hit(self, action, x, y, numberOfClicks);

    SelectAtPos(self, panel_GetDotPosition(self));
    panel_SetDotLength(self, 0);
    panel_LoseInputFocus(self);
    return (struct view *)NULL;
}
