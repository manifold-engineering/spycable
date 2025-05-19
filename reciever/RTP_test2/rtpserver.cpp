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


#include "rtpserver.h"
#include "audioringbuffer.h"
#include "config.h"




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

    unsigned int incoming_channel_count = 0;
    unsigned bytes_per_sample = 0;

    unsigned int payload_type = header.payload_type;
    if (payload_type == 97)
        incoming_channel_count = 2;
    if (payload_type == 97)
        bytes_per_sample = 3;
    else{
        std::cerr << "unsupported stream type, skipping";
        return;
    }

    std::vector<std::string> channel_names(incoming_channel_count);
    unsigned int channel_number = 0;
    for (std::string &channel_name : channel_names){
        std::stringstream name_ss;
        name_ss \
            << inet_ntoa(client_addr.sin_addr) << ":" \
            << ntohs(client_addr.sin_port) << "::0x" \
            << std::hex << header.ssrc << "_"
            << channel_number++;
        channel_name = name_ss.str();
    }

    unsigned int payload_bytes = socket_bytes_received - 12;

    if (payload_bytes % (bytes_per_sample * incoming_channel_count)){
        payload_bytes = payload_bytes % (bytes_per_sample * incoming_channel_count);
        std::cout << "packet length is messed, trimming to " << payload_bytes << std::endl;
    }

    //return;

    std::vector <std::vector<RTPStreamChannel>::iterator> channel_iterators(incoming_channel_count, this->channels.end());

    for(unsigned int i = 0; i<incoming_channel_count; ++i){
        std::string channel_name = channel_names[i];
        auto it = std::find_if(this->channels.begin(), this->channels.end(), [&](const RTPStreamChannel& channel) {
            return channel.hasName(channel_name);
        });
        if (it != this->channels.end()){
            channel_iterators[i] = it;
            it->setUsed(true);
        }
        else{
            for (auto it2 = this->channels.begin(); it2 != this->channels.end(); ++it2){
                if (!it2->getUsed()){
                    it2->reset();
                    it2->setName(channel_name);
                    it2->setUsed(true);
                    channel_iterators[i] = it2;
                    break;
                }
            }
        }
    }


    SampleMetadata metadata;
    metadata.recievedTimestamp=std::chrono::steady_clock::now();
    metadata.sequenceNumber = header.sequence_number;
    metadata.rtpTimestamp = header.timestamp;

    for(unsigned int i = 0; i<payload_bytes; i+=(bytes_per_sample*incoming_channel_count)){
        for (unsigned int j = 0; j<incoming_channel_count; ++j){
            if (bytes_per_sample == 3){
                int32_t current_sample = 0;
                int32_t current_sample_raw = 0;
                const char* source = this->socket_buffer \
                                     + 12\
                                     + i*incoming_channel_count*bytes_per_sample\
                                     + j*bytes_per_sample;

                memcpy(&current_sample_raw, source, bytes_per_sample);
                current_sample = ntohs(current_sample_raw);
                channel_iterators[j]->ingestSample(&current_sample, &metadata);
            }
        }
    }
}



RTPServer::RTPServer(uint16_t port, size_t channel_capacity, size_t samples_capacity)

{


    this->sock = INVALID_SOCKET;
    this->udp_port = port;

    //this->channels.reserve(channel_capacity);

    for (size_t i = 0; i < channel_capacity; ++i) {
        this->channels.emplace_back(samples_capacity); // Directly constructs inside the vector
    }

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

void RTPServer::test_buffer(std::string name, unsigned int number_of_samples)
{


    std::vector<RTPStreamChannel>::iterator channel_iterator = this->channels.end()--;
    channel_iterator--;
    channel_iterator->setName(name);
    channel_iterator->setUsed(true);


    for(unsigned int i = 0; i < number_of_samples; ++i){
        SampleMetadata metadata{
            static_cast<uint16_t>(i+42),   //sequence_number
            i+666,  //rtp timestamp
            std::chrono::steady_clock::now(),
        };

        int32_t sample = 42+i*4;
        channel_iterator->ingestSample(&sample, &metadata);
    }


}

RTPStreamChannel *RTPServer::DEBUGGetOneChannelPtr()
{
    RTPStreamChannel *channel;
    channel = &(*(this->channels.begin()));
    return channel;
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

