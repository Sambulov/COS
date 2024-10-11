

file(GLOB ST_COMMON
  "./*.c"
)

target_sources(HDL PUBLIC ${ST_COMMON})
target_include_directories(HDL PUBLIC
  "./Inc"
)

if (MCU STREQUAL "STM32L0XX")
  add_subdirectory("./STM32L0XX")
  set(MCPU cortex-m0)
else()
  message(SEND_ERROR "MCU ${MCU} unsupported!")
endif()
