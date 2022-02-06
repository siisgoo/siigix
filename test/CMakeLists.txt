cmake_minimum_required(VERSION 3.21)
project(siigix-test-server LANGUAGES CXX VERSION 0.0.1 DESCRIPTION "INet utils")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#########################
#  YCM export commands  #
#########################

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
IF( EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json"  )
    EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
    ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    )
ENDIF()

add_subdirectory(tcp-server)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC
    tcp-server
)
