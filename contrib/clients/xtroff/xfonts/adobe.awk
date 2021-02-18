# local/fonts/bdf/adobe.awk
#
#
# This awk(1) program allocates ENCODING space to PostScript
# encoding sequence.
#
# David A. Cahlander 88/12/17.
#

# allocation of extended characters

BEGIN {
	adobe["space"] = 32
	adobe["exclam"] = 33
	adobe["quotedbl"] = 34
	adobe["numbersign"] = 35
	adobe["dollar"] = 36
	adobe["percent"] = 37
	adobe["ampersand"] = 38
	adobe["quoteright"] = 39
	adobe["parenleft"] = 40
	adobe["parenright"] = 41
	adobe["asterisk"] = 42
	adobe["plus"] = 43
	adobe["comma"] = 44
	adobe["hyphen"] = 45
#	adobe["minus"] = 45	# minus is in Symbol set
	adobe["period"] = 46
	adobe["slash"] = 47
	adobe["zero"] = 48
	adobe["one"] = 49
	adobe["two"] = 50
	adobe["three"] = 51
	adobe["four"] = 52
	adobe["five"] = 53
	adobe["six"] = 54
	adobe["seven"] = 55
	adobe["eight"] = 56
	adobe["nine"] = 57
	adobe["colon"] = 58
	adobe["semicolon"] = 59
	adobe["less"] = 60
	adobe["equal"] = 61
	adobe["greater"] = 62
	adobe["question"] = 63
	adobe["at"] = 64
	adobe["A"] = 65
	adobe["B"] = 66
	adobe["C"] = 67
	adobe["D"] = 68
	adobe["E"] = 69
	adobe["F"] = 70
	adobe["G"] = 71
	adobe["H"] = 72
	adobe["I"] = 73
	adobe["J"] = 74
	adobe["K"] = 75
	adobe["L"] = 76
	adobe["M"] = 77
	adobe["N"] = 78
	adobe["O"] = 79
	adobe["P"] = 80
	adobe["Q"] = 81
	adobe["R"] = 82
	adobe["S"] = 83
	adobe["T"] = 84
	adobe["U"] = 85
	adobe["V"] = 86
	adobe["W"] = 87
	adobe["X"] = 88
	adobe["Y"] = 89
	adobe["Z"] = 90
	adobe["bracketleft"] = 91
	adobe["backslash"] = 92
	adobe["bracketright"] = 93
	adobe["asciicircum"] = 94
	adobe["underscore"] = 95
	adobe["quoteleft"] = 96
	adobe["a"] = 97
	adobe["b"] = 98
	adobe["c"] = 99
	adobe["d"] = 100
	adobe["e"] = 101
	adobe["f"] = 102
	adobe["g"] = 103
	adobe["h"] = 104
	adobe["i"] = 105
	adobe["j"] = 106
	adobe["k"] = 107
	adobe["l"] = 108
	adobe["m"] = 109
	adobe["n"] = 110
	adobe["o"] = 111
	adobe["p"] = 112
	adobe["q"] = 113
	adobe["r"] = 114
	adobe["s"] = 115
	adobe["t"] = 116
	adobe["u"] = 117
	adobe["v"] = 118
	adobe["w"] = 119
	adobe["x"] = 120
	adobe["y"] = 121
	adobe["z"] = 122
	adobe["braceleft"] = 123
	adobe["bar"] = 124
	adobe["braceright"] = 125
	adobe["asciitilde"] = 126
	adobe["onequarter"] = 129	#incorrect
	adobe["onehalf"] = 130		#incorrect
	adobe["threequarters"] = 131	#incorrect
	adobe["exclamdown"] = 161
	adobe["cent"] = 162
	adobe["sterling"] = 163
	adobe["fraction"] = 164
	adobe["yen"] = 165
	adobe["florin"] = 166
	adobe["section"] = 167
	adobe["currency"] = 168
	adobe["quotesingle"] = 169
	adobe["quotedblleft"] = 170
	adobe["guillemotleft"] = 171
	adobe["guilsinglleft"] = 172
	adobe["guilsinglright"] = 173
	adobe["fi"] = 174
	adobe["fl"] = 175
	adobe["endash"] = 177
	adobe["dagger"] = 178
	adobe["daggerdbl"] = 179
	adobe["periodcentered"] = 180
	adobe["paragraph"] = 182
	adobe["bullet"] = 183
	adobe["quotesinglbase"] = 184
	adobe["quotedblbase"] = 185
	adobe["quotedblright"] = 186
	adobe["guillemotright"] = 187
	adobe["ellipsis"] = 188
	adobe["perthousand"] = 189
	adobe["questiondown"] = 191
	adobe["grave"] = 193
	adobe["acute"] = 194
	adobe["circumflex"] = 195
	adobe["tilde"] = 196
	adobe["macron"] = 197
	adobe["breve"] = 198
	adobe["dotaccent"] = 199
	adobe["dieresis"] = 200
	adobe["ring"] = 202
	adobe["cedilla"] = 203
	adobe["hungarumlaut"] = 205
	adobe["ogonek"] = 206
	adobe["caron"] = 207
	adobe["emdash"] = 208
	adobe["AE"] = 225
	adobe["ordfeminine"] = 227
	adobe["Lslash"] = 232
	adobe["Oslash"] = 233
	adobe["OE"] = 234
	adobe["ordmasculine"] = 235
	adobe["ae"] = 241
	adobe["dotlessi"] = 245
	adobe["lslash"] = 248
	adobe["oslash"] = 249
	adobe["oe"] = 250
	adobe["germandbls"] = 251
	adobe["Aacute"] = 256
	adobe["Acircumflex"] = 257
	adobe["Adieresis"] = 258
	adobe["Agrave"] = 259
	adobe["Aring"] = 260
	adobe["Atilde"] = 261
	adobe["Ccedilla"] = 262
	adobe["Eacute"] = 263
	adobe["Ecircumflex"] = 264
	adobe["Edieresis"] = 265
	adobe["Egrave"] = 266
	adobe["Iacute"] = 267
	adobe["Icircumflex"] = 268
	adobe["Idieresis"] = 269
	adobe["Igrave"] = 270
	adobe["Ntilde"] = 271
	adobe["Oacute"] = 272
	adobe["Ocircumflex"] = 273
	adobe["Odieresis"] = 274
	adobe["Ograve"] = 275
	adobe["Otilde"] = 276
	adobe["Scaron"] = 277
	adobe["Uacute"] = 278
	adobe["Ucircumflex"] = 279
	adobe["Udieresis"] = 280
	adobe["Ugrave"] = 281
	adobe["Ydieresis"] = 282
	adobe["Zcaron"] = 283
	adobe["aacute"] = 284
	adobe["acircumflex"] = 285
	adobe["adieresis"] = 286
	adobe["agrave"] = 287
	adobe["aring"] = 288
	adobe["atilde"] = 289
	adobe["ccedilla"] = 290
	adobe["eacute"] = 291
	adobe["ecircumflex"] = 292
	adobe["edieresis"] = 293
	adobe["egrave"] = 294
	adobe["iacute"] = 295
	adobe["icircumflex"] = 296
	adobe["idieresis"] = 297
	adobe["igrave"] = 298
	adobe["ntilde"] = 299
	adobe["oacute"] = 300
	adobe["ocircumflex"] = 301
	adobe["odieresis"] = 302
	adobe["ograve"] = 303
	adobe["otilde"] = 304
	adobe["scaron"] = 305
	adobe["uacute"] = 306
	adobe["ucircumflex"] = 307
	adobe["udieresis"] = 308
	adobe["ugrave"] = 309
	adobe["ydieresis"] = 310
	adobe["zcaron"] = 311

#  Extra characters in the X-character set that are not in
#  the NeWS character set.

	adobe["Eth"] =  320
	adobe["Thorn"] =  321
	adobe["Yacute"] =  322
	adobe["brokenbar"] =  323
	adobe["copyright"] =  324
	adobe["degree"] =  325
	adobe["divide"] =  326
	adobe["eth"] =  327
	adobe["logicalnot"] =  328
	adobe["minus"] =  329
	adobe["mu"] =  330
	adobe["multiply"] =  331
	adobe["onesuperior"] =  332
	adobe["plusminus"] =  333
	adobe["registered"] =  334
	adobe["thorn"] =  335
	adobe["threesuperior"] =  336
	adobe["trademark"] =  337
	adobe["twosuperior"] =  338
	adobe["yacute"] =  339

# family is a mapping from the BDF character set name to the
#	troff name

	family["CourierBoldO"] = "CD"
	family["CourierBoldR"] = "CB"
	family["CourierMediumO"] = "CO"
	family["CourierMediumR"] = "C"
	family["HelveticaBoldO"] = "HD"
	family["HelveticaBoldR"] = "HB"
	family["HelveticaMediumO"] = "HO"
	family["HelveticaMediumR"] = "H"
	family["New Century SchoolbookBoldI"] = "ND"
	family["New Century SchoolbookBoldR"] = "NB"
	family["New Century SchoolbookMediumI"] = "NI"
	family["New Century SchoolbookMediumR"] = "N"
	family["TimesBoldI"] = "BI"
	family["TimesBoldR"] = "B"
	family["TimesMediumI"] = "I"
	family["TimesMediumR"] = "R"
	family["SymbolMediumR"] = "S"
}

