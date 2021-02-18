#include <stdio.h>
#include "commonhd.h"
#include "gmw.h"
#include "libwin.h" 
#include "jslib.h"
#include "jdata.h" 

struct JT dh;
struct wnn_file_head fh;
o_Sheet sheet;


main(argc, argv)
     int argc;
     char **argv;
{
    gmw_connect_UI(&argc, argv,
		   "jisho_editor_panel"l, "jisho_editor_panel.g"l);
    gmw_process_message_loop();
}

FILE *fp = NULL;
w_char *comment;

jishoeditor_open(fname, flen)
Char *fname;
int flen;
{
    char fn[256];
    char buf[256];
    Char Buf[256];

    sStrncpy(fn, fname, flen);
    fn[flen] = 0;

    if(fp) fclose(fp);
    if((fp = fopen(fn, "rw")) == NULL){
	msg("ファイルがオープン出来ません"l);
	perror("fopen:");
	return;
    }

    if(input_header(fp, &dh, &fh) == -1){
	msg("Ｗｎｎの辞書ではありません"l);
	return;
    }
    comment = (w_char *)malloc((dh.maxcomment + 1) * sizeof(w_char));
    get_n_EU_str(fp, dh.maxcomment, comment);

/*
    sprintf(buf, "種類:%s\n語数:%d\nサイズ:%dバイト",
    
	    dh.gosuu, 
    Sstrcpy(Buf, buf);
    panel_set_string(gmw_UI, "message0"l, Buf, Strlen(Buf));
*/
    Sstrcpy(Buf, wnn_dic_types[dh.syurui]);
    panel_set_string(gmw_UI, "syurui"l, Buf, Strlen(Buf));
    
    sprintf(buf, "%d", dh.gosuu); Sstrcpy(Buf, buf);
    panel_set_string(gmw_UI, "gosuu"l, Buf, Strlen(Buf));

    sprintf(buf, "%dバイト", (fseek(fp, 0, 2),ftell(fp))); Sstrcpy(Buf, buf);
    panel_set_string(gmw_UI, "size"l, Buf, Strlen(Buf));

    sprintf(buf, "%s", fh.file_passwd);Sstrcpy(Buf, buf);
    panel_set_string(gmw_UI, "passwd"l, Buf, Strlen(Buf));

    sprintf(buf, "%s", dh.hpasswd);Sstrcpy(Buf, buf);
    panel_set_string(gmw_UI, "hpasswd"l, Buf, Strlen(Buf));

    panel_set_string(gmw_UI, "comment"l, comment, Strlen(comment));

    sprintf(buf, "%s", fh.file_uniq.createhost);Sstrcpy(Buf, buf);
    panel_set_string(gmw_UI, "uniq_create_host"l, Buf, Strlen(Buf));

    sprintf(buf, "%d", fh.file_uniq.inode);Sstrcpy(Buf, buf);
    panel_set_string(gmw_UI, "uniq_inode"l, Buf, Strlen(Buf));

    sprintf(buf, "%d", fh.file_uniq.dev);Sstrcpy(Buf, buf);
    panel_set_string(gmw_UI, "uniq_devno"l, Buf, Strlen(Buf));

    sprintf(buf, "%d", fh.file_uniq.time);Sstrcpy(Buf, buf);
    panel_set_string(gmw_UI, "uniq_time"l, Buf, Strlen(Buf));

    sprintf(buf, "%s", fh.file_uniq_org.createhost);Sstrcpy(Buf, buf);
    panel_set_string(gmw_UI, "uniq_orig_create_host"l, Buf, Strlen(Buf));

    sprintf(buf, "%d", fh.file_uniq_org.inode);Sstrcpy(Buf, buf);
    panel_set_string(gmw_UI, "uniq_orig_inode"l, Buf, Strlen(Buf));

    sprintf(buf, "%d", fh.file_uniq_org.dev);Sstrcpy(Buf, buf);
    panel_set_string(gmw_UI, "uniq_orig_devno"l, Buf, Strlen(Buf));

    sprintf(buf, "%d", fh.file_uniq_org.time);Sstrcpy(Buf, buf);
    panel_set_string(gmw_UI, "uniq_orig_time"l, Buf, Strlen(Buf));
    
    gmw_flush();
    
}
  
jishoeditor_save()
{
    Char Buf[256];
    int len;
    
    if(!fp){
	msg("ファイルがオープンされていません"l);
	return;
    }

    len = panel_get_string(gmw_UI, "passwd"l, Buf);
    Buf[len] = 0; sStrcpy(fh.file_passwd, Buf);

    len = panel_get_string(gmw_UI, "hpasswd"l, Buf);
    Buf[len] = 0; sStrcpy(dh.hpasswd, Buf);

    len = panel_get_string(gmw_UI, "comment"l, Buf);
    Buf[len] = 0; Strcpy(comment, Buf);
    
    fseek(fp, 0, 0);
    output_header(fp, &dh, &fh);
    put_n_EU_str(fp, comment, dh.maxcomment);
    msg("元の辞書に書き戻しました。"l);
}

jishoeditor_exit()
{
    exit(0);
}


msg(s)
Char *s;
{
    panel_set_string(gmw_UI, "error_msg"l, s, Strlen(s));
    gmw_flush();
}

jishoeditor_passwd_change(passwd, len)
Char *passwd;
int len;
{
    char buf[256];
    char Buf[256];

    sStrncpy(buf, passwd, len);
    buf[len] = 0;
    new_pwd(buf, buf);
    Sstrcpy(Buf, buf);
    panel_set_string(gmw_UI, "passwd"l, Buf, Strlen(Buf));
    gmw_flush();
}

jishoeditor_hpasswd_change(passwd, len)
Char *passwd;
int len;
{
    char buf[256];
    char Buf[256];

    sStrncpy(buf, passwd, len);
    buf[len] = 0;
    new_pwd(buf, buf);
    Sstrcpy(Buf, buf);
    panel_set_string(gmw_UI, "hpasswd"l, Buf, Strlen(Buf));
    gmw_flush();
}

jishoeditor_toroku_funou()
{
    panel_set_string(gmw_UI, "passwd"l, "*"l, 1);
    gmw_flush();
}

jishoeditor_hindokousin_funou()
{
    panel_set_string(gmw_UI, "hpasswd"l, "*"l, 1);
    gmw_flush();
}

jishoeditor_clear_passwd()
{
    panel_set_string(gmw_UI, "passwd"l, ""l, 0);
    gmw_flush();
}

jishoeditor_clear_hpasswd()
{
    panel_set_string(gmw_UI, "hpasswd"l, ""l, 0);
    gmw_flush();
}
