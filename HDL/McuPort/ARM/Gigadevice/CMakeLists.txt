

file(GLOB GD_COMMON
  "./*.c"
)

target_sources(HDL PUBLIC ${GD_COMMON})
target_include_directories(HDL PUBLIC
  "./Inc"
)

if (MCU STREQUAL "GD32E23X")
  add_subdirectory("./GD32E23X")
elseif(MCU STREQUAL "GD32F103VG")
  add_subdirectory("./GD32F103VG")
elseif(MCU STREQUAL "GD32F450ZI")
  add_subdirectory("./GD32F450ZI")
else()
  message(SEND_ERROR "MCU ${MCU} unsupported!")
endif()

