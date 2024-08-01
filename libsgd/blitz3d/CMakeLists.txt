cmake_minimum_required(VERSION 3.16)

project(libsgd_blitz3d)

if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(WARNING "### Blitz3D target not available in 64 bit builds, skipping.")
	return()
endif ()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/helloworld.bb DESTINATION blitz3d)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/sgd.bb DESTINATION blitz3d)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/keycodes.bb DESTINATION blitz3d)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples DESTINATION blitz3d FILES_MATCHING PATTERN "*.bb")
