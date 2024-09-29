cmake_minimum_required(VERSION 3.16)

project(libsgd)

set(SRC ${CMAKE_CURRENT_SOURCE_DIR})
set(BIN ${CMAKE_CURRENT_BINARY_DIR})
set(INC ${CMAKE_SOURCE_DIR}/libsgd/include)

##### libsgd #####

set(LIBSGD_SOURCES
	audio.cpp
	graphics.cpp
	internal.cpp
	internal.h
	scene.cpp
	system.cpp
	window.cpp

	include/sgd/sgd.h
	include/sgd/keycodes.h
)

add_library(libsgd ${LIBSGD_SOURCES})
target_include_directories(libsgd PUBLIC ${INC})
target_link_libraries(libsgd scene audio)

install(DIRECTORY ${INC}/sgd DESTINATION include)
install(FILES ${SRC}/helloworld.c DESTINATION lib)
install(FILES ${SRC}/VERSIONS.txt ${SRC}/README.md DESTINATION .)

##### sgd_static *****

include(create_bundled_lib.cmake)

create_bundled_lib(libsgd_static sgd_static libsgd)

##### libsgd_dynamic #####

add_library(libsgd_dynamic SHARED ${LIBSGD_SOURCES} imguibackend.cpp)
set_target_properties(libsgd_dynamic PROPERTIES OUTPUT_NAME sgd_dynamic)
target_include_directories(libsgd_dynamic PUBLIC ${INC})
target_compile_definitions(libsgd_dynamic PUBLIC SGD_DYNAMIC=1 SGD_EXPORT=1)
target_link_libraries(libsgd_dynamic scene audio imgui-backends)

if (SGD_OS_LINUX)
	target_link_options(libsgd_dynamic PRIVATE -fPIC)
endif ()

install(TARGETS libsgd_dynamic DESTINATION lib)

##### More libsgds! #####

if (SGD_BLITZ3D_TARGET_ENABLED)
	add_subdirectory(blitz3d)
endif ()

if (SGD_BLITZMAX_TARGET_ENABLED)
	add_subdirectory(blitzmax)
endif ()

if (SGD_NODEJS_TARGET_ENABLED)
	add_subdirectory(nodejs)
endif ()

if (SGD_PYTHON_TARGET_ENABLED)
	add_subdirectory(python)
endif ()

if (SGD_ZIG_TARGET_ENABLED)
	add_subdirectory(zig)
endif ()

if (SGD_LUA_TARGET_ENABLED)
	add_subdirectory(lua)
endif()

if (SGD_DOCS_TARGET_ENABLED)
	add_subdirectory(docs)
endif ()

##### Tests #####

add_subdirectory(tests)
