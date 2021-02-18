/*
*    Copyright (c) 1988  University of Southern California
*
*    USC Information Sciences Institute
*    4676 Admiralty Way
*    Marina del Rey, California 90292
*    (vaxa.isi.edu  in  pub/util/X)
*
*    This  software  and its  documentation  may be used,  copied,
*    modified,  and  distributed  for  any  purpose  without  fee,
*    provided:
*    --  The above copyright notice appears in all copies.
*    --  This disclaimer appears in all source code copies.
*    --  The names of ISI and USC are not used in advertising or
*        publicity  pertaining  to distribution  of the software
*        without prior specific written permission from ISI.
*    ISI provides this software freely as a public service.  It is
*    NOT a commercial product,  and therefore is not subject to an
*    an implied  warranty  of  merchantability  or  fitness  for a
*    particular purpose.  ISI provides it as is, without warranty.
*
*    3-16-88     Paul Raveling
*    4-17-88     Philip Thompson (phils@athena.mit.edu)
*                    - cleaned up code, made readable.
*    2-19-89         - uses less memory w/ smaller Color & Node struct.
*/
/**********************************************************************/
/*  file: $Source: /mit/phils/X/Lib/RCS/fant.c,v $ */
/*                                                                    */
/*  author: Philip R. Thompson, ($Author: phils $)                    */
/*          $Date: 89/10/17 01:38:31 $      $State: Exp $             */
/*  Copyright (c) 1989, Massachusetts Institute of Technology         */
/*                Philip R. Thompson   (phils@athena.mit.edu)         */
/*                Computer Resource Laboratory (CRL)                  */
/*                Dept. of Architecture and Planning                  */
/*                M.I.T., Rm 9-526                                    */
/*                Cambridge, MA  02139                                */
/**********************************************************************/
#if (!defined(lint) && !defined(SABER))
static char  alcols_rcsid[] =
    "$Header: /mit/phils/X/RCS/xtoxim.c,v 1.17 89/11/09 17:29:54 phils Exp Locker: phils $";
#endif

#include <stdio.h>
#include <sys/types.h>
#include "XimHeader.h"

#define MAXRGB      255     /* Maximum RGB component value */
#define MAXDEPTH    8       /* Maximum depth for color cube tree */
#define MAXNODES    50      /* Maximum number of color tree nodes,
                                expressed as fraction of pixmap size */

typedef struct node {       /* Node of color cube description tree */
    struct node *parent;    /* Pointer to parent node */
    struct node *off;       /* Pointer to 1st offspring node */
    struct node *sibling;   /* Pointer to next sibling */
    u_short minred;         /* Minimum red coordinate */
    u_short maxred;         /* Maximum red coordinate */
    u_short mingreen;       /* Minimum green coordinate */
    u_short maxgreen;       /* Maximum green coordinate */
    u_short minblue;        /* Minimum blue coordinate */
    u_short maxblue;        /* Maximum blue coordinate */
    unsigned  numcols;      /* Number of colors in cube */
    unsigned  numuniq;      /* # colors unique to node */
    unsigned  sumred;       /* Sum of red components */
    unsigned  sumgreen;     /* Sum of green components */
    unsigned  sumblue;      /* Sum of blue components */
} Node;

Node  *cube_root;           /* Root of color cube tree */
int  COLTDEPTH = 0;         /* Color tree depth */
int  coltdepth;             /* Assigned color tree depth */
int  COLTNODES = MAXNODES;  /* tree capacity -
                                    as a multiple of color count */
int  coltnodes;        /* tree capacity (number of nodes) */
int  coltvol;          /* Current tree volume in number of nodes */
int  num_colreq;       /* Number of colors required  */
char *calloc(), *malloc();
extern debug_flag;


