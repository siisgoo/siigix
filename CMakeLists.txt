cmake_minimum_required(VERSION 3.21)

#####################
#  MODULE SETTINGS  #
#####################

include(CMakeSources.cmake)

project(siigix LANGUAGES CXX VERSION ${SIIGIX_VERSION} DESCRIPTION ${SIIGIX_DESCRIPTION})

include(CTest)

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

add_subdirectory(general)
add_subdirectory(tcp-server)

######################
#  creating package  #
######################

install(
    EXPORT ${PROJECT_NAME}Targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    NAMESPACE ${PROJECT_NAME}::
    FILE ${PROJECT_NAME}Targets.cmake
)

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
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/siigixConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/siigixConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/siigix"
)

#############################
#  installing export files  #
#############################

install( TARGETS tcp-server                    DESTINATION ${INSTALL_PREFIX}/lib )
install( TARGETS general                       DESTINATION ${INSTALL_PREFIX}/lib )
install( FILES   ${TCP_SERVER_HEADERS_PUBLIC}  DESTINATION ${INSTALL_PREFIX}/include/ )
install( FILES   ${TCP_SERVER_HEADERS_PRIVATE} DESTINATION ${INSTALL_PREFIX}/include/tcp-server/ )
install( FILES   ${GENERAL_HEADERS_PUBLIC}     DESTINATION ${INSTALL_PREFIX}/include/ )
install( FILES   ${GENERAL_HEADERS_PRIVATE}    DESTINATION ${INSTALL_PREFIX}/include/general )

#############
#  testing  #
#############

# if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
# endif (BUILD_TESTING)
