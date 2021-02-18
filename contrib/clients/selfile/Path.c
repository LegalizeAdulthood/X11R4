#ifndef lint
static char rcsid[] = "$Header: Path.c,v 1.2 89/12/15 11:59:24 kit Exp $";
#endif

/*
 * Copyright 1989 Software Research Associates, Inc., Tokyo, Japan
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Software Research Associates
 * makes no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * SOFTWARE RESEARCH ASSOCIATES DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL SOFTWARE RESEARCH ASSOCIATES BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Erik M. van der Poel
 *         Software Research Associates, Inc., Tokyo, Japan
 *         erik@sra.co.jp
 */

#include <stdio.h>

#ifdef SEL_FILE_IGNORE_CASE
#include <ctype.h>
#endif /* def SEL_FILE_IGNORE_CASE */

#include <pwd.h>
#include "SFinternal.h"
#include <sys/stat.h>

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)

#include <Xw/Xw.h>
#include <Xw/ScrollBar.h>

#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

#ifdef SEL_FILE_PRE_R4_XAW_XMU
#include <X11/Scroll.h>
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
#include <X11/Xaw/Scrollbar.h>
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

#if defined(SYSV) || defined(USG)
extern unsigned short getuid();
extern void qsort();
#endif /* defined(SYSV) || defined(USG) */

typedef struct {
	char	*name;
	char	*dir;
} SFLogin;

SFDir *SFdirs = NULL;

int SFdirEnd;

int SFdirPtr;

int SFbuttonPressed = 0;

static int SFdoNotTouchDirPtr = 0;

static int SFdoNotTouchVorigin = 0;

static SFDir SFrootDir, SFhomeDir;

static SFLogin *SFlogins;

static int SFtwiddle = 0;

int
SFchdir(path)
	char	*path;
{
	int	result;

	result = 0;

	if (strcmp(path, SFcurrentDir)) {
		result = chdir(path);
		if (!result) {
			(void) strcpy(SFcurrentDir, path);
		}
	}

	return result;
}

static
SFfree(i)
	int	i;
{
	register SFDir	*dir;
	register int	j;

	dir = &(SFdirs[i]);

	for (j = dir->nEntries - 1; j >= 0; j--) {
		if (dir->entries[j].shown != dir->entries[j].real) {
			XtFree(dir->entries[j].shown);
		}
		XtFree(dir->entries[j].real);
	}

	XtFree((char *) dir->entries);

	XtFree(dir->dir);

	dir->dir = NULL;
}

static
SFunreadableDir(dir)
	SFDir	*dir;
{
	char	*cannotOpen = "<cannot open> ";

	dir->entries = (SFEntry *) XtMalloc(sizeof(SFEntry));
	dir->entries[0].statDone = 1;
	dir->entries[0].real = XtMalloc((unsigned) (strlen(cannotOpen) + 1));
	(void) strcpy(dir->entries[0].real, cannotOpen);
	dir->entries[0].shown = dir->entries[0].real;
	dir->nEntries = 1;
	dir->nChars = strlen(cannotOpen);
}

#ifdef SEL_FILE_IGNORE_CASE
static
SFstrncmp(p, q, n)
	register char	*p, *q;
	register int	n;
{
	register char	c1, c2;
	char		*psave, *qsave;
	int		nsave;

	psave = p;
	qsave = q;
	nsave = n;

	c1 = *p++;
	if (islower(c1)) {
		c1 = toupper(c1);
	}
	c2 = *q++;
	if (islower(c2)) {
		c2 = toupper(c2);
	}

	while ((--n >= 0) && (c1 == c2)) {
		if (!c1) {
			return strncmp(psave, qsave, nsave);
		}
		c1 = *p++;
		if (islower(c1)) {
			c1 = toupper(c1);
		}
		c2 = *q++;
		if (islower(c2)) {
			c2 = toupper(c2);
		}
	}

	if (n < 0) {
		return strncmp(psave, qsave, nsave);
	}

	return c1 - c2;
}
#endif /* def SEL_FILE_IGNORE_CASE */

static
SFreplaceText(dir, str)
	SFDir	*dir;
	char	*str;
{
	int	len;

	*(dir->path) = 0;
	len = strlen(str);
	if (str[len - 1] == '/') {
		(void) strcat(SFcurrentPath, str);
	} else {
		(void) strncat(SFcurrentPath, str, len - 1);
	}
	if (strncmp(SFcurrentPath, SFstartDir, strlen(SFstartDir))) {
		SFsetText(SFcurrentPath);
	} else {
		SFsetText(&(SFcurrentPath[strlen(SFstartDir)]));
	}

	SFtextChanged();
}

