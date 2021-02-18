/*
	Copyright
		Kyoto University Research Institute for Mathematical Sciences
		1987, 1988, 1989
	Copyright OMRON TATEISI ELECTRONICS CO. 1987, 1988, 1989
	Copyright ASTEC, Inc. 1987, 1988, 1989


    Permission to use, copy, modify, and distribute this software
    and its documentation for any purpose and without any fee is
    hereby granted, subject to the following restrictions:

    The above copyright notice and this permission notice must appear
    in all versions of this software;

    The name of "Wnn" may not be changed; 

    All documentations of software based on "Wnn" must contain the wording
    "This software is based on the original version of Wnn developed by
    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
    OMRON TATEISI ELECTRONICS CO. and
    ASTEC, Inc.", followed by the above copyright notice;

    The name of KURIMS, OMRON and ASTEC may not be used
    for any purpose related to the marketing or advertising
    of any product based on this software.

    KURIMS, OMRON and ASTEC make no representations about
    the suitability of this software for any purpose.
    It is provided "as is" without express or implied warranty.

    KURIMS, OMRON and ASTEC DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
    SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
    IN NO EVENT SHALL KURIMS, OMRON AND ASTEC BE LIABLE FOR ANY SPECIAL,
    INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
    LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
    OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.

    Author: Hideki Tsuiki	Kyoto University
				tsuiki%kaba.or.jp@uunet.uu.net

	    Hiroshi Kuribayashi	OMRON TATEISI ELECTRONICS CO.
				kuri@frf.omron.co.jp
				uunet!nff.ncl.omron.co.jp!kuri

	    Naouki Nide		Kyoto University
				nide%kaba.or.jp@uunet.uu.net

	    Shozo Takeoka	ASTEC, Inc.
				take%astec.co.jp@uunet.uu.net

	    Takashi Suzuki	Advanced Software Technology & Mechatronics
				Research Institute of KYOTO
				suzuki%astem.or.jp@uunet.uu.net

*/
/*	Version 4.0
 */
struct	jdata {
  int	kanji1;
  short	kanji2;			/* */
  short   which ;		/* gyaku henkan? */
  int	serial;			/* index is a serial number of the first 
				 entry which is stored in this entry*/
  int	kosuu;			/* this means the number of elements in this
				 entry */
  int	jishono;		/* jishono in which these entries are stored */
  struct  jdata  *jptr;	/* pointer to another jdata which
				 points out a jdata of the same yomi
				 but (always)different jishono*/
  unsigned short   *hinsi;
  UCHAR *hindo;			/* ���� */
  UCHAR *hindo_in;		/* �������� */
};

struct JT {
  unsigned int total;    /* �ȡ��������� */
  int gosuu;
  char hpasswd[WNN_PASSWD_LEN];
  int syurui;

  int maxcomment;
  int maxhinsi_list;
  int maxserial;
  int maxtable;			/* For UD Dic */
  int maxhontai;		/* For UD Dic  and Static Dic */
  int maxkanji;
  int maxri1[2];		/* For Rev Dic */
  int maxri2;			/* For Rev Dic, is equal to maxserial */
  

  w_char *comment;
  w_char *hinsi_list;

  UCHAR *hindo;  /* ���� */
  unsigned short *hinsi;	/* bunpou data is stored here */
  UCHAR *kanji;	/* kanji data is stored here */
  struct uind1 *table; /* anothe tablefile for user jisho */
  UCHAR *hontai;	/*  the tablefile (index-file) */
  struct rind1 *ri1[2];  	/* For Rev Dic */
  struct rind2 *ri2;		/* For Rev Dic */

  short dirty;
  short hdirty;

/* ��������4�Ĥϡ� ��Ͽ��ǽ���񡢵��Ѵ���ǽ����ˤΤߴط����� */
    int bufsize_kanji;    /* �����ΰ���礭�� */
    int bufsize_serial;   /* ʸˡ�������ΰ���礭�� */
    int bufsize_hontai;     /* ����ǥå���(hontai)���ΰ���礭��*/
    int bufsize_table;     /*  ����ǥå���(table)���ΰ���礭��*/
    int bufsize_ri1[2];  /* ���Ѵ���ǽ����� table ���ΰ���礭��  */

  /* hinsi_list wo kakunou suru */
  int maxnode;
  struct wnn_hinsi_node *node;
};

struct HJT {
  struct wnn_file_uniq dic_file_uniq;
  int maxcomment;
  int maxserial;

  w_char *comment;
  UCHAR *hindo;

