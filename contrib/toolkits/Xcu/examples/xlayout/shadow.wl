XcuWlm
    {
    *dimFacetColor "SteelBlue"
    *brightFacetColor "LightSteelBlue"
    *facetWidth "5"
    *Command.background "white"
    *Command.foreground "black"
    *Command.cursor "dot"
    XcuRc
	{
	formatString "2 x 2"   /* equivalent to XcuTbl "c c." */
	background "LightGray"
	internalWidth "20"
	internalHeight "20"
	interWidth "10"
	interHeight "10"

	XcuCommand { borderWidth "0" name "INSENSITIVE" sensitive "False" }
	XcuCommand { name "Sensitive" sensitive "True" }
	XcuCommand { name "Facets" shadow "False" }
	XcuCommand { name "Shadow" dimFacetColor "black" shadow "True" }
	}
    }