OctreeQuantize(xim, ncolors)
XimImage *xim;
int ncolors;
{
    int i;
    Color  *cmap, *ic = NULL;
    unsigned D = 0, maxd = 0;
    double Dp, sqrt();
    register d, dr, dg, db;

    if (!(cmap = (Color *)calloc((unsigned)ncolors, sizeof(Color)))) {
        XimWarn("Octree: can't calloc() color table");
        return(0);
    }
    if ((ic = (Color *)calloc(xim->datasize, sizeof(Color))) == NULL) {
        XimWarn("Octree: can't calloc() alcols buffer");
        return(0);
    }
    for (i=0; i < xim->datasize; i++) {
        ic[i].red = xim->data[i];
        ic[i].grn = xim->grn_data[i];
        ic[i].blu = xim->blu_data[i];
    }
    free((char *)xim->data);
    xim->data = (byte*)0;
    free((char *)xim->grn_data);
    xim->grn_data = (byte*)0;
    free((char *)xim->blu_data);
    xim->blu_data = (byte*)0;

    if (!(ncolors = alcols(ic, xim->datasize, cmap, ncolors))) {
        XimWarn("OctreeQuantize: no colors returned");
        free((char *)ic);
        free((char *)cmap);
        return(0);
    }
    if (!(xim->data = (byte *)malloc(xim->datasize))) {
        XimWarn("Octree: can't malloc data buffer.");
        free((char *)ic);
        free((char *)cmap);
        return(0);
    }
    for (i=0; i < xim->datasize; i++)
        xim->data[i] = ic[i].pixel;

    xim->ncolors = ncolors;
    if (!(xim->colors = (Color *)calloc((unsigned)xim->ncolors,
            sizeof(Color)))) {
        XimWarn("SetColors: can't calloc() colors.");
        return(0);
    }
    for (i=0; i < xim->ncolors; i++) {
        xim->colors[i].red = cmap[i].red;
        xim->colors[i].grn = cmap[i].grn;
        xim->colors[i].blu = cmap[i].blu;
    }
    if (debug_flag) {
        for (i=0; i < xim->ncolors; i++)
            fprintf(stderr, "%3d:  (%3u,%3u,%3u) [%u]\n", i,
            cmap[i].red, cmap[i].grn, cmap[i].blu, cmap[i].pixel);
        for (i=0; i < xim->datasize; i++) {
            dr = ic[i].red - cmap[ic[i].pixel].red;
            dg = ic[i].grn - cmap[ic[i].pixel].grn;
            db = ic[i].blu - cmap[ic[i].pixel].blu;
            d = (int)sqrt((double)(dr*dr + dg*dg + db*db));
            D += d;
            if (d > maxd)
                maxd = d;
        }
        Dp = (double)D / (double)xim->datasize;
        fprintf(stderr,"Total quantization error = %lu\n", D);
        fprintf(stderr,"Mean error per pixel     = %lg\n", Dp);
        fprintf(stderr,"Maximum error for any pixel = %d / 765\n",maxd);
    }
    free((char *)ic);
    free((char *)cmap);
    xim->bits_channel = 8;
    xim->nchannels = 1;
    return(1);
}


Node *init_node()
{
    Node  *cur;

    if ((cur = (Node *)calloc(1, sizeof(Node))) == NULL) {
        XimWarn("Can't calloc a Node.");
        return((Node *)0);
    }
    coltvol++;              /* Count 1 more node in tree */
    return(cur);
}

prune(node_ptr)
Node *node_ptr;
{
    register Node  *next, *prev, *parent;

    /* Prune all offspring of this node */
    while (node_ptr->off)
        prune (node_ptr->off);
    /* Merge color statistics into parent */
    parent = node_ptr->parent;
    parent->numuniq  += node_ptr->numuniq;
    parent->sumred   += node_ptr->sumred;
    parent->sumgreen += node_ptr->sumgreen;
    parent->sumblue  += node_ptr->sumblue;
    
    /* Remove current node from its sibling list */
    /* or from its parent's offspring pointer */
    next = node_ptr->sibling;
    prev = parent->off;

    if (parent->off == node_ptr)  /* If node is head of sibling list */
        parent->off = next;       /*  change parent's offspring ptr */
    else {
       /* Find node's predecessor on sibling list and queue around node.
       *  Note that the check above guarantees existence of a
       *  predecessor on the sibling list.
       */
        while (prev->sibling != node_ptr)
            prev = prev->sibling;
        prev->sibling = next;
    }       
    /* Release the node's memory */
    free((char *)node_ptr);
    coltvol--;             /* Count 1 less node in tree */
}

