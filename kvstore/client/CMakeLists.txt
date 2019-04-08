cmake_minimum_required(VERSION 2.8.9)
project(client)

include_directories(../include)

find_package (Threads)

file(GLOB SOURCES "../src/*.c")

add_executable(client ${SOURCES}  client.c)

target_link_libraries (client ${CMAKE_THREAD_LIBS_INIT})
