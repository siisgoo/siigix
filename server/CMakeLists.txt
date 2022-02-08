cmake_minimum_required(VERSION 3.21)
project(${SERVER_PROJECT_NAME} LANGUAGES CXX VERSION ${SERVER_PROJECT_VERSION} DESCRIPTION ${SERVER_DESCRIPTION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#####################
#  findig packages  #
#####################

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

#########################
#  building executable  #
#########################

add_library(${PROJECT_NAME} SHARED ${SERVER_SOURCES_LIST})
add_library(siigix::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

#######################
#  linking libraries  #
#######################

target_link_libraries(${PROJECT_NAME} PUBLIC
    general
    Threads::Threads)

################################
#  setting project properties  #
################################

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME    "${PROJECT_NAME}"
    SUFFIX         ""
    PREFIX         ""
    VERSION        "${PROJECT_VERSION}"
    PUBLIC_HEADER  "${SERVER_HEADERS_PUBLIC}"
    PRIVATE_HEADER "${SERVER_HEADERS_PRIVATE}"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    $<BUILD_INTERFACE:${SERVER_INCLUDE_DIRS}/${PROJECT_NAME}>
    $<INSTALL_INTERFACE:include/>
    ${GENERAL_INCLUDE_DIRS}
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
    PUBLIC_HEADER DESTINATION ${CMAKE_SOURCE_DIR}/include/siigix
    PRIVATE_HEADER DESTINATION ${CMAKE_SOURCE_DIR}/include/siigix/${PROJECT_NAME}
)
