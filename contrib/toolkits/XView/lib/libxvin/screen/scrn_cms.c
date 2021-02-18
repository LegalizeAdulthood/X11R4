#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)scrn_cms.c 20.28 89/11/21";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <stdio.h>
#include <strings.h>
#include <xview/base.h>
#include <xview_private/scrn_impl.h>
#include <xview_private/xv_color.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

Xv_private Xv_Colormap_Seg *
screen_create_cms(cms_name, cms_data)
    char           *cms_name;
    Xv_cmsdata     *cms_data;
{
    Xv_Colormap_Seg *cms;

    cms = xv_alloc(Xv_Colormap_Seg);
    if (strlen(cms_name) > CMS_NAMESIZE) {
	(void) strncpy(cms->cms_name, cms_name, (int) CMS_NAMESIZE);
	cms->cms_name[CMS_NAMESIZE] = '\0';
    } else {
	(void) strcpy(cms->cms_name, cms_name);
    }
    cms->cms_type = cms_data->type;
    cms->cms_size = cms_data->size;
    if (cms_data->type == XV_STATIC_CMS) {
	register int    i;

	cms->index_table = xv_alloc_n(short *, cms_data->size * sizeof(short));
	for (i = 0; i <= cms_data->size - 1; i++) {
	    cms->index_table[i] = XV_INVALID_PIXVAL;
	}
    }
    return cms;
}

Xv_private Xv_Colormap *
screen_create_rw_cmap(screen_public)
    Xv_Screen       screen_public;
{
    Screen_info    *screen = SCREEN_PRIVATE(screen_public);
    Xv_Colormap    *cmap;
    Display        *display;

    display = (Display *) xv_get(screen->server, (Attr_attribute)XV_DISPLAY);

    cmap = xv_alloc(Xv_Colormap);
    cmap->id = XCreateColormap(display, RootWindow(display, screen->number),
			       screen->dynamic_visual, AllocNone);
    cmap->allocated = 0;
    cmap->cmap_type = XV_DYNAMIC_CMAP;
    cmap->next = screen->cmap_list;
    screen->cmap_list = cmap;
    return cmap;
}

Xv_private
screen_init_default_cms(screen_public)
    Xv_Screen       screen_public;
{
    Screen_info    *screen = SCREEN_PRIVATE(screen_public);
    Xv_Colormap    *cmap;
    Display        *display;
    XVisualInfo     vinfo;

    display = (Display *) xv_get(screen->server, (Attr_attribute)XV_DISPLAY);

    cmap = xv_alloc(Xv_Colormap);
    cmap->id = DefaultColormap(display, screen->number);
    cmap->allocated = 2;

    if (XMatchVisualInfo(display, screen->number,
			 XDisplayPlanes(display, screen->number),
			 StaticColor, &vinfo)) {
	screen->static_visual = vinfo.visual;
	if (vinfo.visual == DefaultVisual(display, screen->number)) {
	    cmap->cmap_type = XV_STATIC_CMAP;
	}
    }
    if (XMatchVisualInfo(display, screen->number,
			 XDisplayPlanes(display, screen->number),
			 PseudoColor, &vinfo)) {
	screen->dynamic_visual = vinfo.visual;
	if (vinfo.visual == DefaultVisual(display, screen->number)) {
	    cmap->cmap_type = XV_DYNAMIC_CMAP;
	}
    }
    cmap->cms_list = xv_alloc(Xv_Colormap_Seg);
    (void) strcpy(cmap->cms_list->cms_name, XV_DEFAULT_CMS);
    cmap->cms_list->cms_type = XV_STATIC_CMS;
    cmap->cms_list->cms_size = 2;
    cmap->cms_list->index_table = xv_alloc_n(short *, 2 * sizeof(short));
    cmap->cms_list->index_table[0] = WhitePixel(display, screen->number);
    cmap->cms_list->index_table[1] = BlackPixel(display, screen->number);

    screen->default_cmap = screen->cmap_list = cmap;
    screen->default_cms = cmap->cms_list;
    cmap->cms_list->cms_cmap = cmap;
}

Xv_private Xv_Colormap_Seg *
screen_find_cms(screen_public, cms_name)
    Xv_Screen       screen_public;
    char           *cms_name;
{
    Screen_info    *screen = SCREEN_PRIVATE(screen_public);
    Xv_Colormap    *cmap;
    Xv_Colormap_Seg *cms;

    for (cmap = screen->cmap_list; cmap != NULL; cmap = cmap->next) {
	for (cms = cmap->cms_list; cms != NULL; cms = cms->next)
	    if (strcmp(cms_name, cms->cms_name) == 0)
		return (cms);
    }
    return (NULL);
}

/*
 * Allocate the colormap segment cells of the required type (ro/rw) out of
 * the best available colormap. If the cells for this colormap segment have
 * aready been allocated: - If static cms, free the currently allocated cells
 * and reallocate. - If dynamic simply clobber the old values in the
 * allocated cells.
 */
