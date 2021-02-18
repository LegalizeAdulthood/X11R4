/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/textv.c,v 2.16 89/10/05 11:22:14 cfe Exp $ */
/* $ACIS:textv.c 1.7$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/textv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/textv.c,v 2.16 89/10/05 11:22:14 cfe Exp $";
#endif /* lint */

#include <class.h>
#include <ctype.h>

#include <dict.ih>
#include <environ.ih>
#include <envrment.ih>
#include <fontdesc.ih>
#include <graphic.ih>
#include <keystate.ih>
#include <mark.ih>
#include <matte.ih>
#include <menulist.ih>
#include <message.ih>
#include <rectlist.ih>
#include <scroll.ih>
#include <style.ih>
#include <stylesht.ih>
#include <text.ih>
#include <txttroff.ih>
#include <view.ih>
#include <viewref.ih>

#include <txtvinfo.h>
#include <textv.eh>

static struct graphic *pat;

#define TEXT_VIEWREFCHAR '\377'  /* place holder character for viewrefs */

#define NLINES 200    /* This set to keep redisplay from going into an infinite loop if something goes drastically wrong */
#define BX 20
#define BY 5
#define EBX 2
#define EBY 2
#define BADCURPOS -1
#define MAXPARA 200
#define REMOVEDCURSOR 32765
#define TABBASE ((int) 'n')

static struct fontdesc *iconFont = NULL;

static struct keymap *textviewKeymap;
static struct menulist *viewMenus;
static boolean initialExposeStyles;

extern void textview__LookCmd(); /* Needed for menulist functions. */

/* Scroll stuff. */
static void getinfo(), setframe();
static long whatisat();
static struct scrollfns scrollInterface = {getinfo, setframe, NULL, whatisat};

#define Text(self) ((struct text *) ((self)->header.view.dataobject))

boolean textview__InitializeObject(classID, self)
    struct classheader *classID;
    struct textview *self;
{
    long fontSize = 12;
    char bodyFont[100];
    char *font;
    long fontStyle = fontdesc_Plain;
    struct style *defaultStyle;
    boolean justify;

    self->displayLength = 0;
    self->hasInputFocus = FALSE;
    self->dot = NULL;
    self->top = NULL;
    self->frameDot = NULL;
    self->force = FALSE;
    self->nLines = 0;
    self->aLines = 0;
    self->lines = NULL;
    self->bx = BX;
    self->by = BY;
    self->ebx = EBX;
    self->eby = EBY;
    self->hasApplicationLayer = FALSE;
    self->keystate = keystate_Create(self, textviewKeymap);
    self->styleMenus = NULL;
    self->menus = menulist_DuplicateML(viewMenus, self);
    menulist_SetMask(self->menus, textview_NoMenus);
    self->clearFrom = 99999999;
    self->csxPos = BADCURPOS;
    self->csyPos = BADCURPOS;
    self->cshPos = BADCURPOS;
    self->csbPos = BADCURPOS;
    self->cexPos = BADCURPOS;
    self->ceyPos = BADCURPOS;
    self->cehPos = BADCURPOS;
    self->cebPos = BADCURPOS;
    self->scroll = textview_NoScroll;
    self->scrollLine = 0;
    self->scrollDist = -1;
    self->needUpdate = FALSE;
    self->lastStyleMenuVersion = -1;
    self->atMarker = NULL;
    self->displayEnvironment = NULL;
    self->displayEnvironmentPosition = 0;
    self->exposeStyles = initialExposeStyles;
    self->tabWidth = 1;
    self->predrawn = self->prepredrawn = NULL;
    self->predrawnY = self->predrawnX = -1;

    defaultStyle = style_New();
    style_SetName(defaultStyle, "default");

    if ((font = environ_GetProfile("bodyfont")) == NULL || ! fontdesc_ExplodeFontName(font, bodyFont, sizeof(bodyFont), &fontStyle, &fontSize)) {
	strcpy(bodyFont, "Andy");
    }
    justify = environ_GetProfileSwitch("justified", TRUE);
    style_SetFontFamily(defaultStyle, bodyFont);
    style_SetFontSize(defaultStyle, style_ConstantFontSize, fontSize);
    style_AddNewFontFace(defaultStyle, fontStyle);
    if (! justify)
        style_SetJustification(defaultStyle, style_LeftJustified);
    textview_SetDefaultStyle(self,defaultStyle);
    return TRUE;
}

void textview__LinkTree(self, parent)
struct textview *self;
struct view *parent;
{

    int cnt;
    super_LinkTree(self, parent);
    if((cnt = dictionary_CountRefs(self)) > 0){
        char **list;
	struct viewref **vr;
	struct view *view;
	if((list = (char **)malloc(cnt * sizeof(char *))) != NULL){
	    dictionary_ListRefs(self,list,cnt);
	    for(vr = (struct viewref **)list; cnt--; vr++){
		view = (struct view *) dictionary_LookUp(self,(char *)*vr);
		if(view){
		    view_LinkTree(view, self);
		}
	    }
            free(list);
        }
    }
}

void textview__FinalizeObject(classID, self)
    struct classheader *classID;
    struct textview *self;
{
    struct text *t = Text(self);
    int cnt;
    if((cnt = dictionary_CountRefs(self)) > 0){
        char **list;
	struct viewref **vr;
	struct view *view;
	if((list = (char **)malloc(cnt * sizeof(char *))) != NULL){
	    dictionary_ListRefs(self,list,cnt);
	    for(vr = (struct viewref **)list; cnt--; vr++){
		view = (struct view *) dictionary_LookUp(self,(char *)*vr);
		if(view) view_Destroy(view);
		dictionary_Delete(self,(char *)*vr);
		viewref_RemoveObserver(*vr,self);
	    }
            free(list);
        }
    }

    if(t!=NULL){
	if (self->dot != NULL)
	    text_RemoveMark(t, self->dot);
	if (self->top != NULL)
	    text_RemoveMark(t, self->top);
	if (self->frameDot != NULL)
	    text_RemoveMark(t, self->frameDot);
	if (self->predrawn != NULL)
	    text_RemoveMark(t, self->dot);
	if (self->prepredrawn != NULL)
	    text_RemoveMark(t, self->frameDot);
    }

    style_Destroy(self->defaultStyle);
    keystate_Destroy(self->keystate);
}

void textview__ObservedChanged(self, changed, value)
    struct textview *self;
    struct observable *changed;
    long value;
{
    struct view *vself = (struct view *) self;
    if (changed == (struct observable *) vself->dataobject)  {
	if (value == observable_OBJECTDESTROYED)
	    vself->dataobject = NULL;
	else
	    view_WantUpdate(vself, vself);
    }
    else if (value == observable_OBJECTDESTROYED &&
           (class_IsTypeByName(class_GetTypeName(changed), "viewref"))) {

               struct view *vw;
               struct viewref *vr = (struct viewref *)changed;
               if((vw = (struct view *)dictionary_LookUp(self,(char *)vr)) != NULL){
		   view_UnlinkTree(vw);
		   view_Destroy(vw); 
                   dictionary_Delete(self,(char *)vr);
               }
           }
}

void textview__WantUpdate(self, requestor)
    struct textview *self;
    struct view *requestor;
{
    if ((struct view *)self == requestor && self->needUpdate)
        return;

    super_WantUpdate(self, requestor);

    if ((struct view *)self == requestor)
        self->needUpdate = TRUE;
}

void textview__SetDataObject(self, dataObject)
    struct textview *self;
    struct dataobject *dataObject;
{
    if (!class_IsTypeByName(class_GetTypeName(dataObject), "text"))  {
	fprintf(stderr, "Incompatible dataobject associated with textview\n");
	return;
    }

    super_SetDataObject(self, dataObject);
    self->dot = text_CreateMark((struct text *) dataObject, 0, 0);
    self->top = text_CreateMark((struct text *) dataObject, 0, 0);
    self->frameDot = text_CreateMark((struct text *) dataObject, -1, 0);
    /* The frameDot must start at -1 because that is later interpreted
          (near the end of DoUpdate) as meaning that it has never been set */
    self->atMarker = text_CreateMark((struct text *) dataObject, 0, 0);

