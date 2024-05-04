cmake_minimum_required(VERSION 3.25)

project(pickle)

add_executable(pickle
    pickle.cpp
)

set_target_properties(pickle PROPERTIES CXX_STANDARD 20)

add_executable(test test/test_main.cpp
    # user defined tests
    pickle_test.cpp
)
target_include_directories(test PRIVATE ${CMAKE_SOURCE_DIR})

set_target_properties(test PROPERTIES CXX_STANDARD 20)
