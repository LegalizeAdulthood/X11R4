add_executable(logo demo.c printer.c ramtek.c ethernet.c s9000.c)
target_link_libraries(logo PUBLIC xgks)
target_include_directories(logo PUBLIC ..)