Xv_private Xv_Colormap_Seg *
screen_alloc_cms(screen_public, cms_name, cms_data)
    Xv_Screen       screen_public;
    char           *cms_name;
    Xv_cmsdata     *cms_data;
{
    Screen_info    *screen = SCREEN_PRIVATE(screen_public);
    Xv_Colormap_Seg *cms;
    Xv_Colormap_Seg *screen_alloc_rw_cells(), *screen_alloc_ro_cells();
    register XColor *xcolors;
    register int    i;
    Display        *display;

    display = (Display *) xv_get(screen->server, (Attr_attribute)XV_DISPLAY);
    if (DisplayPlanes(display, screen->number) < 2) {
	return (NULL);
    }
    xcolors = xv_alloc_n(XColor *, cms_data->rgb_count * sizeof(XColor));
    for (i = 0; i <= cms_data->rgb_count - 1; i++) {
	(xcolors + i)->red = (unsigned short) cms_data->red[i] << 8;
	(xcolors + i)->green = (unsigned short) cms_data->green[i] << 8;
	(xcolors + i)->blue = (unsigned short) cms_data->blue[i] << 8;
	(xcolors + i)->flags = DoRed | DoGreen | DoBlue;
    }

    if (cms_data->type == XV_DYNAMIC_CMS) {
	cms = screen_alloc_rw_cells(screen_public, cms_name, cms_data, xcolors);
    } else {
	cms = screen_alloc_ro_cells(screen_public, cms_name, cms_data, xcolors);
    }
    xv_free(xcolors);

    if (!cms) {
	xv_error(XV_NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING,
		 (cms_data->type == XV_DYNAMIC_CMS) ?
		 "XAllocColorCells failed" :
		 "XAllocColor failed",
		 NULL);
    }
    return (cms);
}

Xv_private Xv_Colormap_Seg *
screen_alloc_ro_cells(screen_public, cms_name, cms_data, xcolors)
    Xv_Screen       screen_public;
    char           *cms_name;
    Xv_cmsdata     *cms_data;
    XColor         *xcolors;
{
    Screen_info    *screen = SCREEN_PRIVATE(screen_public);
    Display        *display;
    Xv_Colormap_Seg *cms;
    register int    i;

    display = (Display *) xv_get(screen->server, (Attr_attribute)XV_DISPLAY);

    if ((cms = screen_find_cms(screen_public, cms_name)) == NULL) {
	cms = screen_create_cms(cms_name, cms_data);
	cms->next = screen->default_cmap->cms_list;
	screen->default_cmap->cms_list = cms;
	cms->cms_cmap = screen->default_cmap;
    }
    for (i = 0; i <= cms_data->rgb_count - 1; i++) {
	/*
	 * Colormap segment cells in a static colormap can be written into
	 * only once.
	 */
	if (cms->index_table[cms_data->index + i] != XV_INVALID_PIXVAL) {
	    continue;
	}
	/*
	 * XAllocColor() will fail if all the cells in the default colormap
	 * have been allocated read/write.
	 */
	if (!XAllocColor(display, screen->default_cmap->id, xcolors + i)) {
	    return (NULL);
	}
	cms->index_table[cms_data->index + i] = (xcolors + i)->pixel;
    }

    return (cms);
}

Xv_private Xv_Colormap_Seg *
screen_alloc_rw_cells(screen_public, cms_name, cms_data, xcolors)
    Xv_Screen       screen_public;
    char           *cms_name;
    Xv_cmsdata     *cms_data;
    XColor         *xcolors;
{
    Screen_info    *screen = SCREEN_PRIVATE(screen_public);
    Xv_Colormap    *cmap, *screen_create_rw_cmap();
    Xv_Colormap_Seg *cms;
    unsigned long   pixels[256];
    Display        *display =
	(Display *) xv_get(screen->server, (Attr_attribute)XV_DISPLAY);
    register int    i;

    if ((cms = screen_find_cms(screen_public, cms_name)) == NULL) {
	cms = screen_create_cms(cms_name, cms_data);

	/*
	 * If the default cmap is dynamic, dont really know how many free
	 * cells are available since other applications might have allocated
	 * cells out of the colormap. If XAllocColorCells() fails then create
	 * a new colormap and allocate out of it.
	 */
	if ((screen->default_cmap->cmap_type == XV_DYNAMIC_CMAP) &&
	    (XAllocColorCells(display, screen->default_cmap->id, True,
			      NULL, 0, pixels, cms->cms_size))) {
	    cmap = screen->default_cmap;
	} else {
	    /* Find a dynamic colormap to allocate the cells */
	    for (cmap = screen->cmap_list; cmap != NULL; cmap = cmap->next) {
		if ((cmap->cmap_type == XV_DYNAMIC_CMAP) &&
		    (cmap != screen->default_cmap) &&
		    ((cmap->allocated + cms->cms_size) <= 256)) {
		    break;
		}
	    }

	    if (!cmap) {
		cmap = screen_create_rw_cmap(screen_public);
	    }
	    if (!XAllocColorCells(display, cmap->id, True, NULL,
				  0, pixels, cms->cms_size)) {
		return (NULL);
	    }
	}
	cmap->allocated += cms->cms_size;
	cms->base_pixel = pixels[0];
	cms->next = cmap->cms_list;
	cmap->cms_list = cms;
	cms->cms_cmap = cmap;
    } else {
	cmap = cms->cms_cmap;
    }

    for (i = 0; i <= cms_data->rgb_count - 1; i++) {
	(xcolors + i)->pixel = cms->base_pixel + cms_data->index + i;
    }
	
    XStoreColors(display, cmap->id, xcolors, cms_data->rgb_count);

    return (cms);
}

/*ARGSUSED*/
Xv_private void
screen_remove_cms(screen_public, cms)
    Xv_Screen       screen_public;
    Xv_Colormap_Seg *cms;
{
    /*
     * Bug - Ref counting and removal of colormap segments when not
     * referenced must be implemented.
     */
}
