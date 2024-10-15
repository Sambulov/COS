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

target_sources(HDL PUBLIC ${MCU_PORT})
target_include_directories(HDL PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/Port/Inc"
  "${CMAKE_CURRENT_SOURCE_DIR}/Sys/Inc"
  "${CMAKE_CURRENT_SOURCE_DIR}/Sys/Drivers/Inc"
)

set(MCPU cortex-m0)
target_compile_options(HDL PUBLIC -mcpu=${MCPU} -mfloat-abi=soft)
target_compile_definitions(HDL PUBLIC "__MCU_HAL_HDR__=<stm32l0xx_hal.h>")

target_link_options(HDL PUBLIC -specs=nano.specs -lstdc++ -mthumb -mlittle-endian -mthumb-interwork)
target_link_options(HDL PUBLIC -mcpu=${MCPU} -T "${CMAKE_CURRENT_SOURCE_DIR}/Res/linkerscript.ld")
target_link_options(HDL PUBLIC -Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Res/${JS_FILE}" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Res/${SWD_FILE}" "${CMAKE_BINARY_DIR}/mcu.svd")

set_target_properties(HDL PROPERTIES MCU_SUPPORTED "OK")
