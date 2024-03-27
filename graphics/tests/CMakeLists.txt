cmake_minimum_required(VERSION 3.16)

##### graphics tests #####

project(graphics_tests)

add_executable(graphics_tests graphics_tests.cpp)
target_link_libraries(graphics_tests graphics)

add_executable(scene_tests scene_tests.cpp)
target_link_libraries(scene_tests graphics)

add_executable(scene_tests_sprites scene_tests_sprites.cpp)
target_link_libraries(scene_tests_sprites graphics)

add_executable(parent_tests parent_tests.cpp)
target_link_libraries(parent_tests graphics)
