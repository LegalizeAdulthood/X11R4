#########################################################################
#
#  Makefile for X Image Version 1.0.3 11/29/89
#  Before make(ing), set ULIBS, INCLUDES, DEFINES, DESTDIR
#

#################################
#  HDFLIB - where is your HDF library?
#  HDFINCLUDE - where are your HDF header files (so that they can be 
#		accessed by <hdf/????.h>
#  Also libdf.a   - library containing HDF calls (hierarchical data format)
#  If you don't have HDF, you need to get it from anon ftp (128.174.20.50)

HDFLIB=../hdf/libdf.a
HDFINCLUDE=..

#################################
#  ULIBS: Where are your libXw.a, libXaw.a, libXt.a, libXmu.a, libX11.a
#  libaries?
#  libXw.a	- 2D HP widget set library.
#

ULIBS= -L/sdg/lib 

INCLUDES= -I/sdg/netsrc/X.V11R3/include -I$(HDFINCLUDE)

#################################
# DEFINES=  possible values listed below 
# for compiling under SysV use: -DSYSV
# for compiling on Cray (UNICOS) use:  -DSYSV -DUNICOS
# for displaying client on servers that have a static color visual,
# 	use -DDEF_STAT_VISUAL   Sun's OpenWindows X/NeWS server and
#	SGI's X/NeWS server set their default Visual to a static color.

DEFINES=


####################################################################
# DESTDIR - final destination of executable
#

DESTDIR = /usr/local/bin


####################################################################
#

CFLAGS= -g $(INCLUDES) $(DEFINES)
#CFLAGS= -O $(INCLUDES) $(DEFINES)


######################################################################
########## You shouldn't need to edit anything below here ############
######################################################################

PROGRAM= ximage
SOURCES= \
			a.c \
			animate.c \
			control.c \
			contour.c \
			ctlcontour.c \
			data.c \
			getfile.c \
			glob.c \
			gr_xwi.c \
			gr_xwi1.c \
			graph.c \
			i.c \
			image.c \
			intensity.c \
			main.c \
			mem.c \
			notebook.c \
			palentry.c \
			palette.c \
			polar.c \
			read.c \
			shade.c \
			sliceSDS.c \
			write.c \
			xstuff.c \
			xstuff2.c \
			xinit.c 

OBJECTS= $(SOURCES:.c=.o)

###########################################
# CURDATE&VERSION used to log compilation date & version No. in module date.c
#
CURDATE = `date '+"%D"'`
#CURDATE = `date '+"%D@%T"'`
VERSION = '"NCSA X Image Version 1.0.3"'


SLIBS= $(HDFLIB) -lXw -lXaw -lXt -lXmu -lX11 -lm 


##########################################

$(PROGRAM):  $(OBJECTS) date.o $(HDFLIB)
	rm -f $@
	$(CC) -o $@ $(CFLAGS) $(OBJECTS) date.o $(ULIBS) $(SLIBS)

$(HDFLIB):
	cd ../hdf; make

backup: $(SOURCES) 
	rm -f backup
	shar README VERSIONLOG $(SOURCES) date.c *.h ximage.icon ximage.1 Make.noImake Imakefile > backup

install: $(PROGRAM)
	install -m 755 $(PROGRAM) $(DESTDIR)
	cp ximage.1 /usr/man/man1

clean:
	rm -f $(PROGRAM) $(OBJECTS) date.o

a.o:		a.c animate.h

animate.o:	animate.c animate.h xstuff.h getfile.h

control.o:	control.c xstuff.h buttons.h gr_def.h gr_var.h

ctlcontour.o:	ctlcontour.c i.h xstuff.h

data.o:		data.c i.h xstuff.h xstuff2.h

getfile.o:	getfile.c xstuff.h getfile.h

graph.o:	graph.c i.h xstuff.h

gr_xwi.o:	gr_xwi.c gr_xwi.h

gr_xwi1.o:	gr_xwi1.c gr_com.h

i.o:		i.c i.h

image.o: 	image.c i.h buttons.h xstuff.h getfile.h

main.o:		main.c xstuff.h ximage.icon

notebook.o:	notebook.c notebook.h xstuff.h

palentry.o:	palentry.c xstuff.h palentry.h

palette.o:	palette.c buttons.h xstuff.h paletteP.h getfile.h

polar.o:	polar.c i.h xstuff.h

read.o:		read.c i.h

shade.o:	shade.c patterns.h

sliceSDS.o:	sliceSDS.c buttons.h xstuff.h

xstuff.o:	xstuff.c list.h

xstuff2.o:	xstuff2.c xstuff.h xstuff2.h

xinit.o:	xstuff.h

date.o: 	$(OBJECTS)
		cc -c $(DEFINES) -DVERSIONDATE=$(CURDATE) -DVERSION=$(VERSION) date.c
