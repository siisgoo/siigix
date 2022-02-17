####################
#  install prefix  #
####################

set(INSTALL_PREFIX "/usr/local") # change to CACHE

###################
#  project names  #
###################

set(SIIGIX_LIB_NAME     siigix)
set(GENERAL_MODULE_NAME General)
set(NET_MODULE_NAME     Net)

##########################
#  projects directories  #
##########################

set(NET_DIR             ${CMAKE_SOURCE_DIR}/${NET_MODULE_NAME})
set(NET_SOUCES_DIR      ${NET_DIR}/src)
set(NET_INCLUDE_DIR     ${NET_DIR}/include/${SIIGIX_LIB_NAME}/${NET_MODULE_NAME})
set(GENERAL_DIR         ${CMAKE_SOURCE_DIR}/${GENERAL_MODULE_NAME})
set(GENERAL_SOUCES_DIR  ${GENERAL_DIR}/src)
set(GENERAL_INCLUDE_DIR ${GENERAL_DIR}/include/${SIIGIX_LIB_NAME}/${GENERAL_MODULE_NAME})

#######################
#  projects versions  #
#######################

set(SIIGIX_VERSION         0.0.1)
set(GENERAL_MODULE_VERSION 0.0.1)
set(NET_MODULE_VERSION     0.0.1)

##########################
#  projects descrptions  #
##########################

set(SIIGIX_DESCRIPTION         "Educational network library")
set(NET_MODULE_DESCRIPTION     "Network utils")
set(GENERAL_MODULE_DESCRIPTION "General siigix utils")

#################
#  net sources  #
#################

set(NET_SOURCES_LIST
    ${NET_SOUCES_DIR}/INData.cpp
    ${NET_SOUCES_DIR}/DNS.cpp
    ${NET_SOUCES_DIR}/HostEntry.cpp
    ${NET_SOUCES_DIR}/IPAddress.cpp
    ${NET_SOUCES_DIR}/SocketAddress.cpp
    ${NET_SOUCES_DIR}/Protocol.cpp
    ${NET_SOUCES_DIR}/Socket.cpp
    ${NET_INCLUDE_DIR}/Socket.inl
    ${NET_SOUCES_DIR}/Server.cpp
    ${NET_SOUCES_DIR}/ServerClient.cpp
)

set(NET_HEADERS_PUBLIC
    ${NET_INCLUDE_DIR}/DNS.hpp
    ${NET_INCLUDE_DIR}/HostEntry.hpp
    ${NET_INCLUDE_DIR}/Client.hpp
    ${NET_INCLUDE_DIR}/INData.hpp
    ${NET_INCLUDE_DIR}/INetDefs.hpp
    ${NET_INCLUDE_DIR}/ServerMarkupReader.hpp
    ${NET_INCLUDE_DIR}/ServerMarkup.hpp
    ${NET_INCLUDE_DIR}/IPAddress.hpp
    ${NET_INCLUDE_DIR}/Protocol.hpp
    ${NET_INCLUDE_DIR}/Server.hpp
    ${NET_INCLUDE_DIR}/ServerWorkDirectory.hpp
    ${NET_INCLUDE_DIR}/Socket.hpp
    ${NET_INCLUDE_DIR}/IPAddress.hpp
    ${NET_INCLUDE_DIR}/SocketAddress.hpp
    ${NET_INCLUDE_DIR}/SocketOptions.hpp
    ${NET_INCLUDE_DIR}/Socket.inl
)

set(NET_HEADERS_PRIVATE
)

#####################
#  general sources  #
#####################

set(GENERAL_SOURCES_LIST
    ${GENERAL_SOUCES_DIR}/Formatter.cpp
    ${GENERAL_SOUCES_DIR}/Encoding.cpp
    ${GENERAL_SOUCES_DIR}/MarkupObject.cpp
    ${GENERAL_SOUCES_DIR}/MarkupReader.cpp
    ${GENERAL_SOUCES_DIR}/MarkupSignatures.cpp
)

set(GENERAL_HEADERS_PUBLIC
    ${GENERAL_INCLUDE_DIR}/ByteOrder.hpp
    ${GENERAL_INCLUDE_DIR}/Command.hpp
    ${GENERAL_INCLUDE_DIR}/Encoding.hpp
    ${GENERAL_INCLUDE_DIR}/TextConverter.hpp
    ${GENERAL_INCLUDE_DIR}/BitFlag.hpp
    ${GENERAL_INCLUDE_DIR}/eprintf.hpp
    ${GENERAL_INCLUDE_DIR}/Formatter.hpp
    ${GENERAL_INCLUDE_DIR}/MarkupReader.hpp
    ${GENERAL_INCLUDE_DIR}/MarkupSignatures.hpp
    ${GENERAL_INCLUDE_DIR}/MarkupObject.hpp
    ${GENERAL_INCLUDE_DIR}/Logger.hpp
    ${GENERAL_INCLUDE_DIR}/Status.hpp
    ${GENERAL_INCLUDE_DIR}/thread_pool.hpp
    ${GENERAL_INCLUDE_DIR}/Types.hpp
)

set(GENERAL_HEADERS_PRIVATE
)
