

file(GLOB ST_COMMON
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

target_sources(HDL PUBLIC ${ST_COMMON})
target_include_directories(HDL PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/Inc"
)

get_property(MCU GLOBAL PROPERTY MCU)

if (MCU STREQUAL "STM32L0XX")
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/STM32L0XX")
else()
  message(SEND_ERROR "MCU ${MCU} unsupported!")
endif()
