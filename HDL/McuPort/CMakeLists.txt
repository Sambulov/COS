get_property(CORE GLOBAL PROPERTY CORE)
get_property(MANUFACTURER GLOBAL PROPERTY MCU_MANUFACTURER)
get_property(MCU GLOBAL PROPERTY MCU)

if (CORE STREQUAL "ARM")
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ARM")
else()
  message(SEND_ERROR "Core ${CORE} unsupported!")
endif()

get_target_property(MCU_SUPPORT HDL MCU_SUPPORTED)

if(NOT MCU_SUPPORT STREQUAL "OK")
  message(SEND_ERROR "MCU ${MCU} unsupported!")
endif()

get_target_property(MAN_SUPPORT HDL MANUFACTURER_SUPPORTED)

if(NOT MAN_SUPPORT STREQUAL "OK")
  message(SEND_ERROR "MCU manufacturer ${MANUFACTURER} unsupported!")
endif()

