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
add_library(siigix::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        Threads::Threads
)

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

target_include_directories(${PROJECT_NAME}
    PUBLIC
        # $<BUILD_INTERFACE:${GENERAL_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${GENERAL_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

######################
#  installing files  #
######################

include(GNUInstallDirs)
install(
    TARGETS ${PROJECT_NAME}
    EXPORT  siigixTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
    EXPORT      siigixTargets
    NAMESPACE   ${SIIGIX_LIB_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
        DESTINATION ${CMAKE_SOURCE_DIR}/
)
