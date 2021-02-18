\begindata{text,269207760}
\textdsversion{12}
\template{default}
\define{sans
menu:[Font,Sans]
attr:[FontFamily AndySans Int 0]}


\center{\bigger{\bigger{\italic{\bigger{\bigger{\bigger{Happy Birthday}}}}}}}


\begindata{cel,269235776}
\V 2
\begindata{lset,269220812}
\V 1
\begindata{lset,269223560}
\V 1
\begindata{cel,269220612}
\V 2
\begindata{raster,269220476}
2 0 68266 68266 0 0 112 109
bits 269220476 112 109
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
l08m |
l08m |
l7em |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
k03e3e0l |
j03Ge3Gf0k |
jGc062g3fe0j |
i1fc0g62h7ej |
h01fch62h07c0	i |
h0780h62if0i |
h0cie3i1ci |
h18h016280h06	i |
h10h016280h03	i |
h38h018180h01	i |
h3ciGi0180h |
h36i2ai0380h |
h33m0680h |
h30c0l0880h |
h3070l3080h |
h301cle080h |
h300780j03!80	h |
h30gf8j1eg80h |
h30g1f80h01f8	g80h |
h30g01f8hbf40	g80h |
h30h3fHf8h80h |
h30h027ff2i80	h |
h30n80h |
h30n80h |
h30n80h |
h30n80h |
h30n80h |
h38n80h |
h3cm0180h |
h33m0380h |
h3180l0680h |
h30e0l0880h |
h303cl3080h |
h3007le080h |
h3001c0j03!80	h |
h30g78j1eg80h |
h30g0f80h03f8	g80h |
h30g01f8h7fh80	h |
h30h3fHf8h80h |
h30h037ffai80	h |
h30n80h |
h10n80h |
h18m0180h |
h0cm01i |
h07m03i |
h0180l06i |
ie0l08i |
i38l30i |
i0ele0i |
i03c0j0380i |
j78j16j |
j0e80h01f8j |
j01d0h5740j |
k3fHfck |
k016facl |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
\enddata{raster, 269220476}
0 269220476 0 0 0 0 
>OBJ< raster
>VIEW< rasterview
>REF< visible cake
>LINK< 4ÿÀÄ
\enddata{cel,269220612}
0 0 0 269220612 0 0 0
>OBJ< cel
>VIEW< celview
>REF< 
\enddata{lset,269223560}
\begindata{lset,269221040}
\V 1
\begindata{cel,269261840}
\V 2
\begindata{text,269245412}
\textdsversion{12}
\template{default}
\italic{\bigger{\bigger{\bigger{ 

}}}}\enddata{text,269245412}
0 269245412 0 0 0 0 
>OBJ< text
>VIEW< textview
>REF< song text
>LINK< cake_1
\enddata{cel,269261840}
0 0 0 269261840 0 0 0
>OBJ< cel
>VIEW< celview
>REF< 
\enddata{lset,269221040}
1 66 0 0 269223560 269221040 0
>OBJ< 
>VIEW< 
>REF< 
\enddata{lset,269220812}
0 269220812 0 0 0 0 
>OBJ< lset
>VIEW< lsetview
>REF< lset
>LINK< 
\enddata{cel,269235776}
\view{celview,269235776,0,455,160}


\italic{Empower the Ness (below) and click on the cake for Birthday greetings. 
 

For best results, use an IBM RT/PC workstation.}


\begindata{cel,269299000}
\V 2
\begindata{ness,269298656}
\origin{00\13 Oct 1989 at 10:01:56 EDT\wjh:  Fred Hansen\00}
\template{default}
\define{fullwidth
menu:[Justify,Full Width]
attr:[LeftMargin LeftMargin Cm -25431]
attr:[RightMargin RightMargin Cm -27743]}
\define{sans
menu:[Font,Sans]
attr:[FontFamily AndySans Int 0]}


-- \bigger{\italic{Happy Birthday to Ness}} ! 


 

function showcake(object cake)

	raster_copy_subraster(cake)

	raster_select_entire(inset("visible cake"))

	raster_replace_subraster(inset("visible cake"))

	raster_center_image(inset("visible cake"))

end function


boolean lit:= False	-- the file is saved with the unlit cake above


