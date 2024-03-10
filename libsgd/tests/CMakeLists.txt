cmake_minimum_required(VERSION 3.16)

##### libsgd_tests #####

project(libsgd_tests)

add_executable(libsgd_tests libsgd_tests.c)

target_link_libraries(libsgd_tests libsgd)
copy_assets_to_binary_dir(libsgd_tests)
