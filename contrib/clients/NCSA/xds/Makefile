#
#	NCSA XDataSlice source level Makefile
# *	Note that to view files correctly, set your tabstops to 4 spaces. *
#	Commands defined:
#	1.	make
#		Create a working version of NCSA X DataSlice in the current directory
#	2.	make clean
#		Clean all .o and unnecessary files.
#

PROGRAM	= xds
NCSADIR = ..
CURDATE	= `date '+"%a,%D@%T"'`
CC		= cc

#
# xds directories
#
XDSDIR	= ${NCSADIR}/xds
XDSSTD	= ${XDSDIR}/installed
XDSHDF	= ../hdf

#
# X lib directories
#
XDIR	= /usr
XINC	= ${XDIR}/include
XLIB	= ${XDIR}/lib

#
# xds libraries
#
HDFLIB	= ${XDSHDF}/libdf.a
MIRLIB	= ${XDSMIR}/libmirth.a

#
# Compile flags
#
# When compiling with SUN's OPEN WINDOWS, uncomment the following line
# OWFLAGS =   -DOPEN_WINDOWS
# When compiling with any SYS V systems, uncomment the following line
# SYFLAGS   = -DSYSV
#
# When compiling on a Sun3, uncomment the following line
# MAFLAGS   = -DSUN3 -fswitch
# When compiling on a Sun386i, uncomment the following line
# MAFLAGS   = -DSUN386
# When compiling on a Tektroniks XD88, uncomment the following line
# UINC      = -I/usr/include/bsd
# MAFLAGS   = -DTEKXD88
# When compiling on an Iris, uncomment the following 3 lines
# UINC      = -I/usr/include/bsd
# MAFLAGS   = -DIRIS
# LMFLAGS   = -lbsd
# When compiling on IBM Aix, uncomment the following 2 lines
# MAFLAGS   = -Nn4000 -Nd4000 -DAIX -DBSD_INCLUDES
# LMFLAGS   = -lbsd
# When compiling on the Cray Unicos 5.0, uncomment the following 2 lines
# MAFLAGS   = -DUNICOS
# LMFLAGS   = -lnet

# If the toggles are clipped, as will happen on certain clients such as 
# the HP, Cray2 and Sun386i, uncomment the following line 
# XFLAGS    = -DADJUST_TOGGLES
 
#############################################################
########## DO NOT MAKE ANY CHANGES BELOW THIS LINE ##########
#############################################################

# Do not uncomment the following unless you have the Miriad file format libs
# FMFLAGS = -DMIRIADFMT

LFLAGS  = ${HDFLIB} -L${XLIB} -lXaw -lXt -lXmu -lXw -lX11 -lm ${LMFLAGS}
L1FLAGS = ${HDFLIB} ${MIRLIB} -L${XLIB} -lXaw -lXt -lXmu -lXw -lX11 -lm ${LMFLAGS}
CFLAGS  = -O -I${XINC} -I${NCSADIR} ${UINC}
DFLAGS  = ${MAFLAGS} ${SYFLAGS} ${OWFLAGS} ${XFLAGS} ${FMFLAGS}

HDRS	= 	gr_com.h \
			gr_def.h \
			gr_ext.h \
			gr_inc.h \
			gr_var.h \
			td_com.h \
			td_def.h \
			td_inc.h \
			td_var.h
SRCS	= 	gr_ani.c \
			gr_anilev.c \
			gr_arb.c \
			gr_arblev.c \
			gr_axes.c \
			gr_boss.c \
			gr_boslev.c \
			gr_box1.c \
			gr_box2.c \
			gr_cube.c \
			gr_cublev.c \
			gr_dice1.c \
			gr_dice2.c \
			gr_dicelev.c \
			gr_dsplev.c \
			gr_dspl.c \
			gr_file.c \
			gr_fillev.c \
			gr_hist.c \
			gr_hislev.c \
			gr_image.c \
			gr_isolev.c \
			gr_iso1.c \
			gr_iso2.c \
			gr_main.c \
			gr_pal.c \
			gr_proc.c \
			gr_text.c \
			gr_subs.c \
			gr_subslev.c \
			gr_tile.c \
			gr_tillev.c \
			gr_toplev.c \
			gr_vbuff.c \
			gr_view.c \
			gr_viewlev.c \
			gr_xwi.c \
			gr_xwi1.c \
			gr_xwi2.c \
			gr_xwi3.c \
			td_axes.c \
			td_hdf.c \
			td_main.c \
			td_mem.c \
			td_miriad.c \
			td_pixel.c \
			td_port.c
