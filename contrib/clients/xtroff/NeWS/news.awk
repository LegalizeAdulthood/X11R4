# Translate from NeWS format BDF file to X11 format BDF file.
# D. A. Cahlander 88/12/31.  dac@cray.com

BEGIN {

#  Define FONT family name

	family["Times-Bold"] = "B"
	family["Times-BoldItalic"] = "BI"
	family["Courier"] = "C"
	family["Courier-Bold"] = "CB"
	family["Courier-BoldOblique"] = "CN"
	family["Courier-Oblique"] = "CO"
	family["Helvetica"] = "H"
	family["Helvetica-Bold"] = "HB"
	family["Helvetica-BoldOblique"] = "HN"
	family["Helvetica-Oblique"] = "HO"
	family["Times-Italic"] = "I"
	family["Times-Roman"] = "R"
	family["Symbol"] = "S"

#	family["Boston"] = "Boston"
#	family["Cyrillic"] = "Cyrillic"
#	family["Cursor"] = "Cursor"
#	family["GFont"] = "GFont"
#	family["Helvetica-Medium"] = "Hevetica-Medium"
#	family["Icon"] = "Icon"
#	family["Kanji"] = "Kanji"
#	family["Screen"] = "Screen"
#	family["Symbol-Medium"] = "Symbol-Medium"

# Define SWIDTH the same as X11 timR10.bdf

	swidth["space"] = 250
	swidth["exclam"] = 333
	swidth["quotedbl"] = 408
	swidth["numbersign"] = 500
	swidth["dollar"] = 500
	swidth["percent"] = 833
	swidth["ampersand"] = 778
	swidth["quoteright"] = 333
	swidth["parenleft"] = 333
	swidth["parenright"] = 333
	swidth["asterisk"] = 500
	swidth["plus"] = 564
	swidth["comma"] = 250
	swidth["minus"] = 564
	swidth["period"] = 250
	swidth["slash"] = 278
	swidth["zero"] = 500
	swidth["one"] = 500
	swidth["two"] = 500
	swidth["three"] = 500
	swidth["four"] = 500
	swidth["five"] = 500
	swidth["six"] = 500
	swidth["seven"] = 500
	swidth["eight"] = 500
	swidth["nine"] = 500
	swidth["colon"] = 278
	swidth["semicolon"] = 278
	swidth["less"] = 564
	swidth["equal"] = 564
	swidth["greater"] = 564
	swidth["question"] = 444
	swidth["at"] = 921
	swidth["A"] = 722
	swidth["B"] = 667
	swidth["C"] = 667
	swidth["D"] = 722
	swidth["E"] = 611
	swidth["F"] = 556
	swidth["G"] = 722
	swidth["H"] = 722
	swidth["I"] = 333
	swidth["J"] = 389
	swidth["K"] = 722
	swidth["L"] = 611
	swidth["M"] = 889
	swidth["N"] = 722
	swidth["O"] = 722
	swidth["P"] = 556
	swidth["Q"] = 722
	swidth["R"] = 667
	swidth["S"] = 556
	swidth["T"] = 611
	swidth["U"] = 722
	swidth["V"] = 722
	swidth["W"] = 944
	swidth["X"] = 722
	swidth["Y"] = 722
	swidth["Z"] = 611
	swidth["bracketleft"] = 333
	swidth["backslash"] = 278
	swidth["bracketright"] = 333
	swidth["asciicircum"] = 469
	swidth["underscore"] = 500
	swidth["quoteleft"] = 333
	swidth["a"] = 444
	swidth["b"] = 500
	swidth["c"] = 444
	swidth["d"] = 500
	swidth["e"] = 444
	swidth["f"] = 333
	swidth["g"] = 500
	swidth["h"] = 500
	swidth["i"] = 278
	swidth["j"] = 278
	swidth["k"] = 500
	swidth["l"] = 278
	swidth["m"] = 778
	swidth["n"] = 500
	swidth["o"] = 500
	swidth["p"] = 500
	swidth["q"] = 500
	swidth["r"] = 333
	swidth["s"] = 389
	swidth["t"] = 278
	swidth["u"] = 500
	swidth["v"] = 500
	swidth["w"] = 722
	swidth["x"] = 500
	swidth["y"] = 500
	swidth["z"] = 444
	swidth["braceleft"] = 480
	swidth["bar"] = 200
	swidth["braceright"] = 480
	swidth["asciitilde"] = 541
	swidth["exclamdown"] = 333
	swidth["cent"] = 500
	swidth["sterling"] = 500
	swidth["currency"] = 500
	swidth["yen"] = 500
	swidth["brokenbar"] = 200
	swidth["section"] = 500
	swidth["dieresis"] = 333
	swidth["copyright"] = 760
	swidth["ordfeminine"] = 276
	swidth["guillemotleft"] = 500
	swidth["logicalnot"] = 564
	swidth["hyphen"] = 333
	swidth["registered"] = 760
	swidth["macron"] = 333
	swidth["degree"] = 400
	swidth["plusminus"] = 564
	swidth["twosuperior"] = 300
	swidth["threesuperior"] = 300
	swidth["acute"] = 333
	swidth["mu"] = 500
	swidth["paragraph"] = 453
	swidth["periodcentered"] = 250
	swidth["cedilla"] = 333
	swidth["onesuperior"] = 300
	swidth["ordmasculine"] = 310
	swidth["guillemotright"] = 500
	swidth["onequarter"] = 750
	swidth["onehalf"] = 750
	swidth["threequarters"] = 750
	swidth["questiondown"] = 444
	swidth["Agrave"] = 722
	swidth["Aacute"] = 722
	swidth["Acircumflex"] = 722
	swidth["Atilde"] = 722
	swidth["Adieresis"] = 722
	swidth["Aring"] = 722
	swidth["AE"] = 889
	swidth["Ccedilla"] = 667
	swidth["Egrave"] = 611
	swidth["Eacute"] = 611
	swidth["Ecircumflex"] = 611
	swidth["Edieresis"] = 611
	swidth["Igrave"] = 333
	swidth["Iacute"] = 333
	swidth["Icircumflex"] = 333
	swidth["Idieresis"] = 333
	swidth["Eth"] = 722
	swidth["Ntilde"] = 722
	swidth["Ograve"] = 722
	swidth["Oacute"] = 722
	swidth["Ocircumflex"] = 722
	swidth["Otilde"] = 722
	swidth["Odieresis"] = 722
	swidth["multiply"] = 564
	swidth["Oslash"] = 722
	swidth["Ugrave"] = 722
	swidth["Uacute"] = 722
	swidth["Ucircumflex"] = 722
	swidth["Udieresis"] = 722
	swidth["Yacute"] = 722
	swidth["Thorn"] = 556
	swidth["germandbls"] = 500
	swidth["agrave"] = 444
	swidth["aacute"] = 444
	swidth["acircumflex"] = 444
	swidth["atilde"] = 444
	swidth["adieresis"] = 444
	swidth["aring"] = 444
	swidth["ae"] = 667
	swidth["ccedilla"] = 444
	swidth["egrave"] = 444
	swidth["eacute"] = 444
	swidth["ecircumflex"] = 444
	swidth["edieresis"] = 444
	swidth["igrave"] = 278
	swidth["iacute"] = 278
	swidth["icircumflex"] = 278
	swidth["idieresis"] = 278
	swidth["eth"] = 500
	swidth["ntilde"] = 500
	swidth["ograve"] = 500
	swidth["oacute"] = 500
	swidth["ocircumflex"] = 500
	swidth["otilde"] = 500
	swidth["odieresis"] = 500
	swidth["divide"] = 564
	swidth["oslash"] = 500
	swidth["ugrave"] = 500
	swidth["uacute"] = 500
	swidth["ucircumflex"] = 500
	swidth["udieresis"] = 500
	swidth["yacute"] = 500
	swidth["thorn"] = 500
	swidth["ydieresis"] = 500
	swidth["Zcaron"] = 611
	swidth["zcaron"] = 444
	swidth["Scaron"] = 556
	swidth["scaron"] = 389
	swidth["oe"] = 722
	swidth["lslash"] = 278
	swidth["dotlessi"] = 278
	swidth["Ydieresis"] = 722
	swidth["OE"] = 889
	swidth["Lslash"] = 611
	swidth["emdash"] = 1000
	swidth["caron"] = 333
	swidth["ogonek"] = 333
	swidth["hungarumlaut"] = 333
	swidth["ring"] = 333
	swidth["dotaccent"] = 333
	swidth["breve"] = 333
	swidth["tilde"] = 333
	swidth["circumflex"] = 333
	swidth["grave"] = 333
	swidth["perthousand"] = 1000
	swidth["ellipsis"] = 1000
	swidth["quotedblright"] = 444
	swidth["quotedblbase"] = 444
	swidth["quotesinglbase"] = 333
	swidth["bullet"] = 350
	swidth["daggerdbl"] = 500
	swidth["dagger"] = 500
	swidth["endash"] = 500
	swidth["fl"] = 556
	swidth["fi"] = 556
	swidth["guilsinglright"] = 333
	swidth["guilsinglleft"] = 333
	swidth["quotedblleft"] = 444
	swidth["quotesingle"] = 180
	swidth["florin"] = 500
	swidth["fraction"] = 167
	swidth["trademark"] = 980

# define location of "special" character set

	special[32] = "space"
	special[33] = "exclam"
	special[34] = "universal"
	special[35] = "numbersign"
	special[36] = "existential"
	special[37] = "percent"
	special[38] = "ampersand"
	special[39] = "suchthat"
	special[40] = "parenleft"
	special[41] = "parenright"
	special[42] = "asteriskmath"
	special[43] = "plus"
	special[44] = "comma"
	special[45] = "minus"
	special[46] = "period"
	special[47] = "slash"
	special[48] = "zero"
	special[49] = "one"
	special[50] = "two"
	special[51] = "three"
	special[52] = "four"
	special[53] = "five"
	special[54] = "six"
	special[55] = "seven"
	special[56] = "eight"
	special[57] = "nine"
	special[58] = "colon"
	special[59] = "semicolon"
	special[60] = "less"
	special[61] = "equal"
	special[62] = "greater"
	special[63] = "question"
	special[64] = "congruent"
	special[65] = "Alpha"
	special[66] = "Beta"
	special[67] = "Chi"
	special[68] = "Delta"
	special[69] = "Epsilon"
	special[70] = "Phi"
	special[71] = "Gamma"
	special[72] = "Eta"
	special[73] = "Iota"
	special[74] = "theta1"
	special[75] = "Kappa"
	special[76] = "Lambda"
	special[77] = "Mu"
	special[78] = "Nu"
	special[79] = "Omicron"
	special[80] = "Pi"
	special[81] = "Theta"
	special[82] = "Rho"
	special[83] = "Sigma"
	special[84] = "Tau"
	special[85] = "Upsilon"
	special[86] = "sigma1"
	special[87] = "Omega"
	special[88] = "Xi"
	special[89] = "Psi"
	special[90] = "Zeta"
	special[91] = "bracketleft"
	special[92] = "therefore"
	special[93] = "bracketright"
	special[94] = "perpendicular"
	special[95] = "underscore"
	special[96] = "radicalex"
	special[97] = "alpha"
	special[98] = "beta"
	special[99] = "chi"
	special[100] = "delta"
	special[101] = "epsilon"
	special[102] = "phi"
	special[103] = "gamma"
	special[104] = "eta"
	special[105] = "iota"
	special[106] = "phi1"
	special[107] = "kappa"
	special[108] = "lambda"
	special[109] = "mu"
	special[110] = "nu"
	special[111] = "omicron"
	special[112] = "pi"
	special[113] = "theta"
	special[114] = "rho"
	special[115] = "sigma"
	special[116] = "tau"
	special[117] = "upsilon"
	special[118] = "omega1"
	special[119] = "omega"
	special[120] = "xi"
	special[121] = "psi"
	special[122] = "zeta"
	special[123] = "braceleft"
	special[124] = "bar"
	special[125] = "braceright"
	special[126] = "similar"
	special[161] = "Upsilon1"
	special[162] = "minute"
	special[163] = "lessequal"
	special[164] = "fraction"
	special[165] = "infinity"
	special[166] = "florin"
	special[167] = "club"
	special[168] = "diamond"
	special[169] = "heart"
	special[170] = "spade"
	special[171] = "arrowboth"
	special[172] = "arrowleft"
	special[173] = "arrowup"
	special[174] = "arrowright"
	special[175] = "arrowdown"
	special[176] = "degree"
	special[177] = "plusminus"
	special[178] = "second"
	special[179] = "greaterequal"
	special[180] = "multiply"
	special[181] = "proportional"
	special[182] = "partialdiff"
	special[183] = "bullet"
	special[184] = "divide"
	special[185] = "notequal"
	special[186] = "equivalence"
	special[187] = "approxequal"
	special[188] = "ellipsis"
	special[189] = "arrowvertex"
	special[190] = "arrowhorizex"
	special[191] = "carriagereturn"
	special[192] = "aleph"
	special[193] = "Ifraktur"
	special[194] = "Rfraktur"
	special[195] = "weierstrass"
	special[196] = "circlemultiply"
	special[197] = "circleplus"
	special[198] = "emptyset"
	special[199] = "intersection"
	special[200] = "union"
	special[201] = "propersuperset"
	special[202] = "reflexsuperset"
	special[203] = "notsubset"
	special[204] = "propersubset"
	special[205] = "reflexsubset"
	special[206] = "element"
	special[207] = "notelement"
	special[208] = "angle"
	special[209] = "gradient"
	special[210] = "registerserif"
	special[211] = "copyrightserif"
	special[212] = "trademarkserif"
	special[213] = "product"
	special[214] = "radical"
	special[215] = "dotmath"
	special[216] = "logicalnot"
	special[217] = "logicaland"
	special[218] = "logicalor"
	special[219] = "arrowdblboth"
	special[220] = "arrowdblleft"
	special[221] = "arrowdblup"
	special[222] = "arrowdblright"
	special[223] = "arrowdbldown"
	special[224] = "lozenge"
	special[225] = "angleleft"
	special[226] = "registersans"
	special[227] = "copyrightsans"
	special[228] = "trademarksans"
	special[229] = "summation"
	special[230] = "parenlefttp"
	special[231] = "parenleftex"
	special[232] = "parenleftbt"
	special[233] = "bracketlefttp"
	special[234] = "bracketleftex"
	special[235] = "bracketleftbt"
	special[236] = "bracelefttp"
	special[237] = "braceleftmid"
	special[238] = "braceleftbt"
	special[239] = "braceex"
	special[240] = "apple"
	special[241] = "angleright"
	special[242] = "integral"
	special[243] = "integraltp"
	special[244] = "integralex"
	special[245] = "integralbt"
	special[246] = "parenrighttp"
	special[247] = "parenrightex"
	special[248] = "parenrightbt"
	special[249] = "bracketrighttp"
	special[250] = "bracketrightex"
	special[251] = "bracketrightbt"
	special[252] = "bracerighttp"
	special[253] = "bracerightmid"
	special[254] = "bracerightbt"
	special[133] = "box"
	special[132] = "square"
	special[134] = "circle"
	special[135] = "boxrule"
	special[137] = "rule"
	special[148] = "rightceiling"
	special[136] = "boldvertical"
	special[146] = "rightfloor"
	special[141] = "righttopcurly"
	special[144] = "rightmidcurly"
	special[142] = "rightbotcurly"
	special[147] = "leftceiling"
	special[145] = "leftfloor"
	special[139] = "lefttopcurly"
	special[143] = "leftmidcurly"
	special[140] = "leftbotcurly"
}

