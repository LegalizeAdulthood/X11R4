/*
 *	Copyright 1989 OMRON TATEISI ELECTRONICS CO.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of OMRON not be used in 
 * advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission.  OMRON makes no representations 
 * about the suitability of this software for any purpose.  It is provided 
 * "as is" without express or implied warranty.
 *
 * OMRON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * OMRON BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE. 
 *
 *	Author:	Seiji Kuwari	OMRON TATEISI ELECTRONICS CO.
 *				kuwa@frf.omron.junet
 *				uunet!nff.ncl.omron.co.jp!kuwa
 */				

/*	Version 4.0
 */

#include <stdio.h>
#include "xwnmo.h"
#include "buffer.h"
#include "sdefine.h"

struct	wnmo_client	*clients[MAX_CLIENTS];

/*
	env. control routines
 */

init_clients()
{
    register int i;
    for(i = 0; i < MAX_CLIENTS; i++){
	clients[i] = NULL;
    }
    c_c = clients[0] = (struct wnmo_client *)malloc(sizeof(struct wnmo_client));
    clients[0]->c_id = 0;
    c_c->s_ascii_char = 0;
    c_c->e_delete = 1;
}

init_clients1()
{

    c_b = (struct buf *)malloc(sizeof(struct buf));
    cur_rk = c_c->rk;
    default_flag_set();
    set_wnmo_client();
    clear_c_b();
}

#ifndef	X11
is_new_clients(c_id)
C_ID c_id;
{
    int j;

    if (is_text_object() == 0) {
	c_c = clients[0];
	c_c->c_id = c_id;
    } else if ((j = find_client(c_id)) > 0) {
	c_c = clients[j];
    } else {
	new_client(c_id);
    }

}
#else	X11
is_new_client(c_id)
C_ID c_id;
{
    int j;

    if ((j = find_client(c_id)) > 0) {
	c_c = clients[j];
    } else {
	new_client(c_id);
    }

}
#endif	!X11
   
find_client(c_id)
C_ID	c_id;
{
    register int j;

    for(j = 0; j < MAX_CLIENTS; j++){
	if(clients[j]->c_id == c_id)
	    return j;
    }
    return -1;
}

new_client(c_id, x, js)
C_ID	c_id;
XJClient *x;
int	js;
{
    register int j;
#ifdef	X11
    extern char jserver_name_in_uumrc[];
    int ret;
#endif	X11

    for(j = 1; j < MAX_CLIENTS; j++){
    /*
	if(clients[j] == NULL) {
	*/
	if(client_mem[j] == NULL) {
	    client_mem[j] = x;
	    if (x->dispmode != 2) {
		x->w_c = c_c =
		(struct wnmo_client *)malloc(sizeof(struct wnmo_client));
		c_b = (struct buf *)malloc(sizeof(struct buf));
		strcpy(c_c->user_name, x->a_data);

		if ((ret = find_same_user(c_c->user_name)) == -1) {
    			c_c->s_ascii_char = 0;
    			c_c->e_delete = 1;
			uumrc_get_entries(0);
			if (init_key_table() == -1) {
			    return(-1);
			}
		} else {
			copy_env(ret);
		}
		default_flag_set();
		clients[j] = c_c;

		allocate_areas();
		if (js) {
		    connect_jserver(0);
		}
		open_romkan();
		c_c->m_length = c_c->m_chg * 2;
		set_wnmo_client();
		if (create_jw(x, 0, MHL0) < 0)
			return -1;
		clear_c_b();
#ifndef	X11
		tty_setup1(get_char_size());
#endif	!X11
	    } else {
		x->w_c = clients[j] = 0;
		if (create_jw(x, 0, MHL0) < 0)
			return -1;
	    }
	    return (j);
	}
    }
    return -1;	/* clients table over flow */
}
	
find_same_user(user_n)
char *user_n;
{
    int j;

    for(j = 0; j < MAX_CLIENTS; j++){
	if(clients[j] != NULL) {
	    if(strcmp(clients[j]->user_name, user_n) == 0) {
		return(j);
	    }
	}
    }
    return(-1);
}

copy_env(j)
int j;
{
    strcpy(c_c->envrc_name, clients[j]->envrc_name);
    strcpy(c_c->kankana_env_name, clients[j]->kankana_env_name);
    strcpy(c_c->kanarom_env_name, clients[j]->kanarom_env_name);
    strcpy(c_c->rkfile_name, clients[j]->rkfile_name);
    strcpy(c_c->uumkey_name, clients[j]->uumkey_name);

    c_c->m_chg = clients[j]->m_chg;
    c_c->m_history = clients[j]->m_history;
    c_c->m_bunsetsu = clients[j]->m_bunsetsu;
    c_c->m_ichiran_kosu = clients[j]->m_ichiran_kosu;
    c_c->s_ascii_char = clients[j]->s_ascii_char;
    c_c->e_delete = clients[j]->e_delete;
    c_c->h_flag = clients[j]->h_flag;
    c_c->h_on_kuten = clients[j]->h_on_kuten;
    c_c->h_off_def = clients[j]->h_off_def;

    c_c->m_table = clients[j]->m_table;
}

del_client(cl)
struct wnmo_client	*cl;
{
    register int j;
    struct wnmo_client *c_c_sv;

    for(j = 1; j < MAX_CLIENTS; j++) {
	if(clients[j] == cl) {
	    c_c_sv = c_c;
	    c_c = cl;
	    free_areas();
	    if (jl_isconnect(bun_data_)) {
		jl_dic_save_all(bun_data_);
		jl_close(bun_data_);
	    }
	    rk_close(c_c->rk);
	    c_c = c_c_sv;
	    free(cl);
	    clients[j] = NULL;
	    return;
	}
    }
}

set_wnmo_client()
{
	cursor_colum = 0;
	cursor_reverse = 0;
	cursor_underline = 0;
	cursor_invisible = 1;
	cursor_bold = 0;	/* add by KUWA */

	quote_flag = 0;
	c_c->buffer_in = 0;
	c_c->n_redraw = 0;
	c_c->command_func = NULL;
	c_c->buffer_in_nest = 0;
	c_c->buffer_ignored[0] = 0;
	c_c->buffer_ignored[1] = 0;
	c_c->buffer_ignored[2] = 0;
	c_c->buffer_ignored[3] = 0;

}

default_flag_set()
{
	henkan_off_flag = henkan_off_def;
	henkan_on_kuten = 0;	/* 句点入力で変換しない */

	env_kanakan = NULL;
	env_kankana = NULL;
	env_kanarom = NULL;

	kanakan_sticky = 0;
	kankana_sticky = 0;
	kanarom_sticky = 0;
}

epilogue()
{
    int i;

    for (i = 0; i < MAX_CLIENTS; i++) {
	if ((c_c = clients[i]) != 0) {
    	    disconnect_jserver();
	}
    }
}
