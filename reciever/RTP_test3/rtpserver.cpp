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

#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <algorithm>

#include "custom_stream_types.h"
#include "rtpserver.h"
#include "audioringbuffer.h"
#include "server_helper_functions.h"
#include "deinterlacedbuffer.h"

//#include "config.h"




void RTPServer::ingestData()
{
    if (socket_bytes_received< 12){
        std::cerr << "incoming packet too short, skipping" << std::endl;
        return;
    }

    uint8_t header_data[12];
    memcpy(header_data, socket_buffer, 12);
    RTPHeader header;
    header.parse(header_data);


    unsigned int payload_type = header.payload_type;
    unsigned int channel_count = get_channels(payload_type);
    unsigned int bytes_per_sample= get_bytes_per_sample(payload_type);

    std::vector<std::string> channel_names(channel_count);
    unsigned int channel_number = 0;
    for (std::string &channel_name : channel_names){
        channel_name = generate_channel_name(this->client_addr, header, channel_number++);
    }

    size_t payload_bytes = socket_bytes_received - 12;


    size_t total_bytes_per_sample = bytes_per_sample * channel_count;
    size_t sample_count = payload_bytes / total_bytes_per_sample;

    if (sample_count * total_bytes_per_sample != payload_bytes){
        std::cout << "packet length is messed, trimming to " << sample_count << " samples"<<std::endl;
    }

    DeinterlacedBuffer deinterlaced(channel_count, sample_count);

    //std::byte *payload = reinterpret_cast<std::byte*>(socket_buffer[12]);

    std::byte* payload = reinterpret_cast<std::byte*>(socket_buffer + 12);

    deinterlace_pad_incoming_data(payload, &deinterlaced, sample_count, payload_type);




}



RTPServer::RTPServer(uint16_t port, size_t channel_capacity, size_t samples_capacity)

{


    this->sock = INVALID_SOCKET;
    this->udp_port = port;

    //this->channels.reserve(channel_capacity);

}

RTPServer::~RTPServer() {
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
    }

#ifdef _WIN32
    WSACleanup();
#endif
}

bool RTPServer::init() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return false;
    }
#endif

    // Create socket
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        return false;
    }

    // Set up server address structure
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on all interfaces
    serverAddr.sin_port = htons(this->udp_port);

    // Bind socket
    if (bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(sock);
        sock = INVALID_SOCKET;
        return false;
    }

    std::cout << "UDP server listening on port " << DEFAULT_UDP_PORT << "..." << std::endl;
    return true;
}

void RTPServer::run() {
    if (sock == INVALID_SOCKET) {
        std::cerr << "Server not initialized" << std::endl;
        return;
    }


    socklen_t clientAddrLen = sizeof(client_addr);

    while (true) {
        // Clear buffer
        std::memset(socket_buffer, 0, SOCKET_BUFFER_SIZE);

        // Receive data
        socket_bytes_received = recvfrom(sock, socket_buffer, SOCKET_BUFFER_SIZE - 1, 0,
                                         (struct sockaddr*)&client_addr, &clientAddrLen);

        if (socket_bytes_received == SOCKET_ERROR) {
            std::cerr << "recvfrom failed" << std::endl;
            continue;
        }

        //EARLY DEBUG: Print client info and packet content
        //std::cout << "Received " << bytesReceived << " bytes from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;
        //std::cout << "Data: " << buffer << std::endl;

        ingestData();
    }
}



void RTPHeader::parse(const uint8_t *buffer) {
    std::memcpy(raw, buffer, sizeof(raw));

    this->version = (versionAndFlags_raw >> 6) & 0x03;
    this->payload_type = static_cast<uint32_t>(payloadType_raw & 0x7F);
    this->sequence_number = ntohs(sequenceNumber_raw);
    this->timestamp = ntohl(timestamp_raw);
    this->ssrc = ntohl(ssrc_raw);
}

void RTPHeader::print() const {
    std::cout << "Version: " << version << "\n";
    std::cout << "Payload Type: " << payload_type << "\n";
    std::cout << "Sequence Number: " << sequence_number << "\n";
    std::cout << "Timestamp: " << timestamp << "\n";
    std::cout << "SSRC: " << ssrc << "\n";
}

