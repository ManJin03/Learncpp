//
// Created by 33550 on 2026/9/11.
//
#include "myTCP.h"
#include <iostream>

static std::string hostname = "127.0.0.1";
static int port = 12001;
static int bufsize = 1024;

int main(int argc , char* argv[])
{
    if (argc > 1) {
        hostname = argv[1];
        if (argc > 2) {
            port = atoi(argv[2]);
        }
    }
    myTCP::TCPClient client{hostname , port};
    client.con();
    std::cout << "connected " << hostname << ":" << port << '\n';
    while (true) {
        std::string msg;
        std::cout << "Input: ";
        getline(std::cin , msg);
        client.sendm(msg);
        auto reply = client.recvm(bufsize);
        std::cout << reply << std::endl;
        if (msg == "exit") {
            break;
        }
    }
    return 0;
}
