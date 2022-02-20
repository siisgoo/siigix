cmake_minimum_required(VERSION 3.21)
project(${NET_MODULE_NAME} LANGUAGES CXX VERSION ${NET_MODULE_VERSION} DESCRIPTION ${NET_MODULE_DESCRIPTION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#####################
#  findig packages  #
#####################

find_package(OpenSSL REQUIRED)

#########################
#  building executable  #
#########################

add_library(${PROJECT_NAME} SHARED ${NET_SOURCES_LIST})
add_library(sgx::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

################################
#  setting project properties  #
################################

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

#######################
#  linking libraries  #
#######################

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${GENERAL_MODULE_NAME}
)

################################
#  setting project properties  #
################################

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME    "${PROJECT_NAME}"
    SUFFIX         ""
    PREFIX         ""
    VERSION        "${PROJECT_VERSION}"
    PUBLIC_HEADER  "${NET_HEADERS_PUBLIC}"
    PRIVATE_HEADER "${NET_HEADERS_PRIVATE}"
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${NET_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${OPENSSL_INCLUDE_DIR}
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
        DESTINATION ${CMAKE_SOURCE_DIR}
)

