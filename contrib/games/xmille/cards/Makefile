.PRECIOUS: cardlib.a
OFILES=	accident.o repairs.o ace.o\
	flat.o spare.o puncture.o\
	out.o gas.o extra.o\
 	stop.o go.o speed.o end.o right.o\
	25.o 50.o 75.o 100.o 200.o\
 	deck.o blank.o

cardlib.a: $(OFILES)
#	ar cr $@ $?
#	ranlib $@

display: display.o cardlib.a
	$(CC) -o display display.o cardlib.a -lX

clean:
	rm -f $(OFILES) cardlib.a

accident.o: accident accident_mask
	cc -c accident.c
repairs.o: repairs repairs_mask
	cc -c repairs.c
ace.o: ace ace_mask
	cc -c ace.c

flat.o: _flat flat_label flat_mask
	cc -c flat.c
spare.o: spare spare_mask
	cc -c spare.c
puncture.o: puncture puncture_mask
	cc -c puncture.c

out.o: _out out_label out_mask
	cc -c out.c
gas.o: gas gas_mask
	cc -c gas.c
extra.o: extra extra_mask
	cc -c extra.c

stop.o: stop stop_mask
	cc -c stop.c
go.o: go go_mask
	cc -c go.c
speed.o: speed speed_mask
	cc -c speed.c
end.o: end end_mask
	cc -c end.c
right.o: right right_mask
	cc -c right.c

25.o: _25 miles_mask
	cc -c 25.c
50.o: _50
	cc -c 50.c
75.o: _75
	cc -c 75.c
100.o: _100
	cc -c 100.c
200.o: _200
	cc -c 200.c

deck.o: deck_mask deck_red deck_blue deck_both
	cc -c deck.c

blank.o: blank
	cc -c blank.c
