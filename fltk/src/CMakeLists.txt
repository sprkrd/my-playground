
#add_executable(hello WIN32 MACOSX_BUNDLE hello.cpp)
#if (APPLE)
#  target_link_libraries(hello "-framework cocoa")
#endif (APPLE)
#target_include_directories(hello PUBLIC ${FLTK_INCLUDE_DIRS})

add_executable(hello hello.cpp)
target_link_libraries(hello fltk)

