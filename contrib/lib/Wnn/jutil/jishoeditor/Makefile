CFLAGS = -g -DSUN
INCLUDE = -I/usr/include/gsb -I../../include -I..

.c.o:
	$(CC) -Qpath /usr/local/lib/gmw/ $(CFLAGS) $(INCLUDE) -c $< 

all: 
	rm -f jishoeditor
	make jishoeditor

stub: jishoeditor_s.c panel_c.c jishoeditor1.c

jishoeditor_s.c: jishoeditor.stub
	sstub jishoeditor.stub

panel_c.c: panel.stub
	cstub panel.stub

jishoeditor: jishoeditor.o jishoeditor_s.o panel_c.o
	$(CC) $(INCLUDE) -o jishoeditor jishoeditor.o jishoeditor_s.o\
		  panel_c.o \
		/usr/lib/libgsb.a \
		 ../bdic.o ../sstrings.o\
		../gethinsi.o ../pwd.o ../dic_head.o \
		../dic_atojis.o
