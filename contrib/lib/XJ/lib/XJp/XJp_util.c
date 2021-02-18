/*
 * Copyright 1989 FUJITSU LIMITED
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of FUJITSU not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  FUJITSU makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * FUJITSU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL FUJITSU
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Masahiko Narita, FUJITSU LIMITED
 * 	   Yoshitaka Kakuta, FUJITSU LIMITED
 * 	   Shuuichi Akagawa, FUJITSU LIMITED
 *         kakuta@sysrap.ks.fujitsu.co.jp
 */

/* $Header: JXp_util.c,v 2.100 89/06/01 08:42:10 rws Exp $ */

#include <stdio.h>
#include <X11/Xlib.h>
#if !defined(EWS) && !defined(FACOM) && !defined(OMRON) && !defined(UX386)
#include <X11/Xos.h>
#endif
#ifdef UX386
#include <net/in.h>
#else
#include <netinet/in.h>
#endif
#include "XJplib.h"
#include "XJplibint.h"
#include "XJp_errno.h"


int code_conv(string, code, kanji_len)
unsigned char *string;
int 		code;
int 		kanji_len;			/* kanji no kazu */
{
	int 		string_len;
	int 		cnt;
	int 		i;
	unsigned char 	*buffer;
	extern int 	XJp_errno;

	string_len = strlen (string);
	buffer = (unsigned char *)malloc(string_len+1);
	bcopy (string, buffer, string_len);
	switch(code) {
		case XJP_EUC:
			for (cnt=0;cnt<string_len;cnt++) {
				*(string+cnt) |= 0x80;
			}
			break;
		case XJP_SJIS:
			for (cnt=0;cnt<string_len;cnt+=2) {
				Jis_SJis((buffer+cnt), (string+cnt));
			}
			break;
		default:
			free(buffer); /* dit */
			return (-1);
	}
	free(buffer); /* dit */
}


#define FIRS	0
#define SECN	1

Jis_SJis(jis, sjis)
unsigned char *jis, *sjis;
{
	unsigned short jis1, jis2;
	unsigned short sjis1, sjis2;

	jis1 = (short)jis[0];
	jis2 = (short)jis[1];
	if (jis1 <= 0x5e)
		sjis1 = (jis1 >> 1) + (0x81 - (0x21 >> 1));
	else
		sjis1 = (jis1 >> 1) + (0xe0 - (0x5f >> 1));
	if (jis1 & 1) {
		sjis2 = jis2 + (0x40 - 0x21);
		if (sjis2 >= 0x7f)
			sjis2++;
	} else {
		--sjis1;
		sjis2 = jis2 + (0x9f - 0x21);
	}
	sjis[0] = (char)sjis1;
	sjis[1] = (char)sjis2;
}


int
W_id_check(id)
XJp_id	id;
{
	int			i;
	extern unsigned long	_KeepW[MAXWINDOW][2];

	for (i=0;i<MAXWINDOW;i++) {
		if (_KeepW[i][HWID] == id)
			break;
	}
	return(i);
}

/* *************************************************
        Property data set
************************************************* */

int Data_Set(PropData, dispmode, mask, attri, window, aux_data)
unsigned char   *PropData;
XJp_attributes  *attri;
long            dispmode;
unsigned long   mask;
Window          window;
char   		*aux_data;
{
        int             i,j;
        int             Font_Size = 0;
        char            font_name[512];
	extern int 	Xjp_reeno;
	extern int 	_count;
	extern char	*_ascii[];

#ifdef DEBUG
        fprintf (stderr,"XJPlib: dispmode = %d\n",dispmode);
#endif
	if (dispmode > XJP_ROOT) {
		XJp_errno = XJP_L_OPEN_BAD_DM;
		return((XJp_id)-1);
	}
        AttributesSetL (PropData, htonl(dispmode), 0);
        SETDATA (aux_data, PropData+DATA, 128);
        AttributesSetL (PropData, htonl(window), 132);
#ifdef DEBUG
        fprintf(stderr,"XJPlib: window = %X\n",window);
#endif
        AttributesSetL (PropData, htonl(mask), 136);
#ifdef DEBUG
        fprintf (stderr,"XJPlib: mask = %X\n",mask);
#endif
        AttributesSetL (PropData, htonl(attri->p_width), 140);
        AttributesSetL (PropData, htonl(attri->p_height), 144);
        AttributesSetL (PropData, htonl(attri->x), 148);
        AttributesSetL (PropData, htonl(attri->y), 152);
        AttributesSetL (PropData, htonl(attri->width), 156);
        AttributesSetL (PropData, htonl(attri->height), 160);
        AttributesSetL (PropData, htonl(attri->fg), 164);
        AttributesSetL (PropData, htonl(attri->bg), 168);
        AttributesSetL (PropData, htonl(attri->bp), 172);
	AttributesSetL (PropData, htonl(attri->status_x), 176);
	AttributesSetL (PropData, htonl(attri->status_y), 180);
        if (mask & XJP_FONTS) {
        	Font_Size = 
		    Font_Name_Copy(attri->fonts, attri->num_fonts, font_name);
	} else {
		_ascii[_count] = XJP_romanX0201;
	}
        if (Font_Size != 0) {
                SETDATA (font_name, PropData+184, Font_Size);
	}
        return(184 + Font_Size);
}

/*******************************************************************************
    				data <- setdata
*******************************************************************************/

AttributesSetL (data, setdata, cnt)
char    *data;
long    setdata;
int     cnt;
{
	long *ptr;

	ptr = (long *)&data[cnt];
	*ptr = setdata;
}

/*******************************************************************************
		    buffer <- (ank_font+kanji_font+gaiji_font)
*******************************************************************************/

int
Font_Name_Copy(fontinfo, font_num, buffer)
XJp_fontInfo 	*fontinfo;
unsigned int	font_num;
char		*buffer;
{
        int start = 0;
        int i,j;
        int font_length;
	extern int _count;
	extern char *_ascii[];

	if (font_num == 0)
		return(0);
	_ascii[_count] = XJP_ascii;
	for (i = 0;i < font_num;i++, fontinfo++) {
		if (!strcmp(fontinfo->id, XJP_romanX0201))
		    _ascii[_count] = XJP_romanX0201;
		for(j = 0;fontinfo->id[j] != NULL;j++)
			buffer[j+start] = fontinfo->id[j];
		buffer[j+start] = _TAB;
		j++;
		font_length = strlen(fontinfo->fontname);
		SETDATA(fontinfo->fontname, buffer + j + start, font_length);
		j += font_length;
		buffer[j+start] = _CR;
		j++;
		start += j;
	}
        *(buffer+start-1) = NULL;
#ifdef DEBUG
        fprintf(stderr,"FONTNAME is %s\n",buffer);
        fprintf(stderr,"FONTNAME length %d\n",start);
#endif
        return(start);
}


time_out()
{
	extern int _time_flag;
	_time_flag = 1;
}
