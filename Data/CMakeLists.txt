cmake_minimum_required(VERSION 3.21)
project(${DATA_MODULE_NAME} LANGUAGES CXX VERSION ${DATA_MODULE_VERSION} DESCRIPTION ${DATA_MODULE_DESCRIPTION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

###################
#  find packages  #
###################

set(THREADS_PREFER_PTHREAD_FLAG ON)

#########################
#  building executable  #
#########################

# install(
#     DIRECTORY ${DATA_INCLUDE_DIR}
#         DESTINATION ${CMAKE_SOURCE_DIR}/include/siigix
# )

add_library(${PROJECT_NAME} SHARED ${DATA_SOURCES_LIST})
add_library(${SIIGIX_LIB_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC
    siigix::General)

################################
#  setting project properties  #
################################

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME    "${PROJECT_NAME}"
    SUFFIX         ""
    PREFIX         ""
    VERSION        "${PROJECT_VERSION}"
    PUBLIC_HEADER  "${DATA_HEADERS_PUBLIC}"
    PRIVATE_HEADER "${DATA_HEADERS_PRIVATE}"
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${DATA_DIR}/include>
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
    EXPORT siigixTargets
    ARCHIVE        DESTINATION ${CMAKE_INSTALL_LIBDIR}
    # LIBRARY        DESTINATION ${CMAKE_SOURCE_DIR}/lib
    # PUBLIC_HEADER  DESTINATION ${CMAKE_SOURCE_DIR}/include/siigix/${PROJECT_NAME}
    # PRIVATE_HEADER DESTINATION ${CMAKE_SOURCE_DIR}/include/siigix/${PROJECT_NAME}
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
