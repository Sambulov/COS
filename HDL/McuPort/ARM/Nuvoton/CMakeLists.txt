

file(GLOB NU_COMMON
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

target_sources(HDL PUBLIC ${NU_COMMON})
target_include_directories(HDL PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/Inc"
)

get_property(MCU GLOBAL PROPERTY MCU)

if (MCU STREQUAL "NUM46X")
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/NUM46X")
else()
  message(SEND_ERROR "MCU ${MCU} unsupported!")
endif()

