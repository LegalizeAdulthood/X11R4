/***********************************************************
Copyright International Business Machines Corporation 1989

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/


/******************** START OF MODULE SPECIFICATIONS *********************
*
* MODULE NAME:		imcore.c
*
* DESCRIPTIVE NAME:	IMCORE
*
* FUNCTION:		This file includes IMFEP functions and hooks for
*			the IMED functions.
*
* MODULE TYPE:		C
*
* COMPILER:		AIX C
*
* CHANGE ACTIVITY:	0%G% - Modified
*
* STATUS:		Input Method Version %Q%
*
******************** END OF SPECIFICATIONS ******************************/
 
/*-----------------------------------------------------------------------*
*	INCLUDE FILES
*-----------------------------------------------------------------------*/
#include <unistd.h>
#include "im.h"				/* definition of data type	*/
 
/*-----------------------------------------------------------------------*
*	EXTERNAL REFERENCES
*-----------------------------------------------------------------------*/
char	*strcpy(), *strcat(), *NLgetenv(); 
int	strlen();
int	load(), unload();
caddr_t	immalloc();
void	imfree();

/*-----------------------------------------------------------------------*
*	GROVAL VARIABLES
*-----------------------------------------------------------------------*/
int	imerrno = IMNoError;

/*************************************************************************
*
*	IMCORE
*
*************************************************************************/


/*-----------------------------------------------------------------------*
*
*	AreYouThere()
*		return a full path name if the module is accessable.
*		This is a local function.
*
*-----------------------------------------------------------------------*/

static	char	*AreYouThere(language)
IMLanguage	language;
{
	char	*pathname;
	char	*imdirectory;

	if (!language) {
		imerrno	= IMNoSuchLanguage;
		return NULL;
	}

	if (!(imdirectory = NLgetenv("IMPATH")))
		imdirectory = IMDIRECTORY;		/* /usr/lpp/IM */

	/*
	 * Are you there ?
	 */

	pathname = immalloc(strlen(imdirectory) + strlen(language) +
		sizeof IMMODULENAME + 2);
	(void)sprintf(pathname, "%s/%s/%s",
		imdirectory, language, IMMODULENAME);
	if (access(pathname, R_OK) < 0) {	/* have read access ? */
		imerrno	= IMNoSuchLanguage;
		imfree(pathname);
		return NULL;
	}

	return pathname;
}

/*-----------------------------------------------------------------------*
*
*	IMQueryLanguage()
*		check to see if the specified module exists.
*
*-----------------------------------------------------------------------*/

uint	IMQueryLanguage(language)
IMLanguage	language;
{
	char	*pathname;

	if (!(pathname = AreYouThere(language)))
		return FALSE;
	imfree(pathname);
	return TRUE;
}

/*-----------------------------------------------------------------------*
*
*	IMInitialize()
*		load the specified module and initialize it.
*
*-----------------------------------------------------------------------*/

IMFep		IMInitialize(language)
IMLanguage	language;
{
	IMFep	imfep;
	IMFep	(*entry)();
	char	*pathname;

	if (!(pathname = AreYouThere(language)))	/* look for module */
		return NULL;

	if (!(entry = (IMFep (*)())load(pathname))) {	/* try to load it */
		imerrno = IMCouldNotLoad;
		imfree(pathname);
		return NULL;
	}
	imfree(pathname);

	if (!(imfep = (*entry)(language))) {		/* initialize */
		imerrno = IMInitializeError;
		(void)unload((int)entry);
		return NULL;
	}

	imfep->language = immalloc((unsigned)strlen(language) + 1);
	(void)strcpy(imfep->language, language);	/* language stamp */

	imfep->version = IMVersionNumber;		/* version stamp */

	return imfep;
}

/*-----------------------------------------------------------------------*
*
*	IMClose()
*		unload the module
*
*-----------------------------------------------------------------------*/

void	IMClose(imfep)
IMFep	imfep;
{
	if (imfep) {
		imfree(imfep->language);
		(*imfep->imclose)(imfep);
		(void)unload((int)imfep->iminitialize);
	}
	return;
}

 
/*************************************************************************
*
*	IMFEP entry points.
*		Merely invoke each IMFEP entry.
*
*************************************************************************/


/*-----------------------------------------------------------------------*
*
*	IMCreate()
*
*-----------------------------------------------------------------------*/

IMObject	IMCreate(imfep, imcallback, udata)
IMFep		imfep;
caddr_t		imcallback, udata;
{
	IMObject	im;

	if (!imfep) {
		imerrno = IMInvalidParameter;
		return NULL;
	}
	if (!(im = (*imfep->imcreate)(imfep, imcallback, udata)))
		imerrno = imfep->imerrno;
	return im;
}

/*-----------------------------------------------------------------------*
*
*	IMDestroy()
*
*-----------------------------------------------------------------------*/

void		IMDestroy(im)
IMObject	im;
{
	if (im)
		(void)(*im->imfep->imdestroy)(im);
}

/*-----------------------------------------------------------------------*
*
*	IMProcess()
*
*-----------------------------------------------------------------------*/

int		IMProcess(im, key, state, str, len)
IMObject	im;
caddr_t		key, state, str, len;
{
	int	r;			/* return code */

	if (!im) {
		imerrno = IMInvalidParameter;
		return IMError;
	}
	if ((r = (*im->imfep->improcess)(im, key, state, str, len)) == IMError)
		imerrno = im->imfep->imerrno;
	return r;
}

/*-----------------------------------------------------------------------*
*
*	IMProcessAuxiliary()
*
*-----------------------------------------------------------------------*/

int		IMProcessAuxiliary(im, auxid, button,
			panel_row, panel_col, item_row, item_col)
IMObject	im;
caddr_t		auxid, button, panel_row, panel_col, item_row, item_col;
{
	int	r;			/* return code */

	if (!im) {
		imerrno = IMInvalidParameter;
		return IMError;
	}
	if ((r = (*im->imfep->improcessaux)(im, auxid, button,
		panel_row, panel_col, item_row, item_col)) == IMError)
		imerrno = im->imfep->imerrno;
	return r;
}

/*-----------------------------------------------------------------------*
*
*	IMIoctl()
*
*-----------------------------------------------------------------------*/

int		IMIoctl(im, op, arg)
IMObject	im;
caddr_t		op, arg;
{
	int	r;			/* return code */

	if (!im) {
		imerrno = IMInvalidParameter;
		return IMError;
	}
	if ((r = (*im->imfep->imioctl)(im, op, arg)) == IMError)
		imerrno = im->imfep->imerrno;
	return r;
}
