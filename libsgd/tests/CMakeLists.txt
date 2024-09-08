cmake_minimum_required(VERSION 3.16)

# Convert these to scene tests!

##### libsgd_tests #####

project(libsgd_tests)

function(add name)
	add_executable(${name} ${name}.c)
	target_link_libraries(${name} libsgd)
endfunction()

add(libsgd_tests_directionallight)
add(libsgd_tests_pointlight)
add(libsgd_tests_spotlight)
add(libsgd_tests_createmesh)
add(libsgd_tests_scratch)
add(libsgd_tests_fogbug)

add_executable(libsgd_tests_imgui libsgd_tests_imgui.cpp)
target_link_libraries(libsgd_tests_imgui imgui-core libsgd_dynamic)
