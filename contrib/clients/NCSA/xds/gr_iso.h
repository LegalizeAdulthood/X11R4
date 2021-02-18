/*
 * National Center for SuperComputing Applications, University of Illinois
 *
 * This NCSA software product is public domain software.  Permission
 * is hereby granted to do whatever you like with it. Should you wish
 * to make a contribution towards the production of this software, please
 * send us your comments about your experience with the software,  why
 * you liked or disliked it, how you use it, and most importantly, how it
 * helps your work. We will receive your comments at softdev@ncsa.uiuc.edu.
 *
 * Please send bug reports to bugs@ncsa.uiuc.edu
 *
 * Author: Eng-Whatt Toh, National Center for Supercomputing Applications
 *         ewtoh@ncsa.uiuc.edu
 */

extern char **td_Alloc2D();
extern void triangTetra();
extern void mcubeOneValue();
extern void triangCube();

typedef	struct {
	int	good;	/* non-zero if value has been set */
	float32	x, y, z;
	} InterpCoor;
			
typedef	struct {
	InterpCoor	h;	/* horizontal */
	InterpCoor	v;	/* vertical */
	InterpCoor	d;	/* diagonal, either / or \, but not both */
	} InterpData;
