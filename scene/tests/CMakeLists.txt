cmake_minimum_required(VERSION 3.16)

##### scene tests #####

project(scene_tests)

add_executable(scene_tests_scratch scene_tests_scratch.cpp)
target_link_libraries(scene_tests_scratch scene)

add_executable(scene_tests scene_tests.cpp)
target_link_libraries(scene_tests scene)

add_executable(scene_tests_cannon scene_tests_cannon.cpp)
target_link_libraries(scene_tests_cannon scene)

add_executable(scene_tests_model scene_tests_model.cpp)
target_link_libraries(scene_tests_model scene)

add_executable(scene_tests_bloom scene_tests_bloom.cpp)
target_link_libraries(scene_tests_bloom scene)

add_executable(scene_tests_trees scene_tests_trees.cpp)
target_link_libraries(scene_tests_trees scene)

add_executable(scene_tests_forest scene_tests_forest.cpp)
target_link_libraries(scene_tests_forest scene)

add_executable(scene_tests_skinned_model scene_tests_skinned_model.cpp)
target_link_libraries(scene_tests_skinned_model scene)

add_executable(scene_tests_sprites scene_tests_sprites.cpp)
target_link_libraries(scene_tests_sprites scene)

add_executable(scene_tests_overlay scene_tests_overlay.cpp)
target_link_libraries(scene_tests_overlay scene)

add_executable(scene_tests_collisions scene_tests_collisions.cpp)
target_link_libraries(scene_tests_collisions scene)

add_executable(scene_tests_blendtest scene_tests_blendtest.cpp)
target_link_libraries(scene_tests_blendtest scene)

add_executable(scene_tests_terrain scene_tests_terrain.cpp)
target_link_libraries(scene_tests_terrain scene)

add_executable(scene_tests_serialize scene_tests_serialize.cpp)
target_link_libraries(scene_tests_serialize scene)
