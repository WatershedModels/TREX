cmake_minimum_required(VERSION 3.0)
file(GLOB TREX_SRCS *.c)
list(REMOVE_ITEM TREX_SRCS "${CMAKE_SOURCE_DIR}/trex_src/src/trex-r4-debugger.c")
add_executable(trex ${TREX_SRCS})

set_target_properties(trex PROPERTIES
  RELEASE_POSTFIX "${TREX_VER_MAJOR}Rev${TREX_VER_MINOR}Rel"
  DEBUG_POSTFIX "${TREX_VER_MAJOR}Rev${TREX_VER_MINOR}Dbg"
)
set_property(TARGET trex PROPERTY C_STANDARD 11)
install(TARGETS trex RUNTIME DESTINATION ${INSTALL_DIR})
