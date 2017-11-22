set(SOURCE_FILES
  test.ino
)

set(LIBRARIES
  Wire
)

arduino(fk-atlas-test "${SOURCE_FILES}" "${LIBRARIES}")
