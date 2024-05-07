
#add_executable(hello WIN32 MACOSX_BUNDLE hello.cpp)
#if (APPLE)
#  target_link_libraries(hello "-framework cocoa")
#endif (APPLE)
#target_include_directories(hello PUBLIC ${FLTK_INCLUDE_DIRS})

add_executable(hello hello.cpp)
target_link_libraries(hello fltk::fltk-shared)

add_executable(board board.cpp)
target_link_libraries(board fltk::fltk-shared)

add_executable(resize resize.cpp)
target_link_libraries(resize fltk::fltk-shared)

add_executable(pack pack.cpp)
target_link_libraries(pack fltk::fltk-shared)

add_library(iri_board_gui SHARED game_gui.cpp)
target_link_libraries(iri_board_gui fltk::fltk-shared)

add_executable(game_gui_test game_gui_test.cpp)
target_link_libraries(game_gui_test iri_board_gui)
