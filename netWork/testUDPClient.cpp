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
    myUDP::UDPClient client{hostname , port};
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
