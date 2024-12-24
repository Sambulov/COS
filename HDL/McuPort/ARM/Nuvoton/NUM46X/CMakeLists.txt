if (MCU STREQUAL "M463KGCAE")
  set(JS_FILE "flash_463.jlink")
else()
  return()
endif()

set(SWD_FILE "mcu.svd")
set(OOCD_TOOL_CNF "tool.cfg")
set(OOCD_MCU_CNF "mcu.cfg")

file(GLOB MCU_PORT
  "${CMAKE_CURRENT_SOURCE_DIR}/Port/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/Sys/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/Sys/Drivers/Src/*.c"
)

set_property(GLOBAL APPEND PROPERTY HDL_SOURCES ${MCU_PORT})
set_property(GLOBAL APPEND PROPERTY HDL_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/Port/Inc"
  "${CMAKE_CURRENT_SOURCE_DIR}/Sys/Inc"
  "${CMAKE_CURRENT_SOURCE_DIR}/Sys/Drivers/Inc"
)

set(MCPU cortex-m4)
set_property(GLOBAL APPEND PROPERTY HDL_COMPILE_OPTIONS -mcpu=${MCPU})

set_property(GLOBAL APPEND PROPERTY HDL_DEFINES "__MCU_HAL_HDR__=<m460.h>")

set_property(GLOBAL APPEND PROPERTY HDL_LINK_OPTIONS -specs=nano.specs -lstdc++ -mthumb -mlittle-endian -mthumb-interwork)
set_property(GLOBAL APPEND PROPERTY HDL_LINK_OPTIONS -mcpu=${MCPU} -T "${CMAKE_CURRENT_SOURCE_DIR}/Res/linkerscript.ld")
set_property(GLOBAL APPEND PROPERTY HDL_LINK_OPTIONS -Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)

if(DEFINED HARD_FLOAT)
  set_property(GLOBAL APPEND PROPERTY HDL_COMPILE_OPTIONS -mfloat-abi=hard -mfpu=fpv4-sp-d16)
  set_property(GLOBAL APPEND PROPERTY HDL_LINK_OPTIONS -mfloat-abi=hard -mfpu=fpv4-sp-d16)
else ()
  set_property(GLOBAL APPEND PROPERTY HDL_COMPILE_OPTIONS -mfloat-abi=soft)
  set_property(GLOBAL APPEND PROPERTY HDL_LINK_OPTIONS -mfloat-abi=soft)
endif()

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Res/${JS_FILE}" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Res/${SWD_FILE}" "${CMAKE_BINARY_DIR}/mcu.svd")
file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Res/${OOCD_TOOL_CNF}" "${CMAKE_BINARY_DIR}/oocd_tool.cfg")
file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Res/${OOCD_MCU_CNF}" "${CMAKE_BINARY_DIR}/oocd_mcu.cfg")

set_property(GLOBAL PROPERTY MCU_SUPPORTED "OK")
