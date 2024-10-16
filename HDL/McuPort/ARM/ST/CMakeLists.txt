if(NOT MANUFACTURER STREQUAL "ST")
  return()
endif()

set_target_properties(HDL PROPERTIES MANUFACTURER_SUPPORTED "OK")

file(GLOB ST_COMMON
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

target_sources(HDL PUBLIC ${ST_COMMON})
target_include_directories(HDL PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/Inc"
)

#Add all mcu dirs
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/STM32L0XX")

#=================
