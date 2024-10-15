

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
