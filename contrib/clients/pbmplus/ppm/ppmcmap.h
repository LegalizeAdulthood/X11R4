/* ppmcmap.h - header file for colormap routines in libppm
*/

/* Color histogram stuff. */

typedef struct colorhist_item *colorhist_vector;
struct colorhist_item
    {
    pixel color;
    int value;
    };

typedef struct colorhist_list_item *colorhist_list;
struct colorhist_list_item
    {
    struct colorhist_item ch;
    colorhist_list next;
    };

colorhist_vector ppm_computecolorhist( /* pixel **pixels, int cols, int rows, int maxcolors, int *colorsP */ );
/* (Returns a colorhist *colorsP long (with space allocated for maxcolors). */

void ppm_addtocolorhist( /* colorhist_vector chv, int *colorsP, int maxcolors, pixel color, int value, int position */ );

void ppm_freecolorhist( /* colorhist_vector chv */ );


/* Color hash table stuff. */

typedef colorhist_list *colorhash_table;

colorhash_table ppm_computecolorhash( /* pixel **pixels, int cols, int rows, int maxcolors, int *colorsP */ );

int
ppm_lookupcolor( /* colorhash_table cht, pixel color */ );

colorhist_vector ppm_colorhashtocolorhist( /* colorhash_table cht, int maxcolors */ );
colorhash_table ppm_colorhisttocolorhash( /* colorhist_vector chv, int colors */ );

void ppm_addtocolorhash( /* colorhash_table cht, pixel color, int value */ );

colorhash_table ppm_alloccolorhash( /* */ );

void ppm_freecolorhash( /* colorhash_table cht */ );
