#!/bin/csh -f
################################################
set nonomatch
################################################
################################################
if ( "$#argv" < 6) then
	echo 'usage: depends ${DEPEND} ${BASEDIR} ${XINCDIR} ${CC} ${CLASS} ${INCLUDES} [${AFSBASEDIR}]'
	exit(1)
endif
################################################
set DEPEND=$1
set BASEDIR=$2
set XINCDIR=$3
set CC="$4"
set CLASS=$5
set INCLUDES="$6"
if ($#argv == 7) then
	set AFSBASEDIR=$7
else
	set AFSBASEDIR="/no/afs/includes/to/look/for"
endif

################################################
set HFILES=`/bin/ls *.ch` 
set CFILES=`/bin/ls *.c`
if ( "$#CFILES" == 0 && "$#HFILES" == 0 ) then
	echo "No Dependencies to Check...continuing...."
	exit(0)
endif
################################################
set TMP_MKFILE=/tmp/.mkfile_$$
set H_DEPENDS=/tmp/Hdep_$$
set C_DEPENDS=/tmp/Cdep_$$
set DEPENDS=.depends
################################################
mv Makefile Makefile.BAK
sed -n -e '1,/##### DEPENDENCY LINE - DO NOT DELETE #####/p' Makefile.BAK > Makefile
################################################
if ( "$#HFILES" != 0) then
	foreach i (${HFILES})
		${CLASS} -M ${INCLUDES} ${i} | egrep : | egrep -v ":.*:" >> $H_DEPENDS
	end
	cat Makefile ${H_DEPENDS} >> ${TMP_MKFILE}
	set IHFILES=`echo ${HFILES} | sed -e 's;\.ch;.ih;g'`
	make -f ${TMP_MKFILE} ${IHFILES} INCLUDES="${INCLUDES}"
endif
################################################
echo >! ${C_DEPENDS}
if ( "$#CFILES" != 0) then
	${DEPEND} -s "##### DEPENDENCY LINE - DO NOT DELETE #####" ${INCLUDES} -f ${C_DEPENDS} -w 1 *.c
endif
################################################
if (-e ${H_DEPENDS} && ! -z ${H_DEPENDS}) then
	cat ${H_DEPENDS} >> ${C_DEPENDS}
endif

cat ${C_DEPENDS} | sed \
	-e s+${BASEDIR}+\$\{BASEDIR\}/+g \
	-e s+${XINCDIR}+\$\{XINCDIR\}/+g \
	-e s+${AFSBASEDIR}+\$\{AFSBASEDIR\}/+g \
	-e s+//+/+g \
	| egrep -v " /|	/|#####" \
	> ${DEPENDS}
################################################
if (! -z $DEPENDS) then
	cat $DEPENDS >> Makefile
endif
rm -f ${H_DEPENDS}.bak ${C_DEPENDS}.bak ${TMP_MKFILE}.bak ${DEPENDS}.bak $H_DEPENDS $C_DEPENDS $TMP_MKFILE

