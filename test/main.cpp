#include <siigix/tcp-server.hpp>

#include <iostream>
#include <string>
#include <sys/types.h>

std::string clientAddrToStr(TCP::Server::Client& cli) {
    __uint32_t ip = cli.getAddr().sin_addr.s_addr;
    __uint16_t port = cli.getAddr().sin_port;
    return std::string() + std::to_string(int(reinterpret_cast<char*>(&ip)[0])) + '.' +
        std::to_string(int(reinterpret_cast<char*>(&ip)[1])) + '.' +
        std::to_string(int(reinterpret_cast<char*>(&ip)[2])) + '.' +
        std::to_string(int(reinterpret_cast<char*>(&ip)[3])) + ':' +
        std::to_string(port);
}

int
main(int argc, char **argv)
{
    TCP::Server server(
            [](const IOBuff& data, TCP::Server::Client& cli) {
                std::cout << "Client: " << clientAddrToStr(cli) <<
                    "send data: " << data.len() << "bytes: " <<
                    (char *)data.read() << std::endl;
            },

            [](TCP::Server::Client& conn_cli) {
                /* std::cout << "Client: " << inet_to_str(conn_cli.getHost()) << " connected\n"; */
            },

            [](TCP::Server::Client& disconn_cli) {
                /* std::cout << "Client: " << inet_to_str(conn_cli.getHost()) << " disconnected\n"; */
            },

            false, 8000
    );

    int err;
    if ((err = server.start()) == TCP::ServerStatus::running) {
        server.joinLoop();
        return 0;
    } else {
        std::cerr << "Startup server ruinned!" << std::endl;
        std::cerr << "Error code: " << err << std::endl;
        return 1;
    }


    return 0;
}
