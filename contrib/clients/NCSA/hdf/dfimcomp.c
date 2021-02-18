/*****************************************************************************
* 
*			  NCSA HDF version 3.00
*				December, 1989
*
* NCSA HDF Version 3.00 source code and documentation are in the public
* domain.  Specifically, we give to the public domain all rights for future
* licensing of the source code, all resale rights, and all publishing rights.
* 
* We ask, but do not require, that the following message be included in all
* derived works:
* 
* Portions developed at the National Center for Supercomputing Applications at
* the University of Illinois at Urbana-Champaign.
* 
* THE UNIVERSITY OF ILLINOIS GIVES NO WARRANTY, EXPRESSED OR IMPLIED, FOR THE
* SOFTWARE AND/OR DOCUMENTATION PROVIDED, INCLUDING, WITHOUT LIMITATION,
* WARRANTY OF MERCHANTABILITY AND WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE
* 
* NCSA HDF Group:
*  Mike Folk, Swami Natarajan, Chin_Chau Low, Likkai Ng, Peter Webb,
*  Brian Calvert and Drew Hess.
*****************************************************************************/

/************************************************************************/
/*  Module Name : imcomp                        */
/*  Exports     : DFCimcomp(), DFCunimcomp()                */
/*  Purpose     : Compresses color images               */
/*  Author  : Eng-Kiat Koh                      */
/*  Date    : June 30th, 1988                   */
/*  Functions   : DFCimcomp(), compress(), init_global(), cnt_color()   */
/*        set_palette(), fillin_color(), map(), nearest_color() */
/*        DFCunimcomp(), sqr()                                  */
/************************************************************************/


#include "df.h"

#define PALSIZE 256
#define BIT8 0
#define BIT24 1

#ifndef MAC
#define MAXCOLOR 32768
#else /*MAC*/
#define MAXCOLOR 768
#endif /*MAC*/

#define RED 0
#define GREEN 1
#define BLUE 2
#define EPSILON 0.5
#define LO 1
#define HI 0

struct rgb
{
  char c[3];
};

struct box
{
  float bnd[3][2];
  int *pts;
  int nmbr_pts;
  int nmbr_distinct;
  struct box *left;
  struct box *right;
};


char *new_pal;          /* pointer to new palette           */


static int *hist;                /* histogram for distinct colors    */
static struct box *frontier;     /* pointer to the list of boxes     */
static struct rgb *distinct_pt;  /* contains all distinct rgb points */

static void sel_palette();
static void compress();
static void init_global();
static int cnt_color();
static void set_palette();
static void fillin_color();
static int indx();
static void map();
static int nearest_color();
static long int sqr();
static void init();
static int partition();
static struct box *find_box();
static void split_box();
static void assign_color();
static int select_dim();
static float find_med();
static void classify();
static int next_pt();

extern char *calloc();

static struct rgb *color_pt;	/* contains the hi-lo colors for each block */
static char *image;		/* contains the compressed image            */
static int trans[MAXCOLOR];     /* color translation table                  */

/************************************************************************/
/*  Function    : DFCimcomp                                             */
/*  Purpose : Performs Imcomp Compression                           */
/*  Parameters  :                                                       */
/*    xdim, ydim - dimensions of image                                  */
/*                 IT IS ASSUMED THAT THE DIMENSIONS ARE A MULTIPLE OF 4*/
/*    in, out    - input image array and output image buffer size of in */
/*                 is xdim*ydim bytes for 8 bit per pixel mode. It is 3 */
/*                 times that for 24 bits per pixel mode. The output    */
/*                 buffer is always (xdim*ydim)/4.                      */
/*    in_pal     - input palette. Consist of rgb triples unlike seq-type*/
/*                 palette. This is a NULL pointer if operating at the  */
/*                 24 bit per pixel mode.                               */
/*    out_pal    - output palette. Consist of PALSIZE color entries.    */
/*                 each entry is an rgb triple.                         */
/*    mode       - Either BIT8 or BIT24                                 */
/*  Returns     : none                          */
/*  Called by   : External routines                                     */
/*  Calls       : init_global(), compress(), cnt_color(), set_palette(),*/
/*        sel_palette(), map()                                  */
/************************************************************************/

