add_executable(mkgksfont mkfont.c)
target_include_directories(mkgksfont PRIVATE ../include)

add_custom_target(fonts)

set(FONTS r.src R.src b.src B.src i.src I.src sc.src SC.src G.src)
foreach(fontSource ${FONTS})
    get_filename_component(font ${fontSource} NAME_WE)
    add_custom_command(OUTPUT ${font}.xgksfont
	COMMAND mkgksfont ${CURRENT_SOURCE_DIR}/${fontSource} ${CURRENT_BINARY_DIR}/${font}.xgksfont
	MAIN_DEPENDENCY ${fontSource})
endforeach()
