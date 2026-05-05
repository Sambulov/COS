if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/proj_select.txt")
  file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/proj_select.txt" PROJECT_DEF)
  list(GET PROJECT_DEF 0 PROJECT)
  list(GET PROJECT_DEF 1 BOARD)
  list(GET PROJECT_DEF 2 REMOUTE)
  list(GET PROJECT_DEF 3 LAUNCHER)
else()
  file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/proj_select.txt" 
      "DEMO\nDEMO_STM32F407\n\n"
  )
  set(PROJECT "DEMO")
  set(BOARD "DEMO_STM32F407")
endif()

configure_file(proj_select.txt proj_select_copy.txt COPYONLY)

set(dir "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}")
if (EXISTS "${dir}")
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}")
else()
  message(SEND_ERROR "Project (${PROJECT}) not found!" )
endif()

set_property(GLOBAL APPEND PROPERTY PROJECT_DEFINITIONS 
  ${BOARD}
)

set_property(GLOBAL PROPERTY BOARD ${BOARD})
set_property(GLOBAL PROPERTY REMOUTE_GDB_TARGET ${REMOUTE})
set_property(GLOBAL PROPERTY REMOUTE_GDB_LAUNCHER ${LAUNCHER})
