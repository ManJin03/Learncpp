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
        port = atoi(argv[1]);
    }
    myTCP::TCPServer server(port);
    std::cout << "server started(port: " << port << ")\n";
    while (true) {
        myTCP::TCPbase con = server.acceptm();
        std::cout << "connected\n";
        while (true) {
            auto msg = con.recvm(bufsize);
            if (msg.empty()) break;
            std::cout << "recieved " << msg << std::endl;
            auto reply = "TCP:" + msg;
            con.sendm(reply);
            if (msg == "exit") {
                exit(0);
            }
        }
    }
    return 0;
}
