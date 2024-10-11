

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

add_compile_definitions("__MCU_HAL_HDR__=<stm32l0xx_hal.h>")

set(LINKER_SCRIPT "./Res/linkerscript.ld")
