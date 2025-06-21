#ifndef SERVER_HELPER_FUNCTIONS_H
#define SERVER_HELPER_FUNCTIONS_H


#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif


#include <string>
#include <sstream>
#include <iostream>

#include "rtpserver.h"

std::string generate_channel_name (struct sockaddr_in client_addr, RTPHeader header, unsigned int num){
    std::stringstream name_ss;
    name_ss \
        << inet_ntoa(client_addr.sin_addr) << ":" \
        << ntohs(client_addr.sin_port) << "::0x" \
        << std::hex << header.ssrc << "_" \
        << num;
    return name_ss.str();
}


#endif // SERVER_HELPER_FUNCTIONS_H
