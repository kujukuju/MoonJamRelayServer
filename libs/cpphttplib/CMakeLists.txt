cmake_minimum_required(VERSION 3.15)
project(cpphttplib)
set(CMAKE_CXX_STANDARD 11)

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

include_directories("${PROJECT_INCLUDE_DIR}")
message(${PROJECT_INCLUDE_DIR})

add_library(cpphttplib STATIC "${PROJECT_INCLUDE_DIR}/cpphttplib/httplib.cc")

target_include_directories(cpphttplib INTERFACE "${PROJECT_INCLUDE_DIR}")