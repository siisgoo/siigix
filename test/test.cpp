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
on_data_recive(const std::string& data, TCP::Server::Client& cli)
{
    std::cout << "data recved" << std::endl;
}

/* TEST(TCP_Server, Init) { */
/* } */

int
main(int argc, char** argv)
{
    TCP::Server server(8080);
    server.setConnHandler(on_connect);
    server.setDataHandler(on_data_recive);
    server.setDisconnHandler(on_disconnect);

    try {
        int rc = server.start();
        /* if (rc == TCP::Server::ServerStatus::running) { */
            std::cerr << "Server started succesfully!" << std::endl;
            std::cerr.flush();
            server.joinLoop();
        /* } else { */
        /*     std::cerr << "Server initialization FAILED!" << std::endl; */
        /*     std::cerr.flush(); */
        /* } */
    } catch (std::exception& except) {
        std::cerr << except.what();
    } catch (const char* errmsg) {
        std::cerr << errmsg << std::endl;
    }

    /* ::testing::InitGoogleTest(&argc, argv); */
    /* return RUN_ALL_TESTS(); */

    /* try { */
    /*     ListenSocket server(8080); */
    /*     while (true) { */
    /*         DataSocket acc = server.Accept(); */
    /*         TCP::Protocol accTCP(acc); */

    /*         std::string recvMsg; */
    /*         accTCP.recvMessage(recvMsg); */
    /*         std::cout << recvMsg << std::endl; */
    /*         std::cout.flush(); */

    /*         accTCP.sendMessage("", "Sending JeeZZZZZZZZZZ!!!"); */
    /*     } */
    /* } catch (std::exception& except) { */
    /*     std::cerr << except.what(); */
    /* } */

    return 0;
}
