file(GLOB SOURCE_FILES *.cpp)
set(LIBRARIES Wire SPI)

option(FK_ENABLE_ATLAS_ORP "Enable/disable ORP sensor." ON)
if(FK_ENABLE_ATLAS_ORP)
  add_definitions(-DFK_ENABLE_ATLAS_ORP)
else()
  add_definitions(-DFK_DISABLE_ATLAS_ORP)
endif(FK_ENABLE_ATLAS_ORP)

arduino(fk-atlas-module "${SOURCE_FILES}" "${LIBRARIES}")
