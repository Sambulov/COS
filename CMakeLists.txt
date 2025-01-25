cmake_minimum_required(VERSION 3.6)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(firmware)
set(EXECUTABLE ${PROJECT_NAME}.elf)
enable_language(C CXX ASM)

# Set C standart
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
#set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_C_EXTENSIONS OFF)

# Uncomment to mitigate c++17 absolute addresses warnings

add_subdirectory("${CMAKE_SOURCE_DIR}/Projects")
add_subdirectory("${CMAKE_SOURCE_DIR}/HDL")
add_subdirectory("${CMAKE_SOURCE_DIR}/CL")

get_property(TOOLCHAIN_PREFIX GLOBAL PROPERTY TOOLCHAIN_PREFIX)
get_property(TOOLCHAIN_DIR GLOBAL PROPERTY TOOLCHAIN_DIR)
set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_AR ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}ar)
set(CMAKE_OBJCOPY ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_OBJDUMP ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}objdump)
set(CMAKE_SIZE ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}size)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register")
if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  message(STATUS "Maximum optimization for speed")
  set_property(GLOBAL APPEND PROPERTY HDL_LINK_OPTIONS -O2)
  set_property(GLOBAL APPEND PROPERTY HDL_COMPILE_OPTIONS -O2)
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
  message(STATUS "Maximum optimization for speed, debug info included")
  set_property(GLOBAL APPEND PROPERTY HDL_LINK_OPTIONS -O2 -g)
  set_property(GLOBAL APPEND PROPERTY HDL_COMPILE_OPTIONS -O2 -g)
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
  message(STATUS "Maximum optimization for size")
  set_property(GLOBAL APPEND PROPERTY HDL_LINK_OPTIONS -Os)
  set_property(GLOBAL APPEND PROPERTY HDL_COMPILE_OPTIONS -Os)
else()
  message(STATUS "Minimal optimization, debug info included")
  set(CMAKE_VERBOSE_MAKEFILE ON)
  add_compile_definitions("DEBUG")
  set_property(GLOBAL APPEND PROPERTY HDL_LINK_OPTIONS -O0 -g3 -ggdb -fno-builtin)
  set_property(GLOBAL APPEND PROPERTY HDL_COMPILE_OPTIONS -O0 -g3 -ggdb -fno-builtin)
endif()

set_property(GLOBAL APPEND PROPERTY HDL_LINK_OPTIONS -Wl,--gc-sections -Wl,--print-memory-usage -Wl,-Map=output.map -T ${CMAKE_BINARY_DIR}/linkerscript.ld)
set_property(GLOBAL APPEND PROPERTY HDL_COMPILE_OPTIONS -std=gnu11 -Wall -Wextra -ffunction-sections -fdata-sections -ffreestanding)

# Get compiler options
get_property(COMPILE_FLAGS GLOBAL PROPERTY HDL_COMPILE_OPTIONS)
# Get linker options
get_property(LINKER_FLAGS GLOBAL PROPERTY HDL_LINK_OPTIONS)
# Get compile def
get_property(PROJECT_DEFINITIONS GLOBAL PROPERTY PROJECT_DEFINITIONS)
get_property(HDL_DEFINITIONS GLOBAL PROPERTY HDL_DEFINES)
get_property(CL_DEFINITIONS GLOBAL PROPERTY CL_DEFINES)
# Get sources
get_property(PROJECT_SOURCES GLOBAL PROPERTY PROJECT_SOURCES)
get_property(HDL_SOURCES GLOBAL PROPERTY HDL_SOURCES)
get_property(CL_SOURCES GLOBAL PROPERTY CL_SOURCES)
# Get headers
get_property(CL_HEADERS GLOBAL PROPERTY CL_HEADERS)
get_property(HDL_HEADERS GLOBAL PROPERTY HDL_HEADERS)
get_property(PROJECT_HEADERS GLOBAL PROPERTY PROJECT_HEADERS)


include_directories( ${PROJECT_HEADERS} )
include_directories( ${HDL_HEADERS} )
include_directories( ${CL_HEADERS} )

string (REPLACE ";" " " COMPILE_FLAGS "${COMPILE_FLAGS}")
string (REPLACE ";" " " LINKER_FLAGS "${LINKER_FLAGS}")

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  #"-mthumb-interwork -fmessage-length=0 -O0 -g3 -ggdb -fno-builtin -std=gnu11 -Wall -Wextra -ffunction-sections -fdata-sections -ffreestanding -mthumb -mlittle-endian -mfloat-abi=soft -mcpu=cortex-m23"
  set(CMAKE_C_FLAGS_DEBUG ${COMPILE_FLAGS})
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
  #"-O0 -g3 -ggdb -fno-builtin -Wl,--gc-sections -Wl,--print-memory-usage -Wl,-Map=output.map -T ${CMAKE_BINARY_DIR}/linkerscript.ld"
else()
  set(CMAKE_C_FLAGS ${COMPILE_FLAGS})
  set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS_DEBUG}")
endif()

set(CMAKE_EXE_LINKER_FLAGS ${LINKER_FLAGS})

add_compile_definitions(${HDL_DEFINITIONS})
add_compile_definitions(${CL_DEFINITIONS})
add_compile_definitions(${PROJECT_DEFINITIONS})

add_executable(${EXECUTABLE} ${HDL_SOURCES} ${PROJECT_SOURCES}  ${CL_SOURCES} ${LINKER_SCRIPT})

# Указываем, что нужно использовать newlib-nano
#target_link_libraries(${EXECUTABLE} -specs=nano.specs)

# Указываем, что нужно использовать nosys.specs (если не используются системные вызовы)
target_link_libraries(${EXECUTABLE} -specs=nosys.specs)

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE} ${HEX_FILE}
  COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE} ${BIN_FILE}
  COMMENT "Building ${HEX_FILE} Building ${BIN_FILE}")

# Print executable size
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
  COMMAND ${CMAKE_SIZE} ${EXECUTABLE})

get_property(PROJECT_BOARD GLOBAL PROPERTY BOARD)
get_property(PROJECT_CORE GLOBAL PROPERTY CORE)
get_property(PROJECT_MCU_MAN GLOBAL PROPERTY MCU_MANUFACTURER)
get_property(PROJECT_TOOLCHAIN_DIR GLOBAL PROPERTY TOOLCHAIN_DIR)
get_property(PROJECT_MCU GLOBAL PROPERTY MCU)

file(WRITE "${CMAKE_SOURCE_DIR}/.vscode/settings.json" 
{\n
  \t\"BOARD\":"${PROJECT_BOARD}",\n
  \t\"MCU\":"${PROJECT_MCU}",\n
  \t\"CORE\":"${PROJECT_CORE}",\n
  \t\"MANUFACTURER\":"${PROJECT_MCU_MAN}",\n
  \t\"TOOLCHAIN_DIR\":"${PROJECT_TOOLCHAIN_DIR}"\n
}
)

#message(SEND_ERROR "${HEADERS}")

# Write to flash
# set(OPEN_OCD C:\\NuOpenOCD\\bin\\openocd.exe)

# add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
#         COMMENT "write to flash ..."
#         COMMAND ${OPEN_OCD} -s "${CMAKE_SOURCE_DIR}/${MCU}/Res" 
#         -f "${CMAKE_SOURCE_DIR}/${MCU}/Res/tool.cfg" 
#         -f "${CMAKE_SOURCE_DIR}/${MCU}/Res/mcu.cfg" 
#         -c "init" -c "halt"
#         -c "flash write_image erase ${CMAKE_SOURCE_DIR}/build/bmc.hex"
#         -c "reset run")
