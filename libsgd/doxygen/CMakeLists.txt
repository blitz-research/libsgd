cmake_minimum_required(VERSION 3.16)

project(libsgd_doxygen)

find_package(Doxygen)

if (DOXYGEN_FOUND)

	# Configure Doxyfile
	set(DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile ${DOXYFILE})

	# note the option ALL which allows to build the docs together with the application
	add_custom_target(libsgd_doxygen ALL
		COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		VERBATIM )

	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION .)

else()
	message(WARNING "Can't find doxygen, skipping docs.")
endif()
