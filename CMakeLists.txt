cmake_minimum_required(VERSION 3.6)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set project name
project(firmware)

enable_language(C CXX ASM)
set(EXECUTABLE ${PROJECT_NAME}.elf)

# Set C standart
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
#set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_C_EXTENSIONS OFF)


# Set toolchain dir, empty - use default
set(TOOLCHAIN_DIR "")
# Set toolchain prefix
set(TOOLCHAIN_PREFIX arm-none-eabi-)


if(NOT DEFINED BOARD) 
  set(BOARD "ATB_RK3568J_SMC_R3")
endif()

# Set terget core name
if(BOARD STREQUAL "ATB_RK3568J_SMC_R3")
  set(MANUFACTURER "Nuvoton")
  set(MCU  "NUM46X")
  set(CORE "ARM")
  #set(MCPU cortex-m4)
  set(PROJECT "ATB_RK3568J_SMC")
  #add_compile_definitions("__MCU_HAL_HDR__=<m460.h>")
elseif(BOARD STREQUAL "ATB_RK3568J_SMC_R1")
  set(MANUFACTURER "Gigadevice")
  set(MCU "GD32E23X")
  set(CORE "ARM")
  #set(MCPU cortex-m23)
  set(PROJECT "ATB_RK3568J_SMC")
  #add_compile_definitions("__MCU_HAL_HDR__=<gd32e23x.h>")
elseif(BOARD STREQUAL "ATB_RK3568J_SMC_R0")
  set(MANUFACTURER "ST")
  set(MCU "STM32L0XX")
  set(CORE "ARM")
  #set(MCPU cortex-m0)
  set(PROJECT "ATB_RK3568J_SMC")
  #add_compile_definitions("__MCU_HAL_HDR__=<stm32l0xx_hal.h>")
  add_compile_definitions("STM32L071KB")
  add_compile_definitions("STM32L071xx")
elseif(BOARD STREQUAL "ATB_USPD_MPC")
  set(MANUFACTURER "Gigadevice")
  set(MCU "GD32F103VG")
  set(CORE "ARM")
  #set(MCPU cortex-m3)
  set(PROJECT "ATB_USPD_MPC")
  #add_compile_definitions("__MCU_HAL_HDR__=<gd32f10x.h>")
  add_compile_definitions("GD32F10X_XD")
elseif(BOARD STREQUAL "ATB_USPD_MPC2")
  set(MANUFACTURER "Gigadevice")
  set(MCU "GD32E23X")
  set(CORE "ARM")
  #set(MCPU cortex-m23)
  set(PROJECT "ATB_USPD_MPC2")
  #add_compile_definitions("__MCU_HAL_HDR__=<gd32e23x.h>")  
elseif(BOARD STREQUAL "ATB_USPD20K")
  set(MANUFACTURER "Gigadevice")
  set(MCU "GD32E23X")
  set(CORE "ARM")
  #set(MCPU cortex-m23)
  set(PROJECT "ATB_USPD20K")
  #add_compile_definitions("__MCU_HAL_HDR__=<gd32e23x.h>")  
elseif(BOARD STREQUAL "ATB_3500")
  set(MANUFACTURER "Gigadevice")
  set(MCU "GD32F450ZI")
  set(CORE "ARM")
  #set(MCPU cortex-m4)
  set(PROJECT "ATB3500")
  #add_compile_definitions("__MCU_HAL_HDR__=<gd32f4xx.h>")
  add_compile_definitions("GD32F450")
elseif(BOARD STREQUAL "ATB_MUVV")
  set(MANUFACTURER "Gigadevice")
  set(MCU "GD32E23X")
  set(CORE "ARM")
  #set(MCPU cortex-m23)
  set(PROJECT "ATB_MUVV")
  #add_compile_definitions("__MCU_HAL_HDR__=<gd32e23x.h>")  
elseif(BOARD STREQUAL "GD32E23X_DEMO")
  set(MANUFACTURER "Gigadevice")
  set(MCU "GD32E23X")
  set(CORE "ARM")
  #set(MCPU cortex-m23)
  set(PROJECT "GD32E23X_DEMO")
  #add_compile_definitions("__MCU_HAL_HDR__=<gd32e23x.h>")  
else()
  message( SEND_ERROR "Board not supported!" )
endif()

add_compile_definitions("${BOARD}")

