

file(GLOB ARM_MCU_SOURCES
  "./*.c"
  "./CMSIS/*.c"
)

target_sources(HDL PUBLIC ${ARM_MCU_SOURCES})
target_include_directories(HDL PUBLIC
  "./CMSIS"
)

if (MANUFACTURER STREQUAL "Gigadevice")
  add_subdirectory("./Gigadevice")
elseif(MANUFACTURER STREQUAL "Nuvoton")
  add_subdirectory("./Nuvoton")
elseif(MANUFACTURER STREQUAL "ST")
  add_subdirectory("./ST")
else()
  message(SEND_ERROR "Manufacturer ${MANUFACTURER} unsupported!")
endif()

target_compile_options(HDL PUBLIC -mthumb -mlittle-endian -mthumb-interwork -ffunction-sections -fdata-sections -fno-common -fmessage-length=0)

set_property(GLOBAL PROPERTY HDL_DEFINES "ARM_MATH_CM3;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING;")
