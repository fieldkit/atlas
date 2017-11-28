set(SOURCE_FILES
  main.cpp
)

set(LIBRARIES
  Wire
)

arduino(fk-atlas-test "${SOURCE_FILES}" "${LIBRARIES}")
