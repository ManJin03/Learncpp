//
// Created by 33550 on 2026/9/29.
//

#ifndef LEARNCPP_MYTCP_H
#define LEARNCPP_MYTCP_H
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace myTCP
{
    class TCPbase
    {
        using socket_t = int;
        socket_t TCPsocket{};

    public:
        TCPbase()
        {
            TCPsocket = socket(AF_INET , SOCK_STREAM , 0);
        }

        TCPbase(int sofd) : TCPsocket{sofd} {};

        ~TCPbase()
        {
            close(TCPsocket);
        }

        [[nodiscard]] socket_t socketfd() const { return TCPsocket; };


        void sendm(const std::string& message) const
        {
            if (const auto n = send(TCPsocket , message.c_str() , message.size() , 0) ; n < 0) {
                perror("send");
            }
        }

        [[nodiscard]] std::string recvm(const int bufsize) const
        {
            std::string buf(bufsize , '\0');
            const auto n = recv(TCPsocket , buf.data() , bufsize , 0);
            if (n < 0) {
                perror("recv");
                return {};
            }
            if (n == 0) {
                perror("close");
                return {};
            }
            buf.resize(n);
            return buf;
        }
    };

    class TCPClient : public TCPbase
    {
        sockaddr_in host{};

    public:
        TCPClient() = delete;

        TCPClient(const TCPClient&) = delete;

        TCPClient(const std::string& hostname = "127.0.0.1" , const int port = 12001)
        {
            host.sin_family = AF_INET;
            host.sin_port = htons(port);
            inet_pton(AF_INET , hostname.c_str() , &host.sin_addr);
        }

        bool con()
        {
            if (connect(socketfd() , reinterpret_cast<sockaddr*>(&host) , sizeof(host)) < 0) {
                perror("connect");
                return false;
            };
            return true;
        }
    };

    class TCPServer : public TCPbase
    {
        int port{};

    public:
        TCPServer() = delete;

        TCPServer(const TCPServer&) = delete;

        TCPServer(const int port = 12001 , const int lis = 1) : port{port}
        {
            sockaddr_in host{};
            host.sin_family = AF_INET;
            host.sin_port = htons(port);
            host.sin_addr.s_addr = INADDR_ANY;
            if (bind(socketfd() , reinterpret_cast<sockaddr*>(&host) , sizeof(host)) < 0) {
                perror("bind");
            }
            listen(socketfd() , lis);
        }

        [[nodiscard]] TCPbase acceptm() const
        {
            sockaddr host{};
            socklen_t len = sizeof(host);
            auto confd = accept(socketfd() , &host , &len);
            if (confd < 0) {
                perror("accept");
                return {};
            }
            return TCPbase{confd};
        }
    };
}
#endif //LEARNCPP_MYTCP_H
