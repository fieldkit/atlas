set(SOURCE_FILES
  module.cpp
  atlas.cpp
  sensors.cpp
  exceptions.cpp
)

set(LIBRARIES
  Wire
)

arduino(fk-atlas-module "${SOURCE_FILES}" "${LIBRARIES}")