    self->predrawn = text_CreateMark((struct text *) dataObject, 0, 0);
    self->prepredrawn = text_CreateMark((struct text *) dataObject, 0, 0);
    mark_SetStyle(self->predrawn, TRUE, FALSE);
    mark_SetStyle(self->prepredrawn, TRUE, FALSE);

    mark_SetStyle(self->dot, FALSE, FALSE);
    mark_SetStyle(self->top, FALSE, FALSE);
    mark_SetStyle(self->frameDot, FALSE, FALSE);
    mark_SetStyle(self->atMarker, FALSE, FALSE);
    self->lastStyleMenuVersion = -1;
    menulist_SetMask(self->menus, textview_NoMenus);
    self->movePosition = -1;
}

struct view *textview__GetApplicationLayer(self)
    struct textview *self;
{

    self->hasApplicationLayer = TRUE;
     return (struct view *) scroll_Create(self, scroll_LEFT);
}

void textview__DeleteApplicationLayer(self, scrollbar)
    struct textview *self;
    struct scroll *scrollbar;
{
    self->hasApplicationLayer = FALSE;
    scroll_Destroy(scrollbar);
}

static void EnsureSize(self, lines)
    struct textview *self;
    long lines;
{
    register int i, j;
    register long nSize;

    if (lines < self->aLines) return;
    i = (lines > 10) ? (lines<<1) : lines + 2;
    nSize = i * (sizeof(struct linedesc));
    self->lines = (struct linedesc *) ((self->lines != NULL) ? realloc(self->lines, nSize) : malloc(nSize));
    for(j=self->aLines; j<i; j++)  {
	self->lines[j].y = 0;
	self->lines[j].height = 0;
	self->lines[j].containsView = FALSE;
	self->lines[j].data = text_CreateMark(Text(self),0,0);
    }
    self->aLines = i;
}

/* Updates the cursor.  If oldcursor is true then it adds the elements to the old rectangle list.  If it is false then it adds them to the new rectangle list and then at the end does an invert rectangle.
*/

static void UpdateCursor(self, oldCursor)
    struct textview *self;
    boolean oldCursor;
{
    int csx, csy, csh, csb, cex, cey, ceh, ceb, by;

    csx = self->csxPos; csy = self->csyPos;
    csh = self->cshPos; csb = self->csbPos;
    cex = self->cexPos; cey = self->ceyPos;
    ceh = self->cehPos; ceb = self->cebPos;

    if (csx == BADCURPOS && cex == BADCURPOS && ((mark_GetPos(self->top) < mark_GetPos(self->dot) || mark_GetPos(self->top) >= mark_GetPos(self->dot) + mark_GetLength(self->dot)) || ! self->hasInputFocus)) {
	if (oldCursor)
	    rectlist_ResetList();
	else
	    rectlist_InvertRectangles(self);
	return;
    }

    /* Handle start and end positions that lie offscreen. */

    if (csx == BADCURPOS)  {
	csx = 0;
	if (cey-ceh > 0)  {
	    /* ending is not on first line, fake a big first line. */

	    csy = cey-ceh;
	    csh = csy;
	    csb = 0;
	}
	else  {
	    /* ending is on first line, put beginning there too */

	    csy = cey;
	    csh = ceh;
	    csb = ceb;
	}
    }

    if (cex == BADCURPOS)  { 
	cex = textview_GetLogicalWidth(self) - 1;
	if (csy == textview_GetLogicalHeight(self))  {
	    /* beginning is at bottom, put ending there too-> */

	    cey = csy;
	    ceh = csh;
	    ceb = csb;
	}
	else  {
	    /* beginning not at bottom, fake big second line. */

	    cey = textview_GetLogicalHeight(self) - 1;
	    ceh = cey-csy;
	    ceb = 0;
	}
    }

    textview_SetFont(self,iconFont);
    textview_SetTransferMode(self,graphic_XOR);

    by = (self->hasApplicationLayer) ? self->by : self->eby;

    if (oldCursor)
	rectlist_ResetList();

    if (csx != cex || csy != cey)  {
	/* Fix up sometimes bogus x values.  We move them in an extra pixel if necessary to show an empty rectangle. */

	if (csx < 0) csx = 0;
	if (csx > textview_GetLogicalWidth(self) - 1) csx = textview_GetLogicalWidth(self)-1;
	if (cex < 0) cex = 0;
	if (cex > textview_GetLogicalWidth(self) - 1) cex = textview_GetLogicalWidth(self) - 1;
	if (cex == 0 && cey != csy)  {
	    ceh = ((cey = (cey - ceh)) == csy) ? csh : cey - csy;
	    cex = textview_GetLogicalWidth(self) - 1;
	}
	if (cey <= by)  {
	    self->cexPos = self->csxPos = BADCURPOS;
	    if (oldCursor)
		rectlist_ResetList();
	    else
		rectlist_InvertRectangles(self);
	    return;
	}

	/* Now figure out how to draw it. */

	if (csy == cey)  {
	    /* start and end are on same line */

	    if (oldCursor)
		rectlist_AddOldRectangle(csy, csy - csh, csx, cex);
	    else
		rectlist_AddNewRectangle(csy, csy - csh, csx, cex, 0);
	}
	else  { 
	    /* Box first line */

	    if (oldCursor)
		rectlist_AddOldRectangle(csy, csy - csh, csx, textview_GetLogicalWidth(self) - 1);
	    else
		rectlist_AddNewRectangle(csy, csy - csh, csx, textview_GetLogicalWidth(self) - 1, 0);

	    if (cey-ceh!=csy)  {
		/* Box middle lines */

		if (oldCursor)
		    rectlist_AddOldRectangle(cey - ceh, csy, 0,textview_GetLogicalWidth(self) - 1);
		else
		    rectlist_AddNewRectangle(cey - ceh, csy, 0,textview_GetLogicalWidth(self) - 1, 0);
	    }

	    /* Box last line */

	    if (oldCursor)
		rectlist_AddOldRectangle(cey, cey - ceh, 0, cex);
	    else
		rectlist_AddNewRectangle(cey, cey - ceh, 0, cex, 0);
	}
    }
    else  {
	textview_MoveTo(self,csx,csy-csb);
	textview_DrawString(self,"|",graphic_NOMOVEMENT);
    }
    if (! oldCursor)  {
	rectlist_InvertRectangles(self);
	self->csxPos = csx;
	self->csyPos = csy;
	self->cshPos = csh;
	self->csbPos = csb;
	self->cexPos = cex;
	self->ceyPos = cey;
	self->cehPos = ceh;
	self->cebPos = ceb;
    }
}

static void XorCursor(self)
    struct textview *self;
{
    rectlist_ResetList();
    UpdateCursor(self, FALSE);
}


/* This routine redraws that which must change.  The key concept is this: marks have a modified flag that can be cleared, and which is set whenever the text within a marker is modified.  A view consists of a set of lines and the marks representing the corresponding text.  When we are supposed to update a view, we redraw the lines which correspond to modified marks. *//* 

 *//* For each line that needs to be redrawn, we call the type-specific redrawing routine.  For now, we will call specific routines in the document handler, but this will change soon. */

