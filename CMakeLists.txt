cmake_minimum_required(VERSION 3.10)

set ( DIRECTORIES
      ${CMAKE_CURRENT_SOURCE_DIR}/tcp
      ${CMAKE_CURRENT_SOURCE_DIR}/udp
    )

set ( SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/udp/udp_server.c
    )

add_compile_options("-g")
add_library(networking_shared SHARED EXCLUDE_FROM_ALL ${SOURCES})
target_include_directories(networking_shared PUBLIC ${DIRECTORIES})