cmake_minimum_required(VERSION 3.16)

##### graphics tests #####

project(graphics_tests)

add_executable(graphics_tests graphics_tests.cpp)
target_link_libraries(graphics_tests graphics)
copy_assets_to_binary_dir(graphics_tests)

add_executable(scene_tests scene_tests.cpp)
target_link_libraries(scene_tests graphics)
copy_assets_to_binary_dir(scene_tests)
