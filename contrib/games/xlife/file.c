/*
 * XLife Copyright 1989 Jon Bennett jb7m+@andrew.cmu.edu, jcrb@cs.cmu.edu
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The copyright holders make no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "defs.h"
#include "data.h"
#include "struct.h"
#include <stdio.h>
#include <pwd.h>

char *gethome()
{
    return(getpwuid(getuid())->pw_dir);
}


char *checktilda(stng)
char *stng;
{
    static char full[1024];
    struct passwd *pw;
    int i;
    if (stng[0]=='~') {
	i=1;
	while(stng[i]!='/' && stng[i]!='\0')
	    full[i-1]=stng[i], i++;
	if (i==1) 
	    strcpy(full,gethome());
	else {
	    full[i-1]='\0';
	    if((pw = getpwnam(full)) == NULL){
		XClearWindow(disp,inputw);
		XDrawString(disp,inputw,ntextgc,ICOORDS(0,0),SYSERR,strlen(SYSERR));
	    }
	    else{
		strcpy(full,pw->pw_dir);
	    }
	}
	strcat(full,stng+i);
    } else
	strcpy(full,stng);
    return(full);
}

char *addlifeext(buf)
 char *buf;
{
    int len=strlen(buf);
    if(strcmp(buf+len-5,".life")){
	return(".life");
    }
    return("");
}

void savefile(){

    FILE *savefl;
    char outbuf[100];
    struct cell *ptr;
    
    if(state==STOP) {
	strcpy(outbuf,checktilda(inpbuf+8));
	strcat(outbuf,addlifeext(outbuf));
	if((savefl=fopen(outbuf,"w")) != NULL){
	    ptr=head;
	    while(ptr){
		savebox(ptr->live1,ptr->live2,ptr->x,ptr->y,savefl);
		ptr=ptr->next;
	    }
	    fclose(savefl);
	}
	else{
	    XClearWindow(disp,inputw);
	    XDrawString(disp,inputw,ntextgc,ICOORDS(0,0),SYSERR,strlen(SYSERR));
	}
    }
    else{
	strcpy(inpbuf,"Save to:");
	minbuflen=8;
	strcat(inpbuf,savedirbuf);
	XClearWindow(disp,inputw);
	XDrawString(disp, inputw, ntextgc,ICOORDS(0,0),inpbuf, strlen(inpbuf));
    }
}

int do_loadfile(filename)
    char *filename;
{
    FILE *loadfl;
    int x,y;

    if((loadfl=fopen(filename,"r")) != NULL){
	    while(fscanf(loadfl,"%d %d\n",&x,&y) != EOF)
	    {
		addcell(x + xpos,y + ypos);
	    }	    
	    fclose(loadfl);
	    return 1;
    } else
	return 0;
}

void loadfile(){

    char outbuf[100];
    
    if(!state){
	strcpy(outbuf,checktilda(inpbuf+10));
	strcat(outbuf,addlifeext(outbuf));
	if (do_loadfile(outbuf)) {
	    scale = 1;
	    XClearWindow(disp,lifew);
	    redrawscreen();
	}
	else{
	    XClearWindow(disp,inputw);
	    XDrawString(disp,inputw,ntextgc,ICOORDS(0,0),SYSERR,strlen(SYSERR));
	}
    }
    else{
	strcpy(inpbuf,"Load from:");
	minbuflen=10;
	strcat(inpbuf,loadirbuf);
	XClearWindow(disp,inputw);
	XDrawString(disp, inputw, ntextgc,ICOORDS(0,0),inpbuf, strlen(inpbuf));
    }
}
    
