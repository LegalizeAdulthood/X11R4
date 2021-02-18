/* 
 *		Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 * These procedures are used to convert the enumerated data types of xGKS into integers
 * and back; the integer codes of the "fake" ENUMS of the old version are used, but 
 * basically any kind of coding could be used (though picture files coded one way would
 * not make sense to a sled program compiled using a different code).
 * The idea is to make the code independent of the way these types are defined
 * in GKS.
 *
 * The conversion functions are:
 *	num_to_style
 *	style_to_num
 *	num_to_vert
 *	vert_to_num
 *	num_to_horz
 *	horz_to_num
 *	num_to_path
 *	path_to_num
 *	num_to_vis
 *	vis_to_num
 */

#include <xgks.h>


Gflinter num_to_style(i)
int i;
{
	Gflinter prov;
	if (i == 1)
		prov = GSOLID; 
	else	/* default */
		prov = GHOLLOW; 
	return(prov);
}

style_to_num(style)
Gflinter style;
{
	int i;
	if (style == GSOLID)
		i = 1;
	else
		i = 0;
	return(i);
}
		

Gtxver num_to_vert(i)
int i;
{
	Gtxver prov;
	switch (i)
	{
		case 1: prov = GTV_TOP; break;
		case 2: prov = GTV_CAP; break;
		case 3: prov = GTV_HALF; break;
		case 4: prov = GTV_BASE; break;
		case 5: prov = GTV_BOTTOM; break;
		case 6: prov = GTV_NORMAL; break;
		default: prov = GTV_NORMAL; 
	} /* end switch */
	return(prov);
}

vert_to_num(ver)
Gtxver ver;
{
	int i;
	switch (ver)
	{
		case GTV_TOP: i = 1; break;
		case GTV_CAP: i = 2; break;
		case GTV_HALF: i = 3; break;
		case GTV_BASE: i = 4; break;
		case GTV_BOTTOM: i = 5; break;
		case GTV_NORMAL: i = 6; break;
		default: i = 6;
	}
	return(i);
}

Gtxhor num_to_horz(i)
int i;
{
	Gtxhor prov;
	switch (i)
	{
		case 1: prov = GTH_RIGHT; break;
		case 2: prov = GTH_LEFT; break;
		case 3: prov = GTH_CENTER; break;
		case 6: prov = GTH_NORMAL; break;
		default: prov = GTH_NORMAL;
	} /* end switch */
	return(prov);
}

horz_to_num(hor)
Gtxhor hor;
{
	int i;
	switch (hor)
	{
		case GTH_RIGHT: i = 1; break;
		case GTH_LEFT: i = 2; break;
		case GTH_CENTER: i = 3; break;
		case GTH_NORMAL: i = 6; break;
		default: i = 6;
	}
	return(i);
}

Gtxpath num_to_path(i)
int i;
{
	Gtxpath prov;
	switch (i)
	{
		case 1: prov = GTP_RIGHT; break;
		case 2: prov = GTP_LEFT; break;
		case 3: prov = GTP_UP; break;
		case 4: prov = GTP_DOWN; break;
		default: prov = GTP_RIGHT;
	} /* end switch */
	return(prov);
}

path_to_num(p)
Gtxpath p;
{
	int i;
	switch (p)
	{
		case GTP_RIGHT: i = 1; break;
		case GTP_LEFT: i = 2; break;
		case GTP_UP: i = 3; break;
		case GTP_DOWN: i = 4; break;
		default: i = 1;
	}
	return(i);
}

Gsegvis num_to_vis(i)
int i;
{
	Gsegvis vis;
	if (i == 1)
		vis = GVISIBLE;
	else
		vis = GINVISIBLE;
	return(vis);
}

vis_to_num(vis)
Gsegvis vis;
{
	int i;
	if (vis == GVISIBLE)
		i = 1;
	else
		i = 0;
	return(i);
}
