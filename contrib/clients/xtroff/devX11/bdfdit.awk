# devpsc/bdfdit.awk
#
#
# This awk(1) program serves two purposes.  It helps convert
# X11 bdf font metrics files (bdf files) into ditroff
# font description files (as input to makedev), and it
# helps create the ditroff device description file (DESC) for
# bdf fonts.
#

BEGIN {
	SCALE = 5.0		# font width scale factor to 
				# achieve proper dynamic range
				# (ditroff widths are <= 256)

	# file names for pieces. These are cat'ed together.
	header = "temp.header"	# comments and ligature list
	spaces = "temp.spaces"	# space widths and "charset" header
	trailer = "temp.trailer"# actual character data
	aux = "temp.aux"

	isspecial = 0
	istext = 1

# font name translations

	font["Times-Medium-R"] = "R"
	font["Times-Medium-I"] = "I"
	font["Times-Bold-R"] = "B"
	font["Times-Bold-I"] = "BI"
#	font["Times-Medium-R"] = "TR"
#	font["Times-Medium-I"] = "TI"
#	font["Times-Bold-R"] = "TB"
#	font["Times-Bold-I"] = "TD"
	font["Courier-Medium-R"] = "C"
	font["Courier-Medium-O"] = "CO"
	font["Courier-Bold-R"] = "CB"
	font["Courier-Bold-O"] = "CD"
	font["Helvetica-Medium-R"] = "H"
	font["Helvetica-Medium-O"] = "HO"
	font["Helvetica-Bold-R"] = "HB"
	font["Helvetica-Bold-O"] = "HD"
	font["New Century Schoolbook-Medium-R"] = "nc"
	font["New Century Schoolbook-Medium-I"] = "nC"
	font["New Century Schoolbook-Bold-R"] = "Nc"
	font["New Century Schoolbook-Bold-I"] = "NC"
	font["Charter-Medium-R"] = "A"
	font["Charter-Medium-I"] = "AI"
	font["Charter-Bold-R"] = "AB"
	font["Charter-Bold-I"] = "AD"
	font["Symbol-Medium-R"] = "S"

# ligatures

	ligature["ff"] = "ff"
	ligature["ffi"] = "Fi"
	ligature["ffl"] = "Fl"
	ligature["fi"] = "fi"
	ligature["fl"] = "fl"

# ditmap is a mapping from non-ascii bdf file character names
#	(plus a few special ones) to DITROFF \( special codes.
#	Note that some chars have more than one code (separated by spaces).

	ditmap[".ditspace"] = "\\^ \\| \\&"
	ditmap["AE"] = "AE"
	ditmap["Alpha"] = "*A"
	ditmap["Beta"] = "*B"
	ditmap["Chi"] = "*X"
	ditmap["Delta"] = "*D"
	ditmap["Epsilon"] = "*E"
	ditmap["Eta"] = "*Y"
	ditmap["Gamma"] = "*G"
	ditmap["Iota"] = "*I"
	ditmap["Kappa"] = "*K"
	ditmap["Lambda"] = "*L"
	ditmap["Lslash"] = "PL"
	ditmap["Mu"] = "*M"
	ditmap["Nu"] = "*N"
	ditmap["OE"] = "OE"
	ditmap["Omega"] = "*W"
	ditmap["Omicron"] = "*O"
	ditmap["Oslash"] = "O/"
	ditmap["Phi"] = "*F"
	ditmap["Pi"] = "*P"
	ditmap["Psi"] = "*Q"
	ditmap["Rho"] = "*R"
	ditmap["Sigma"] = "*S"
	ditmap["Tau"] = "*T"
	ditmap["Theta"] = "*H"
	ditmap["Upsilon"] = "*U"
	ditmap["Xi"] = "*C"
	ditmap["Zeta"] = "*Z"
 	ditmap["acute"] = "aa \\'"
	ditmap["ae"] = "ae"
	ditmap["aleph"] = "al"
	ditmap["alpha"] = "*a"
 	ditmap["angle"] = "ag"
 	ditmap["angleleft"] = "l< L<"
 	ditmap["angleright"] = "r> R>"
	ditmap["apple"] = "AL"
	ditmap["approxequal"] = "~="
 	ditmap["arrowboth"] = "<>"
 	ditmap["arrowdbldown"] = "d="
 	ditmap["arrowdblboth"] = "io"
 	ditmap["arrowdblleft"] = "<: lh"	# left double arrow (& hand)
 	ditmap["arrowdblright"] = ":> im rh"	# right double arrow (& hand)
 	ditmap["arrowdblup"] = "u="
	ditmap["arrowdown"] = "da"
	ditmap["arrowleft"] = "<-"
	ditmap["arrowright"] = "->"
	ditmap["arrowup"] = "ua"
	ditmap["asciicircum"] = "a^"
	ditmap["asciitilde"] = "a~"
	ditmap["asteriskmath"] = "**"
	ditmap["bar"] = "or"
	ditmap["beta"] = "*b"
# 	ditmap["br"] = "br"	# box rule
# 	ditmap["bu"] = "bu"	# bullet
# 	ditmap["bv"] = "bv"	# bold vertical
# 	ditmap["bx"] = "bx"	# box
	ditmap["breve"] = "be"
	ditmap["bullet"] = "bu"
	ditmap["caron"] = "hc"
	ditmap["cedilla"] = "cd"
	ditmap["cent"] = "ct"
	ditmap["chi"] = "*x"
	ditmap["circumflex"] = "^"
 	ditmap["carriagereturn"] = "cr"
# 	ditmap["ci"] = "ci"	# circle
 	ditmap["circlemultiply"] = "ax"
 	ditmap["circleplus"] = "a+"
	ditmap["club"] = "Cc"
	ditmap["congruent"] = "=~"
	ditmap["copyrightserif"] = "co"
	ditmap["dagger"] = "dg"
	ditmap["daggerdbl"] = "dd"
	ditmap["degree"] = "de"
	ditmap["delta"] = "*d"
 	ditmap["diamond"] = "Cd"
 	ditmap["dieresis"] = "um .."	# umlaut
 	ditmap["dotaccent"] = "dt"
	ditmap["dotlessi"] = "ui"
	ditmap["divide"] = "di"
 	ditmap["dotmath"] = "m."
 	ditmap["element"] = "mo"
	ditmap["emdash"] = "em"
	ditmap["emptyset"] = "es"
	ditmap["endash"] = "en"
	ditmap["epsilon"] = "*e"
	ditmap["equal"] = "eq"	;	mathonly["eq"] = "equal"
	ditmap["equivalence"] = "=="
	ditmap["eta"] = "*y"
 	ditmap["exclamdown"] = "!! I!"
	ditmap["existential"] = "te"
	ditmap["ff"] = "ff"
	ditmap["ffi"] = "Fi"
	ditmap["ffl"] = "Fl"
	ditmap["fi"] = "fi"
	ditmap["fl"] = "fl"
 	ditmap["florin"] = "$D"
	ditmap["germandbls"] = "ss"
	ditmap["gamma"] = "*g"
	ditmap["gradient"] = "gr"
 	ditmap["grave"] = "ga \\`"
	ditmap["greaterequal"] = ">="
	ditmap["guillemotleft"] = "d<"
	ditmap["guillemotright"] = "d>"
	ditmap["guillmotleft"] = "d<"
	ditmap["guillmotright"] = "d>"
	ditmap["guilsinglleft"] = "l<"
	ditmap["guilsinglright"] = "r>"
	ditmap["heart"] = "bs Ch"
	ditmap["hungarumlaut"] = "''"
	ditmap["hyphen"] = "hy"
	ditmap["infinity"] = "if"
	ditmap["integral"] = "is"
# 	ditmap["lb"] = "lb"	# left bot curly
# 	ditmap["lc"] = "lc"	# left ceil
# 	ditmap["lf"] = "lf"	# left floor
# 	ditmap["lk"] = "lk"	# left center curly
 	ditmap["logicaland"] = "an la"
 	ditmap["logicalor"] = "lo"
	ditmap["lozenge"] = "dm"
	ditmap["intersection"] = "ca"
	ditmap["iota"] = "*i"
	ditmap["kappa"] = "*k"
	ditmap["lambda"] = "*l"
	ditmap["lessequal"] = "<="
	ditmap["logicalnot"] = "no"
	ditmap["lslash"] = "Pl"
	ditmap["onequarter"] = "14"
	ditmap["onehalf"] = "12"
	ditmap["threequarters"] = "34"
 	ditmap["macron"] = "mc ma"
	ditmap["minus"] = "\\- mi"	;	mathonly["mi"] = "minus"
# 	ditmap["minus"] = "\\- mi"
 	ditmap["minute"] = "fm mt"
	ditmap["mu"] = "*m"
	ditmap["multiply"] = "mu"
 	ditmap["notelement"] = "!m"
 	ditmap["notsubset"] = "!s"
	ditmap["notequal"] = "!="
	ditmap["nu"] = "*n"
 	ditmap["ogonek"] = "og"
	ditmap["oe"] = "oe"
	ditmap["omega"] = "*w"
	ditmap["omicron"] = "*o"
	ditmap["oslash"] = "o/"
 	ditmap["onehalf"] = "12"
 	ditmap["onequarter"] = "14"
	ditmap["paragraph"] = "pp"
	ditmap["partialdiff"] = "pd"
	ditmap["phi"] = "*f"
	ditmap["pi"] = "*p"
	ditmap["plus"] = "pl"	;	mathonly["pl"] = "plus"
	ditmap["plusminus"] = "+-"
	ditmap["propersubset"] = "sb"
	ditmap["propersuperset"] = "sp"
	ditmap["proportional"] = "pt"
	ditmap["psi"] = "*q"
 	ditmap["questiondown"] = "?? I?"
	ditmap["quotedblbase"] = "bq"
 	ditmap["quotedblleft"] = "lq oq"
 	ditmap["quotedblright"] = "rq cq"
 	ditmap["quotesingle"] = "n'"
	ditmap["radical"] = "sr"
	ditmap["radicalex"] = "rn"
	ditmap["reflexsubset"] = "ib"
	ditmap["reflexsuperset"] = "ip"
	ditmap["registerserif"] = "rg"
	ditmap["rho"] = "*r"
 	ditmap["ring"] = "ri"
 	ditmap["second"] = "sd"
	ditmap["section"] = "sc"
	ditmap["sigma"] = "*s"
	ditmap["sigma1"] = "ts"
	ditmap["similar"] = "ap"
	ditmap["slash"] = "sl"
	ditmap["spade"] = "Cs"
 	ditmap["sterling"] = "ps po"
	ditmap["suchthat"] = "cm"
	ditmap["tau"] = "*t"
	ditmap["therefore"] = "tf"
	ditmap["theta"] = "*h"
	ditmap["tilde"] = "~"
	ditmap["trademarkserif"] = "tm"
	ditmap["underscore"] = "ul \\_"
# 	ditmap["ul"] = "ul"	# under rule
# 	ditmap["underscore"] = "\\_"
# 	ditmap["vr"] = "vr"	# vertical rule
	ditmap["union"] = "cu"
	ditmap["universal"] = "fa"
	ditmap["upsilon"] = "*u"
	ditmap["xi"] = "*c"
	ditmap["yen"] = "yn"
 	ditmap["yen"] = "yi yn $J"
	ditmap["zeta"] = "*z"
	ditmap["circle"] = "ci"
	ditmap["square"] = "sq"

# Additional troff characters.  The mod to add these special
# characters to the bdf character set is needed.

	ditmap["box"] = "bx"
	ditmap["boxrule"] = "br"
	ditmap["rule"] = "ru"
	ditmap["boldvertical"] = "bv"
	ditmap["lefttopcurly"] = "lt"
	ditmap["leftbotcurly"] = "lb"
	ditmap["righttopcurly"] = "rt"
	ditmap["rightbotcurly"] = "rb"
# 	ditmap["lt"] = "lt"	# left top curly
# 	ditmap["ob"] = "ob"	# outline bullet
# 	ditmap["rb"] = "rb"	# right bot curly
# 	ditmap["rc"] = "rc"	# right ceil
# 	ditmap["rf"] = "rf"	# right floor
# 	ditmap["rk"] = "rk"	# right center curly
# 	ditmap["rn"] = "rn"	# root extender
# 	ditmap["rt"] = "rt"	# rith top curly
# 	ditmap["ru"] = "ru"	# rule
	ditmap["leftmidcurly"] = "lk"
	ditmap["rightmidcurly"] = "rk"
	ditmap["leftfloor"] = "lf"
	ditmap["rightfloor"] = "rf"
	ditmap["leftceiling"] = "lc"
	ditmap["rightceiling"] = "rc"
	ditmap["outlinebullet"] = "ob"
 	ditmap["perpendicular"] = "bt"
 	ditmap["perthousand"] = "pm"

#  Characters in the ISO Latan 1 set
#  Assignment of troff characters made by D. A. Cahlander 89/01/09.
#  Disabled since the DESC.out file can only describe 256-character fonts.

#	ditmap["Aacute"] = "A'"
#	ditmap["Acircumflex"] = "A^"
	ditmap["Adieresis"] = "A:"
#	ditmap["Agrave"] = "A`"
#	ditmap["Aring"] = "Ao"
#	ditmap["Atilde"] = "A~"
#	ditmap["Ccedilla"] = "Cc"
#	ditmap["Eacute"] = "E'"
#	ditmap["Ecircumflex"] = "E^"
#	ditmap["Edieresis"] = "E."
#	ditmap["Egrave"] = "E`"
#	ditmap["Iacute"] = "I'"
#	ditmap["Icircumflex"] = "I^"
#	ditmap["Idieresis"] = "I."
#	ditmap["Igrave"] = "I`"
#	ditmap["Ntilde"] = "N~"
#	ditmap["Oacute"] = "O'"
#	ditmap["Ocircumflex"] = "O^"
	ditmap["Odieresis"] = "O:"
#	ditmap["Ograve"] = "O`"
#	ditmap["Otilde"] = "O~"
#	ditmap["Scaron"] = "Sc"
#	ditmap["Uacute"] = "U'"
#	ditmap["Ucircumflex"] = "U^"
	ditmap["Udieresis"] = "U:"
#	ditmap["Ugrave"] = "U`"
#	ditmap["Ydieresis"] = "Y."
#	ditmap["Zcaron"] = "Zc"
#	ditmap["aacute"] = "a'"
#	ditmap["acircumflex"] = "a^"
	ditmap["adieresis"] = "a:"
#	ditmap["agrave"] = "a`"
#	ditmap["aring"] = "ao"
#	ditmap["atilde"] = "a~"
#	ditmap["ccedilla"] = "cc"
#	ditmap["eacute"] = "e'"
#	ditmap["ecircumflex"] = "e^"
#	ditmap["edieresis"] = "e."
#	ditmap["egrave"] = "e`"
#	ditmap["iacute"] = "i'"
#	ditmap["icircumflex"] = "i^"
#	ditmap["idieresis"] = "i."
#	ditmap["igrave"] = "i`"
#	ditmap["ntilde"] = "n~"
#	ditmap["oacute"] = "o'"
#	ditmap["ocircumflex"] = "o^"
	ditmap["odieresis"] = "o:"
#	ditmap["ograve"] = "o`"
#	ditmap["otilde"] = "o~"
#	ditmap["scaron"] = "sc"
#	ditmap["uacute"] = "u'"
#	ditmap["ucircumflex"] = "u^"
	ditmap["udieresis"] = "u:"
#	ditmap["ugrave"] = "u`"
#	ditmap["ydieresis"] = "y."
#	ditmap["zcaron"] = "zc"

# Characters that have subroutine proceedures that are used
# to draw them with psdit.

	psdit["onequarter"] = "14"
	psdit["onehalf"] = "12"
	psdit["threequarters"] = "34"

	psdit["square"] = 1
	psdit["box"] = 1
	psdit["circle"] = 1
	psdit["boxrule"] = 1
	psdit["boldvertical"] = 1
	psdit["rule"] = 1

	psdit["lefttopcurly"] = 1
	psdit["leftbotcurly"] = 1
	psdit["righttopcurly"] = 1
	psdit["rightbotcurly"] = 1
	psdit["leftmidcurly"] = 1
	psdit["rightmidcurly"] = 1
	psdit["leftfloor"] = 1
	psdit["rightfloor"] = 1
	psdit["leftceiling"] = 1
	psdit["rightceiling"] = 1

# No mapping exists for these characters.

	ditmap[".noPS1"] = "ma fe"	# male female
	ditmap[".noPS2"] = "~~" 	# almost exactly equals
	ditmap[".noPS3"] = ">< <> !> !<"	# funny relations
	ditmap[".noPS4"] = "uj"		# dotless (undotted) j
	ditmap[".noPS5"] = "cp"		# cap (upside down breve)
	ditmap[".noPS6"] = "-+"		# - over + ?
	ditmap[".noPS7"] = "vr"		# vertical rule 

#	ditmap[".noPS90"] = "wi tp sg sa rd ra os =p =. 4d"
#	ditmap[".noPS91"] = "le ld hi gp ge el eg dc"
#	ditmap[".noPS92"] = "cy bc b9 b0 as an ai \\` \\_ \\'"
#	ditmap[".noPS93"] = "UT Sl SQ RC RB LE GE EL EG DT CB *V"

# L. and l. are used for line drawing on systems without graphics

# ascii is a mapping which contains the troff character names
#	for the printable ascii characters.  The values are the ascii
#	character codes but are not used in this program.  A test is
#	made to see if a specific name is in the table.

	ascii["space"] = 32
	ascii["exclam"] = 33
	ascii["quotedbl"] = 34
	ascii["numbersign"] = 35
	ascii["dollar"] = 36
	ascii["percent"] = 37
	ascii["ampersand"] = 38
	ascii["quoteright"] = 39
	ascii["parenleft"] = 40
	ascii["parenright"] = 41
	ascii["asterisk"] = 42
	ascii["plus"] = 43
	ascii["comma"] = 44
	ascii["hyphen"] = 45
#	ascii["minus"] = 45
	ascii["period"] = 46
	ascii["slash"] = 47
	ascii["zero"] = 48
	ascii["one"] = 49
	ascii["two"] = 50
	ascii["three"] = 51
	ascii["four"] = 52
	ascii["five"] = 53
	ascii["six"] = 54
	ascii["seven"] = 55
	ascii["eight"] = 56
	ascii["nine"] = 57
	ascii["colon"] = 58
	ascii["semicolon"] = 59
	ascii["less"] = 60
	ascii["equal"] = 61
	ascii["greater"] = 62
	ascii["question"] = 63
	ascii["at"] = 64
	ascii["A"] = 65
	ascii["B"] = 66
	ascii["C"] = 67
	ascii["D"] = 68
	ascii["E"] = 69
	ascii["F"] = 70
	ascii["G"] = 71
	ascii["H"] = 72
	ascii["I"] = 73
	ascii["J"] = 74
	ascii["K"] = 75
	ascii["L"] = 76
	ascii["M"] = 77
	ascii["N"] = 78
	ascii["O"] = 79
	ascii["P"] = 80
	ascii["Q"] = 81
	ascii["R"] = 82
	ascii["S"] = 83
	ascii["T"] = 84
	ascii["U"] = 85
	ascii["V"] = 86
	ascii["W"] = 87
	ascii["X"] = 88
	ascii["Y"] = 89
	ascii["Z"] = 90
	ascii["bracketleft"] = 91
	ascii["backslash"] = 92
	ascii["bracketright"] = 93
#	ascii["asciicircum"] = 94	# 195 see ditmap, should be ascii!
	ascii["underscore"] = 95
	ascii["quoteleft"] = 96
	ascii["a"] = 97
	ascii["b"] = 98
	ascii["c"] = 99
	ascii["d"] = 100
	ascii["e"] = 101
	ascii["f"] = 102
	ascii["g"] = 103
	ascii["h"] = 104
	ascii["i"] = 105
	ascii["j"] = 106
	ascii["k"] = 107
	ascii["l"] = 108
	ascii["m"] = 109
	ascii["n"] = 110
	ascii["o"] = 111
	ascii["p"] = 112
	ascii["q"] = 113
	ascii["r"] = 114
	ascii["s"] = 115
	ascii["t"] = 116
	ascii["u"] = 117
	ascii["v"] = 118
	ascii["w"] = 119
	ascii["x"] = 120
	ascii["y"] = 121
	ascii["z"] = 122
	ascii["braceleft"] = 123
	ascii["bar"] = 124
	ascii["braceright"] = 125
#	ascii["asciitilde"] = 126	# 196 see ditmap, should be ascii !

	ditmap[".proctext"] = "18 38 58 78 13 23"

	# character-code and width info for synthetic characters

	cc = 132	# manufacture unique character codes
	proc["18"] = cc++ " 0 833"	# 1/8
	proc["38"] = cc++ " 0 833"	# 3/8
	proc["58"] = cc++ " 0 833"	# 5/8
	proc["78"] = cc++ " 0 833"	# 7/8
	proc["13"] = cc++ " 0 833"	# 1/3
	proc["23"] = cc++ " 0 833"	# 2/3
} # BEGIN

