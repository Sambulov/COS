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
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register")
if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  message(STATUS "Maximum optimization for speed")
  add_compile_options(-O2)
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
  message(STATUS "Maximum optimization for speed, debug info included")
  add_compile_options(-O2 -g)
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
  message(STATUS "Maximum optimization for size")
  add_compile_options(-Os)
else()
  message(STATUS "Minimal optimization, debug info included")
  set(CMAKE_VERBOSE_MAKEFILE ON)
  add_compile_definitions("DEBUG")
  add_compile_options(-Og -g3 -gdwarf-2 -ggdb)
  add_link_options(-Og -g3 -gdwarf-2 -ggdb)
endif()

add_subdirectory("${CMAKE_SOURCE_DIR}/Projects")
add_subdirectory("${CMAKE_SOURCE_DIR}/HDL")
add_subdirectory("${CMAKE_SOURCE_DIR}/CL")

get_property(PROJECT_SOURCES GLOBAL PROPERTY PROJECT_SOURCES)
get_property(PROJECT_HEADERS GLOBAL PROPERTY PROJECT_HEADERS)

get_property(PROJECT_DEFINITIONS GLOBAL PROPERTY PROJECT_DEFINITIONS)
add_compile_definitions(${PROJECT_DEFINITIONS})

get_property(TOOLCHAIN_PREFIX GLOBAL PROPERTY TOOLCHAIN_PREFIX)
get_property(TOOLCHAIN_DIR GLOBAL PROPERTY TOOLCHAIN_DIR)
set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_AR ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}ar)
set(CMAKE_OBJCOPY ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_OBJDUMP ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}objdump)
set(CMAKE_SIZE ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}size)

# Set compiler options
get_property(MAIN_CFLAGS GLOBAL PROPERTY HDL_COMPILE_OPTIONS)
add_compile_options(${MAIN_CFLAGS})

# Set linker options
get_property(LINKER_FLAGS GLOBAL PROPERTY HDL_LINK_OPTIONS)
add_link_options(${LINKER_FLAGS})
# Set compile def
get_property(DEFINITIONS GLOBAL PROPERTY HDL_DEFINES)
add_compile_definitions(${DEFINITIONS})
get_property(HDL_SOURCES GLOBAL PROPERTY HDL_SOURCES)
get_property(HDL_HEADERS GLOBAL PROPERTY HDL_HEADERS)


# Set compile def
get_property(DEFINITIONS GLOBAL PROPERTY CL_DEFINES)
add_compile_definitions(${DEFINITIONS})
get_property(CL_SOURCES GLOBAL PROPERTY CL_SOURCES)
get_property(CL_HEADERS GLOBAL PROPERTY CL_HEADERS)

include_directories( ${PROJECT_HEADERS} )
include_directories( ${HDL_HEADERS} )
include_directories( ${CL_HEADERS} )


add_executable(${EXECUTABLE} ${PROJECT_SOURCES} ${HDL_SOURCES} ${CL_SOURCES} ${LINKER_SCRIPT})

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
