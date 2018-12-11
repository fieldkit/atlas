include(Arduino)

enable_arduino_toolchain()

file(GLOB sources *.cpp ../common/*.cpp)

find_package(FkModule)

add_arduino_library(fk-atlas-test "${sources}")

target_include_directories(fk-atlas-test PRIVATE "../common")

fk_configure_module(fk-atlas-test)

add_arduino_firmware(fk-atlas-test)
