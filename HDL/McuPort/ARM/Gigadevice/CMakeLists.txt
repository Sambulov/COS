

file(GLOB GD_COMMON
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

target_sources(HDL PUBLIC ${GD_COMMON})
target_include_directories(HDL PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/Inc"
)

get_property(MCU GLOBAL PROPERTY MCU)

if (MCU STREQUAL "GD32E23X")
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/GD32E23X")
elseif(MCU STREQUAL "GD32F103VG")
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/GD32F103VG")
elseif(MCU STREQUAL "GD32F450ZI")
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/GD32F450ZI")
else()
  message(SEND_ERROR "MCU ${MCU} unsupported!")
endif()