void DFCimcomp(xdim, ydim, in, out, in_pal, out_pal, mode)
int32 xdim, ydim;
int mode;
char in[], out[], in_pal[], out_pal[];
{
  char raster[48];
  int blocks, nmbr, i, j, k, l, x, y;
  void init_global();
  void compress();
  int cnt_color();
  void set_palette();
  void sel_palette();
  void map();
  void fillin_color();

  init_global(xdim, ydim, out, out_pal);

  /* compress pixel blocks */
  blocks = 0;
  for (i=0; i<(ydim/4); i++)
    for (j=0; j<(xdim/4); j++)
    {
      switch (mode)
      {
        case BIT8 : /* 8 bit per pixel format */
            k = 0;
            for (y=(i*4); y<(i*4+4); y++)
              for (x=(j*4); x<(j*4+4); x++)         
              {
            l = y*xdim + x;
            raster[k++] = in_pal[3*in[l]];
            raster[k++] = in_pal[3*in[l]+1];
            raster[k++] = in_pal[3*in[l]+2];
              } /* end of for x */
            compress(raster,blocks);
            break;

    case BIT24: /* 24 bit per pixel format */
            k = 0;
            for (y=(i*4); y<(i*4+4); y++)
              for (x=(j*4); x<(j*4+4); x++)         
              {
            l = 3 *(y*xdim + x);
            raster[k++] = in[l];
            raster[k++] = in[l+1];
            raster[k++] = in[l+2];
              } /* end of for x */
            compress(raster,blocks);
            break;
    
    default   : /* unsupported format */
            printf("Error : Unsupported Format \n");
            exit(1);
            break;
      } /* end of switch */
      
      blocks++;
    } /* end of for j */
 
  /* set palette */
  nmbr = cnt_color(blocks);
/*
  printf("Number of colors %d \n", nmbr);
*/
  if (nmbr <= PALSIZE)
    set_palette(blocks);
  else
  {
    sel_palette(blocks,nmbr,color_pt);
    map(blocks);
  }

  fillin_color(blocks);

} /* end of DFCimcomp */






/************************************************************************/
/*  Function    : compress                                  */
/*  Purpose : Given a block of 16 pixels, sets up a 16 bit bitmap   */
/*                and assigns a lo and hi color for the block. For block*/
/*                i, hi color is stored in color_pt[2i] and lo in       */
/*                color_pt[2i+1]. Each color is then reduced to 15 bits */
/*                by truncating the lower order 3 bits of each component*/
/*  Parameter   :                           */
/*    raster     - contains the 16 pixels of a block. Each pixel is 3   */
/*         bytes, 1 byte for each color component               */
/*    block  - pixel block number                                   */
/*  Returns     : none                                                  */
/*  Called by   : DFCimcomp()                       */
/*  Calls       : none                          */
/************************************************************************/

static void compress(raster, block)
char raster[];
int block;
{
  float y[16], y_av;
  int i, j, k, l, bit;
  int high, hi, lo;
  int c_hi[3], c_lo[3];

  /* calculate luminance */
  y_av = 0.0;
  for (i=0; i<16; i++)
  {
    j = 3*i;
    y[i] = 0.3*(float)raster[j] + 0.59*(float)raster[j+1] + 
           0.11*(float)raster[j+2];
/*    printf("compress: y[%d] is %f\n",i,y[i]);*/
    y_av = y_av + y[i];
  }
  y_av = y_av / 16.0;
/*  printf("y_av is %f\n",y_av); */

  /* initialize c_hi and c_lo */
  for (i=RED; i<=BLUE; i++)
  {
    c_hi[i] = 0;
    c_lo[i] = 0;
  }

  /* build bit map */
  k = 4 * block;
  high = 0;
  hi = 2 * block;
  lo = hi + 1;
  for (i=0; i<2; i++)
  {
    bit = 128;
    for (j = (i*8); j<(i*8+8); j++)
    {
      if (y[j] > y_av)
      {
    image[k] = image[k] | bit;
    high++;
        for (l=RED; l<= BLUE; l++)
      c_hi[l] = c_hi[l] + (int)raster[3*j+l];
      }
      else
      {
    for (l=RED; l<=BLUE; l++)
          c_lo[l] = c_lo[l] + (int)raster[3*j+l];
      } /* end of if */

      bit = bit >> 1;
    } /* end of for j */
   
    k++;
  } /* end of for i */

  /* calculate hi lo color */
  for (i=RED; i<=BLUE; i++)
  {
    if (high != 0)
      color_pt[hi].c[i] = (int)((float)c_hi[i] / (float)high);
    if (high != 16)
      color_pt[lo].c[i] = (int)((float)c_lo[i] / (float)(16 - high));
    color_pt[hi].c[i] = color_pt[hi].c[i] >> 3;
    color_pt[lo].c[i] = color_pt[lo].c[i] >> 3;

  }
} /* end of compress */