static int
SFexpand(str)
	char	*str;
{
	int	len;
	int	cmp;
	char	*name, *growing;
	SFDir	*dir;
	SFEntry	*entry, *max;

	len = strlen(str);

	dir = &(SFdirs[SFdirEnd - 1]);

	if (dir->beginSelection == -1) {
		str = strcpy(XtMalloc((unsigned) (strlen(str) + 1)), str);
		SFreplaceText(dir, str);
		XtFree(str);
		return;
	} else if (dir->beginSelection == dir->endSelection) {
		SFreplaceText(dir, dir->entries[dir->beginSelection].shown);
		return;
	}

	max = &(dir->entries[dir->endSelection + 1]);

	name = dir->entries[dir->beginSelection].shown;
	(void) strcpy((growing = XtMalloc((unsigned) (strlen(name) + 1))),
		name);

	cmp = 0;
	while (!cmp) {
		entry = &(dir->entries[dir->beginSelection]);
		while (entry < max) {
			if (cmp = strncmp(growing, entry->shown, len)) {
				break;
			}
			entry++;
		}
		len++;
	}

	/*
	 * SFreplaceText() expects filename
	 */
	growing[len - 2] = ' ';

	growing[len - 1] = 0;
	SFreplaceText(dir, growing);
	XtFree(growing);
}

static int
SFfindFile(dir, str)
	SFDir		*dir;
	register char	*str;
{
	register int	i, last, max;
	register char	*name, save;
	SFEntry		*entries;
	int		len;
	int		begin, end;
	int		result;

	len = strlen(str);

	if (str[len - 1] == ' ') {
		SFexpand(str);
		return 1;
	} else if (str[len - 1] == '/') {
		len--;
	}

	max = dir->nEntries;

	entries = dir->entries;

	i = 0;
	while (i < max) {
		name = entries[i].shown;
		last = strlen(name) - 1;
		save = name[last];
		name[last] = 0;

#ifdef SEL_FILE_IGNORE_CASE
		result = SFstrncmp(str, name, len);
#else /* def SEL_FILE_IGNORE_CASE */
		result = strncmp(str, name, len);
#endif /* def SEL_FILE_IGNORE_CASE */

		name[last] = save;
		if (result <= 0) {
			break;
		}
		i++;
	}
	begin = i;
	while (i < max) {
		name = entries[i].shown;
		last = strlen(name) - 1;
		save = name[last];
		name[last] = 0;

#ifdef SEL_FILE_IGNORE_CASE
		result = SFstrncmp(str, name, len);
#else /* def SEL_FILE_IGNORE_CASE */
		result = strncmp(str, name, len);
#endif /* def SEL_FILE_IGNORE_CASE */

		name[last] = save;
		if (result) {
			break;
		}
		i++;
	}
	end = i;

	if (begin != end) {
		if (
			(dir->beginSelection != begin) ||
			(dir->endSelection != end - 1)
		) {
			dir->changed = 1;
			dir->beginSelection = begin;
			if (str[strlen(str) - 1] == '/') {
				dir->endSelection = begin;
			} else {
				dir->endSelection = end - 1;
			}
		}
	} else {
		if (dir->beginSelection != -1) {
			dir->changed = 1;
			dir->beginSelection = -1;
			dir->endSelection = -1;
		}
	}

	if (
		SFdoNotTouchVorigin ||
		((begin > dir->vOrigin) && (end < dir->vOrigin + SFlistSize))
	) {
		SFdoNotTouchVorigin = 0;
		return 0;
	}

	i = begin - 1;
	if (i > max - SFlistSize) {
		i = max - SFlistSize;
	}
	if (i < 0) {
		i = 0;
	}

	if (dir->vOrigin != i) {
		dir->vOrigin = i;
		dir->changed = 1;
	}

	return 0;
}

static
SFunselect()
{
	SFDir	*dir;

	dir = &(SFdirs[SFdirEnd - 1]);
	if (dir->beginSelection != -1) {
		dir->changed = 1;
	}
	dir->beginSelection = -1;
	dir->endSelection = -1;
}

