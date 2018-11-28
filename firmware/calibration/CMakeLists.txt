include(Arduino)

enable_arduino_toolchain()

set(sources main.cpp)

find_package(FkModule)

add_arduino_library(fk-atlas-calibration "${sources}")

fk_configure_module(fk-atlas-calibration)

add_arduino_firmware(fk-atlas-calibration)
