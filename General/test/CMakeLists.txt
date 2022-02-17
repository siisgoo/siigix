cmake_minimum_required(VERSION 3.21)
project(siigix-general-test LANGUAGES CXX VERSION 0.0.1 DESCRIPTION "Unit tests")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(xcfg_read xcfg_read.cpp)

target_link_libraries(xcfg_read PUBLIC
    siigix::General
# gtest_main
)

# include(GoogleTest)
# gtest_discover_tests(${PROJECT_NAME})
