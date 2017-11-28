set(SOURCE_FILES
  main.cpp
  atlas.cpp
  sensors.cpp
  exceptions.cpp
)

set(LIBRARIES
  Wire
)

arduino(fk-atlas-module "${SOURCE_FILES}" "${LIBRARIES}")
