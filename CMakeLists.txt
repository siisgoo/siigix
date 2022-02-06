cmake_minimum_required(VERSION 3.21)
project(siigix LANGUAGES CXX VERSION ${SIIGIX_VERSION} DESCRIPTION ${SIIGIX_DESCRIPTION})

include(CMakeSources.cmake)

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

########################
#  installing headers  #
########################

install(
    DIRECTORY include/ DESTINATION ${INSTALL_PREFIX}/include/siigix
)
