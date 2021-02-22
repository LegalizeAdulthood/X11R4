/*
 *              Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 * LINTLIBRARY
 */
#include <xgks.h>

#include <stdio.h>

/*ARGSUSED*/
GKSERR open_gks (err_file,memory)
   struct _iobuf *err_file;
   int memory;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR close_gks ()
   {
return 0;
   }
/*ARGSUSED*/
GKSERR emerg_close ()
   {
return 0;
   }
/*ARGSUSED*/
GKSERR s_aspect_flags (flags)
   ENUM *flags;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR open_ws (ws_id, connection, ws_type)
   WSID ws_id;
   char *connection;
   WSTYPE ws_type;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR s_deferral (ws_id, deferral_mode, regen_mode)
   WSID ws_id;
   ENUM deferral_mode;
   ENUM regen_mode;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR message (ws_id, string)
   WSID ws_id;
   char *string;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR s_window (trans,xmin,xmax,ymin,ymax)
   int trans;
   REAL xmin,xmax,ymin,ymax;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR s_viewport (trans,xmin,xmax,ymin,ymax)
   int trans;
   REAL xmin,xmax,ymin,ymax;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR s_viewport_priority (trans,ref_trans,priority)
   int trans,ref_trans;
   ENUM priority;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR s_ws_window (ws_id,xmin,xmax,ymin,ymax)
   WSID ws_id;
   REAL xmin,xmax,ymin,ymax;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR s_ws_viewport (ws_id,xmin,xmax,ymin,ymax)
   WSID ws_id;
   REAL xmin,xmax,ymin,ymax;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR s_color_rep (ws_id,idx,red,green,blue)
   WSID ws_id;
   IDX idx;
   REAL red,green,blue;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR fill_area (num_pts,pts)
   int num_pts;
   WCPT *pts;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR s_fill_rep (ws_id,idx,style,style_idx,color)
   WSID ws_id;
   IDX idx, style_idx,color;
   ENUM style;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR polyline (num_pts,pts)
   int num_pts;
   WCPT *pts;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR s_pline_rep (ws_id,idx,type,width,color)
   WSID ws_id;
   IDX idx, type, color;
   REAL width;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR polymarker (num_pts,pts)
   int num_pts;
   WCPT *pts;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR s_seg_trans (name,matrix)
   SEGMENT name;
   REAL *matrix;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR eval_trans_matrix (Fixed_pt,Trans_vector,Theta,ScaleX,ScaleY,CoorSystem,Matrix)
   WCPT *Fixed_pt,*Trans_vector;
   REAL Theta,ScaleX,ScaleY;
   int CoorSystem;
   REAL Matrix[6];
   {
return 0;
   }
/*ARGSUSED*/
GKSERR accum_trans_matrix (matin, fixed_point, trans_vec, theta, sx, sy, swtch, matout)
   REAL matin[3][2];
   WCPT *fixed_point, *trans_vec;
   REAL theta, sx, sy;
   int swtch;
   REAL matout[3][2];
   {
return 0;
   }
/*ARGSUSED*/
GKSERR s_char_up_vector (up_vector)
   WCPT *up_vector;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR s_text_rep (ws_id,idx,bundle)
   WSID ws_id;
   IDX idx;
   TEXT_BUNDLE *bundle;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR text (at,string)
   WCPT *at;
   char *string;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR init_locator (ws_id,dev,pos,ntrans,prompt,xmin,xmax,ymin,ymax,data)
   WSID ws_id;
   int dev;
   WCPT *pos;
   IDX ntrans;
   int prompt;
   REAL xmin,xmax,ymin,ymax;
   struct loc_data *data;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR s_locator_mode (ws_id,dev,mode,echo)
   WSID ws_id;
   int dev;
   ENUM mode,echo;
   {
return 0;
   }
/*ARGSUSED*/
GKSERR req_locator (ws_id,dev,status,ntrans,pos)
   WSID ws_id;
   int dev;
   ENUM *status;
   IDX *ntrans;
   WCPT *pos;
   {
return 0;
   }
