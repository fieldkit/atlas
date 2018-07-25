file(GLOB SOURCE_FILES *.cpp)
set(LIBRARIES Wire SPI)

option(FK_ENABLE_ATLAS_ORP "Enable/disable ORP sensor." ON)
if(FK_ENABLE_ATLAS_ORP)
  add_definitions(-DFK_ENABLE_ATLAS_ORP)
else()
  add_definitions(-DFK_DISABLE_ATLAS_ORP)
endif(FK_ENABLE_ATLAS_ORP)

option(FK_ENABLE_MS5803 "Enable/disable MS5803 sensor." OFF)
if(FK_ENABLE_MS5803)
  add_definitions(-DFK_ENABLE_MS5803)
else()
  add_definitions(-DFK_DISABLE_MS5803)
endif(FK_ENABLE_MS5803)

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

arduino(fk-atlas-module "${SOURCE_FILES}" "${LIBRARIES}")