static void DoUpdate(self, reformat)
    struct textview *self;
    boolean reformat;
{
    register struct mark *lob, *tob;
    register struct linedesc *tl;
    long lobChars;
    register int line;
    int textLength = text_GetLength(Text(self));
    int curx, cury, height, force, cStart, cEnd, mStart, mLength, csx;
    int csy, csh, csb, cex, cey, ceh, ceb, ysleft, xs, ys, t,stopline,redrawline;
    boolean cursorVisible = 0;
    boolean changed;
    int cont;		/* force continuation of redraw */
    enum textview_ScrollDirection scrolling = textview_NoScroll;	/* set to v->scroll if this is a scrolling update */
    boolean zapRest;
    struct rectangle tempSrcRect;
    struct point tempDstOrigin;	/* Temps for graphics operations */
    struct formattinginfo info;
    long textheight;
    long bx, by;
    
    if (self->lastStyleMenuVersion != Text(self)->styleSheet->version)  {
        /* Have to recompute the size of the tab character */
	
	text_InitStateVector(&info.sv);
	text_ApplyEnvironment(&info.sv, self->defaultStyle, Text(self)->rootEnvironment);
	info.sv.CurCachedFont = fontdesc_Create(info.sv.CurFontFamily,
				info.sv.CurFontAttributes, info.sv.CurFontSize);
	info.myWidths = fontdesc_WidthTable(info.sv.CurCachedFont, 	    textview_GetDrawable(self));
	self->tabWidth = info.myWidths[TABBASE];

	/* Now reset the menus for the new styles */

	if (self->styleMenus != NULL)
	    menulist_Destroy(self->styleMenus);
	self->styleMenus = menulist_DuplicateML(stylesheet_GetMenuList(((struct text *) Text(self))->styleSheet, textview__LookCmd, &textview_classinfo), self);
	self->lastStyleMenuVersion = ((struct text *) Text(self))->styleSheet->version;
	menulist_SetMask(self->menus, textview_NoMenus);
    }
    self->needUpdate = FALSE;
    if (iconFont == NULL)
	iconFont = fontdesc_Create("Icon",fontdesc_Plain,12);

    /* copy dimensions out of the view */

    by = cury = (self->hasApplicationLayer) ? self->by : self->eby;
    bx = curx = (self->hasApplicationLayer) ? self->bx : self->ebx;
    xs = textview_GetLogicalWidth(self) - 2 * bx;
    ys = textview_GetLogicalHeight(self) - 2 * by;

    lob = self->top;	/* the first char on the screen */
    lobChars = 0;
    mark_SetLength(lob,0);	/* ignore the length part, which can grow at random */
    force = self->force;

    if (self->scroll == textview_ScrollForward || self->scroll == textview_ScrollBackward)  {
	for (line = 0; line < self->nLines && ! mark_GetModified(self->lines[line].data); line++);
	if (line == self->nLines && line > 1)  {
	    /* At this point we know that only scrolling has occurred and we can just do the bit blt and continue formatting */

	    int sy, dy, h, newLine, movement, lasty, extendedTop = 0;

	    scrolling = self->scroll;

	    if (self->scroll == textview_ScrollForward)  {
		/* Scrolling toward the end of the document */

		/* Determine area that has to be moved */

                if(self->lines[self->nLines - 1].height > self->lines[self->nLines - 1].textheight) {
                    /* last line contains a view that will probably need a full redraw */
                    stopline = self->nLines - 2;
                    redrawline = stopline + 1;
                   mark_SetModified(self->lines[redrawline].data,TRUE); 
                }
                else{
                    stopline = self->nLines - 1;
                    redrawline = -1;
                }
                sy = self->lines[self->scrollLine].y;
		dy = self->lines[0].y;
/*		lasty = self->lines[self->nLines - 1].y + self->lines[self->nLines - 1].height; */
		lasty = self->lines[stopline].y + self->lines[stopline].height;
		if (self->ceyPos >= lasty && self->ceyPos == self->csyPos && self->cexPos == self->csxPos)  {
		    /* We are viewing the end of the file with the carat at the end. */
		    
		    lasty = cury + ys;
		}
		h = lasty - sy;
		movement = sy - dy;
		
		/* Remove Cursor if it is on the scroll-1 line */

		if (cursorVisible && mark_GetLength(self->dot) == 0 && 
                     mark_GetPos(self->dot) >= mark_GetPos(self->lines[self->scrollLine-1].data) && mark_GetPos(self->dot) < mark_GetPos(self->lines[self->scrollLine].data)) {
		    textview_SetTransferMode(self,graphic_XOR);
		    XorCursor(self);
                    }

		textview_SetTransferMode(self,graphic_COPY);
		rectangle_SetRectSize(&tempSrcRect,0,sy,textview_GetLogicalWidth(self),h);
		point_SetPt(&tempDstOrigin,0,dy);
		textview_BitBlt(self, &tempSrcRect, self,&tempDstOrigin, (struct rectangle *) NULL);

		rectangle_SetRectSize(&tempSrcRect,0,dy+h,textview_GetLogicalWidth(self),movement);
		pat = textview_WhitePattern(self);
		textview_FillRect(self,&tempSrcRect,pat);
		
                /* remove scrolled off views */
                for(line = 0; line < self->scrollLine; line++)
                    if(self->lines[line].containsView){
                        textview_ViewMove(self,&self->lines[line],textview_REMOVEVIEW);
  /*                      mark_SetModified(self->lines[line].data, TRUE); */
                    }

		for (newLine = 0, line = self->scrollLine; line <= stopline; line++, newLine++)  {
		    mark_SetPos(self->lines[newLine].data,  mark_GetPos(self->lines[line].data));
		    mark_SetLength(self->lines[newLine].data, mark_GetLength(self->lines[line].data));
		    mark_SetStyle(self->lines[newLine].data, mark_IncludeBeginning(self->lines[line].data), mark_IncludeEnding(self->lines[line].data));
		    self->lines[newLine].nChars = self->lines[line].nChars;
		    mark_SetModified(self->lines[newLine].data, FALSE);
		    mark_SetObjectFree(self->lines[newLine].data, mark_ObjectFree(self->lines[line].data));
		    self->lines[newLine].y = self->lines[line].y - movement;
		    self->lines[newLine].height = self->lines[line].height;
		    self->lines[newLine].xMax = self->lines[line].xMax;
		    self->lines[newLine].containsView = self->lines[line].containsView;
		    if (self->lines[newLine].containsView)  {
			/* Put code for moving inset here */
                       textview_ViewMove(self,&self->lines[newLine],-movement);
  /*                     cont = 0;
                        textview_LineRedraw(self, textview_MoveView, self->lines[newLine].data, curx, self->lines[newLine].y , xs, ysleft, force, &cont, &info); */
		    }
		}
		self->nLines -= self->scrollLine;
		if (self->csxPos != BADCURPOS)  {
		    extendedTop = (self->csyPos - self->cshPos) < by;
		    if (self->ceyPos > lasty)  {
			self->cehPos -= self->ceyPos - lasty;
			self->ceyPos = lasty;

			rectangle_SetRectSize(&tempSrcRect,0,lasty,textview_GetLogicalWidth(self), textview_GetLogicalHeight(self) - lasty);
			pat = textview_WhitePattern(self);
			textview_FillRect(self,&tempSrcRect, pat);
		    }
		    self->csyPos -= movement;
		    self->ceyPos -= movement;
		    if (self->ceyPos - self->cehPos < dy)
			self->cehPos = self->ceyPos - dy;
		    if (self->ceyPos <= by)  {
			if (extendedTop)  {
			    /* clear out the top part of the selection box */

			    rectangle_SetRectSize(&tempSrcRect, 0, 0, textview_GetLogicalWidth(self), by);
			    pat = textview_WhitePattern(self);
			    textview_FillRect(self, &tempSrcRect, pat);
			}
			self->csyPos = BADCURPOS;
			self->csxPos = BADCURPOS;
			self->ceyPos = BADCURPOS;
			self->cexPos = BADCURPOS;
		    }
		    else if ((self->csyPos - self->cshPos) < by)  {
			if (! extendedTop)  {
			    /* Just moved into the border, so color it black */
			    rectangle_SetRectSize(&tempSrcRect,0,0,
				textview_GetLogicalWidth(self),by);
			    pat = textview_BlackPattern(self);
			    textview_FillRect(self,&tempSrcRect, pat);

			}
			self->csyPos = by;
			self->csxPos = 0;
			self->cshPos = by;
		    }
		}
		if (cursorVisible && redrawline >=0 && mark_GetPos(self->dot) >= mark_GetPos(self->lines[redrawline].data)){
                    /* if cursor is on the view line that need updated */
                        rectlist_ResetList();
			self->csyPos = BADCURPOS;
			self->csxPos = BADCURPOS;
			self->ceyPos = BADCURPOS;
			self->cexPos = BADCURPOS;
                }
		cursorVisible = self->csxPos != BADCURPOS || self->cexPos != BADCURPOS;
	    }
	    else  {
		/* Scrolling toward the beginning of the document */

		int lastLine;

		sy = self->lines[0].y;
		movement = self->scrollDist;
		dy = sy + self->scrollDist;
		lasty = textview_GetLogicalHeight(self) - by;
		h = 0;
		
		/* determine the height of the area to raster op and the number of lines that will be moved. */

		for (lastLine = 0; lastLine < self->nLines; lastLine++)  {
		    if (dy + h + self->lines[lastLine].height > lasty) break;
		    h += self->lines[lastLine].height;
		}

		textview_SetTransferMode(self, graphic_COPY);
		rectangle_SetRectSize(&tempSrcRect,0,sy,textview_GetLogicalWidth(self),h);
		point_SetPt(&tempDstOrigin,0,dy);
		textview_BitBlt(self,&tempSrcRect,self,&tempDstOrigin, (struct rectangle *) NULL);

		rectangle_SetRectSize(&tempSrcRect,0,0, textview_GetLogicalWidth(self),dy);
		pat = textview_WhitePattern(self);
		textview_FillRect(self,&tempSrcRect,pat);

		rectangle_SetRectSize(&tempSrcRect, 0, dy+h, textview_GetLogicalWidth(self), textview_GetLogicalHeight(self) - (dy + h));
		pat = textview_WhitePattern(self);
		textview_FillRect(self,&tempSrcRect,pat);

                newLine = (self->aLines > self->nLines)? self->nLines + 1 :self->nLines ;
		self->nLines = self->scrollLine + lastLine;
		EnsureSize(self, self->nLines);

                /* remove scrolled off views */
                for(line = lastLine + 1; line < newLine ; line++)
                    if(self->lines[line].containsView){
                        textview_ViewMove(self,&self->lines[line],textview_REMOVEVIEW);
/*                        mark_SetModified(self->lines[line].data, TRUE); */
                    }

		for (newLine = self->nLines - 1, line = lastLine-1; 	line >= 0; line--, newLine--)  {
		    mark_SetPos(self->lines[newLine].data,  mark_GetPos(self->lines[line].data));
		    mark_SetLength(self->lines[newLine].data, mark_GetLength(self->lines[line].data));
		    mark_SetStyle(self->lines[newLine].data, mark_IncludeBeginning(self->lines[line].data), mark_IncludeEnding(self->lines[line].data));
		    self->lines[newLine].nChars = self->lines[line].nChars;
		    mark_SetModified(self->lines[newLine].data, FALSE);
		    mark_SetObjectFree(self->lines[newLine].data, mark_ObjectFree(self->lines[line].data));
		    self->lines[newLine].y = self->lines[line].y + movement;
		    self->lines[newLine].height = self->lines[line].height;
		    self->lines[newLine].xMax = self->lines[line].xMax;
		    self->lines[newLine].containsView = self->lines[line].containsView;
		    if (self->lines[newLine].containsView)  {
			/* Put code for moving inset here */
                        textview_ViewMove(self,&self->lines[newLine],movement);
		    }
		}
		
		if (self->csxPos != BADCURPOS)  {
		    if (self->csyPos - self->cshPos < by)
			self->cshPos = self->csyPos - by;
		    self->csyPos += movement;
		    self->ceyPos += movement;
		    if (self->csyPos > (dy + h))  {
			if (self->csyPos - self->cshPos >= dy + h)  {
			    self->csxPos = BADCURPOS;
			    self->csyPos = BADCURPOS;
			    self->cexPos = BADCURPOS;
			    self->ceyPos = BADCURPOS;
			}
			else  {
			    self->cshPos -= self->csyPos - (dy + h);
			    self->csyPos = dy + h;
			}
		    }
		    if (self->ceyPos > dy + h)  {
			if (self->csxPos != BADCURPOS)  {
			    self->cexPos = textview_GetLogicalWidth(self) - 1;
			    self->ceyPos = dy + h;
			    self->cehPos = (self->ceyPos == self->csyPos) ? self->cshPos : (self->ceyPos - self->csyPos);
			}
		    }
		}
	    }
	}
	else  {
	    /* Both scrolling and some text has changed.  Have no choice but to redraw the entire view. */
	    self->force = 1;
	}
    }
    self->scroll = textview_NoScroll;
    self->scrollDist = -1;

    cursorVisible = self->csxPos != BADCURPOS || self->cexPos != BADCURPOS;

    if (reformat && self->force)  {
/* 	the reason for doing a full-screen clear is that redrawing the
	   screen with one clear is about twice as fast as doing it line by line->  Sigh. */

	self->force = 0;

	/* Zap cursor since it may spill into other insets. */

	if (cursorVisible)  { 
	    XorCursor(self);
	    cursorVisible = FALSE;
	}
	line = 0;
	while (line < self->nLines)  {
            if(self->lines[line].containsView)
                textview_ViewMove(self,&self->lines[line],textview_REMOVEVIEW); 
	    mark_SetModified(self->lines[line++].data, TRUE);
	} 

	textview_SetTransferMode(self, graphic_COPY);
	textview_GetVisualBounds(self,&tempSrcRect);
	pat = textview_WhitePattern(self);
	textview_FillRect(self,&tempSrcRect,pat);
    }

    cStart = mark_GetPos(self->dot);
    cEnd = cStart + mark_GetLength(self->dot);
    textview_SetTransferMode(self,graphic_COPY);
    csx = BADCURPOS;
    csy = BADCURPOS;
    cex = BADCURPOS;
    cey = BADCURPOS;
    csh = 0; csb = 0; ceh = 0; ceb = 0;
    ysleft=ys;
    zapRest=TRUE;
    line = 0;
    cont = 0;

    while (line < NLINES)  {	/* for each potential line on the screen */
	EnsureSize(self, line);
	tl = &(self->lines[line]);
	tob = tl->data;

	/* move next mark to follow last redisplayed line, if necessary
	this sets the modified flag on the mark iff a change was performed
	inline expansion of doc_makefollow(lob,tob) */

	if (reformat && mark_GetPos(tob) != mark_GetPos(lob) + lobChars)  {
/*            if(tl->containsView)
                textview_ViewMove(self,tl,textview_REMOVEVIEW); */
	    mark_SetPos(tob, mark_GetPos(lob) + lobChars);
	    mark_SetLength(tob, 0);
	    mark_SetModified(tob, 1);
	}

	/* next check to see if the line must be redrawn */

	if (reformat && ((cont && scrolling != textview_ScrollBackward) || line >= self->nLines || mark_GetModified(tob) || cury != tl->y || (scrolling == textview_ScrollBackward && line < self->scrollLine)))  {
	    /* zap cursor if need to */

	    if (cursorVisible && self->ceyPos >= cury && (! scrolling || (self->csxPos == self->cexPos && self->csyPos == self->ceyPos && self->ceyPos == self->lines[self->nLines - 1].y + self->lines[self->nLines - 1].height))) {
		XorCursor(self);
		cursorVisible = FALSE;
	    }

#if 1
	    height = textview_LineRedraw(self, textview_FullLineRedraw, tob, curx, cury, xs, ysleft, force, &cont,&textheight, &info);
#else /* 1 */
	    height = textview_LineRedraw(self, textview_FullLineRedraw, tob, curx, cury, xs, ysleft, force, &cont,&tl->textheight, &info);
#endif /* 1 */
	    tl->containsView = (info.foundView != NULL);
            tl->height = height;	/* set the new length */
#if 1
            tl->textheight = textheight;
#endif /* 1 */
	    tl->nChars = info.lineLength;
	    tl->y = cury;
	    ysleft -= height;
	    mark_SetModified(tob,0);	/* clear the mod flag */
	}
	else  {
	    ysleft -= tl->height;
	    cont = 0;
	}
	mStart = mark_GetPos(tob);
	mLength = tl->nChars;

	if (reformat && cury + tl->textheight > ys + by)  {
	    cury += tl->height;
	    zapRest = FALSE;
	    break;
	}
	cury += tl->height;
	/* Now do the cursor computations.  Note that we try to
	   avoid recomputing the other end of the cursor if it is the same
	   point on the screen. */
	
	if (cStart >= mStart && (cStart < mStart+mLength || mStart + mLength == textLength) && self->hasInputFocus)  {
	    if (csx < 0)  {
		csx = textview_LineRedraw(self, textview_GetCoordinate, tob, curx, cury, xs, ys, cStart, NULL,NULL, &info);

                /* If we seem to be at the beginning of the line, and we aren't */
                /* going to draw a caret, adjust back to the inset boundary. */
                if (cStart == mStart && cEnd != cStart)
                    csx = 0;

		csy = cury;
		csb = info.lineBelow;
		csh = tl->height;
	    }
	}
	if (cEnd != cStart && self->hasInputFocus)  {
	    if (cEnd >= mStart && (cEnd < mStart+mLength || mStart + mLength == textLength))  {
		if (cex < 0)  {
		    cex = textview_LineRedraw(self, textview_GetCoordinate, tob, curx, cury, xs, ys, cEnd,NULL,NULL, &info);

                    /* If we seem to be at the beginning of the line, and we aren't */
                    /* going to draw a caret, adjust back to the inset boundary. */
                    if (cEnd == mStart && cEnd != cStart)
                        cex = 0;

		    cey=cury;
		    ceb= info.lineBelow;
		    ceh=tl->height;
		}
	    }
	}
	else  {
	    cex=csx;
	    cey=csy;
	}

	/* markatend is checked here, since one null line should be redrawn in all
	   redisplays, since this null line could contain the cursor, should it be at the end of the buffer. */
      
	line++;
	if (mark_GetPos(tob) >= textLength) break;
	lob = tob;		/* last mark object */
	lobChars = self->lines[line-1].nChars;
    }

    if (reformat && (self->zapRest = zapRest))  {
	/* consider zapping the rest of the screen */

	if (cury < self->clearFrom)  {
	    textview_SetTransferMode(self, graphic_COPY);
	    rectangle_SetRectSize(&tempSrcRect, 0, cury, textview_GetLogicalWidth(self), textview_GetLogicalHeight(self) - cury);
	    pat = textview_WhitePattern(self);
	    textview_FillRect(self,&tempSrcRect,pat);
	}
    }
    self->clearFrom = cury;

    self->nLines = line;

    /* now draw the cursor */

    changed =  ! (self->csxPos == csx && self->csyPos == csy
	    && self->cshPos == csh && self->csbPos == csb
	    && self->cexPos == cex && self->ceyPos == cey
	    && self->cehPos == ceh && self->cebPos == ceb);

    if (changed || ! cursorVisible)  {
	if (cursorVisible)  {
	    UpdateCursor(self, TRUE);
	}
	else
	    rectlist_ResetList();
	self->csxPos = csx;
	self->csyPos = csy;
	self->cshPos = csh;
	self->csbPos = csb;
	self->cexPos = cex;
	self->ceyPos = cey;
	self->cehPos = ceh;
	self->cebPos = ceb;
	UpdateCursor(self, FALSE);
    }

    self->displayLength = textLength;

    /* now check if we're supposed to force the cursor to be visible
    contains expansion of doc_setmarkpos */

    if ((line = mark_GetPos(self->frameDot)) != -1) {
	/* line is now the position to start back from */

	if (! textview_Visible(self, line)) {
	    mark_SetPos(self->frameDot, -1);		/* prevent recursive loops */
	    t = textview_MoveBack(self, line, textview_GetLogicalHeight(self)/3,
		textview_MoveByPixels, 0, 0);			/* our new top */
	    textview_SetTopPosition(self, t);			/* adjust the frame */
	}
	else mark_SetPos(self->frameDot, -1);
    }

/* Not the greatest place for this but... */                                             
    if (self->hasInputFocus) {

        long mask;
        boolean readonly = (textview_GetDotPosition(self) < text_GetFence(Text(self))) || text_GetReadOnly(Text(self));

        mask = ((mark_GetLength(self->dot) > 0) ? textview_SelectionMenus : textview_NoSelectionMenus) |
               (readonly ? 0 : textview_NotReadOnlyMenus);

        if (menulist_SetMask(self->menus, mask)) {
            if (readonly || !(mask & textview_SelectionMenus))
                menulist_UnchainML(self->menus, textview_StyleMenusKey);
            else
                menulist_ChainBeforeML(self->menus, self->styleMenus, textview_StyleMenusKey);
            textview_PostMenus(self, self->menus);
        }
    }
}

