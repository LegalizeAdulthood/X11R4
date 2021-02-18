#include "gmw.h"
#include "libwin.h"
#include "comm.h"
panel_get_value(receiver,mb0)
Char *mb0;
{
  int m0 = gmw_name_create(mb0);
  int rlen;
  static selector;
  if (selector==0)
    selector = gmw_name_create("get_value"l);
  gmw_put_packet_header(MSG_RPC,receiver,2);
  gmw_check_outbuf(2);
  gmw_putw(selector);
  gmw_putw(m0);
  rlen = gmw_get_ret_header();
  if (rlen!=1)
    gmw_return_size_error();
  return gmw_getw();
}
panel_set_value(receiver,mb0,m1)
Char *mb0;
{
  int m0 = gmw_name_create(mb0);
  static selector;
  if (selector==0)
    selector = gmw_name_create("set_value"l);
  gmw_put_packet_header(MSG_SEND,receiver,3);
  gmw_check_outbuf(3);
  gmw_putw(selector);
  gmw_putw(m0);
  gmw_putw(m1);
}
panel_get_string(receiver,mb0,rb0,r0)
Char *mb0;
Char *rb0;
{
  int m0 = gmw_name_create(mb0);
  int r;
  int rlen;
  static selector;
  if (selector==0)
    selector = gmw_name_create("get_string"l);
  gmw_put_packet_header(MSG_RPC,receiver,2);
  gmw_check_outbuf(2);
  gmw_putw(selector);
  gmw_putw(m0);
  rlen = gmw_get_ret_header();
  if (rlen<=0)
    gmw_return_size_error();
  r = gmw_getw();
  if (rlen!=(r+1)/2+1)
    gmw_return_size_error();
  gmw_get_v_char(r,rb0,r0);
  return r;
}
panel_set_string(receiver,mb0,mb1,m1)
Char *mb0;
Char *mb1;
{
  int m0 = gmw_name_create(mb0);
  static selector;
  if (selector==0)
    selector = gmw_name_create("set_string"l);
  gmw_put_packet_header(MSG_SEND,receiver,3+(m1+1)/2);
  gmw_check_outbuf(3);
  gmw_putw(selector);
  gmw_putw(m0);
  gmw_put_v_char(mb1,m1);
}
panel_get_tool_by_name(receiver,mb0)
Char *mb0;
{
  int m0 = gmw_name_create(mb0);
  int rlen;
  static selector;
  if (selector==0)
    selector = gmw_name_create("get_tool_by_name"l);
  gmw_put_packet_header(MSG_RPC,receiver,2);
  gmw_check_outbuf(2);
  gmw_putw(selector);
  gmw_putw(m0);
  rlen = gmw_get_ret_header();
  if (rlen!=1)
    gmw_return_size_error();
  return gmw_getw();
}
