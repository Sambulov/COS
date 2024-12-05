set(BOARD "DEMO_GD32E23X")
#set(BOARD "DEMO_NUM463KG")
#set(BOARD "")

set(dir "${CMAKE_CURRENT_SOURCE_DIR}/${BOARD}")
if (EXISTS "${dir}")
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${BOARD}")
else()
  message( SEND_ERROR "Board (${BOARD}) not supported!" )
endif()

set_property(GLOBAL APPEND PROPERTY PROJECT_DEFINITIONS 
  ${BOARD}
)
