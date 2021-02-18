#===============================================================================
#
#	Makefile for IMCORE.
#
#	description :
#		This is the primary Makefile for IMCORE.
#		This invokes the secondary Makefile, 'Make.ALL' with setting,
#		$(TARGET) and $(BUILD).
#
#===============================================================================

CC	= cc
SHELL	= /bin/sh
SECONDMAKE	= Make.ALL

#-------------------------------------------------------------------------------
#
#	Specify Compile Option.
#
#-------------------------------------------------------------------------------

COMPOPT	= -O
DEFINES = -DIMSTATICLOAD

#-------------------------------------------------------------------------------
#
#	$(MAKE) each module passing them necessary information.
#
#-------------------------------------------------------------------------------

all :
	$(MAKE) -f $(SECONDMAKE) COMPOPT=$(COMPOPT) BUILD=`pwd` TARGET=`pwd`

#-------------------------------------------------------------------------------
#
#	Utility commands.
#
#-------------------------------------------------------------------------------

clean \
cleanup \
lint :
	$(MAKE) -f $(SECONDMAKE) COMPOPT=$(COMPOPT) BUILD=`pwd` TARGET=`pwd`