/************************************************************************/
/*  Function    : init_global                       */
/*  Purpose : Allocates memory for global variables                 */
/*  Parameter   :                           */
/*    xdim, ydim - x and y dimension of image               */
/*    out        - pointer to output buffer                             */
/*    out_pal    - pointer to output palette                            */
/*  Returns     : none                              */
/*  Called by   : DFCimcomp()                       */
/*  Calls       : none                          */
/************************************************************************/

static void init_global(xdim, ydim, out, out_pal)
int32 xdim, ydim;
char *out, *out_pal;
{
  int i, j;

  /* allocate memory */
  image = out;
  new_pal = out_pal;
  color_pt = (struct rgb *)calloc((unsigned)((xdim*ydim)/8),
         sizeof(struct rgb));

  if (image == NULL || color_pt == NULL || new_pal == NULL)
  {
    printf("Error : Out of Memory\n");
    exit(1);
  }

  /* initialize */
  for (i=0; i<(xdim*ydim/4); i++)
    image[i] = 0;

  for (i=0; i<(xdim*ydim/8); i++)
    for (j=RED; j<= BLUE; j++)
      color_pt[i].c[j] = 0;

  for (i=0; i<MAXCOLOR; i++)
    trans[i] = -1;
} /* end of init_global */





/************************************************************************/
/*  Function    : cnt_color                                 */
/*  Purpose : Counts the number of distinct colors compressd image  */
/*  Parameter   :                           */
/*    blocks     - total number of pixel blocks             */
/*  Returns     : Number of distinct colors                             */
/*  Called by   : DFCimcomp()                                           */
/*  Calls       : indx()                        */
/************************************************************************/

static int cnt_color(blocks)
int blocks;
{
  int temp[MAXCOLOR];
  int i, k, count;
  int indx();

  for (i=0; i<MAXCOLOR; i++)
    temp[i] = -1;

  for (i=0; i<(2*blocks); i++)
  {
    k = indx(color_pt[i].c[RED],color_pt[i].c[GREEN],color_pt[i].c[BLUE]);
/*    printf("cnt_color: k is %d\n",k); */
    temp[k] = 0;
  }

  count  = 0;
  for (i = 0; i< MAXCOLOR; i++)
    if (temp[i] == 0)
      count++;

  return count;
} /* end of cnt_color */





/************************************************************************/
/*  Function    : set_palette                       */
/*  Purpose : The number of distinct colors is less than the desired*/
/*                output palette size. Therefore each distinct color can*/
/*        be a palette entry. Function enters each distinct     */
/*                color as a palette entry and sets up the translation  */
/*                table. It also shifts each color component left 3 bits*/
/*                so that each color component is again 8 bits wide     */
/*  Parameter   :                           */
/*    blocks     - total number of pixel blocks                         */
/*  Returns     : none                          */
/*  Called by   : DFCimcomp()                       */
/*  Calls       : indx()                        */
/************************************************************************/

static void set_palette(blocks)
int blocks;
{
  int ent, i, k;
  int indx();

  ent = 0;
  for (i = 0; i<(2*blocks); i++)
  {
    k = indx(color_pt[i].c[RED],color_pt[i].c[GREEN],color_pt[i].c[BLUE]);
    if (trans[k] == -1)
    {
      new_pal[3*ent] = color_pt[i].c[RED] << 3;
      new_pal[3*ent+1] = color_pt[i].c[GREEN] << 3;
      new_pal[3*ent+2] = color_pt[i].c[BLUE] << 3;  
      trans[k] = ent;
      ent++;
    }
  }
} /* end of set_palette */





/************************************************************************/
/*  Function    : fillin_color                      */
/*  Purpose : For each pixel block, fills in the pointers into the  */
/*                palette.                                              */
/*  Parameter   :                           */
/*    blocks     - total number of pixel blocks             */
/*  Returns     : none                          */
/*  Called by   : DFCimcomp()                       */
/*  Calls       : none                          */
/************************************************************************/

