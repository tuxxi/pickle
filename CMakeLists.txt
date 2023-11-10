cmake_minimum_required(VERSION 3.25)

project(pickle)

add_executable(pickle
    main.cpp
)


set_target_properties(pickle PROPERTIES 
    # CXX_EXTENSIONS OFF
    CXX_STANDARD 20)
