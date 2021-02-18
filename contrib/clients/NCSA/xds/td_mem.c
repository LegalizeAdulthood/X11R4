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
 *	File:		td_mem.c
 *	Contents:	memory operations
 *
 */

#include "td_com.h"

void
td_Free3d(ptr)
float32	***ptr;
{
	if (ptr != NULL)
	{
		free(ptr);
		ptr = NULL;
	}
}

void
td_Free2dChar(ptr)
char **ptr;
{
	if (ptr != NULL)
	{
		free(ptr);
		ptr = NULL;
	}
}

void
td_Free2dInt(ptr)
int **ptr;
{
	if (ptr != NULL)
	{
		free(ptr);
		ptr = NULL;
	}
}

void
td_Free2dVRect(ptr)
A_VRect_t **ptr;
{
	if (ptr != NULL)
	{
		free(ptr);
		ptr = NULL;
	}
}

void
td_Free2d(ptr)
char	**ptr;
{
	int i=0;

	if (ptr != NULL)
	{
		while (ptr[i] != NULL)
		{
			free((char *)ptr[i]);
			i++;
		}
		free((char *)ptr);
		ptr = NULL;
	}
}

void
td_Free(ptr)
char	*ptr;
{
	if (ptr != NULL)
	{
		free((char *)ptr);
		ptr = NULL;
	}
}

float32
***td_Malloc3Dfloat32(x,y,z)
int32 	x,y,z;
{
	float32	***arr,*p,**q;
	int32	i,j;

	arr = (float32 ***)XtMalloc((unsigned)(x*sizeof(float32 **)+
						x*y*sizeof(float32 *)+
						x*y*z*sizeof(float32)));

	if (arr == NULL)
	{
		gr_TextMsgOut("***ERROR: Not enough memory when mallocing 3D SDS.\n");
		sprintf(msg,"%ld bytes required in td_Malloc3Dfloat32\n",
			x*sizeof(float32 **)+x*y*sizeof(float32 *)+x*y*z*sizeof(float32));
		gr_TextMsgOut(msg);
		return(NULL);
	}

	q = (float32 **)(arr+x);
	p = (float32 *)(q+x*y);
	for (i=0;i<x;i++)
	{
		arr[i] = q;
		q += y;
		for (j=0;j<y;j++)
		{
			arr[i][j] = p;
			p += z;
		}
	}

	return(arr);
}

A_VRect_t
**td_Malloc2DVRect(x,y,rem)
int 	x,y;
char	*rem;
{
	A_VRect_t **arr,*p;
	int	i;

	arr = (A_VRect_t **)XtMalloc((unsigned)(x*sizeof(A_VRect_t *) +
		x*y*sizeof(A_VRect_t)) );

	if (arr == NULL)
	{
		sprintf(msg,"***ERROR: Not enough memory in %s.\n",rem);
		gr_TextMsgOut(msg);
		sprintf(msg,"%ld bytes required in td_Malloc2DVRect\n",
			y*sizeof(A_VRect_t *)+x*y*sizeof(A_VRect_t));
		gr_TextMsgOut(msg);
		return(NULL);
	}

	p = (A_VRect_t *)(arr+x);
	for (i=0;i<x;i++)
	{
		arr[i] = p;
		p += y;
	}

	return(arr);
}

char
**td_Malloc2DChar(x,y,rem)
int 	x,y;
char	*rem;
{
	char **arr,*p;
	int	i;

	arr = (char **)XtMalloc((unsigned)(y*sizeof(char *)+
						x*y*sizeof(char)) );

	if (arr == NULL)
	{
		sprintf(msg,"***ERROR: Not enough memory in %s.\n",rem);
		gr_TextMsgOut(msg);
		sprintf(msg,"%ld bytes required in td_Malloc2DChar\n",
			y*sizeof(char *)+x*y*sizeof(char));
		gr_TextMsgOut(msg);
		return(NULL);
	}

	p = (char *)(arr+y);
	for (i=y-1;i>=0;i--)
	{
		arr[i] = p;
		p += x;
	}

	return(arr);
}

char
**td_Alloc2D(x,y,bytesize,rem)
int 	x,y;
int		bytesize;
char	*rem;
{
	char **arr,*p;
	int	i;

	arr = (char **)XtMalloc((unsigned)(y*sizeof(char *)+
						x*y*bytesize) );

	if (arr == NULL)
	{
		sprintf(msg,"***ERROR: Not enough memory in %s.\n",rem);
		gr_TextMsgOut(msg);
		sprintf(msg,"%ld bytes required in td_Malloc2DChar\n",
			y*sizeof(char *)+x*y*bytesize);
		gr_TextMsgOut(msg);
		return(NULL);
	}

	p = (char *)(arr+y);
	for (i=y-1;i>=0;i--)
	{
		arr[i] = p;
		p += x*bytesize;
	}

	return(arr);
}

int
**td_Malloc2DInt(x,y,rem)
int 	x,y;
char	*rem;
{
	int **arr,*p;
	int	i;

	arr = (int **)XtMalloc((unsigned)(y*sizeof(int *)+
						x*y*sizeof(int)) );

	if (arr == NULL)
	{
		sprintf(msg,"***ERROR: Not enough memory in %s.\n",rem);
		gr_TextMsgOut(msg);
		sprintf(msg,"%ld bytes required in td_Malloc2DInt\n",
			y*sizeof(int *)+x*y*sizeof(int));
		gr_TextMsgOut(msg);
		return(NULL);
	}

	p = (int *)(arr+y);
	for (i=y-1;i>=0;i--)
	{
		arr[i] = p;
		p += x;
	}

	return(arr);
}

char
*td_Malloc1D(x,y,size,rem)
int x,y;
unsigned long size;
char *rem;
{
	unsigned int elems;
	char *arr;

	elems = x*y;
	
	arr = (char *)XtCalloc(elems,size);
	if (arr == NULL)
	{
		sprintf(msg,"***ERROR: Not enough memory in %s.\n",rem);
		gr_TextMsgOut(msg);
		sprintf(msg,"%ld bytes required in td_Malloc1D.\n",elems*size);
		gr_TextMsgOut(msg);
	}

	return(arr);
}

char
**td_Malloc2D(x,y,size,rem)
int x,y;
unsigned long size;
char *rem;
{
	unsigned int elems;
	char	**arr;

	elems = (x*y)+1;
	
	arr = (char **)XtCalloc(elems,size);
	if (arr == NULL)
	{
		sprintf(msg,"***ERROR: Not enough memory in %s.\n",rem);
		gr_TextMsgOut(msg);
		sprintf(msg,"%ld bytes required in td_Malloc2D.\n",elems*size);
		gr_TextMsgOut(msg);
	}

	return(arr);
}

char
*td_Malloc(size,rem)
long size;
char *rem;
{
    char *tmp;

    tmp = (char *)XtMalloc(size);
    if (tmp == NULL)
    {
        sprintf(msg,"Not enough memory when mallocing %s.\n",rem);
        gr_TextMsgOut(msg);
        sprintf(msg,"%ld bytes required in td_Malloc.\n",size);
        gr_TextMsgOut(msg);
    }

    return(tmp);
}
