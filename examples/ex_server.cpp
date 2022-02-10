/* #include <gtest/gtest.h> */

#include <Server.hpp>
#include <General.hpp>

#include <iostream>
#include <string>
#include <sys/types.h>

using namespace siigix;

const std::string
placeInMidOf(std::string str, const char c, int len = 200)
{
    std::string ret;
    ret.replace(0, len, &c);
    ret.replace(len-str.length()/2, str.length(), str);

    return ret;
}

void
on_connect(TCP::ServerClientTCP& cli)
{
    std::cout << placeInMidOf("new client connected!", '-') << std::endl;
    std::cout << "Remote host IP: " << cli.getIP() << std::endl;
    std::cout << placeInMidOf("", '-') << std::endl;

    cli.sendMessage("Hello, you connected to siigix server\n");
}

void
on_disconnect(TCP::ServerClientTCP& cli)
{
    std::cout << placeInMidOf("client disconnected!", '-') << std::endl;
    std::cout << "Disconnected host IP: " << cli.getIP() << std::endl;
    std::cout << placeInMidOf("", '-') << std::endl;
    std::cout.flush();
}

void
on_data_recive(const std::string& data, TCP::ServerClientTCP& cli)
{
    std::cout << placeInMidOf("data recived from host: " + cli.getIP(), '-') << std::endl;
    std::cout << data << std::endl;
    std::cout << placeInMidOf("END DATA", '-') << std::endl;

    std::stringstream response_body, response;

    response_body << "<title>Test C++ HTTP Server</title>\n"
        << "<h1>Test page</h1>\n"
        << "<p>This is body of the test page...</p>\n"
        << "<h2>Request headers</h2>\n"
        << "<pre>" << data << "</pre>\n"
        << "<em><small>Test C++ Http Server</small></em>\n";

    response << "HTTP/1.1 200 OK\r\n"
        << "Version: HTTP/1.1\r\n"
        << "Content-Type: text/html; charset=utf-8\r\n"
        << "Content-Length: " << response_body.str().length()
        << "\r\n\r\n"
        << response_body.str();

    cli.sendMessage(response.str());
}

int
main(int argc, char** argv)
{
    port_t port = (argc > 1) ? atoi(argv[1]) : 8000;
    TCP::ServerTCP server("127.0.0.1", port);
    server.setConnHandler(on_connect);
    server.setDataHandler(on_data_recive);
    server.setDisconnHandler(on_disconnect);

    try {
        if (server.start() == TCP::ServerTCP::ServerStatus::running) {
            std::cerr << "Server started succesfully on port: " << port << std::endl;
            server.joinLoop();
        } else {
            std::cerr << "AHTUNG! Server initialization FAILED!" << std::endl;
            std::cerr << "Exit." << std::endl;
            return 1;
        }
    }
    catch (std::system_error& sys_err) {
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
