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

arduino(fk-atlas-module "${SOURCE_FILES}" "${LIBRARIES}")
