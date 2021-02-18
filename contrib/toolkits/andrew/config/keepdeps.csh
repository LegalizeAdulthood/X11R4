#!/bin/csh -f
echo >! tmp.dep
mv Makefile Makefile.BAK
set DEPFILES=`/bin/ls *.[cH]`
if ($#DEPFILES != 0) then
echo "## Keeping Old Dependency Information"
ed Makefile.BAK <<!  >>& /dev/null
/##### DEPENDENCY LINE - DO NOT DELETE #####/
1,.d
w tmp.dep
q
!
endif

