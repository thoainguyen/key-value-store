cmake_minimum_required(VERSION 2.8.9)
project(kvs-server)

include_directories(./include)

find_package (Threads)

file(GLOB SOURCES "src/*.c")

add_executable(server ${SOURCES})

target_link_libraries (server ${CMAKE_THREAD_LIBS_INIT})
