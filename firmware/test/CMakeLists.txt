set(SOURCE_FILES
  main.cpp
)

set(LIBRARIES
  SPI
  Wire
)

arduino(fk-atlas-test "${SOURCE_FILES}" "${LIBRARIES}")
