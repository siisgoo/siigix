cmake_minimum_required(VERSION 3.21)
project(siigix-test-server LANGUAGES CXX VERSION 0.0.1 DESCRIPTION "INet utils")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(test_server test.cpp)
add_executable(test_client test_client.cpp)

target_link_libraries(test_server PUBLIC
    siigix::general
    siigix::server
    # gtest_main
)

target_link_libraries(test_client PUBLIC
    siigix::general
    siigix::server
    # gtest_main
)

# include(GoogleTest)
# gtest_discover_tests(${PROJECT_NAME})