static void fillin_color(blocks)
int blocks;
{
  int i, j, k;

  for (i=0; i<blocks; i++)
    for (j=HI; j<=LO; j++)
    {
      k = indx(color_pt[2*i+j].c[RED],color_pt[2*i+j].c[GREEN],
           color_pt[2*i+j].c[BLUE]);
      image[i*4+2+j] = trans[k];
    }
} /* end of fillin_color */





/************************************************************************/
/*  Function    : indx                          */
/*  Purpose : Maps an rgb triple (5 bits each) to an integer array  */
/*        index                         */
/*  Parameter   :                           */
/*    r, g, b    - color components                 */
/*  Returns     : returns an array index                */
/*  Called by   : set_palette(), fillin_color(), map()                  */
/*  Calls       : none                          */
/************************************************************************/

static int indx(r, g, b)
char r, g, b;
{
  int temp;

  temp = 0;
  temp = (r << 10) | (g  << 5) | b ;
  return temp;
} /* end of indx */






/************************************************************************/
/*  Function    : map                           */
/*  Purpose : Maps a color_pt to the closest representative color   */
/*        Sets up translation table             */
/*  Parameter   :                           */
/*    blocks     - total number of pixel blocks             */
/*  Returns     : none                          */
/*  Called by   : DFCimcomp()                       */
/*  Calls       : nearest_color()                   */
/************************************************************************/

static void map(blocks)
int blocks;
{
  int i, k;
  int r, g, b;
  int indx();
  int nearest_color();

  for (i=0; i<(2*blocks); i++)
  {
    k = indx(color_pt[i].c[RED], color_pt[i].c[GREEN], color_pt[i].c[BLUE]);
 
    if (trans[k] == -1)
    {
      r = color_pt[i].c[RED]<<3;
      g = color_pt[i].c[GREEN]<<3;
      b = color_pt[i].c[BLUE]<<3;
      trans[k] = nearest_color(r, g, b);
/*
      printf("map: %d %d %d mapped to %d %d %d\n", r, g, b, new_pal[trans[k]*3],
          new_pal[trans[k]*3+1], new_pal[trans[k]*3+2]);
*/
    }
  }
} /* end of map */
  



/************************************************************************/
/*  Function    : nearest_color                     */
/*  Purpose : Finds the nearest palette color           */
/*  Parameter   :                           */
/*    r, g, b    - color component                  */
/*  Returns     : Entry number of the closest color in the palette      */
/*  Called by   : map()                         */
/*  Calls       : sqr()                         */
/************************************************************************/

static int nearest_color(r, g, b)
char r, g, b;
{
  int i, near; 
  long int min, error;
  long int sqr();

  min = sqr(r-new_pal[0]) + sqr(g-new_pal[1]) + sqr(b-new_pal[2]);
  near = 0;
  for (i=1; i<PALSIZE; i++)
  {
    error = sqr(r-new_pal[3*i]) + sqr(g-new_pal[3*i+1]) + 
        sqr(b-new_pal[3*i+2]);
    if (error < min)
    {
    min = error;
        near = i;
    }
  }

  return near;
} /* end of nearest_color */




/************************************************************************/
/*  Function    : sqr                           */
/*  Purpose : Computes the square of an integer         */
/*  Parameter   :                           */
/*    x      - an integer                       */
/*  Returns     : The square of x                   */
/*  Called by   : nearest_color()                   */
/*  Calls       : none                          */
/************************************************************************/

static long int sqr(x)
int x;
{
  return (x*x);
}





/************************************************************************/
/*  Function    : DFCunimcomp                       */
/*  Purpose : 'Decompresses' the compressed image           */
/*  Parameter   :                           */
/*    xdim, ydim - dimensions of image                  */
/*    in, out    - Input buffer and output buffer. Size of input buffer */
/*         is (xdim*ydim)/4. Size of output buffer is 4 times   */
/*         that. It 'restores' images into seq-type files       */
/*  Returns     : none                          */
/*  Called by   : External routines                 */
/*  Calls       : none                          */
/************************************************************************/

