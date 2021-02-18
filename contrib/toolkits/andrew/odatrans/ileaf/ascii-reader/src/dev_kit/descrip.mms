!***************************************************************!
!	      Copyright (C) 1989 by Interleaf, Inc.		!
!	      Ten Canal Park, Cambridge, Mass. 02141		!
!	See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT	!
!***************************************************************!

! Builds the components of the ASCII Reader Developer's Toolkit:
!    --	A version of the ASCII Reader library suitable for export
!	(that is, a version which doesn't incorporate Leafisms
!	like "cdefs.h", INTERLEAF_INCLUDE_FILES, and /G_FLOAT).
!    --	Copies of "public" source files

CFLAGS = /NOLIST/OPTIMIZE		! CC options

.C.OBJ
	$(CC) /OBJECT=D_USER:$(MMS$TARGET) $(CDEFINES) $(CFLAGS) -
	    $(MMS$SOURCE)
	@- PURGE D_USER:[]$(MMS$TARGET)

.DEFAULT
	!% Don't know how to make $(MMS$TARGET)

PUBLIC_SOURCE = ar5defs.h, ar5csave.h, ar5test.c

OBJECT_FILES1 = ar5autonum.obj, ar5buf.obj, ar5cmpn.obj, ar5csave.obj, -
	ar5cvt.obj, ar5docread.obj, ar5dump.obj, ar5dumpDg.obj, -
	ar5error.obj, ar5font.obj, ar5frameread.obj, ar5free.obj, -
	ar5freeDg.obj, ar5index.obj, ar5init.obj, ar5io.obj, ar5lex.obj
OBJECT_FILES2 = ar5loadDg.obj, ar5memory.obj, ar5printf.obj, ar5read.obj, -
	ar5ref.obj, ar5shrutils.obj, ar5strings.obj, ar5stubs.obj, -
	ar5table.obj, ar5token.obj, ar5unknowns.obj


COMPILE_ALL : $(PUBLIC_SOURCE) LIBAR5.OLB
	@ ! No actions

LIBAR5.OLB : $(OBJECT_FILES1) $(OBJECT_FILES2)
	LIBRARY/CREATE D_USER:[]$(MMS$TARGET) $(OBJECT_FILES1)
	LIBRARY/INSERT D_USER:[]$(MMS$TARGET) $(OBJECT_FILES2)
	- PURGE/LOG D_USER:[]$(MMS$TARGET)

ar5defs.h	: [-]ar5defs.h
	COPY $(MMS$SOURCE) D_USER:[]$(MMS$TARGET)
	- PURGE/LOG D_USER:[]$(MMS$TARGET)

ar5csave.h	: [-]ar5csave.h
	COPY $(MMS$SOURCE) D_USER:[]$(MMS$TARGET)
	- PURGE/LOG D_USER:[]$(MMS$TARGET)

ar5test.c	: [-]ar5test.c
	COPY $(MMS$SOURCE) D_USER:[]$(MMS$TARGET)
	- PURGE/LOG D_USER:[]$(MMS$TARGET)


ar5autonum.obj	: [-]ar5autonum.c

ar5buf.obj	: [-]ar5buf.c

ar5cmpn.obj	: [-]ar5cmpn.c

ar5csave.obj	: [-]ar5csave.c

ar5cvt.obj	: [-]ar5cvt.c

ar5docread.obj	: [-]ar5docread.c

ar5dump.obj	: [-]ar5dump.c

ar5dumpDg.obj	: [-]ar5dumpDg.c

ar5error.obj	: [-]ar5error.c

ar5font.obj	: [-]ar5font.c

ar5frameread.obj : [-]ar5frameread.c

ar5free.obj	: [-]ar5free.c

ar5freeDg.obj	: [-]ar5freeDg.c

ar5index.obj	: [-]ar5index.c

ar5init.obj	: [-]ar5init.c

ar5io.obj	: [-]ar5io.c

ar5lex.obj	: [-]ar5lex.c

ar5loadDg.obj	: [-]ar5loadDg.c

ar5memory.obj	: [-]ar5memory.c

ar5printf.obj	: [-]ar5printf.c

ar5read.obj	: [-]ar5read.c

ar5ref.obj	: [-]ar5ref.c

ar5shrutils.obj	: [-]ar5shrutils.c

ar5strings.obj	: [-]ar5strings.c

ar5stubs.obj	: [-]ar5stubs.c

ar5table.obj	: [-]ar5table.c

ar5token.obj	: [-]ar5token.c

ar5unknowns.obj	: [-]ar5unknowns.c
