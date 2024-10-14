

file(GLOB MCU_PORT
  "./Port/*.c"
  "./Sys/*.c"
  "./Sys/Drivers/Src/*.c"
)

target_sources(HDL PUBLIC ${MCU_PORT})
target_include_directories(HDL PUBLIC
  "./Port/Inc"
  "./Sys/Inc"
  "./Sys/Drivers/Inc"
)

set(MCPU cortex-m4)
target_compile_options(HDL PUBLIC -mcpu=${MCPU})
target_compile_definitions(HDL PUBLIC "__MCU_HAL_HDR__=<gd32f4xx.h>")

target_link_options(HDL PUBLIC -specs=nano.specs -lstdc++ -mthumb -mlittle-endian -mthumb-interwork)
target_link_options(HDL PUBLIC -mcpu=${MCPU} -T "${CMAKE_CURRENT_SOURCE_DIR}/Res/linkerscript.ld")
target_link_options(HDL PUBLIC -Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)

if(DEFINED HARD_FLOAT)
  target_compile_options(HDL PUBLIC -mfloat-abi=hard -mfpu=fpv4-sp-d16)
  target_link_options(HDL PUBLIC -mfloat-abi=hard -mfpu=fpv4-sp-d16)
else ()
  target_compile_options(HDL PUBLIC -mfloat-abi=soft)
  target_link_options(HDL PUBLIC -mfloat-abi=soft)
endif()
