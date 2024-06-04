cmake_minimum_required(VERSION 3.16)

##### scene tests #####

project(scene_tests)

add_executable(scene_tests_model scene_tests_model.cpp)
target_link_libraries(scene_tests_model scene)

add_executable(scene_tests_overlay scene_tests_overlay.cpp)
target_link_libraries(scene_tests_overlay scene)

add_executable(scene_tests_sprites scene_tests_sprites.cpp)
target_link_libraries(scene_tests_sprites scene)

add_executable(scene_tests_rayhit scene_tests_rayhit.cpp)
target_link_libraries(scene_tests_rayhit scene)

add_executable(scene_tests_blocks scene_tests_blocks.cpp)
target_link_libraries(scene_tests_blocks scene)

add_executable(scene_tests_collisions scene_tests_collisions.cpp)
target_link_libraries(scene_tests_collisions scene)
