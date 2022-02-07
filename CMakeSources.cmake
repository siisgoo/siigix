####################
#  install prefix  #
####################

set(INSTALL_PREFIX "/usr/local") # change to CACHE

###################
#  project names  #
###################

set(GENERAL_PROJECT_NAME    general)
set(TCP_SERVER_PROJECT_NAME tcp-server)

##########################
#  projects directories  #
##########################

set(TCP_SERVER_DIR ${CMAKE_SOURCE_DIR}/tcp-server)
set(GENERAL_DIR    ${CMAKE_SOURCE_DIR}/general)

#######################
#  projects versions  #
#######################

set(SIIGIX_VERSION             0.0.1)
set(GENERAL_PROJECT_VERSION    0.0.1)
set(TCP_SERVER_PROJECT_VERSION 0.0.1)

##########################
#  projects descrptions  #
##########################

set(SIIGIX_DESCRIPTION     "INet utils")
set(TCP_SERVER_DESCRIPTION "TCP socket based server")
set(GENERAL_DESCRIPTION    "General siigix utils")

########################
#  tcp-server sources  #
########################

set(TCP_SERVER_SOURCES_LIST
    ${TCP_SERVER_DIR}/src/Server.cpp
    ${TCP_SERVER_DIR}/src/ServerClient.cpp
)

set(TCP_SERVER_HEADERS_PUBLIC
    ${TCP_SERVER_DIR}/include/${TCP_SERVER_PROJECT_NAME}.hpp
)

set(TCP_SERVER_HEADERS_PRIVATE
    ${TCP_SERVER_DIR}/include/${TCP_SERVER_PROJECT_NAME}/Server.hpp
)

set(TCP_SERVER_INCLUDE_DIRS
    ${TCP_SERVER_DIR}/include
)

#####################
#  general sources  #
#####################

set(GENERAL_SOURCES_LIST
    ${GENERAL_DIR}/src/IOBuff.cpp
    ${GENERAL_DIR}/src/Socket.cpp
    ${GENERAL_DIR}/src/Protocol.cpp
    ${GENERAL_DIR}/src/ThreadPool.cpp
)

set(GENERAL_HEADERS_PUBLIC
    ${GENERAL_DIR}/include/General.hpp
)

set(GENERAL_HEADERS_PRIVATE
    ${GENERAL_DIR}/include/${GENERAL_PROJECT_NAME}/Utils.hpp
    ${GENERAL_DIR}/include/${GENERAL_PROJECT_NAME}/ThreadPool.hpp
    ${GENERAL_DIR}/include/${GENERAL_PROJECT_NAME}/IOBuff.hpp
    ${GENERAL_DIR}/include/${GENERAL_PROJECT_NAME}/Log.hpp
    ${GENERAL_DIR}/include/${GENERAL_PROJECT_NAME}/Status.hpp
    ${GENERAL_DIR}/include/${GENERAL_PROJECT_NAME}/Protocol.hpp
    ${GENERAL_DIR}/include/${GENERAL_PROJECT_NAME}/Socket.hpp
    ${GENERAL_DIR}/include/${GENERAL_PROJECT_NAME}/BitFlag.hpp
)

set(GENERAL_INCLUDE_DIRS
    ${GENERAL_DIR}/include
)
