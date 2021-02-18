/* rast.h - header file for Sun raster files
*/

#ifndef _RAST_H_
#define _RAST_H_

#define PIX_ERR		-1

struct rasterfile {
    int ras_magic;
#define	RAS_MAGIC	0x59a66a95
    int ras_width;
    int ras_height;
    int ras_depth;
    int ras_length;
    int ras_type;
#define RT_OLD		0
#define RT_STANDARD	1
#define RT_BYTE_ENCODED	2
#define RT_EXPERIMENTAL 0xffff
    int ras_maptype;
#define RMT_NONE	0
#define RMT_EQUAL_RGB	1
#define RMT_RAW		2
    int ras_maplength;
    };

struct pixrectops {
    int	(*pro_rop)();
    int	(*pro_stencil)();
    int	(*pro_batchrop)();
    int	(*pro_nop)();
    int	(*pro_destroy)();
    int	(*pro_get)();
    int	(*pro_put)();
    int	(*pro_vector)();
    struct pixrect *(*pro_region)();
    int	(*pro_putcolormap)();
    int	(*pro_getcolormap)();
    int	(*pro_putattributes)();
    int	(*pro_getattributes)();
    };

struct pr_size {
    int x, y;
    };
struct pr_pos {
    int x, y;
    };

struct pixrect {
    struct pixrectops *pr_ops;
    struct pr_size pr_size;
    int pr_depth;
    struct mpr_data *pr_data;	/* work-alike only handles memory pixrects */
    };

struct mpr_data {
    int md_linebytes;
    unsigned char *md_image;	/* note, byte not short -- avoid pr_flip() */
    struct pr_pos md_offset;
    short md_primary;
    short md_flags;
    };

typedef struct {
    int type;
    int length;
    unsigned char *map[3];
    } colormap_t;

/* And the routine definitions. */

struct pixrect *mem_create( /* int w, int h, int depth */ );
void mem_free( /* struct pixrect *p */ );

int pr_dump( /* struct pixrect *p, FILE *out, colormap_t *colormap, int type, int copy_flag */ );

int pr_load_header( /* FILE *in, struct rasterfile *hP */ );

int pr_load_colormap( /* FILE *in, struct rasterfile *hP, colormap_t *colormap */ );

struct pixrect *pr_load_image( /* FILE *in, struct rasterfile *hP, colormap_t *colormap */ );

#endif /*_RAST_H_*/