void textview__FullUpdate(self, type, left, top, width, height)
    struct textview *self;
    enum view_UpdateType type;
    long left;
    long top;
    long width;
    long height;
{
    long dotpos;

    self->force = TRUE;
    if (textview_Visible(self, (dotpos = textview_GetDotPosition(self))))  {
	textview_FrameDot(self, dotpos); 
    }
    self->csxPos = BADCURPOS;		/* Indication that cursor is not visible */
    self->cexPos = BADCURPOS;
    DoUpdate(self, TRUE);
}

void textview__WantNewSize(self,requestor)
struct textview *self;
struct view *requestor;
{
    self->force = TRUE;
    self->csxPos = BADCURPOS;		/* Indication that cursor is not visible */
    self->cexPos = BADCURPOS;
    textview_WantUpdate(self, self);
}

void textview__Update(self)
    struct textview *self;
{
    DoUpdate(self, TRUE);
}

struct view *textview__Hit(self, action, x, y, numberOfClicks)
    struct textview *self;
    enum view_MouseAction action;
    long x;
    long y;
    long numberOfClicks;
{
    int newPos, oldPos, oldLen, oldMid;
    struct view *vptr;
    static int leftStartPos;
    static int rightStartPos;
    int redrawCursor = 0;
    int newLeftPos ;
    int newRightPos;

    newPos = textview_Locate(self,x,y, &vptr);
    oldPos = mark_GetPos(self->dot);
    oldLen = mark_GetLength(self->dot);
    oldMid = oldPos + (oldLen>>1);

    if (action == view_LeftDown || action == view_RightDown)  {
	if (vptr != NULL)
	    return view_Hit(vptr, action, view_EnclosedXToLocalX(vptr, x), view_EnclosedYToLocalY(vptr, y), numberOfClicks);

 	if (! self->hasInputFocus)
	    textview_WantInputFocus(self, self);
	
	textview_GetClickPosition(self, newPos, numberOfClicks, action, oldPos, oldPos + oldLen, &newLeftPos, &newRightPos);
	
	if (action == view_LeftDown)  {
	    mark_SetPos(self->dot, newLeftPos);
	    mark_SetLength(self->dot, newRightPos - newLeftPos);
	    leftStartPos = newLeftPos;
	    rightStartPos = newRightPos;
	}
	else  {
	    int lPos = oldPos;
	    int rPos = oldPos + oldLen;
	    
	    if (numberOfClicks == 1)  {
		if (newPos < oldMid)  {
		    leftStartPos = rightStartPos = rPos;
		    lPos = newLeftPos;
		}
		else  {
		    leftStartPos = rightStartPos = lPos;
		    rPos = newRightPos;
		}
	    }
	    else  {
		if (newPos < leftStartPos)
		    lPos = newLeftPos;
		else if (newPos >= leftStartPos)    /* with multiple right clicks left and right start pos are the same */
		    rPos = newRightPos;
	    }
		
	    mark_SetPos(self->dot,lPos);
	    mark_SetLength(self->dot, rPos - lPos);
	}
    }
    else if (action == view_LeftMovement || action == view_RightMovement || (numberOfClicks == 1 && (action == view_LeftUp || action == view_RightUp)))  {
	int lPos = leftStartPos;
	int rPos = rightStartPos;

	textview_GetClickPosition(self, newPos, numberOfClicks, action, leftStartPos, rightStartPos, &newLeftPos, &newRightPos);
	redrawCursor = FALSE;

	if (newPos <= leftStartPos)
	    lPos = newLeftPos;
	else if (newPos >= rightStartPos)  
	    rPos = newRightPos;

	if (lPos != oldPos || rPos != oldPos + oldLen)  {
	    mark_SetPos(self->dot, lPos);
	    mark_SetLength(self->dot, rPos - lPos);
	    redrawCursor = TRUE;
	}
    }

    if (action == view_LeftMovement || action == view_RightMovement)  {
	if (redrawCursor)
	    DoUpdate(self, FALSE);
    }
    else
	textview_WantUpdate(self, self);

    return (struct view *) self;
}

