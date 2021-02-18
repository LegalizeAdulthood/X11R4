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
 *
 * CMU SUCKS
 */

/****************************************************************

			 /-------------\
			/		\
		       /		 \
		      /			  \
		      |	  XXXX	   XXXX	  |
		      |	  XXXX	   XXXX	  |
		      |	  XXX	    XXX	  |
		      \		X	  /
		       --\     XXX     /--
			| |    XXX    | |
			| |	      | |
			| I I I I I I I |
			|  I I I I I I	|
			 \	       /
			  --	     --
			    \-------/
		    XXX			   XXX
		   XXXXX		  XXXXX
		   XXXXXXXXX	     XXXXXXXXXX
			  XXXXX	  XXXXX
			     XXXXXXX
			  XXXXX	  XXXXX
		   XXXXXXXXX	     XXXXXXXXXX
		   XXXXX		  XXXXX
		    XXX			   XXX

			  **************
			  *  BEWARE!!  *
			  **************

			All ye who enter here:
		    Most of the code in this module
		       is twisted beyond belief!

			   Tread carefully.

		    If you think you understand it,
			      You Don't,
			    So Look Again.

 ****************************************************************/
/*
(The above was taken from the emacs display code by James Gosling)
  but it applies here as well.......
*/

#include "defs.h"
#include "data.h"
#include "struct.h"
#include "tab.h"
    
    void generate(){

	register u_long *tmpptr;
	register u_long t1,t2,t3,t4,y,tlive,tl1,tl2,add;
	struct cell *cptr,*tptr,*cptrup,*cptrdn,*cptrlf,*cptrrt;
	cptr = head;
	numcells=0;
	generations++;
	while(cptr ){
	    cptrup=cptr->up;
	    cptrdn=cptr->dn;
	    cptrlf=cptr->lf;
	    cptrrt=cptr->rt;
	    
	    t1=cptr->live1&0xff;
	    if(t1){
		if(cptrup==NULL){
		    cptrup=link(cptr->x,cptr->y - 8);
		    cptrup->dn=cptr;
		}
		t2=tab1[t1];
		cptrup->on[7]+=t2;
		cptr->on[1]+=t2;
		cptr->on[0]+=tab2[t1];
	    }
	    
	    t1=(cptr->live2 & 0xff000000)>>24;	    
	    if(t1){
		if(cptrdn==NULL){
		    cptrdn=link(cptr->x,cptr->y + 8);
		    cptrdn->up=cptr;
		}
		t2=tab1[t1];
		cptrdn->on[0]+=t2;
		cptr->on[6]+=t2;
		cptr->on[7]+=tab2[t1];
	    }
	    
	    if((cptr->live1 & 0x1010101) || (cptr->live2 & 0x1010101)){
		if(cptrlf==NULL){
		    cptrlf=link(cptr->x - 8,cptr->y);
		    cptrlf->rt=cptr;
		}
		add=(cptr->live1 & 0x1)?0x10000000:0;
		cptrlf->on[0]+=add;
		cptrlf->on[1]+=add;
		for(y=1;y<4;y++){
		    add=(cptr->live1 & (0x1 << (y<<3)))?0x10000000:0;
		    cptrlf->on[y-1]+=add;
		    cptrlf->on[y]+=add;
		    cptrlf->on[y+1]+=add;
		    
		}
		for(y=4;y<7;y++){
		    add=(cptr->live2 & (0x1 << ((y-4)<<3)))?0x10000000:0;
		    cptrlf->on[y-1]+=add;
		    cptrlf->on[y]+=add;
		    cptrlf->on[y+1]+=add;
		}
		add=(cptr->live2 & 0x1000000)?0x10000000:0;
		cptrlf->on[6]+=add;
		cptrlf->on[7]+=add;

		
	    if(cptr->live1 & 0x1){
		if(cptrlf->up==NULL){
		    cptrlf->up=link(cptr->x - 8,cptr->y - 8);
		}
		cptrlf->up->on[7]+= 0x10000000;
		cptrlf->up->dn=cptrlf;
	    }
	    
		if(cptr->live2 & 0x1000000){
		    if(cptrlf->dn==NULL){
			cptrlf->dn=link(cptr->x - 8,cptr->y + 8);
		    }
		    cptrlf->dn->on[0]+= 0x10000000;
		    cptrlf->dn->up=cptrlf;
		}
	    
	    }
	    
	    if((cptr->live1 & 0x80808080) || (cptr->live2 & 0x80808080)){
		if(cptrrt == NULL){
		    cptrrt=link(cptr->x + 8,cptr->y);
		    cptrrt->lf=cptr;
		}
		add=(cptr->live1 & 0x80)?0x1:0;
		cptrrt->on[0]+=add;
		cptrrt->on[1]+=add;
		for(y=1;y<4;y++){
		    add=(cptr->live1 & (0x80 << (y<<3)))?0x1:0;
		    cptrrt->on[y-1]+=add;
		    cptrrt->on[y]+=add;
		    cptrrt->on[y+1]+=add;
		}
		for(y=4;y<7;y++){
		    add=(cptr->live2 & (0x80 << ((y-4)<<3)))?0x1:0;
		    cptrrt->on[y-1]+=add;
		    cptrrt->on[y]+=add;
		    cptrrt->on[y+1]+=add;
		}
		add=(cptr->live2 & 0x80000000)?0x1:0;
		cptrrt->on[6]+=add;
		cptrrt->on[7]+=add;

			    
		if(cptr->live1 & 0x80){
		    if(cptrrt->up==NULL){
			cptrrt->up=link(cptr->x + 8,cptr->y - 8);
		    }
		    cptrrt->up->on[7]+= 0x1;
		    cptrrt->up->dn=cptrrt;
		}
		
		if(cptr->live2 & 0x80000000){
		    if(cptrrt->dn==NULL){
			cptrrt->dn=link(cptr->x + 8,cptr->y + 8);
		    }
		    cptrrt->dn->on[0]+= 0x1;
		    cptrrt->dn->up=cptrrt;
		}
	    	    
	    }
	    
	    t1=(cptr->live1 & 0xff00) >> 8;
	    t2=(cptr->live1 & 0xff0000) >> 16;
	    
	    if(t1){
		t3 = tab1[t1];
		cptr->on[1]+=tab2[t1];
		cptr->on[0]+=t3;
		cptr->on[2]+=t3;
	    }

	    t1=(cptr->live1 & 0xff000000) >> 24;
	    	    
	    if(t2){
		t3 = tab1[t2];
		cptr->on[2]+=tab2[t2];
		cptr->on[1]+=t3;
		cptr->on[3]+=t3;
	    }

	    t2=(cptr->live2 & 0xff);
	    	    
	    if(t1){
		t3 = tab1[t1];
		cptr->on[3]+=tab2[t1];
		cptr->on[2]+=t3;
		cptr->on[4]+=t3;
	    }

	    t1=(cptr->live2 & 0xff00) >> 8;

	    if(t2){
		t3 = tab1[t2];
		cptr->on[4]+=tab2[t2];
		cptr->on[3]+=t3;
		cptr->on[5]+=t3;
	    }
	    
	    t2=(cptr->live2 & 0xff0000) >> 16;	    

	    if(t1){
		t3 = tab1[t1];
		cptr->on[5]+=tab2[t1];
		cptr->on[4]+=t3;
		cptr->on[6]+=t3;
	    }
	    
	    if(t2){
		t3 = tab1[t2];
		cptr->on[6]+=tab2[t2];
		cptr->on[5]+=t3;
		cptr->on[7]+=t3;
	    }

	    cptr->up=cptrup;
	    cptr->dn=cptrdn;
	    cptr->lf=cptrlf;
	    cptr->rt=cptrrt;
	    	    
	    cptr=cptr->next;
	}
	
	cptr=head;
	while(cptr){
	    tl1=cptr->live1;
	    tl2=cptr->live2;
	    cptr->olive1=tl1;
	    cptr->olive2=tl2;
	    tlive=0;
	    tmpptr=cptr->on;
	    for(y=0;y < 32;y+=8){
		t4=tl1>>y;
		t3= *tmpptr++;
		
		t1=(0x1<<((t3)&0xf));
		t2=t4&0x1;
		if ((t2&&(t1&live))||((!t2)&&(t1&born)))
		    tlive |= 0x1<<(y);

		t1=(0x1<<((t3>>(4))&0xf));
		t2=t4&0x2;
		if ((t2&&(t1&live))||((!t2)&&(t1&born)))
		    tlive |= 0x2<<(y);
				
		t1=(0x1<<((t3>>(8))&0xf));
		t2=t4&0x4;
		if ((t2&&(t1&live))||((!t2)&&(t1&born)))
		    tlive |= 0x4<<(y);
				
		t1=(0x1<<((t3>>(12))&0xf));
		t2=t4&0x8;
		if ((t2&&(t1&live))||((!t2)&&(t1&born)))
		    tlive |= 0x8<<(y);

		t1=(0x1<<((t3>>(16))&0xf));
		t2=t4&0x10;
		if ((t2&&(t1&live))||((!t2)&&(t1&born)))
		    tlive |= 0x10<<(y);
				
		t1=(0x1<<((t3>>(20))&0xf));
		t2=t4&0x20;
		if ((t2&&(t1&live))||((!t2)&&(t1&born)))
		    tlive |= 0x20<<(y);
		
		t1=(0x1<<((t3>>(24))&0xf));
		t2=t4&0x40;
		if ((t2&&(t1&live))||((!t2)&&(t1&born)))
		    tlive |= 0x40<<(y);
                         
		t1=(0x1<<((t3>>28)));
		t2=t4&0x80;
		if ((t2&&(t1&live))||((!t2)&&(t1&born)))
		    tlive |= 0x80<<(y);

	    }

	    cptr->live1 = tlive;	
	    tlive=0;
		
	    for(y=0;y < 32;y+=8){
		t4=tl2>>y;
		t3= *tmpptr++;

		t1=(0x1<<((t3)&0xf));
		t2=t4&0x1;
		if ((t2&&(t1&live))||((!t2)&&(t1&born)))
		    tlive |= 0x1<<(y);

		t1=(0x1<<((t3>>(4))&0xf));
		t2=t4&0x2;
		if ((t2&&(t1&live))||((!t2)&&(t1&born)))
		    tlive |= 0x2<<(y);
				
		t1=(0x1<<((t3>>(8))&0xf));
		t2=t4&0x4;
		if ((t2&&(t1&live))||((!t2)&&(t1&born)))
		    tlive |= 0x4<<(y);
				
		t1=(0x1<<((t3>>(12))&0xf));
		t2=t4&0x8;
		if ((t2&&(t1&live))||((!t2)&&(t1&born)))
		    tlive |= 0x8<<(y);

		t1=(0x1<<((t3>>(16))&0xf));
		t2=t4&0x10;
		if ((t2&&(t1&live))||((!t2)&&(t1&born)))
		    tlive |= 0x10<<(y);
				
		t1=(0x1<<((t3>>(20))&0xf));
		t2=t4&0x20;
		if ((t2&&(t1&live))||((!t2)&&(t1&born)))
		    tlive |= 0x20<<(y);
		
		t1=(0x1<<((t3>>(24))&0xf));
		t2=t4&0x40;
		if ((t2&&(t1&live))||((!t2)&&(t1&born)))
		    tlive |= 0x40<<(y);
                         
		t1=(0x1<<((t3>>28)));
		t2=t4&0x80;
		if ((t2&&(t1&live))||((!t2)&&(t1&born)))
		    tlive |= 0x80<<(y);
		
	    }
	    cptr->live2 = tlive;

	    tmpptr=cptr->on;
	    *tmpptr=0;
	    *(tmpptr+1)=0;
	    *(tmpptr+2)=0;
	    *(tmpptr+3)=0;
	    *(tmpptr+4)=0;
	    *(tmpptr+5)=0;
	    *(tmpptr+6)=0;
	    *(tmpptr+7)=0;

	    if(dispcells){
		for(y=0;y<32;y++)
		    if(cptr->live1 & (1<<y)) numcells++;
		for(y=0;y<32;y++)
		    if(cptr->live2 & (1<<y)) numcells++;
	    }
		
	    if(cptr->live1 | cptr->live2){
		cptr->dead=0;
		cptr=cptr->next;
	    }
	    else{
		cptr->dead++;
		if(cptr->dead > maxdead){
		    tptr=cptr->next;
    		    kill(cptr);
		    cptr=tptr;
		}
		else{
		    cptr=cptr->next;
		}
	    }
	}
	redisplay();
    }