# The following dumps the character set defined above to standard
# output.  It is triggared if the input file is named "DESC".
# This info is used in the device description DESC to describe the
# global (over all fonts) character set of the device.
# Note that this dumps the information in random order (awk's
# enumeration of ditmap), but troff (makedev) shouldn't care.  This
# is the only place the resolution, font sizes, unitwidth, etc. are
# wired.  If they get too large, troff will explode, so change them
# at your own risk.

FILENAME == "DESC" {
	SIZES = "6 8 10 12 14 18 24"
	RES = 72 * 8
	print "sizes", SIZES, 0
	print "res", RES
	print "hor 1"
	print "vert 1"
	print "unitwidth", int((1000.0 / SCALE) * (72.0 / RES))
	print "charset"
	outstr = ""
	for (i in ditmap) {
		n = ditmap[i]
		if ((length(outstr)+length(n)) > 60) {
			print outstr
			outstr = n " "
		} else {
			outstr = outstr n " "
		}
	}
	if (length(outstr) != 0) print outstr
	exit
}

# Define the actions on the fields in the BDF format.
#	Note that 3 files are generated which are cat'ed together
#	outside this program.

# handle header information

/^STARTFONT/ {
	print "#", $0 > header
	next
}

/^COMMENT Begin LogicalFontDescription/ {next}
/^COMMENT END LogicalFontDescription/ {next}
/^COMMENT .* end of inserted font properties/	{next}

