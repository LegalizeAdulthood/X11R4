XcuWlm
    {
    *Command.cursor "dot"
    *Command.shadow "True"
    XcuRc
	{
	formatString "2 x 2"   /* equivalent to XcuTbl "c c." */
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