#  Header from fonts/bdf/75dpi/timR24.bdf
#
#	STARTFONT 2.1
#	FONT -Adobe-Times-Medium-R-Normal--10-100-75-75-P-54-ISO8859-1
#	SIZE 10 75 75
#	FONTBOUNDINGBOX 11 11 -1 -3
#	STARTPROPERTIES 23
#	COMMENT Begin LogicalFontDescription
#	FONTNAME_REGISTRY ""
#	FAMILY_NAME "Times"
#	FOUNDRY "Adobe"
#	WEIGHT_NAME "Medium"
#	SETWIDTH_NAME "Normal"
#	SLANT "R"
#	ADD_STYLE_NAME ""
#	PIXEL_SIZE 10
#	POINT_SIZE 100
#	RESOLUTION_X 75
#	RESOLUTION_Y 75
#	SPACING "P"
#	AVERAGE_WIDTH 54
#	CHARSET_REGISTRY "ISO8859"
#	CHARSET_ENCODING "1"
#	DEVICE_FONT_NAME "Times-Roman"
#	COMMENT END LogicalFontDescription
#	CHARSET_COLLECTIONS "ASCII ISOLATIN1 ADOBESTANDARD"
#	FULL_NAME "Times Roman"
#	COPYRIGHT "Copyright (c) 1984, 1987 Adobe Systems, Inc., Portions Copyright 1988 Digital Equipment Corp."
#	COMMENT ***** end of inserted font properties
#	FONT_ASCENT 8
#	FONT_DESCENT 3
#	CAP_HEIGHT 7
#	X_HEIGHT 5
#	ENDPROPERTIES 
#	CHARS 228
#	STARTCHAR space
#	ENCODING 32
#	SWIDTH 250 0
#	DWIDTH 2 0
#	BBX 1 1 0 0
#	BITMAP
#	00
#	ENDCHAR

