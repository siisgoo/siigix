cmake_minimum_required(VERSION 3.21)
project(${GENERAL_MODULE_NAME} LANGUAGES CXX VERSION ${GENERAL_MODULE_VERSION} DESCRIPTION ${GENERAL_MODULE_DESCRIPTION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

###################
#  find packages  #
###################

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

#########################
#  building executable  #
#########################

add_library(${PROJECT_NAME} SHARED ${GENERAL_SOURCES_LIST})
add_library(${SIIGIX_LIB_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC
    Threads::Threads)

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
    $<BUILD_INTERFACE:${GENERAL_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include/>
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${GENERAL_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

######################
#  installing files  #
######################

include(GNUInstallDirs)
install(
    TARGETS ${PROJECT_NAME}
    EXPORT siigixTargets
    ARCHIVE        DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY        DESTINATION ${CMAKE_SOURCE_DIR}/lib
    PUBLIC_HEADER  DESTINATION ${CMAKE_SOURCE_DIR}/include/siigix/${PROJECT_NAME}
    PRIVATE_HEADER DESTINATION ${CMAKE_SOURCE_DIR}/include/siigix/${PROJECT_NAME}
)

install(
    EXPORT      siigixTargets
    NAMESPACE   ${SIIGIX_LIB_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

#############
#  testing  #
#############

add_subdirectory(test)