void textview__ReceiveInputFocus(self)
    struct textview *self;
{
    self->hasInputFocus = TRUE;
    self->keystate->next = NULL;
    menulist_SetMask(self->menus, textview_NoMenus);
    textview_PostKeyState(self, self->keystate);
    textview_WantUpdate(self, self);
}

void textview__LoseInputFocus(self)
    struct textview *self;
{
    self->hasInputFocus = FALSE;
    textview_WantUpdate(self, self);
}

void textview__SetDotPosition(self, newPosition)
    struct textview *self;
    long newPosition;
{
    long len;

    if (newPosition < 0)
	newPosition = 0;
    else  {
	if (newPosition > (len = text_GetLength(Text(self))))
	    newPosition = len;
    }
    mark_SetPos(self->dot, newPosition);
    textview_WantUpdate(self, self);
	
}

void textview__SetDotLength(self, newLength)
    struct textview *self;
    long newLength;
{
    if (newLength < 0)
	newLength = 0;
    mark_SetLength(self->dot, newLength);
    textview_WantUpdate(self, self);
}

long textview__GetDotPosition(self)
    struct textview *self;
{
    return mark_GetPos(self->dot);
}

long textview__GetDotLength(self)
    struct textview *self;
{
    return mark_GetLength(self->dot);
}

