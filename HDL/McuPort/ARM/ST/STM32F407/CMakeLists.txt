if (MCU STREQUAL "STM32F407VG")
  set(JS_FILE "flash_407vg.jlink")
  set(LD_FILE "linkerscript_407vg.ld")
else()
  return()
endif()

set(SWD_FILE "stm32f407.svd")
set(OOCD_TOOL_CNF "stlink.cfg")
set(OOCD_MCU_CNF "oocd_stm32f4x.cfg")

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

set(MCPU cortex-m4)
set_property(GLOBAL APPEND PROPERTY HDL_COMPILE_OPTIONS -mcpu=${MCPU})

set_property(GLOBAL APPEND PROPERTY HDL_DEFINES "__MCU_HAL_HDR__=<stm32f4xx_hal.h>")

if(DEFINED HARD_FLOAT)
  set_property(GLOBAL APPEND PROPERTY HDL_COMPILE_OPTIONS -mfloat-abi=hard -mfpu=fpv4-sp-d16)
  set_property(GLOBAL APPEND PROPERTY HDL_LINK_OPTIONS -mfloat-abi=hard -mfpu=fpv4-sp-d16)
else ()
  set_property(GLOBAL APPEND PROPERTY HDL_COMPILE_OPTIONS -mfloat-abi=soft)
  set_property(GLOBAL APPEND PROPERTY HDL_LINK_OPTIONS -mfloat-abi=soft)
endif()

file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Res/${LD_FILE}" "${CMAKE_BINARY_DIR}/linkerscript.ld")
file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Res/${JS_FILE}" "${CMAKE_BINARY_DIR}/flash.jlink")
file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Res/${SWD_FILE}" "${CMAKE_BINARY_DIR}/mcu.svd")
file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Res/${OOCD_TOOL_CNF}" "${CMAKE_BINARY_DIR}/oocd_tool.cfg")
file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Res/${OOCD_MCU_CNF}" "${CMAKE_BINARY_DIR}/oocd_mcu.cfg")

set_property(GLOBAL PROPERTY MCU_SUPPORTED "OK")
