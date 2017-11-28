project(ATLAS)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_VERBOSE_MAKEFILE on)

set(ARDUINO_IDE "${CMAKE_CURRENT_SOURCE_DIR}/../arduino-1.8.3") 
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../arduino-cmake-minimal/cmake/")

include(ArduinoMinimal)

set(EXTRA_CXX_FLAGS_PROJECT "-Wall -Werror")
set(Adafruit_ZeroDMA_RECURSE True)

add_subdirectory(firmware/module)
add_subdirectory(firmware/test)
