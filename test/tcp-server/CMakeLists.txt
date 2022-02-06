cmake_minimum_required(VERSION 3.21)
project(tcp-server LANGUAGES CXX VERSION 0.0.1 DESCRIPTION "TCP/IP server")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#############
#  sources  #
#############

set(SOURCES_LIST
    src/iobuff.cpp
    src/serverClient.cpp
    src/socket.cpp
    src/threadPool.cpp
    src/server.cpp
)

set(HEADERS_PUBLIC
    include/${PROJECT_NAME}.hpp
)

set(HEADERS_PRIVATE
    include/${PROJECT_NAME}/threadPool.hpp
    include/${PROJECT_NAME}/iobuff.hpp
    include/${PROJECT_NAME}/server.hpp
    include/${PROJECT_NAME}/status.hpp
    include/${PROJECT_NAME}/types.hpp
    include/${PROJECT_NAME}/socket.hpp
    include/${PROJECT_NAME}/bitflag.hpp
)

#####################
#  findig packages  #
#####################

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

#########################
#  building executable  #
#########################

add_library(${PROJECT_NAME} SHARED ${SOURCES_LIST})
add_library(siigix::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

#######################
#  linking libraries  #
#######################

target_link_libraries(${PROJECT_NAME} Threads::Threads)

################################
#  setting project properties  #
################################

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME    "${PROJECT_NAME}"
    SUFFIX         ""
    PREFIX         ""
    VERSION        "${PROJECT_VERSION}"
    PUBLIC_HEADER  "${HEADERS_PUBLIC}"
    PRIVATE_HEADER "${HEADERS_PRIVATE}"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}>
    $<INSTALL_INTERFACE:include/>
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
    $<INSTALL_INTERFACE:include>
)

######################
#  installing files  #
######################

include(GNUInstallDirs)
install(
    TARGETS ${PROJECT_NAME}
    EXPORT siigixTargets
    ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}

    LIBRARY       DESTINATION ${CMAKE_SOURCE_DIR}/lib
    PUBLIC_HEADER DESTINATION ${CMAKE_SOURCE_DIR}/include
    PRIVATE_HEADER DESTINATION ${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}
)
