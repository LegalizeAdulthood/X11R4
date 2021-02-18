OFILES=	accident.o repairs.o ace.o\
	flat.o spare.o puncture.o\
	out.o gas.o extra.o\
 	stop.o go.o speed.o end.o right.o\
	25.o 50.o 75.o 100.o 200.o\
 	deck.o blank.o

cardlib.a: $(OFILES)
	ar cr $@ $?
	ranlib $@

display: display.o cardlib.a
	$(CC) -o display display.o cardlib.a -lX

clean:
	rm -f $(OFILES) cardlib.a

accident.o: _accident accident_mask
repairs.o: _repairs repairs_mask
ace.o: _ace ace_mask

flat.o: _flat flat_mask
spare.o: _spare spare_mask
puncture.o: _puncture puncture_mask

out.o: _out out_mask
gas.o: _gas gas_mask
extra.o: _extra extra_mask

stop.o: _stop stop_mask
go.o: _go go_mask
speed.o: _speed speed_mask
end.o: _end end_mask
right.o: _right right_mask

25.o: _25 miles_mask
50.o: _50
75.o: _75
100.o: _100
200.o: _200

deck.o: deck_mask deck_red deck_blue

blank.o: _blank
