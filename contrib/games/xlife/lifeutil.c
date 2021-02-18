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
#include "stdio.h"


unsigned long hash(x,y)
unsigned long x,y;
{
    unsigned long val;
    x >>= 3;
    y >>= 3;
    val = (x & 0x7f)<< 7;
    val += y & 0x7f;
    return(val);
}

void kill(ptr)
 struct cell *ptr;
{
    unsigned long hv=hash(ptr->x,ptr->y);

    if(ptr == head){
	    head = ptr->next;
    }
    else{
	ptr->fore->next=ptr->next;
    }
    if(ptr == cells[hv]){
	cells[hv] = ptr->hnext;
    }
    else{
	ptr->hfore->hnext=ptr->hnext;
    }
    if(ptr->next) ptr->next->fore=ptr->fore;
    if(ptr->hnext) ptr->hnext->hfore=ptr->hfore;
    if(ptr->rt) ptr->rt->lf=NULL;
    if(ptr->lf) ptr->lf->rt=NULL;
    if(ptr->up) ptr->up->dn=NULL;
    if(ptr->dn) ptr->dn->up=NULL;
    ptr->next=freep;
    freep=ptr;
    numboxes--;
}

struct cell *create(x,y,hv)
unsigned long x,y,hv;
{
    struct cell *ptr;

    if(freep == NULL){
	if((ptr= (struct cell *)malloc(sizeof(struct cell))) ==NULL){
	    perror("create: malloc error: ");
	    exit(-1);
	}
    }
    else{
	ptr=freep;
	freep=freep->next;
    }
    bzero(ptr,sizeof(struct cell));
    
    ptr->next=head;
    head=ptr;
    
    if(ptr->next != NULL){
	ptr->next->fore=ptr;
    }	
    ptr->hnext=cells[hv];
    cells[hv]= ptr;
    
    if(ptr->hnext != NULL){
	ptr->hnext->hfore=ptr;
    }
    ptr->x = x;
    ptr->y = y;
    numboxes++;
    return(ptr);

}

struct cell *link(x,y)
 unsigned long x,y;
{
    struct cell *ptr;
    unsigned long hv;
    
    x &= 0xfffffff8;
    y &= 0xfffffff8;
    hv=hash(x,y);
    
    if (cells[hv]==NULL) return(create(x,y,hv));
    ptr = cells[hv];
    while(ptr!=NULL){
	if ((ptr->x == x) && (ptr->y == y)){
	    return(ptr);
	}
	ptr=ptr->hnext;
    }

    return(create(x,y,hv));
}

void addcell(x,y)
 unsigned long x,y;
{
    unsigned long ydx,xdx;
    struct cell *ptr;
    
    ptr = link(x,y);
    ydx = y - ptr->y;
    xdx = x - ptr->x;
    ptr->dead=0;
    if(ydx > 3){
	ptr->live2 |= 1 << ( (ydx - 4)*8  + xdx);
    }
    else{
	ptr->live1 |= 1 << ( ydx*8  + xdx);
    }
}

void deletecell(x,y)
unsigned long x,y;
{
    unsigned long ydx,xdx;
    struct cell *ptr;
    
    ptr = link(x,y);
    ydx = y - ptr->y;
    xdx = x - ptr->x;
    
    if(ydx > 3){
	ptr->live2 &= 0xffffffff^(1 << ( (ydx - 4)*8  + xdx));
    }
    else{
	ptr->live1 &= 0xffffffff^(1 << ( ydx*8  + xdx));
    }
}


XPoint onpoints[8192],offpoints[8192];

XRectangle onrects[4096],offrects[4096];
    