static int
SFcompareLogins(p, q)
	SFLogin	*p, *q;
{
	return strcmp(p->name, q->name);
}

static
SFgetHomeDirs()
{
	struct passwd	*pw;
	struct stat	statBuf;
	int		alloc;
	int		i;
	SFEntry		*entries;
	int		len;
	int		maxChars;

	alloc = 0;
	i = 0;

	maxChars = -1;

	if (pw = getpwuid((int) getuid())) {
		if (
			(!stat(pw->pw_dir, &statBuf)) &&
			((statBuf.st_mode & S_IFMT) == S_IFDIR)
		) {
			alloc = 1;
			i = 1;
			entries = (SFEntry *) XtMalloc(sizeof(SFEntry));
			SFlogins = (SFLogin *) XtMalloc(sizeof(SFLogin));
			entries[0].real = XtMalloc(2);
			(void) strcpy(entries[0].real, "~");
			entries[0].shown = entries[0].real;
			entries[0].statDone = 1;
			SFlogins[0].name = "";
			SFlogins[0].dir = XtMalloc((unsigned)
				(strlen(pw->pw_dir) + 1));
			(void) strcpy(SFlogins[0].dir, pw->pw_dir);
		}
	}

	(void) setpwent();

	while ((pw = getpwent()) && (*(pw->pw_name))) {
		if (
			(!stat(pw->pw_dir, &statBuf)) &&
			((statBuf.st_mode & S_IFMT) == S_IFDIR)
		) {
			if (i >= alloc) {
				alloc *= 2;
				entries = (SFEntry *) XtRealloc(
					(char *) entries,
					(unsigned) (alloc * sizeof(SFEntry))
				);
				SFlogins = (SFLogin *) XtRealloc(
					(char *) SFlogins,
					(unsigned) (alloc * sizeof(SFLogin))
				);
			}
			len = strlen(pw->pw_name);
			entries[i].real = XtMalloc((unsigned) (len + 3));
			(void) strcat(strcpy(entries[i].real, "~"),
				pw->pw_name);
			entries[i].shown = entries[i].real;
			entries[i].statDone = 1;
			if (len > maxChars) {
				maxChars = len;
			}
			SFlogins[i].name = XtMalloc((unsigned)
				(strlen(pw->pw_name) + 1));
			(void) strcpy(SFlogins[i].name, pw->pw_name);
			SFlogins[i].dir = XtMalloc((unsigned)
				(strlen(pw->pw_dir) + 1));
			(void) strcpy(SFlogins[i].dir, pw->pw_dir);
			i++;
		}
	}

	SFhomeDir.dir			= XtMalloc(1)	;
	SFhomeDir.dir[0]		= 0		;
	SFhomeDir.path			= SFcurrentPath	;
	SFhomeDir.entries		= entries	;
	SFhomeDir.nEntries		= i		;
	SFhomeDir.vOrigin		= 0		;	/* :-) */
	SFhomeDir.nChars		= maxChars + 2	;
	SFhomeDir.hOrigin		= 0		;
	SFhomeDir.changed		= 1		;
	SFhomeDir.beginSelection	= -1		;
	SFhomeDir.endSelection		= -1		;

#if defined(SYSV) || defined(USG)
	qsort((char *) entries, (unsigned)i, sizeof(SFEntry), SFcompareEntries);
	qsort((char *) SFlogins, (unsigned)i, sizeof(SFLogin), SFcompareLogins);
#else /* defined(SYSV) || defined(USG) */
	qsort((char *) entries, i, sizeof(SFEntry), SFcompareEntries);
	qsort((char *) SFlogins, i, sizeof(SFLogin), SFcompareLogins);
#endif /* defined(SYSV) || defined(USG) */

	for (i--; i >= 0; i--) {
		(void) strcat(entries[i].real, "/");
	}
}

static int
SFfindHomeDir(begin, end)
	char	*begin, *end;
{
	char	save;
	char	*theRest;
	int	i;

	save = *end;
	*end = 0;

	for (i = SFhomeDir.nEntries - 1; i >= 0; i--) {
		if (!strcmp(SFhomeDir.entries[i].real, begin)) {
			*end = save;
			theRest = XtMalloc((unsigned) (strlen(end) + 1));
			(void) strcpy(theRest, end);
			(void) strcat(strcat(strcpy(SFcurrentPath,
				SFlogins[i].dir), "/"), theRest);
			XtFree(theRest);
			SFsetText(SFcurrentPath);
			SFtextChanged();
			return 1;
		}
	}

	*end = save;

	return 0;
}