void DFCunimcomp(xdim, ydim, in, out)
int32 xdim, ydim;
char in[], out[];
{
  int bitmap, temp;
  int i, j, k, x, y;
  char hi_color, lo_color;

  for (y=0; y<(ydim/4); y++)
    for (x=0; x<xdim; x=x+4)
    {
      k = y*xdim + x;
      hi_color = in[k+2]; 
      lo_color = in[k+3];

      bitmap = (in[k] << 8) | in[k+1];

      for (i=(y*4); i<(y*4+4); i++)
      {
        temp = bitmap >> (3 + y*4 - i)*4;
        for (j=x; j<(x+4); j++)
        {
      if ((temp & 8) == 8)
        out[i*xdim+j] = hi_color;
      else
        out[i*xdim+j] = lo_color;
      temp = temp << 1;
    }
      }
    } /* end of for x */
} /* end of DFCunimcomp */



/************************************************************************/
/*  Module Name : color                         */
/*  Exports     : sel_palette(); new_pal, pointer to a new color palette*/
/*  Purpose     : Quantizes colors                  */
/*  Author  : Eng-Kiat Koh                      */
/*  Date    : June 30th, 1988                   */
/*  Functions   : sel_palette(), init(), sort(), partition(), find_box()*/
/*        split_box(), assign_color(), select_dim(), find_med() */
/*                classify(), next_pt()                                 */
/************************************************************************/







/************************************************************************/
/*  Function    : sel_palette                       */
/*  Purpose : Selects PALSIZE palette colors out of a list of colors*/
/*        in color_pt                       */
/*  Parameter   :                           */
/*    blocks     - number of pixel blocks               */
/*    distinct   - number of distinct colors                */
/*    color_pt   - contains the lo hi colors for each pixel block       */
/*  Returns     : none                          */
/*  Called by   : DFCimcomp()                       */
/*  Calls       : init(), split_box(), find_box(), assign_color()   */
/************************************************************************/

static void sel_palette(blocks,distinct, color_pt)
int blocks, distinct;
struct rgb *color_pt;
{
  int boxes;
/*  int i, j;*/
  struct box *ptr;
  struct box *find_box();
  void init();
  void split_box();
  void assign_color();

  init(blocks, distinct, color_pt);

  /* split box into smaller boxes with about equal number of points */
  for (boxes=1; boxes < PALSIZE; boxes++)
  {
/*
    ptr=frontier->right;
    j = 0;
    while (ptr != NULL)
    {
      printf("Box %d, distinct %d, total %d\n",j,ptr->nmbr_distinct,
             ptr->nmbr_pts);
      for (i=0; i<ptr->nmbr_distinct; i++)
        printf("pt %d: %d %d %d",i,distinct_pt[ptr->pts[i]].c[RED],
                distinct_pt[ptr->pts[i]].c[GREEN], 
            distinct_pt[ptr->pts[i]].c[BLUE]);
      j++;
      ptr = ptr->right;
    }
*/

    ptr = find_box();
    split_box(ptr);
  } 

  assign_color();
}






/************************************************************************/
/*  Function    : init                          */
/*  Purpose : Initializes the global variables, sets up the first   */
/*        box. It will contain all the color points     */
/*  Parameter   :                           */
/*    blocks     - number of pixel blocks               */
/*    distinct   - number of distinct colors                */
/*    color_pt   - contains the lo hi colors for each pixel block       */
/*  Returns     : none                          */
/*  Called by   : sel_palette()                     */
/*  Calls       : none                          */
/************************************************************************/

static void init(blocks, distinct, color_pt)
int blocks, distinct;
struct rgb *color_pt;
{
  int i, j, k, l;
  int temp[MAXCOLOR];
  struct box *first;
  struct box *dummy;

  /* alloc memory */
  hist = (int *)calloc((unsigned)distinct,sizeof(int));
  distinct_pt = (struct rgb *)calloc((unsigned)distinct,sizeof(struct rgb));

  for (i=0; i<distinct; i++)
    hist[i] = 0;


  /* select distinct pts and set up histogram */
  for (i=0; i<MAXCOLOR; i++)
    temp[i] = -1;

