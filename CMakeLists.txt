cmake_minimum_required(VERSION 3.0)

project(Server VERSION 1.0. LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(BUILD_SHARED_LIBS TRUE)

add_subdirectory(lib/protobuf_messages)

