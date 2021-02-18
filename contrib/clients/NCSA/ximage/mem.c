/*
 * This NCSA software product is public domain software.  Permission
 * is granted to do whatever you like with it. Should you feel compelled 
 * to make a contribution for the use of this software, please send 
 * comments as to what you like or don't like about the software, 
 * how you use it, how it helps your work, etc. to softdev@ncsa.uiuc.edu.
 *
 * Please send bug reports to bugs@ncsa.uiuc.edu
 *
 * Author: Dave Thompson, National Center for Supercomputing Applications
 *         davet@ncsa.uiuc.edu
 */

char *XIMalloc(size) 
unsigned int size;
{
/*	return((char *) XtMalloc(size+512));*/
	return((char *) XtMalloc(size));
}

char *XICalloc(nelem,elsize)
unsigned int nelem,elsize;
{
char	*p;
char	*q;
int	x,size;
/*	return(calloc(nelem+10,elsize));*/
	return((char *) XtCalloc(nelem,elsize));

} /* XICalloc() */


void XIFree(ptr)
char *ptr;
{
/*	free(ptr);*/
	XtFree(ptr);

}
