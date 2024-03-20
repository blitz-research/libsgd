cmake_minimum_required(VERSION 3.16)

##### libsgd_tests #####

project(libsgd_tests)

add_executable(libsgd_tests libsgd_tests.c)

function(add name)
	add_executable(${name} ${name}.c)
	target_link_libraries(${name} libsgd)
endfunction()

add(libsgd_tests_skinnedmesh)
add(libsgd_tests_directionallight)
add(libsgd_tests_spotlight)
