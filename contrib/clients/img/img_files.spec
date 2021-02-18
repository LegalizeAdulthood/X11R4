
	Preview:  A sample color-mapped image file
	------------------------------------------

  Block

	-------------------
   1	| "SCMI" | "   1" |	<---  File Identification
	-------------------
	---------------------
   2A	| "AT" | "      50" |	<---  Section header (attributes)
	---------------------
	-----------------------------------------------------------------------
   2B	| " 512" | " 464" | " 128" | "GDA 1 222.21 (-114.5 54.8) (17.2 84.0)" |
	-----------------------------------------------------------------------
	---------------------						|
   3A	| "CM" | "     384" |	<---  Section header (color map)	|
	---------------------					   Attributes:
	-------------						width, height,
   3B	| b | b | b |  <---  Color map, color[0]		# colors,
	-------------						associated data
	| b | b | b |  <---  color[1]
	-------------
	     ...				['b' => 8-bit byte]
	-------------
	| b | b | b |  <---  color[n-1]
	-------------
	---------------------
   4A	| "PD" | "  237568" |	<---  Section header (pixel data)
	---------------------
	---------------------
   4B	| bbbbb... ...bbbbb |  <--- Pixel data
	---------------------



	Sample associated data in attributes:

	"GDA 1 222.21 (-114.5 54.8) (17.2 84.0)"
	  \   \    \         \           \
	   \   \    \         \	     Longitute/Latitude @ top right
	    \   \    \         --- Longitude/Latitude @ bottom left
	     \   \    ---  Scale, pixels per radian of longitude
	      \   --- Version # of application data format
	       ---  Application identification




			"Img" Image File Formats


	This is the beginning of a larger specification document
	for an overall subsystem to handle simple image processing.
	The subsystem is tentatively named "Img".  This document
	includes only these specification sections:


	1  Img File Formats

	1.1  Color Mapped Images
	1.2  24-bit RGB Images


	Additional sections have not been written, but would describe
	the imglib routines and the set of programs which manipulate
	images in this format.  Most of this information can be extracted
	from source modules by using the ccomex comment extractor.

	As with the file format, imglib functions were designed
	to make the interface from application code as simple as possible.
	Most of imglib is independent of the underlying graphics or
	window system.  One module supplies such system-dependent
	support for X11R3.



1  Img File Formats

1.1  Color Mapped Images

	Purposes of this particular file format are:

	1.  Represent color-mapped images which use no more than 256 colors.
	2.  Conserve disk space.
	3.  Conserve processing time for reading and displaying images.
	4.  Minimize complexity in basic image manipulation functions.
	5.  Allow extensibility in future versions.


	The format defined here represents an uncompressed image file.
	Such images would normally be stored in compressed format, as
	used by the UNIX* compress/uncompress utility.  (* UNIX is
	a trademark of AT&T.)  When this is true, the filename ends
	with ".Z".

	    One simple way to read compressed files is to pipe input
	    through uncompress, approximately as follows:

		if ( compressed )
		   {
		   sprintf ( command, "uncompress <%s", filename );
		   img_file = popen ( command,  "r" );
		   }
		else
		   img_file = fopen ( filename, "r" );

	In addition to easy availability of compress/uncompress,
	use of this particular compress tool set generally produces
	better compression than run length encoding.  It also improves
	on storage of bitmaps:  Using one bit per byte in compressed
	format usually produces better compression than packing 8 bits
	per byte.



	Some design rules applied to all parts of this file format
	are:

	1.  All data is a byte stream:  The format has no dependence
	    on byte or word order for any particular machine.

	2.  Pixel data, which are indices to the color map, and RGB
	    component values are 8-bit unsigned binary values.

	3.  Attribute data, except RGB values in the color map, are
	    stored in fixed-size ascii fields.  Integers use either
	    4-byte or 8-byte fields; programs can read or write them
	    via scanf/printf functions using, for example, "%4u" or
	    "%4i" format.

	4.  Whenever the length of a section of the file may vary
	    depending on image contents, that section is preceded
	    by a prefix which identifies it and gives its length
	    in bytes.  This facilitates use of block-oriented i/o
	    operations to optimize i/o speed; it does this in a
	    way which allows future extension of the image format
	    to include sections whose size does not depend on data
	    in the attribute section.



1.1.1	Top Level Structure

	A color-mapped image file contains the following blocks:

	1.   File identification
	2A.  Attribute prefix
	2B.  Attributes
	3A.  Color map prefix
	3B.  Color map
	4A.  Pixel data prefix
	4B.  Pixel data



