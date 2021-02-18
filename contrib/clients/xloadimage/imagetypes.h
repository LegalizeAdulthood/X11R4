/* imagetypes.h:
 *
 * supported image types and the imagetypes array declaration.  when you
 * add a new image type, only the makefile and this header need to be
 * changed.
 *
 * jim frost 10.15.89
 */

Image *facesLoad();
Image *pbmLoad();
Image *sunRasterLoad();
Image *xbitmapLoad();
Image *xpixmapLoad();

int facesIdent();
int pbmIdent();
int sunRasterIdent();
int xbitmapIdent();
int xpixmapIdent();

struct {
  int    (*identifier)(); /* print out image info if this kind of image */
  Image *(*loader)();     /* load image if this kind of image */
  char  *name;            /* name of this image format */
} ImageTypes[] = {
  sunRasterIdent, sunRasterLoad, "Sun Rasterfile",
  pbmIdent,       pbmLoad,       "Portable Bit Map (PBM)",
  facesIdent,     facesLoad,     "Faces Project",
  xbitmapIdent,   xbitmapLoad,   "X Bitmap",
  NULL,           NULL,          NULL
};
