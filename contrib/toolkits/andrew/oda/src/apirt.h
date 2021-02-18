
/*
		(c) Copyright 1988 Carnegie Mellon University

 See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
 CMU-ITC-071) for full text & permitted uses.


	Include file defining API routine alias real names

    WARNING: this file is generated from the API.
    WARNING: modifications to this file will disappear
    WARNING: when the file is next generated.

*/

#ifndef TK_INCLUDED_APIRT
#define TK_INCLUDED_APIRT

/* Maybe these macros should be moved ??? */
#define IN
#define OUT

#ifndef USE_LONG_NAMES

#define AppendSequence	appseq
#define AppendSubordinate	appsub
#define Array2StrctName	ary2sn
#define AttrDocProfPriority	dppri
#define Chars2String	chrstr
#define ComponentAttrPriority	compri
#define ConcatString	catstr
#define ContentAttrPriority	conpri
#define ContractSequence	conseq
#define CopyConstituent	cpcons
#define CopySequence	cpyseq
#define DeleteConstExpr	dlcnex
#define DeleteConstituent	delcns
#define DeleteDocument	deldoc
#define DeleteIterator	delgen
#define DeleteSequence	delseq
#define DeleteSequenceDeeply	dlsqd
#define DeleteString	delstr
#define Dsclass2Ints	ds2int
#define EvalNumExpr	evlnum
#define EvalObjIdExpr	evlobi
#define EvalRefBind	evlrbv
#define EvalStringExpr	evlstr
#define ExpandSequence	expseq
#define FinalToolKit	tkfinl
#define FindDocumentProfile	fnddp
#define FindDocumentRoot	fndrt
#define GetAttr	getatt
#define GetAttrName	gattnm
#define GetContent	gcontt
#define GetContentKind	gcntkn
#define GetDocument	getdoc
#define GetIntAttr	ginatt
#define GetParmMask	getprm
#define GetValueAttr	gvlatt
#define GetValueName	gvalnm
#define HasAttr	hasatt
#define HoldAttr	hldatt
#define HoldConstituent	hldcns
#define HoldString	hldstr
#define InitializeDefaultValueLists	intdvl
#define InitToolKit	tkinit
#define Ints2Dsclass	int2ds
#define isode_PrintODIF	isprdf
#define isode_ReadODIF	isred
#define isode_WriteODIF	iswrt
#define LabelDocument	lbldoc
#define LabelStructure	lblstr
#define LabelStyles	lblsty
#define LookUpObjectId	lkobid
#define MakeComponent	mkcomp
#define MakeContent	mkcont
#define MakeDocument	mkdoc
#define MakeDocumentProfile	mkdocp
#define MakeSequence	mkseq
#define MakeString	mkstr
#define MakeStyle	mkstyl
#define MakeStylesIterator	mkstgn
#define MakeSubgraphIterator	mksbgn
#define NextConstituent	nxtcns
#define ReadODIF	rdodif
#define ReleaseAttr	rlsatt
#define ReleaseConstituent	rlscns
#define ReleaseString	rlsstr
#define RemAttr	rematt
#define SetAttr	setatt
#define SetFontFamily	stffst
#define SetFontFamilyArray	stffar
#define SetIntAttr	sinatt
#define SetLayoutAttr	setlat
#define SetPresAttr	setrat
#define SetStringAttr	ststat
#define SetValueAttr	svlatt
#define String2StrctName	str2sn
#define StyleAttrPriority	stypri
#define UnspecifyDefaultValueLists	unsdvl
#define UnspecifyFont	unsfnt
#define UnspecParms	unsprm
#define WriteConstituentState	dbgcns
#define WriteDocumentState	dbgdoc
#define WriteODIF	wrodif
#define WriteStylesState	dbgsty
#define WriteSubgraphState	dbgsbg

#endif

#endif