prune_level()
{
    register Node  *cur, *parent, *sibling, *offspring;
    register level;

#ifdef TRACE
    fprintf(stderr, "ILP:  %d levels, %d nodes\n", coltdepth, coltvol);
#endif
    cur = cube_root;
    level = 0;
    while (cur) {
        parent   = cur->parent;         /* Save pointers to parent, */
        offspring = cur->off;           /* offspring, */
        sibling  = cur->sibling;        /* and sibling nodes */
        if (level == coltdepth)         /* If at maximum depth */
            prune(cur);                 /*  prune this node */
        /* Locate next node */
        if (offspring)  {       /* Descend if possible */
            cur = offspring;
            level++;
            continue;
        }
        if (sibling)  {         /* Move to sibling if possible */
            cur = sibling;
            continue;
        }
        cur = parent;               /* else move to next node */
        level--;                    /* via parent */
        while (cur) {
            if (cur->sibling) {
                cur = cur->sibling;
                break;
            }
            cur = cur->parent;
            level--;
        }
    }
    coltdepth--;               /* Allow 1 less level in tree */
#ifdef TRACE
    fprintf(stderr, "FLP:  %d levels, %d nodes\n", coltdepth, coltvol);
#endif
}


/* Input:
*    rgb_pixmap  Color[]
*            Pointer to array of Color structures defining
*            RGB values for a set of pixels (typically a pixmap
*            or an "ideal" color map).  Only the red/green/blue
*            data are used as input.
*    pixmap_size int
*            Number of Color elements (i.e., number of pixels)
*            in rgb_pixmap
*    color_map   Color[]
*            Pointer to local color map (array of Colors);
*            No contents are used as input.
*    colmap_size int
*            Maximum size (in number of elements) of color map
*
*    Non-parameter (global) data:
*    COLTNODES   int
*            Maximum size of color tree, as fraction of
*            pixmap size.  Any value < 2 defaults to 10.
*            (e.g., 10 => max # of nodes to keep in color tree
*            during allocation is pixmap_size / 10).
*    COLTDEPTH   int
*            Depth to use for color classification tree.
*            Any value < 2 defaults to log2 of colmap_size,
*            rounded up, but never to more than MAXDEPTH (8).
* Output:
*    allocd_size int
*    <return value>  Number of colors actually allocated
*    rgb_pixmap  Color[]
*            Array of Colors for each pixel.  Pixel values
*            are supplied in each element, RGB values are
*            unaltered from original contents
*    color_map   Color[]
*            Local color map; Elements are initialized
*            to both RGB values and corresponding pixel values
*/

