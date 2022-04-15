cmake_minimum_required(VERSION 3.10)

set ( DIRECTORIES
      ${CMAKE_CURRENT_SOURCE_DIR}/tcp
      ${CMAKE_CURRENT_SOURCE_DIR}/udp
      ${CMAKE_CURRENT_SOURCE_DIR}/socket_operations
    )

set ( SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/tcp/tcp_server.c
      ${CMAKE_CURRENT_SOURCE_DIR}/tcp/tcp_client.c
      ${CMAKE_CURRENT_SOURCE_DIR}/udp/udp_server.c
      ${CMAKE_CURRENT_SOURCE_DIR}/udp/udp_client.c
      ${CMAKE_CURRENT_SOURCE_DIR}/socket_operations/tcp_operations.c
      ${CMAKE_CURRENT_SOURCE_DIR}/socket_operations/udp_operations.c
    )

add_compile_options("-g")
add_library(networking_shared SHARED EXCLUDE_FROM_ALL ${SOURCES})
target_include_directories(networking_shared PUBLIC ${DIRECTORIES})