####################
#  install prefix  #
####################

set(INSTALL_PREFIX "/usr/local") # change to CACHE

###################
#  project names  #
###################

set(GENERAL_PROJECT_NAME general)
set(SERVER_PROJECT_NAME  server)
set(CLIENT_PROJECT_NAME  client)

##########################
#  projects directories  #
##########################

set(SERVER_DIR  ${CMAKE_SOURCE_DIR}/server)
set(CLIENT_DIR  ${CMAKE_SOURCE_DIR}/client)
set(GENERAL_DIR ${CMAKE_SOURCE_DIR}/general)

#######################
#  projects versions  #
#######################

set(SIIGIX_VERSION          0.0.1)
set(GENERAL_PROJECT_VERSION 0.0.1)
set(SERVER_PROJECT_VERSION  0.0.1)
set(CLIENT_PROJECT_VERSION  0.0.1)

##########################
#  projects descrptions  #
##########################

set(SIIGIX_DESCRIPTION  "INet utils")
set(SERVER_DESCRIPTION  "Socket based server")
set(CLIENT_DESCRIPTION  "Socket based client")
set(GENERAL_DESCRIPTION "General siigix utils")

####################
#  server sources  #
####################

set(SERVER_SOURCES_LIST
    ${SERVER_DIR}/src/Server.cpp
    ${SERVER_DIR}/src/ServerClient.cpp
)

set(SERVER_HEADERS_PUBLIC
    ${SERVER_DIR}/include/Server.hpp
)

set(SERVER_HEADERS_PRIVATE
    ${SERVER_DIR}/include/${SERVER_PROJECT_NAME}/Server.hpp
)

set(SERVER_INCLUDE_DIRS
    ${SERVER_DIR}/include
)

####################
#  client sources  #
####################

set(CLIENT_SOURCES_LIST
    ${CLIENT_DIR}/src/client.cpp
)

set(CLIENT_HEADERS_PUBLIC
    ${CLIENT_DIR}/include/Client.hpp
)

set(CLIENT_HEADERS_PRIVATE
    ${CLIENT_DIR}/include/${CLIENT_PROJECT_NAME}/Server.hpp
)

set(CLIENT_INCLUDE_DIRS
    ${CLIENT_DIR}/include
)

#####################
#  general sources  #
#####################

set(GENERAL_SOURCES_LIST
    ${GENERAL_DIR}/src/INData.cpp
    ${GENERAL_DIR}/src/Socket.cpp
    ${GENERAL_DIR}/include/${GENERAL_PROJECT_NAME}/Socket.inl
    ${GENERAL_DIR}/src/Protocol.cpp
    ${GENERAL_DIR}/src/ThreadPool.cpp
)

set(GENERAL_HEADERS_PUBLIC
    ${GENERAL_DIR}/include/General.hpp
)

set(GENERAL_HEADERS_PRIVATE
    ${GENERAL_DIR}/include/${GENERAL_PROJECT_NAME}/Utils.hpp
    ${GENERAL_DIR}/include/${GENERAL_PROJECT_NAME}/ThreadPool.hpp
    ${GENERAL_DIR}/include/${GENERAL_PROJECT_NAME}/INData.hpp
    ${GENERAL_DIR}/include/${GENERAL_PROJECT_NAME}/Log.hpp
    ${GENERAL_DIR}/include/${GENERAL_PROJECT_NAME}/Status.hpp
    ${GENERAL_DIR}/include/${GENERAL_PROJECT_NAME}/Protocol.hpp
    ${GENERAL_DIR}/include/${GENERAL_PROJECT_NAME}/Socket.hpp
    ${GENERAL_DIR}/include/${GENERAL_PROJECT_NAME}/BitFlag.hpp
)

set(GENERAL_INCLUDE_DIRS
    ${GENERAL_DIR}/include
)
