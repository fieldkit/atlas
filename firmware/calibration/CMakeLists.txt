include(Arduino)

enable_arduino_toolchain()

file(GLOB sources *.cpp ../common/*.cpp)

find_package(FkModule)

add_arduino_library(fk-atlas-calibration "${sources}")

target_include_directories(fk-atlas-calibration PRIVATE "../common")

fk_configure_module(fk-atlas-calibration)

add_arduino_firmware(fk-atlas-calibration)
