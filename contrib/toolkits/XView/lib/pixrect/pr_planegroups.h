/*	@(#)pr_planegroups.h 1.4 88/02/08 SMI	*/

/*
 * 	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE
 *	file for terms of the license.
 */

#ifndef	pr_planegroups_DEFINED
#define	pr_planegroups_DEFINED

/* Masks for frame buffer planes and plane group number */
#define	PIX_ALL_PLANES		0x00FFFFFF
#define	PIX_GROUP_MASK		0x7F

/* Macros to encode or extract group into or from attribute word */
#define	PIX_GROUP(g)		((g) << 25)
#define	PIX_ATTRGROUP(attr)	((unsigned)(attr) >> 25)

/* Flag bit which inhibits plane mask setting (for setting group only) */
#define	PIX_DONT_SET_PLANES	(1 << 24)
 
/*  Plane groups definitions */
#define	PIXPG_CURRENT		0
#define PIXPG_MONO		1
#define PIXPG_8BIT_COLOR	2
#define PIXPG_OVERLAY_ENABLE	3
#define PIXPG_OVERLAY	 	4
#define	PIXPG_INVALID		127

/* Plane groups functions */
extern int pr_available_plane_groups();
extern int pr_get_plane_group();
extern void pr_set_plane_group();
extern void pr_set_planes();

#endif	pr_planegroups_DEFINED
