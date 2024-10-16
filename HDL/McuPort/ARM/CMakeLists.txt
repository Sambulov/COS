if(NOT CORE STREQUAL "ARM")
  return()
endif()

set_target_properties(HDL PROPERTIES CORE_SUPPORTED "OK")

file(GLOB ARM_MCU_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/*.c"
)

target_sources(HDL PUBLIC ${ARM_MCU_SOURCES})
target_include_directories(HDL PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/CMSIS"
)

#Add all manufacturer dirs
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Gigadevice")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Nuvoton")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ST")

#=================

target_compile_options(HDL PUBLIC -mthumb -mlittle-endian -mthumb-interwork -ffunction-sections -fdata-sections -fno-common -fmessage-length=0)

set_property(GLOBAL PROPERTY HDL_DEFINES "ARM_MATH_CM3;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING;")
