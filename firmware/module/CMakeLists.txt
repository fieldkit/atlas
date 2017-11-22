set(SOURCE_FILES
  module.cpp
)

set(LIBRARIES
  Wire
)

arduino(fk-atlas-module "${SOURCE_FILES}" "${LIBRARIES}")