#  Change the version number and fill in what is needed for bdftosnf

/^StartFont / {
	print "STARTFONT 2.1"
	next
}

/^Comment / { $1 = "COMMENT" }

/^Font / {
	fontname = $2
	next
}

/^Size / {
	if (length(family[fontname]))
		print "FONT devpsc." family[fontname] "." $2 "." 75
	else
		print "FONT " fontname "." $2 "." 75
	$1 = "SIZE"
}

/^FontBoundingBox / {
	$1 = "FONTBOUNDINGBOX"
	print
	print "STARTPROPERTIES 5"
	print "DEVICE_FONT_NAME \"" fontname "\""
	print "RESOLUTION_X 75"
	print "RESOLUTION_Y 75"
	if ($3 > $5) {
		print "FONT_ASCENT", $3
		print "FONT_DESCENT", -$5
	} else {
		print "FONT_ASCENT", $5
		print "FONT_DESCENT", -$3
	}
	print "ENDPROPERTIES"
	next
}

/^Chars / { $1 = "CHARS" }

/^StartChar / {
	$1 = "STARTCHAR"
	ch = $2
	if (fontname == "Symbol")
		next
}

/^Encoding / {
	$1 = "ENCODING"

#  Change encoded name for the Symbol set since dumpfont
#  gives the wrong name.

	if (fontname == "Symbol") {
		if (length(special[$2]))
			ch = special[$2]
		else
			ch = "<unknown>"
		print "STARTCHAR", ch
		print
		next
	}

#  Change to unencoded for "<unknown>" characters.
#  These characters could even be removed from the character set.

	if ((ch == "<unknown>") && length(family[fontname]))
		$2 = -1
}

#  SWIDTH is not provided.  It is not needed for xtroff, but
#  should be supplied for completeness.  Here it is supplied
#  from a canned table.

/^DWidth / {
	if (length(swidth[ch]) != 0)
		print "SWIDTH", swidth[ch], "0"
	else
		print "SWIDTH 250 0"
	$1 = "DWIDTH"
}

/^Bitmap/ { $1 = "BITMAP" }

/^EndChar/ { $1 = "ENDCHAR" }

/^EndFont/ { $1 = "ENDFONT" }

# other

{ print }
