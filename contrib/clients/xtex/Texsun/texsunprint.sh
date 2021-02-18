#!/bin/sh
#
#	Local configurations
#
DVIPRINTER=${DVIPRINTER-dvipslpr}
#
dviInput=$1
dviOutput=""
removeOutput=0
shift;
if [ -z "$*" ]
then
   dviOutput=$dviInput
else
  pages=""
  for pagenumber
  do
    pages="$pages =$pagenumber"
  done
  echo pages are "$pages"
  dviOutput=/tmp/"`basename $dviInput | sed -e 's/.dvi//g' `-pages.dvi"
  dviselect -i $dviInput -o $dviOutput $pages
  removeOutput=1
fi
$DVIPRINTER $dviOutput
if [ $removeOutput -eq 1 ]
then
    rm -f $dviOutput
fi
