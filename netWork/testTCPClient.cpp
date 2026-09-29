//
// Created by 33550 on 2026/9/11.
//
#include "myTCP.h"
#include <iostream>

static std::string hostname = "127.0.0.1";
static int port = 12001;
static int bufsize = 1024;

int main()
{
    myTCP::TCPClient client{hostname , port};
    client.con();
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
