/* #include <gtest/gtest.h> */

#include <Server.hpp>
#include <General.hpp>

#include <iostream>
#include <string>
#include <sys/types.h>

using namespace siigix;

void
on_connect(TCP::Server::Client& cli)
{
    const char *hello = "HTTP/1.1 200 OK\nContent-Type: message/http\nContent-Length: 19\n\n<p>Hello world!</p>";
    cli.sendMessage(hello);
    std::cout << "-----------------new client connected.------------" << std::endl << "host ip: " << cli.getIP() << std::endl << "-----------------" << std::endl;
}

void
on_disconnect(TCP::Server::Client& cli)
{
    std::cout << "-----------------client disconnected.------------" << std::endl << "host ip: " << cli.getIP() <<
       "-------------------------------" << std::endl;
}

void
on_data_recive(const std::string& data, TCP::Server::Client& cli)
{
    std::cout << "-----------------data recved:------------ " << std::endl << data << "-------------------end data------------" << std::endl;
}

int
main(int argc, char** argv)
{
    port_t port = (argc > 1) ? atoi(argv[1]) : 8090;
    TCP::Server server(port);
    server.setConnHandler(on_connect);
    server.setDataHandler(on_data_recive);
    server.setDisconnHandler(on_disconnect);

    try {
        if (server.start() == TCP::Server::ServerStatus::running) {
            std::cerr << "Server started succesfully on port: " << port << std::endl;
            server.joinLoop();
        } else {
            std::cerr << "AHTUNG! Server initialization FAILED!" << std::endl;
            std::cerr << "Exit." << std::endl;
            return 1;
        }
    } catch (std::system_error& sys_err) {
        std::cerr << sys_err.what() << " Code: " << sys_err.code();
    } catch (std::exception& except) {
        std::cerr << except.what();
    } catch (const char* errmsg) {
        std::cerr << errmsg << std::endl;
    } catch (...) {
        std::exception_ptr p = std::current_exception();
        std::cerr <<(p ? p.__cxa_exception_type()->name() : "null") << std::endl;
    }

    return 0;
}
