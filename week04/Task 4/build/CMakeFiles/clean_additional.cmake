# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "GPIO_Button.bin"
  "GPIO_Button.hex"
  "GPIO_Button.map"
  )
endif()
