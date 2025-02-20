if(NOT CORE STREQUAL "ARM")
  return()
endif()

set_property(GLOBAL PROPERTY CORE_SUPPORTED "OK")

file(GLOB SRC
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/*.c"
)

set_property(GLOBAL APPEND PROPERTY HDL_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/"
)

set_property(GLOBAL APPEND PROPERTY HDL_SOURCES ${SRC})

set_property(GLOBAL APPEND PROPERTY HDL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/CMSIS")

#Add all manufacturer dirs
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Gigadevice")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Nuvoton")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ST")

#=================
set_property(GLOBAL APPEND PROPERTY HDL_COMPILE_OPTIONS -mthumb -mlittle-endian -mthumb-interwork -fmessage-length=0)
set_property(GLOBAL APPEND PROPERTY HDL_DEFINES "ARM_MATH_CM3;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING;")
