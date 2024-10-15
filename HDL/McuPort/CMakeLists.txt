get_property(CORE GLOBAL PROPERTY CORE)

if (CORE STREQUAL "ARM")
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ARM")
else()
  message(SEND_ERROR "Core ${CORE} unsupported!")
endif()
