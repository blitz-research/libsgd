cmake_minimum_required(VERSION 3.16)

##### audio tests #####

project(audio_tests)

add_executable(audio_tests audio_tests.cpp)

target_link_libraries(audio_tests core audio)