SFupdatePath()
{
	static int	alloc;
	static int	wasTwiddle = 0;
	char		*begin, *end;
	int		i, j;
	int		len;
	int		prevChange;
	int		SFdirPtrSave, SFdirEndSave;
	SFDir		*dir;

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
	Arg		arglist[20];
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	if (!SFdirs) {
		SFdirs = (SFDir *) XtMalloc((alloc = 10) * sizeof(SFDir));
		dir = &(SFdirs[0]);
		dir->dir = XtMalloc(2);
		(void) strcpy(dir->dir, "/");
		(void) SFchdir("/");
		(void) SFgetDir(dir);
		for (j = 1; j < alloc; j++) {
			SFdirs[j].dir = NULL;
		}
		dir->path = SFcurrentPath + 1;
		dir->vOrigin = 0;
		dir->hOrigin = 0;
		dir->changed = 1;
		dir->beginSelection = -1;
		dir->endSelection = -1;
		SFgetHomeDirs();
	}

	SFdirEndSave = SFdirEnd;
	SFdirEnd = 1;

	SFdirPtrSave = SFdirPtr;
	SFdirPtr = 0;

	begin = NULL;

	if (SFcurrentPath[0] == '~') {
		if (!SFtwiddle) {
			SFtwiddle = 1;
			dir = &(SFdirs[0]);
			SFrootDir = *dir;
			*dir = SFhomeDir;
			dir->changed = 1;
		}
		end = SFcurrentPath;
		SFdoNotTouchDirPtr = 1;
		wasTwiddle = 1;
	} else {
		if (SFtwiddle) {
			SFtwiddle = 0;
			dir = &(SFdirs[0]);
			*dir = SFrootDir;
			dir->changed = 1;
		}
		end = SFcurrentPath + 1;
	}

	i = 0;

	prevChange = 0;

	while (*end) {
		while (*end++ == '/') {
			;
		}
		end--;
		begin = end;
		while ((*end) && (*end++ != '/')) {
			;
		}
		if ((end - SFcurrentPath <= SFtextPos) && (*(end - 1) == '/')) {
			SFdirPtr = i - 1;
			if (SFdirPtr < 0) {
				SFdirPtr = 0;
			}
		}
		if (*begin) {
			if (*(end - 1) == '/') {
				char save = *end;

				if (SFtwiddle) {
					if (SFfindHomeDir(begin, end)) {
						return;
					}
				}
				*end = 0;
				i++;
				SFdirEnd++;
				if (i >= alloc) {
					SFdirs = (SFDir *) XtRealloc(
						(char *) SFdirs,
						(unsigned) ((alloc *= 2) *
							sizeof(SFDir))
					);
					for (j = alloc / 2; j < alloc; j++) {
						SFdirs[j].dir = NULL;
					}
				}
				dir = &(SFdirs[i]);
				if (
					(!(dir->dir)) ||
					prevChange ||
					strcmp(dir->dir, begin)
				) {
					if (dir->dir) {
						SFfree(i);
					}
					prevChange = 1;
					len = strlen(begin) + 1;
					dir->dir = XtMalloc((unsigned) len);
					(void) strcpy(dir->dir, begin);
					dir->path = end;
					dir->vOrigin = 0;
					dir->hOrigin = 0;
					dir->changed = 1;
					dir->beginSelection = -1;
					dir->endSelection = -1;
					(void) SFfindFile(dir - 1, begin);
					if (
						SFchdir(SFcurrentPath) ||
						SFgetDir(dir)
					) {
						SFunreadableDir(dir);
						break;
					}
				}
				*end = save;
				if (!save) {
					SFunselect();
				}
			} else {
				if (SFfindFile(&(SFdirs[SFdirEnd-1]), begin)) {
					return;
				}
			}
		} else {
			SFunselect();
		}
	}

	if ((end == SFcurrentPath + 1) && (!SFtwiddle)) {
		SFunselect();
	}

	for (i = SFdirEnd; i < alloc; i++) {
		if (SFdirs[i].dir) {
			SFfree(i);
		}
	}

	if (SFdoNotTouchDirPtr) {
		if (wasTwiddle) {
			wasTwiddle = 0;
			SFdirPtr = SFdirEnd - 2;
			if (SFdirPtr < 0) {
				SFdirPtr = 0;
			}
		} else {
			SFdirPtr = SFdirPtrSave;
		}
		SFdoNotTouchDirPtr = 0;
	}

	if ((SFdirPtr != SFdirPtrSave) || (SFdirEnd != SFdirEndSave)) {

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR)
		i = 0;
		XtSetArg(arglist[i], XtNsliderMax, SFdirEnd + 3 - 1);	i++;
		XtSetArg(arglist[i], XtNsliderOrigin, SFdirPtr);	i++;
		XtSetValues(selFileHScroll, arglist, i);
#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

#ifdef SEL_FILE_PRE_R4_XAW_XMU
		XtScrollBarSetThumb(
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
		XawScrollBarSetThumb(
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

			selFileHScroll,
			(float) (((double) SFdirPtr) / SFdirEnd),
			(float) (((double) ((SFdirEnd < 3) ? SFdirEnd : 3)) /
				SFdirEnd)
		);
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWSCROLLBAR) */

	}

	if (SFdirPtr != SFdirPtrSave) {
		SFdrawLists(SF_DO_SCROLL);
	} else {
		for (i = 0; i < 3; i++) {
			if (SFdirPtr + i < SFdirEnd) {
				if (SFdirs[SFdirPtr + i].changed) {
					SFdirs[SFdirPtr + i].changed = 0;
					SFdrawList(i, SF_DO_SCROLL);
				}
			} else {
				SFclearList(i, SF_DO_SCROLL);
			}
		}
	}
}

