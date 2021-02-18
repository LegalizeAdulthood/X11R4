#include "gmw.h"
#include "libwin.h"
#include "comm.h"
int OOP_NAME_open;
int OOP_NAME_save;
int OOP_NAME_exit;
int OOP_NAME_passwd_change;
int OOP_NAME_hpasswd_change;
int OOP_NAME_clear_passwd;
int OOP_NAME_clear_hpasswd;
int OOP_NAME_toroku_funou;
int OOP_NAME_hindokousin_funou;

gmw_init_process_message()
{
  OOP_NAME_open = gmw_name_create("open"l);
  OOP_NAME_save = gmw_name_create("save"l);
  OOP_NAME_exit = gmw_name_create("exit"l);
  OOP_NAME_passwd_change = gmw_name_create("passwd_change"l);
  OOP_NAME_hpasswd_change = gmw_name_create("hpasswd_change"l);
  OOP_NAME_clear_passwd = gmw_name_create("clear_passwd"l);
  OOP_NAME_clear_hpasswd = gmw_name_create("clear_hpasswd"l);
  OOP_NAME_toroku_funou = gmw_name_create("toroku_funou"l);
  OOP_NAME_hindokousin_funou = gmw_name_create("hindokousin_funou"l);
}

gmw_process_message_internal(sender, mbuf, mlen)
o_Any sender;  int *mbuf, mlen;
{
  int *rbuf;
  int r;
  if (mbuf[0]==OOP_NAME_open) {
    if (mlen<=1)
      gmw_message_size_error("jishoeditor_open");
    gmw_conv_v_char(&mbuf[1+1],mbuf[1]);
    if (mlen!=1+1+(mbuf[1]+1)/2)
      gmw_message_size_error("jishoeditor_open");
    {
      r = jishoeditor_open(&mbuf[1+1],mbuf[1]);
      if (gmw_is_rpc()) {
        gmw_put_packet_header(MSG_RET,sender,0);
        gmw_check_outbuf(0);
        gmw_flush();
    }
      gmw_end_process_message();
      return;
    }
  }
  if (mbuf[0]==OOP_NAME_save) {
    if (mlen!=1)
      gmw_message_size_error("jishoeditor_save");
    {
      r = jishoeditor_save();
      if (gmw_is_rpc()) {
        gmw_put_packet_header(MSG_RET,sender,0);
        gmw_check_outbuf(0);
        gmw_flush();
    }
      gmw_end_process_message();
      return;
    }
  }
  if (mbuf[0]==OOP_NAME_exit) {
    if (mlen!=1)
      gmw_message_size_error("jishoeditor_exit");
    {
      r = jishoeditor_exit();
      if (gmw_is_rpc()) {
        gmw_put_packet_header(MSG_RET,sender,0);
        gmw_check_outbuf(0);
        gmw_flush();
    }
      gmw_end_process_message();
      return;
    }
  }
  if (mbuf[0]==OOP_NAME_passwd_change) {
    if (mlen<=1)
      gmw_message_size_error("jishoeditor_passwd_change");
    gmw_conv_v_char(&mbuf[1+1],mbuf[1]);
    if (mlen!=1+1+(mbuf[1]+1)/2)
      gmw_message_size_error("jishoeditor_passwd_change");
    {
      r = jishoeditor_passwd_change(&mbuf[1+1],mbuf[1]);
      if (gmw_is_rpc()) {
        gmw_put_packet_header(MSG_RET,sender,0);
        gmw_check_outbuf(0);
        gmw_flush();
    }
      gmw_end_process_message();
      return;
    }
  }
  if (mbuf[0]==OOP_NAME_hpasswd_change) {
    if (mlen<=1)
      gmw_message_size_error("jishoeditor_hpasswd_change");
    gmw_conv_v_char(&mbuf[1+1],mbuf[1]);
    if (mlen!=1+1+(mbuf[1]+1)/2)
      gmw_message_size_error("jishoeditor_hpasswd_change");
    {
      r = jishoeditor_hpasswd_change(&mbuf[1+1],mbuf[1]);
      if (gmw_is_rpc()) {
        gmw_put_packet_header(MSG_RET,sender,0);
        gmw_check_outbuf(0);
        gmw_flush();
    }
      gmw_end_process_message();
      return;
    }
  }
  if (mbuf[0]==OOP_NAME_clear_passwd) {
    if (mlen!=1)
      gmw_message_size_error("jishoeditor_clear_passwd");
    {
      r = jishoeditor_clear_passwd();
      if (gmw_is_rpc()) {
        gmw_put_packet_header(MSG_RET,sender,0);
        gmw_check_outbuf(0);
        gmw_flush();
    }
      gmw_end_process_message();
      return;
    }
  }
  if (mbuf[0]==OOP_NAME_clear_hpasswd) {
    if (mlen!=1)
      gmw_message_size_error("jishoeditor_clear_hpasswd");
    {
      r = jishoeditor_clear_hpasswd();
      if (gmw_is_rpc()) {
        gmw_put_packet_header(MSG_RET,sender,0);
        gmw_check_outbuf(0);
        gmw_flush();
    }
      gmw_end_process_message();
      return;
    }
  }
  if (mbuf[0]==OOP_NAME_toroku_funou) {
    if (mlen!=1)
      gmw_message_size_error("jishoeditor_toroku_funou");
    {
      r = jishoeditor_toroku_funou();
      if (gmw_is_rpc()) {
        gmw_put_packet_header(MSG_RET,sender,0);
        gmw_check_outbuf(0);
        gmw_flush();
    }
      gmw_end_process_message();
      return;
    }
  }
  if (mbuf[0]==OOP_NAME_hindokousin_funou) {
    if (mlen!=1)
      gmw_message_size_error("jishoeditor_hindokousin_funou");
    {
      r = jishoeditor_hindokousin_funou();
      if (gmw_is_rpc()) {
        gmw_put_packet_header(MSG_RET,sender,0);
        gmw_check_outbuf(0);
        gmw_flush();
    }
      gmw_end_process_message();
      return;
    }
  }
  gmw_call_super(sender,mbuf,mlen);
}

