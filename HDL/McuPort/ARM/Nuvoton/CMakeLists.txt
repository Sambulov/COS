if(NOT MANUFACTURER STREQUAL "Nuvoton")
  return()
endif()

set_target_properties(HDL PROPERTIES MANUFACTURER_SUPPORTED "OK")

file(GLOB NU_COMMON
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

target_sources(HDL PUBLIC ${NU_COMMON})
target_include_directories(HDL PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/Inc"
)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/NUM46X")