int alcols(rgb_pixmap, pixmap_size, color_map, colmap_size)
Color *rgb_pixmap, *color_map;
unsigned  pixmap_size;
int  colmap_size;
{
    register i;             /* Pixel index */
    int  pt;                /* Pruning threshold */
    int  next_pt;           /* Next pruning thesh. */
    register Node *cur = NULL;   /* Current node */
    register Node *next = NULL;  /* Next node  */
    register Node *prev = NULL;  /* Previous node */
    int  level;             /* Level in tree */
    int  li;                /* Level index */
    u_short red, grn, blu;  /* Pixel value */
    u_short mp;             /* Midpoint of a color range */

    /* Initialize tree to describe color cube */
    if (COLTDEPTH >= 2)         /* If color tree depth is fixed */
        coltdepth = COLTDEPTH;  /* use global value   */
    else  {                     /* select default */
        coltdepth = 0;
        for (i=colmap_size; i > 0; i = i>>1) /* set coltdepth = log2 */
            coltdepth++;
        if ((1<<coltdepth) == colmap_size)
            coltdepth--;            /* Powers of 2 would be off by 1 */
        if (coltdepth > MAXDEPTH)   /* Restrict maximum tree depth */
            coltdepth = MAXDEPTH;   /* to compiled max.   */
    }
    if (COLTNODES != 0)       /* If pixmap/color tree size ratio OK */
        coltnodes = COLTNODES;      /* use global value   */
    else
        coltnodes = MAXNODES;       /* used compiled max  */

    if (pixmap_size >= 10000)               /* If pixmap isn't small  */
        coltnodes = pixmap_size / coltnodes;  /* Convert node count */
    else                                /* else override to assure */
        coltnodes = pixmap_size;            /* good color resolution  */

    if (debug_flag)
        fprintf(stderr, "Initial # nodes = %d.\n", coltnodes);
    coltvol = 0;                    /* Initial node count = 0 */
    if (!(cube_root = init_node())) {       /* Allocate root node */
        XimWarn("can't allocate root node");
        return(0);
    }
    cube_root->maxred  = MAXRGB;
    cube_root->maxgreen = MAXRGB;   /* Set root color range */
    cube_root->maxblue = MAXRGB;

    /* For each pixel, locate or allocate a node at each
    *  level of the color cube tree which contains this
    *  color and update the count and sum values for
    *  colors in that node.
    */
    for (i=0; i < pixmap_size; i++) {
        if (coltvol > coltnodes)        /* If tree is too large */
            prune_level();              /* prune 1 level off */
        red = (u_short)rgb_pixmap[i].red;       /* Get pixel's color */
        grn = (u_short)rgb_pixmap[i].grn;
        blu = (u_short)rgb_pixmap[i].blu;

        cur = cube_root;                    /* Start at root node */
        for (li=0; li < coltdepth; li++) {  /* For each level of tree */
            level = li + 1;
            if (cur->off == NULL) { /* If nothing on level create one */
                if (!(next = init_node())) {
                    XimWarn("can't allocate a level node");
                    return(0);
                }
                next->parent = cur;     /* Create 1st element */
                cur->off    = next;
                /* Set colors of 1st element to contain pixel */
                mp = cur->minred + (cur->maxred - cur->minred)/2;
                if (red <= mp) {
                    next->minred = cur->minred;
                    next->maxred = mp;
                } else {
                    next->minred = mp+1;
                    next->maxred = cur->maxred;
                }
                mp = cur->mingreen + (cur->maxgreen - cur->mingreen)/2;
                if (grn <= mp) {
                    next->mingreen = cur->mingreen;
                    next->maxgreen = mp;
                } else {
                    next->mingreen = mp+1;
                    next->maxgreen = cur->maxgreen;
                }
                mp = cur->minblue + (cur->maxblue - cur->minblue)/2;
                if (blu <= mp) {
                    next->minblue = cur->minblue;
                    next->maxblue = mp;
                } else {
                    next->minblue = mp+1;
                    next->maxblue = cur->maxblue;
                }
            }  /* Create new level */
            
            cur = cur->off;         /* Point to 1st entry on level */
            while (cur) {           /* Search siblings at this level */
                prev = cur;
                if ((red >= cur->minred) && (red <= cur->maxred)
                    && (grn >= cur->mingreen) && (grn <= cur->maxgreen)
                    && (blu >= cur->minblue) && (blu <= cur->maxblue))
                    break;
                cur = cur->sibling;
            }
            if (cur == NULL) {   /* for pixel's color create sibling */
                if (!(cur = init_node())) {
                    XimWarn("can't allocate a sibbling node");
                    return(0);
                }
                prev->sibling = cur;
                prev = prev->parent;
                cur->parent = prev;

                /* Set colors of new element to contain pixel */
                mp = prev->minred + (prev->maxred - prev->minred)/2;
                if (red <= mp) {
                    cur->minred = prev->minred;
                    cur->maxred = mp;
                } else {
                    cur->minred = mp+1;
                    cur->maxred = prev->maxred;
                }
                mp = prev->mingreen +(prev->maxgreen -prev->mingreen)/2;
                if (grn <= mp) {
                    cur->mingreen = prev->mingreen;
                    cur->maxgreen = mp;
                } else {
                    cur->mingreen = mp+1;
                    cur->maxgreen = prev->maxgreen;
                }
                mp = prev->minblue + (prev->maxblue - prev->minblue)/2;
                if (blu <= mp) {
                    cur->minblue = prev->minblue;
                    cur->maxblue = mp;
                } else {
                    cur->minblue = mp+1;
                    cur->maxblue = prev->maxblue;
                }

            }  /* Create new sibling */
            
            /* cur now points to an existing node
               representing the pixel's color.  */
            cur->numcols++;             /* Count colors in node */

            if (level == coltdepth) {  /* If this is a leaf... */
                cur->numuniq++;        /* Incr. unique color count */
                cur->sumred   += red;  /* Sum rgb values for */
                cur->sumgreen += grn;  /* later derivation of */
                cur->sumblue  += blu;  /* mean cube color */
            }
        }   /* for each level */
    }    /* for each pixel */

    /* Set number of color points (number of pixels) in root node. */
    cube_root->numcols = pixmap_size;

    /* Select colors from the color tree */
    pt = 0;         /* Initial pruning threshold = 0 */

    do {
        num_colreq = 0;
        next_pt   = 0x7fffffff;
        cur = cube_root;
        do {
            if (cur->off && (cur->numcols > pt))    /* If offspring */
                next = cur->off;            /* it won't be pruned */
            else {                          /* Can't descend... */
                if (cur->sibling)           /* try to move laterally */
                    next = cur->sibling;
                else  {   /* Can't move laterally, Move up and across */
                    next = cur;
                    while (next->parent) {
                        next = next->parent;
                        if (next->sibling) {
                            next = next->sibling;
                            break;
                        }
                    }
                    if (next->parent == NULL)   /* Quit ascending */
                        next = NULL;            /* above root node */
                }
            }
            /* If current node has a sub-threshold color count,
            *  then prune it (and its subtree).  Make next node current.
            *  If new current node is a leaf, increment leaf count.
            */
            if (cur->numcols <= pt)
                prune(cur);
            else
                if (cur->numcols < next_pt)
                    next_pt = cur->numcols;
            cur = next;
        } while (cur);

        pt = next_pt;       /* Set next pruning threshold */
        /* now determine how many colors would be allocated
        *  from current tree.
        */
        i = 0;                  /* Start at element 0 of color map */
        cur = cube_root;        /* Start at root of tree  */
        while (cur) {
            /* If current node's unique color count is nonzero,
               then this node would allocate a color.  */
            if (cur->numuniq)           /* If node has unique colors */
                i++;                    /* count a color def for it */
            /* Locate next node */
            if (cur->off)               /* If an offspring exists */
                cur = cur->off;         /* descend in tree */
            else {                      /* Can't descend...   */
                if (cur->sibling)       /* try to move laterally */
                    cur = cur->sibling;
                else  {                 /* Move up and across */
                    while (cur->parent) {
                        cur = cur->parent;
                        if (cur->sibling) {
                            cur = cur->sibling;
                            break;
                        }
                    }
                    if (cur->parent == NULL)    /* Quit ascending */
                        cur = NULL;             /* above root node */
                }
            }
        }
        num_colreq = i;             /* Save # colors allocated */
#ifdef TRACE
        fprintf(stderr, " # unique colors %d\n", num_colreq);
#endif TRACE
    } while (num_colreq > colmap_size);

    /* Set color definitions in color map */
    cur = cube_root;        /* Start at root of tree  */
    i = 0;            /* Start at element 0 of color map */

    while (cur) {
        /* If current node's unique color count is nonzero,
           then allocate a color for this node.  */
        if (cur->numuniq)  {            /* If unique colors define it */
            color_map[i].red = (byte)(cur->sumred / cur->numuniq);
            color_map[i].grn = (byte)(cur->sumgreen / cur->numuniq);
            color_map[i].blu = (byte)(cur->sumblue  / cur->numuniq);
            cur->numcols = i;      /* Save color map index in numcols */
            i++;                   /* incr count of colors allocated */
        }
        /* Locate next node */
        if (cur->off)               /* If an offspring node exists */
            cur = cur->off;         /* descend in tree */
        else {                      /* Can't descend...   */
            if (cur->sibling)       /* try to move laterally */
                cur = cur->sibling;
            else {                  /* Move up and across */
                while (cur->parent) {
                    cur = cur->parent;
                    if (cur->sibling) {
                        cur = cur->sibling;
                        break;
                    }
                }
                if (cur->parent == NULL)    /* Quit trying to ascend */
                    cur = NULL;             /* above root node */
            }
        }
    }
    num_colreq = i;                 /* Save # colors allocated */
    if (debug_flag)
        fprintf(stderr, "Final node count = %d\n", coltvol);

    /* Assign color indices to pixels */
    for (i=0; i < pixmap_size; i++) {
        red = (u_short)rgb_pixmap[i].red;
        grn = (u_short)rgb_pixmap[i].grn;
        blu = (u_short)rgb_pixmap[i].blu;
        cur = cube_root;            /* Start at root node */
        prev = cur;

        /* Find lowest cube in tree which contains this color */
        while (cur->off) {
            cur = cur->off;         /* Go 1 level deeper */
            while ((cur) && (prev != cur)) {  /* siblings */
                if ((red >= cur->minred) && (red <= cur->maxred)
                    && (grn >= cur->mingreen) && (grn <= cur->maxgreen)
                    && (blu >= cur->minblue) && (blu <= cur->maxblue)) {
                    prev = cur;     /* Save ptr to matching node */
                    break;
                }
                cur = cur->sibling;
            }
            if (prev != cur)        /* Escape if pixel was */
                break;              /* not found at this depth */
        }
        rgb_pixmap[i].pixel = (byte)prev->numcols;   /* Assign index */
    }
    /* Release storage used by the color cube tree */
    cur = cube_root->off;
    while (cur) {                   /* Prune all offspring of root */
        next = cur->sibling;
        prune(cur);
        cur = next;
    }
    free((char *)cube_root);           /* Release root node  */
    return(num_colreq);                /* Return # colors allocated */
}

/*** end alcols.c ***/
