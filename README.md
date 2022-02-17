# SIIGIX

![Lines of code](https://img.shields.io/tokei/lines/github.com/siisgoo/siigix?style=flat-square)

Moduled crosplatform C++ library(almoust framework)

# Table of content

- [About](#About)
    - [Used sources](#UsedSources)
    - [Supported platforms](#Platforms)
- [Modules](#Modules)
	- [General](#General)
    - [Net](#Net)
    - [Data](#Data)
    - [Crypto](#Crypto)
- [Integration](#Integration)
    - [Getting sources](#GettingSources)
    - [Integrate to cmake project](#CMakeInteg)

<a name="About"></a>
# About

sIIgIx - educationaly library for integration to my another educationaly projects.

<a name="UsedSources"></a>
## Used Sources
A lot of code copied from [Poco](https://github.com/pocoproject/poco)(IPAddress, SocketAddress, DNS has rewrited with non-factory methods), [Loki-Astari](https://github.com/Loki-Astari) object oriented socket model.
Project [bshoshany thread-pool](https://github.com/bshoshany/thread-pool) was fully copied.

<a name="Platforms"></a>
## Suppoted platforms

Now siigix supports GNU/Linux, other Unix(Solaris, FreeBSD) and Windows. But Im test only GNU/Linux version.

<a name="Modules"></a>
# Modules

siigix Library split to modules: General, Net, Data, Crypto

<a name="General"></a>
## General

Module includes Basic siigix Types, Data processing, multithreading tools/wrappers, Logger and Errors printing tools, interface Classes like a Comand...

-

<a name="Net"></a>
## Net

INet tools, sockets, socure sockets, Protocol factory, Server/Client builder

-

<a name="Data"></a>
## Data

Module includes Specific data processing: DBMS, Config readers(json, xml, sgx) 

-

<a name="Crypto"></a>
## Crypto

-

<a name="Integration"></a>
# Integration

<a name="GettingSources"></a>
## Getting sources

<a name="CMakeInteg"></a>
## CMake integration

For siigix integration throw CMake:
``cmake
find_package(
``
