cmake_minimum_required(VERSION 3.21)
project(siigix-examples LANGUAGES CXX VERSION 0.0.1 DESCRIPTION "siigix examples")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ex-server ex_server.cpp)
add_executable(ex-client ex_client.cpp)

target_link_libraries(ex-server PUBLIC
    siigix::general
    siigix::server
)

target_link_libraries(ex-client PUBLIC
    siigix::general
    siigix::client
)
