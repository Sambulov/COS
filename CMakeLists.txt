cmake_minimum_required(VERSION 3.19)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(POLICY CMP0156)
  cmake_policy(SET CMP0156 NEW)
endif()
if(POLICY CMP0179)
  cmake_policy(SET CMP0179 NEW)
endif()
if(POLICY CMP0181)
  cmake_policy(SET CMP0181 NEW)
endif()
if(POLICY CMP0126)
  cmake_policy(SET CMP0126 NEW)
endif()


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

# Set C standart
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
#set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_C_EXTENSIONS OFF)

project(firmware)
enable_language(C CXX ASM)

get_property(LIB_ENABLE_LWIP GLOBAL PROPERTY LIB_ENABLE_LWIP)
get_property(LIB_ENABLE_MBEDTLS GLOBAL PROPERTY LIB_ENABLE_MBEDTLS)
get_property(LIB_ENABLE_LITTLE_FS GLOBAL PROPERTY LIB_ENABLE_LITTLE_FS)
get_property(LIB_ENABLE_EMBEDDED_CLI GLOBAL PROPERTY LIB_ENABLE_EMBEDDED_CLI)

if(LIB_ENABLE_EMBEDDED_CLI STREQUAL "ENABLE")
  set_property(GLOBAL APPEND PROPERTY HDL_HEADERS
    "${CMAKE_SOURCE_DIR}/ThirdParty/EmbeddedCLI/"
  )
  file(GLOB MBEDTLS_SRC
    "${CMAKE_SOURCE_DIR}/ThirdParty/EmbeddedCLI/*.c"
  )
  set_property(GLOBAL APPEND PROPERTY HDL_SOURCES ${MBEDTLS_SRC})
endif()

if(LIB_ENABLE_LWIP STREQUAL "ENABLE")
  set(LWIP_DIR "${CMAKE_SOURCE_DIR}/ThirdParty/LwIP")
  set_property(GLOBAL APPEND PROPERTY HDL_HEADERS
    "${LWIP_DIR}/src/include/"
    #"${LWIP_DIR}/contrib/"
    #"${LWIP_DIR}/contrib/ports/unix/port/include"
    #"${LWIP_DIR}/contrib/examples/example_app"
  )
  include(${LWIP_DIR}/src/Filelists.cmake)
  set_property(GLOBAL APPEND PROPERTY HDL_SOURCES ${lwipnoapps_SRCS})
  if(LIB_ENABLE_MBEDTLS STREQUAL "ENABLE")
    set_property(GLOBAL APPEND PROPERTY HDL_SOURCES ${lwipmbedtls_SRCS})
  endif()
endif()

if(LIB_ENABLE_MBEDTLS STREQUAL "ENABLE")
  set_property(GLOBAL APPEND PROPERTY HDL_HEADERS
    "${CMAKE_SOURCE_DIR}/ThirdParty/MbedTLS/include"
    "${CMAKE_SOURCE_DIR}/ThirdParty/MbedTLS/library"
  )
  file(GLOB MBEDTLS_SRC
    "${CMAKE_SOURCE_DIR}/ThirdParty/MbedTLS/library/*.c"
  )
  set_property(GLOBAL APPEND PROPERTY HDL_SOURCES ${MBEDTLS_SRC})
endif()

if(LIB_ENABLE_LITTLE_FS STREQUAL "ENABLE")
  set_property(GLOBAL APPEND PROPERTY HDL_HEADERS
    "${CMAKE_SOURCE_DIR}/ThirdParty/littlefs/"
  )
  file(GLOB LITTLEFS_SRC
    "${CMAKE_SOURCE_DIR}/ThirdParty/littlefs/*.c"
  )
  set_property(GLOBAL APPEND PROPERTY HDL_SOURCES ${LITTLEFS_SRC})
endif()

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
  set (LWIP_DEFINITIONS LWIP_DEBUG=1)
  set(CMAKE_VERBOSE_MAKEFILE ON)
  add_compile_definitions("DEBUG")
  set_property(GLOBAL APPEND PROPERTY HDL_LINK_OPTIONS -O0 -g3 -ggdb -fno-builtin)
  set_property(GLOBAL APPEND PROPERTY HDL_COMPILE_OPTIONS -O0 -g3 -ggdb -fno-builtin)
endif()

set(LINKER_SCRIPT "${CMAKE_BINARY_DIR}/linkerscript.ld")
if(EXISTS "${CMAKE_BINARY_DIR}/linkerscript_custom.ld")
  set(LINKER_SCRIPT "${CMAKE_BINARY_DIR}/linkerscript_custom.ld")
endif()

set_property(GLOBAL APPEND PROPERTY HDL_LINK_OPTIONS -Wl,--gc-sections -Wl,--print-memory-usage -Wl,-Map=output.map -T  ${LINKER_SCRIPT})
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

set(EXECUTABLE ${PROJECT_NAME}.elf)
add_executable(${EXECUTABLE} ${HDL_SOURCES} ${PROJECT_SOURCES} ${CL_SOURCES})

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
get_property(PROJECT_REMOUTE_GDB_TARGET GLOBAL PROPERTY REMOUTE_GDB_TARGET)
get_property(PROJECT_REMOUTE_GDB_LAUNCHER GLOBAL PROPERTY REMOUTE_GDB_LAUNCHER)

file(WRITE "${CMAKE_SOURCE_DIR}/.vscode/settings.json" 
{\n
  \t\"BOARD\":"${PROJECT_BOARD}",\n
  \t\"MCU\":"${PROJECT_MCU}",\n
  \t\"CORE\":"${PROJECT_CORE}",\n
  \t\"MANUFACTURER\":"${PROJECT_MCU_MAN}",\n
  \t\"TOOLCHAIN_DIR\":"${PROJECT_TOOLCHAIN_DIR}",\n
  \t\"REMOUTE_GDB_TARGET\":"${PROJECT_REMOUTE_GDB_TARGET}",\n
  \t\"REMOUTE_GDB_LAUNCHER\":"${PROJECT_REMOUTE_GDB_LAUNCHER}"\n
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
