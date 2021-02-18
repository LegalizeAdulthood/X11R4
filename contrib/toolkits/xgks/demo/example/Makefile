#
# XXXX-XXX (C) COPYRIGHT IBM CORPORATION 1988
# LICENSED MATERIALS - PROPERTY OF IBM
# REFER TO COPYRIGHT INSTRUCTIONS FORM NUMBER G120-2083
#
#
all: hello

hello: hello.c
	cc hello.c -I../../include -o hello ../../lib/libxgks.a -lX11 -lm

install:

clean:
	-rm -f hello.o hello