long textview__GetTopPosition(self)
    struct textview *self;
{
    return mark_GetPos(self->top);
}

void textview__SetTopPosition(self, newTopPosition)
    struct textview *self;
    long newTopPosition;
{
    long len;
    long curTop;


    if (newTopPosition < 0)
	newTopPosition = 0;
    else  {
	if (newTopPosition > (len = text_GetLength(Text(self))))
	    newTopPosition = len;
    }
    
    if ((curTop = mark_GetPos(self->top)) != newTopPosition)  {
	long line;

	mark_SetPos(self->top, newTopPosition);
	if (curTop < newTopPosition)  {
	    if (self->scroll != textview_ScrollBackward)   {
		line = textview_FindLineNumber(self, newTopPosition);
		if (line != -1)  {
		    if (line == 0) return;
		    self->scroll = textview_ScrollForward;
		    self->force = FALSE;
		    self->scrollLine = line;
		}
		else  {
		    self->scroll = textview_NoScroll;
		    self->force = TRUE;
		}
	    }
	    else
		self->scroll = textview_MultipleScroll;
	}
	else  if (self->scroll != textview_ScrollForward)  {
	    if (self->scrollDist != -1)  {
		self->scroll = textview_ScrollBackward;
		self->force = FALSE;
	    }
	    else  {
		self->force = TRUE;
		self->scroll = textview_NoScroll;
	    }
	}
	else
	    self->scroll = textview_MultipleScroll;
    }
    if (self->scroll == textview_MultipleScroll)  {
	self->force = TRUE;
    }
    textview_WantUpdate(self, self);
}

void textview__SetBorder(self, xBorder, yBorder)
    struct textview *self;
    long xBorder;
    long yBorder;
{
    self->bx = xBorder;
    self->by = yBorder;
    
    /* Have to do some update here for resetting the border. */
}

void textview__SetEmbeddedBorder(self, xBorder, yBorder)
    struct textview *self;
    long xBorder;
    long yBorder;
{
    self->ebx = xBorder;
    self->eby = yBorder;
    
    /* Have to do some update here for resetting the border. */
}

void textview__CollapseDot(self)
    struct textview *self;
{
    long pos;
    
    pos = mark_GetPos(self->dot) + mark_GetLength(self->dot);
    mark_SetPos(self->dot, pos);
    mark_SetLength(self->dot, 0);
    textview_WantUpdate(self, self);
}

void textview__GetClickPosition(self, position, numberOfClicks, action, startLeft, startRight, leftPos, rightPos)
    struct textview *self;
    long position;
    long numberOfClicks;
    enum view_MouseAction action;
    long startLeft;
    long startRight;
    long *leftPos;
    long *rightPos;
{
    register int pos;
    struct text *text = Text(self);
    register int textLength = text_GetLength(text);
    int extEnd;

    switch (numberOfClicks % 3)  {
	case 1:
	    /* Single Click */

	    *leftPos = *rightPos = position;
	    break;
	    
	case 2:
	    /* Double Click - word select */

	    if (position < textLength && isalnum(text_GetChar(text,position)) != 0)  {
		/* Inside a word */

		pos = position - 1;
		for (pos = position - 1; pos >= 0 && isalnum(text_GetChar(text,pos)) != 0; pos--);
		*leftPos = ++pos;
		for (pos = position + 1; pos < textLength && isalnum(text_GetChar(text, pos)) != 0; pos++);
		*rightPos = pos;
	    }
	    else if (position > 0 && isalnum(text_GetChar(text, position - 1)) != 0)  {
		/* Right of Word */

		*rightPos = position;
		for (pos = position - 1; pos >= 0 && isalnum(text_GetChar(text, pos)) != 0; pos--);
		*leftPos = pos + 1;
	    }
	    else  {
		/* No word either side */
		
		if (action == view_LeftDown)  {
		    *rightPos = *leftPos = position;
		}
		else  {
		    if (position <= startLeft)  {
			for (pos = position + 1; pos < textLength && isalnum(text_GetChar(text, pos)) == 0; pos++);
			for (pos -= 1; pos >= 0 && isspace(text_GetChar(text, pos)) == 0; pos--);
			pos += 1;
		    }
		    else  {
			for (pos = position - 1; pos >= 0 && isalnum(text_GetChar(text, pos)) == 0; pos--);
			for (pos += 1; pos < textLength && isspace(text_GetChar(text, pos)) == 0; pos++);
		    }
		    *rightPos = *leftPos = pos;
		}
	    }
	    break;

	case 0:
	    /* Triple Click - Paragraph select */

	    extEnd = (action == view_LeftDown || action == view_LeftMovement);
	    if (position < textLength && text_GetChar(text,position) != '\n')  {
		/* Inside a paragraph */

		pos = position - 1;
		for (pos = position - 1; pos >= 0 && text_GetChar(text,pos) != '\n'; pos--);
		*leftPos = ++pos;
		for (pos = position + 1; pos < textLength && text_GetChar(text,pos) != '\n'; pos++);
		*rightPos = (extEnd && pos < textLength) ? pos + 1 : pos;
	    }
	    else if (position > 0 && text_GetChar(text,position - 1) != '\n')  {
		/* Right of a paragraph */

		*rightPos = (extEnd && position < textLength) ? position + 1 : position;
		for (pos = position - 1; pos >= 0 && text_GetChar(text, pos) != '\n'; pos--);
		*leftPos = pos + 1;
	    }
	    else  {
		/* Between two new lines */

		*rightPos = (extEnd && position < textLength) ? position + 1 : position;
		*leftPos = position;
	    }
	    break;
    }
}

boolean textview__Visible(self, pos)
    struct textview *self;
    long pos;
{
    register struct mark *lineMark;
    register long len = self->displayLength;
    register long endMark;
    
    if (self->nLines <= 0 || pos < mark_GetPos(self->top) || (pos == mark_GetPos(self->top) && ((pos != 0) && (text_GetChar(Text(self), pos - 1)) != '\n')))
	return FALSE;
    lineMark = self->lines[self->nLines - 1].data;
    if (pos < (endMark = mark_GetPos(lineMark) + self->lines[self->nLines-1].nChars))
	return TRUE;
    if (pos == len)  {
	if (pos == mark_GetPos(lineMark))
	    return (self->nLines != 1 || text_GetChar(Text(self), pos - 1) == '\n');
	else if (pos == endMark)
	    return (text_GetChar(Text(self), pos - 1) != '\n'); 
    }
    return FALSE;
}

long textview__Locate(self, x, y, foundView)
    struct textview *self;
    long x;
    long y;
    struct view **foundView;
{
    register long i;
    register long textLength = text_GetLength(Text(self));
    register long end = self->nLines;
    struct formattinginfo info;
    long pos;
    long by, bx;

    if(self->aLines > end && self->lines[end].containsView) end++; 
    if (foundView)
        *foundView = NULL;

    if (y < self->lines[0].y)   /* Tamper with clicks in top margin */
        y = self->lines[0].y;

    by = (self->hasApplicationLayer) ? self->by : self->eby;
    bx = (self->hasApplicationLayer) ? self->bx : self->ebx;

