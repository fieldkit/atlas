project(fk-atlas)
cmake_minimum_required(VERSION 3.5)

set(CMAKE_MODULE_PATH $ENV{CMAKE_MODULE_PATH})

add_subdirectory(firmware/module)
add_subdirectory(firmware/calibration)
add_subdirectory(firmware/test)
