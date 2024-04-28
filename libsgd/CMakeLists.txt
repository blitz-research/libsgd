cmake_minimum_required(VERSION 3.16)

project(libsgd)

# Laters gators!
if (SGD_OS_EMSCRIPTEN)
	return()
endif ()

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
)

add_library(libsgd ${LIBSGD_SOURCES})

target_include_directories(libsgd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(libsgd graphics audio)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/sgd DESTINATION include)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/VERSIONS.txt ${CMAKE_CURRENT_SOURCE_DIR}/README.md DESTINATION .)

##### sgd_static *****

function(create_bundled_lib output_lib output_libname input_lib)

	function(collect_dependencies target)

		if (NOT TARGET ${target})
			return()
		endif ()

		get_target_property(alias ${target} ALIASED_TARGET)
		if (TARGET ${alias})
			set(target ${alias})
		endif ()

		set(done_prop bundled_lib_${output_lib}_${target}_done)
		get_property(done GLOBAL PROPERTY ${done_prop})
		if (done)
			return()
		endif ()
		set_property(GLOBAL PROPERTY ${done_prop} ON)

		get_target_property(_type ${target} TYPE)
		if (${_type} STREQUAL "STATIC_LIBRARY")
			list(APPEND static_libs ${target})
		endif ()

		set(_link_libs_type LINK_LIBRARIES)
		if (${_type} STREQUAL "INTERFACE_LIBRARY")
			set(_link_libs_type INTERFACE_LINK_LIBRARIES)
		endif ()
		get_target_property(target_dependencies ${target} ${_link_libs_type})

		foreach (dependency IN LISTS target_dependencies)
			collect_dependencies(${dependency})
		endforeach ()

		set(static_libs ${static_libs} PARENT_SCOPE)

	endfunction()

	collect_dependencies(${input_lib})

	list(REMOVE_DUPLICATES static_libs)

	# message("### Static lib dependencies: ${static_libs}")

	set(output_file "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${output_libname}${CMAKE_STATIC_LIBRARY_SUFFIX}")

	if (SGD_OS_WINDOWS)

		foreach (target IN LISTS static_libs)
			list(APPEND lib_files $<TARGET_FILE:${target}>)
		endforeach ()

		add_custom_command(
			COMMAND ${CMAKE_AR} /NOLOGO /OUT:${output_file} ${lib_files}
			OUTPUT ${output_file}
			DEPENDS ${static_libs}
			VERBATIM)

	elseif (SGD_OS_LINUX)

		file(WRITE ${CMAKE_BINARY_DIR}/${output_lib}.ar.in
			"CREATE ${output_file}\n")

		foreach (target IN LISTS static_libs)
			file(APPEND ${CMAKE_BINARY_DIR}/${output_lib}.ar.in
				"ADDLIB $<TARGET_FILE:${target}>\n")
		endforeach ()

		file(APPEND ${CMAKE_BINARY_DIR}/${output_lib}.ar.in "SAVE\n")
		file(APPEND ${CMAKE_BINARY_DIR}/${output_lib}.ar.in "END\n")

		file(GENERATE
			OUTPUT ${CMAKE_BINARY_DIR}/${output_lib}.ar
			INPUT ${CMAKE_BINARY_DIR}/${output_lib}.ar.in)

		add_custom_command(
			COMMAND ${CMAKE_AR} -M < ${CMAKE_BINARY_DIR}/${output_lib}.ar
			OUTPUT ${output_file}
			DEPENDS ${static_libs}
			VERBATIM)

	elseif (SGD_OS_MACOS)

		# Based on:
		#
		# https://stackoverflow.com/questions/8170450/combine-static-libraries-on-apple/8170851#8170851

		foreach (target IN LISTS static_libs)
			list(APPEND lib_files $<TARGET_FILE:${target}>)
		endforeach ()

		add_custom_command(
			COMMAND libtool -static -o ${output_file} ${lib_files}
			OUTPUT ${output_file}
			DEPENDS ${static_libs}
			VERBATIM)

	else ()

		message(FATAL_ERROR "***** TODO *****")

	endif ()

	add_custom_target(${output_lib} ALL DEPENDS ${output_file})

endfunction()

create_bundled_lib(libsgd_static sgd_static libsgd)

##### libsgd_dynamic #####

add_library(libsgd_dynamic SHARED ${LIBSGD_SOURCES} imguibackend.cpp)

set_target_properties(libsgd_dynamic PROPERTIES OUTPUT_NAME sgd_dynamic)

target_include_directories(libsgd_dynamic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(libsgd_dynamic PUBLIC SGD_DYNAMIC=1 SGD_EXPORT=1)

target_link_libraries(libsgd_dynamic graphics audio imgui-backends)

if (SGD_OS_LINUX)
	target_link_options(libsgd_dynamic PRIVATE -fPIC)
endif ()

install(TARGETS libsgd_dynamic DESTINATION lib)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/helloworld.c DESTINATION lib)

##### More libsgds! #####

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/blitz3d/examples DESTINATION blitz3d FILES_MATCHING PATTERN "*.bb")

if (SGD_BLITZMAX_TARGET_ENABLED)
    add_subdirectory(blitzmax)
endif ()

if (SGD_NODEJS_TARGET_ENABLED)
	add_subdirectory(nodejs)
endif ()

if (SGD_PYTHON_TARGET_ENABLED)
	add_subdirectory(python)
endif ()

##### Doxygen #####

add_subdirectory(doxygen)

##### Tests #####

add_subdirectory(tests)