    for (i = 0; i < end ; i++)  { 
 	if (y >= self->lines[i].y && y < self->lines[i].y + self->lines[i].height)  {
	    pos = textview_LineRedraw(self, textview_GetPosition, self->lines[i].data, bx, by, textview_GetLogicalWidth(self) - 2 * bx, textview_GetLogicalHeight(self) - 2 * by, x, NULL,NULL, &info);
	    if (foundView)
	        *foundView = info.foundView;
	    return pos;
	}
	if (mark_GetPos(self->lines[i].data) + self->lines[i].nChars >= textLength)
	    return textLength;
    }

    if (i == 0)
	return textLength;
    else
	return mark_GetPos(self->lines[i - 1].data) + self->lines[i - 1].nChars;
}

void textview__GetTextSize(self, width, height)
    struct textview *self;
    long *width;
    long *height;
{
    long by = (self->hasApplicationLayer) ? self->by : self->eby;
    long bx = (self->hasApplicationLayer) ? self->bx : self->ebx;

    *width = textview_GetLogicalWidth(self) - 2 * bx;
    *height = textview_GetLogicalHeight(self) - 2 * by;
}

long textview__MoveBack(self, pos, units, type, distMoved, linesAdded)
    struct textview *self;
    long pos;
    long units;
    enum textview_MovementUnits type;
    long *distMoved;
    long *linesAdded;
{
    struct mark *tm;
    long lastn [MAXPARA];
    long posn[MAXPARA];
    struct text *text = Text(self);
    long textLength = text_GetLength(text);
    long pp, vxs, vys, length;
    long totalHeight, height, accumHeight, numLines, tpos, spos, newTop;
    register int tp, px;
    struct formattinginfo info;

    if (units < 0) return pos;
    
    accumHeight = 0;
    numLines = 0;
    textview_GetTextSize(self, &vxs, &vys);
    tm=mark_New();
    tpos = textview_GetTopPosition(self);
    spos = pos;
    pp = pos;
    for (;;)  {
	/* first move tp back to start of paragraph */

	if (pp > 0)  {
	    tp = pp-1;
	    while (tp >= 0 && text_GetChar(text,tp) != '\012') tp = tp-1;
	}
	else tp = -1;

	/* tp is -1 if paragraph starts document */

	tp++;		/* skip forward to first char in paragraph */
	pp = tp;	/* remember paragraph start */
	px = 0;
	totalHeight = 0;
	while (px < MAXPARA)  {		/* scan lines in the paragraph */
	    lastn[px] = tp;
	    posn[px] = totalHeight;
	    mark_SetPos(tm,tp);		/* start mark at paragraph start */
	    height = textview_LineRedraw(self, textview_GetHeight, tm, 0, 0, vxs, vys, 0, NULL,NULL, &info);
	    length = info.lineLength;

	    /* handle stopping prematurely at end of file, instead of at a newline */

	    if (tp+length == textLength && text_GetChar(text, textLength-1) != '\012') length ++;
	    if (pos == tp || (pos >= tp && pos < tp+length))  {
		/* we've stumbled over pos again */

		if (spos < tpos)  {
		    /* Thumbing backwards */
		    accumHeight = height;
		    numLines = 1;
		    newTop = tp;
		    while (1)  {
			tp += length;
			mark_SetPos(tm,tp);		/* start mark at paragraph start */
			height = textview_LineRedraw(self, textview_GetHeight, tm, 0, 0, vxs, vys, 0, NULL,NULL, &info);
			length = info.lineLength;
			if (tp+length == textLength && text_GetChar(text, textLength-1) != '\012') length ++;
			if (tpos == tp || (tpos >= tp && tpos < tp+length))  {
			    if (distMoved != NULL)  {
				*distMoved = accumHeight;
			    }
			    if (linesAdded != NULL)  {
				*linesAdded = numLines;
			    }
			    return newTop;
			}
			
			accumHeight += height;
			numLines++;
			
			if (accumHeight >= vys)  {
			    if (distMoved != NULL)  {
				*distMoved = -1;
			    }
			    if (linesAdded != NULL)  {
				*linesAdded = -1;
			    }
			    
			    return newTop;
			}
		    }
		}

		if (type == textview_MoveByLines)  {
		    if (units <= px)  {
			mark_Destroy(tm);
			if (distMoved)
			    *distMoved = accumHeight + totalHeight - posn[px-units];
			if (linesAdded)
			    *linesAdded = numLines - (px - units);
			return lastn[px-units];
		    }
		    else break;
		}
		else  {
		    if (units <= totalHeight + height)  {
			register int i;

			mark_Destroy(tm);
			for (i = 0; i <= px && totalHeight + height - posn[i] >= units; i++);
			if (distMoved)
			    *distMoved = accumHeight + totalHeight - posn[i-1];
			if (linesAdded)
			    *linesAdded = numLines - (i - 1);
			return lastn[i-1];
		    }
		    else break;
		}
	    }

	    if (tp >= textLength) break;
	    tp += length;
	    totalHeight += height;
	    numLines++;
	    px++;
	}

	/* here we've moved back px lines, but still haven't made it
	move back one more paragraph and try again */

	accumHeight += totalHeight;
	if (pp <= 0)  {
	    mark_Destroy(tm);
	    if (distMoved)
		*distMoved = accumHeight;
	    if (linesAdded)
		*linesAdded = numLines;
	    return 0;	/* can't go any farther */
	}

	if (type == textview_MoveByLines)
	    units -= px;
	else
	    units -= totalHeight;
	pos = pp;
	pp--;	/* looking at LF that terminated preceding para */
    }
}

long textview__MoveForward(self, pos, units, type, distMoved, linesAdded)
    struct textview *self;
    long pos;
    long units;
    enum textview_MovementUnits type;
    long *distMoved;
    long *linesAdded;
{
    struct mark *tm;
    int vxs, vys, length;
    register int i;
    struct formattinginfo info;

    if (units < 0) return pos;
    tm = mark_New();
    textview_GetTextSize(self, &vxs, &vys);
    i=0;
    while (i<units)  {
	mark_SetPos(tm,pos);
	textview_LineRedraw(self, textview_GetHeight, tm, 0, 0, vxs, vys, 0, NULL,NULL, &info);
	length = info.lineLength;
	pos = pos+length;
	i++;
    }
    mark_Destroy(tm);
    return pos;
}
#define DEFAULTHEIGHT 20
static int CalculateLineHeight(self)
    struct textview *self;
{

    struct style *defaultStyle;
    struct fontdesc *defaultFont;
    struct FontSummary *fontSummary;
    char fontFamily[256];
    long refBasis, refOperand, refUnit, fontSize;
    long by = (self->hasApplicationLayer) ? self->by : self->eby;

