#
# XXXX-XXX (C) COPYRIGHT IBM CORPORATION 1988
# LICENSED MATERIALS - PROPERTY OF IBM
# REFER TO COPYRIGHT INSTRUCTIONS FORM NUMBER G120-2083
#
# standard makefile for use with U. of I. GKS
#

CFLAGS = -O -I/usr/include/local -I../../include

# gks libraries

LIBS = -lgks -lgksrpc -lrpc -lm

# yourprog depends on its .o file and the gks libraries
# yourprog.o depends on yourprog.c, but make "knows" that.

red: red.o 
	$(CC) $(CFLAGS) -o $@ $@.o $(LIBS)

blue: blue.o 
	$(CC) $(CFLAGS) -o $@ $@.o $(LIBS)

