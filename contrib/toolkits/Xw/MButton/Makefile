
OBJECTS = multiTest.o MButton.o
LIBS =  ../lib/libXw.a ../lib/libXt.a -lX11
# DEC and Sun
CFLAGS = -I../ -I./
#CFLAGS = -I../ -I./ -Wc,-Nd2000 -Wc,-Ns2000

Test: $(OBJECTS)
	cc $(CFLAGS) -o multiTest $(OBJECTS) $(LIBS)




