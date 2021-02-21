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
 *  this file includes:
 *	ident
 *	pt_mult
 *	mat_mult
 *	s_trans_mat
 *	s_rot_mat
 */

#include <math.h>
#include <xgks.h>

/*
 *  ident
 *
 *  description:	sets transformation matrix to identity
 *
 *  parameters:		trans (Gfloat *) - transformation matrix
 *			(GKS format)
 */

ident(trans)
Gfloat *trans;
{
	trans[0] = 1;
	trans[1] = 0;
	trans[2] = 0;
	trans[3] = 0;
	trans[4] = 1;
	trans[5] = 0;
}  /* end ident */


/*
 *  pt_mult
 *
 *  description:	multiplies list of pts by transformation
 *			matrix
 *
 *  parameters:		pts (Gpoint *) - array of pts
 *			nopts (int) - no of pts
 *			trans (Gfloat *) - transformation matrix
 *			(GKS format)
 */

pt_mult(pts,nopts,trans)
Gpoint *pts;
int nopts;
Gfloat *trans;
{
	int i;
	Gfloat x,y;

	for (i=0; i<nopts; i++)
	{
		x = pts[i].x;
		y = pts[i].y;

		pts[i].x = trans[0] * x + trans[1] * y + trans[2];
		pts[i].y = trans[3] * x + trans[4] * y + trans[5];
	}
}  /* end pt_mult */


/*
 *  mat_mult
 *
 *  description:	multiplies two matrices
 *			result + mat1 * mat2
 *
 *  parameters:		mat1 (Gpoint *) - matrix in GKS format
 *			mat2 (Gpoint *) - matrix in GKS format
 *			result (Gpoint *) - resulting matrix
 */

mat_mult(mat1,mat2,result)
Gfloat *mat1;
Gfloat *mat2;
Gfloat *result;
{
	Gfloat temp[6];
	int i;

	temp[0] = mat1[0] * mat2[0] + mat1[3] * mat2[1];
	temp[1] = mat1[1] * mat2[0] + mat1[4] * mat2[1];
	temp[2] = mat1[2] * mat2[0] + mat1[5] * mat2[1] + mat2[2];
	temp[3] = mat1[0] * mat2[3] + mat1[3] * mat2[4];
	temp[4] = mat1[1] * mat2[3] + mat1[4] * mat2[4];
	temp[5] = mat1[2] * mat2[3] + mat1[5] * mat2[4] + mat2[5];

	for (i=0; i<6; i++)
		result[i] = temp[i];
}  /* end mat_mult */


/*
 *  s_trans_mat
 *
 *  descripton:		set up translation matrix
 *
 *  parameters:		trans (Gpoint) - transformation matrix
 *			x (Gfloat) - translation in x direction
 *			y (Gfloat) - translation in y direction
 */

s_trans_mat(trans,x,y)
Gfloat *trans;
Gfloat x,y;
{
	ident(trans);
	trans[2] = x;
	trans[5] = y;
}  /* end s_trans_mat */


/*
 *  s_rot_mat
 *
 *  description:	set up rotation matrix
 *
 *  parameters:		trans (Gpoint) - transformation matrix
 *			pt (Gpoint) - pivot point
 *			radians - roatation angle in degrees
 */

s_rot_mat(trans,pt,radians)
Gfloat *trans;
Gpoint pt;
Gfloat radians;
{
	Gfloat sin_theta, cos_theta;

	sin_theta = sin(radians);
	cos_theta = cos(radians);

	trans[0] = cos_theta;
	trans[1] = - sin_theta;
	trans[2] = (1.0 - cos_theta) * pt.x
		+ sin_theta * pt.y;
	trans[3] = sin_theta;
	trans[4] = cos_theta;
	trans[5] = (1.0 - cos_theta) * pt.y
		- sin_theta * pt.x;
}  /* end s_rot_trans */
