# makefile for graphic editor xGKS version

# home directory for graphics related work
PROJ = /mntm/1/kubitz/graphics

IHDIR = /andrew/Xgks/source/xgks/include
INCLUDES = -I$(IHDIR)
#CFLAGS = -g
CFLAGS = $(INCLUDES)
LFLAGS = -vxu

# matrix film recorder library
# MATRIX = $(PROJ)/matrix/lib/libmatrix

# for the loader
CLIBDIR = /andrew/Xgks/source/xgks/lib
LIBS = $(CLIBDIR)/libxgks.a -lX11 -lm  

INC = add_objects_func.h clr_selector.h \
	clr_selector_draw.h clr_selector_func.h clr_selector_key.h \
	changes.h color.h \
	conversions.h defs.h draw.h \
	func.h functbl.h key.h \
	menu.h object_defs.h object_tbl.h \
	objects.h objects_func.h palette.h \
	popup.h screen_items.h trans.h ws.h
# attributes.h	debug.h
SRC = add_objects.c add_objects_subs.c \
	clr_selector_draw.c \
	clr_selector_exec.c color.c \
	conversions.c draw.c draw_menu.c \
	draw_palette.c dummies.c edit_picture.c \
	error.c functbl.c \
	grid.c \
	init_tbls.c input.c \
	io.c main.c main_menu.c \
	menu.c object_list.c object_tbl.c \
	objects_copy.c \
	objects_detect.c objects_draw.c objects_free.c \
	objects_rotate.c \
	palette.c photodummy.c \
	picture.c popup.c prompt.c \
	read_input.c read_menu.c \
	screen_items.c trans.c trans_subs.c undo.c \
	write_output.c
# attributes.c
# photo.c
OBJ = add_objects.o add_objects_subs.o\
	clr_selector_draw.o \
	clr_selector_exec.o color.o \
	conversions.o draw.o draw_menu.o \
	draw_palette.o dummies.o edit_picture.o \
	error.o functbl.o \
	grid.o \
	init_tbls.o input.o  \
	io.o main.o main_menu.o \
	menu.o object_list.o object_tbl.o \
	objects_copy.o \
	objects_detect.o objects_draw.o objects_free.o \
	objects_rotate.o \
	palette.o photodummy.o \
	picture.o popup.o prompt.o \
	read_input.o read_menu.o \
	screen_items.o trans.o trans_subs.o undo.o \
	write_output.o
# attributes.o
# photo.o

sled: $(OBJ)
	@echo "Getting libxgks from" $(CLIBDIR)
	cc -g $(OBJ) $(LIBS) -lm  -o sled

.c.o:	;	cc $(CFLAGS) -c $<

lintinc: $(INC)
	lint $(LFLAGS) $(INC)

lint: $(SRC)
	lint $(LFLAGS) $(SRC)

depend:	
	(co -l Makefile; makedepend $(INCLUDES) *.c; ci -f -q -m"make depend" -u Makefile);

clean: 
	rm *.o core

# DO NOT DELETE THIS LINE -- make depend depends on it.

