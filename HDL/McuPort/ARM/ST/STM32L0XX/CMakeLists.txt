if (MCU STREQUAL "STM32L071KB")
  set(JS_FILE "flash_071kb.jlink")
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

set(MCPU cortex-m0)
set_property(GLOBAL APPEND PROPERTY HDL_COMPILE_OPTIONS -mcpu=${MCPU} -mfloat-abi=soft)
set_property(GLOBAL APPEND PROPERTY HDL_DEFINES "__MCU_HAL_HDR__=<stm32l0xx_hal.h>")

set_property(GLOBAL APPEND PROPERTY HDL_LINK_OPTIONS -specs=nano.specs -lstdc++ -mthumb -mlittle-endian -mthumb-interwork)
set_property(GLOBAL APPEND PROPERTY HDL_LINK_OPTIONS -mcpu=${MCPU} -T "${CMAKE_CURRENT_SOURCE_DIR}/Res/linkerscript.ld")
set_property(GLOBAL APPEND PROPERTY HDL_LINK_OPTIONS -Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)

file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Res/${JS_FILE}" "${CMAKE_BINARY_DIR}/flash.jlink")
file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Res/${SWD_FILE}" "${CMAKE_BINARY_DIR}/mcu.svd")

set_property(GLOBAL PROPERTY MCU_SUPPORTED "OK")