# change font sequence
# FONT -Adobe-Times-Bold-R-Normal--8-80-75-75-P-47-ISO8859-1
# new name is something like
# devpsc.r.8.75

/^FONT / {
	n = split($0, arg, "-")
	charset = arg[3] arg[4] arg[5]
	if (length(family[charset]) == 0) {
		unknown = 1
		print $0
		next
	}
	print "COMMENT", $0
	print "FONT devpsc." family[charset] "." arg[8] "." arg[10]
	if (arg[3] == "Symbol")
		symbol = 1
	next
}

# CHARSET_REGISTRY "ISO8859"
/^CHARSET_REGISTRY / {
	if (unknown || symbol)
		print
	else
		printf "CHARSET_REGISTRY \"PostScript\"\n"
	next
}

# CHARSET_COLLECTIONS "ASCII ISOLATIN1 ADOBESTANDARD"
/^CHARSET_COLLECTIONS / {
	if (unknown || symbol)
		print
	else
		printf "CHARSET_COLLECTIONS \"ASCII POSTSCRIPT ADOBESTANDARD\"\n"
	next
}

# COPYRIGHT "Copyright (c) 1984, 1987 Adobe Systems, Inc., Portions Copyright 1988 Digital Equipment Corp."
/^COPYRIGHT / {
	if (unknown || symbol)
		print
	else {
		printf "COPYRIGHT \"Copyright (c) 1984, 1987 Adobe Systems, Inc., "
		printf "Portions Copyright 1988 Digital Equipment Corp., "
		printf "Portions Copyright 1988 Cray Research, Inc.\"\n"
	}
	next
}

# set character name

/^STARTCHAR .*/ {
	name = $2
	print
	next
}

# change ENCODING to PostScript encoding

/^ENCODING .*/ {
	if (unknown || symbol) {
		print
		next
	}
	if (length(code = adobe[name]) == 0)	# if character not standard
		code = -1
	print "ENCODING", code
	next
}

# other lines

{ print }

END { }
