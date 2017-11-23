set(SOURCE_FILES
  test.cpp
)

set(LIBRARIES
  Wire
)

arduino(fk-atlas-test "${SOURCE_FILES}" "${LIBRARIES}")
