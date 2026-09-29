//
// Created by 33550 on 2026/9/11.
//
#include "myUDP.h"
#include <iostream>

static std::string hostname = "127.0.0.1";
static int port = 12000;
static int bufsize = 1024;

int main()
{
    myUDP::UDPServer server{port};
    while (true) {
        sockaddr addr{};
        std::string msg = server.recvm(bufsize , &addr);
        auto reply = "UDP:" + msg;
        std::cout << "recieved " << msg << std::endl;
        server.sendm(reply , &addr);
        if (msg == "exit") {
            break;
        }
    }
    return 0;
}
