/* #include <gtest/gtest.h> */

#include <tcp-server.hpp>
#include <General.hpp>

#include <iostream>
#include <string>
#include <sys/types.h>

using namespace siigix;

void
on_connect(TCP::Server::Client& cli)
{
    std::cout << "new client connected" << std::endl;
}

void
on_disconnect(TCP::Server::Client& cli)
{
    std::cout << "client disconnected" << std::endl;
}

void
on_data_recive(const IOBuff& data, TCP::Server::Client& cli)
{
    std::cout << "data recved" << std::endl;
}

/* TEST(TCP_Server, Init) { */
/* } */

int
main(int argc, char** argv)
{
    /* TCP::Server server; */
    /* server.setConnHandler(on_connect); */
    /* server.setDataHandler(on_data_recive); */
    /* server.setDisconnHandler(on_disconnect); */

    /* try { */
    /*     int rc = server.start(); */
    /*     if (rc == TCP::Server::ServerStatus::running) { */
    /*         std::cerr << "Server started succesfully!" << std::endl; */
    /*         server.joinLoop(); */
    /*     } else { */
    /*         std::cerr << "Server initialization FAILED!" << std::endl; */
    /*     } */
    /* } catch (std::exception& except) { */
    /*     std::cerr << except.what(); */
    /* } */

    /* ::testing::InitGoogleTest(&argc, argv); */
    /* return RUN_ALL_TESTS(); */

    try {
        ListenSocket server(8080);
        while (true) {
            DataSocket acc = server.Accept();
            TCP::Protocol accTCP(acc);

            IOBuff buff;
            accTCP.recvMessage(buff);
            std::cout << static_cast<char*>(buff) << std::endl;

            IOBuff sendBuff;
            sendBuff.add("Jeez");
            accTCP.sendMessage("", sendBuff);
        }
    } catch (std::exception& except) {
        std::cerr << except.what();
    }

    return 0;
}
