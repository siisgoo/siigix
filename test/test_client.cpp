/* #include <gtest/gtest.h> */

#include <tcp-server.hpp>
#include <General.hpp>

#include <iostream>
#include <string>
#include <sys/types.h>

using namespace siigix;

int
main(int argc, char** argv)
{
    try {
        ConnectSocket conn("127.0.0.1", 8080);
        TCP::Protocol proto(conn);
        proto.sendMessage("", "LOOOOOOOOK!!! this is server send message");

        std::string msg;
        proto.recvMessage(msg);
        std::cout << "Recived message: " << msg << std::endl;
    } catch (std::exception& except) {
        std::cerr << except.what();
    }

    return 0;
}