OBJS	= 	td_axes.o \
			td_hdf.o \
			td_main.o \
			td_mem.o \
			td_miriad.o \
			td_pixel.o \
			td_port.o \
			gr_ani.o \
			gr_anilev.o \
			gr_arb.o \
			gr_arblev.o \
			gr_axes.o \
			gr_boss.o \
			gr_boslev.o \
			gr_box1.o \
			gr_box2.o \
			gr_cube.o \
			gr_cublev.o \
			gr_dice1.o \
			gr_dice2.o \
			gr_dicelev.o \
			gr_dsplev.o \
			gr_dspl.o \
			gr_file.o \
			gr_fillev.o \
			gr_hist.o \
			gr_hislev.o \
			gr_image.o \
			gr_isolev.o \
			gr_iso1.o \
			gr_iso2.o \
			gr_main.o \
			gr_pal.o \
			gr_proc.o \
			gr_text.o \
			gr_subs.o \
			gr_subslev.o \
			gr_tile.o \
			gr_tillev.o \
			gr_toplev.o \
			gr_vbuff.o \
			gr_view.o \
			gr_viewlev.o \
			gr_xwi.o \
			gr_xwi1.o \
			gr_xwi2.o \
			gr_xwi3.o

# Generate normal XDS
${PROGRAM}:  td_file.o ${OBJS} ${HDFLIB}
	rm -f ${PROGRAM}
	${CC} ${CFLAGS} -o $@ td_file.o ${OBJS} ${LFLAGS}

${HDFLIB} :
	@echo "Creating HDF library..."
	cd ${XDSHDF}; make libdf.a
	@echo "Created HDF library."

${MIRLIB} :
	@echo "Creating MIRIAD library."
	cd ${MIRLIB}; make libmirth.a
	@echo "Created MIRIAD library."

.c.o	:
	rm -f $*.o
	${CC} ${CFLAGS} ${DFLAGS} -c $*.c

clean:
	rm -f *.o xds xdsLog

#
# Define dependencies
#

td_file.o	: $(OBJS)
	rm -f td_file.o
	${CC} ${CFLAGS} ${DFLAGS} -DCOMPILEDATE=$(CURDATE) -c td_file.c
gr_ani.o	: gr_ani.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_anilev.o	: gr_anilev.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_arb.o	: gr_arb.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_arblev.o	: gr_arblev.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_axes.o	: gr_axes.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_boss.o	: gr_boss.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_boslev.o	: gr_boslev.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_box1.o	: gr_box1.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_box2.o	: gr_box2.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_cube.o	: gr_cube.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_cublev.o	: gr_cublev.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_dice1.o	: gr_dice1.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_dice2.o	: gr_dice2.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_dicelev.o: gr_dicelev.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_dspl.o	: gr_dspl.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_dsplev.o	: gr_dsplev.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_file.o	: gr_file.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_fillev.o	: gr_fillev.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_hist.o	: gr_hist.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_hislev.o	: gr_hislev.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_image.o	: gr_image.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_iso1.o	: gr_iso1.c gr_iso.h gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_iso2.o	: gr_iso2.c gr_iso.h gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_isolev.o	: gr_isolev.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_main.o	: gr_main.c gr_xwi.h gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_pal.o	: gr_pal.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_proc.o	: gr_proc.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_text.o	: gr_text.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_subs.o	: gr_subs.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_subslev.o: gr_subslev.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_tile.o	: gr_tile.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_tillev.o	: gr_tillev.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_toplev.o	: gr_toplev.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_vbuff.o	: gr_vbuff.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_view.o	: gr_view.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_viewlev.o: gr_viewlev.c gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_xwi.o	: gr_xwi.c gr_xwi.h gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_xwi1.o	: gr_xwi1.c gr_xwi.h gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_xwi2.o	: gr_xwi2.c gr_xwi.h gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
gr_xwi3.o	: gr_xwi3.c gr_xwi.h gr_def.h gr_ext.h gr_inc.h gr_var.h xds.h
td_axes.o	: td_axes.c td_def.h td_inc.h td_var.h xds.h
td_file.o	: td_file.c td_def.h td_inc.h td_var.h xds.h
td_hdf.o	: td_hdf.c td_def.h td_inc.h td_var.h xds.h
td_main.o	: td_main.c td_def.h td_inc.h td_var.h xds.h
td_mem.o	: td_mem.c td_def.h td_inc.h td_var.h xds.h
td_miriad.o	: td_miriad.c td_def.h td_inc.h td_var.h xds.h
td_pixel.o	: td_pixel.c td_def.h td_inc.h td_var.h xds.h
td_port.o	: td_port.c td_def.h td_inc.h td_var.h xds.h
