/* pbmtobg.c - read a portable bitmap and produce BitGraph graphics
**
** Copyright 1989 by Mike Parker.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/

#include <stdio.h>

#include "pbm.h"

int nco;

main(ac,av)
int ac;
char **av;
{
 int rows;
 int cols;
 int format;
 bit *bitrow;
 int row;
 unsigned int sixteen;
 int i;
 unsigned int mask;
 int op;
 int x;
 int y;

 op = 3;
 switch (ac)
  { case 1:
       break;
    case 2:
       op = atoi(av[1]);
       break;
    case 3:
       x = atoi(av[1]);
       y = atoi(av[2]);
       printf("\33:%d;%dm",x,y);
       break;
    case 4:
       op = atoi(av[1]);
       x = atoi(av[2]);
       y = atoi(av[3]);
       printf("\33:%d;%dm",x,y);
       break;
  }
 nco = 0;
 pbm_readpbminit(stdin,&cols,&rows,&format);
 printf("\33P:%d;%d;%ds\n",op,cols,rows);
 bitrow = pbm_allocrow(cols);
 for (row=0;row<rows;row++)
  { pbm_readpbmrow(stdin,bitrow,cols,format);
    sixteen = 0;
    mask = 0x8000;
    for (i=0;i<cols;i++)
     { if (bitrow[i]==PBM_BLACK) sixteen |= mask;
       mask >>= 1;
       if (mask == 0)
	{ mask = 0x8000;
	  write16(sixteen);
	  sixteen = 0;
	}
     }
    if (mask != 0x8000)
     { write16(sixteen);
     }
  }
 putchar('\n');
 exit(0);
}

write16(sixteen)
unsigned int sixteen;
{
 if (nco > 75)
  { putchar('\n');
    nco = 0;
  }
 if (sixteen & 0xfc00)
  { putchar(0100+(sixteen>>10));
    nco ++;
  }
 if (sixteen & 0xfff0)
  { putchar(0100+((sixteen>>4)&0x3f));
    nco ++;
  }
 putchar(060+(sixteen&0xf));
 nco ++;
}
