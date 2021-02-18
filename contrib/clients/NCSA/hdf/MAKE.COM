$ define  lnk$library  sys$library:vaxcrtl
$ cc/debug=all df,dfr8,dfgroup,dfcomp,dfimcomp,dfsd,dfkit,,dfan,df24,dfgr,dfp,dfutil
$ cc/debug=all hdfls,hdfrseq,r8tohdf,hdftotek,tektohdf,hdfcomp,hdftor8
$ library/create df df,dfr8,dfgroup,dfcomp,dfimcomp,dfsd,dfkit,dfan,dfgr,dfp,df24
$ link hdfls,df/library
$ link hdfrseq,df/library
$ link hdfcomp,df/library
$ link hdftor8,df/library
$ link r8tohdf,df/library
$ link hdftotek,df/library
$ link tektohdf,df/library
