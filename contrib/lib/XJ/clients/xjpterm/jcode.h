/*
 *	Copyright 1989 OMRON TATEISI ELECTRONICS CO.
 *
 *	Author:	Seiji Kuwari	OMRON TATEISI ELECTRONICS CO.
 *				kuwa@frf.omron.junet
 *				uunet!nff.ncl.omron.co.jp!kuwa
 */				

/*	@(#)jcode.h	1.1	87/02/26	*/

#define	KI	"\033$B"	/* £Ê£É£Ó¥³¡Ý¥É´Á»ú¥¤¥ó¡¡¥¨¥¹¥±¡Ý¥×¥·¡Ý¥±¥ó¥¹ */
#define	KO	"\033(B"	/* £Ê£É£Ó¥³¡Ý¥É´Á»ú¥¢¥¦¥È¥¨¥¹¥±¡Ý¥×¥·¡Ý¥±¥ó¥¹ */

#ifndef lint
#ifndef WCHAR_T
typedef unsigned short int wchar_t;
#define WCHAR_T
#endif	/* WCHAR_T */
#endif	/* lint */