  k = 0;
  for (i=0; i<(2*blocks); i++)
  {
    j = (color_pt[i].c[RED] << 10) | (color_pt[i].c[GREEN] << 5) |
        color_pt[i].c[BLUE];

    if (temp[j] == -1)
    {
      /* new pt */
      temp[j] = k;
      for (l=RED; l<=BLUE; l++)
    distinct_pt[k].c[l] = color_pt[i].c[l];
      k++;
   }

   hist[temp[j]]++;
  }

      
  /* set up first box */
  first = (struct box *)malloc(sizeof(struct box));
  for (i=RED; i<=BLUE; i++)
  {
    first->bnd[i][LO] = 999.9;
    first->bnd[i][HI] = -999.9;

    for (j=0; j<distinct; j++)
    {
      if (first->bnd[i][LO] > (float)distinct_pt[j].c[i])
    first->bnd[i][LO] = (float)distinct_pt[j].c[i];

      if (first->bnd[i][HI] < (float)distinct_pt[j].c[i])
    first->bnd[i][HI] = (float)distinct_pt[j].c[i];
    } /* end of for j */

    first->bnd[i][LO] = first->bnd[i][LO] - EPSILON;
    first->bnd[i][HI] = first->bnd[i][HI] + EPSILON;
  } /* end of for i */

  first->pts = (int *)calloc((unsigned)distinct,sizeof(int));
  for (i=0; i<distinct; i++)
    first->pts[i] = i;
  first->nmbr_pts = 2*blocks;
  first->nmbr_distinct = distinct;

  dummy = (struct box *)malloc(sizeof(struct box));
  frontier = dummy;
  dummy->right = first;
  first->left = dummy;
  first->right = NULL;
  dummy->nmbr_pts = 0;
} /* end of init */







/************************************************************************/
/*  Function    : sort                          */
/*  Purpose : Performs quick sort on the points in a box along a    */
/*        given dimension                   */
/*  Parameter   :                           */
/*    l, r   - index of leftmost and rightmost element      */
/*    dim    - dimension along which sorting is done        */
/*    rank   - an array which carries the index of the points to be */
/*         sorted                       */
/*  Returns     : none                          */
/*  Called by   : find_med()                        */
/*  Calls       : partition()                       */
/************************************************************************/

static void sort(l,r,dim, rank)
int l, r, dim, rank[];
{
  int i;
  int partition();
  void sort();

  if (r > l)
  {
    i = partition(l, r, dim, rank);
    sort(l, i-1, dim, rank);
    sort(i+1, r, dim, rank);
  }
}






/************************************************************************/
/*  Function    : partition                     */
/*  Purpose : Partitions the list into 2 parts as in the quick sort */
/*        algorithm                     */
/*  Parameter   :                           */
/*    l, r   - index of leftmost and rightmost element      */
/*    dim    - dimension along which sorting is done        */
/*    rank   - an array which carries the index of the points to be */
/*  Returns     : index where list is partitioned           */
/*  Called by   : sort()                        */
/*  Calls       : none                          */
/************************************************************************/

static int partition(l, r, dim, rank)
int l, r, dim, rank[];
{
  int i, j, temp;
  char v;
  
  v = distinct_pt[rank[r]].c[dim];
  i = l-1;
  j = r;

  /* repeat until i and j crosses */
  do
  {
    /* repeat until an element >= v is found */
    do
      i++;
    while (distinct_pt[rank[i]].c[dim] < v);

    /* repeat until an element <= v is found */
    do
      j--;
    while ((j>0) && (distinct_pt[rank[j]].c[dim] > v));

    /* swap pointers */
    temp = rank[i];
    rank[i] = rank[j];
    rank[j] = temp;
  }
  while (i < j);

  /* position partitioning element at location i */
  temp = rank[j];
  rank[j] = rank[i];
  rank[i] = rank[r];
  rank[r] = temp;

  return i;
}





/************************************************************************/
/*  Function    : find_box                      */
/*  Purpose : Finds the box with the largest number of color points */
/*        The points need not necessarily be distinct. But in   */
/*        order to partition the box, there must be at least  2 */
/*        distinct points                   */
/*  Parameter   : none                          */
/*  Returns     : pointer to box selected for splitting         */
/*  Called by   : sel_palette()                     */
/*  Calls       : none                          */
/************************************************************************/

static struct box *find_box()
{
  struct box *temp;
  struct box *max;
  int max_pts;

  max_pts = 1;
  max = NULL;
  temp = frontier->right;
  while (temp != NULL)
    if ((temp->nmbr_distinct > 1) && (max_pts < temp->nmbr_pts))
    {
      max_pts = temp->nmbr_pts;
      max = temp;
      temp = temp->right;
    }
    else
      temp = temp->right;

  if (max == NULL)
  {
    printf("Error : Number of color points < palette \n");
    exit(1);
  }

  return max;
}





