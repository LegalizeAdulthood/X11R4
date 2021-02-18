.\"
.\" 		    Copyright 1989 TOSHIBA CORPORATION
.\"
.\" Permission to use, copy, modify, and distribute this software and its
.\" documentation for any purpose and without fee is hereby granted, provided
.\" that the above copyright notice appear in all copies and that both that
.\" copyright notice and this permission notice appear in supporting
.\" documentation, and that the name of TOSHIBA Corporation not be used in 
.\" advertising or publicity pertaining to distribution of the software 
.\" without specific, written prior permission. TOSHIBA Corporation makes no 
.\" representations about the suitability of this software for any purpose.  
.\" It is provided "as is" without express or implied warranty.
.\"
.\" TOSHIBA DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING 
.\" ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL 
.\" TOSHIBA BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR 
.\" ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
.\" WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
.\" ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
.\" SOFTWARE.
.\"
.\" Author:  
.\" 		Katsuhisa Yano
.\" 		Tomoko Koi
.\"		Personal Computer Design Dept.
.\"		TOSHIBA CORPORATION OME WORKS
.\"		5-20-7 SORD BLD. MASAGO 
.\"		CHIBA-CITY CHIBA 260 JAPAN
.\"
.H 1 ����
.PP
��������������ƴؿ��� Xlib ��ʸ����Υ��������䤤��碌������ʤɤ�Ԥ��ؿ��� EUC ʸ���󤬰�����褦�˳�ĥ������ΤǤ���
��ĥ���줿�ؿ��ǤϴĶ��ѿ� CSWIDTH �򻲾Ȥ��ޤ��Τǡ��ؿ���ƤӤ������� CSWIDTH �����������ꤹ��ɬ�פ�����ޤ���
�ޤ����ץ������Ǥϼ��Υե�����򥤥󥯥롼�ɤ��Ʋ�������
.DS
#include <X11/XJEuc.h>
.DE
.P
�ؿ��ΰ����ʤɤΥץ���ࡦ���󥿥ե������ϤǤ���¤� Xlib �˹�碌�Ƥ��ޤ��Τǡ��äˤ��Ȥ��񤭤Τʤ���ΤˤĤ��Ƥ� Xlib �δؿ��򻲾Ȥ��Ʋ�������
.P
�ץ���ࡦ���󥿥ե������� Xlib ���礭���㤦���� XFontStruct ��¤�Τμ�갷���Ǥ���
��ĥ���줿�ؿ��Ǥ� XFontStruct ��¤�Τ�����˼��ι�¤�Τ���Ѥ��Ʋ�������
.DS
typedef struct {
	XFontStruct	*font_std[4];	/* standard font structure table */
	XFontStruct	*font_ext[4];	/* extension font structure table */
	int		ascent;		/* maximum ascent */
	int		descent;	/* maximum descent */
} XJeFontStruct;
.DE
.P
XJeFontStruct ��¤�Τˤϳƥ����ɥ��åȤ��Ȥˣ������ɸ�ࡢ��ĥ�ˤΥե���Ȥ���ꤹ�뤳�Ȥ��Ǥ��ޤ���
EUC ʸ������������ƴؿ��ϥǥե���Ȥ�ɸ��ե���Ȥ���Ѥ��ޤ�����ɸ��ե���Ȥ��Ѱդ���Ƥ����ϰϳ��Υ����ɤ�ʸ���ˤĤ��Ƥϳ�ĥ�ѥե���Ȥ���Ѥ��ޤ���
ɸ��ե���ȤȤ��ƻȤ� XFontStruct ��¤�ΤؤΥݥ��󥿤�ƥ����ɥ��åȤ��Ȥ� font_std[] �����ꤷ�Ʋ�������
��ĥ�ե���Ȥ���Ѥ������ font_ext[] �����ꤷ�Ʋ�������
���Ѥ��ʤ������ɥ��åȤˤϣΣգ̣̥ݥ��󥿤����ꤷ�Ʋ�������
.H 1 �ե���ȤΥ���
.H 2 XJeLoadQueryFont
.DS
XJeFontStruct *XJeLoadQueryFont(display, name_table)
    Display *display;
    XJeFontNameStruct *name_table;
