

file(GLOB ARM_MCU_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/*.c"
)

target_sources(HDL PUBLIC ${ARM_MCU_SOURCES})
target_include_directories(HDL PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/CMSIS"
)

get_property(MANUFACTURER GLOBAL PROPERTY MCU_MANUFACTURER)

if (MANUFACTURER STREQUAL "Gigadevice")
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Gigadevice")
elseif(MANUFACTURER STREQUAL "Nuvoton")
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Nuvoton")
elseif(MANUFACTURER STREQUAL "ST")
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ST")
else()
  message(SEND_ERROR "MCU manufacturer ${MANUFACTURER} unsupported!")
endif()

target_compile_options(HDL PUBLIC -mthumb -mlittle-endian -mthumb-interwork -ffunction-sections -fdata-sections -fno-common -fmessage-length=0)

set_property(GLOBAL PROPERTY HDL_DEFINES "ARM_MATH_CM3;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING;")
