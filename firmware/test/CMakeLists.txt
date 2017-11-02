project(ARDUINO_PROJECTS)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_VERBOSE_MAKEFILE on)

set(ARDUINO_IDE "${CMAKE_CURRENT_SOURCE_DIR}/../../../arduino-1.8.3")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../../../arduino-cmake-minimal/cmake/")

include(ArduinoMinimal)

set(SOURCE_FILES
  test.ino
)

set(LIBRARIES
  Wire
)

include_directories(../../)

arduino(fk-aatlas-test "${SOURCE_FILES}" "${LIBRARIES}")