.DE
.VL 16 0 1
.LI display
�إ����ФؤΥ��ͥ���������ꤷ�ޤ���
.LI name_table
XJeFontNameStruct ��¤�ΤؤΥݥ��󥿤���ꤷ�ޤ���
.LE
.P
XJeLoadQueryFont �ؿ��� name_table �ǻ��ꤵ�줿̾���Υե���Ȥ���ɤ���XJeFontStruct ��¤�ΤؤΥݥ��󥿤��֤��ޤ���
XJeFontNameStruct ��¤�Τϼ��Τ褦�ˤʤäƤ��ޤ���
.DS
typedef struct {
	char	*fontname_std[4];	/* standard font name */
	char	*fontname_ext[4];	/* extension font name */
} XJeFontNameStruct;
.DE
.P
fontname_std[] �ˤ�ɸ��ե���ȤȤ��ƻ��Ѥ���ե����̾��ƥ����ɥ��åȤ��Ȥ����ꤷ��fontname_ext[] �ˤϳ�ĥ�ѥե���ȤΥե����̾��ƥ����ɥ��åȤ��Ȥ����ꤷ�Ʋ�������
���Ѥ��ʤ������ɥ��åȤˤϣΣգ̣̥ݥ��󥿤����ꤷ�Ʋ�������
�ؿ����֤� XJeFontStruct ��¤�Τ� font_std[] �� font_ext[] �ˤϡ��ƥ����ɥ��åȤǻ��Ѥ��� XFontStruct ��¤�ΤؤΥݥ��󥿤�����ޤ���
�ޤ� ascent �� descent �ˤϻ��ꤵ�줿�ե���Ȥ���Ǥ� ascent �� descent �κ����ͤ�����ޤ���
�ե���Ȥ����Ĥ���ɤǤ��ʤ����ϡ��Σգ̣̤��֤���ޤ���
.H 1 ʸ���󥵥������䤤��碌
.H 2 ʸ���󥵥����η׻�
.DS
int XJeTextWidth(font_struct, string, count)
    XJeFontStruct *font_struct;
    unsigned char *string;
    int count;
.DE
.VL 16 0 1
.LI font_struct
XJeFontStruct ��¤�Τ���ꤷ�ޤ���
.LI string
EUC ʸ�������ꤷ�ޤ���
.LI count
�׻�������Х��ȿ���ʸ�����ǤϤ���ޤ���ˤ���ꤷ�ޤ���
.LE
.P
XJeTextWidth �� EUC ʸ����������֤��ޤ����ͤϥԥ�����ñ�̤Ǥ���
font_struct �ˤ� XJeFontStruct ��¤�Τ���ꤷ�Ʋ�������
count �ˤ�ʸ�����ǤϤʤ��Х��ȿ�����ꤷ�Ʋ�������
ʸ������Υ����ɥ��åȣ��������б�����ե���Ȥ����ꤵ��Ƥ��ʤ����ϡ�
�����ɥ��åȣ��Υե���Ȥ����Ѥ���ޤ���
.H 2 �����������η׻�
.DS
XJeTextExtents(font_struct, string, nchars, direction_return, 
	       font_ascent_return, font_descent_return, overall_return)
    XJeFontStruct *font_struct;
    unsigned char *string;
    int nchars;
    int *direction_return;
    int *font_ascent_return, *font_descent_return;
    XCharStruct	*overall_return;
.DE
.VL 24 0 1
.LI font_struct 
XJeFontStruct ��¤�Τ���ꤷ�ޤ���
.LI string
EUC ʸ�������ꤷ�ޤ���
.LI nchars
�׻�����Х��ȿ���ʸ�����ǤϤ���ޤ���ˤ���ꤷ�ޤ���
.LI direction_return
�����������֤���ޤ���
.LI font_ascent_return
�ե���Ȥ� ascent �κ����ͤ��֤���ޤ���
.LI font_descent_return
�ե���Ȥ� descent �κ����ͤ��֤���ޤ���
.LI overall_return
XCharStruct ��¤�Τ��֤���ޤ���
.LE
.P
XJeTextExtents �� EUC ʸ������Ф�������Ū�ʥ������������˷׻����ޤ���
font_struct �ˤ� XJeFontStruct ��¤�Τ���ꤷ�Ʋ�������
count �ˤ�ʸ�����ǤϤʤ��Х��ȿ�����ꤷ�Ʋ�������
direction_return �ˤϺǸ��ʸ���������������֤���ޤ���
font_ascent_return �ˤϻ��Ѥ����ե���Ȥ� ascent �κ����ͤ��֤���ޤ���
font_descent_return �ˤϻ��Ѥ����ե���Ȥ� descent �κ����ͤ��֤���ޤ���
overall_return �ˤĤ��Ƥ� XTextExtents �򻲾Ȥ��Ʋ�������
ʸ������Υ����ɥ��åȣ��������б�����ե���Ȥ����ꤵ��Ƥ��ʤ����ϡ�
�����ɥ��åȣ��Υե���Ȥ����Ѥ���ޤ���
.H 1 ʸ���������
.H 2 �ƥ�����ʸ���������
.DS
XJeDrawString(display, d, gc, font_struct, x, y, string, length)
Display *display;
Drawable d;
GC gc;
XJeFontStruct *font_struct;
int x, y;
char *string;
int length;
.DE
.VL 16 0 1
.LI display
�إ����ФؤΥ��ͥ���������ꤷ�ޤ���
.LI d
�����о�ʪ����ꤷ�ޤ���
.LI gc
GC ����ꤷ�ޤ���
.LI font_struct
XJeFontStruct ��¤�Τ���ꤷ�ޤ���
.LI x

