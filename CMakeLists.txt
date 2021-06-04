# cmake_minimum_required(VERSION <specify CMake version here>)
project(OS234123_HW3)

set(CMAKE_CXX_STANDARD 11)

add_executable( OS234123-HW3 segel.c  segel.h  server.c  request.c  request.h  output.c  client.c  queue.h )