    if ((defaultStyle = textview_GetDefaultStyle(self)) == NULL)
        return DEFAULTHEIGHT;
    style_GetFontFamily(defaultStyle, fontFamily, sizeof (fontFamily));
    style_GetFontSize(defaultStyle, &refBasis, &fontSize);
    style_GetFontScript(defaultStyle, &refBasis, &refOperand, &refUnit);
    defaultFont = fontdesc_Create(fontFamily, refOperand, fontSize);
    if ((fontSummary = fontdesc_FontSummary(defaultFont, textview_GetDrawable(self))) == NULL)
        return DEFAULTHEIGHT;
    return fontSummary->maxHeight + by + by;
}
#define MAXWIDTH 1024
#define MINWIDTH 125
#define UNLIMITED 3000000
enum view_DSattributes textview__DesiredSize(self, width, height, pass, desiredwidth, desiredheight)
struct textview *self;
long width, height;
enum view_DSpass pass;
long *desiredwidth, *desiredheight;
{
    struct mark *tm;
    long  len, txheight,totheight,curx,cury,xs,ys = 0,maxlines,newwidth,sw;
    struct formattinginfo info;
    long pos = 0;
    long bx, by;

    if(Text(self) == NULL || ((len = text_GetLength(Text(self)))== 0)) {
	*desiredwidth = width;
	*desiredheight = CalculateLineHeight(self);
	return(view_HeightFlexible | view_WidthFlexible);
    }
    sw = width;
    totheight = 0;
    by = (self->hasApplicationLayer) ? self->by : self->eby;
    bx = (self->hasApplicationLayer) ? self->bx : self->ebx;

    cury = by;
    curx = bx;

    tm = mark_New();
    switch(pass){
	case view_HeightSet:
	    ys = height - 2 * by;
	    xs = UNLIMITED;
	    txheight = textview_LineRedraw(self, textview_GetHeight, tm, curx,cury,xs,ys, 0, NULL,NULL, &info);
	    if(txheight){
		maxlines = ys / txheight ;

	    }
	    else maxlines = 0;
	    if(maxlines) newwidth = info.totalWidth / maxlines;
	    else newwidth = (width > MINWIDTH && width < MAXWIDTH) ? width :MAXWIDTH;
	    if(newwidth	< MINWIDTH) 
		/* this really should be the width of the longest word in the text */
		newwidth = MINWIDTH;
	    width = newwidth;
	    break;
	case view_NoSet:
	    if(width <= 0 || width > 1024) width = 256;
	case view_WidthSet:
	    ys = UNLIMITED;
	    break;
    }
    xs = width - 2 * bx;
    while (pos < len )  {
	mark_SetPos(tm,pos);
	txheight = textview_LineRedraw(self, textview_GetHeight, tm, curx,cury,xs,ys, 0, NULL,NULL, &info);
	 if(info.lineLength == 0){
	     *desiredwidth = sw;
	     *desiredheight = CalculateLineHeight(self);
	     return(view_HeightFlexible | view_WidthFlexible);
	 }
	 if(pass == view_HeightSet && totheight + txheight > ys ){
	     if(pos == 0 || width >= MAXWIDTH) break;
#if 0
	     newwidth = (width * len)/ pos ;
	     if(newwidth == width ) width += 10;
	     else width = newwidth;
#endif /* 0 */
	     width += 10;
	     if(width > MAXWIDTH) width = MAXWIDTH;
	     pos = 0;	
	     xs = width - 2 * bx;
	     totheight = 0;
	     continue;
	 }
	pos += info.lineLength;
	totheight += txheight;
    }
    mark_Destroy(tm);
    *desiredwidth = width;
    *desiredheight = totheight + by + by;
    return(view_HeightFlexible | view_WidthFlexible);
}

void textview__GetOrigin(self, width, height, originX, originY)
    struct textview *self;
    long width;
    long height;
    long *originX;
    long *originY;
{
  /* NB...GetOrigin assumes BX and BY are 0 and is used to insert inline text
children. */

    struct formattinginfo info;

    textview_LineRedraw(self, textview_GetHeight, self->top, 0, 0, width,
                        height, 0, NULL, NULL, &info);
    *originY = info.lineAbove;
    *originX = 0;
}

void textview__FrameDot(self, pos)
    struct textview *self;
    long pos;
{
    mark_SetPos(self->frameDot, pos);
}

long textview__FindLineNumber(self, pos)
    struct textview *self;
    long pos;
{
    register struct linedesc *tl = self->lines;    
    register int i;
    register long len = text_GetLength(Text(self));
    register long endMark;
    
    for (i = 0; i < self->nLines; i++, tl++)  {
	if (pos >= mark_GetPos(tl->data))  {
	    if (pos < (endMark = mark_GetPos(tl->data) + tl->nChars))
		return i;
	    if (pos == len)  {
		if (pos == mark_GetPos(tl->data))  {
		    if (i != 0 || text_GetChar(Text(self), pos - 1) == '\n')
			return i;
		}
		else if (pos == endMark && text_GetChar(Text(self), pos - 1) != '\n')
		    return i;
	    }
	}
    }
    return -1;
}

static void getinfo(self, total, seen, dot)
    struct textview *self;
    struct range *total, *seen, *dot;
{

    total->beg = 0;
    total->end = text_GetLength(Text(self));
    seen->beg = textview_GetTopPosition(self);
    if (self->nLines != 0)
        seen->end = mark_GetPos(self->lines[self->nLines - 1].data) + self->lines[self->nLines - 1].nChars;
    else
        seen->end = 0;
    dot->beg = textview_GetDotPosition(self);
    dot->end = dot->beg + textview_GetDotLength(self);
}

static long whatisat(self, numerator, denominator)
    struct textview *self;
    long numerator, denominator;
{

    long coord;

    coord = numerator * textview_GetLogicalHeight(self);
    coord /= denominator;

    return textview_Locate(self, 0, coord, NULL);
}

static void setframe(self, position, numerator, denominator)
    struct textview *self;
    long position, numerator, denominator;
{
    long dist, lines, coord;
    long newpos;

    coord = numerator * textview_GetLogicalHeight(self);
    coord /= denominator;

    newpos = textview_MoveBack(self, position, coord, textview_MoveByPixels, &dist, &lines);
    if (newpos < textview_GetTopPosition(self) && self->scroll != textview_ScrollForward && self->scroll != textview_MultipleScroll)  {
	if (dist == -1)  {
	    self->scrollDist = -1;
	    self->scroll = textview_MultipleScroll;
	}
	if (self->scrollDist == -1)  {
	    self->scrollDist = dist;
	    self->scrollLine = lines;
	}
	else  {
	    self->scrollDist += dist;
	    if (self->scrollDist >= textview_GetLogicalHeight(self))
		self->scrollDist = -1;
	    else
		self->scrollLine += lines;
	}
    }
    textview_SetTopPosition(self, newpos);
}

char *textview__GetInterface(self, interfaceName)
    struct textview *self;
    char *interfaceName;
{

    if (strcmp(interfaceName, "scroll,vertical") == 0)
        return (char *) &scrollInterface;
    return NULL;
}

boolean  textview__InitializeClass(classID)
    struct classheader *classID;
{
    extern struct keymap *textview_InitKeyMap();

    textviewKeymap = textview_InitKeyMap(&textview_classinfo, &viewMenus);
    initialExposeStyles =
        environ_GetProfileSwitch("ExposeStylesOnStartup", FALSE);

    return TRUE;
}

void textview__SetDefaultStyle(self, styleptr)
    struct textview *self;
    struct style *styleptr;
{
    self->defaultStyle = styleptr;
}

struct style *textview__GetDefaultStyle(self)
    struct textview *self;
{
    return self->defaultStyle;
}

void textview__Print(self, f, process, final, toplevel)
    struct textview *self;
    FILE *f;
    char *process;
    char *final;
    int toplevel;
{
    /* This is really screwed. This should return an error and the guy above this */
    /* layer should handle it. */
    if (class_Load("texttroff") == NULL) {
	message_DisplayString(self, 0, "Print aborted: could not load class \"texttroff\".");
        return;
    }

    if (strcmp(process, "troff") == 0 && strcmp(final, "PostScript") == 0)
	texttroff_WriteTroff(self, Text(self), f, toplevel); 
}

static void CreateMatte(self, vr)
struct textview *self;
struct viewref *vr;
{
    struct view *v =
      (struct view *) dictionary_LookUp(self, (char *) vr);

    if (v == NULL && class_IsTypeByName(vr->viewType, "view")) { 
        v = (struct view *) matte_Create(vr, (struct view *) self);
        if (v != NULL) {
            viewref_AddObserver(vr, self);
            dictionary_Insert(self, (char *) vr, (char *) v);
        }
    }

    if (v != NULL) {
        if (v->parent != (struct view *) self)
            view_LinkTree(v, (struct view *) self);
        view_InitChildren(v);
    }
}

void textview__InitChildren(self)
struct textview *self;
{
    struct text *d = Text(self);
    long pos = 0;

    while (pos < text_GetLength(d)) {
        long gotlen;
        char *s = text_GetBuf(d, pos, 10240, &gotlen);
        while (gotlen--) {
            if (*s == TEXT_VIEWREFCHAR) {
                struct environment *env =
                  environment_GetInnerMost(d->rootEnvironment, pos);
                if (env != NULL && env->type == environment_View)
                    CreateMatte(self, env->data.viewref);
            }
            s++, pos++;
	}
    }
}

boolean textview__CanView(self, TypeName)
struct textview *self;
char *TypeName;
{
    return class_IsTypeByName(TypeName, "text");
}