SFsetText(path)
	char	*path;
{

#if defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT)

#ifdef SEL_FILE_JAPANESE
	static wchar_t	*wstr = NULL;
	static int	alloc = 0;
	int		len;

	len = convEUCtoJWS((unsigned char *) path, (wchar_t *) NULL);
	while (len + 1 > alloc) {
		alloc = 2 * (alloc + 1);
		wstr = (wchar_t *) XtRealloc((char *) wstr, (unsigned) (alloc *
			sizeof(wchar_t)));
	}
	(void) convEUCtoJWS((unsigned char *) path, wstr);

	XwTextClearBuffer(selFileField);
	XwTextInsert(selFileField, wstr);
#else /* def SEL_FILE_JAPANESE */
	XwTextClearBuffer(selFileField);
	XwTextInsert(selFileField, path);
#endif /* def SEL_FILE_JAPANESE */

#else /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */
	{

#ifdef SEL_FILE_PRE_R4_XAW_XMU
		XtTextBlock	text;
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
		XawTextBlock	text;
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

		text.firstPos = 0;
		text.length = strlen(path);
		text.ptr = path;
		text.format = FMT8BIT;

#ifdef SEL_FILE_PRE_R4_XAW_XMU
		XtTextReplace(selFileField, 0, strlen(SFtextBuffer), &text);
#else /* def SEL_FILE_PRE_R4_XAW_XMU */
		XawTextReplace(selFileField, 0, strlen(SFtextBuffer), &text);
#endif /* def SEL_FILE_PRE_R4_XAW_XMU */

	}
#endif /* defined(SEL_FILE_XW) || defined(SEL_FILE_XWTEXTEDIT) */

}

/* ARGSUSED */
void
SFbuttonPressList(w, n, event)
	Widget			w;
	int			n;
	XButtonPressedEvent	*event;
{
	SFbuttonPressed = 1;
}

/* ARGSUSED */
void
SFbuttonReleaseList(w, n, event)
	Widget			w;
	int			n;
	XButtonReleasedEvent	*event;
{
	SFDir	*dir;

	SFbuttonPressed = 0;

	if (SFcurrentInvert[n] != -1) {
		if (n < 2) {
			SFdoNotTouchDirPtr = 1;
		}
		SFdoNotTouchVorigin = 1;
		dir = &(SFdirs[SFdirPtr + n]);
		SFreplaceText(
			dir,
			dir->entries[dir->vOrigin + SFcurrentInvert[n]].shown
		);
		SFmotionList(w, n, event);
	}
}