/************************************************************************/
/*  Function    : split_box                     */
/*  Purpose : Splits a selected box into 2 and reinserts the 2 sub- */
/*        boxes into the frontier list              */
/*  Parameter   :                           */
/*    ptr    - pointer to box to be split               */
/*  Returns     : none                          */
/*  Called by   : sel_palette()                     */
/*  Calls       : find_med(), select_dim(), classify()          */
/************************************************************************/

static void split_box(ptr)
struct box *ptr;
{
  int dim, j, i;
  float median;
  struct box *l_child, *r_child;
  int select_dim();
  float find_med();
  void classify();
  
  dim = select_dim(ptr);
  median = find_med(ptr, dim);

  /* create 2 child */
  l_child = (struct box *)malloc(sizeof(struct box));
  r_child = (struct box *)malloc(sizeof(struct box));
  
  for (i=RED; i<=BLUE; i++)
    for (j=HI; j<=LO; j++)
    {
      l_child->bnd[i][j] = ptr->bnd[i][j];
      r_child->bnd[i][j] = ptr->bnd[i][j];
    }
  l_child->bnd[dim][HI] = median;
  r_child->bnd[dim][LO] = median;

  classify(ptr,l_child);
  classify(ptr,r_child);

  r_child->right = ptr->right;
  r_child->left = l_child;
  l_child->right = r_child;
  l_child->left = ptr->left;
  (ptr->left)->right = l_child;
  if (ptr->right != NULL)
    (ptr->right)->left = r_child;
} /* end of split_box */






/************************************************************************/
/*  Function    : assign_color                      */
/*  Purpose : Assigns a color to each box. It computes the average  */
/*        color of all the points in the box            */
/*        Sets up the new_pal buffer. Each color component is   */
/*        shifted left 3 bits because of the truncation when    */
/*        color_pt was set up                   */
/*  Parameter   : none                          */
/*  Returns     : none                          */
/*  Called by   : sel_palette()                     */
/*  Calls       : none                          */
/************************************************************************/


static void assign_color()
{
  struct box *temp;
  int ent, k, j;
  int c[3];

  temp = frontier->right;
  for (ent=0; ent<PALSIZE; ent++)
  {
    for (k=RED; k<=BLUE; k++)
      c[k] = 0;

/*
    printf("Box %d: number of pts %d\n", ent, temp->nmbr_pts);
*/

    for (j=0; j<temp->nmbr_distinct; j++)
    {
/*
      printf("pt %d:", j);
*/
      for (k=RED; k<=BLUE; k++)
      {
/*
        printf("%d ",distinct_pt[temp->pts[j]].c[k]);
*/
    c[k] = c[k] + distinct_pt[temp->pts[j]].c[k]*hist[temp->pts[j]];
      }
/*
      printf("\n");
*/
    }

    for (k=RED; k<=BLUE; k++)
    {
      c[k] = c[k] / temp->nmbr_pts;
      new_pal[3*ent+k] = c[k] << 3;
    }
    
    temp = temp->right;
  } /* end of for entry */
}





/************************************************************************/
/*  Function    : select_dim                        */
/*  Purpose : Selects the dimension with the largest spread         */
/*  Parameter   :                           */
/*    ptr    - pointer to desired box               */
/*  Returns     : dimension where the box is to be split        */
/*  Called by   : split_box()                       */
/*  Calls       : none                          */
/************************************************************************/

static int select_dim(ptr)
struct box *ptr;
{
  int i, j, low[3], high[3];
  int max;


  for (j=RED; j<=BLUE; j++)
  {
    low[j] = distinct_pt[ptr->pts[0]].c[j];
    high[j] = distinct_pt[ptr->pts[0]].c[j];
  }

  for (i=1; i<ptr->nmbr_distinct; i++)
    for (j=RED; j<=BLUE; j++)
    {
      if (low[j] > distinct_pt[ptr->pts[i]].c[j])
    low[j] = distinct_pt[ptr->pts[i]].c[j];
      if (high[j] < distinct_pt[ptr->pts[i]].c[j])
    high[j] = distinct_pt[ptr->pts[i]].c[j];
    }

  max = high[RED] - low[RED];
  i = RED;
  for (j=GREEN; j<=BLUE; j++)
    if (max < (high[j] - low[j]))
    {
      max = high[j] - low[j];
      i = j;
    }

  return i;
} /* end of select_dim */





