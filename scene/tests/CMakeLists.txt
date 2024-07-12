cmake_minimum_required(VERSION 3.16)

##### scene tests #####

project(scene_tests)

add_executable(scene_tests scene_tests.cpp)
target_link_libraries(scene_tests scene)

add_executable(scene_tests_model scene_tests_model.cpp)
target_link_libraries(scene_tests_model scene)

add_executable(scene_tests_overlay scene_tests_overlay.cpp)
target_link_libraries(scene_tests_overlay scene)
