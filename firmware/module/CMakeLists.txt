set(SOURCE_FILES
  module.cpp
  atlas.cpp
  logging.cpp
  sensors.cpp
)

set(LIBRARIES
  Wire
)

arduino(fk-atlas-module "${SOURCE_FILES}" "${LIBRARIES}")
