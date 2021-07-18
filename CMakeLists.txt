cmake_minimum_required(VERSION 3.17)
project(MoonJamRelayServer)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-D_GLIBCXX_USE_NANOSLEEP -DASIO_STANDALONE -D_WEBSOCKETPP_CPP11_STRICT_ -DNOMINMAX)

if (UNIX)
    SET(CMAKE_CXX_FLAGS "-pthread")
endif()

include_directories(libs/asio/include)
#include_directories(libs/httpserver/include)

add_subdirectory(libs/cpphttplib)
add_subdirectory(libs/websocketpp)

include_directories(src)

add_executable(MoonJamRelayServer
        src/main.cpp
        src/APIServer.cpp
        src/Helpers.cpp
        src/KeyManager.cpp
        src/PacketAccumulator.cpp
        src/RelayServer.cpp
        src/RoomManager.cpp
        src/WebsocketServer.cpp)

target_link_libraries(MoonJamRelayServer
        cpphttplib
        websocketpp)