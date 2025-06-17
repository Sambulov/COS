get_property(CORE GLOBAL PROPERTY CORE)
get_property(MANUFACTURER GLOBAL PROPERTY MCU_MANUFACTURER)
get_property(MCU GLOBAL PROPERTY MCU)

#Add all core dirs
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ARM")

#=================

get_property(CORE_SUPPORT GLOBAL PROPERTY CORE_SUPPORTED)

if(NOT CORE_SUPPORT STREQUAL "OK")
  message(SEND_ERROR "Core ${CORE} unsupported!")
endif()

get_property(MCU_SUPPORT GLOBAL PROPERTY MCU_SUPPORTED)

if(NOT MCU_SUPPORT STREQUAL "OK")
  message(SEND_ERROR "MCU ${MCU} unsupported!")
endif()

get_property(MAN_SUPPORT GLOBAL PROPERTY MANUFACTURER_SUPPORTED)

if(NOT MAN_SUPPORT STREQUAL "OK")
  message(SEND_ERROR "MCU manufacturer ${MANUFACTURER} unsupported!")
endif()

