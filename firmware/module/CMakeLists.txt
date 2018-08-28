include(Arduino)

enable_arduino_toolchain()

file(GLOB sources *.cpp)

find_package(FkModule)
find_package(atlas-protocol)
find_package(SparkFun_MS5803)

add_arduino_library(fk-atlas-standard "${sources}")

fk_configure_module(fk-atlas-standard)

target_link_libraries(fk-atlas-standard atlas-protocol)
target_link_libraries(fk-atlas-standard SparkFun_MS5803-14BA_Breakout_Arduino_Library)

option(FK_MODULE_WIRE11AND13 "Use TwoWire11And13 for Module to Core communications." OFF)
if(FK_MODULE_WIRE11AND13)
  target_compile_options(fk-atlas-standard PUBLIC -DFK_HARDWARE_WIRE11AND13_ENABLE)
  target_compile_options(firmware-common PUBLIC -DFK_HARDWARE_WIRE11AND13_ENABLE)

  target_compile_options(fk-atlas-standard PUBLIC -DFK_MODULE_WIRE11AND13)
  target_compile_options(firmware-common PUBLIC -DFK_MODULE_WIRE11AND13)
endif(FK_MODULE_WIRE11AND13)

option(FK_ENABLE_ATLAS_ORP "Enable/disable ORP sensor." ON)
if(FK_ENABLE_ATLAS_ORP)
  target_compile_options(fk-atlas-standard PUBLIC -DFK_ENABLE_ATLAS_ORP)
else()
  target_compile_options(fk-atlas-standard PUBLIC -DFK_DISABLE_ATLAS_ORP)
endif(FK_ENABLE_ATLAS_ORP)

option(FK_ENABLE_MS5803 "Enable/disable MS5803 sensor." OFF)
if(FK_ENABLE_MS5803)
  target_compile_options(fk-atlas-standard PUBLIC -DFK_ENABLE_MS5803)
else()
  target_compile_options(fk-atlas-standard PUBLIC -DFK_DISABLE_MS5803)
endif(FK_ENABLE_MS5803)

option(FK_PROFILE_AMAZON "Amazon Profile" ON)
if(FK_PROFILE_AMAZON)
  add_arduino_library(fk-atlas-amazon "${sources}")

  fk_configure_module(fk-atlas-amazon)

  target_link_libraries(fk-atlas-amazon atlas-protocol)
  target_link_libraries(fk-atlas-amazon SparkFun_MS5803-14BA_Breakout_Arduino_Library)

  target_compile_options(fk-atlas-amazon PUBLIC -DFK_PROFILE_AMAZON)
  target_compile_options(fk-atlas-amazon PUBLIC -DFK_ENABLE_MS5803)
  target_compile_options(fk-atlas-amazon PUBLIC -DFK_DISABLE_ATLAS_ORP)
  if(FK_MODULE_WIRE11AND13)
    target_compile_options(fk-atlas-amazon PUBLIC -DFK_HARDWARE_WIRE11AND13_ENABLE)
    target_compile_options(fk-atlas-amazon PUBLIC -DFK_MODULE_WIRE11AND13)
  endif(FK_MODULE_WIRE11AND13)

  add_arduino_firmware(fk-atlas-amazon)
endif(FK_PROFILE_AMAZON)

add_arduino_firmware(fk-atlas-standard)