/^COMMENT/ {
	$1 = "# "
	print > header
	next
} 

/^FONT / {
	print "#  " $2 > header
	split($0, arg, "-")
	print "name", font[arg[3] "-" arg[4] "-" arg[5]] > header
	if (arg[2] == "S") {
		print "special" > header
		isspecial = 1
		istext = 0
	}
	next
}

/^X_HEIGHT / {
	ascender = $2
	next
}

/^ENDPROPERTIES/ { next }

# set character name

/^STARTCHAR .*/ {
	charname = $2
	next
}

# set encoding

/^ENCODING .*/ {
	charcode = $2
	next
}

/^SWIDTH [0-9]* [0-9]*/ {
	width = $2
	next
}

/^DWIDTH [0-9]* [0-9]*/ {
	dwidth = $2
	next
}

# 1   2       3       4       5
/^BBX [-0-9]* [-0-9]* [-0-9]* [-0-9]*/ {
	if (charcode == -1)			# unused character
		next

	bblly = $5
	bbury = $3 + $5

#	ignore out of range characters
	if (charcode >= 256)
		next

#	compute width
	scaledwidth = int(0.5 + (width / SCALE))
	if ((scaledwidth < 0) || (scaledwidth > 256)) {
		print "ERROR! Width out of range!"
		exit 2
	}

#	handle space
	if (charname == "space") {		# special char widths
		spacewidth = scaledwidth
		printf "ligatures " > header
		printf "spacewidth %d\n", spacewidth > spaces
		print "charset" > spaces
		printf "\\|\t%d 0 000\t1/6 em space\n", int (0.5 + (1000.0/6.0) / SCALE) > trailer
		printf "\\^\t%d 0 000\t1/12 em space\n", int (0.5 + (1000.0/12.0) / SCALE) > trailer
		printf "\\&\t00 0 000\tno space\n" > trailer
		next
	}

#	parse ligatures
	if (length(ligature[charname]))
		printf "%s ", charname > header

#	ascender/descender flags
	ad = 0
	if (bblly < 0) ad += 1
	if (bbury > ascender) ad += 2

#	dump the description line
	if (length(ascii[charname]) > 0) {
		printf "%c\t%d %d 0%2o\t%s\n", charcode, scaledwidth, ad, charcode, charname  > trailer
		# look for ditmap synonyms
		if (length(ditmap[charname]) > 0) {
			n = split(ditmap[charname],others," ")
			for (i = 1; i <= n; i++) {
				oth = others[i];
				if ((length(mathonly[oth]) > 0) && (isspecial != 1)) continue;
				printf "%s\t\"\n", others[i]  > trailer
			}
		}
	} else if (length(ditmap[charname]) > 0) {
		# not a printable ascii character
		n = split(ditmap[charname],others," ")
		printf "%s\t%d %d 0%o\t%s\n", others[1], scaledwidth, ad, charcode, charname  > trailer
		for (i = 2; i <= n; i++) {
			printf "%s\t\"\n", others[i]  > trailer
		}
	}

# dump a line for PS specific actual width/hack table

	specialcode = 0
	if (length(psdit[charname]) != 0)
		specialcode = 1
	printf "%d %d %d\n", charcode, width, specialcode > aux
	next
}

END	{
	printf "0\n" > header

	# dump the "fudged" characters.
	for (i in proc) {
		p = proc[i]
		split(p,vals," ")
		scaledwidth = int(0.5 + (vals[3] / SCALE))
		if (((istext == 1) && (index(ditmap[".proctext"],i) != 0)) || ((isspecial == 1) && (index(ditmap[".procspecial"],i) != 0))) {
			printf "%s\t%d %d 0%o\tfudgedproc!\n", i, scaledwidth, vals[2], vals[1] > trailer

			printf "%d %d %d\n", vals[1], vals[3], 1 > aux
		}
	}
	exit

}