/************************************************************************/
/*  Function    : find_med                      */
/*  Purpose : Finds the point where the box is to be split. It finds*/
/*        a point such that the 2 new boxes have about the same */
/*        number of color points.               */
/*  Parameter   :                           */
/*    ptr    - pointer to box to be split               */
/*    dim    - dimension to split box               */
/*  Returns     : point where the box is to be cut          */
/*  Called by   : split_box()                       */
/*  Calls       : next_pt()                     */
/************************************************************************/

static float find_med(ptr,dim)
struct box *ptr;
int dim;
{
  int i, j, count, next, prev;
  int *rank;
  float median;
  int next_pt();
  void sort();

  rank = (int *)calloc((unsigned)ptr->nmbr_distinct,sizeof(int));
  for (i=0; i<ptr->nmbr_distinct; i++)
    rank[i] = ptr->pts[i];

  sort(0,ptr->nmbr_distinct-1,dim,rank);
/*
  for (i=0; i<ptr->nmbr_distinct; i++)
    printf("find_med: sorted list is %d\n",distinct_pt[rank[i]].c[dim]);
*/
  

  count = 0;
  i = 0;
  while ((i < ptr->nmbr_distinct) && (count < ptr->nmbr_pts/2))
  {
    next = next_pt(dim, i, rank, ptr->nmbr_distinct);
    for (j=i; j<next; j++)
      count = count + hist[rank[j]];

    prev = i;
    i = next; 
  }

  if (prev == 0)
  {
    /* the first distinct point overshot the median */
    median = (float)distinct_pt[rank[prev]].c[dim] + EPSILON;
  }
  else
    median = (float)distinct_pt[rank[prev-1]].c[dim] + EPSILON;
   

  return median;
} /* end of find_med */
  





/************************************************************************/
/*  Function    : classify                      */
/*  Purpose : Looks at the color points in the parent and selects   */
/*        the points that belong to the child           */
/*  Parameter   :                           */
/*    ptr    - pointer to parent                    */
/*    child  - pointer to child box                 */
/*  Returns     : none                          */
/*  Called by   : split_box()                       */
/*  Calls       : none                          */
/************************************************************************/

static void classify(ptr, child)
struct box *ptr, *child;
{
  int i, j;
  int *temp;
  int distinct, total;

  temp = (int *)calloc((unsigned)ptr->nmbr_distinct,sizeof(int));

  distinct = 0;
  total = 0;
  for (i=0; i<ptr->nmbr_distinct; i++)
  {
    j = ptr->pts[i];
    if ((((float)distinct_pt[j].c[RED] >= child->bnd[RED][LO]) &&
         ((float)distinct_pt[j].c[RED] <= child->bnd[RED][HI])) &&
        (((float)distinct_pt[j].c[GREEN] >= child->bnd[GREEN][LO]) &&
         ((float)distinct_pt[j].c[GREEN] <= child->bnd[GREEN][HI])) &&
        (((float)distinct_pt[j].c[BLUE] >= child->bnd[BLUE][LO]) &&
         ((float)distinct_pt[j].c[BLUE] <= child->bnd[BLUE][HI])))
    {
      /* pt is in new box */
      temp[distinct] = j;
      distinct++;
      total = total + hist[j];
    } /* end of if */
  } /* end of for i */

  /* assign points */
  child->nmbr_pts = total;
  child->nmbr_distinct = distinct;
  child->pts = (int *)calloc((unsigned)distinct,sizeof(int));
  for (i=0; i<distinct; i++)
    child->pts[i] = temp[i];

} /* end of classify */





/************************************************************************/
/*  Function    : next_pt                       */
/*  Purpose : Determines the next point that has a different value  */
/*        from the current point along  a dimension     */
/*  Parameter   :                           */
/*    dim    - dimension where box is to be split           */
/*    i      - index to current point               */
/*    rank       - sorted list of points to be searched starting from i */
/*    distinct   - length of sorted list                                */
/*  Returns     : index of point that has a different value     */
/*  Called by   : find_med                      */
/*  Calls       : none                          */
/************************************************************************/

static int next_pt(dim, i, rank, distinct)
int dim, i, rank[], distinct;
{
  int j;
  char old;

  old = distinct_pt[rank[i]].c[dim];
  for (j=(i+1); j<distinct; j++)
    if (distinct_pt[rank[j]].c[dim] != old)
      break;

  return j;
} /* end of next_pt */