  short hdirty;
  int bufsize_serial;
};


/*
 * structures concerning UD dicts
 */

#define AL_INT(x) ((int)((char *)(x) + 3) & ~0x3)


#define ENDPTR 0  /* The end of (int) pointer list in uind2->next */
#define RD_ENDPTR 0xffffffff  /* The end of (int) pointer list in rind2->next */

struct uind1 {
  int pter1;			/* pointer to uind1 */
  int pter;			/* pinter to uind2 */
  unsigned int yomi1;
  unsigned int yomi2;
};

struct uind2 {
  int next;			/* pointer to uind2 */
  int serial;
  int kanjipter;
  w_char kosuu;
  w_char yomi[1];		/* actually it is variable length */
};

/* MAXTABLE MAX... ha user file no ookisani kuwaete touroku no tame 
   ni totteoku ookisa*/
#define MAXTABLE 100		/* you can touroku MAXTABLE element without 
				 realloc the jisho*/
#define MAXHONTAI MAXTABLE * 4 * 4
#define MAXKANJI MAXTABLE * 10
#define MAXSERIAL MAXTABLE

/* 
 * structures concerning REV dicts.
 */

struct rind1 {
  int pter1;			/* pointer to rind1 */
  int pter;			/* pinter to rind2 */
};

struct rind2 {
  int next[2];			/* pointer to rind2 */
  int kanjipter;
};

/*
 * Reverse Dict ni kansite, dotira wo hikuka?
 */

#define D_YOMI 0
#define D_KANJI 1

#define KANJI_str1(pter, which) (((which) == D_YOMI)?((w_char *)((pter) + 2)):\
    ((w_char *)((pter) + 2) + Strlen((w_char *)((pter) + 2)) + 1))

#define KANJI_str(p, which) ((*((p) + 1) & FORWARDED)? \
    KANJI_str1(((*(w_char *)((p) + 2)) << 16 | (*(w_char *)((p) + 4))) \
	       + (p),which): \
    KANJI_str1((p), which))

#define Get_kanji_len(pter, which) (Strlen(KANJI_str(pter, which)))

/* 
 * structures concerning SD dicts.
 */

#define ST_NORMAL 1
#define ST_NOENT 2
#define ST_NOPTER 3
#define ST_SMALL 4
#define ST_TABLE 5

/*
 *Concerning HINSI. But these may not be used.
 */

#define SAKUJO_HINSI 0xfffe /* sakujo sareta tango no hinsi */
#define NANDEMO_EEYO 0xfffd /* nandemo maeni koreru to iu hinsi */

/*
 *Concerning KANJI and COMMENT.
 */

#define HAS_YOMI 1
#define HAS_COMMENT 2
#define FORWARDED 4

/* #define SEPARATE_CHAR '\0'*/
/* Choose character which do not appear as the first byte of w_char */

#define DIC_YOMI_CHAR  1
#define DIC_COMMENT_CHAR  2


#define DIC_HIRAGANA "\\y"
#define DIC_KATAKANA "\\k"

/*#define	DIC_HANKAKU	"\\h"	/* Ⱦ�� *//* �ɤߤΤޤ� */
#define	DIC_ZENKAKU	"\\z"	/* ���� *//* ������ */
#define	DIC_NUM_KAN	"\\chan"	/* ������ *//* ���� */
#define	DIC_NUM_KANSUUJI "\\chas"	/* ������ *//* ɴ�󽽻� */
#define	DIC_NUM_KANOLD	"\\chao"	/* ������ *//* ��ɴ������ */
#define	DIC_NUM_HANCAN	"\\nhc"	/* Ⱦ�ѿ��� *//* 1,234 */
#define	DIC_NUM_ZENCAN	"\\nzc"	/* ���ѿ��� *//* ���������� */
#define DIC_HEX		"\\X"  /* \Xa4a2 */
#define DIC_HEXc	"\\x"  /* \xa4a2 */
#define DIC_OCT 	"\\0" /* \040   */
#define DIC_ESC 	"\\\\" /* \\ = \  */

/* �ѿ� */
/*#define	WNN_ALP_HAN	-4	/* Ⱦ�� *//* �ɤߤΤޤ� */
/*#define	WNN_ALP_ZEN	-30	/* ���� */
/* ���� */
/*#define	WNN_KIG_HAN	-5	/* Ⱦ�� *//* �ɤߤΤޤ� */
/*#define	WNN_KIG_JIS	-40	/* ����(JIS) */
/*#define	WNN_KIG_ASC	-41	/* ����(ASC) */