static int
SFcheckDir(n, dir)
	int		n;
	SFDir		*dir;
{
	struct stat	statBuf;
	int		i;

	if (
		(!stat(".", &statBuf)) &&
		(statBuf.st_mtime != dir->st_mtime)
	) {

		/*
		 * If the pointer is currently in the window that we are about
		 * to update, we must warp it to prevent the user from
		 * accidentally selecting the wrong file.
		 */
		if (SFcurrentInvert[n] != -1) {
			XWarpPointer(
				SFdisplay,
				None,
				XtWindow(selFileLists[n]),
				0,
				0,
				0,
				0,
				0,
				0
			);
		}

		for (i = dir->nEntries - 1; i >= 0; i--) {
			if (dir->entries[i].shown != dir->entries[i].real) {
				XtFree(dir->entries[i].shown);
			}
			XtFree(dir->entries[i].real);
		}
		XtFree((char *) dir->entries);
		if (SFgetDir(dir)) {
			SFunreadableDir(dir);
		}
		if (dir->vOrigin > dir->nEntries - SFlistSize) {
			dir->vOrigin = dir->nEntries - SFlistSize;
		}
		if (dir->vOrigin < 0) {
			dir->vOrigin = 0;
		}
		if (dir->hOrigin > dir->nChars - SFcharsPerEntry) {
			dir->hOrigin = dir->nChars - SFcharsPerEntry;
		}
		if (dir->hOrigin < 0) {
			dir->hOrigin = 0;
		}
		dir->beginSelection = -1;
		dir->endSelection = -1;
		SFdoNotTouchVorigin = 1;
		if ((dir + 1)->dir) {
			(void) SFfindFile(dir, (dir + 1)->dir);
		} else {
			(void) SFfindFile(dir, dir->path);
		}

#ifndef SEL_FILE_R2_XT

		if (!SFworkProcAdded) {
			(void) XtAppAddWorkProc(SFapp, SFworkProc, NULL);
			SFworkProcAdded = 1;
		}

#endif /* ndef SEL_FILE_R2_XT */

		return 1;
	}

	return 0;
}

static int
SFcheckFiles(dir)
	SFDir	*dir;
{
	int		from, to;
	int		result;
	char		old, new;
	int		i;
	char		*str;
	int		last;
	struct stat	statBuf;

	result = 0;

	from = dir->vOrigin;
	to = dir->vOrigin + SFlistSize;
	if (to > dir->nEntries) {
		to = dir->nEntries;
	}

	for (i = from; i < to; i++) {
		str = dir->entries[i].real;
		last = strlen(str) - 1;
		old = str[last];
		str[last] = 0;
		if (stat(str, &statBuf)) {
			new = ' ';
		} else {
			switch (statBuf.st_mode & S_IFMT) {
			case S_IFDIR:
				new = '/';
				break;
			case S_IFREG:
				if (statBuf.st_mode & 0111) {
					new = '*';
				} else {
					new = ' ';
				}
				break;

#ifdef S_IFSOCK
			case S_IFSOCK:
				new = '=';
				break;
#endif /* def S_IFSOCK */

			default:
				new = ' ';
				break;
			}
		}
		str[last] = new;
		if (new != old) {
			result = 1;
		}
	}

	return result;
}

SFdirModTimer()
{
	static int	n = -1;
	static int	f = 0;
	char		save;
	SFDir		*dir;

	if ((!SFtwiddle) && (SFdirPtr < SFdirEnd)) {
		n++;
		if ((n > 2) || (SFdirPtr + n >= SFdirEnd)) {
			n = 0;
			f++;
			if ((f > 2) || (SFdirPtr + f >= SFdirEnd)) {
				f = 0;
			}
		}
		dir = &(SFdirs[SFdirPtr + n]);
		save = *(dir->path);
		*(dir->path) = 0;
		if (SFchdir(SFcurrentPath)) {
			*(dir->path) = save;

			/*
			 * force a re-read
			 */
			*(dir->dir) = 0;

			SFupdatePath();
		} else {
			*(dir->path) = save;
			if (
				SFcheckDir(n, dir) ||
				((f == n) && SFcheckFiles(dir))
			) {
				SFdrawList(n, SF_DO_SCROLL);
			}
		}
	}

#ifdef SEL_FILE_R2_XT

	SFdirModTimerId = XtAddTimeOut((unsigned long) 1000,

#else /* def SEL_FILE_R2_XT */

	SFdirModTimerId = XtAppAddTimeOut(SFapp, (unsigned long) 1000,

#endif /* def SEL_FILE_R2_XT */

		SFdirModTimer, (caddr_t) NULL);
}
