#define	FTYPE_PAL	1
#define	FTYPE_IMAGE	2

struct GetFile {
/******* Passed to GetAFile ************/
	char	type;		/* What type of file to get */
	void	(*yesfile)();	/* procedure to call when done */
	void	(*nofile)();	/* procededure to call if cancelled */
	char	*param;		/* parameter to pass back */

/******* Returned values ***************/
	char	*directory;	/* directory of file */
	char	*base;		/* unexpanded base name */
	char	**file;		/* list of files */
	int	numFiles;
	char	isHDF;		/* is it an HDF file */
	char	isFloat;
	int	xdim;
	int	ydim;
	};
