if(NOT MANUFACTURER STREQUAL "Nuvoton")
  return()
endif()

set_property(GLOBAL PROPERTY MANUFACTURER_SUPPORTED "OK")

file(GLOB SRC
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

set_property(GLOBAL APPEND PROPERTY HDL_SOURCES ${SRC})

set_property(GLOBAL APPEND PROPERTY HDL_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/Inc"
)

#Add all mcu dirs
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/NUM46X")

#=================


