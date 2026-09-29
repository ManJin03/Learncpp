//
// Created by 33550 on 2026/9/29.
//

#ifndef LEARNCPP_MYUDP_H
#define LEARNCPP_MYUDP_H

#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace myUDP
{
    class UDPbase
    {
        using socket_t = int;
        socket_t UDPsocket{};

    public:
        UDPbase()
        {
            UDPsocket = socket(AF_INET , SOCK_DGRAM , 0);
        }

        ~UDPbase()
        {
            close(UDPsocket);
        }

        [[nodiscard]] socket_t socketfd() const { return UDPsocket; }

        void sendm(const std::string& message , const sockaddr* dst) const
        {
            sendto(socketfd() , message.c_str() , message.size() , 0 ,
                   dst , sizeof(sockaddr));
        }

        std::string recvm(int bufsize , sockaddr* src) const
        {
            std::string buf(bufsize , '\0');
            socklen_t len = sizeof(sockaddr);
            ssize_t n = recvfrom(socketfd() , buf.data() , bufsize , 0 ,
                                 src , &len);
            buf.resize(n);
            return buf;
        }
    };

    class UDPClient : public UDPbase
    {
        sockaddr_in host{};

    public:
        UDPClient() = delete;

        UDPClient(const UDPClient&) = delete;

        UDPClient(const std::string& hostname = "127.0.0.1" , const int port = 12000)
        {
            host.sin_family = AF_INET;
            host.sin_port = htons(port);
            inet_pton(AF_INET , hostname.c_str() , &host.sin_addr);
        }

        void sendm(const std::string& message)
        {
            UDPbase::sendm(message,reinterpret_cast<sockaddr*>(&host));
        }

        std::string recvm(const int bufsize)
        {
            return UDPbase::recvm(bufsize,reinterpret_cast<sockaddr*>(&host));
        }
    };

    class UDPServer : public UDPbase
    {
        int port;

    public:
        UDPServer() = delete;

        UDPServer(const UDPServer&) = delete;

        UDPServer(const int port = 12000) : port{port}
        {
            sockaddr_in host{};
            host.sin_family = AF_INET;
            host.sin_port = htons(port);
            host.sin_addr.s_addr = INADDR_ANY;
            if (bind(socketfd() , reinterpret_cast<sockaddr*>(&host) , sizeof(host)) < 0) {
                perror("bind");
            }
        }
    };
}
#endif //LEARNCPP_MYUDP_H
