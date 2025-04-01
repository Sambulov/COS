
if (MCU STREQUAL "GD32E230K6")
  set(JS_FILE "flash_k6.jlink")
  set(LD_FILE "linkerscript_k6.ld")
elseif (MCU STREQUAL "GD32E230C6")
  set(JS_FILE "flash_c6.jlink")
  set(LD_FILE "linkerscript_c6.ld")
elseif (MCU STREQUAL "GD32E230C8")
  set(JS_FILE "flash_c8.jlink")
  set(LD_FILE "linkerscript_c8.ld")
else()
  return()
endif()

set(SWD_FILE "mcu.svd")

file(GLOB MCU_PORT
  "${CMAKE_CURRENT_SOURCE_DIR}/Port/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/Sys/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/Sys/Drivers/Src/*.c"
)

set_property(GLOBAL APPEND PROPERTY HDL_SOURCES ${MCU_PORT})
set_property(GLOBAL APPEND PROPERTY HDL_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/Port/Inc"
  "${CMAKE_CURRENT_SOURCE_DIR}/Sys/Inc"
  "${CMAKE_CURRENT_SOURCE_DIR}/Sys/Drivers/Inc"
)

set(MCPU cortex-m23)
set_property(GLOBAL APPEND PROPERTY HDL_COMPILE_OPTIONS -mcpu=${MCPU} -mfloat-abi=soft)
set_property(GLOBAL APPEND PROPERTY HDL_DEFINES "__MCU_HAL_HDR__=<gd32e23x.h>")

file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Res/${LD_FILE}" "${CMAKE_BINARY_DIR}/linkerscript.ld")
file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Res/${JS_FILE}" "${CMAKE_BINARY_DIR}/flash.jlink")
file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Res/${SWD_FILE}" "${CMAKE_BINARY_DIR}/mcu.svd")

set_property(GLOBAL PROPERTY MCU_SUPPORTED "OK")