marker Cakes := 
"\
\begindata{raster,269298496}
2 0 68266 68266 0 0 112 109
bits 269298496 112 109
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
l08m |
l08m |
l7em |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
k03e3e0l |
j03Ge3Gf0k |
jGc062g3fe0j |
i1fc0g62h7ej |
h01fch62h07c0	i |
h0780h62if0i |
h0cie3i1ci |
h18h016280h06	i |
h10h016280h03	i |
h38h018180h01	i |
h3ciGi0180h |
h36i2ai0380h |
h33m0680h |
h30c0l0880h |
h3070l3080h |
h301cle080h |
h300780j03!80	h |
h30gf8j1eg80h |
h30g1f80h01f8	g80h |
h30g01f8hbf40	g80h |
h30h3fHf8h80h |
h30h027ff2i80	h |
h30n80h |
h30n80h |
h30n80h |
h30n80h |
h30n80h |
h38n80h |
h3cm0180h |
h33m0380h |
h3180l0680h |
h30e0l0880h |
h303cl3080h |
h3007le080h |
h3001c0j03!80	h |
h30g78j1eg80h |
h30g0f80h03f8	g80h |
h30g01f8h7fh80	h |
h30h3fHf8h80h |
h30h037ffai80	h |
h30n80h |
h10n80h |
h18m0180h |
h0cm01i |
h07m03i |
h0180l06i |
ie0l08i |
i38l30i |
i0ele0i |
i03c0j0380i |
j78j16j |
j0e80h01f8j |
j01d0h5740j |
k3fHfck |
k016facl |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
\enddata{raster, 269298496}
\view{rasterview,269298496,1,0,0} 
 \
\begindata{raster,269286484}
2 0 68266 68266 0 0 112 109
bits 269286484 112 109
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
l38m |
l16m |
l09m |
l0480l |
l0480l |
l0a40l |
l0a40l |
l1240l |
l2240l |
l4440l |
l4480l |
l8880l |
l89m |
l92m |
l92m |
l54m |
l4cm |
l28m |
l18m |
l14m |
l7em |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
k03e3e0l |
j03Ge3Gf0k |
jGc062g3fe0j |
i1fc0g62h7ej |
h01fch62h07c0	i |
h0780h62if0i |
h0cie3i1ci |
h18h016280h06	i |
h10h016280h03	i |
h38h018180h01	i |
h3ciGi0180h |
h36i2ai0380h |
h33m0680h |
h30c0l0880h |
h3070l3080h |
h301cle080h |
h300780j03!80	h |
h30gf8j1eg80h |
h30g1f80h01f8	g80h |
h30g01f8hbf40	g80h |
h30h3fHf8h80h |
h30h027ff2i80	h |
h30n80h |
h30n80h |
h30n80h |
h30n80h |
h30n80h |
h38n80h |
h3cm0180h |
h33m0380h |
h3180l0680h |
h30e0l0880h |
h303cl3080h |
h3007le080h |
h3001c0j03!80	h |
h30g78j1eg80h |
h30g0f80h03f8	g80h |
h30g01f8h7fh80	h |
h30h3fHf8h80h |
h30h037ffai80	h |
h30n80h |
h10n80h |
h18m0180h |
h0cm01i |
h07m03i |
h0180l06i |
ie0l08i |
i38l30i |
i0ele0i |
i03c0j0380i |
j78j16j |
j0e80h01f8j |
j01d0h5740j |
k3fHfck |
k016facl |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
\enddata{raster, 269286484}
\view{rasterview,269286484,2,0,0} "



extend "visible cake"   on mouse "any"

	if mouseaction = mouseleftup then

		lit := not lit

		if lit then

			showcake(FirstObject(second(Cakes)))

			sing()

		else

			showcake(FirstObject(Cakes))

			replace(base(currentselection(inset("song text"))), " \\n")

		end if

	end if

end mouse   end extend


function sing()

	marker m

	m := last(base(currentselection(inset("song text"))))

	m := last(replace(m, "\\nHappy birthday to you\\n"))

	im_ForceUpdate()

	play_notes("L7 CC L4 DCF    E P4")

	m := last(replace(m, "\\nHappy birthday to you\\n"))

	play_notes("L7 CC L4 DCG   F P4")

	im_ForceUpdate()

	m := last(replace(m, "\\nHappy birthday,\\n\\tDear Nessie\\n"))

	play_notes("L7 CC L4 >C <A  FED P4")

	im_ForceUpdate()

	m := last(replace(m, "\\nHappy birthday to you\\n"))

	play_notes("L7 A#A# L4 AF G  F.") 

end function\
\enddata{ness,269298656}
0 269298656 0 0 0 0 
>OBJ< ness
>VIEW< nessview
>REF< ness
>LINK< 
\enddata{cel,269299000}
\view{celview,269299000,3,528,193}

\enddata{text,269207760}