1.1.2  Section Prefixes

	Each "prefix" block identifies a section and report reports
	its size.  Format:

		  +0	  +2
		-----------------
		| xx | nnnnnnnn |
		-----------------
		   \	   \___ Length of data in next section:  %8d or %8u
		    \
		     \___ Section identifier:	AT  =>  Attributes
						CM  =>  Color Map
						PD  =>  Pixel Data

	Other prefixes could be used to represent data sections
	in formats defined outside this specification.



1.1.2  File Identification

	This identifies the file type and the version number of the
	file format.

		   +0	  +4
		---------------
		| SCMI | nnnn |
		---------------
		    \	   \___ File format version #:  %4d or %4u
		     \
		      \___ File type code:  "SCMI"



1.1.3  Attributes

	The "attributes" section defines image characteristics other
	than the content of its color map.  This consists of the image
	width and height, number of colors, and associated data for
	application use.  For purposes of this file format, associated
	data is only a sequence of bytes.

	An appropriate convention for higher level specifications
	would be to begin any associated data with an application code
	and version number to identify its format.

		-----------------
		| AT | nnnnnnnn |  <---  Section header
		-----------------

		   +0	  +4	 +8    +12
		---------------------------------------------
		| nnnn | nnnn | nnnn | [... assoc data ...] |
		---------------------------------------------
		    \	   \	  \		\__Associated data, if any
		     \	    \	   \__Number of colors:  %4d or %4u
		      \	     \__Height, pixels:  %4d or %4u
		       \__Width%4d or %4u


		Size of associated data is block size - 12 bytes.
		(Block size from section header)


1.1.4  Color Map

	The color map is an array with one entry for each color
	in the image.  For grey-scale images, each RGB component
	is identical.  Black-and-white images have a two-entry
	color table.


		-----------------
		| CM | nnnnnnnn |  <---  Section header
		-----------------

		  Red  Grn Blue
		   /   /   /
		-------------
		| b | b | b |  <---  color[0]	@ +0
		-------------
		| b | b | b |  <---  color[1]	@ +3
		-------------
		     ...
		-------------
		| b | b | b |  <---  color[k]	@ +3*k
		-------------
		     ...
		-------------
		| b | b | b |  <---  color[n-1]	@ +3*(n-1)
		-------------


	Each color component byte (b) is an unsigned 8-bit integer.

	Block size is 3*n, where n is the number of colors defined
	in the attributes.


1.1.5  Pixel Data

	Pixel data is an array with one 8-bit entry for each pixel.
	Pixels are in conventional scan line order.


		-----------------
		| CM | nnnnnnnn |  <---  Section header
		-----------------

		---------------------
		| bbbbb... ...bbbbb |  <--- Pixel data
		---------------------

	Each pixel value byte (b) is an 8-bit unsigned integer which
	is the index of a color map entry.  Valid range is [0,n-1],
	where n is the number of colors defined in the attributes.

	Size of the pixel data section is width*height bytes.

	Order of pixels is row major -- Top to bottom by rows,
	left to right within each row.



1.2  24-bit RGB Images

	Purposes of this particular file format are:

	1.  Represent images using 24 bits per pixel:  8 bits each
	    for red, green, and blue values.
	2.  Conserve disk space.
	3.  Maintain reasonable consistency with existing techniques
	    for acquiring RGB image data
	4.  Minimize complexity in basic image manipulation function.
	5.  Allow using an ordinary text editor to define image attributes.


	As with color mapped images, 24-bit RGB images can optionally
	be stored in compressed format.  Any file which is compressed
	has ".Z" appended to its filename.


1.2.1	Top Level Structure

	A 24-bit RGB image is defined by four separate files which
	are identified by file naming conventions:

	1.  Attributes:		<name>.a[.Z]
	2.  Red component:	<name>.r[.Z]
	3.  Green component:	<name>.g[.Z]
	4.  Blue component:	<name>.b[.Z]

	For any given image, the various component files may be
	either compressed or uncompressed on an individual basis.


1.2.2  Attribute File

	The attribute file contains precisely the data which would
	appear in the attribute section (section 2B) of a color-mapped
	image file, except that the color count is meaningless:

		   +0	  +4	 +8    +12
		---------------------------------------------
		| nnnn | nnnn | xxxx | [... assoc data ...] |
		---------------------------------------------
		    \	   \	  \		\__Associated data, if any
		     \	    \	   \__Reserved for color count, 4 chars
		      \	     \__Height, pixels:  %4d or %4u
		       \__Width%4d or %4u


	Size of associated data is the file size - 12.

	Although attribute files can be stored in compressed format,
	keeping them in easily editable uncompressed format can
	simplify generating and maintaining image data bases.


1.2.3  RGB Component Files

	Each RGB component file contains 8-bit unsigned pixel values
	for the red, green, or blue component of the image.

	Each file represents width*height bytes, where width and height
	are defined in the corresponding attribute file.

	Order of pixels is row major -- Top to bottom by rows,
	left to right within each row.
