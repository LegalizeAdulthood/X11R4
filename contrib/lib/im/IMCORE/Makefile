#===============================================================================
#
#	Makefile for the IMCORE
#
#	Desctiption : This is the Makefile for IMCORE.
#		This Makefile creates a library file, 'libIM.a', and stores it
#		in to the $(TARGET)/$(USRLIB) directory which is usually
#		../usr/lib directory.
#		'libIM.a' contains three object files, imcore.o, imkeybind.o
#		and imutil.o. These object files are created by the source
#		files in the ./source directory.
#		This Makefile is invoked by another Makefile in the parent
#		directory (parent Makefile). The parent Makefile MUST set
#		two parameters, $(TARGET) and $(BUILD), these are the top of
#		the target directory and the top of the source directory,
#		respectively. So you shouldn't set these two parameters in this
#		Makefile (though as default, they are actually set to relative
#		paths in some lines later in this Makefile. These are
#		overridden by the parent Makefile).
#
#		Usually, only the portion you may need to modify in this
#		Makefile is compiler option settings. $(INCLUDE), $(COMPOPT)
#		$(OPTIONS) and $(DEFINES).
#
#===============================================================================

#-------------------------------------------------------------------------------
#
#	The following are usually constants.
#
#-------------------------------------------------------------------------------

CC	= cc
SHELL	= /bin/sh

#-------------------------------------------------------------------------------
#
#	$(HERE) is the name of THIS directory.  See below for the description
#		of $(BUILD).
#
#-------------------------------------------------------------------------------

HERE	= IMCORE

#-------------------------------------------------------------------------------
#
#	The following are set (overridden) by the parent Makefile.
#
#	$(TARGET) is the ROOT directory of the target. For example, a module
#		may be stored into the directory, $(TARGET)/usr/bin/module.
#		$(TARGET) must be the absolute path yet it can be NULL.
#	$(BUILD) is the TOP directory of build source hierachy. This must be
#		the absolute path.  This directory ( where this Makefile
#		resides ) is represented as $(BUILD)/$(HERE)
#
#-------------------------------------------------------------------------------

TARGET	= ..
BUILD	= ..

#-------------------------------------------------------------------------------
#
#	$(USRLIB) is a directory into which the library is stored.
#		The library will be stored in $(TARGET)/$(USRLIB).
#
#-------------------------------------------------------------------------------

USRLIB	= usr/lib

#-------------------------------------------------------------------------------
#
#	This directory ( $(HERE) ) may contain the following sub-directories.
#	$(INCDIR)	: Include directory. This must exist.
#	$(SRCDIR)	: Source directory. This must exist.
#	$(OBJDIR)	: Directory into which objects are stored.
#			  Usually, this is same to $(SRCDIR).
#
#-------------------------------------------------------------------------------

INCDIR	= $(BUILD)/$(HERE)/include
SRCDIR	= $(BUILD)/$(HERE)/source
OBJDIR	= $(BUILD)/$(HERE)/source

#-------------------------------------------------------------------------------
#
#	$(INCLUDE)	: Additional include directory needed for compiling.
#	$(COMPOPT)	: Compile options (changable), e.g. -g,....
#	$(OPTIONS)	: Compile options (not changable), e.g., -Nn, -Nd,....
#	$(DEFINES)	: Define flags, e.g., -D,....
#
#-------------------------------------------------------------------------------

INCLUDE	= -I$(INCDIR) -I/usr/include/X11
COMPOPT	= -O
OPTIONS	= -Nn5000 -Nd5000
DEFINES	=
CFLAGS	= $(INCLUDE) $(COMPOPT) $(OPTIONS) $(DEFINES)

#-------------------------------------------------------------------------------
#
#	Source and object files. You shouldn't touch.
#
#-------------------------------------------------------------------------------

SOURCES	= $(SRCDIR)/imcore.c    \
	  $(SRCDIR)/imkeybind.c \
	  $(SRCDIR)/imload.c \
	  $(SRCDIR)/imutil.c

OBJECTS	= $(OBJDIR)/imcore.o    \
	  $(OBJDIR)/imkeybind.o \
	  $(OBJDIR)/imload.o \
	  $(OBJDIR)/imutil.o

HEADERS = $(INCDIR)/im.h \
	  $(INCDIR)/imerrno.h \
	  $(INCDIR)/imlang.h \
	  $(INCDIR)/imkeybind.h \
	  $(INCDIR)/AIXkeymap.h \
	  $(SRCDIR)/imloadable.c

LIBRARY = $(TARGET)/$(USRLIB)/libIM.a

#-------------------------------------------------------------------------------
#
#	Here starts the dependency lines.
#
#-------------------------------------------------------------------------------

default	:	all

all	:	$(LIBRARY)

$(LIBRARY) : $(OBJECTS)
	@if [ -f $@ ]; \
	then \
		echo "adding objects to library $(LIBRARY)."; \
		ar rv $@ $?; \
	else \
		echo "creating library $(LIBRARY)."; \
		lorder $(OBJECTS) | tsort | xargs ar qv $@; \
	fi

$(OBJECTS) : $(HEADERS)
	$(CC) $(CFLAGS) -c $<
	@mv ./$(@F) $@

#-------------------------------------------------------------------------------
#
#	Utility commands.
#
#-------------------------------------------------------------------------------

cleanup \
clean	:
	-rm -f $(OBJECTS)

lint	:
	lint $(SOURCES) $(INCLUDE) $(OPTIONS) $(DEFINES)