.LI y
������֤���ꤷ�ޤ���
.LI string
EUC ʸ�������ꤷ�ޤ���
.LI length
���褹��Х��ȿ���ʸ�����ǤϤ���ޤ���ˤ���ꤷ�ޤ���
.LE
.P
XJeDrawString �� EUC ʸ��������褷�ޤ���
font_struct �ˤ� XJeFontStruct ��¤�Τ���ꤷ�Ʋ�������
count �ˤ�ʸ�����ǤϤʤ��Х��ȿ�����ꤷ�Ʋ�������
ʸ������Υ����ɥ��åȣ��������б�����ե���Ȥ����ꤵ��Ƥ��ʤ����ϡ�
�����ɥ��åȣ��Υե���Ȥ����Ѥ���ޤ���
������˥ե���Ȥ��ڤ��ؤ����ϡ�GC ��Υե���Ȥ��Ѥ��ޤ���
���äơ��ؿ������������Ȥ��� GC ��Υե���Ȥϡ��Ǹ�˻��Ѥ����ե���ȤȤʤ�ޤ���
���顼��ȯ�����Ƥ�����ޤ����褵��Ƥ��뤳�Ȥ�����ޤ���
���顼���ͤˤĤ��Ƥ� XDrawString �򻲾Ȥ��Ʋ�������
.H 2 ���᡼�����ƥ�����ʸ���������
.DS
XJeDrawImageString(display, d, gc, font_struct, x, y, string, length)
Display *display;
Drawable d;
GC gc;
XJeFontStruct *font_struct;
int x, y;
char *string;
int length;
.DE
.VL 16 0 1
.LI display
�إ����ФؤΥ��ͥ���������ꤷ�ޤ���
.LI d
�����о�ʪ����ꤷ�ޤ���
.LI gc
GC ����ꤷ�ޤ���
.LI font_struct
XJeFontStruct ��¤�Τ���ꤷ�ޤ���
.LI x

.LI y
������֤���ꤷ�ޤ���
.LI string
EUC ʸ�������ꤷ�ޤ���
.LI length
���褹��Х��ȿ���ʸ�����ǤϤ���ޤ���ˤ���ꤷ�ޤ���
.LE
.P
XJeDrawImageString �� EUC ʸ��������褷�ޤ���
font_struct �ˤ� XJeFontStruct ��¤�Τ���ꤷ�Ʋ�������
count �ˤ�ʸ�����ǤϤʤ��Х��ȿ�����ꤷ�Ʋ�������
�Хå������ɤζ���ι⤵�ϡ����Ѥ����ե���Ȥκ���� ascent �� descent �򤿤�����ΤȤʤ�ޤ���
ʸ������Υ����ɥ��åȣ��������б�����ե���Ȥ����ꤵ��Ƥ��ʤ����ϡ�
�����ɥ��åȣ��Υե���Ȥ����Ѥ���ޤ���
������˥ե���Ȥ��ڤ��ؤ����ϡ�GC ��Υե���Ȥ��Ѥ��ޤ���
���äơ��ؿ������������Ȥ��� GC ��Υե���Ȥϡ��Ǹ�˻��Ѥ����ե���ȤȤʤ�ޤ���
���顼��ȯ�����Ƥ�����ޤ����褵��Ƥ��뤳�Ȥ�����ޤ���
���顼���ͤˤĤ��Ƥ� XDrawImageString �򻲾Ȥ��Ʋ�������
