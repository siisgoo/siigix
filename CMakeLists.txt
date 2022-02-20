cmake_minimum_required(VERSION 3.21)
include(CMakeSources.cmake)
project(${SIIGIX_LIB_NAME} LANGUAGES CXX VERSION ${SIIGIX_VERSION} DESCRIPTION ${SIIGIX_DESCRIPTION})

##########################
#  setting c++ standart  #
##########################

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

#####################
#  sub directories  #
#####################

add_subdirectory(${GENERAL_MODULE_NAME})
add_subdirectory(${NET_MODULE_NAME})
add_subdirectory(${DATA_MODULE_NAME})

######################
#  creating package  #
######################

# install(
#     EXPORT      ${PROJECT_NAME}Targets
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
#     NAMESPACE   ${PROJECT_NAME}::
#     FILE        ${PROJECT_NAME}Targets.cmake
# )

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "Config.cmake.in"
    "siigixConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/siigix
    PATH_VARS
        CMAKE_INSTALL_LIBDIR
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/siigixConfigVersion.cmake
    VERSION       ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/siigixConfig.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/siigixConfigVersion.cmake"
          DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/siigix"
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
