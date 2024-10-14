

file(GLOB NU_COMMON
  "./*.c"
)

target_sources(HDL PUBLIC ${NU_COMMON})
target_include_directories(HDL PUBLIC
  "./Inc"
)

if (MCU STREQUAL "NUM46X")
  add_subdirectory("./NUM46X")
else()
  message(SEND_ERROR "MCU ${MCU} unsupported!")
endif()

