file(GLOB SOURCE_FILES *.cpp)
set(LIBRARIES Wire SPI)

# Get the current working branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git log -1 --format=%H
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_definitions("-DFIRMWARE_GIT_HASH=\"${GIT_COMMIT_HASH}\"")
add_definitions("-DFIRMWARE_GIT_BRANCH=\"${GIT_BRANCH}\"")
add_definitions("-DFIRMWARE_BUILD=\"$ENV{BUILD_TAG}\"")

arduino(fk-atlas-standard "${SOURCE_FILES}" "${LIBRARIES}")

option(FK_MODULE_WIRE11AND13 "Use TwoWire11And13 for Module to Core communications." OFF)
if(FK_MODULE_WIRE11AND13)
  target_compile_options(fk-atlas-standard PUBLIC -DFK_HARDWARE_WIRE11AND13_ENABLE)
  target_compile_options(fk-atlas-standard PUBLIC -DFK_MODULE_WIRE11AND13)
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
  arduino(fk-atlas-amazon "${SOURCE_FILES}" "${LIBRARIES}")
  target_compile_options(fk-atlas-amazon PUBLIC -DFK_PROFILE_AMAZON)
  target_compile_options(fk-atlas-amazon PUBLIC -DFK_ENABLE_MS5803)
  target_compile_options(fk-atlas-amazon PUBLIC -DFK_DISABLE_ATLAS_ORP)
  if(FK_MODULE_WIRE11AND13)
    target_compile_options(fk-atlas-amazon PUBLIC -DFK_HARDWARE_WIRE11AND13_ENABLE)
    target_compile_options(fk-atlas-amazon PUBLIC -DFK_MODULE_WIRE11AND13)
  endif(FK_MODULE_WIRE11AND13)
endif(FK_PROFILE_AMAZON)

add_definitions(-DPHYLUM_ENABLE_SERIAL_FLASH -DPHYLUM_ENABLE_SD)
target_compile_options(arduino_zero_phylum PUBLIC -DPHYLUM_ENABLE_SERIAL_FLASH -DPHYLUM_ENABLE_SD)
