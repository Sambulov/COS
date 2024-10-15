if(NOT MANUFACTURER STREQUAL "Gigadevice")
  return()
endif()

set_target_properties(HDL PROPERTIES MANUFACTURER_SUPPORTED "OK")

file(GLOB GD_COMMON
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

target_sources(HDL PUBLIC ${GD_COMMON})
target_include_directories(HDL PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/Inc"
)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/GD32E23X")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/GD32F103")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/GD32F450")