void displayline(line,x,y,oline)
unsigned long line,x,y,oline;
{
    int sc,yy;
    unsigned long diff;
    sc = 1 << (scale -1);
    diff=line ^ oline;
    if (scale == 1){
	if(diff & 0x1){
	    if(line & 0x1){
		onpoints[onpt].x= x - xpos;
		onpoints[onpt].y= y - ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= x - xpos;
		offpoints[offpt].y= y - ypos;
		offpt++;
	    }
	}
	++x;
	if(diff & 0x2){
	    if(line & 0x2){
		onpoints[onpt].x= x - xpos;
		onpoints[onpt].y= y - ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= x - xpos;
		offpoints[offpt].y= y - ypos;
		offpt++;
	    }
	}
	++x;
	if(diff & 0x4){
	    if(line & 0x4){
		onpoints[onpt].x= x - xpos;
		onpoints[onpt].y= y - ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= x - xpos;
		offpoints[offpt].y= y - ypos;
		offpt++;
	    }
	}
	++x;
	if(diff & 0x8){
	   if(line & 0x8){
		onpoints[onpt].x= x - xpos;
		onpoints[onpt].y= y - ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= x - xpos;
		offpoints[offpt].y= y - ypos;
		offpt++;
	    }
	}
	++x;
	if(diff & 0x10){
	    if(line & 0x10){
		onpoints[onpt].x= x - xpos;
		onpoints[onpt].y= y - ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= x - xpos;
		offpoints[offpt].y= y - ypos;
		offpt++;
	    }
	}
	++x;
	if(diff & 0x20){
	    if(line & 0x20){
		onpoints[onpt].x= x - xpos;
		onpoints[onpt].y= y - ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= x - xpos;
		offpoints[offpt].y= y - ypos;
		offpt++;
	    }
	}
	++x;
	if(diff & 0x40){
	    if(line & 0x40){
		onpoints[onpt].x= x - xpos;
		onpoints[onpt].y= y - ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= x - xpos;
		offpoints[offpt].y= y - ypos;
		offpt++;
	    }
	}
	++x;	
	if(diff & 0x80){
	    if(line & 0x80){
		onpoints[onpt].x= x - xpos;
		onpoints[onpt].y= y - ypos;
		onpt++;
	    }
	    else{
		offpoints[offpt].x= x - xpos;
		offpoints[offpt].y= y - ypos;
		offpt++;
	    }
	}
    }
    else{
	yy=((y - ypos) << (scale -1));
	if(diff & 0x1){
	    if(line & 0x1){
		onrects[onrect].x= ((x - xpos) << (scale -1));
		onrects[onrect].y= yy;
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((x - xpos) << (scale -1));
		offrects[offrect].y= yy;
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x2){
	    if(line & 0x2){
		onrects[onrect].x= ((x - xpos) << (scale -1));
		onrects[onrect].y= yy;
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((x - xpos) << (scale -1));
		offrects[offrect].y= yy;
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x4){
	    if(line & 0x4){
		onrects[onrect].x= ((x - xpos) << (scale -1));
		onrects[onrect].y= yy;
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((x - xpos) << (scale -1));
		offrects[offrect].y= yy;
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x8){
	    if(line & 0x8){
		onrects[onrect].x= ((x - xpos) << (scale -1));
		onrects[onrect].y= yy;
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((x - xpos) << (scale -1));
		offrects[offrect].y= yy;
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x10){
	    if(line & 0x10){
		onrects[onrect].x= ((x - xpos) << (scale -1));
		onrects[onrect].y= yy;
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((x - xpos) << (scale -1));
		offrects[offrect].y= yy;
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x20){
	    if(line & 0x20){
		onrects[onrect].x= ((x - xpos) << (scale -1));
		onrects[onrect].y= yy;
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((x - xpos) << (scale -1));
		offrects[offrect].y= yy;
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x40){
	    if(line & 0x40){
		onrects[onrect].x= ((x - xpos) << (scale -1));
		onrects[onrect].y= yy;
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((x - xpos) << (scale -1));
		offrects[offrect].y= yy;
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
	++x;
	if(diff & 0x80){
	    if(line & 0x80){
		onrects[onrect].x= ((x - xpos) << (scale -1));
		onrects[onrect].y= yy;
		onrects[onrect].width = sc;
		onrects[onrect].height = sc;
		onrect++;
	    }
	    else{
		offrects[offrect].x= ((x  - xpos) << (scale -1));
		offrects[offrect].y= yy;
		offrects[offrect].width = sc;
		offrects[offrect].height = sc;
		offrect++;
	    }
	}
    }
	
}

void displaybox(live1,live2,x,y,olive1,olive2)
unsigned long live1,live2,x,y,olive1,olive2;
{
    displayline(live1,x,y,olive1);
    displayline(live1>>8,x,++y,olive1>>8);
    displayline(live1>>16,x,++y,olive1>>16);
    displayline(live1>>24,x,++y,olive1>>24);
    displayline(live2,x,++y,olive2);
    displayline(live2>>8,x,++y,olive2>>8);
    displayline(live2>>16,x,++y,olive2>>16);
    displayline(live2>>24,x,++y,olive2>>24);
    
}

void displaystats()
{
    char stats[100];   
    if(dispcells){
	sprintf(stats,"Generations: %6lu, Boxes: %6lu, Cells: %7lu ",generations,numboxes,numcells);
    }
    else{	
	sprintf(stats,"Generations: %6lu, Boxes: %6lu",generations,numboxes);
    }
    XClearWindow(disp,inputw);
    XDrawString(disp, inputw, ntextgc,ICOORDS(0,0),stats, strlen(stats));
}

void redisplay()
{
    struct cell *ptr;
    unsigned long x,y,ctr=0;
    
    onpt=offpt=0;
    onrect=offrect=0;
    displaystats();    
    if(state==HIDE) return;
    ptr = head;

    while(ptr != NULL){
	x=ptr->x;
	y=ptr->y;
	if((((x > xpos-8) && (x < (xpos + 8 + (width >> (scale -1))))) && ((y > ypos-8) && (y < (ypos + 8 + (height >> (scale -1)))))) && (ptr->dead < 3)){
	    displaybox(ptr->live1,ptr->live2,x,y,ptr->olive1,ptr->olive2);
	    ctr++;
	    if(scale == 1){
		if(ctr==128){
		    XDrawPoints(disp,lifew,blackgc,offpoints,offpt,CoordModeOrigin);
		    XDrawPoints(disp,lifew,whitegc,onpoints,onpt,CoordModeOrigin);
		    onpt=offpt=0;
		    ctr=0;
		}
	    }
	    else{
		if(ctr==64){
		    XFillRectangles(disp,lifew,blackgc,offrects,offrect,CoordModeOrigin);
		    XFillRectangles(disp,lifew,whitegc,onrects,onrect,CoordModeOrigin);
		    onrect=offrect=0;
		    ctr=0;
		}
	    }
	}
    
	ptr = ptr->next;
    }
    if(ctr){
	if(scale == 1){
	    XDrawPoints(disp,lifew,blackgc,offpoints,offpt,CoordModeOrigin);
    	    XDrawPoints(disp,lifew,whitegc,onpoints,onpt,CoordModeOrigin);
	    onpt=offpt=0;
	    ctr=0;
	}
	else{
	    XFillRectangles(disp,lifew,blackgc,offrects,offrect,CoordModeOrigin);
	    XFillRectangles(disp,lifew,whitegc,onrects,onrect,CoordModeOrigin);
	    onrect=offrect=0;
	    ctr=0;
	}
    }
    XFlush(disp);
}


void center()
{
    double x,y;
    int ctr=0;
    struct cell *ptr;
    x=0.0;
    y=0.0;
    ptr=head;
    XClearWindow(disp,lifew);
    while(ptr){
	if(!ptr->dead){
	    x+= ptr->x;
	    y+= ptr->y;
	    ctr++;
	}
	ptr=ptr->next;
    }
    x/=ctr;
    y/=ctr;
    xpos=x- (width >> scale);
    ypos=y- (height >> scale);
    redrawscreen();
}
    

void clear()
{
    struct cell *ptr,*nptr;
    
    bzero(cells,4*HASHSIZE);
    ptr=head;

    while(ptr){
	nptr=ptr->next;
	free(ptr);
	ptr=nptr;
    }
    head=NULL;
    state=STOP;
    generations=0;
    numboxes=0;
    numcells=0;
    XClearWindow(disp,lifew);
    XClearWindow(disp,inputw);
    displaystats();
}



void saveline(line,x,y,savefl)
unsigned long line,x,y;
FILE *savefl;
{
    
    if(line & 0x1) fprintf(savefl,"%d %d\n",x - xpos,y - ypos);
    x++;
    if(line & 0x2) fprintf(savefl,"%d %d\n",x - xpos,y - ypos);
    x++;
    if(line & 0x4) fprintf(savefl,"%d %d\n",x - xpos,y - ypos);
    x++;
    if(line & 0x8) fprintf(savefl,"%d %d\n",x - xpos,y - ypos);
    x++;
    if(line & 0x10) fprintf(savefl,"%d %d\n",x - xpos,y - ypos);
    x++;
    if(line & 0x20) fprintf(savefl,"%d %d\n",x - xpos,y - ypos);
    x++;
    if(line & 0x40) fprintf(savefl,"%d %d\n",x - xpos,y - ypos);
    x++;
    if(line & 0x80) fprintf(savefl,"%d %d\n",x - xpos,y - ypos);
}
    
void savebox(live1,live2,x,y,savefl)
unsigned long live1,live2,x,y;
FILE *savefl;
{
    saveline(live1,x,y,savefl);
    saveline(live1>>8,x,++y,savefl);
    saveline(live1>>16,x,++y,savefl);
    saveline(live1>>24,x,++y,savefl);
    saveline(live2,x,++y,savefl);
    saveline(live2>>8,x,++y,savefl);
    saveline(live2>>16,x,++y,savefl);
    saveline(live2>>24,x,++y,savefl); 
}

void redrawscreen()
{
    struct cell *ptr;

    ptr=head;
    
    while (ptr){
	ptr->olive1^=ptr->olive1;
	ptr->olive2^=ptr->olive2;
	ptr=ptr->next;
    }
    redisplay();
}
	    

void newrules()
{
    int i,j,k;
    char *ptr;
    
    if(state==RULES){
	ptr=inpbuf;
	strcpy(inpbuf,"Rules:   ");
	ptr=inpbuf+7;
	k=live;
	for(i=0; i<9; i++)
	{	if(k&1)
		{   *ptr=i+48;
		    ptr++;
		}
		k>>=1;
	}
	*ptr='/';
	ptr++;
	k=born;
	for(i=0; i<9; i++)
	{	if(k&1)
		{   *ptr=i+48;
		    ptr++;
		}
		k>>=1;
	}
	strcpy(ptr,"   New Rules:  ");
	minbuflen=strlen(inpbuf);
	XClearWindow(disp,inputw);
	XDrawString(disp, inputw, ntextgc,ICOORDS(0,0),inpbuf, strlen(inpbuf));
    }
    else{
	k=0;
	ptr=inpbuf+minbuflen;
	while((*ptr>47)&&(*ptr<58))
	{   k|=(1<<(*ptr-48));
	    ptr++;
	}
	live=k;
	k=0;
	if(*ptr=='/')
	{   ptr++;
	    while((*ptr>47)&&(*ptr<58))
	    {   k|=(1<<(*ptr-48));
		ptr++;
	    }
	    born=k;
	}
	XClearWindow(disp,inputw);
    }
}

void randomize(){
    unsigned long num;
    int x;

    for(num = (width * height) >> (scale + 6);num;num--){
	addcell(random()%(width>>(scale-1))+xpos,random()%(height>>(scale-1))+ypos);
    }
    redisplay();
}

