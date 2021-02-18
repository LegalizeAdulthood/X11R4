lc -ml df.c dfr8.c dfgroup.c dfcomp.c dfimcomp.c dfsd.c dfkit.c 
lc -ml dfan.c dfgr.c df24.c dfp.c
oml df r df.obj dfr8.obj dfgroup.obj dfcomp.obj dfimcomp.obj ipasm.obj
oml df r dfan.obj df24.obj dfgr.obj dfp.obj dfsd.obj  dfkit.obj 
lc -ml hdfls.c r8tohdf.c hdfrseq.c hdfcomp.c hdftor8.c tektohdf.c hdftotek.c
link c:\lc\l\c+hdfls,hdfls,nul,df+c:\lc\l\lc
link c:\lc\l\c+r8tohdf,r8tohdf,nul,df+c:\lc\l\lc
link c:\lc\l\c+hdfrseq,hdfrseq,nul,df+c:\lc\l\lc
link c:\lc\l\c+hdftor8,hdftor8,nul,df+c:\lc\l\lc
link c:\lc\l\c+hdfcomp,hdfcomp,nul,df+c:\lc\l\lc
link c:\lc\l\c+tektohdf,tektohdf,nul,df+c:\lc\l\lc
link c:\lc\l\c+hdftotek,hdftotek,nul,df+c:\lc\l\lc