if (CORE STREQUAL "ARM")
  # Set linker script file
  set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/HDL/McuPort/ARM/${MANUFACTURER}/${MCU}/Res/linkerscript.ld")

  # Set defines
  set(DEFINES "ARM_MATH_CM3;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING;")

  set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}gcc)
  set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}g++)
  set(CMAKE_ASM_COMPILER ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}gcc)
  set(CMAKE_AR ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}ar)
  set(CMAKE_OBJCOPY ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}objcopy)
  set(CMAKE_OBJDUMP ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}objdump)
  set(CMAKE_SIZE ${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}size)

  #set(HEADER_FILE_ONLY ON)

  # Uncomment to mitigate c++17 absolute addresses warnings
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register")
  if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
      message(STATUS "Maximum optimization for speed")
      add_compile_options(-Ofast)
  elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
      message(STATUS "Maximum optimization for speed, debug info included")
      add_compile_options(-Ofast -g)
  elseif("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
      message(STATUS "Maximum optimization for size")
      add_compile_options(-Os)
  else()
      message(STATUS "Minimal optimization, debug info included")
      add_compile_definitions("DEBUG")
      add_compile_options(-O0 -ggdb)
  endif()
  
  # Add C defines
  #add_compile_definitions(${MCU})


  # Add you source file
  file(GLOB PROJ_SOURCES
    "${CMAKE_SOURCE_DIR}/Projects/${PROJECT}/*.c"
    "${CMAKE_SOURCE_DIR}/Projects/${PROJECT}/BLDL/*.c"
  )
  # Add you source file
  set(PROJ_HEADERS
    "${CMAKE_SOURCE_DIR}/Projects/${PROJECT}/Inc"
    "${CMAKE_SOURCE_DIR}/Projects/${PROJECT}/BLDL/Inc"
  )
  # Add Include directories
  include_directories( ${PROJ_HEADERS}
    # "${CMAKE_SOURCE_DIR}/Projects/${PROJECT}/Inc"
    # "${CMAKE_SOURCE_DIR}/Projects/${PROJECT}/BLDL/Inc"
  )

  #add_subdirectory("${CMAKE_SOURCE_DIR}/HDL")
  #get_property(LINKER_SCRIPT GLOBAL PROPERTY LD_SCRIPT)


  # Set compiler options
  #add_compile_options(-mcpu=${MCPU} )
  #add_compile_options(-mfloat-abi=soft -mthumb -mlittle-endian -mthumb-interwork -ffunction-sections -fdata-sections -fno-common -fmessage-length=0)
  #add_compile_options(-mthumb -mlittle-endian -mthumb-interwork -ffunction-sections -fdata-sections -fno-common -fmessage-length=0)

  # Set linker options
  # add_link_options(-specs=nano.specs -lstdc++ -mthumb -mlittle-endian -mthumb-interwork)
  # add_link_options(-mcpu=${MCPU} -T ${LINKER_SCRIPT})
  # add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)

  add_subdirectory("${CMAKE_SOURCE_DIR}/HDL")

  get_target_property(MAIN_CFLAGS HDL COMPILE_OPTIONS)
  get_target_property(LINKER_FLAGS HDL LINK_OPTIONS)

  add_compile_options(${MAIN_CFLAGS})
  add_link_options(${LINKER_FLAGS})

  add_subdirectory("${CMAKE_SOURCE_DIR}/CL")

  add_executable(${EXECUTABLE} ${PROJ_SOURCES} ${LINKER_SCRIPT})
  target_link_libraries(${EXECUTABLE} PUBLIC HDL)
  target_link_libraries(${EXECUTABLE} PUBLIC CodeLib)

  set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
  set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

  add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE} ${HEX_FILE}
    COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE} ${BIN_FILE}
    COMMENT "Building ${HEX_FILE} Building ${BIN_FILE}")


  # Print executable size
  add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_SIZE} ${EXECUTABLE})
endif() #ARM



# Write to flash
# set(OPEN_OCD C:\\NuOpenOCD\\bin\\openocd.exe)

# message("DEFINES = ${DEFINES}")

# add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
#         COMMENT "write to flash ..."
#         COMMAND ${OPEN_OCD} -s "${CMAKE_SOURCE_DIR}/${MCU}/Res" 
#         -f "${CMAKE_SOURCE_DIR}/${MCU}/Res/tool.cfg" 
#         -f "${CMAKE_SOURCE_DIR}/${MCU}/Res/mcu.cfg" 
#         -c "init" -c "halt"
#         -c "flash write_image erase ${CMAKE_SOURCE_DIR}/build/bmc.hex"
#         -c "reset run")
