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

/*
 *	File:		td_miriad.c
 *	Contents:	Miriad file calls
 *
 */

#include "td_com.h"


/*
 *	Load specified SDS into memory
 */
int
td_MiriadLoad(db,nColors,nSplitColors)
A_Data_t	*db;
int	nColors,nSplitColors;
{
	Boolean first=TRUE;
	int i,j,k,ret=0,tno=0,ind[3];
	float32 *tmp;

    db->rank = 3;
    db->min = 0.0; db->max = 10.0;

#ifdef MIRIADFMT
	xyopen_c(&tno,db->pathName,"old",db->rank,db->dims);
#endif

	if (tno == 0)
		return(-1);
	else
	{
		db->data = (float32 ***)
			td_Malloc3Dfloat32(db->dims[0],db->dims[1],db->dims[2]);
		tmp = (float32 *)td_Malloc1D(1,db->dims[0],sizeof(float32),
			"temp float32");
		if ((db->data != NULL) && (tmp != NULL))
		{
			for (i=0;i<db->dims[2];i++)
			{
				ind[0] = i+1;
#ifdef MIRIADFMT
				xysetpl_c(tno,1,ind);
#endif
				for (j=0;j<db->dims[1];j++)
				{
#ifdef MIRIADFMT
					xyread_c(tno,j+1,tmp);
#endif
					for (k=0;k<db->dims[0];k++)
					{
						db->data[k][j][i] = tmp[k];
						if (first)
						{
							db->min = db->max = db->data[0][0][0];
							first = FALSE;
						}
						else
						if (db->min > db->data[k][j][i])
							db->min = db->data[k][j][i];
						else
						if (db->max < db->data[k][j][i])
							db->max = db->data[k][j][i];
					}
				}
			}   
            db->range = db->max - db->min;
			db->rangeFrac = (float32)((float32)(nColors)/db->range);
			db->rangeFracSplit = (float32)((float32)(nSplitColors)/db->range);

			if (ret == -1)
			{
				sprintf(msg,
				"***ERROR: Cannot read 3D SDS from %s.\n",
					db->pathName);
				gr_TextMsgOut(msg);
				gr_TextMsgOut
				("Are you sure this is a 3D 32-bit floating point dataset?\n");
			}
		}
		else
		{
			td_Free3d(db->data);
			ret = -1;
			sprintf(msg,
			"***ERROR: Not enough memory to load MIRIAD image %s.\n",
					db->pathName);
			gr_TextMsgOut(msg);
			gr_TextMsgOut
			("I suggest you quit NCSA X DataSlice and retry the loading.\n");
		}

		return(ret);
	}
}
