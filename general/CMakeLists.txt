cmake_minimum_required(VERSION 3.21)
project(${GENERAL_PROJECT_NAME} LANGUAGES CXX VERSION ${GENERAL_PROJECT_VERSION} DESCRIPTION ${GENERAL_DESCRIPTION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#########################
#  building executable  #
#########################

add_library(${PROJECT_NAME} SHARED ${GENERAL_SOURCES_LIST})
add_library(siigix::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

################################
#  setting project properties  #
################################

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME    "${PROJECT_NAME}"
    SUFFIX         ""
    PREFIX         ""
    VERSION        "${PROJECT_VERSION}"
    PUBLIC_HEADER  "${GENERAL_HEADERS_PUBLIC}"
    PRIVATE_HEADER "${GENERAL_HEADERS_PRIVATE}"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    $<BUILD_INTERFACE:${GENERAL_INCLUDE_DIRS}/${PROJECT_NAME}>
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
    LIBRARY        DESTINATION ${CMAKE_SOURCE_DIR}/lib
    PUBLIC_HEADER  DESTINATION ${CMAKE_SOURCE_DIR}/include
    PRIVATE_HEADER DESTINATION ${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}
)
