cmake_minimum_required(VERSION 3.15)
project(websocketpp)
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_definitions(-DASIO_STANDALONE -D_WEBSOCKETPP_CPP11_STRICT_)

include_directories("${PROJECT_INCLUDE_DIR}")
message(${PROJECT_INCLUDE_DIR})

add_library(websocketpp INTERFACE)

target_include_directories(websocketpp INTERFACE "${PROJECT_INCLUDE_DIR}")